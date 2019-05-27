#include "scanner.h"

void set_verbose(int value) {
    Verbose = value;
}

int file_line_scanner(char *pattern, char *filename) {
    hs_database_t *database;
    hs_compile_error_t *compile_err;
    if (hs_compile(pattern, HS_FLAG_SOM_LEFTMOST | HS_FLAG_UTF8,
                HS_MODE_BLOCK, NULL, &database, &compile_err) != HS_SUCCESS) {
        fprintf(stderr, "ERROR: Unable to compile pattern \"%s\": %s\n", pattern,
                compile_err->message);
        hs_free_compile_error(compile_err);
        return -1;
    }


    hs_scratch_t *scratch = NULL;
    if (hs_alloc_scratch(database, &scratch) != HS_SUCCESS) {
        fprintf(stderr, "ERROR: Unable to allocate scratch space\n");
        hs_free_database(database);
        return -1;
    }

    /*char *input_data = "This is just a test string to match against";*/
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "ERROR: Unable to open file \"%s\": %s\n", filename,
                strerror(errno));
        hs_free_scratch(scratch);
        hs_free_database(database);
        return -1;
    }

    printf("\033[0;35m%s\n\033[0m", filename);

    size_t line_length;
    file_line ctx;
    ctx.pattern = pattern;
    ctx.filename = filename;
    ctx.line_no = 1;
    while (fgets(ctx.line, MAX_LINE_LENGTH, fp) != NULL) {
        ctx.line_length = strlen(ctx.line);
        if (ctx.line[ctx.line_length - 1] != '\n') { 
            fprintf(stderr, "WARNING: clipping line to %zu", ctx.line_length);
        }

        if (hs_scan(database, ctx.line, ctx.line_length, 0, scratch,
                    print_line_match, &ctx) != HS_SUCCESS) {
            fprintf(stderr, "ERROR: Unable to scan input buffer\n");
            hs_free_scratch(scratch);
            hs_free_database(database);
            return -1;
        }

        ctx.line_no++;
    }


    hs_free_scratch(scratch);
    hs_free_database(database);

    return 0;
}

int block_file_scanner(char *pattern, char *filename) {
    hs_database_t *database;
    hs_compile_error_t *compile_err;
    if (hs_compile(pattern, HS_FLAG_SOM_LEFTMOST | HS_FLAG_UTF8,
                HS_MODE_BLOCK, NULL, &database, &compile_err) != HS_SUCCESS) {
        fprintf(stderr, "ERROR: Unable to compile pattern \"%s\": %s\n", pattern,
                compile_err->message);
        hs_free_compile_error(compile_err);
        return -1;
    }


    hs_scratch_t *scratch = NULL;
    if (hs_alloc_scratch(database, &scratch) != HS_SUCCESS) {
        fprintf(stderr, "ERROR: Unable to allocate scratch space\n");
        hs_free_database(database);
        return -1;
    }

    /*char *input_data = "This is just a test string to match against";*/
    char *input_data;
    ssize_t length = read_file(filename, &input_data);

    if (length < 1) {
        // already freed buffer and displayed error in read_file()
        hs_free_scratch(scratch);
        hs_free_database(database);
        return -1;
    }

    printf("Scanning %zd bytes\n", length);

    // TODO: Update ctx to use a struct containing filename and line no map
    if (hs_scan(database, input_data, length, 0, scratch,
                event_handler, pattern) != HS_SUCCESS) {
        fprintf(stderr, "ERROR: Unable to scan input buffer\n");
        free(input_data);
        hs_free_scratch(scratch);
        hs_free_database(database);
        return -1;
    }

    free(input_data);
    hs_free_scratch(scratch);
    hs_free_database(database);

    return 0;
}

static ssize_t read_file(const char *filename, char **buffer) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        fprintf(stderr, "ERROR: Unable to open file \"%s\": %s\n", filename,
                strerror(errno));
        return -1;
    }

    // Seek to end and get length
    fseek(fp, 0L, SEEK_END);
    long length = ftell(fp);

    if (length <= 0) {
        fprintf(stderr, "ERROR: \"%s\" is an empty file\n", filename);
        fclose(fp);
        return -1;
    }

    // Go back to begining
    fseek(fp, 0L, SEEK_SET);

    if (length > UINT_MAX) {
        fprintf(stderr, "WARNING: clipping input file from %ld to %u bytes\n",
                length, UINT_MAX);
        length = UINT_MAX;
    }

    *buffer = malloc(length);
    if (!*buffer) {
        fprintf(stderr, "ERROR: unable to allocate buffer for file\n");
        fclose(fp);
        return -1;
    }

    char *placeholder = *buffer;
    size_t bytes_left = length;
    while (bytes_left) {
        size_t bytes_read = fread(placeholder, 1, bytes_left, fp);
        bytes_left -= bytes_read;
        placeholder += bytes_read;
        if (ferror(fp)) {
            fprintf(stderr, "ERROR: Unable to read file \"%s\"\n", filename);
            free(*buffer);
            fclose(fp);
            return -1;
        }
    }

    fclose(fp);

    return length;
}

static int event_handler(unsigned int id, unsigned long long from,
        unsigned long long to, unsigned int flags, void *ctx) {
    printf("Match for pattern \"%s\" at from %llu to %llu\n", (char *)ctx, from, to);
    return 0;
}

static int print_line_match(unsigned int id, unsigned long long from,
        unsigned long long to, unsigned int flags, void *ctx) {
    file_line *fl = (file_line *)ctx;
    char *p = fl->line;
    /*printf("Match for pattern \"%s:%ld\" at from %llu to %llu\n",*/
            /*fl->filename, fl->line_no, from, to);*/
    printf("\033[0;32m%ld\033[0m:\t", fl->line_no);
    if (from > INT_MAX || to > INT_MAX) {
        printf("%s", fl->line);
    } else {
        printf("%.*s", (int) from, fl->line);
        p = fl->line + from;
        printf("\033[0;31m%.*s\033[0m", (int) (to - from), p);
        p = fl->line + to;
        printf("%s", p);
    }
    return 0;
}
