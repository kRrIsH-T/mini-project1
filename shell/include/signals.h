#ifndef SIGNALS_H
#define SIGNALS_H

#include "shell.h"

typedef struct {
    pid_t pid;
    char *command;
    int status;
} Job;

void setup_signals();
void signal_handler(int sig);
void add_job(pid_t pid, const char *cmd);
void remove_job(pid_t pid);
Job *find_job(int job_id);
void list_jobs();

#endif
