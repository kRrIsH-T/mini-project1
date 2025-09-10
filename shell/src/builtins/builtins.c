#include "../../include/builtins.h"

int is_builtin(const char *cmd) {
    return strcmp(cmd, "hop") == 0 ||
           strcmp(cmd, "reveal") == 0 ||
           strcmp(cmd, "log") == 0;
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
    }
    return 0;
}
