#ifndef BUILTINS_H
#define BUILTINS_H

#include "shell.h"

// History entry structure for log command
typedef struct {
    char command[MAX_COMMAND_LEN];
    time_t timestamp;
} history_entry_t;
void load_history(void);
void save_history(void);

// Builtin command function declarations
int builtin_hop(char** args);
int builtin_reveal(char** args);
int builtin_log(char** args);
int builtin_activities(char** args);
int builtin_ping(char** args);
int builtin_fg(char** args);
int builtin_bg(char** args);

// Utility functions
int is_builtin(char* command);
int execute_builtin(char** args);

// History management
void add_to_history(char* command);
void print_history(void);
void clear_history(void);
int execute_from_history(int index);
void load_history(void);
void save_history(void);

// Path utilities for hop and reveal
char* resolve_path(char* path);
int change_directory(char* path);

#endif
