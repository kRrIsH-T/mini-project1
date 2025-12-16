#include "../../include/signals.h"
#include "../../include/execution.h"
#include "../../include/shell.h"
#include <unistd.h>

// Global variables for signal handling
pid_t foreground_pid = 0;
process_t* foreground_proc = NULL;

void setup_signal_handlers(void) {
    struct sigaction sa;
    
    // Setup SIGINT handler (Ctrl-C)
    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, NULL);
    
    // Setup SIGTSTP handler (Ctrl-Z)
    sa.sa_handler = sigtstp_handler;
    sigaction(SIGTSTP, &sa, NULL);
    
    // Setup SIGCHLD handler (child process termination)
    sa.sa_handler = sigchld_handler;
    sigaction(SIGCHLD, &sa, NULL);
    
    // Ignore SIGTTOU to allow background processes
    signal(SIGTTOU, SIG_IGN);
}

void sigint_handler(int sig) {
    (void)sig; // Suppress unused parameter warning
    
    if (foreground_pid > 0) {
        // Send SIGINT to foreground process group
        kill(-foreground_pid, SIGINT);
    }
    
    // Don't terminate the shell itself
    printf("\n");
}

void sigtstp_handler(int sig) {
    (void)sig; // Suppress unused parameter warning
    
    if (foreground_pid > 0) {
        // Send SIGTSTP to foreground process group
        kill(-foreground_pid, SIGTSTP);
        
        // The process will be added to background list by execute_pipeline
        foreground_pid = 0;
        foreground_proc = NULL;
        
        printf("\n");
    }
}

void sigchld_handler(int sig) {
    (void)sig; // Suppress unused parameter warning
    
    // This is handled by check_background_processes() in main loop
    // to avoid race conditions
}

void bring_to_foreground(process_t* proc) {
    if (!proc) return;
    
    foreground_pid = proc->pid;
    foreground_proc = proc;
    
    // Send SIGCONT if stopped
    if (proc->status == PROC_STOPPED) {
        kill(proc->pid, SIGCONT);
        proc->status = PROC_RUNNING;
    }
    
    printf("%s\n", proc->command);
    
    // Set as foreground process group
    tcsetpgrp(STDIN_FILENO, proc->pid);
    
    // Wait for process
    int status;
    waitpid(proc->pid, &status, WUNTRACED);
    
    // Restore shell to foreground
    foreground_pid = 0;
    foreground_proc = NULL;
    tcsetpgrp(STDIN_FILENO, shell_pgid);
}

void send_to_background(process_t* proc) {
    if (!proc) return;
    
    if (proc->status == PROC_STOPPED) {
        kill(proc->pid, SIGCONT);
        proc->status = PROC_RUNNING;
        printf("[%d] %s &\n", proc->job_number, proc->command);
    }
}

void stop_foreground_process(void) {
    if (foreground_pid > 0) {
        kill(-foreground_pid, SIGTSTP);
    }
}

void terminate_foreground_process(void) {
    if (foreground_pid > 0) {
        kill(-foreground_pid, SIGINT);
    }
}
