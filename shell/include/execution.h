#ifndef EXECUTION_H
#define EXECUTION_H

#include "shell.h"
#include "parser.h"

// Process status for background jobs
typedef enum {
    PROC_RUNNING,
    PROC_STOPPED,
    PROC_COMPLETED
} process_status_t;

// Background process structure
typedef struct process {
    pid_t pid;
    char command[MAX_COMMAND_LEN];
    int job_number;
    process_status_t status;
    struct process* next;
} process_t;

// Global process list
extern process_t* process_list;
extern int job_counter;

// Main execution functions
int execute_command(shell_cmd_t* cmd);
int execute_pipeline(cmd_group_t* group, int background);
int execute_single_command(command_t* cmd, int input_fd, int output_fd);

// I/O redirection functions
int setup_input_redirection(char* filename);
int setup_output_redirection(char* filename, int append);
void restore_io(int saved_stdin, int saved_stdout);

// Background process management
void add_background_process(pid_t pid, char* command);
void add_stopped_process(pid_t pid, char* command);
void check_background_processes(void);
process_t* find_process_by_pid(pid_t pid);
process_t* find_process_by_job(int job_number);
void remove_process(pid_t pid);
void print_job_status(process_t* proc, char* status_msg);

// Sequential execution
int execute_sequential(shell_cmd_t* cmd);

// External command execution
int execute_external_command(char** args, int input_fd, int output_fd);

#endif
