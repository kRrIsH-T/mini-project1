#include "../../include/builtins.h"

int is_builtin(const char *cmd) {
    return strcmp(cmd, "hop") == 0 ||
           strcmp(cmd, "reveal") == 0 ||
           strcmp(cmd, "log") == 0 ||
           strcmp(cmd, "activities") == 0 ||
           strcmp(cmd, "ping") == 0 ||
           strcmp(cmd, "fg") == 0 ||
           strcmp(cmd, "bg") == 0;
}

int execute_builtin(Command *cmd) {
    if (strcmp(cmd->argv[0], "hop") == 0) {
        builtin_hop(cmd);
        return 1;
    } else if (strcmp(cmd->argv[0], "reveal") == 0) {
        builtin_reveal(cmd);
        return 1;
    } else if (strcmp(cmd->argv[0], "log") == 0) {
        builtin_log(cmd);
        return 1;
    } else if (strcmp(cmd->argv[0], "activities") == 0) {
        builtin_activities(cmd);
        return 1;
    } else if (strcmp(cmd->argv[0], "ping") == 0) {
        builtin_ping(cmd);
        return 1;
    } else if (strcmp(cmd->argv[0], "fg") == 0) {
        builtin_fg(cmd);
        return 1;
    } else if (strcmp(cmd->argv[0], "bg") == 0) {
        builtin_bg(cmd);
        return 1;
    }
    return 0;
}
