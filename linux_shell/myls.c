// myls.c
#include <stdio.h>
#include <dirent.h>
#include "myls.h"

void myls_command() {
    DIR *d;
    struct dirent *dir;
    d = opendir(".");

    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (dir->d_name[0] != '.') { // Hide hidden files
                printf("%s\n", dir->d_name);
            }
        }

        closedir(d);
    } else {
        perror("myls");
    }
}
