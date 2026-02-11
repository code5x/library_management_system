#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>

/* Log file paths */
#define CATALOG_LOG      "logs/catalog_history.log"
#define MEMBER_LOG       "logs/member_operation.log"
#define CIRCULATION_LOG  "logs/circulation_history.log"

/* Logger functions */
void log_catalog_operation(
    const char *book_id,
    const char *book_name,
    const char *shelf_number,
    const char *operation,
    const char *second_operation
);

void log_member_operation(
    const char *member_id,
    const char *member_name,
    const char *operation,
    const char *second_operation
);

void log_circulation_operation(
    const char *book_id,
    const char *book_name,
    const char *shelf_number,
    const char *member_id,
    const char *member_name,
    const char *operation
);

#endif
