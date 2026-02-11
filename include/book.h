#ifndef BOOK_H
#define BOOK_H

#include <stdio.h>

#define SUBJECT_COUNT 24
#define PAGE_SIZE 10
#define MAX_RESULTS 1000

/* Book structure */
typedef struct {
    char id[10];              // BK000001
    char title[100];
    char author[100];
    char publisher[100];
    char subject[50];
    char isbn[20];
    char language[20];        // stored in lowercase
    int published_year;
    int edition;
    int pages;
    int total_quantity;
    int available_quantity;
    char shelf_number[50];
} Book;

/* Book management functions */
void add_book();
void edit_book();
void delete_book();
void search_book();
void view_books();

/* Helper functions */
void generate_shelf_number(Book *book);
int is_shelf_number_unique(const char *shelf);

#endif
