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


/* Miscellaneous constants */
#define MAXLINE 40000 /* max text line length */
#define BUFFSIZE 100000 /* buffer size for reads and writes */
/* Define some port number that can be used for our examples */
#define SERV_PORT 9877 /* TCP and UDP */
#define SERV_PORT_STR "9877" /* TCP and UDP */
#define UNIXSTR_PATH "/tmp/unix.str" /* Unix domain stream */
#define UNIXDG_PATH "/tmp/unix.dg" /* Unix domain datagram */
/* Following shortens all the typecasts of pointer arguments: */
#define SA struct sockaddr
#define HAVE_STRUCT_SOCKADDR_STORAGE

void delay(int tot_ms)
{
    // Converting time into milli_seconds

 
    // Storing start time
    clock_t start_time = clock();
 
    // looping till required time is not achieved
    while (clock() < start_time + tot_ms)
        ;
}

int random_percent(){
    int x =rand()/(RAND_MAX/100);
    return x;
}

char* RFC_time() {
    
    time_t now;
    time( &now );
    struct tm *gm = gmtime(&now);
    int hours, minutes, seconds, day, month, year;
    hours = gm->tm_hour;         
    minutes = gm->tm_min;        
    seconds = gm->tm_sec;        
    day = gm->tm_mday;            
    month = gm->tm_mon + 1;      
    year = gm->tm_year + 1900;   
    struct timeval tv;
    gettimeofday(&tv,NULL);

    int tm_since = (tv.tv_usec/1000);
    int s = tm_since/1000;
    int ms = tm_since - s*1000;
    char *buff;
    buff = (char*)malloc(MAXLINE*sizeof(char));
    sprintf(buff,"%04d-%02d-%02dT%02d:%02d:%02d.%03dZ", year, month, day, hours, minutes, seconds, ms);
   
    return buff;


}

void create_file_path(char* path){
    
    char buff[MAXLINE];
    char command[MAXLINE] = "";
    char* token[MAXLINE];
    
    int i = 0;
    strcpy(buff, path);
  
    token[i] = strtok(buff,"/");
    while(token[i]!=NULL){ 

        i++;
        token[i] = strtok(NULL,"/");
        
  

    }

    int j;
    for(j = 0; j<i-1; j++){
        sprintf(command,"%sls\n",command);
        sprintf(command,"%smkdir %s\n",command, token[j]);
        

        sprintf(command,"%scd %s\n",command, token[j]);



    }

    sprintf(command,"%stouch %s\n",command, token[i-1]);
    system(command);


    return;
}
