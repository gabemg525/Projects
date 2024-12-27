#include "asgn2_helper_funcs.h"
#include "connection.h"
#include "debug.h"
#include "response.h"
#include "request.h"
#include "queue.h"
#include "rwlock.h"
#include <pthread.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME  1099511628211UL
// Hash table entry (slot may be filled or empty).
typedef struct {
    const char *key; // key is NULL if this slot is empty
    rwlock_t *rwlock;
} ht_entry;

// Hash table structure: create with ht_create, free with ht_destroy.
struct ht {
    ht_entry *entries; // hash slots
    size_t capacity; // size of _entries array
    size_t length; // number of items in hash table
};

#define INITIAL_CAPACITY 16 // must not be zero
typedef struct ht ht;
ht *ht_create(void);
void ht_destroy(ht *table);
void *ht_get(ht *table, const char *key);
const char *ht_set(ht *table, const char *key, rwlock_t *rwlock);
size_t ht_length(ht *table);
static bool ht_expand(ht *);
static const char *ht_set_entry(ht_entry *, size_t, const char *, rwlock_t *, size_t *);

ht *ht_create(void) {
    // Allocate space for hash table struct.
    ht *table = malloc(sizeof(ht));
    if (table == NULL) {
        return NULL;
    }
    table->length = 0;
    table->capacity = INITIAL_CAPACITY;

    // Allocate (zero'd) space for entry buckets.
    table->entries = calloc(table->capacity, sizeof(ht_entry));
    if (table->entries == NULL) {
        free(table); // error, free table before we return!
        return NULL;
    }
    return table;
}

void ht_destroy(ht *table) {
    // First free allocated keys.
    for (size_t i = 0; i < table->capacity; i++) {
        free((void *) table->entries[i].key);
    }

    // Then free entries array and table itself.
    free(table->entries);
    free(table);
}

typedef struct {
    const char *key; // current key
    rwlock_t *rwlock; // current value

    // Don't use these fields directly.
    ht *_table; // reference to hash table being iterated
    size_t _index; // current index into ht._entries
} hti;

hti ht_iterator(ht *table);
bool ht_next(hti *it);

static uint64_t hash_key(const char *key) {
    uint64_t hash = FNV_OFFSET;
    for (const char *p = key; *p; p++) {
        hash ^= (uint64_t) (unsigned char) (*p);
        hash *= FNV_PRIME;
    }
    return hash;
}

void *ht_get(ht *table, const char *key) {
    // AND hash with capacity-1 to ensure it's within entries array.
    uint64_t hash = hash_key(key);
    size_t index = (size_t) (hash & (uint64_t) (table->capacity - 1));

    // Loop till we find an empty entry.
    while (table->entries[index].key != NULL) {
        if (strcmp(key, table->entries[index].key) == 0) {

            // Found key, return value.
            return table->entries[index].rwlock;
        }
        // Key wasn't in this slot, move to next (linear probing).
        index++;
        if (index >= table->capacity) {
            // At end of entries array, wrap around.
            index = 0;
        }
    }
    return NULL;
}

const char *ht_set(ht *table, const char *key, rwlock_t *rwlock) {

    // if (rwlock == NULL) {
    //     return NULL;
    // }

    // If length will exceed half of current capacity, expand it.
    if (table->length >= table->capacity / 2) {
        if (!ht_expand(table)) {
            return NULL;
        }
    }

    // Set entry and update length.
    return ht_set_entry(table->entries, table->capacity, key, rwlock, &table->length);
}

static const char *ht_set_entry(
    ht_entry *entries, size_t capacity, const char *key, rwlock_t *rwlock, size_t *plength) {

    // AND hash with capacity-1 to ensure it's within entries array.
    uint64_t hash = hash_key(key);
    size_t index = (size_t) (hash & (uint64_t) (capacity - 1));

    // Loop till we find an empty entry.
    while (entries[index].key != NULL) {
        if (strcmp(key, entries[index].key) == 0) {
            // Found key (it already exists), update value.
            entries[index].rwlock = rwlock;
            return entries[index].key;
        }
        // Key wasn't in this slot, move to next (linear probing).
        index++;
        if (index >= capacity) {
            // At end of entries array, wrap around.
            index = 0;
        }
    }

    // Didn't find key, allocate+copy if needed, then insert it.
    if (plength != NULL) {
        key = strdup(key);
        if (key == NULL) {
            return NULL;
        }
        (*plength)++;
    }
    entries[index].key = (char *) key;

    //I changed this
    entries[index].rwlock = rwlock_new(N_WAY, 1);
    return key;
}

