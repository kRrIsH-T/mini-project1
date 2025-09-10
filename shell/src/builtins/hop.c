#include "../../include/builtins.h"

void builtin_hop(Command *cmd) {
    char *path;

    if (cmd->argc < 2) {
        path = getenv("HOME");
    } else {
        path = cmd->argv[1];
    }

    if (chdir(path) != 0) {
        perror("hop");
    }
}
