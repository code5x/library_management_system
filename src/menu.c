#include "../include/menu.h"
#include "../include/book.h"
#include "../include/member.h"
#include "../include/issue.h"
#include "../include/ui_util.h"

#include <stdio.h>

/* Main menu */
void main_menu() {
    int choice;

    while (1) {
        printf("\n\n============ Main Menu ============\n\n");
        printf("1. Add new book\n");
        printf("2. Edit book details\n");
        printf("3. Delete book record\n");
        printf("4. Search book record\n");
        printf("5. View book records\n");
        printf("6. Add new member\n");
        printf("7. Edit member details\n");
        printf("8. Search / View members\n");
        printf("9. Issue book to members\n");
        printf("10. Return issued book\n");
        printf("11. Renew issued book\n");
        printf("12. View issued book with member\n");
        printf("0. Exit\n");

        printf("Choose: ");
        scanf("%d", &choice);
        (void)getchar();  // clear newline

        switch (choice) {
        case 1:
            clear_screen();
            add_book();
            break;
        case 2:
            clear_screen();
            edit_book();
            break;
        case 3:
            clear_screen();
            delete_book();
            break;
        case 4:
            clear_screen();
            search_book();
            break;
        case 5:
            clear_screen();
            view_books();
            break;
        case 6:
            clear_screen();
            add_member();
            break;
        case 7:
            clear_screen();
            edit_member();
            break;
        case 8:
            clear_screen();
            search_view_member();
            break;
        case 9:
            clear_screen();
            issue_book();
            break;
        case 10:
            clear_screen();
            return_book();
            break;
        case 11:
            clear_screen();
            renew_book();
            break;
        case 12:
            clear_screen();
            view_issued_books_with_member();
            break;
        case 0:
            printf("Exiting program...\n");
            return;
        default:
            printf("Invalid choice\n");
        }
    }
}