// Expand hash table to twice its current size. Return true on success,
// false if out of memory.
static bool ht_expand(ht *table) {
    // Allocate new entries array.
    size_t new_capacity = table->capacity * 2;
    if (new_capacity < table->capacity) {
        return false; // overflow (capacity would be too big)
    }
    ht_entry *new_entries = calloc(new_capacity, sizeof(ht_entry));
    if (new_entries == NULL) {
        return false;
    }

    // Iterate entries, move all non-empty ones to new table's entries.
    for (size_t i = 0; i < table->capacity; i++) {
        ht_entry entry = table->entries[i];
        if (entry.key != NULL) {
            ht_set_entry(new_entries, new_capacity, entry.key, entry.rwlock, NULL);
        }
    }

    // Free old entries array and update this table's details.
    free(table->entries);
    table->entries = new_entries;
    table->capacity = new_capacity;
    return true;
}

size_t ht_length(ht *table) {
    return table->length;
}

hti ht_iterator(ht *table) {
    hti it;
    it._table = table;
    it._index = 0;
    return it;
}

bool ht_next(hti *it) {
    // Loop till we've hit end of entries array.
    ht *table = it->_table;
    while (it->_index < table->capacity) {
        size_t i = it->_index;
        it->_index++;
        if (table->entries[i].key != NULL) {
            // Found next non-empty item, update iterator key and value.
            ht_entry entry = table->entries[i];
            it->key = entry.key;
            it->rwlock = entry.rwlock;
            return true;
        }
    }
    return false;
}
// int TABLE_SIZE = 507;

// int PAIRS = 1000;

// int rotater = 0;

// typedef struct UrlIDPair {
//     char *url;
//     unsigned int id;
// } UrlIDPair;

// typedef struct HashTable {
//     rwlock_t **rwlock;
//     UrlIDPair *url_id_pairs;
// } HashTable;

// HashTable *rwlock_ht = NULL;

// void initHashTable(HashTable *table) {
//     table->rwlock = (rwlock_t**)malloc(sizeof(rwlock_t*) * TABLE_SIZE);
//     table->url_id_pairs = malloc(sizeof(UrlIDPair) * PAIRS);
//     for (int i = 0; i < TABLE_SIZE; i++) {
//         table->rwlock[i] = rwlock_new(N_WAY, 1);
//     }
//     for (int i = 0; i < PAIRS; i++) {
//         table->url_id_pairs[i].url = NULL;
//         table->url_id_pairs[i].id = PAIRS + 1;
//     }
// }

// unsigned int hash(const char *url) {

//     unsigned int hash = 0;

//     while (*url) {
//         hash = (hash << 5) ^ *url++; // Left shift by 5 and XOR with the next character
//     }

//     return hash % TABLE_SIZE;
// }

// int search_ht(const char *url) {
//     for (int i = 0; i < PAIRS; i++) {
//         if (rwlock_ht->url_id_pairs[i].url == NULL) {
//             // fprintf(stderr,"couldn't find returning -1\n");
//             return -1;
//         }
//         if (strcmp(rwlock_ht->url_id_pairs[i].url, url) == 0) {
//             // fprintf(stderr,"search_ht returning %d on loop %d\n", table->url_id_pairs[i].id, i);
//             return rwlock_ht->url_id_pairs[i].id;
//         }
//     }
//     // fprintf(stderr,"couldn't find returning -1\n");
//     return -1;
// }

// void store_pair(const char *url, unsigned int id) {
//     int fix = 0;
//     for (int i = 0; i < PAIRS; i++) {
//         if (rwlock_ht->url_id_pairs[i].url == NULL) {

//             rwlock_ht->url_id_pairs[i].url = strdup(url);
//             rwlock_ht->url_id_pairs[i].id = id;
//             fix = 1;
//             // fprintf(stderr,"storing url: %s, id: %d in index %d\n", table->url_id_pairs[i].url, table->url_id_pairs[i].id, i);
//             // sleep(1);
//             return;
//         }
//     }
//     if (fix == 0) {
//         // sleep(1);
//         rwlock_ht->url_id_pairs[rotater].url = strdup(url);
//         rwlock_ht->url_id_pairs[rotater].id = id;
//         // fprintf(stderr,"storing url: %s, id: %d in index %d\n", table->url_id_pairs[rotater].url, table->url_id_pairs[rotater].id, rotater);
//         rotater = rotater + 1 % PAIRS;
//     }
// }

