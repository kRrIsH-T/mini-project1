#ifndef BUILTINS_H
#define BUILTINS_H

#include "shell.h"

int is_builtin(const char *cmd);
int execute_builtin(Command *cmd);
void builtin_hop(Command *cmd);
void builtin_reveal(Command *cmd);
void builtin_log(Command *cmd);

#endif
