#ifndef UTILS_H
#define UTILS_H

#include "shell.h"

// Path utilities for builtins
char* resolve_path(char* path);
int change_directory(char* path);

// String utilities
char* trim_whitespace(char* str);
char** split_string(char* str, char delimiter, int* count);
void free_string_array(char** arr, int count);
int string_compare(const void* a, const void* b);

// Path utilities
char* get_absolute_path(char* path);
char* get_relative_to_home(char* path);
int path_exists(char* path);
int is_directory(char* path);

// Process utilities
int get_process_count(void);
char* get_process_name(pid_t pid);
int is_process_running(pid_t pid);
void send_signal_to_process(pid_t pid, int signal);

// File utilities
int file_exists(char* filename);
long get_file_size(char* filename);
int copy_file(char* src, char* dest);

// System information
void get_username_hostname(void);
char* get_current_time_string(void);

// Memory utilities
void* safe_malloc(size_t size);
char* safe_strdup(char* str);

#endif
