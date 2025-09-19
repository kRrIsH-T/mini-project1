#include "../../include/background.h"
#include "../../include/signals.h"

int check_background_flag(Command *cmd) {
    if (cmd->argc > 0 && strcmp(cmd->argv[cmd->argc - 1], "&") == 0) {
        cmd->argv[cmd->argc - 1] = NULL;
        cmd->argc--;
        return 1;
    }
    return 0;
}

void execute_background(Command *cmd) {
    pid_t pid = fork();

    if (pid == 0) {
        execvp(cmd->argv[0], cmd->argv);
        perror("execvp");
        exit(1);
    } else if (pid > 0) {
        char cmdstr[COMMAND_MAX];
        cmdstr[0] = '\0';
        for (int i = 0; i < cmd->argc; i++) {
            strcat(cmdstr, cmd->argv[i]);
            if (i < cmd->argc - 1) {
                strcat(cmdstr, " ");
            }
        }
        add_job(pid, cmdstr);
    } else {
        perror("fork");
    }
}
