#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include "mywc.h"

#define BUFFER_SIZE 1024

void mywc_command(char **args) {
    int fd;
    int lines, words, bytes, chars, max_line_length;

    // If no file is provided, read from stdin (STDIN_FILENO)
    if (args[1] == NULL) {
        fd = STDIN_FILENO;  // Standard input
    } else {
        // Open the file provided in the arguments
        fd = open(args[1], O_RDONLY);
        if (fd == -1) {
            perror("open failed");
            return;
        }
    }

    count_file(fd, &lines, &words, &bytes, &chars, &max_line_length);

    if (fd != STDIN_FILENO) {
        close(fd);  // Close the file if it was opened
    }

    printf("lines: %d words: %d bytes: %d %s\n", lines, words, bytes, args[1] ? args[1] : "(stdin)");
}

void count_file(int fd, int *lines, int *words, int *bytes, int *chars, int *max_line_length) {
    char buffer[BUFFER_SIZE];
    int num_read;
    int in_word = 0;
    int line_length = 0;

    *lines = *words = *bytes = *chars = *max_line_length = 0;

    // Read data from the file descriptor in chunks
    while ((num_read = read(fd, buffer, BUFFER_SIZE)) > 0) {
        for (int i = 0; i < num_read; i++) {
            char ch = buffer[i];
            (*bytes)++;
            (*chars)++;

            if (ch == '\n') {
                (*lines)++;
                if (line_length > *max_line_length) {
                    *max_line_length = line_length;
                }
                line_length = 0;
            } else {
                line_length++;
            }

            if (isspace(ch)) {
                in_word = 0;
            } else if (!in_word) {
                in_word = 1;
                (*words)++;
            }
        }
    }

    if (line_length > *max_line_length) {
        *max_line_length = line_length;
    }

    if (num_read == -1) {
        perror("read failed");
    }
}
