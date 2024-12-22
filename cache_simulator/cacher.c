#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>

#define MAX_DATA_SIZE       8000
#define PREVIOUS_CACHE_SIZE 5000

int CO = 0, CA = 0, misses = 0;
char **previous_cached;
// Node structure
typedef struct Node {
    char *data;
    int referenced;
    struct Node *next;
} Node;

// Linked list structure
typedef struct LinkedList {
    Node *head;
    unsigned int capacity;
    unsigned int curr_size;
} LinkedList;

// used to store historical values
int search_and_insert_previous_cache(char *data) {
    for (int i = 0; i < PREVIOUS_CACHE_SIZE; i++) {
        if (previous_cached[i] == NULL) {
            previous_cached[i] = strdup(data);
            return 0;
        }
        if (strcmp(previous_cached[i], data) == 0) {
            return 1;
        }
    }
    return 0;
}

// Function to initialize an empty linked list
void initializeLinkedList(LinkedList *list, unsigned int capacity) {
    list->head = NULL;
    list->capacity = capacity;
    list->curr_size = 0;
}

// Function to insert a new node at the beginning of the linked list
// used by FIFO
void insertAtBeginning(LinkedList *list, char *newData) {
    // Create a new node
    Node *newNode = (Node *) malloc(sizeof(Node));
    if (newNode == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    // Allocate memory for the data and copy it
    newNode->data = strdup(newData);
    if (newNode->data == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    // Set the next pointer to the current head
    newNode->next = list->head;

    // Update the head to the new node
    list->head = newNode;
    list->curr_size += 1;
}

// Function to remove the last node from the linked list
// used by FIFO and LRU
void removeLastNode(LinkedList *list) {
    if (list->head == NULL) {
        // List is empty, nothing to remove
        return;
    }

    Node *current = list->head;
    Node *previous = NULL;

    // Traverse to the end of the list
    while (current->next != NULL) {
        previous = current;
        current = current->next;
    }

    // Remove the last node
    if (previous == NULL) {
        // Only one node in the list
        free(current->data);
        free(current);
        list->head = NULL;
    } else {
        // More than one node in the list
        free(current->data);
        free(current);
        previous->next = NULL;
    }
    list->curr_size--;
}

void printLinkedList(const LinkedList *list) {
    Node *current = list->head;
    while (current != NULL) {
        printf("%s -> ", current->data);
        current = current->next;
    }
    printf("NULL\n");
}

void freeLinkedList(LinkedList *list) {
    Node *current = list->head;
    while (current != NULL) {
        Node *next = current->next;
        free(current->data);
        free(current);
        current = next;
    }
    list->head = NULL;
}

// Function to check if a data value is already in the linked list
// used by all policies
int containsValue(const LinkedList *list, const char *data_to_find) {
    Node *current = list->head;
    while (current != NULL) {
        if (strcmp(current->data, data_to_find) == 0) {
            return 1; // Value found in the list
        }
        current = current->next;
    }
    return 0; // Value not found in the list
}

// Function to move a node containing a specific value to the beginning of the list
// Used by LRU policy
int moveToBeginningIfContains(LinkedList *list, const char *targetData) {
    Node *current = list->head;
    Node *previous = NULL;

    // Traverse the list to find the node with the target data
    while (current != NULL && strcmp(current->data, targetData) != 0) {
        previous = current;
        current = current->next;
    }

    // If the value is found, move the node to the beginning of the list
    if (current != NULL) {
        // If the node is not already at the beginning
        if (previous != NULL) {
            previous->next = current->next;
            current->next = list->head;
            list->head = current;
        }

        return 1;
    }

    return 0;
}

// Function to insert data into circular linked list
// used for clock eviction
// Handles HITS and MISSES
void insertCircular(LinkedList *list, char *data) {
    Node *newNode = (Node *) malloc(sizeof(Node));
    int search = search_and_insert_previous_cache(data);
    if (list->head == NULL) {
        // printf("head was null\n");
        list->head = newNode;
        newNode->next = newNode;
        newNode->data = strdup(data);
        list->curr_size++;
        CO++;
        printf("MISS\n");
        misses++;
        return;
    }
    Node *endNode = list->head;

    do {

        if (strcmp(data, endNode->data) == 0) {
            printf("HIT\n");
            endNode->referenced = 1;
            free(newNode);
            return;
        }
        endNode = endNode->next;

    } while (endNode->next != list->head);

    if (strcmp(data, endNode->data) == 0) {
        printf("HIT\n");
        endNode->referenced = 1;
        free(newNode);
        return;
    }
    printf("MISS\n");
    misses++;
    newNode->data = strdup(data);
    Node *new_head = NULL;
    if (list->curr_size < list->capacity) {
        list->curr_size++;
        new_head = list->head;
        newNode->next = list->head;
        endNode->next = newNode;
        CO++;
    } else {
        if (search == 1) {
            CA++;
        } else {
            CO++;
        }
        Node *to_remove = list->head;
        Node *before_to_remove = endNode;
        while (to_remove->referenced == 1) {
            to_remove->referenced = 0;
            to_remove = to_remove->next;
            before_to_remove = before_to_remove->next;
        }

        // printf("end node: %s\n", endNode->data);
        before_to_remove->next = newNode;
        new_head = to_remove->next;
        newNode->next = new_head;

        free(to_remove);
    }

    list->head = new_head;
}

// Function to print the circular linked list for clock eviction
void printListCircular(LinkedList *list) {
    if (list->head == NULL) {
        printf("List is empty\n");
        return;
    }

    Node *current = list->head;
    printf("head: %s\n", list->head->data);
    do {
        printf("(%s,%d)->", current->data, current->referenced);
        current = current->next;
    } while (current != list->head);
    printf("\n");
}

// Function to free memory allocated for the linked list
void freeLinkedListCircular(LinkedList *list) {
    if (list->head == NULL || list->head->data == NULL) {
        return;
    }
    Node *current = list->head->next;

    while (strcmp(current->data, list->head->data) != 0) {
        Node *temp = current;
        current = current->next;
        free(temp->data);
        free(temp);
    }
    free(list->head->data);
    free(list->head);
    list->head = NULL;
}

int CE_cacher(unsigned int capacity) {
    LinkedList myList;
    initializeLinkedList(&myList, capacity);

    char *buffer = malloc(sizeof(char) * MAX_DATA_SIZE);
    while (fgets(buffer, MAX_DATA_SIZE, stdin) != NULL) {
        // Print the read line
        size_t len = strlen(buffer);
        if (len == 0) {
            fprintf(stderr, "invalid input\n");
            return 1;
        }
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
        insertCircular(&myList, buffer);
        // printListCircular(&myList);
    }
    CA = misses - CO;
    printf("%d %d\n", CO, CA);

    freeLinkedListCircular(&myList);

    return 0;
}

int lru_cacher(unsigned int capacity) {
    // Create and initialize a linked list
    // Create and initialize a linked list
    LinkedList myList;
    initializeLinkedList(&myList, capacity);
    char *buffer = malloc(sizeof(char) * MAX_DATA_SIZE);
    int search = 0;
    while (fgets(buffer, MAX_DATA_SIZE, stdin) != NULL) {
        // Print the read line
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
        // printf("------%s----\n", buffer);
        search = search_and_insert_previous_cache(buffer);
        if (moveToBeginningIfContains(&myList, buffer) == 1) {
            printf("HIT\n");
        } else {
            printf("MISS\n");
            misses++;
            insertAtBeginning(&myList, buffer);
            if (myList.curr_size > myList.capacity) {
                removeLastNode(&myList);
                if (search == 1) {
                    CA++;
                } else {
                    CO++;
                }
            } else {
                CO++;
            }
        }
        // printLinkedList(&myList);
    }
    CA = misses - CO;
    printf("%d %d\n", CO, CA);

    // Free the memory allocated for the linked list
    freeLinkedList(&myList);

    return 0;
}

int fifo_cacher(unsigned int capacity) {
    // Create and initialize a linked list
    LinkedList myList;
    initializeLinkedList(&myList, capacity);
    char *buffer = malloc(sizeof(char) * MAX_DATA_SIZE);
    int search = 0;
    while (fgets(buffer, MAX_DATA_SIZE, stdin) != NULL) {
        // Print the read line
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
        search = search_and_insert_previous_cache(buffer);
        // printf("------%s----\n", buffer);
        if (containsValue(&myList, buffer) == 1) {
            printf("HIT\n");
        } else {
            printf("MISS\n");
            insertAtBeginning(&myList, buffer);
            misses++;
            if (myList.curr_size > myList.capacity) {
                removeLastNode(&myList);
                if (search == 1) {
                    CA++;
                } else {
                    CO++;
                }

            } else {

                CO++;
            }
        }
        // printLinkedList(&myList);
    }
    CA = misses - CO;
    printf("%d %d\n", CO, CA);

    // printLinkedList(&myList);

    // Free the memory allocated for the linked list
    freeLinkedList(&myList);

    return 0;
}

int main(int argc, char *argv[]) {
    if (!(argc == 3 || argc == 4)) {
        warnx("wrong arguments: %s threads port_num", argv[0]);
        fprintf(stderr, "usage: %s <threads> <port>\n", argv[0]);
        return EXIT_FAILURE;
    }
    unsigned int size;
    char *policy = "-F";
    int opt;
    while ((opt = getopt(argc, argv, "N:CLF")) != -1) {
        switch (opt) {
        case 'N': size = atoi(optarg); break;

        case 'C': policy = "-C"; continue;
        case 'L': policy = "-L"; continue;
        case 'F': policy = "-F"; continue;
        default: fprintf(stderr, "Usage: %s [-N size] <policy>\n", argv[0]); exit(EXIT_FAILURE);
        }
    }
    if (size <= 0) {
        fprintf(stderr, "invalid size argument\n");
        exit(EXIT_FAILURE);
    }
    if (strcmp(argv[1], "-N") != 0) {
        fprintf(stderr, "Usage: %s [-N size] <policy>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    if (argc == 4
        && !(strcmp(argv[3], "-F") == 0 || strcmp(argv[3], "-L") == 0
             || strcmp(argv[3], "-C") == 0)) {
        fprintf(stderr, "invalid policy argument\n");
        exit(EXIT_FAILURE);
    }

    previous_cached = (char **) malloc(sizeof(char *) * PREVIOUS_CACHE_SIZE);
    for (int i = 0; i < PREVIOUS_CACHE_SIZE; i++) {
        previous_cached[i] = NULL;
    }

    if (strcmp(policy, "-F") == 0) {
        fifo_cacher(size);
    } else if (strcmp(policy, "-L") == 0) {
        lru_cacher(size);
    } else if (strcmp(policy, "-C") == 0) {
        CE_cacher(size);
    } else {
        fprintf(stderr, "Invalid Policy\n");
        return 1;
    }

    return 0;
}
