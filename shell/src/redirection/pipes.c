#include "../../include/redirection.h"

void parse_pipes(char *input, PipeCommand *pcmd) {
    pcmd->pipe_count = 0;
    pcmd->input_file = NULL;
    pcmd->output_file = NULL;
    pcmd->append_output = 0;

    char *dup = strdup(input);
    char *pipe_token = strtok(dup, "|");

    while (pipe_token != NULL && pcmd->pipe_count < ARGS_MAX) {
        parse_input(pipe_token, &pcmd->pipes[pcmd->pipe_count]);
        pcmd->pipe_count++;
        pipe_token = strtok(NULL, "|");
    }

    free(dup);
}

void execute_pipes(PipeCommand *pcmd) {
    if (pcmd->pipe_count == 1) {
        execute_command(&pcmd->pipes[0]);
        return;
    }

    int pipes[ARGS_MAX - 1][2];

    for (int i = 0; i < pcmd->pipe_count - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            return;
        }
    }

    for (int i = 0; i < pcmd->pipe_count; i++) {
        pid_t pid = fork();

        if (pid == 0) {
            if (i > 0) {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }
            if (i < pcmd->pipe_count - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            for (int j = 0; j < pcmd->pipe_count - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            execvp(pcmd->pipes[i].argv[0], pcmd->pipes[i].argv);
            perror("execvp");
            exit(1);
        }
    }

    for (int i = 0; i < pcmd->pipe_count - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    for (int i = 0; i < pcmd->pipe_count; i++) {
        wait(NULL);
    }
}
