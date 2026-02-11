#include "../include/login.h"
#include <stdio.h>
#include <string.h>
#include <crypt.h>

int login() {
    char username[50];
    char password[50];

    while (1) {
        printf("\n============== LOGIN ==============\n\n");
        printf("Enter the Username: ");
        fgets(username, sizeof(username), stdin);
        username[strcspn(username, "\n")] = '\0';

        printf("Enter the Password: ");
        fgets(password, sizeof(password), stdin);
        password[strcspn(password, "\n")] = '\0';

        char *hash = crypt(password, LOGIN_PASSWORD_HASH);

        if (strcmp(username, LOGIN_USERNAME) == 0 &&
            hash != NULL &&
            strcmp(hash, LOGIN_PASSWORD_HASH) == 0) {
            printf("\nLogin successful\n");
            return 1;
        }

        printf("\nInvalid login\n");
        printf("1. Try again\n0. Exit\nChoose: ");
        fgets(username, sizeof(username), stdin);
        if (username[0] == '0')
            return 0;
    }
}
