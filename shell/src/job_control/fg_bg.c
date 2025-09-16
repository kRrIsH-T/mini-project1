#include "../../include/job_control.h"

extern Job jobs[];
extern int job_count;

void builtin_fg(Command *cmd) {
    if (cmd->argc < 2) {
        fprintf(stderr, "Usage: fg <job>\n");
        return;
    }

    int job_id = atoi(cmd->argv[1]);
    Job *job = find_job(job_id);

    if (job != NULL) {
        waitpid(job->pid, NULL, 0);
    } else {
        fprintf(stderr, "Job not found\n");
    }
}

void builtin_bg(Command *cmd) {
    if (cmd->argc < 2) {
        fprintf(stderr, "Usage: bg <job>\n");
        return;
    }

    int job_id = atoi(cmd->argv[1]);
    Job *job = find_job(job_id);

    if (job != NULL) {
        kill(job->pid, SIGCONT);
    } else {
        fprintf(stderr, "Job not found\n");
    }
}
