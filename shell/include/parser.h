#ifndef PARSER_H
#define PARSER_H

#include "shell.h"

// Token types for the CFG
typedef enum {
    TOKEN_WORD,
    TOKEN_PIPE,               // |
    TOKEN_AMPERSAND,          // &
    TOKEN_INPUT_REDIRECT,     // <
    TOKEN_OUTPUT_REDIRECT,    // >
    TOKEN_APPEND_REDIRECT,    // >>
    TOKEN_SEMICOLON,          // ;
    TOKEN_EOF,
    TOKEN_INVALID
} token_type_t;

// Token structure
typedef struct {
    token_type_t type;
    char* value;
} token_t;

// Single command structure
typedef struct {
    char** args;              // Command arguments
    int argc;                 // Argument count
    char* input_file;         // Input redirection file
    char* output_file;        // Output redirection file
    int append_output;        // 1 if >>, 0 if >
} command_t;

// Command group (pipeline)
typedef struct {
    command_t* commands;      // Array of commands in pipeline
    int command_count;        // Number of commands in pipeline
} cmd_group_t;

// Complete shell command
typedef struct {
    cmd_group_t* groups;      // Array of command groups
    int group_count;          // Number of command groups
    int background;           // 1 if ends with &
    int sequential;           // 1 if contains ;
} shell_cmd_t;

// CFG parser functions
int parse_command(char* input, shell_cmd_t* cmd);
int parse_group(token_t* tokens, int start, int end, cmd_group_t* group);
int parse_single_command(token_t* tokens, int start, int end, command_t* cmd);
int validate_syntax(char* input);
void free_shell_cmd(shell_cmd_t* cmd);

// Tokenizer functions
token_t* tokenize(char* input, int* token_count);
void free_tokens(token_t* tokens, int count);

// Grammar validation functions
int is_valid_shell_cmd(token_t* tokens, int count);
int is_valid_cmd_group(token_t* tokens, int start, int end);
int is_valid_atomic(token_t* tokens, int start, int end);

#endif
