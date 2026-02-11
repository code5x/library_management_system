#include "../include/file_handler.h"
#include <stdlib.h>

/* Check if file exists */
int file_exists(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL)
        return 0;

    fclose(fp);
    return 1;
}

/* Open file for reading */
FILE* open_file_read(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    return fp;
}

/* Open file for writing (overwrite) */
FILE* open_file_write(const char *filename) {
    FILE *fp = fopen(filename, "wb");
    return fp;
}

/* Open file for append */
FILE* open_file_append(const char *filename) {
    FILE *fp = fopen(filename, "ab");
    return fp;
}

/* Get total record count in binary file */
long get_file_record_count(const char *filename, size_t record_size) {
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL)
        return 0;

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fclose(fp);

    return size / record_size;
}
