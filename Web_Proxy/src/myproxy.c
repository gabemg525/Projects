#define _POSIX_C_SOURCE 200809L


#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdbool.h>
#include <netdb.h>
#include <signal.h>
#include <poll.h>
/* OpenSSL headers */
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>

#include "myproxy.h"





/* Miscellaneous constants */
#define MAXLINE 40000 /* max text line length */
#define BUFFSIZE 100000 /* buffer size for reads and gets */
#define MAXURLS 9999999
/* Define some port number that can be used for our examples */
#define SERV_PORT 9877 /* TCP and UDP */
#define SERV_PORT_STR "9877" /* TCP and UDP */
#define UNIXSTR_PATH "/tmp/unix.str" /* Unix domain stream */
#define UNIXDG_PATH "/tmp/unix.dg" /* Unix domain datagram */
/* Following shortens all the typecasts of pointer arguments: */
#define SA struct sockaddr
#define HAVE_STRUCT_SOCKADDR_STORAGE


pthread_mutex_t mutex;

struct arg_struct {

    int connfd;
    char access_log_path[MAXLINE];
    
    const SSL_METHOD* meth;


};


char **forbidden_urls;
char* forbidden_path;
int forbidden_size=0;
// param 1: path to the file containing forbidden ip's/hostnames, 
// param 2: ip/hostname being checked
// return: returns 1 if the string matches any in forbidden, 0 if none;
int check_forbidden(char* to_check){
    int i;
    for(i=0; i<forbidden_size; i++){
        if(strcmp(forbidden_urls[i],to_check)==0){
            return 1;
        }
    }
    return 0;
}


