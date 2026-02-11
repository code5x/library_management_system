#ifndef ISSUE_H
#define ISSUE_H

#define MAX_ISSUE_DAY 30
#define ISSUE_PAGE_SIZE 10
#define MAX_ISSUE_RESULTS 1000

#include <stdio.h>

/* Issue / Circulation structure */
typedef struct {
    char book_id[10];        // BK000001
    char member_id[10];      // MB00001

    char issue_date[11];     // YYYY-MM-DD
    char due_date[11];       // YYYY-MM-DD
    char return_date[11];    // YYYY-MM-DD (empty if not returned)

    int is_returned;         // 0 = not returned, 1 = returned
} Issue;

/* Circulation operations */
void issue_book();
void return_book();
void renew_book();
void view_issued_books_with_member();

/* Automatic operations */
void check_overdue_and_block_unblock_members();

/* Helper functions */
int is_book_issued_to_member(const char *book_id, const char *member_id);
int get_issued_record(const char *book_id, const char *member_id, Issue *issue);

#endif
