#include "../../include/builtins.h"
#include "../../include/execution.h"
#include "../../include/utils.h"

// Structure for sorting processes
typedef struct {
    char command[MAX_COMMAND_LEN];
    pid_t pid;
    process_status_t status;
} process_info_t;

int compare_processes(const void *a, const void *b) {
    process_info_t *proc_a = (process_info_t *)a;
    process_info_t *proc_b = (process_info_t *)b;
    return strcmp(proc_a->command, proc_b->command);
}

int builtin_activities(char** args) {
    (void)args; // Unused parameter
    
    // Count active processes
    int count = 0;
    process_t *current = process_list;
    while (current) {
        if (current->status == PROC_RUNNING || current->status == PROC_STOPPED) {
            count++;
        }
        current = current->next;
    }
    
    if (count == 0) {
        return 0; // No active processes
    }
    
    // Create array for sorting
    process_info_t *processes = safe_malloc(count * sizeof(process_info_t));
    int i = 0;
    
    current = process_list;
    while (current && i < count) {
        if (current->status == PROC_RUNNING || current->status == PROC_STOPPED) {
            strcpy(processes[i].command, current->command);
            processes[i].pid = current->pid;
            processes[i].status = current->status;
            i++;
        }
        current = current->next;
    }
    
    // Sort lexicographically by command name
    qsort(processes, count, sizeof(process_info_t), compare_processes);
    
    // Print sorted processes
    for (i = 0; i < count; i++) {
        const char *state = (processes[i].status == PROC_RUNNING) ? "Running" : "Stopped";
        printf("[%d] : %s - %s\n", processes[i].pid, processes[i].command, state);
    }
    
    free(processes);
    return 0;
}