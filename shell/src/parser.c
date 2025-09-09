#include "../include/shell.h"

void parse_input(char *input, Command *cmd) {
    cmd->argc = 0;
    char *token = strtok(input, " ");
    
    while (token != NULL && cmd->argc < ARGS_MAX - 1) {
        cmd->argv[cmd->argc++] = token;
        token = strtok(NULL, " ");
    }
    
    if (cmd->argc > 0) {
        cmd->argv[cmd->argc] = NULL;
    }
}
