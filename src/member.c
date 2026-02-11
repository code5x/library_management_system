#include "../include/member.h"
#include "../include/file_handler.h"
#include "../include/validator.h"
#include "../include/logger.h"
#include "../include/date_util.h"
#include "../include/id_generator.h"
#include "../include/ui_util.h"

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* Common member type */
static const char *MEMBER_TYPE[MEMBER_TYPE_COUNT] = {
    "Student",
    "Teacher",
    "Staff",
    "Researcher",
    "General",
    "Others"
};

/* Choose member type from predefined list */
void choose_member_type(char *member_type) {
    int choice;

    while (1) {
        for (int i = 0; i < MEMBER_TYPE_COUNT; i++) {
            printf("\t%d. %s\n", i + 1, MEMBER_TYPE[i]);
        }

        choice = read_int_with_range(
            "Choose: ", 1, MEMBER_TYPE_COUNT, 0);

        strcpy(member_type, MEMBER_TYPE[choice - 1]);
        return;
    }
}

/* lowercase copy */
static void to_lower_copy(char *dst, const char *src) {
    int i = 0;
    while (src[i]) {
        dst[i] = tolower((unsigned char)src[i]);
        i++;
    }
    dst[i] = '\0';
}

/* case-insensitive partial match */
static int match_text(const char *field, const char *query) {
    char f[256], q[256];
    to_lower_copy(f, field);
    to_lower_copy(q, query);
    return strstr(f, q) != NULL;
}

/* Add new member */
void add_member() {
    Member member;
    memset(&member, 0, sizeof(Member));

    printf("\n\n============ Add new member ============\n\n");

    read_string("1. Name: ", member.name, sizeof(member.name), 0);

    while (1) {
        read_string("2. Date of birth (YYYY-MM-DD): ",
                    member.dob, sizeof(member.dob), 0);
        if (is_valid_date(member.dob))
            break;
        printf("Invalid date. Try again.\n");
    }

    read_string("3. Phone number (without country code): ",
                member.phone, sizeof(member.phone), 0);

    read_string("4. Address: ",
                member.address, sizeof(member.address), 0);

    printf("5. Type: \n");
    choose_member_type(member.type);

    member.status = 1;        /* active */
    member.borrow_count = 0;

    generate_member_id(member.id);

    FILE *fp = open_file_append(MEMBER_FILE);
    if (fp == NULL) {
        printf("Error: cannot open member file\n");
        PAUSE_AND_CLEAR();
        return;
    }

    fwrite(&member, sizeof(Member), 1, fp);
    fclose(fp);

    log_member_operation(
        member.id,
        member.name,
        "add member",
        "-"
    );

    printf("\nMember added successfully\n");
    printf("Member ID: %s\n", member.id);
    printf("Name: %s\n", member.name);
    printf("Type: %s\n", member.type);

    PAUSE_AND_CLEAR();
}

