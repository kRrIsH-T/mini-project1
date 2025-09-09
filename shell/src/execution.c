#include "../include/shell.h"

void execute_command(Command *cmd) {
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
