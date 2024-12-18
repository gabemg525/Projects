#include <stdio.h>
#include "mycat.h"

void mycat_command(char **args) {
    FILE *file = NULL;

    // Check if a file argument is provided
    if (args[1] == NULL) {
        file = stdin;  // Use standard input if no file is provided
    } else {
        file = fopen(args[1], "r");
        if (!file) {
            perror(args[1]);
            return;
        }
    }

    // Read from the file or stdin and print to stdout
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        printf("%s", buffer);
    }

    // Close the file if it's not stdin
    if (file != stdin) {
        fclose(file);
    }

    printf("\n");  // Print a newline for cleanliness
}
