#pragma once

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define COUNTOF(x) (sizeof(x) / sizeof(0[x]))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define FATAL(args...)                          \
    do {                                        \
        fprintf(stderr, args);                  \
        exit(EXIT_FAILURE);                     \
    } while(0)

#define RESET "\x1B[0m"
#define BOLD  "\x1B[1m"
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"

static char *str_dup(const char *s) //allocates new string
{
    char *ret_str = malloc(strlen(s) + 1);
    if (!ret_str)
        return ret_str;
    return strcpy(ret_str, s);
}

static unsigned strsubct(const char *str, const char *sub) //no side effects
{
    unsigned count = 0;
    if (str && *str && sub && *sub) {
        while ((str = strstr(str, sub))) {
            count++;
            str += strlen(sub);
        }
    }
    return count;
}

static char *strrepl(const char *str, const char *find, const char *repl) //allocates new string
{
    if (str && find && *find && repl) {
        int flen = strlen(find);
        int rlen = strlen(repl);
        int diff = rlen - flen;
        char *retstr = malloc(strlen(str) + 1 + MAX(0, diff * strsubct(str, find)));
        str = strcpy(retstr, str);
        while ((str = strstr(str, find))) {
            char *start = (char *)str + flen;
            size_t sz_rem = strlen(start);
            memmove(start + diff, start, sz_rem);
            strncpy((char *)str, repl, rlen);
            *(start + diff + sz_rem) = '\0';
            str += rlen;
        }
        return retstr;
    }
    return NULL;
}

static long getline(char **line, size_t *n, FILE *stream)
{
    int k;
    size_t c = 0;

    if (!*line) {
        *n = 4;
        *line = malloc(*n);
    }

    for (k = fgetc(stream); k != EOF && k != '\n'; c++, k = fgetc(stream)){

        if (c >= *n - 2) {
            *n *= 2;
            *line = realloc(*line, *n);
        }
        (*line)[c] = k;
    }

    if (c == 0 && k == EOF) {
        return -1;
    } else if (k == EOF) {
        if (ferror(stream))
            return -1;
    } else if (k == '\n') {
        (*line)[c] = '\n';
        c++;
    }

    (*line)[c] = '\0';
    return c;
}

static char *next_tok(char **line) //modifies line double ptr, copies token
{
    if (!line || !*line || !**line)
        return NULL;

    for (; isspace(**line); (*line)++)
        if (!**line)
            return NULL;

    char *start = *line;
    for (; !isspace(**line) && **line; (*line)++);

    char *ret;
    ret = malloc(1 + *line - start);
    memcpy(ret, start, *line - start);
    ret[*line - start] = '\0';

    for (; isspace(**line) && **line; (*line)++);

    return ret;
}

static char *rmqt(const char *str) //allocates new string
{
    if (*str == '"' && str[strlen(str) - 1] == '"') {
        char *ret = malloc(strlen(str) - 1);
        strncpy(ret, str + 1, strlen(str) - 1);
        ret[strlen(str) - 2] = '\0';
        return ret;
    } else {
        return str_dup(str);
    }
}

static char *addqt(const char *str) //allocates new string
{
    if (!str)
        return NULL;
    char *ret = malloc(strlen(str) + 3);
    strcpy(ret + 1, str);
    ret[0] = '\"';
    size_t size = strlen(ret);
    ret[size] = '\"';
    ret[size + 1] = '\0';
    return ret;
}

static void *add_element(void *base, size_t *nmemb, size_t size, unsigned i, void **new_elem)
{
    base = realloc(base, ++(*nmemb) * size);
    memmove((char *)base + (i + 1) * size,
            (char *)base + i * size,
            (*nmemb - (i + 1)) * size);
    *new_elem = ((char *)base + i * size);
    return base;
}

static void remove_element(void *base, size_t *nmemb, size_t size, unsigned i)
{
    (*nmemb)--;
    memmove((char *)base + i * size,
            (char *)base + (i + 1) * size,
            (*nmemb - i) * size);
}
