#ifndef REDIRECTION_H
#define REDIRECTION_H

#include "shell.h"

typedef struct {
    char *input_file;
    char *output_file;
    int append_output;
    int pipe_count;
    Command pipes[ARGS_MAX];
} PipeCommand;

void parse_pipes(char *input, PipeCommand *pcmd);
void execute_pipes(PipeCommand *pcmd);

#endif
