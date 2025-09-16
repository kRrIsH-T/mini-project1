#include "../../include/job_control.h"

extern Job jobs[];
extern int job_count;

void builtin_ping(Command *cmd) {
    if (cmd->argc < 3) {
        fprintf(stderr, "Usage: ping <signal> <pid|job>\n");
        return;
    }

    int signal = atoi(cmd->argv[1]);
    int target = atoi(cmd->argv[2]);

    if (target > 0 && target <= job_count) {
        kill(jobs[target - 1].pid, signal);
    } else {
        kill(target, signal);
    }
}
