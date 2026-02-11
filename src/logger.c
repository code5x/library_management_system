#include "../include/logger.h"
#include "../include/file_handler.h"
#include <time.h>
#include <string.h>

/* Get current local date & time */
static void get_datetime(char *buffer, int size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", t);
}

/* Catalog (book) log */
void log_catalog_operation(
    const char *book_id,
    const char *book_name,
    const char *shelf_number,
    const char *operation,
    const char *second_operation
) {
    int exists = file_exists(CATALOG_LOG);

    FILE *fp = fopen(CATALOG_LOG, "a");
    if (fp == NULL) return;

    if (!exists) {
        fprintf(fp, "======================================================================================================================\n");
        fprintf(fp, "| Book Id  |           Book Title           |      Shelf       |     Date & Time     |  Operation  |  2nd Operation  |\n");
        fprintf(fp, "======================================================================================================================\n");
    }

    char datetime[20];
    get_datetime(datetime, sizeof(datetime));

    fprintf(fp, "| %-8s | %-30s | %-16s | %-19s | %-11s | %-15s |\n",
            book_id,
            book_name,
            shelf_number,
            datetime,
            operation,
            second_operation ? second_operation : "-");
    fprintf(fp, "----------------------------------------------------------------------------------------------------------------------\n");

    fclose(fp);
}

/* Member operation log */
void log_member_operation(
    const char *member_id,
    const char *member_name,
    const char *operation,
    const char *second_operation
) {
    int exists = file_exists(MEMBER_LOG);

    FILE *fp = fopen(MEMBER_LOG, "a");
    if (fp == NULL) return;

    if (!exists) {
        fprintf(fp, "========================================================================================\n");
        fprintf(fp, "| Member ID |     Member Name      |     Date & Time     |  Operation  | 2nd Operation |\n");
        fprintf(fp, "========================================================================================\n");
    }

    char datetime[20];
    get_datetime(datetime, sizeof(datetime));

    fprintf(fp, "| %-9s | %-20s | %-19s | %-11s | %-13s |\n",
            member_id,
            member_name,
            datetime,
            operation,
            second_operation ? second_operation : "-");
    fprintf(fp, "----------------------------------------------------------------------------------------\n");

    fclose(fp);
}

/* Circulation log */
void log_circulation_operation(
    const char *book_id,
    const char *book_name,
    const char *shelf_number,
    const char *member_id,
    const char *member_name,
    const char *operation
) {
    int exists = file_exists(CIRCULATION_LOG);

    FILE *fp = fopen(CIRCULATION_LOG, "a");
    if (fp == NULL) return;

    if (!exists) {
    fprintf(fp, "=======================================================================================================================================\n");
    fprintf(fp, "| Book Id  |           Book Title           |      Shelf       | Member ID |     Member Name      |     Date & Time     |  Operation  |\n");
    fprintf(fp, "=======================================================================================================================================\n");
    }

    char datetime[20];
    get_datetime(datetime, sizeof(datetime));

    fprintf(fp, "| %-8s | %-30s | %-16s | %-9s | %-20s | %-19s | %-11s |\n",
            book_id,
            book_name,
            shelf_number,
            member_id,
            member_name,
            datetime,
            operation);
    fprintf(fp, "---------------------------------------------------------------------------------------------------------------------------------------\n");

    fclose(fp);
}
