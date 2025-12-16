#include "../../include/execution.h"
#include "../../include/utils.h"
#include "../../include/builtins.h"
#include "../../include/signals.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

// Global process list and job counter
process_t *process_list = NULL;
int job_counter = 0;

int execute_command(shell_cmd_t *cmd)
{
    if (!cmd || cmd->group_count == 0)
        return 1;

    // Part D: Handle sequential execution
    if (cmd->group_count > 1) {
        return execute_sequential(cmd);
    }
    
    // Single command group
    cmd_group_t *first_group = &cmd->groups[0];
    return execute_pipeline(first_group, cmd->background);
}

int execute_sequential(shell_cmd_t *cmd)
{
    int last_exit_status = 0;
    
    for (int i = 0; i < cmd->group_count; i++) {
        cmd_group_t *group = &cmd->groups[i];
        
        // Check if this specific group should run in background
        int group_background = (i == cmd->group_count - 1) ? cmd->background : 0;
        
        last_exit_status = execute_pipeline(group, group_background);
        
        // Continue to next command even if current one fails
    }
    
    return last_exit_status;
}

int execute_pipeline(cmd_group_t *group, int background)
{
    if (!group || group->command_count == 0)
        return 1;

    // Special case: single built-in command should run in shell process
    if (group->command_count == 1 && is_builtin(group->commands[0].args[0])) {
        return execute_single_command(&group->commands[0], -1, -1);
    }

    int num_commands = group->command_count;
    int pipes[num_commands - 1][2];
    pid_t pids[num_commands];
    pid_t pgid = 0; // Process group ID for the pipeline

    // Create pipes
    for (int i = 0; i < num_commands - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            return 1;
        }
    }

    // Fork and execute each command
    for (int i = 0; i < num_commands; i++) {
        command_t *cmd = &group->commands[i];
        pids[i] = fork();
        if (pids[i] < 0) {
            perror("fork");
            return 1;
        } else if (pids[i] == 0) {
            // Child process
            
            // Set process group (first process becomes group leader)
            if (i == 0) {
                setpgid(0, 0);
                pgid = getpid();
            } else {
                setpgid(0, pgid);
            }
            
            // Background processes shouldn't access terminal
            if (background) {
                int null_fd = open("/dev/null", O_RDONLY);
                if (null_fd != -1) {
                    dup2(null_fd, STDIN_FILENO);
                    close(null_fd);
                }
            }
            
            // Set up input pipe (except for first command)
            if (i > 0) {
                dup2(pipes[i-1][0], STDIN_FILENO);
            }
            
            // Set up output pipe (except for last command)
            if (i < num_commands - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }
            
            // Close all pipes
            for (int j = 0; j < num_commands - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            
            // Execute the command
            execute_single_command(cmd, -1, -1);
            exit(1);
        } else {
            // Parent: Set process group for child
            if (i == 0) {
                pgid = pids[i];
                setpgid(pids[i], pgid);
            } else {
                setpgid(pids[i], pgid);
            }
        }
    }

    // Parent: Close all pipes
    for (int i = 0; i < num_commands - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    if (background) {
        // Add to background processes and return immediately
        for (int i = 0; i < num_commands; i++) {
            add_background_process(pids[i], group->commands[i].args[0]);
        }
        return 0;
    } else {
        // Foreground: Set as foreground process group and wait
        foreground_pid = pgid;
        tcsetpgrp(STDIN_FILENO, pgid);
        
        // Wait for all children
        int last_status = 0;
        for (int i = 0; i < num_commands; i++) {
            int status;
            waitpid(pids[i], &status, WUNTRACED);
            if (WIFSTOPPED(status)) {
                // Process was stopped (Ctrl-Z)
                add_stopped_process(pids[i], group->commands[i].args[0]);
                printf("[%d] Stopped %s\n", job_counter, group->commands[i].args[0]);
            }
            last_status = status;
        }
        
        // Restore shell to foreground
        foreground_pid = 0;
        tcsetpgrp(STDIN_FILENO, shell_pgid);
        
        return WIFEXITED(last_status) ? WEXITSTATUS(last_status) : 1;
    }
}

int execute_single_command(command_t *cmd, int input_fd, int output_fd)
{
    if (!cmd || !cmd->args || cmd->argc == 0)
        return 1;

    // Check if it's a built-in
    if (is_builtin(cmd->args[0])) {
        // Save original file descriptors for restoration
        int saved_stdin = -1, saved_stdout = -1;
        
        // Handle input redirection for builtins
        if (cmd->input_file) {
            int fd = open(cmd->input_file, O_RDONLY);
            if (fd == -1) {
                printf("No such file or directory\n");
                return 1;
            }
            saved_stdin = dup(STDIN_FILENO);
            dup2(fd, STDIN_FILENO);
            close(fd);
        } else if (input_fd != -1) {
            saved_stdin = dup(STDIN_FILENO);
            dup2(input_fd, STDIN_FILENO);
        }
        
        // Handle output redirection for builtins
        if (cmd->output_file) {
            int flags = cmd->append_output ? O_WRONLY | O_CREAT | O_APPEND : O_WRONLY | O_CREAT | O_TRUNC;
            int fd = open(cmd->output_file, flags, 0644);
            if (fd == -1) {
                printf("Unable to create file for writing\n");
                if (saved_stdin != -1) {
                    dup2(saved_stdin, STDIN_FILENO);
                    close(saved_stdin);
                }
                return 1;
            }
            saved_stdout = dup(STDOUT_FILENO);
            dup2(fd, STDOUT_FILENO);
            close(fd);
        } else if (output_fd != -1) {
            saved_stdout = dup(STDOUT_FILENO);
            dup2(output_fd, STDOUT_FILENO);
        }
        
        // Execute the builtin
        int result = execute_builtin(cmd->args);
        
        // Restore original file descriptors
        if (saved_stdin != -1) {
            dup2(saved_stdin, STDIN_FILENO);
            close(saved_stdin);
        }
        if (saved_stdout != -1) {
            dup2(saved_stdout, STDOUT_FILENO);
            close(saved_stdout);
        }
        
        return result;
    }

    // Set up I/O redirection in current process (since we're already forked)
    // Input redirection
    if (cmd->input_file) {
        int fd = open(cmd->input_file, O_RDONLY);
        if (fd == -1) {
            printf("No such file or directory\n");
            exit(1);
        }
        dup2(fd, STDIN_FILENO);
        close(fd);
    } else if (input_fd != -1) {
        dup2(input_fd, STDIN_FILENO);
    }

    // Output redirection
    if (cmd->output_file) {
        int flags = cmd->append_output ? O_WRONLY | O_CREAT | O_APPEND : O_WRONLY | O_CREAT | O_TRUNC;
        int fd = open(cmd->output_file, flags, 0644);
        if (fd == -1) {
            perror("open output file");
            exit(1);
        }
        dup2(fd, STDOUT_FILENO);
        close(fd);
    } else if (output_fd != -1) {
        dup2(output_fd, STDOUT_FILENO);
    }

    // Execute the command
    execvp(cmd->args[0], cmd->args);
    printf("Command not found!\n");
    exit(1);
}

// Background and stopped process management
void add_background_process(pid_t pid, char *command)
{
    process_t *proc = safe_malloc(sizeof(process_t));
    proc->pid = pid;
    strncpy(proc->command, command, MAX_COMMAND_LEN - 1);
    proc->command[MAX_COMMAND_LEN - 1] = '\0';
    proc->job_number = ++job_counter;
    proc->status = PROC_RUNNING;
    proc->next = process_list;
    process_list = proc;
    
    printf("[%d] %d\n", proc->job_number, pid);
}

void add_stopped_process(pid_t pid, char *command)
{
    process_t *proc = safe_malloc(sizeof(process_t));
    proc->pid = pid;
    strncpy(proc->command, command, MAX_COMMAND_LEN - 1);
    proc->command[MAX_COMMAND_LEN - 1] = '\0';
    proc->job_number = ++job_counter;
    proc->status = PROC_STOPPED;
    proc->next = process_list;
    process_list = proc;
}

void check_background_processes(void)
{
    process_t *current = process_list;
    process_t *prev = NULL;
    
    while (current) {
        int status;
        pid_t result = waitpid(current->pid, &status, WNOHANG);
        
        if (result > 0) {
            // Process has terminated
            if (WIFEXITED(status)) {
                printf("%s with pid %d exited normally\n", current->command, current->pid);
            } else {
                printf("%s with pid %d exited abnormally\n", current->command, current->pid);
            }
            
            // Remove from list
            if (prev) {
                prev->next = current->next;
            } else {
                process_list = current->next;
            }
            
            process_t *to_remove = current;
            current = current->next;
            free(to_remove);
        } else {
            prev = current;
            current = current->next;
        }
    }
}

// Rest of the functions remain the same...
process_t *find_process_by_pid(pid_t pid)
{
    process_t *current = process_list;
    while (current)
    {
        if (current->pid == pid)
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

process_t *find_process_by_job(int job_number)
{
    process_t *current = process_list;
    while (current)
    {
        if (current->job_number == job_number)
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void remove_process(pid_t pid)
{
    process_t **current = &process_list;
    while (*current)
    {
        if ((*current)->pid == pid)
        {
            process_t *to_remove = *current;
            *current = (*current)->next;
            free(to_remove);
            return;
        }
        current = &((*current)->next);
    }
}

void print_job_status(process_t *proc, char *status_msg)
{
    printf("[%d] %s %s\n", proc->job_number, status_msg, proc->command);
}

// I/O redirection functions
int setup_input_redirection(char *filename)
{
    int fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        printf("No such file or directory\n");
        return -1;
    }
    
    dup2(fd, STDIN_FILENO);
    close(fd);
    return 0;
}

int setup_output_redirection(char *filename, int append)
{
    int flags = O_WRONLY | O_CREAT;
    if (append)
        flags |= O_APPEND;
    else
        flags |= O_TRUNC;
    
    int fd = open(filename, flags, 0644);
    if (fd == -1)
    {
        perror("open");
        return -1;
    }
    
    dup2(fd, STDOUT_FILENO);
    close(fd);
    return 0;
}

void restore_io(int saved_stdin, int saved_stdout)
{
    if (saved_stdin != -1)
    {
        dup2(saved_stdin, STDIN_FILENO);
        close(saved_stdin);
    }
    if (saved_stdout != -1)
    {
        dup2(saved_stdout, STDOUT_FILENO);
        close(saved_stdout);
    }
}

int execute_external_command(char **args, int input_fd, int output_fd)
{
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        return 1;
    }
    else if (pid == 0)
    {
        // Child process
        
        // Set up input redirection if specified
        if (input_fd != -1)
        {
            dup2(input_fd, STDIN_FILENO);
            close(input_fd);
        }
        
        // Set up output redirection if specified
        if (output_fd != -1)
        {
            dup2(output_fd, STDOUT_FILENO);
            close(output_fd);
        }
        
        // Execute command
        execvp(args[0], args);
        printf("Command not found!\n");
        exit(1);
    }
    else
    {
        // Parent: wait for child
        int status;
        waitpid(pid, &status, 0);
        return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
    }
}
