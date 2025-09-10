#include "../../include/builtins.h"

void builtin_log(Command *cmd) {
    FILE *logfile;
    char line[COMMAND_MAX];

    logfile = fopen(".shell_history", "r");
    if (logfile == NULL) {
        return;
    }

    while (fgets(line, sizeof(line), logfile) != NULL) {
        printf("%s", line);
    }

    fclose(logfile);
}
