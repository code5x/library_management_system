#include "../include/book.h"
#include "../include/file_handler.h"
#include "../include/validator.h"
#include "../include/logger.h"
#include "../include/id_generator.h"
#include "../include/ui_util.h"

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* Common book subjects */
static const char *SUBJECTS[SUBJECT_COUNT] = {
    "General",
    "Philosophy & Psychology",
    "Religion",
    "Social Science",
    "Language",
    "Science",
    "Technology",
    "Arts",
    "Literature",
    "History",
    "Geography and Travel",
    "Fiction",
    "Children's book",
    "Self help",
    "Business & Economic",
    "Health & wellness",
    "Education",
    "Biography & Autobiography",
    "Politics & Government",
    "Law & Legal Studies",
    "Environment & Ecology",
    "Mathematics",
    "Engineering",
    "Uncategorized"
};

/* Choose subject from predefined list */
void choose_subject(char *subject) {
    int choice;

    while (1) {
        for (int i = 0; i < SUBJECT_COUNT; i++) {
            printf("\t%d. %s\n", i + 1, SUBJECTS[i]);
        }

        choice = read_int_with_range(
            "Choose: ", 1, SUBJECT_COUNT, 0);

        strcpy(subject, SUBJECTS[choice - 1]);
        return;
    }
}

/* Check shelf number uniqueness */
int is_shelf_number_unique(const char *shelf) {
    FILE *fp = open_file_read(BOOK_FILE);
    if (fp == NULL)
        return 1;

    Book temp;
    while (fread(&temp, sizeof(Book), 1, fp)) {
        if (strcmp(temp.shelf_number, shelf) == 0) {
            fclose(fp);
            return 0;
        }
    }

    fclose(fp);
    return 1;
}

/* Generate shelf number automatically */
void generate_shelf_number(Book *book) {
    char subject_code[5] = "GEN";
    char author_code[5] = "AUT";
    char lang_code[5] = "LAN";

    /* Subject short code (first 3 chars) */
    strncpy(subject_code, book->subject, 3);
    subject_code[3] = '\0';

    /* Author short code (first 3 chars) */
    strncpy(author_code, book->author, 3);
    author_code[3] = '\0';

    /* Language short code (first 3 chars) */
    strncpy(lang_code, book->language, 3);
    lang_code[3] = '\0';

    for (int i = 0; subject_code[i]; i++)
        subject_code[i] = toupper(subject_code[i]);
    for (int i = 0; author_code[i]; i++)
        author_code[i] = toupper(author_code[i]);
    for (int i = 0; lang_code[i]; i++)
        lang_code[i] = toupper(lang_code[i]);

    sprintf(book->shelf_number, "%s.%s.%s.%d",
            subject_code,
            author_code,
            lang_code,
            book->published_year);
}

/* Add new book */
void add_book() {
    Book book;
    memset(&book, 0, sizeof(Book));

    printf("\n\n============ Add new book ============\n\n");

    read_string("1. Title: ", book.title, sizeof(book.title), 0);
    read_string("2. Author: ", book.author, sizeof(book.author), 0);
    read_string("3. Publisher: ", book.publisher, sizeof(book.publisher), 0);
    printf("4. Subject:\n");
    choose_subject(book.subject);
    read_string("5. ISBN: ", book.isbn, sizeof(book.isbn), 0);
    read_string("6. Language: ", book.language, sizeof(book.language), 0);
    to_lowercase(book.language);

    book.published_year = read_int_with_range(
        "7. Published year: ", 1500, 2100, 1);

    book.edition = read_int_with_range(
        "8. Edition: ", 1, 100, 1);

    book.pages = read_int_with_range(
        "9. Pages: ", 1, 10000, 1);

    int qty = read_int_with_range(
        "10. Quantity: ", 1, 10000, 1);

    book.total_quantity = qty;
    book.available_quantity = qty;

    generate_book_id(book.id);
    generate_shelf_number(&book);

    while (!is_shelf_number_unique(book.shelf_number)) {
        printf(
            "Shelf number %s is match with existing record\n",
            book.shelf_number
        );
        read_string("Enter a new shelf number: ",
                    book.shelf_number,
                    sizeof(book.shelf_number),
                    0);
    }

    FILE *fp = open_file_append(BOOK_FILE);
    if (fp == NULL) {
        printf("Error: cannot open book file\n");
        PAUSE_AND_CLEAR();
        return;
    }

    fwrite(&book, sizeof(Book), 1, fp);
    fclose(fp);

    log_catalog_operation(
        book.id,
        book.title,
        book.shelf_number,
        "add book",
        "-"
    );

    printf("\nBook added successfully\n");
    printf("Book ID: %s\n", book.id);
    printf("Title: %s\n", book.title);
    printf("Author: %s\n", book.author);
    printf("Shelf: %s\n", book.shelf_number);
    PAUSE_AND_CLEAR();
}

