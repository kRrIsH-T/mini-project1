#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <pwd.h>
#include <dirent.h>
#include <time.h>

// Constants
#define MAX_INPUT_SIZE 4096
#define MAX_TOKENS 64
#define MAX_PATH_LEN 1024
#define MAX_HISTORY 15
#define MAX_COMMAND_LEN 256
#define MAX_JOBS 64

// Global variables
extern char home_dir[MAX_PATH_LEN];
extern char prev_dir[MAX_PATH_LEN];
extern char current_dir[MAX_PATH_LEN];
extern char username[256];
extern char hostname[256];

// Shell state
extern int shell_running;
extern pid_t shell_pgid;

// Function declarations for main shell operations
void init_shell(void);
void display_prompt(void);
char* read_input(void);
void cleanup_shell(void);
void update_current_dir(void);

#endif
