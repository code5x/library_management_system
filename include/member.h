#ifndef MEMBER_H
#define MEMBER_H

#define MEMBER_TYPE_COUNT 6
#define PAGE_SIZE 10
#define MAX_RESULTS 1000

#include <stdio.h>

/* Member structure */
typedef struct {
    char id[10];          // MB00001
    char name[100];
    char dob[15];         // YYYY-MM-DD
    char phone[15];
    char address[200];
    int status;           // 1 = active, 0 = blocked
    char type[20];        // Student, Teacher, etc
    int borrow_count;
} Member;

/* Member management functions */
void add_member();
void edit_member();
void search_view_member();

#endif
