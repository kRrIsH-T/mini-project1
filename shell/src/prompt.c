#include "../include/shell.h"
#include "../include/utils.h"
#include "../include/builtins.h"
#include "../include/execution.h"

void init_shell(void)
{
    // Get current working directory as home directory
    if (getcwd(home_dir, sizeof(home_dir)) == NULL)
    {
        perror("getcwd");
        exit(1);
    }

    // Initialize previous directory as empty (no previous directory yet)
    prev_dir[0] = '\0';
    strcpy(current_dir, home_dir);

    // Get username and hostname
    get_username_hostname();

    // Set shell as process group leader
    shell_pgid = getpid();
    if (setpgid(shell_pgid, shell_pgid) < 0)
    {
        // In some environments (like WSL/testing), setpgid may not be allowed
        // This is not fatal for basic shell functionality
        perror("setpgid (non-fatal)");
        shell_pgid = getpgrp(); // Use current process group instead
    }

    // Load command history
    load_history();
}

void display_prompt(void)
{
    update_current_dir();

    // Replace home directory with ~ in path display
    char display_path[MAX_PATH_LEN];
    char *relative_path = get_relative_to_home(current_dir);

    if (relative_path)
    {
        strcpy(display_path, relative_path);
        free(relative_path);
    }
    else
    {
        strcpy(display_path, current_dir);
    }

    printf("<%s@%s:%s> ", username, hostname, display_path);
    fflush(stdout);
}

void update_current_dir(void)
{
    if (getcwd(current_dir, sizeof(current_dir)) == NULL)
    {
        perror("getcwd");
        strcpy(current_dir, home_dir); // Fallback to home
    }
}

void cleanup_shell(void)
{
    // Save command history
    save_history();

    // Kill all background processes
    process_t *current = process_list;
    while (current)
    {
        if (current->status == PROC_RUNNING)
        {
            kill(current->pid, SIGKILL);
        }
        current = current->next;
    }

    // Free process list
    while (process_list)
    {
        process_t *temp = process_list;
        process_list = process_list->next;
        free(temp);
    }
}

// For persistence, use load_history() and save_history() as in your prompt.c