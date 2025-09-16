#include "../../include/signals.h"

#define MAX_JOBS 256

Job jobs[MAX_JOBS];
int job_count = 0;

void setup_signals() {
    signal(SIGINT, signal_handler);
    signal(SIGTSTP, signal_handler);
    signal(SIGCHLD, signal_handler);
}

void signal_handler(int sig) {
    if (sig == SIGCHLD) {
        pid_t pid;
        int status;
        while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
            remove_job(pid);
        }
    }
}

void add_job(pid_t pid, const char *cmd) {
    if (job_count < MAX_JOBS) {
        jobs[job_count].pid = pid;
        jobs[job_count].command = malloc(strlen(cmd) + 1);
        strcpy(jobs[job_count].command, cmd);
        jobs[job_count].status = 1;
        job_count++;
    }
}

void remove_job(pid_t pid) {
    for (int i = 0; i < job_count; i++) {
        if (jobs[i].pid == pid) {
            free(jobs[i].command);
            for (int j = i; j < job_count - 1; j++) {
                jobs[j] = jobs[j + 1];
            }
            job_count--;
            break;
        }
    }
}

Job *find_job(int job_id) {
    if (job_id > 0 && job_id <= job_count) {
        return &jobs[job_id - 1];
    }
    return NULL;
}

void list_jobs() {
    for (int i = 0; i < job_count; i++) {
        printf("[%d] %d %s\n", i + 1, jobs[i].pid, jobs[i].command);
    }
}
