#include "../include/parser.h"

int has_pipe(const char *input) {
    return strchr(input, '|') != NULL;
}

int has_redirect(const char *input) {
    return strchr(input, '<') != NULL || strchr(input, '>') != NULL;
}

char *trim_whitespace(char *str) {
    while (*str && isspace(*str)) str++;
    char *end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) end--;
    *(end + 1) = '\0';
    return str;
}