void* proxy_serv(void *arguments){
   
    
    struct arg_struct *args = arguments;
    char buff[BUFFSIZE+1],request_line[BUFFSIZE+1],request[BUFFSIZE+1],hostname_line[BUFFSIZE+1], hostname[BUFFSIZE+1],send_back[MAXLINE+1];
    char* web_buff;
    char stat_code[30];
    web_buff = (char*)malloc(sizeof(char)*BUFFSIZE);
    int response_size=0;
    int n;
    int web_fd;
    struct pollfd pfd;

    X509                *cert = NULL;
    SSL *ssl;
    SSL_CTX *ctx;
    if((ctx = SSL_CTX_new(args->meth))==NULL){
        fputs("SSL_CTX_new error\n", stderr);
        exit(EXIT_FAILURE);
    }
    ssl = SSL_new(ctx);
    
    char** to_check_forbidden;
    to_check_forbidden=(char**)malloc(sizeof(char*)*MAXLINE);
    int to_check_i = 0;
    int connfd = args->connfd;



    bzero(buff, BUFFSIZE+1);
    //
    // read the message from client and copy it in buffer
    int client_buff_size = read(connfd, buff, BUFFSIZE+1);
    // print buffer which contains the client contents
    printf("From client:\n%s\n", buff);



    strcpy(request, buff);
    char* saveptr;
    strcpy(request_line, strtok_r(buff,"\r\n",&saveptr));
    strtok_r(NULL,"\n",&saveptr);
    char* host_buff;
    host_buff = strtok_r(NULL,"\r\n",&saveptr);
    strcpy(hostname_line,host_buff);
    // printf("strlen(hostname_line): %d\n",strlen(hostname_line));
    hostname_line[strlen(hostname_line)]=0;

//--------------error code 501
    char request_line_copy[BUFFSIZE+1];
    strcpy(request_line_copy, request_line);
    char request_type[100];
    strcpy(request_type, strtok_r(request_line_copy," ",&saveptr));
    // printf("request_type: ----%s----][\n",request_type);
    if(!((strcmp(request_type,"GET")==0)||(strcmp(request_type,"HEAD")==0))){

        strcpy(web_buff,"HTTP/1.1 501 Not Implemented\r\n");
        send(connfd, web_buff, strlen(web_buff), 0);
        strcpy(stat_code,"501");
        goto to_access_log;
    }
//------------

    char hostname_wo_port[MAXLINE],outgoing_port[MAXLINE],hostname_w_port[MAXLINE];
    strtok_r(hostname_line," ",&saveptr);
    strcpy(hostname, strtok_r(NULL,"\r\n",&saveptr));
    strcpy(hostname_w_port,hostname);
    

    strcpy(hostname_wo_port, strtok_r(hostname,":",&saveptr));

    // printf("hostname_wo_port: %s\nhostname: %s\n",hostname_wo_port,hostname);

    if(strcmp(hostname_wo_port,hostname_w_port)!=0){
    strcpy(outgoing_port, strtok_r(NULL,"",&saveptr));

    }else{
        strcpy(outgoing_port, "NO PORT");
  
    }
    
    
    
    to_check_forbidden[to_check_i]=(char*)malloc(sizeof(char)*MAXLINE);
    strcpy(to_check_forbidden[to_check_i],hostname);
    to_check_i++;
    n=check_forbidden(hostname_wo_port);
//---------------error code 403
    
    if(n==1){
        strcpy(web_buff,"HTTP/1.1 403 Forbidden\r\nContent-Length: 0\r\n\r\n");
        send(connfd, web_buff, strlen(web_buff), 0);
        strcpy(stat_code,"403");
        goto to_access_log;
    }
//---------------

    
    // printf("request_line: %s\n", request_line);
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    struct sockaddr_in *ip_access;
 
    struct sockaddr_storage peer_addr;
    socklen_t peer_addr_len;
    ssize_t nread;
    char buf[BUFFSIZE];
    
    

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;   
    hints.ai_socktype = SOCK_STREAM; 
    
    hints.ai_protocol = 0;  
    unsigned char ip[MAXLINE+1]; 
    
    if ((n = getaddrinfo( hostname_wo_port,"domain",&hints,&result))!=0){
        fputs("Could not resolve domain name, sending \"502 Bad Gateway\" to client", stderr);
        pthread_exit(EXIT_FAILURE);
    }
  

    int first = 0;
    struct sockaddr_in servaddr;

    if ( (web_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        fputs("socket error\n", stderr);
        pthread_exit(EXIT_FAILURE);
    }
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    printf("outgoing_port: %s\n",outgoing_port);
    if(strcmp(outgoing_port,"NO PORT")==0){
        printf("no port specified by client, setting to 443\n");
        servaddr.sin_port = htons(443); //change later
    }else{
        printf("port specified by client setting to %s\n", outgoing_port);
        servaddr.sin_port = htons(atoi(outgoing_port));
    }

    for(rp = result; rp!=NULL; rp=rp->ai_next){

        ip_access = (struct sockaddr_in *) rp->ai_addr;
        // printf("IP address is: %s \n",inet_ntoa( ip_access->sin_addr ) );	
        if(first==0){
            strcpy(ip,inet_ntoa( ip_access->sin_addr));
        }
        to_check_forbidden[to_check_i]=(char*)malloc(sizeof(char)*MAXLINE);
        strcpy(to_check_forbidden[to_check_i],ip);
        to_check_i++;
        n=check_forbidden(inet_ntoa( ip_access->sin_addr));

        if(n==1){
            strcpy(web_buff,"HTTP/1.1 403 Forbidden\r\nContent-Length: 0\r\n\r\n");
            send(connfd, web_buff, strlen(web_buff), 0);
            strcpy(stat_code,"403");
            goto to_access_log;
        }
        first++;
    }
    int to_check_size = to_check_i;

    if (inet_pton(AF_INET, ip, &servaddr.sin_addr) <= 0){
        char *err=(char*)malloc(strlen(ip)*sizeof(char));
        sprintf(err,"inet_pton error for %s\n", ip);
        fputs(err, stderr);
        pthread_exit(EXIT_FAILURE);
    }

    struct timeval tv;
    tv.tv_sec = 6; // set high for submission
    tv.tv_usec = 0;
    setsockopt(web_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

    int flags = fcntl(web_fd, F_GETFL, 0);
    if (flags < 0) {
        fprintf(stderr, "Error: could not get socket flags (%s)\n", strerror(errno));
        pthread_exit(EXIT_FAILURE);
    }
    flags |= O_NONBLOCK;
    if (fcntl(web_fd, F_SETFL, flags) < 0) {
        fprintf(stderr, "Error: could not set socket flags (%s)\n", strerror(errno));
        pthread_exit(EXIT_FAILURE);
    }
    SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);
    // printf("hanging on connect\n");
    if (connect(web_fd, (SA *) &servaddr, sizeof(servaddr)) < 0){
        if (errno != EINPROGRESS) {
            fprintf(stderr, "Error: could not connect to server (%s)\n", strerror(errno));
            pthread_exit(EXIT_FAILURE);
        }

        pfd.fd = web_fd;
        pfd.events = POLLOUT;
        // timeout after 20 seconds
        int ret = poll(&pfd, 1, 20000);  
        if (ret == -1) {
            fprintf(stderr, "Error: poll() failed (%s)\n", strerror(errno));
            pthread_exit(EXIT_FAILURE);
        } else if (ret == 0) {
            fprintf(stderr, "Error: connection timed out\n");
            strcpy(web_buff,"HTTP/1.1 504 Gateway Timeout\r\nContent-Length: 0\r\n\r\n");
            send(connfd, web_buff, strlen(web_buff), 0);
            strcpy(stat_code,"504");
            goto to_access_log;
            pthread_exit(EXIT_FAILURE);
        }

        int err;
        socklen_t len = sizeof(err);
        if (getsockopt(web_fd, SOL_SOCKET, SO_ERROR, &err, &len) == -1 || err != 0) {
            fprintf(stderr, "Error: could not connect to server (%s)\n", strerror(err));
            pthread_exit(EXIT_FAILURE);
        }
    }
    flags &= ~O_NONBLOCK;
    if (fcntl(web_fd, F_SETFL, flags) == -1) {
        perror("fcntl");
        pthread_exit(EXIT_FAILURE);
    }
    
    // printf("hanging on SSL_set_fd\n");
    if((n=SSL_set_fd(ssl, web_fd))!=1){
        fputs("SSL_set_fd error\n", stderr);
        pthread_exit(EXIT_FAILURE);
    }
    if((n=SSL_connect(ssl))<0){
        fputs("SSL_connect error\n", stderr);
        pthread_exit(EXIT_FAILURE);
    }
    int m=SSL_get_error(ssl,n);
    if(m<0){
        fputs("SSL_get_error, SSL_connect error\n", stderr);
        pthread_exit(EXIT_FAILURE);
    }
    // printf("hanging on certificate\n");
    cert = SSL_get_peer_certificate(ssl);
    if (cert == NULL){
        fputs("Error: Could not get a certificate\n", stderr);
        pthread_exit(EXIT_FAILURE);
    }
    // printf("hanging on verify\n");
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);

    // printf("request:\n%s\n", request);

    // printf("hanging on write\n");
    if(((n = SSL_write(ssl, request, client_buff_size)))<0){
        fputs("write error\n",stderr);
        pthread_exit(EXIT_FAILURE);
    }


    int begin_read = 0;
    int just_1 = 0;

    char* retval;
    while((n = SSL_read(ssl, web_buff, BUFFSIZE))>0){
        sleep(1);
        send(connfd, web_buff, n, 0); 
        for(to_check_i=0; to_check_i<to_check_size; to_check_i++){
            m = check_forbidden(to_check_forbidden[to_check_i]);
            if(m==1){
                // printf("mmm actually that's FORBIDDEN\n");
                strcpy(web_buff,"\r\nConnection: close\r\n\r\n");
                send(connfd, web_buff, strlen(web_buff), 0);
                strcpy(stat_code,"403");
                goto to_access_log;
            }
        }
        response_size=response_size+n;   
        printf("bytes read: %d\n", n);
        bzero(web_buff, BUFFSIZE);
       
    }

    strcpy(stat_code,"200");
    free(web_buff);

    to_access_log:

    int access_log = open(args->access_log_path, O_WRONLY | O_APPEND);
    char access_log_buff[MAXLINE];
    sprintf(access_log_buff,"%s \"%s\" %s %d\n",RFC_time(),request_line,stat_code,response_size);
  
    write(access_log,access_log_buff,strlen(access_log_buff));

    close(access_log);
    for(to_check_i=0; to_check_i<to_check_size; to_check_i++){
        free(to_check_forbidden[to_check_i]);
    }
    free(to_check_forbidden);
    free(web_buff);
    
    SSL_free(ssl);
    close(web_fd);
    close(connfd);
    X509_free(cert);
    SSL_CTX_free(ctx);
    pthread_exit(retval);
    return retval;


    
}


