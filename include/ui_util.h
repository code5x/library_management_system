#ifndef UI_UTIL_H
#define UI_UTIL_H

#define PAUSE_AND_CLEAR() \
    do { press_enter_to_continue(); clear_screen(); } while (0)

void press_enter_to_continue();
void clear_screen();

#endif
