#ifndef MYWC_H
#define MYWC_H

void mywc_command(char **args);
void count_file(int fd, int *lines, int *words, int *bytes, int *chars, int *max_line_length);

#endif
