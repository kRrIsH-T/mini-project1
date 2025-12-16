#include "../../include/builtins.h"
#include "../../include/utils.h"
#include "../../include/parser.h"
#include "../../include/execution.h"

// Global history array
static history_entry_t history[MAX_HISTORY];
static int history_count = 0;
static int history_start = 0;

int builtin_log(char **args)
{
    if (args[1] == NULL)
    {
        // Print history
        print_history();
    }
    else if (strcmp(args[1], "purge") == 0)
    {
        // Clear history
        clear_history();
    }
    else if (strcmp(args[1], "execute") == 0)
    {
        // Execute command from history
        if (args[2] == NULL)
        {
            printf("log: execute requires an index\n");
            return 1;
        }

        int index = atoi(args[2]);
        if (index < 1 || index > history_count)
        {
            printf("log: invalid index\n");
            return 1;
        }

        return execute_from_history(index);
    }
    else
    {
        printf("log: invalid option '%s'\n", args[1]);
        return 1;
    }

    return 0;
}

void add_to_history(char *command)
{
    char *trimmed = trim_whitespace(command);
    // Do not store any shell_cmd if the command name of an atomic command is log itself.
    if (strncmp(trimmed, "log", 3) == 0 && (trimmed[3] == '\0' || trimmed[3] == ' '))
    {
        return;
    }

    // Don't add if identical to previous command
    if (history_count > 0)
    {
        int prev_index = (history_start + history_count - 1) % MAX_HISTORY;
        if (strcmp(history[prev_index].command, trimmed) == 0)
        {
            return;
        }
    }

    // Add command to history
    int index;
    if (history_count < MAX_HISTORY)
    {
        index = history_count;
        history_count++;
    }
    else
    {
        index = history_start;
        history_start = (history_start + 1) % MAX_HISTORY;
    }

    strncpy(history[index].command, trimmed, MAX_COMMAND_LEN - 1);
    history[index].command[MAX_COMMAND_LEN - 1] = '\0';
    history[index].timestamp = time(NULL);
}

void print_history(void)
{
    for (int i = 0; i < history_count; i++)
    {
        int index = (history_start + i) % MAX_HISTORY;
        printf("%s\n", history[index].command);
    }
}

void clear_history(void)
{
    history_count = 0;
    history_start = 0;
}

int execute_from_history(int index)
{
    // Index is 1-based, from newest to oldest
    int actual_index = (history_start + history_count - index) % MAX_HISTORY;

    if (index < 1 || index > history_count)
    {
        return 1;
    }

    char *command = history[actual_index].command;
    printf("%s\n", command);

    // Parse and execute the command
    shell_cmd_t cmd;
    if (validate_syntax(command))
    {
        if (parse_command(command, &cmd) == 0)
        {
            int result = execute_command(&cmd);
            free_shell_cmd(&cmd);
            return result;
        }
    }
    else
    {
        printf("Invalid Syntax!\n");
        return 1;
    }

    return 0;
}

void load_history(void)
{
    char history_file[MAX_PATH_LEN];
    const char *suffix = "/.shell_history";
    size_t home_len = strlen(home_dir);
    size_t suffix_len = strlen(suffix);

    // Check if the combined path will fit
    if (home_len + suffix_len >= MAX_PATH_LEN)
    {
        fprintf(stderr, "Error: home_dir path too long for history file\n");
        return;
    }

    // Construct path safely
    if (snprintf(history_file, MAX_PATH_LEN, "%s%s", home_dir, suffix) >= MAX_PATH_LEN)
    {
        fprintf(stderr, "Error: home_dir path too long for history file\n");
        return;
    }

    FILE *file = fopen(history_file, "r");
    if (!file)
    {
        return; // No history file exists yet
    }

    char line[MAX_COMMAND_LEN];
    while (fgets(line, sizeof(line), file) && history_count < MAX_HISTORY)
    {
        // Remove newline
        line[strcspn(line, "\n")] = '\0';

        if (strlen(line) > 0)
        {
            add_to_history(line);
        }
    }

    fclose(file);
}

void save_history(void)
{
    char history_file[MAX_PATH_LEN];
    const char *suffix = "/.shell_history";
    size_t home_len = strlen(home_dir);
    size_t suffix_len = strlen(suffix);

    // Check if the combined path will fit
    if (home_len + suffix_len >= MAX_PATH_LEN)
    {
        fprintf(stderr, "Error: home_dir path too long for history file\n");
        return;
    }

    // Construct path safely
    if (snprintf(history_file, MAX_PATH_LEN, "%s%s", home_dir, suffix) >= MAX_PATH_LEN)
    {
        fprintf(stderr, "Error: home_dir path too long for history file\n");
        return;
    }

    FILE *file = fopen(history_file, "w");
    if (!file)
    {
        return;
    }

    for (int i = 0; i < history_count; i++)
    {
        int index = (history_start + i) % MAX_HISTORY;
        fprintf(file, "%s\n", history[index].command);
    }

    fclose(file);
}