#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <pwd.h>

#define COMMAND_MAX 1024
#define ARGS_MAX 64
#define PATH_MAX 4096

typedef struct {
    int argc;
    char *argv[ARGS_MAX];
} Command;

void prompt();
char *read_input();
void parse_input(char *input, Command *cmd);
void execute_command(Command *cmd);
void log_command(const char *cmd);

#endif
