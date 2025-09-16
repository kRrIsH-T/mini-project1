#include "../../include/job_control.h"

extern Job jobs[];
extern int job_count;

void builtin_activities(Command *cmd) {
    list_jobs();
}
