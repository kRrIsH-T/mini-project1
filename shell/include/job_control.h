#ifndef JOB_CONTROL_H
#define JOB_CONTROL_H

#include "shell.h"
#include "signals.h"

void builtin_activities(Command *cmd);
void builtin_ping(Command *cmd);
void builtin_fg(Command *cmd);
void builtin_bg(Command *cmd);

#endif
