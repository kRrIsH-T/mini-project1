#ifndef BUILTINS_H
#define BUILTINS_H

#include "shell.h"
#include "job_control.h"

int is_builtin(const char *cmd);
int execute_builtin(Command *cmd);
void builtin_hop(Command *cmd);
void builtin_reveal(Command *cmd);
void builtin_log(Command *cmd);
void builtin_activities(Command *cmd);
void builtin_ping(Command *cmd);
void builtin_fg(Command *cmd);
void builtin_bg(Command *cmd);

#endif
