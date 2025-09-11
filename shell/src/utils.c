#include "../include/shell.h"

void log_command(const char *cmd) {
    FILE *logfile = fopen(".shell_history", "a");
    if (logfile != NULL) {
        fprintf(logfile, "%s\n", cmd);
        fclose(logfile);
    }
}
