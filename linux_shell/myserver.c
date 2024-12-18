#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include "myls.h"
#include "mycat.h"
#include "mywc.h"

#define PORT 9090           // Use port 9090 as requested
#define BUFFER_SIZE 1024
#define MAX_ARGS 10         // Maximum number of command arguments

// Function to trim trailing whitespace (spaces, newlines, etc.)
void trim_whitespace(char *str) {
    char *end;

    // Trim leading space
    while (*str == ' ') str++;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\n' || *end == '\r')) end--;

    // Write the new null terminator character
    *(end + 1) = '\0';
}

// Function to parse the buffer into arguments
void parse_command(char *buffer, char *args[]) {
    char *token;
    int i = 0;

    // Use strtok to split the buffer into command and arguments
    token = strtok(buffer, " ");
    while (token != NULL && i < MAX_ARGS - 1) {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;  // Null-terminate the argument list
}

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    int bytes_received;

    // Receive data from client
    bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received < 0) {
        perror("Error receiving data from client");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    // Null-terminate the buffer to make it a proper string
    buffer[bytes_received] = '\0';

    // Trim any extra whitespace (such as newline characters)
    trim_whitespace(buffer);

    // Parse the command and arguments
    char *args[MAX_ARGS];
    parse_command(buffer, args);

    // Redirect stdout to a pipe so we can capture output
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe failed");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == 0) {  // Child process
        // Redirect stdout to pipe
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);  // Close read end of pipe

        // Execute the appropriate command with arguments
        if (strcmp(args[0], "mywc") == 0) {
            mywc_command(args);
        } else if (strcmp(args[0], "mycat") == 0) {
            mycat_command(args);  // Pass args to mycat_command
        } else if (strcmp(args[0], "myls") == 0) {
            myls_command();
        } else {
            printf("Invalid command received: %s\n", args[0]);
        }

        exit(EXIT_SUCCESS);
    } else if (pid > 0) {  // Parent process
        close(pipefd[1]);  // Close write end of pipe

        // Read from the pipe and send output back to the client
        char command_output[BUFFER_SIZE];
        ssize_t nread;
        while ((nread = read(pipefd[0], command_output, BUFFER_SIZE)) > 0) {
            send(client_socket, command_output, nread, 0);
        }

        // Close the pipe and wait for the child to exit
        close(pipefd[0]);
        wait(NULL);  // Wait for the child process to complete
    } else {
        perror("fork failed");
    }

    // Close client socket after processing
    close(client_socket);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // Create a socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Bind the socket to an IP address and port (PORT 9090)
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Binding failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 3) == -1) {
        perror("Listening failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);

    while (1) {
        // Accept an incoming connection
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_len);
        if (client_socket < 0) {
            perror("Failed to accept connection");
            continue;
        }

        printf("Accepted connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Handle the client request
        handle_client(client_socket);
    }

    // Close the server socket
    close(server_socket);

    return 0;
}
