#include "../include/shell.h"
#include "../include/parser.h"
#include "../include/execution.h"
#include "../include/signals.h"
#include "../include/builtins.h"
#include "../include/utils.h"
#include <stdint.h>

// Global variables
char home_dir[MAX_PATH_LEN];
char prev_dir[MAX_PATH_LEN];
char current_dir[MAX_PATH_LEN];
char username[256];
char hostname[256];
int shell_running = 1;
uint64_t total_bytes_read = 0;
pid_t shell_pgid;

int main(void)
{
    printf("Initializing OSN Shell...\n");

    // Initialize shell
    init_shell();

    // Setup signal handlers
    setup_signal_handlers();

    char *input;
    shell_cmd_t cmd;

    // Main shell loop
    while (shell_running)
    {
        // Check for completed background processes
        check_background_processes();

        // Display prompt and read input
        display_prompt();
        input = read_input();

        if (!input)
        {
            // EOF (Ctrl-D) received
            printf("logout\n");
            
            // Kill all background processes
            process_t *current = process_list;
            while (current) {
                kill(current->pid, SIGKILL);
                current = current->next;
            }
            
            exit(0);
        }

        // Skip empty input
        if (strlen(trim_whitespace(input)) == 0)
        {
            free(input);
            continue;
        }

        // Add to history (if not a log command)
        if (strncmp(trim_whitespace(input), "log", 3) != 0)
        {
            add_to_history(input);
        }

        // Parse input
        if (validate_syntax(input))
        {
            if (parse_command(input, &cmd) == 0)
            {
                // Execute the command (handles all cases now)
                execute_command(&cmd);
                free_shell_cmd(&cmd);
            }
            else
            {
                printf("Invalid Syntax!\n");
            }
        }
        else
        {
            printf("Invalid Syntax!\n");
        }

        free(input);
    }

    cleanup_shell();
    return 0;
}
