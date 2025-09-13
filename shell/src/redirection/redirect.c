#include "../../include/redirection.h"

int handle_redirect(Command *cmd) {
    int input_fd = -1;
    int output_fd = -1;

    for (int i = 0; i < cmd->argc; i++) {
        if (strcmp(cmd->argv[i], "<") == 0) {
            if (i + 1 < cmd->argc) {
                input_fd = open(cmd->argv[i + 1], O_RDONLY);
                if (input_fd < 0) {
                    perror("open");
                    return -1;
                }
                dup2(input_fd, STDIN_FILENO);
                cmd->argv[i] = NULL;
                cmd->argc = i;
            }
        } else if (strcmp(cmd->argv[i], ">") == 0) {
            if (i + 1 < cmd->argc) {
                output_fd = open(cmd->argv[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (output_fd < 0) {
                    perror("open");
                    return -1;
                }
                dup2(output_fd, STDOUT_FILENO);
                cmd->argv[i] = NULL;
                cmd->argc = i;
            }
        } else if (strcmp(cmd->argv[i], ">>") == 0) {
            if (i + 1 < cmd->argc) {
                output_fd = open(cmd->argv[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
                if (output_fd < 0) {
                    perror("open");
                    return -1;
                }
                dup2(output_fd, STDOUT_FILENO);
                cmd->argv[i] = NULL;
                cmd->argc = i;
            }
        }
    }

    return 0;
}
