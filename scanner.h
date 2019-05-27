#ifndef SCANNER_H
#define SCANNER_H

#include <hs.h>
#include <stdio.h>
#include <stdlib.h>

#include <limits.h>
#include <errno.h>
#include <string.h>

#define MAX_LINE_LENGTH 1000

static int Verbose = 0;
void set_verbose(int value);

typedef struct {
    char *pattern;
    char *filename;
    long line_no;
    char line[MAX_LINE_LENGTH];
    size_t line_length;
} file_line;

int file_line_scanner(char *pattern, char *filename);

int block_file_scanner(char *pattern, char *filename);

static ssize_t read_file(const char *filename, char **buffer);

static int event_handler(unsigned int id, unsigned long long from,
        unsigned long long to, unsigned int flags, void *ctx);

static int print_line_match(unsigned int id, unsigned long long from,
        unsigned long long to, unsigned int flags, void *ctx);

#endif
