#include "../../include/builtins.h"
#include "../../include/execution.h"
#include "../../include/signals.h"
#include <signal.h>
#include <sys/wait.h>

int builtin_fg(char** args) {
    int job_number;
    process_t *proc = NULL;
    
    if (args[1] == NULL) {
        // Find most recent job
        int max_job = 0;
        process_t *current = process_list;
        while (current) {
            if (current->job_number > max_job && 
                (current->status == PROC_STOPPED || current->status == PROC_RUNNING)) {
                max_job = current->job_number;
                proc = current;
            }
            current = current->next;
        }
        
        if (!proc) {
            printf("No jobs in background\n");
            return 1;
        }
    } else {
        job_number = atoi(args[1]);
        proc = find_process_by_job(job_number);
        
        if (!proc) {
            printf("No such job\n");
            return 1;
        }
    }
    
    // Print the command
    printf("%s\n", proc->command);
    
    // Set as foreground process
    foreground_pid = proc->pid;
    tcsetpgrp(STDIN_FILENO, proc->pid);
    
    // If stopped, send SIGCONT
    if (proc->status == PROC_STOPPED) {
        kill(proc->pid, SIGCONT);
    }
    
    proc->status = PROC_RUNNING;
    
    // Wait for process
    int status;
    waitpid(proc->pid, &status, WUNTRACED);
    
    if (WIFSTOPPED(status)) {
        // Process was stopped again
        proc->status = PROC_STOPPED;
        printf("[%d] Stopped %s\n", proc->job_number, proc->command);
    } else {
        // Process completed, remove from list
        remove_process(proc->pid);
    }
    
    // Restore shell to foreground
    foreground_pid = 0;
    tcsetpgrp(STDIN_FILENO, shell_pgid);
    
    return 0;
}

int builtin_bg(char** args) {
    int job_number;
    process_t *proc = NULL;
    
    if (args[1] == NULL) {
        printf("Usage: bg <job_number>\n");
        return 1;
    }
    
    job_number = atoi(args[1]);
    proc = find_process_by_job(job_number);
    
    if (!proc) {
        printf("No such job\n");
        return 1;
    }
    
    if (proc->status == PROC_RUNNING) {
        printf("Job already running\n");
        return 1;
    }
    
    if (proc->status == PROC_STOPPED) {
        // Resume the stopped job
        kill(proc->pid, SIGCONT);
        proc->status = PROC_RUNNING;
        printf("[%d] %s &\n", proc->job_number, proc->command);
        return 0;
    }
    
    return 1;
}