/* Edit member details */
void edit_member() {
    char input[50];
    Member member;
    int found = 0;
    printf("\n\n============ Edit Member Details ============\n\n");

    while (1) {
        printf("Enter the member id to be edit: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';

        FILE *fp = open_file_read(MEMBER_FILE);
        if (fp == NULL) {
            printf("Member database not found\n");
            PAUSE_AND_CLEAR();
            return;
        }

        while (fread(&member, sizeof(Member), 1, fp)) {
            if (strcmp(member.id, input) == 0) {
                found = 1;
                break;
            }
        }
        fclose(fp);

        if (found)
            break;

        printf("ID doesn't match\n");
        printf("1. Type again\n0. Go back\nChoose: ");
        fgets(input, sizeof(input), stdin);
        if (input[0] == '0') {
            clear_screen();
            return;
        }
    }

    while (1) {
        printf("\nMember details:\n");
        printf("ID: %s\n", member.id);
        printf("1. Name: %s\n", member.name);
        printf("2. Date of birth: %s\n", member.dob);
        printf("3. Phone number: %s\n", member.phone);
        printf("4. Address: %s\n", member.address);
        printf("5. Type: %s\n", member.type);

        if (member.status == 1)
            printf("Member is active\n6. Block member\n");
        else
            printf("Member is blocked\n6. Activate member\n");

        printf("Borrow count: %d\n", member.borrow_count);
        printf("0. Go back\n");

        int choice = read_int_with_range(
            "What you want to change. Choose (0-6): ", 0, 6, 0);

        if (choice == 0) {
            clear_screen();
            return;
        }

        switch (choice) {
        case 1:
            read_string("New name: ",
                        member.name, sizeof(member.name), 0);
            log_member_operation(member.id, member.name,
                                 "edit member", "name");
            break;

        case 2:
            while (1) {
                read_string("New date of birth (YYYY-MM-DD): ",
                            member.dob, sizeof(member.dob), 0);
                if (is_valid_date(member.dob))
                    break;
                printf("Invalid date\n");
            }
            log_member_operation(member.id, member.name,
                                 "edit member", "DOB");
            break;

        case 3:
            read_string("New phone number (without country code): ",
                        member.phone, sizeof(member.phone), 0);
            log_member_operation(member.id, member.name,
                                 "edit member", "phone number");
            break;

        case 4:
            read_string("New address: ",
                        member.address, sizeof(member.address), 0);
            log_member_operation(member.id, member.name,
                                 "edit member", "address");
            break;

        case 5:
            printf("5. New type: \n");
            choose_member_type(member.type);
            log_member_operation(member.id, member.name,
                                 "edit member", "type");
            break;

        case 6:
            if (member.status == 1) {
                if (confirm_yes_no("Are you confirm to block (y/n): ")) {
                    member.status = 0;
                    log_member_operation(member.id, member.name,
                                         "edit member", "make block");
                }
            } else {
                if (confirm_yes_no("Are you confirm to unblock (y/n): ")) {
                    member.status = 1;
                    log_member_operation(member.id, member.name,
                                         "edit member", "make active");
                }
            }
            break;
        }

        /* Rewrite record */
        FILE *fp = fopen(MEMBER_FILE, "rb+");
        Member temp;
        while (fread(&temp, sizeof(Member), 1, fp)) {
            if (strcmp(temp.id, member.id) == 0) {
                fseek(fp, -sizeof(Member), SEEK_CUR);
                fwrite(&member, sizeof(Member), 1, fp);
                break;
            }
        }
        printf("Member details update successful\n");
        fclose(fp);
        PAUSE_AND_CLEAR();
        return;
    }
}

/* Search / view members (basic) */
void search_view_member() {
    int choice;
    char input[100];

    while (1) {
        printf("\n\n============ Search / View members ============\n\n");
        printf("1. Search member with id\n");
        printf("2. Search member with name\n");
        printf("3. Search member with phone number\n");
        printf("4. Search member with address\n");
        printf("5. View members with type\n");
        printf("6. View members with borrow count\n");
        printf("7. View all members\n");
        printf("8. View only active members\n");
        printf("9. View only blocked members\n");
        printf("10. View only borrower members\n");
        printf("0. Go to main menu\n");

        choice = read_int_with_range("Choose: ", 0, 10, 0);
        if (choice == 0) {
            clear_screen();
            return;
        }

        Member results[MAX_RESULTS];
        int found = 0;

        FILE *fp = open_file_read(MEMBER_FILE);
        if (!fp) {
            printf("No member record found\n");
            PAUSE_AND_CLEAR();
            return;
        }

        /* ---------------- SEARCH CASES ---------------- */
        if (choice >= 1 && choice <= 4) {
            const char *field_name[] = {"", "id", "name", "phone number", "address"};
            printf("Enter member %s to search: ", field_name[choice]);
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = '\0';

            Member m;
            while (fread(&m, sizeof(Member), 1, fp)) {
                if ((choice == 1 && match_text(m.id, input)) ||
                    (choice == 2 && match_text(m.name, input)) ||
                    (choice == 3 && match_text(m.phone, input)) ||
                    (choice == 4 && match_text(m.address, input))) {
                    results[found++] = m;
                }
            }
        }

        /* ---------------- TYPE GROUP ---------------- */
        else if (choice == 5) {
            char types[50][50];
            int type_count[50] = {0};
            int total_types = 0;
            Member m;

            while (fread(&m, sizeof(Member), 1, fp)) {
                int idx = -1;
                for (int i = 0; i < total_types; i++) {
                    if (strcasecmp(types[i], m.type) == 0) {
                        idx = i;
                        break;
                    }
                }
                if (idx == -1) {
                    strcpy(types[total_types], m.type);
                    type_count[total_types++] = 1;
                } else {
                    type_count[idx]++;
                }
            }

            printf("\n");
            for (int i = 0; i < total_types; i++) {
                printf("%d. %s --> %d member found\n",
                       i + 1, types[i], type_count[i]);
            }
            printf("0. Go back\n");

            int t = read_int_with_range("Choose: ", 0, total_types, 0);
            if (t == 0) {
                fclose(fp);
                continue;
            }

            rewind(fp);
            while (fread(&m, sizeof(Member), 1, fp)) {
                if (strcasecmp(m.type, types[t - 1]) == 0)
                    results[found++] = m;
            }
        }

        /* ---------------- BORROW COUNT ---------------- */
        else if (choice == 6) {
            int bc = read_int_with_range(
                "Enter borrow count (0 - 5): ", 0, 5, 0);

            Member m;
            while (fread(&m, sizeof(Member), 1, fp)) {
                if (m.borrow_count == bc)
                    results[found++] = m;
            }
        }

        /* ---------------- SIMPLE FILTERS ---------------- */
        else {
            Member m;
            while (fread(&m, sizeof(Member), 1, fp)) {
                if (choice == 7)
                    results[found++] = m;
                else if (choice == 8 && m.status == 1)
                    results[found++] = m;
                else if (choice == 9 && m.status == 0)
                    results[found++] = m;
                else if (choice == 10 && m.borrow_count > 0)
                    results[found++] = m;
            }
        }

        fclose(fp);

        /* ---------------- DISPLAY WITH PAGINATION ---------------- */
        if (found == 0) {
            printf("\nNo record found\n");
            PAUSE_AND_CLEAR();
            continue;
        }

        int page = 0;
        int total_pages = (found + PAGE_SIZE - 1) / PAGE_SIZE;

        while (1) {
            int start = page * PAGE_SIZE;
            int end = start + PAGE_SIZE;
            if (end > found)
                end = found;

            printf("\n-------------------------------------------------------------------------------------------------------------------------------\n");
            printf("No  |   ID    |         Name         |    DOB     |   Phone    |            Address             |    Type    | Status  | Borrow\n");
            printf("-------------------------------------------------------------------------------------------------------------------------------\n");

            for (int i = start; i < end; i++) {
                printf("%-3d | %-7s | %-20s | %-10s | %-10s | %-30s | %-10s | %-7s | %d\n",
                       i + 1,
                       results[i].id,
                       results[i].name,
                       results[i].dob,
                       results[i].phone,
                       results[i].address,
                       results[i].type,
                       results[i].status ? "Active" : "Blocked",
                       results[i].borrow_count);
            }

            printf("\nPage %d of %d | total %d record found\n",
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
