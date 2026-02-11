#include "../include/id_generator.h"
#include <stdio.h>
#include <string.h>

#define ID_COUNTER_FILE "data/id_counter.dat"

typedef struct {
    long book_id;
    long member_id;
} IdCounter;

/* Load counters from file */
static void load_counters(IdCounter *counter) {
    FILE *fp = fopen(ID_COUNTER_FILE, "rb");
    if (fp == NULL) {
        counter->book_id = 0;
        counter->member_id = 0;
        return;
    }
    fread(counter, sizeof(IdCounter), 1, fp);
    fclose(fp);
}

/* Save counters to file */
static void save_counters(const IdCounter *counter) {
    FILE *fp = fopen(ID_COUNTER_FILE, "wb");
    fwrite(counter, sizeof(IdCounter), 1, fp);
    fclose(fp);
}

/* Generate book ID */
void generate_book_id(char *id) {
    IdCounter counter;
    load_counters(&counter);

    counter.book_id++;

    sprintf(id, "BK%06ld", counter.book_id);

    save_counters(&counter);
}

/* Generate member ID */
void generate_member_id(char *id) {
    IdCounter counter;
    load_counters(&counter);

    counter.member_id++;

    sprintf(id, "MB%05ld", counter.member_id);

    save_counters(&counter);
}
