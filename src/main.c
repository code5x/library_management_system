#include "../include/login.h"
#include "../include/menu.h"
#include "../include/issue.h"
#include "../include/ui_util.h"

#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

/* Create required directories if not exist */
void init_directories() {
    mkdir("data", 0777);
    mkdir("logs", 0777);
}

int main() {
    init_directories();
    clear_screen();

    /* Login first */
    if (!login()) {
        printf("Program terminated\n");
        return 0;
    }

    /* Auto overdue check at program start */
    check_overdue_and_block_unblock_members();

    sleep(2);

    /* Show main menu */
    main_menu();

    printf("\nThank you for using Library Management System\n");
    return 0;
}
