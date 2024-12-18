#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>  // For open()

#include "myls.h"         // Include your custom header for 'ls'
#include "mycat.h"        // Include your custom header for 'cat'
#include "mywc.h"         // Include your custom header for 'wc'
#include "server_command.h"  // Include the new server command header


#define MAX_INPUT 1024
#define MAX_ARGS 64
#define MAX_COMMANDS 10

void parse_input(char *input, char **commands) {
    int i = 0;
    char *token = strtok(input, "|");
    while (token != NULL) {
        commands[i++] = token;
        token = strtok(NULL, "|");
    }
    commands[i] = NULL;
}

void parse_command(char *command, char **args) {
    int i = 0;
    char *token = strtok(command, " ");
    while (token != NULL) {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;
}

// Function to display custom man pages
void display_man_page(const char *command) {
    char man_page_path[256];
    snprintf(man_page_path, sizeof(man_page_path), "man_pages/%s.txt", command);

    int fd = open(man_page_path, O_RDONLY);
    if (fd == -1) {
        perror("Man page not found");
        return;
    }

    char buffer[1024];
    ssize_t bytes_read;
    while ((bytes_read = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0';  // Null-terminate the string
        printf("%s", buffer);
    }

    close(fd);
}

int execute_one_command(char *command) {

    char *args[MAX_ARGS];
    parse_command(command , args);

    if (strcmp(args[0], "lmywc") == 0) {
        mywc_command(args);  // Call your custom wc command
    } else if (strcmp(args[0], "lmycat") == 0) {
        mycat_command(args);  // Call your custom cat command
    } else if (strcmp(args[0], "lmyls") == 0) {
        myls_command();  // Call your custom ls command
    } else if (strcmp(args[0], "mycat") == 0 ||
                strcmp(args[0], "mywc") == 0 ||
                strcmp(args[0], "myls") == 0) {
        server_command(args);  // Send the command to the server
    } else if (strcmp(args[0], "man") == 0) {
        if (args[1] == NULL) {
            printf("Usage: man [command]\n");
        } else {
            display_man_page(args[1]);
        }
    
    } else {
        execvp(args[0], args);  // Execute external command
    }
    return 0;
}
int createProcess(int inFd, int outFd, char *command) {

    pid_t pid;
    if ((pid = fork ()) == 0) {
        // Use inFd if it is set
        if (inFd != STDIN_FILENO) {
            dup2(inFd, STDIN_FILENO);
            close(inFd);
        }

        if (outFd != STDOUT_FILENO) {
            dup2(outFd, STDOUT_FILENO);
            close(outFd);
        }
        // Execute the command
        execute_one_command(command);
        // Return an error
        _exit(255);

    } else {

        int stat = 0;
        waitpid(pid, &stat, 0);
        // Check whether the child returned an error
        return WIFEXITED(stat) && WEXITSTATUS(stat) != 255;
    } 

    return 0;
}
void execute_commands(char *input) {
    char *commands[MAX_COMMANDS];
    
    int num_commands = 0;

    parse_input(input, commands);

    // Count the number of commands
    while (commands[num_commands] != NULL) {
        num_commands++;
    }

    // Declare input and output pipe arrays
    int fd[2];  // Pipes for input redirection
    int errorFree;
    int inFd = STDIN_FILENO;
    int outFd = STDOUT_FILENO;
    // Iterate through each command
    for (int i = 0; i < num_commands - 1; i++) {
        pipe(fd);
        errorFree = createProcess(inFd, fd[1], commands[i]);
        close(fd[1]);
        inFd = fd[0];
      
    }
    if (inFd != STDIN_FILENO) {
        dup2(inFd, STDIN_FILENO);
    }



    // Reset the fds
    // int outFd2 = dup(STDOUT_FILENO);
    // dup2(inFd, STDIN_FILENO);
    // dup2(outFd, STDOUT_FILENO);
    // Execute the last command


    if (execute_one_command(commands[num_commands - 1]) < 0) {
        // dup2(outFd2, STDOUT_FILENO);
        printf("Error: command not found!\n");

        exit(EXIT_FAILURE);
    }
}


int main() {
    char input[MAX_INPUT];

    while (1) {
        printf("mysh> ");
        fflush(stdout);

        if (fgets(input, MAX_INPUT, stdin) == NULL) {
            perror("Error reading input");
            continue;
        }

        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "exit") == 0) {
            break;
        }

        execute_commands(input);
    }

    return 0;
}
