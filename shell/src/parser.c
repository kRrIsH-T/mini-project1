#include "../include/shell.h"

void parse_input(char *input, Command *cmd) {
    cmd->argc = 0;

    char *dup = strdup(input);
    char *token = strtok(dup, " \t");

    while (token != NULL && cmd->argc < ARGS_MAX - 1) {
        cmd->argv[cmd->argc] = malloc(strlen(token) + 1);
        strcpy(cmd->argv[cmd->argc], token);
        cmd->argc++;
        token = strtok(NULL, " \t");
    }

    if (cmd->argc > 0) {
        cmd->argv[cmd->argc] = NULL;
    }

    free(dup);
}
