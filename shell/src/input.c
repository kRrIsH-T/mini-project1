#include "../include/shell.h"

char *read_input() {
    char *line = malloc(COMMAND_MAX);
    if (line == NULL) {
        perror("malloc");
        return NULL;
    }

    if (fgets(line, COMMAND_MAX, stdin) == NULL) {
        free(line);
        return NULL;
    }

    line[strcspn(line, "\n")] = 0;
    return line;
}