/* Edit book details */
void edit_book() {
    char input[50];
    Book book;
    FILE *fp;
    int found = 0;
    printf("\n\n============ Edit Book Details ============\n\n");

    while (1) {
        printf("Enter the book id or shelf number to be edit: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';

        fp = open_file_read(BOOK_FILE);
        if (fp == NULL) {
            printf("Book database not found\n");
            PAUSE_AND_CLEAR();
            return;
        }

        while (fread(&book, sizeof(Book), 1, fp)) {
            if (strcmp(book.id, input) == 0 ||
                strcmp(book.shelf_number, input) == 0) {
                found = 1;
                break;
            }
        }
        fclose(fp);

        if (found)
            break;

        printf("ID or shelf number doesn't match\n");
        printf("1. Type again\n0. Go back\nChoose: ");
        fgets(input, sizeof(input), stdin);
        if (input[0] == '0') {
            clear_screen();
            return;
        }
    }

    while (1) {
        printf("\nBook details:\n");
        printf("ID: %s\n", book.id);
        printf("1. Title: %s\n", book.title);
        printf("2. Author: %s\n", book.author);
        printf("3. Publisher: %s\n", book.publisher);
        printf("4. Subject: %s\n", book.subject);
        printf("5. ISBN: %s\n", book.isbn);
        printf("6. Language: %s\n", book.language);
        printf("7. Published year: %d\n", book.published_year);
        printf("8. Edition: %d\n", book.edition);
        printf("9. Pages: %d\n", book.pages);
        printf("Total quantity: %d\n", book.total_quantity);
        printf("Available quantity: %d\n", book.available_quantity);
        printf("10. Quantity (Add / remove)\n");
        printf("11. Shelf number: %s\n", book.shelf_number);
        printf("0. Go back\n");

        int choice = read_int_with_range(
            "What you want to change. Choose (0-11): ", 0, 11, 0);

        if (choice == 0) {
            clear_screen();
            return;
        }

        switch (choice) {
        case 1:
            read_string("New title: ", book.title, sizeof(book.title), 0);
            log_catalog_operation(book.id, book.title,
                                  book.shelf_number, "edit book", "title");
            break;

        case 2:
            read_string("New author: ", book.author, sizeof(book.author), 0);
            log_catalog_operation(book.id, book.title,
                                  book.shelf_number, "edit book", "author");
            break;

        case 3:
            read_string("New publisher: ", book.publisher, sizeof(book.publisher), 0);
            log_catalog_operation(book.id, book.title,
                                  book.shelf_number, "edit book", "publisher");
            break;

        case 4:
            printf("3. Subject:\n");
            choose_subject(book.subject);
            log_catalog_operation(book.id, book.title,
                                  book.shelf_number, "edit book", "subject");
            break;

        case 5:
            read_string("New ISBN: ", book.isbn, sizeof(book.isbn), 0);
            log_catalog_operation(book.id, book.title,
                                  book.shelf_number, "edit book", "ISBN");
            break;

        case 6:
            read_string("New language: ", book.language, sizeof(book.language), 0);
            to_lowercase(book.language);
            log_catalog_operation(book.id, book.title,
                                  book.shelf_number, "edit book", "language");
            break;

        case 7:
            book.published_year = read_int_with_range(
                "New published year: ", 1500, 2100, 1);
            log_catalog_operation(book.id, book.title,
                                  book.shelf_number, "edit book", "published year");
            break;

        case 8:
            book.edition = read_int_with_range(
                "New edition: ", 1, 100, 1);
            log_catalog_operation(book.id, book.title,
                                  book.shelf_number, "edit book", "edition");
            break;

        case 9:
            book.pages = read_int_with_range(
                "New pages: ", 1, 10000, 1);
            log_catalog_operation(book.id, book.title,
                                  book.shelf_number, "edit book", "pages");
            break;

        case 10: {
            int opts = 0;
            char line[20];

            do {
                printf("1. Add quantity\n2. Remove quantity\nChoose: ");
                fgets(line, sizeof(line), stdin);
                sscanf(line, "%d", &opts);

                int num;
                if (opts == 1) {
                    num = read_int_with_range("Enter number: ", 1, 10000, 1);
                    book.total_quantity += num;
                    book.available_quantity += num;
                    log_catalog_operation(book.id, book.title,
                                          book.shelf_number, "edit book", "add quantity");
                } else if (opts == 2) {
                    num = read_int_with_range("Enter number: ", 1, 10000, 1);
                    if (num <= book.available_quantity &&
                        num < book.total_quantity) {
                        book.total_quantity -= num;
                        book.available_quantity -= num;
                        log_catalog_operation(book.id, book.title,
                                              book.shelf_number, "edit book", "remove quantity");
                    } else {
                        printf("Invalid quantity\n");
                        PAUSE_AND_CLEAR();
                        return;
                    }
                } else {
                    printf("Invalid option\n");
                }

            } while (opts != 1 && opts != 2);

            break;
        }

        case 11:
            while (1) {
                read_string("New shelf number: ",
                            book.shelf_number,
                            sizeof(book.shelf_number),
                            0);
                if (is_shelf_number_unique(book.shelf_number))
                    break;
                printf("Shelf number already exists. Try again.\n");
            }
            log_catalog_operation(book.id, book.title,
                                  book.shelf_number, "edit book", "shelf number");
            break;
        }

        /* Rewrite record */
        fp = fopen(BOOK_FILE, "rb+");
        Book temp;
        while (fread(&temp, sizeof(Book), 1, fp)) {
            if (strcmp(temp.id, book.id) == 0) {
                fseek(fp, -sizeof(Book), SEEK_CUR);
                fwrite(&book, sizeof(Book), 1, fp);
                break;
            }
        }
        printf("Book record update successful\n");
        fclose(fp);
        PAUSE_AND_CLEAR();
        return;
    }
}

/* Delete book */
void delete_book() {
    char input[50];
    Book book;
    int found = 0;
    printf("\n\n============ Delete Book Record ============\n\n");

    while (1) {
        printf("Enter the book id or shelf number to be delete: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';

        FILE *fp = open_file_read(BOOK_FILE);
        if (fp == NULL) {
            printf("Book database not found\n");
            PAUSE_AND_CLEAR();
            return;
        }

        while (fread(&book, sizeof(Book), 1, fp)) {
            if (strcmp(book.id, input) == 0 ||
                strcmp(book.shelf_number, input) == 0) {
                found = 1;
                break;
            }
        }
        fclose(fp);

        if (found)
            break;

        printf("ID or shelf number doesn't match\n");
        printf("1. Type again\n0. Go back\nChoose: ");
        fgets(input, sizeof(input), stdin);
        if (input[0] == '0') {
            clear_screen();
            return;
        }
    }

    printf("\nBook details:\n");
    printf("ID: %s\nTitle: %s\nAuthor: %s\n", book.id, book.title, book.author);
    printf("Total quantity: %d\nAvailable quantity: %d\n",
           book.total_quantity, book.available_quantity);

    if (book.total_quantity != book.available_quantity) {
        printf("Someone had borrowed this book. Can't delete record at this moment\n");
        PAUSE_AND_CLEAR();
        return;
    }

    if (!confirm_yes_no("It will delete all records for this book. Are you sure to delete? (y/n): ")) {
        PAUSE_AND_CLEAR();
        return;
    }

    FILE *fp = open_file_read(BOOK_FILE);
    FILE *temp = fopen("data/temp_books.dat", "wb");

    Book b;
    while (fread(&b, sizeof(Book), 1, fp)) {
        if (strcmp(b.id, book.id) != 0)
            fwrite(&b, sizeof(Book), 1, temp);
    }

    fclose(fp);
    fclose(temp);

    remove(BOOK_FILE);
    rename("data/temp_books.dat", BOOK_FILE);

    log_catalog_operation(
        book.id,
        book.title,
        book.shelf_number,
        "delete book",
        "-"
    );

    printf("Book deleted successfully\n");
    PAUSE_AND_CLEAR();
}

/* helper: convert string to lowercase copy */
static void str_to_lower_copy(char *dest, const char *src) {
    int i = 0;
    while (src[i]) {
        dest[i] = tolower((unsigned char)src[i]);
        i++;
    }
    dest[i] = '\0';
}

/* helper: case-insensitive partial match */
static int match_text(const char *field, const char *query) {
    char field_l[256];
    char query_l[256];

    str_to_lower_copy(field_l, field);
    str_to_lower_copy(query_l, query);

    return strstr(field_l, query_l) != NULL;
}

/* Search book with pagination */
void search_book() {
    int choice;
    char query[100];
    char input[10];

    do {
        printf("\n\n============= Search Book =============\n\n");
        printf("Search book by -\n");
        printf("1. Id\n");
        printf("2. Title\n");
        printf("3. Author\n");
        printf("4. Publisher\n");
        printf("5. Subject\n");
        printf("6. ISBN\n");
        printf("7. Language\n");
        printf("8. Published year\n");
        printf("9. Shelf number\n");
        printf("0. Go back\n");

        choice = read_int_with_range("Choose: ", 0, 9, 0);
        if (choice == 0) {
            clear_screen();
            return;
        }

        const char *field_name[] = {
            "", "id", "title", "author", "publisher", "subject",
            "ISBN", "language", "published year", "shelf number"
        };

        printf("Enter the %s to search: ", field_name[choice]);
        fgets(query, sizeof(query), stdin);
        query[strcspn(query, "\n")] = '\0';

        printf("Search for\n");
        printf("1. All books (default)\n");
        printf("2. Only available book\n");
        printf("Choose: ");
        fgets(input, sizeof(input), stdin);

        int only_available = (input[0] == '2');

        FILE *fp = open_file_read(BOOK_FILE);
        if (fp == NULL) {
            printf("Book database not found\n");
            PAUSE_AND_CLEAR();
            return;
        }

        /* collect matched records */
        Book results[MAX_RESULTS];
        int total_found = 0;
        Book book;

        while (fread(&book, sizeof(Book), 1, fp)) {

            if (only_available && book.available_quantity <= 0)
                continue;

            int match = 0;

            switch (choice) {
            case 1:
                match = match_text(book.id, query);
                break;
            case 2:
                match = match_text(book.title, query);
                break;
            case 3:
                match = match_text(book.author, query);
                break;
            case 4:
                match = match_text(book.publisher, query);
                break;
            case 5:
                match = match_text(book.subject, query);
                break;
            case 6:
                match = match_text(book.isbn, query);
                break;
            case 7:
                match = match_text(book.language, query);
                break;
            case 8:
                if (atoi(query) == book.published_year)
                    match = 1;
                break;
            case 9:
                match = match_text(book.shelf_number, query);
                break;
            }

            if (match && total_found < MAX_RESULTS) {
                results[total_found++] = book;
            }
        }
        fclose(fp);

        if (total_found == 0) {
            printf("\nNo record found\n");
        } else {
            int page = 0;
            int total_pages = (total_found + PAGE_SIZE - 1) / PAGE_SIZE;

            while (1) {
                int start = page * PAGE_SIZE;
                int end = start + PAGE_SIZE;
                if (end > total_found)
                    end = total_found;

                printf("\n-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
                printf("No  |    ID    |             Title              |          Author           |         Publisher         |          Subject          |      ISBN       |  Language  | Year | Ed | Pages | Total | Avail | Shelf number\n");
                printf("-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

                for (int i = start; i < end; i++) {
                    printf("%-3d | %-8s | %-30s | %-25s | %-25s | %-25s | %-15s | %-10s | %-4d | %-2d | %-5d | %-5d | %-5d | %-16s\n",
                           i + 1,                      /* serial number */
                           results[i].id,
                           results[i].title,
                           results[i].author,
                           results[i].publisher,
                           results[i].subject,
                           results[i].isbn,
                           results[i].language,
                           results[i].published_year,
                           results[i].edition,
                           results[i].pages,
                           results[i].total_quantity,
                           results[i].available_quantity,
                           results[i].shelf_number);
                }

                printf("\nPage %d of %d | Total %d records found\n",
                       page + 1, total_pages, total_found);

                printf("n = next page, p = previous page, q = quit : ");
                fgets(input, sizeof(input), stdin);

                if ((input[0] == 'n' || input[0] == 'N') && page < total_pages - 1)
                    page++;
                else if ((input[0] == 'p' || input[0] == 'P') && page > 0)
                    page--;
                else if (input[0] == 'q' || input[0] == 'Q')
                    break;
            }
        }

        printf("\nDo you want to search again (y/n): ");
        fgets(input, sizeof(input), stdin);

    } while (input[0] == 'y' || input[0] == 'Y');
}

/* View Books */
void view_books() {
    int choice;
    char input[50];

    while (1) {
        printf("\n\n============ View book list ============\n\n");
        printf("1. View all books\n");
        printf("2. View only available books\n");
        printf("3. View only not available books\n");
        printf("4. View only issued books\n");
        printf("5. View under subject\n");
        printf("6. View under author\n");
        printf("7. View under publisher\n");
        printf("8. View under language\n");
        printf("9. View under published year\n");
        printf("10. View under edition\n");
        printf("0. Go back\n");

        choice = read_int_with_range("Choose: ", 0, 10, 0);
        if (choice == 0) {
            clear_screen();
            return;
        }

        FILE *fp = open_file_read(BOOK_FILE);
        if (!fp) {
            printf("No book record found\n");
            PAUSE_AND_CLEAR();
            return;
        }

        Book results[MAX_RESULTS];
        int count = 0;
        Book book;

        /* ---------------- SIMPLE FILTER CASES ---------------- */
        if (choice >= 1 && choice <= 4) {
            while (fread(&book, sizeof(Book), 1, fp)) {
                if (choice == 2 && book.available_quantity <= 0) continue;
                if (choice == 3 && book.available_quantity > 0) continue;
                if (choice == 4 &&
                    book.available_quantity == book.total_quantity) continue;

                results[count++] = book;
            }
            fclose(fp);
        }

        /* ---------------- GROUP FILTER CASES ---------------- */
        else {
            char groups[100][100];
            int group_count[100] = {0};
            int total_groups = 0;

            while (fread(&book, sizeof(Book), 1, fp)) {
                char key[100], key_l[100];
                int found = -1;

                if (choice == 5) strcpy(key, book.subject);
                if (choice == 6) strcpy(key, book.author);
                if (choice == 7) strcpy(key, book.publisher);
                if (choice == 8) strcpy(key, book.language);
                if (choice == 9) sprintf(key, "%d", book.published_year);
                if (choice == 10) sprintf(key, "%d", book.edition);

                str_to_lower_copy(key_l, key);

                for (int i = 0; i < total_groups; i++) {
                    char temp[100];
                    str_to_lower_copy(temp, groups[i]);
                    if (strcmp(temp, key_l) == 0) {
                        found = i;
                        break;
                    }
                }

                if (found == -1) {
                    strcpy(groups[total_groups], key);
                    group_count[total_groups] = 1;
                    total_groups++;
                } else {
                    group_count[found]++;
                }
            }
            fclose(fp);

            int group_page = 0;
            int total_group_pages = (total_groups + PAGE_SIZE - 1) / PAGE_SIZE;

            int selected_group = -1;

            while (1) {
                int start = group_page * PAGE_SIZE;
                int end = start + PAGE_SIZE;
                if (end > total_groups)
                    end = total_groups;

                printf("\n----------------------------------\n");
                for (int i = start; i < end; i++) {
                    if (choice == 9) {
                        printf("%d. Year %s --> %d book found\n", i + 1, groups[i], group_count[i]);
                    } else if (choice == 10) {
                        printf("%d. Edition %s --> %d book found\n", i + 1, groups[i], group_count[i]);
                    } else {
                        printf("%d. %s --> %d book found\n", i + 1, groups[i], group_count[i]);
                    }
                }
                printf("----------------------------------\n");
                printf("Page %d of %d\n",
                group_page + 1, total_group_pages);

                printf("n = next, p = previous, 0 = back\n");
                printf("Or choose number: ");

                fgets(input, sizeof(input), stdin);

                if ((input[0] == 'n' || input[0] == 'N') && group_page < total_group_pages - 1) {
                    group_page++;
                    continue;
                }

                if ((input[0] == 'p' || input[0] == 'P') && group_page > 0) {
                    group_page--;
                    continue;
                }

                int num = atoi(input);
                if (num == 0)
                    break;

                if (num >= 1 && num <= total_groups) {
                    selected_group = num - 1;
                    break;
                }

                printf("Invalid choice\n");
            }

            if (selected_group == -1)
                continue;

            fp = open_file_read(BOOK_FILE);
            while (fread(&book, sizeof(Book), 1, fp)) {
                if (choice == 5 && strcmp(book.subject, groups[selected_group]) == 0)
                    results[count++] = book;
                if (choice == 6 && strcasecmp(book.author, groups[selected_group]) == 0)
                    results[count++] = book;
                if (choice == 7 && strcasecmp(book.publisher, groups[selected_group]) == 0)
                    results[count++] = book;
                if (choice == 8 && strcasecmp(book.language, groups[selected_group]) == 0)
                    results[count++] = book;
                if (choice == 9 && book.published_year == atoi(groups[selected_group]))
                    results[count++] = book;
                if (choice == 10 && book.edition == atoi(groups[selected_group]))
                    results[count++] = book;
            }
            fclose(fp);
        }

        /* ---------------- DISPLAY WITH PAGINATION ---------------- */
        if (count == 0) {
            printf("\nNo record found\n");
            PAUSE_AND_CLEAR();
            continue;
        }

        int page = 0;
        int total_pages = (count + PAGE_SIZE - 1) / PAGE_SIZE;

        while (1) {
            int start = page * PAGE_SIZE;
            int end = start + PAGE_SIZE;
            if (end > count) end = count;

            printf("\n-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
            printf("No  |    ID    |             Title              |          Author           |         Publisher         |          Subject          |      ISBN       |  Language  | Year | Ed | Pages | Total | Avail | Shelf number\n");
            printf("-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

            for (int i = start; i < end; i++) {
                printf("%-3d | %-8s | %-30s | %-25s | %-25s | %-25s | %-15s | %-10s | %-4d | %-2d | %-5d | %-5d | %-5d | %-16s\n",
                       i + 1,
                       results[i].id,
                       results[i].title,
                       results[i].author,
                       results[i].publisher,
                       results[i].subject,
                       results[i].isbn,
                       results[i].language,
                       results[i].published_year,
                       results[i].edition,
                       results[i].pages,
                       results[i].total_quantity,
                       results[i].available_quantity,
                       results[i].shelf_number);
            }

            printf("\nPage %d of %d | Total %d records found\n",
                   page + 1, total_pages, count);

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
