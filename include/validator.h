#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <stdio.h>

/* Read integer with range validation */
int read_int_with_range(
    const char *prompt,
    int min,
    int max,
    int allow_out_of_range
);

/* Read string input */
void read_string(
    const char *prompt,
    char *buffer,
    int size,
    int allow_empty
);

/* Yes / No confirmation (default yes if press enter) */
int confirm_yes_no(const char *prompt);

/* Convert string to lowercase */
void to_lowercase(char *str);

#endif
