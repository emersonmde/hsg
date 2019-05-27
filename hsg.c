#include <stdio.h>
#include <getopt.h>

#include "scanner.h"

void print_help(void);
void print_version(void);

int main(int argc, char **argv) {
    int c;
    static int verbose = 0;

    while (1) {
        static struct option long_options[] = {
            {"verbose", no_argument, &verbose, 1},
            {"help",    no_argument, NULL,     'H'},
            {"version", no_argument, NULL,     'V'}
        };
        int option_index = 0;

        c = getopt_long(argc, argv, "", long_options, &option_index);

        if (c == -1)
            break;
        
        switch (c) {
            case 0:
                // Set a flag; do nothing
                break;

            case 'H':
                print_help();
                return 0;

            case 'V':
                print_version();
                return 0;

            case '?':
                // Received an unknown option
                print_help();
                return 2;

            default:
                abort();
        }
    }

    char *pattern;
    char *filename;

    // TODO Default to stdin without filename
    // Print any remaining command line arguments
    if (optind + 2 <= argc) {
        // TODO Add multiple files
        pattern = argv[optind++];
        filename = argv[optind++];
    } else {
        printf("error: the following required arugments were not provided:\n");
        printf("\t<pattern>\n");
        printf("\t<path>\n");
        printf("\n");
        print_help();
        return 2;
    }

    if (verbose) {
        printf("Using pattern: %s\n", pattern);
        printf("Searching filename: %s\n", filename);
        set_verbose(verbose);
    }

    if (file_line_scanner(pattern, filename) < 0)
        return 1;

    return 0;
}


void print_help() {
    printf("usage: \n\thsg [--version] [--help] <pattern> <path>\n");
}

void print_version() {
    printf("hsg version 0.1\n");
}