void handle_sigtstp(int sig) {
        printf("exited with ^Z\n\n");
        exit(0);

}
void update_forbidden(int sig) {
    //clear forbidden_urls before update
    pthread_mutex_lock(&mutex);
    for(int j=0;j<forbidden_size;j++){
        free(forbidden_urls[j]);
    }
    FILE* forbidden = fopen(forbidden_path, "r");
    if (forbidden == NULL) {
        fputs("Failed to open file.\n",stderr);
    }
    char line[MAXLINE];
    int bytes_read = 0;
    int i = 0;
    while (fgets(line, sizeof(line), forbidden) != NULL) {
  
        line[strlen(line)-1]=0;
        forbidden_urls[i] = (char*)malloc(sizeof(char)*MAXLINE);
        strcpy(forbidden_urls[i],line);
        i++;
    }
    forbidden_size=i;
    printf("\n\nforbidden urls updated! here they are:\n");

    for(int k = 0; k<forbidden_size; k++){
        printf("%s\n",forbidden_urls[k] );
    }
    fclose(forbidden);
    pthread_mutex_unlock(&mutex);
    return 0;

}

int main( int argc, char *argv[] )     
{

    printf("hello stop bein gay\n");
    int count;
    SSL_library_init();

    OpenSSL_add_ssl_algorithms();
    
    
    const SSL_METHOD* meth = SSLv23_client_method();
    

    // set port in main
    // while loop listens for client requests
    // returns file descriptor for request
    // pass the socket descriptor to threaded function
    // allocate buffers that are needed, (in the thread)
    // read request header from client
    // parse request header for specific things
    // tcp connections from book server examples
    // initialize ssl library 
    int n;

    if(argc != 4){
        fputs("usage: ./bin/myproxy <listening port> <forbidden files path> <access log path>\n",stderr);
        exit(EXIT_FAILURE);
    }
    

    char forbidden_files[MAXLINE+1],access_log_files[MAXLINE+1];
    strcpy(forbidden_files,argv[2]);

    // strcpy(access_log_files,argv[3]);

    int sockfd, connfd;
 
    socklen_t len;
    struct sockaddr_in servaddr, cli;
 
    // socket create and verification
    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        fputs("socket error\n",stderr);
        exit(EXIT_FAILURE);
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(atoi(argv[1])); 
    if ((n = bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
        fputs("socket bind failed...\n",stderr);
      
        exit(EXIT_FAILURE);
    }

    if ((n = listen(sockfd, 5)) != 0) {
        fputs("listen error\n",stderr);
        exit(EXIT_FAILURE);
    }

    len = sizeof(cli);
    int k=0;
    pthread_t thread_arr[99999];
    struct arg_struct args;



  
    char access_log_path[MAXLINE];
    forbidden_path = (char*)malloc(sizeof(char)*MAXLINE);
    
    sprintf(access_log_path, "./%s", argv[3]);
    create_file_path(access_log_path);
    sprintf(forbidden_path, "./%s", argv[2]);
    create_file_path(forbidden_path);
    forbidden_urls = (char**)malloc(sizeof(char*)*MAXURLS);
    





    pthread_mutex_init(&mutex, NULL);


    struct sigaction sa1,sa2;
    sa1.sa_handler = &handle_sigtstp;
    sa1.sa_flags = SA_RESTART;
    sigaction(SIGTSTP, &sa1,NULL);
    sa2.sa_handler = &update_forbidden;
    sa2.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa2,NULL);
    update_forbidden(1);


    args.meth = meth;
    strcpy(args.access_log_path, access_log_path);
    while(1){
        printf("waiting for accept\n");
        args.connfd = accept(sockfd, (SA*)&cli, &len);

        if (args.connfd < 0) {
            fputs("server accept failed...\n", stderr);
            exit(EXIT_FAILURE);
        }
    
        if ((n = pthread_create(&thread_arr[k], NULL, &proxy_serv, (void*)&args))!=0){
            fputs("pthread_create error\n",stderr);
            exit(EXIT_FAILURE);
            return 1;
        }
   

        k++;
    }

    pthread_mutex_destroy(&mutex);
    exit(0);

    return 0;
}