// rwlock_t *rwlock_assign(const char *url) {
//     unsigned int id = 0;
//     int index;

//     if ((index = search_ht(url)) >= 0) {
//         id = index;
//     } else {
//         id = hash(url);
//         store_pair(url, id);
//     }

//     return rwlock_ht->rwlock[id];
// }

// void freeHashTable(HashTable *table) {
//     for(int i = 0; i<TABLE_SIZE; i++) {
//         rwlock_delete(&table->rwlock[i]);
//     }
//     free(table->rwlock);
//     free(table->url_id_pairs);
// }

typedef struct arg_struct {
    pthread_mutex_t *ht_lock;
    int thread_num;
} arg_struct;

void *worker_thread(void *arguments);
void *dispatcher_thread(void *arguments);
int check_available(int threads, int *thread_connection);
void handle_get(conn_t *, pthread_mutex_t *);
void handle_put(conn_t *, pthread_mutex_t *);
void handle_unsupported(conn_t *);

queue_t **waiting_requests;
int ordering = 0;
ht *rwlock_ht = NULL;
int main(int argc, char *argv[]) {
    if (!(argc == 2 || argc == 4)) {
        warnx("wrong arguments: %s threads port_num", argv[0]);
        fprintf(stderr, "usage: %s <threads> <port>\n", argv[0]);
        return EXIT_FAILURE;
    }
    rwlock_ht = ht_create();

    int threads = 4; // Default number of threads
    int port = 0; // Default port

    int opt;
    while ((opt = getopt(argc, argv, "t:")) != -1) {
        switch (opt) {
        case 't': threads = atoi(optarg); break;
        default: fprintf(stderr, "Usage: %s [-t threads] <port>\n", argv[0]); exit(EXIT_FAILURE);
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Error: Port number is missing.\n");
        exit(EXIT_FAILURE);
    }

    port = atoi(argv[optind]);

    if (port <= 0) {
        fprintf(stderr, "Error: Invalid port number.\n");
        exit(EXIT_FAILURE);
    }

    pthread_t thread_arr[threads];

    int n;
    int i;
    // TABLE_SIZE = 2013;
    // PAIRS = 1000/threads;
    waiting_requests = (queue_t **) malloc(sizeof(queue_t *) * threads);
    for (i = 0; i < threads; i++) {
        waiting_requests[i] = queue_new(threads);
    }

    // return 1;

    pthread_mutex_t *ht_lock = malloc(sizeof(pthread_mutex_t));

    pthread_mutex_init(ht_lock, NULL);

    arg_struct **args = (arg_struct **) malloc(sizeof(arg_struct *) * threads);
    for (i = 0; i < threads; i++) {
        args[i] = (arg_struct *) malloc(sizeof(arg_struct));
        args[i]->thread_num = i;
        args[i]->ht_lock = ht_lock;

        if ((n = pthread_create(&thread_arr[i], NULL, &worker_thread, (void *) args[i])) != 0) {
            fputs("pthread_create error\n", stderr);
            exit(EXIT_FAILURE);
            return 1;
        }
    }
    Listener_Socket sock;
    listener_init(&sock, port);
    i = 0;
    while (1) {
        // fprintf(stderr,"pushing to thread %d\n", i);
        uintptr_t connfd = listener_accept(&sock);

        if (!queue_push(waiting_requests[i], (void *) connfd)) {
            fprintf(stderr, "Failed to push %lu\n", connfd);
        }
        i = (i + 1) % threads;
    }
    for (i = 0; i < threads; i++) {
        free(args[i]);
        queue_delete(&waiting_requests[i]);
    }
    free(waiting_requests);

    // freeHashTable(rwlock_ht);
    ht_destroy(rwlock_ht);

    return 1;
}

void *worker_thread(void *arguments) {
    arg_struct *args = (arg_struct *) arguments;
    char *retval = NULL;
    while (1) {
        uintptr_t connfd = 0;

        if (!queue_pop(waiting_requests[args->thread_num], (void **) &connfd)) {
            fprintf(stderr, "Failed to pop\n");
        }
        // fprintf(stderr,"thread %d handling conn %lu\n",args->thread_num,connfd);
        conn_t *conn = conn_new(connfd);

        const Response_t *res = conn_parse(conn);

        if (res != NULL) {
            conn_send_response(conn, res);

        } else {
            // debug("%s", conn_str(conn));
            const Request_t *req = conn_get_request(conn);
            if (req == &REQUEST_GET) {
                handle_get(conn, args->ht_lock);
            } else if (req == &REQUEST_PUT) {
                handle_put(conn, args->ht_lock);
            } else {
                handle_unsupported(conn);
            }
        }
        conn_delete(&conn);
        close(connfd);
    }

    return retval;
}

void handle_get(conn_t *conn, pthread_mutex_t *ht_lock) {

    char *uri = conn_get_uri(conn);
    const Response_t *res = NULL;
    int status_code = 0;

    pthread_mutex_lock(ht_lock);

    rwlock_t *rwlock = NULL;
    while ((rwlock = ht_get(rwlock_ht, uri)) == NULL) {
        ht_set(rwlock_ht, uri, rwlock);
    }
    pthread_mutex_unlock(ht_lock);
    reader_lock(rwlock);

    int fd = open(uri, O_RDONLY);
    if (fd < 0) {
        // debug("%s: %d", uri, errno);
        if (errno == EACCES || errno == EISDIR || errno == ENOENT) {
            if (errno == ENOENT) {
                res = &RESPONSE_NOT_FOUND;
                status_code = 404;
                reader_unlock(rwlock);
                goto out;
            }
            res = &RESPONSE_FORBIDDEN;
            status_code = 403;
            reader_unlock(rwlock);
            goto out;
        } else {
            res = &RESPONSE_INTERNAL_SERVER_ERROR;
            status_code = 500;
            reader_unlock(rwlock);
            goto out;
        }
    }

    struct stat st;
    // stat(uri, &st);
    if (fstat(fd, &st) == -1) {
        res = &RESPONSE_INTERNAL_SERVER_ERROR;
        status_code = 500;
        close(fd);
        reader_unlock(rwlock);

        goto out;
    }
    int infile_size = st.st_size;

    if (S_ISDIR(st.st_mode)) {
        res = &RESPONSE_INTERNAL_SERVER_ERROR;
        status_code = 500;
        close(fd);
        reader_unlock(rwlock);

        goto out;
    }

    res = conn_send_file(conn, fd, infile_size);

    if (res == NULL) {
        res = &RESPONSE_OK;
    }

    status_code = 200;
    close(fd);
    reader_unlock(rwlock);

out:

    fprintf(stderr, "GET,/%s,%d,%s\n", uri, status_code, conn_get_header(conn, "Request-Id"));

    if (res != &RESPONSE_OK)
        conn_send_response(conn, res);
}

void handle_unsupported(conn_t *conn) {
    conn_send_response(conn, &RESPONSE_NOT_IMPLEMENTED);
}

void handle_put(conn_t *conn, pthread_mutex_t *ht_lock) {

    char *uri = conn_get_uri(conn);
    const Response_t *res = NULL;

    int status_code = 0;
    // debug("handling put request for %s", uri);

    // Check if file already exists before opening it.
    bool existed = access(uri, F_OK) == 0;
    // debug("%s existed? %d", uri, existed);

    pthread_mutex_lock(ht_lock);

    rwlock_t *rwlock = NULL;
    while ((rwlock = ht_get(rwlock_ht, uri)) == NULL) {

        ht_set(rwlock_ht, uri, rwlock);
    }
    pthread_mutex_unlock(ht_lock);
    writer_lock(rwlock);

    int fd = open(uri, O_CREAT | O_TRUNC | O_WRONLY, 0600);
    if (fd < 0) {
        // debug("%s: %d", uri, errno);
        if (errno == EACCES || errno == EISDIR || errno == ENOENT) {
            res = &RESPONSE_FORBIDDEN;
            status_code = 403;
            writer_unlock(rwlock);
            close(fd);
            goto out;
        } else {
            res = &RESPONSE_INTERNAL_SERVER_ERROR;
            status_code = 500;
            writer_unlock(rwlock);
            close(fd);
            goto out;
        }
    }

    res = conn_recv_file(conn, fd);

    if (res == NULL && existed) {
        res = &RESPONSE_OK;
        status_code = 200;
    } else if (res == NULL && !existed) {
        res = &RESPONSE_CREATED;
        status_code = 201;
    }
    close(fd);
    writer_unlock(rwlock);

out:

    fprintf(stderr, "PUT,/%s,%d,%s\n", uri, status_code, conn_get_header(conn, "Request-Id"));

    conn_send_response(conn, res);
}
