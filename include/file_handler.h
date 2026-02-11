#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <stdio.h>

/* File paths */
#define BOOK_FILE    "data/books.dat"
#define MEMBER_FILE  "data/members.dat"
#define ISSUE_FILE   "data/issues.dat"

/* File open helpers */
FILE* open_file_read(const char *filename);
FILE* open_file_write(const char *filename);
FILE* open_file_append(const char *filename);

/* Generic helpers */
int file_exists(const char *filename);
long get_file_record_count(const char *filename, size_t record_size);

#endif
