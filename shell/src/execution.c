#include "../include/shell.h"
#include "../include/builtins.h"

void execute_command(Command *cmd) {
    if (is_builtin(cmd->argv[0])) {
        execute_builtin(cmd);
        return;
    }

    pid_t pid = fork();

    if (pid == 0) {
        execvp(cmd->argv[0], cmd->argv);
        perror("execvp");
        exit(1);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
    } else {
        perror("fork");
    }
}
