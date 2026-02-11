#include "../include/date_util.h"
#include <stdio.h>
#include <string.h>

/* Convert YYYY-MM-DD string to struct tm */
static int parse_date(const char *date_str, struct tm *tm_date) {
    if (strlen(date_str) != 10)
        return 0;

    int y, m, d;
    if (sscanf(date_str, "%d-%d-%d", &y, &m, &d) != 3)
        return 0;

    memset(tm_date, 0, sizeof(struct tm));
    tm_date->tm_year = y - 1900;
    tm_date->tm_mon  = m - 1;
    tm_date->tm_mday = d;

    /* Normalize and validate */
    if (mktime(tm_date) == -1)
        return 0;

    if (tm_date->tm_mday != d ||
        tm_date->tm_mon  != m - 1 ||
        tm_date->tm_year != y - 1900)
        return 0;

    return 1;
}

/* Get today's date */
void get_today_date(char *date_str) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    strftime(date_str, 11, "%Y-%m-%d", t);
}

/* Add days to a date */
void add_days_to_date(const char *start_date, int days, char *result_date) {
    struct tm tm_date;

    if (!parse_date(start_date, &tm_date)) {
        strcpy(result_date, "");
        return;
    }

    tm_date.tm_mday += days;
    mktime(&tm_date);

    strftime(result_date, 11, "%Y-%m-%d", &tm_date);
}

/* Compare two dates */
int compare_dates(const char *date1, const char *date2) {
    struct tm tm1, tm2;

    if (!parse_date(date1, &tm1) || !parse_date(date2, &tm2))
        return 0;

    time_t t1 = mktime(&tm1);
    time_t t2 = mktime(&tm2);

    if (t1 < t2) return -1;
    if (t1 > t2) return 1;
    return 0;
}

/* Validate date */
int is_valid_date(const char *date_str) {
    struct tm tm_date;
    return parse_date(date_str, &tm_date);
}

/* Get difference in days between two dates */
int get_day_difference(const char *start_date, const char *end_date) {
    struct tm tm_start, tm_end;

    /* Validate both dates */
    if (!parse_date(start_date, &tm_start) ||
        !parse_date(end_date, &tm_end)) {
        return 0;  /* invalid date */
    }

    time_t t_start = mktime(&tm_start);
    time_t t_end   = mktime(&tm_end);

    /* Difference in seconds */
    double diff_seconds = difftime(t_end, t_start);

    /* Convert seconds to days */
    int diff_days = (int)(diff_seconds / (60 * 60 * 24));

    return diff_days;
}
