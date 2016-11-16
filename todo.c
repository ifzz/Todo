#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>

#include "common.h"
#include "database.h"

static Date get_current_date()
{
    time_t t = time(NULL);
    struct tm *time = localtime(&t);
    return (Date){time->tm_year + 1900, time->tm_mon + 1, time->tm_mday};
}

static Time get_current_time()
{
    time_t t = time(NULL);
    struct tm *time = localtime(&t);
    return (Time){time->tm_hour, time->tm_min};
}

static char *next_tok(char **line)
{
    if (!line || !*line || !**line)
        return NULL;

    for (; isspace(**line); (*line)++)
        if (!**line)
            return NULL;
    char *start = *line;
    for (; !isspace(**line); (*line)++)
        if (!**line)
            return start;

    **line = '\0';
    (*line)++;
    return start;
}

void interactive_mode(Database *db)
{
    char *tok, *remaining, *line = NULL;
    size_t size;
    Event *events;

    for (;;) {
        printf("> ");
        fflush(stdout);

        if (getline(&line, &size, stdin) == -1)
            FATAL("Failed to read from stdin!");

        remaining = line;

        while (*remaining) {
            tok = next_tok(&remaining);

            if (tok == NULL)
                break;

            if (!strcmp(tok, "today")) {
                if (database_query_date(db, get_current_date(), &events, &size) != -1) {
                    event_print_arr(events, size, PRINT_ALL);
                    free(events);
                }
            } else if (!strcmp(tok, "tomorrow")) {
                if (database_query_date(db,
                                        date_add_days(get_current_date(), 1),
                                        &events, &size) != -1) {
                    event_print_arr(events, size, PRINT_ALL);
                    free(events);
                }
            } else if (!strcmp(tok, "quit") || !strcmp(tok, "q")) {
                exit(EXIT_SUCCESS);
            } else {
                fprintf(stderr, "Unrecognised token \"%s\"", tok);
                break;
            }
        }
    }
}

static FILE *get_default_file(void)
{
    char *home = getenv("HOME");
    if (home == NULL)
        exit(EXIT_FAILURE);

    char *path = "/.dbtodo";
    char *fullpath = malloc(strlen(home) + strlen(path) + 1);
    strcpy(fullpath, home);
    strcat(fullpath, path);

    FILE *f = fopen(fullpath, "r");
    free(fullpath);
    if (f == NULL)
        exit(EXIT_FAILURE);
    else
        return f;
}

int main(int argc, char **argv)
{

    FILE *f = get_default_file();

    /* if (!f) */
    /*     FATAL("Failed to open file \"%s\"\n", argv[1]); */

    Database db;
    database_load(&db, f);

    int option;
    while ((option = getopt(argc, argv, "fi")) != -1) {
        switch (option) {
        case 'f':
            break;
        case 'i':
            interactive_mode(&db);
            break;
        case '?':
            break;
        }
    }
}
