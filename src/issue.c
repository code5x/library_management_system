#include "../include/issue.h"
#include "../include/book.h"
#include "../include/member.h"
#include "../include/file_handler.h"
#include "../include/date_util.h"
#include "../include/logger.h"
#include "../include/validator.h"
#include "../include/ui_util.h"

#include <string.h>
#include <stdlib.h>

/* Check if book is issued to a member */
int is_book_issued_to_member(const char *book_id, const char *member_id) {
    FILE *fp = open_file_read(ISSUE_FILE);
    if (fp == NULL)
        return 0;

    Issue issue;
    while (fread(&issue, sizeof(Issue), 1, fp)) {
        if (strcmp(issue.book_id, book_id) == 0 &&
            strcmp(issue.member_id, member_id) == 0 &&
            issue.is_returned == 0) {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

/* Get issue record */
int get_issued_record(const char *book_id,
                      const char *member_id,
                      Issue *out_issue) {
    FILE *fp = open_file_read(ISSUE_FILE);
    if (fp == NULL)
        return 0;

    Issue issue;
    while (fread(&issue, sizeof(Issue), 1, fp)) {
        if (strcmp(issue.book_id, book_id) == 0 &&
            strcmp(issue.member_id, member_id) == 0 &&
            issue.is_returned == 0) {
            *out_issue = issue;
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

/* Issue book */
void issue_book() {
    char input[50];
    Book book;
    Member member;
    int found_book = 0, found_member = 0;
    printf("\n\n========== Issue Book to Member ==========\n\n");

    /* Find book */
    while (1) {
        printf("Enter the book id or shelf number to issue: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';

        FILE *fpb = open_file_read(BOOK_FILE);
        if (fpb == NULL) {
            printf("Book database not found\n");
            PAUSE_AND_CLEAR();
            return;
        }

        while (fread(&book, sizeof(Book), 1, fpb)) {
            if (strcmp(book.id, input) == 0 ||
                strcmp(book.shelf_number, input) == 0) {
                found_book = 1;
                break;
            }
        }
        fclose(fpb);

        if (found_book)
            break;

        printf("Book not found\n1. Try again\n0. Go back\nChoose: ");
        fgets(input, sizeof(input), stdin);
        if (input[0] == '0') {
            clear_screen();
            return;
        }
    }

    if (book.available_quantity < 1) {
        printf("This book is currently not available\n");
        PAUSE_AND_CLEAR();
        return;
    }

    /* Find member */
    while (1) {
        printf("Enter the member id: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';

        FILE *fpm = open_file_read(MEMBER_FILE);
        if (fpm == NULL) {
            printf("Member database not found\n");
            PAUSE_AND_CLEAR();
            return;
        }

        while (fread(&member, sizeof(Member), 1, fpm)) {
            if (strcmp(member.id, input) == 0) {
                found_member = 1;
                break;
            }
        }
        fclose(fpm);

        if (found_member)
            break;

        printf("Member not found\n1. Try again\n0. Go back\nChoose: ");
        fgets(input, sizeof(input), stdin);
        if (input[0] == '0') {
            clear_screen();
            return;
        }
    }

    if (member.status == 0) {
        printf("The member is blocked. Can't issue book to this member\n");
        PAUSE_AND_CLEAR();
        return;
    }

    if (member.borrow_count >= 5) {
        printf("Member borrow count reach limit. Can't issue book\n");
        PAUSE_AND_CLEAR();
        return;
    }

    if (is_book_issued_to_member(book.id, member.id)) {
        printf("This member already issue this book and has not returned\n");
        PAUSE_AND_CLEAR();
        return;
    }

    /* Create issue record */
    Issue issue;
    memset(&issue, 0, sizeof(Issue));
    strcpy(issue.book_id, book.id);
    strcpy(issue.member_id, member.id);
    get_today_date(issue.issue_date);
    add_days_to_date(issue.issue_date, MAX_ISSUE_DAY, issue.due_date);
    issue.is_returned = 0;

    FILE *fpi = open_file_append(ISSUE_FILE);
    fwrite(&issue, sizeof(Issue), 1, fpi);
    fclose(fpi);

    /* Update book */
    FILE *fpb = fopen(BOOK_FILE, "rb+");
    Book tempb;
    while (fread(&tempb, sizeof(Book), 1, fpb)) {
        if (strcmp(tempb.id, book.id) == 0) {
            tempb.available_quantity--;
            fseek(fpb, -sizeof(Book), SEEK_CUR);
            fwrite(&tempb, sizeof(Book), 1, fpb);
            break;
        }
    }
    fclose(fpb);

    /* Update member */
    FILE *fpm = fopen(MEMBER_FILE, "rb+");
    Member tempm;
    while (fread(&tempm, sizeof(Member), 1, fpm)) {
        if (strcmp(tempm.id, member.id) == 0) {
            tempm.borrow_count++;
            fseek(fpm, -sizeof(Member), SEEK_CUR);
            fwrite(&tempm, sizeof(Member), 1, fpm);
            break;
        }
    }
    fclose(fpm);

    log_circulation_operation(
        book.id,
        book.title,
        book.shelf_number,
        member.id,
        member.name,
        "issue"
    );

    printf("\nBook '%s : %s' issued to member '%s : %s'\n",
           book.title, book.id, member.name, member.id);
    printf("Issue date %s | Due date %s\n",
           issue.issue_date, issue.due_date);
    PAUSE_AND_CLEAR();
}

/* Return book */
void return_book() {
    char input[50];
    Book book;
    Member member;
    Issue issue;
    int found_book = 0, found_member = 0;
    printf("\n\n========== Return Book from Member ==========\n\n");

    /* Find book */
    while (1) {
        printf("Enter the book id or shelf number that returned: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';

        FILE *fpb = open_file_read(BOOK_FILE);
        if (fpb == NULL) {
            printf("Book database not found\n");
            PAUSE_AND_CLEAR();
            return;
        }

        while (fread(&book, sizeof(Book), 1, fpb)) {
            if (strcmp(book.id, input) == 0 ||
                strcmp(book.shelf_number, input) == 0) {
                found_book = 1;
                break;
            }
        }
        fclose(fpb);

        if (found_book)
            break;

        printf("Book not found\n1. Try again\n0. Go back\nChoose: ");
        fgets(input, sizeof(input), stdin);
        if (input[0] == '0') {
            clear_screen();
            return;
        }
    }

    if (book.available_quantity == book.total_quantity) {
        printf("This book is not to return. All quantity already available\n");
        PAUSE_AND_CLEAR();
        return;
    }

    /* Find member */
    while (1) {
        printf("Enter the member id: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';

        FILE *fpm = open_file_read(MEMBER_FILE);
        if (fpm == NULL) {
            printf("Memebr database not found\n");
            PAUSE_AND_CLEAR();
            return;
        }

        while (fread(&member, sizeof(Member), 1, fpm)) {
            if (strcmp(member.id, input) == 0) {
                found_member = 1;
                break;
            }
        }
        fclose(fpm);

        if (found_member)
            break;

        printf("Member not found\n1. Try again\n0. Go back\nChoose: ");
        fgets(input, sizeof(input), stdin);
        if (input[0] == '0') {
            clear_screen();
            return;
        }
    }

    if (!get_issued_record(book.id, member.id, &issue)) {
        printf("This member didn't borrow this book\n");
        PAUSE_AND_CLEAR();
        return;
    }

    get_today_date(issue.return_date);
    issue.is_returned = 1;

    /* Update issue record */
    FILE *fpi = fopen(ISSUE_FILE, "rb+");
    Issue tempi;
    while (fread(&tempi, sizeof(Issue), 1, fpi)) {
        if (strcmp(tempi.book_id, book.id) == 0 &&
            strcmp(tempi.member_id, member.id) == 0 &&
            tempi.is_returned == 0) {
            fseek(fpi, -sizeof(Issue), SEEK_CUR);
            fwrite(&issue, sizeof(Issue), 1, fpi);
            break;
        }
    }
    fclose(fpi);

    /* Update book */
    FILE *fpb = fopen(BOOK_FILE, "rb+");
    Book tempb;
    while (fread(&tempb, sizeof(Book), 1, fpb)) {
        if (strcmp(tempb.id, book.id) == 0) {
            tempb.available_quantity++;
            fseek(fpb, -sizeof(Book), SEEK_CUR);
            fwrite(&tempb, sizeof(Book), 1, fpb);
            break;
        }
    }
    fclose(fpb);

    /* Update member */
    FILE *fpm = fopen(MEMBER_FILE, "rb+");
    Member tempm;
    while (fread(&tempm, sizeof(Member), 1, fpm)) {
        if (strcmp(tempm.id, member.id) == 0) {
            tempm.borrow_count--;
            fseek(fpm, -sizeof(Member), SEEK_CUR);
            fwrite(&tempm, sizeof(Member), 1, fpm);
            break;
        }
    }
    fclose(fpm);

    log_circulation_operation(
        book.id,
        book.title,
        book.shelf_number,
        member.id,
        member.name,
        "return"
    );

    printf("\nBook '%s : %s' returned from member '%s : %s'\n",
           book.title, book.id, member.name, member.id);
    printf("Issue date %s , return date %s , borrow for %d days.\n",
           issue.issue_date, issue.return_date, get_day_difference(issue.issue_date, issue.return_date));
    PAUSE_AND_CLEAR();
}

/* Renew book */
void renew_book() {
    char input[50];
    Book book;
    Member member;
    Issue issue;
    int found_book = 0, found_member = 0;
    printf("\n\n============ Renew Issue Book ============\n\n");

    /* Find book */
    while (1) {
        printf("Enter the book id or shelf number to renew: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';

        FILE *fpb = open_file_read(BOOK_FILE);
        if (fpb == NULL) {
            printf("Book database not found\n");
            PAUSE_AND_CLEAR();
            return;
        }

        while (fread(&book, sizeof(Book), 1, fpb)) {
            if (strcmp(book.id, input) == 0 ||
                strcmp(book.shelf_number, input) == 0) {
                found_book = 1;
                break;
            }
        }
        fclose(fpb);

        if (found_book)
            break;

        printf("Book not found\n1. Try again\n0. Go back\nChoose: ");
        fgets(input, sizeof(input), stdin);
        if (input[0] == '0') {
            clear_screen();
            return;
        }
    }

    if (book.available_quantity == book.total_quantity) {
        printf("Can't renew this book. All quantity already available\n");
        PAUSE_AND_CLEAR();
        return;
    }

    /* Find member */
    while (1) {
        printf("Enter the member id: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';

        FILE *fpm = open_file_read(MEMBER_FILE);
        if (fpm == NULL) {
            printf("Member database not found\n");
            PAUSE_AND_CLEAR();
            return;
        }

        while (fread(&member, sizeof(Member), 1, fpm)) {
            if (strcmp(member.id, input) == 0) {
                found_member = 1;
                break;
            }
        }
        fclose(fpm);

        if (found_member)
            break;

        printf("Member not found\n1. Try again\n0. Go back\nChoose: ");
        fgets(input, sizeof(input), stdin);
        if (input[0] == '0') {
            clear_screen();
            return;
        }
    }

    if (!get_issued_record(book.id, member.id, &issue)) {
        printf("Can't renew. The member didn't borrow this book yet\n");
        PAUSE_AND_CLEAR();
        return;
    }

    get_today_date(issue.issue_date);
    add_days_to_date(issue.issue_date, MAX_ISSUE_DAY, issue.due_date);

    /* Update issue record */
    FILE *fpi = fopen(ISSUE_FILE, "rb+");
    Issue tempi;
    while (fread(&tempi, sizeof(Issue), 1, fpi)) {
        if (strcmp(tempi.book_id, book.id) == 0 &&
            strcmp(tempi.member_id, member.id) == 0 &&
            tempi.is_returned == 0) {
            fseek(fpi, -sizeof(Issue), SEEK_CUR);
            fwrite(&issue, sizeof(Issue), 1, fpi);
            break;
        }
    }
    fclose(fpi);

    log_circulation_operation(
        book.id,
        book.title,
        book.shelf_number,
        member.id,
        member.name,
        "renew"
    );

    printf("\nBook '%s : %s' renewed to member '%s : %s'\n",
           book.title, book.id, member.name, member.id);
    printf("Renew date %s | Due date %s\n",
           issue.issue_date, issue.due_date);
    PAUSE_AND_CLEAR();
}

/* fetch book title */
static void get_book_title(const char *book_id, char *title) {
    FILE *fp = open_file_read(BOOK_FILE);
    Book b;
    strcpy(title, "N/A");

    if (!fp) return;

    while (fread(&b, sizeof(Book), 1, fp)) {
        if (strcmp(b.id, book_id) == 0) {
            strcpy(title, b.title);
            break;
        }
    }
    fclose(fp);
}

/* fetch member name */
static void get_member_name(const char *member_id, char *name) {
    FILE *fp = open_file_read(MEMBER_FILE);
    Member m;
    strcpy(name, "N/A");

    if (!fp) return;

    while (fread(&m, sizeof(Member), 1, fp)) {
        if (strcmp(m.id, member_id) == 0) {
            strcpy(name, m.name);
            break;
        }
    }
    fclose(fp);
}

/* View issued book with member */
void view_issued_books_with_member() {
    int choice;
    char input[20];
    char today[11];
    get_today_date(today);

    while (1) {
        printf("\n\n======== View Issued Book With Member ========\n\n");
        printf("1. Issued and returned\n");
        printf("2. Issued but not returned (due date not exceeded)\n");
        printf("3. Issued but not returned (due date exceeded)\n");
        printf("0. Go back to main menu\n");

        choice = read_int_with_range("Choose: ", 0, 3, 0);
        if (choice == 0) {
            clear_screen();
            return;
        }

        FILE *fp = open_file_read(ISSUE_FILE);
        if (!fp) {
            printf("No issue record found\n");
            PAUSE_AND_CLEAR();
            return;
        }

        /* read all issue records */
        Issue issues[MAX_ISSUE_RESULTS];
        int total = 0;
        Issue temp;

        while (fread(&temp, sizeof(Issue), 1, fp)) {
            issues[total++] = temp;
        }
        fclose(fp);

        int found = 0;
        int page = 0;

        /* reverse traversal (latest first) */
        Issue results[MAX_ISSUE_RESULTS];

        for (int i = total - 1; i >= 0; i--) {
            Issue *is = &issues[i];

            if (choice == 1 && is->is_returned == 1)
                results[found++] = *is;

            else if (choice == 2 &&
                     is->is_returned == 0 &&
                     compare_dates(today, is->due_date) <= 0)
                results[found++] = *is;

            else if (choice == 3 &&
                     is->is_returned == 0 &&
                     compare_dates(today, is->due_date) > 0)
                results[found++] = *is;
        }

        if (found == 0) {
            printf("\nNo record found\n");
            continue;
        }

        int total_pages = (found + ISSUE_PAGE_SIZE - 1) / ISSUE_PAGE_SIZE;

        while (1) {
            int start = page * ISSUE_PAGE_SIZE;
            int end = start + ISSUE_PAGE_SIZE;
            if (end > found) end = found;

            if (choice == 1) {
                printf("--------------------------------------------------------------------------------------------------------------------------------------------\n");
                printf("SI  | Book ID  |           Book Title           | Member ID |     Member Name      | Issue Date  |  Due Date   | Return Date | Kept Days\n");
                printf("--------------------------------------------------------------------------------------------------------------------------------------------\n");
            } else if (choice == 2) {
                printf("---------------------------------------------------------------------------------------------------------------------------------\n");
                printf("SI  | Book ID  |           Book Title           | Member ID |     Member Name      | Issue Date  |  Due Date   | Days Kept\n");
                printf("---------------------------------------------------------------------------------------------------------------------------------\n");
            } else {
                printf("------------------------------------------------------------------------------------------------------------------------------\n");
                printf("SI  | Book ID  |           Book Title           | Member ID |     Member Name      | Issue Date  |  Due Date   | Days Exceeded\n");
                printf("------------------------------------------------------------------------------------------------------------------------------\n");
            }

            for (int i = start; i < end; i++) {
                char book_title[100];
                char member_name[100];

                get_book_title(results[i].book_id, book_title);
                get_member_name(results[i].member_id, member_name);

                int days;

                if (choice == 1)
                    days = get_day_difference(results[i].issue_date,
                                              results[i].return_date);
                else if (choice == 2)
                    days = get_day_difference(results[i].issue_date, today);
                else
                    days = get_day_difference(results[i].due_date, today);

                printf("%-3d | %-8s | %-30s | %-9s | %-20s | %-11s | %-11s",
                       i + 1,
                       results[i].book_id,
                       book_title,
                       results[i].member_id,
                       member_name,
                       results[i].issue_date,
                       results[i].due_date);

                if (choice == 1)
                    printf(" | %-11s | %-2d\n",
                           results[i].return_date, days);
                else
                    printf(" | %-2d\n", days);
            }

            printf("\nPage %d of %d | Total %d records\n",
                   page + 1, total_pages, found);

            printf("n = next, p = previous, q = quit : ");
            fgets(input, sizeof(input), stdin);

            if ((input[0] == 'n' || input[0] == 'N') && page < total_pages - 1)
                page++;
            else if ((input[0] == 'p' || input[0] == 'P') && page > 0)
                page--;
            else if (input[0] == 'q' || input[0] == 'Q')
                break;
        }
    }
}

/* Automatic overdue & return check and make block or unblock*/
void check_overdue_and_block_unblock_members() {
    FILE *fpm = open_file_read(MEMBER_FILE);
    if (!fpm)
        return;

    Member member;
    char today[11];
    get_today_date(today);

    /* Loop through all members */
    while (fread(&member, sizeof(Member), 1, fpm)) {

        int should_block = 0;

        FILE *fpi = open_file_read(ISSUE_FILE);
        if (fpi) {
            Issue issue;

            /* Check all issues of this member */
            while (fread(&issue, sizeof(Issue), 1, fpi)) {
                if (strcmp(issue.member_id, member.id) == 0 &&
                    issue.is_returned == 0 &&
                    compare_dates(today, issue.due_date) > 0) {

                    should_block = 1;
                    break;
                }
            }
            fclose(fpi);
        }

        /* Update member status ONLY if changed */
        if (should_block && member.status == 1) {
            member.status = 0;

            FILE *fpw = fopen(MEMBER_FILE, "rb+");
            Member temp;
            while (fread(&temp, sizeof(Member), 1, fpw)) {
                if (strcmp(temp.id, member.id) == 0) {
                    fseek(fpw, -sizeof(Member), SEEK_CUR);
                    fwrite(&member, sizeof(Member), 1, fpw);

                    log_member_operation(
                        member.id,
                        member.name,
                        "edit member",
                        "make block"
                    );
                    break;
                }
            }
            fclose(fpw);
        }

        else if (!should_block && member.status == 0) {
            member.status = 1;

            FILE *fpw = fopen(MEMBER_FILE, "rb+");
            Member temp;
            while (fread(&temp, sizeof(Member), 1, fpw)) {
                if (strcmp(temp.id, member.id) == 0) {
                    fseek(fpw, -sizeof(Member), SEEK_CUR);
                    fwrite(&member, sizeof(Member), 1, fpw);

                    log_member_operation(
                        member.id,
                        member.name,
                        "edit member",
                        "make active"
                    );
                    break;
                }
            }
            fclose(fpw);
        }
    }

    fclose(fpm);
}
