#ifndef DATE_UTIL_H
#define DATE_UTIL_H

#include <time.h>

/* Get today date as string YYYY-MM-DD */
void get_today_date(char *date_str);

/* Add days to a given date (YYYY-MM-DD) */
void add_days_to_date(const char *start_date, int days, char *result_date);

/* Compare two dates (YYYY-MM-DD)
   return:
   -1 if date1 < date2
    0 if date1 == date2
    1 if date1 > date2
*/
int compare_dates(const char *date1, const char *date2);

/* Validate date format and value */
int is_valid_date(const char *date_str);

/* Get difference in days between two dates */
int get_day_difference(const char *start_date, const char *end_date);

#endif
