#include "../include/ui_util.h"
#include <stdio.h>
#include <stdlib.h>

/* Pause */
void press_enter_to_continue() {
    printf("\nPress ENTER key to continue...");
    fflush(stdout);

    while (getchar() != '\n');
}

/* Clear screen (Unix-based system) */
void clear_screen() {
    system("clear");
}
