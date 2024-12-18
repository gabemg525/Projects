#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_PORT 9090  // Define the server's port
#define SERVER_IP "127.0.0.1"  // Define the server's IP address

void server_command(char **args) {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[1024];
    int i = 0;
    
    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up server address struct
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    // Convert IP address
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid address / Address not supported");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection to server failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Build the command string
    memset(buffer, 0, sizeof(buffer));
    while (args[i] != NULL) {
        strcat(buffer, args[i]);
        strcat(buffer, " ");
        i++;
    }

    // Send the command to the server
    send(sockfd, buffer, strlen(buffer), 0);

    // Receive the response from the server
    memset(buffer, 0, sizeof(buffer));
    int bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        printf("%s", buffer);
    } else {
        printf("No response from server\n");
    }

    // Close the socket
    close(sockfd);
}
