#include "../include/validator.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

/* Read string input */
#include <stdio.h>
#include <string.h>

void read_string(
    const char *prompt,
    char *buffer,
    int size,
    int allow_empty
) {
    while (1) {
        printf("%s", prompt);
        fflush(stdout);

        if (fgets(buffer, size, stdin) == NULL)
            continue;

        /* Check if input was too long */
        if (buffer[strlen(buffer) - 1] != '\n') {
            int ch;
            /* Clear remaining input */
            while ((ch = getchar()) != '\n' && ch != EOF);

            printf("Input is very long. Try again.\n");
            continue;
        }

        /* Remove newline */
        buffer[strcspn(buffer, "\n")] = '\0';

        if (!allow_empty && strlen(buffer) == 0) {
            printf("Input cannot be empty. Try again.\n");
            continue;
        }

        return;
    }
}

/* Read integer with range validation */
int read_int_with_range(
    const char *prompt,
    int min,
    int max,
    int allow_out_of_range
) {
    char input[50];
    int value;

    while (1) {
        printf("%s", prompt);
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL)
            continue;

        if (sscanf(input, "%d", &value) != 1) {
            printf("Invalid number. Try again.\n");
            continue;
        }

        if (value >= min && value <= max)
            return value;

        if (allow_out_of_range) {
            char confirm_prompt[100];
            snprintf(confirm_prompt, sizeof(confirm_prompt),
                     "Are you sure %d (y/n): ", value);

            if (confirm_yes_no(confirm_prompt))
                return value;
        }

        printf("Value out of range (%d - %d). Try again.\n", min, max);
    }
}

/* Yes / No confirmation */
int confirm_yes_no(const char *prompt) {
    char input[10];

    printf("%s", prompt);
    fflush(stdout);

    if (fgets(input, sizeof(input), stdin) == NULL)
        return 1;

    if (input[0] == '\n')
        return 1;

    return (input[0] == 'y' || input[0] == 'Y');
}

/* Convert string to lowercase */
void to_lowercase(char *str) {
    for (int i = 0; str[i]; i++)
        str[i] = tolower((unsigned char)str[i]);
}
