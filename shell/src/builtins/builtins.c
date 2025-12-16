#include "../../include/builtins.h"
#include <string.h>
#include <stdio.h>

int is_builtin(char *command)
{
    return strcmp(command, "hop") == 0 ||
           strcmp(command, "reveal") == 0 ||
           strcmp(command, "log") == 0 ||
           strcmp(command, "activities") == 0 ||
           strcmp(command, "ping") == 0 ||
           strcmp(command, "fg") == 0 ||
           strcmp(command, "bg") == 0;
}

int execute_builtin(char** args) {
    // printf("DEBUG: Executing builtin: %s\n", args[0]);
    if (strcmp(args[0], "hop") == 0) return builtin_hop(args);
    if (strcmp(args[0], "reveal") == 0) return builtin_reveal(args);
    if (strcmp(args[0], "log") == 0) return builtin_log(args);
    if (strcmp(args[0], "activities") == 0) return builtin_activities(args);
    if (strcmp(args[0], "ping") == 0) return builtin_ping(args);
    if (strcmp(args[0], "fg") == 0) return builtin_fg(args);
    if (strcmp(args[0], "bg") == 0) return builtin_bg(args);
    return 1; 
}