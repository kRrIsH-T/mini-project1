#include "../include/parser.h"
#include "../include/utils.h"

// Tokenize input string according to CFG
token_t* tokenize(char* input, int* token_count) {
    token_t* tokens = safe_malloc(MAX_TOKENS * sizeof(token_t));
    *token_count = 0;
    
    int i = 0;
    int len = strlen(input);
    
    while (i < len && *token_count < MAX_TOKENS - 1) {
        // Skip whitespace
        while (i < len && (input[i] == ' ' || input[i] == '\t' || 
                          input[i] == '\n' || input[i] == '\r')) {
            i++;
        }
        
        if (i >= len) break;
        
        // Check for multi-character operators (only >> is supported)
        if (i < len - 1) {
            if (input[i] == '>' && input[i + 1] == '>') {
                tokens[*token_count].type = TOKEN_APPEND_REDIRECT;
                tokens[*token_count].value = safe_strdup(">>");
                (*token_count)++;
                i += 2;
                continue;
            }
        }
        
        // Check for single-character operators
        switch (input[i]) {
            case '|':
                tokens[*token_count].type = TOKEN_PIPE;
                tokens[*token_count].value = safe_strdup("|");
                (*token_count)++;
                i++;
                break;
            case '&':
                tokens[*token_count].type = TOKEN_AMPERSAND;
                tokens[*token_count].value = safe_strdup("&");
                (*token_count)++;
                i++;
                break;
            case '<':
                tokens[*token_count].type = TOKEN_INPUT_REDIRECT;
                tokens[*token_count].value = safe_strdup("<");
                (*token_count)++;
                i++;
                break;
            case '>':
                tokens[*token_count].type = TOKEN_OUTPUT_REDIRECT;
                tokens[*token_count].value = safe_strdup(">");
                (*token_count)++;
                i++;
                break;
            case ';':
                tokens[*token_count].type = TOKEN_SEMICOLON;
                tokens[*token_count].value = safe_strdup(";");
                (*token_count)++;
                i++;
                break;
            default:
                // Parse word token
                int start = i;
                while (i < len && input[i] != '|' && input[i] != '&' && 
                       input[i] != '<' && input[i] != '>' && input[i] != ';' &&
                       input[i] != ' ' && input[i] != '\t' && 
                       input[i] != '\n' && input[i] != '\r') {
                    i++;
                }
                
                if (i > start) {
                    tokens[*token_count].type = TOKEN_WORD;
                    int word_len = i - start;
                    tokens[*token_count].value = safe_malloc(word_len + 1);
                    strncpy(tokens[*token_count].value, input + start, word_len);
                    tokens[*token_count].value[word_len] = '\0';
                    (*token_count)++;
                }
                break;
        }
    }
    
    // Add EOF token
    tokens[*token_count].type = TOKEN_EOF;
    tokens[*token_count].value = NULL;
    (*token_count)++;
    
    return tokens;
}

// Validate syntax according to CFG
int validate_syntax(char* input) {
    int token_count;
    token_t* tokens = tokenize(input, &token_count);
    
    int result = is_valid_shell_cmd(tokens, token_count - 1); // Exclude EOF
    
    free_tokens(tokens, token_count);
    return result;
}

// Check if sequence matches shell_cmd grammar rule
int is_valid_shell_cmd(token_t* tokens, int count) {
    if (count == 0) return 1; // Empty command is valid
    
    int i = 0;
    
    // First cmd_group
    int group_end = i;
    while (group_end < count && 
           tokens[group_end].type != TOKEN_AMPERSAND &&
           tokens[group_end].type != TOKEN_SEMICOLON) {
        group_end++;
    }
    
    if (!is_valid_cmd_group(tokens, i, group_end - 1)) {
        return 0;
    }
    
    i = group_end;
    
    // Handle sequential/background operators
    while (i < count) {
        if (tokens[i].type == TOKEN_AMPERSAND ||
            tokens[i].type == TOKEN_SEMICOLON) {
            i++; // Skip operator
            
            // Find next group
            int next_group_start = i;
            int next_group_end = i;
            
         while (next_group_end < count && 
             tokens[next_group_end].type != TOKEN_AMPERSAND &&
             tokens[next_group_end].type != TOKEN_SEMICOLON) {
                next_group_end++;
            }
            
            if (next_group_start < count) {
                if (!is_valid_cmd_group(tokens, next_group_start, next_group_end - 1)) {
                    return 0;
                }
            }
            
            i = next_group_end;
        } else {
            return 0; // Unexpected token
        }
    }
    
    return 1;
}

// Check if sequence matches cmd_group grammar rule
int is_valid_cmd_group(token_t* tokens, int start, int end) {
    if (start > end) return 0;
    
    int i = start;
    
    // First atomic
    int atomic_end = i;
    while (atomic_end <= end && tokens[atomic_end].type != TOKEN_PIPE) {
        atomic_end++;
    }
    
    if (!is_valid_atomic(tokens, i, atomic_end - 1)) {
        return 0;
    }
    
    i = atomic_end;
    
    // Handle pipes
    while (i <= end) {
        if (tokens[i].type == TOKEN_PIPE) {
            i++; // Skip pipe
            
            int next_atomic_start = i;
            int next_atomic_end = i;
            
            while (next_atomic_end <= end && tokens[next_atomic_end].type != TOKEN_PIPE) {
                next_atomic_end++;
            }
            
            if (next_atomic_start <= end) {
                if (!is_valid_atomic(tokens, next_atomic_start, next_atomic_end - 1)) {
                    return 0;
                }
            } else {
                return 0; // Pipe with no following command
            }
            
            i = next_atomic_end;
        } else {
            return 0; // Unexpected token
        }
    }
    
    return 1;
}

// Check if sequence matches atomic grammar rule
int is_valid_atomic(token_t* tokens, int start, int end) {
    if (start > end) return 0;
    
    // Must start with a name (word)
    if (tokens[start].type != TOKEN_WORD) {
        return 0;
    }
    
    int i = start + 1;
    
    // Parse remaining tokens (names, input, output)
    while (i <= end) {
        if (tokens[i].type == TOKEN_WORD) {
            i++; // Regular argument
        } else if (tokens[i].type == TOKEN_INPUT_REDIRECT) {
            i++; // Skip <
            if (i <= end && tokens[i].type == TOKEN_WORD) {
                i++; // Skip filename
            } else {
                return 0; // < without filename
            }
        } else if (tokens[i].type == TOKEN_OUTPUT_REDIRECT || 
                   tokens[i].type == TOKEN_APPEND_REDIRECT) {
            i++; // Skip > or >>
            if (i <= end && tokens[i].type == TOKEN_WORD) {
                i++; // Skip filename
            } else {
                return 0; // > without filename
            }
        } else {
            return 0; // Invalid token in atomic
        }
    }
    
    return 1;
}

// Parse command into structured format
int parse_command(char* input, shell_cmd_t* cmd) {
    int token_count;
    token_t* tokens = tokenize(input, &token_count);
    
    // Initialize command structure
    memset(cmd, 0, sizeof(shell_cmd_t));
    
    if (token_count <= 1) { // Only EOF token
        free_tokens(tokens, token_count);
        return -1;
    }
    
    // Check for background (&) at the end
    cmd->background = 0;
    if (token_count >= 2 && tokens[token_count - 2].type == TOKEN_AMPERSAND) {
        cmd->background = 1;
        token_count--; // Remove & from processing
    }
    
    // Split by semicolons for sequential commands
    cmd->group_count = 1;
    for (int i = 0; i < token_count - 1; i++) {
        if (tokens[i].type == TOKEN_SEMICOLON) {
            cmd->group_count++;
        }
    }
    
    cmd->groups = safe_malloc(cmd->group_count * sizeof(cmd_group_t));
    
    // Parse each group (separated by semicolons)
    int group_idx = 0;
    int start = 0;
    
    for (int i = 0; i <= token_count - 1; i++) {
        if (i == token_count - 1 || tokens[i].type == TOKEN_SEMICOLON) {
            int end = (i == token_count - 1) ? i : i - 1;
            
            // Parse this group (handle pipes within group)
            if (parse_group(tokens, start, end, &cmd->groups[group_idx]) != 0) {
                free_tokens(tokens, token_count);
                return -1;
            }
            
            group_idx++;
            start = i + 1;
        }
    }
    
    free_tokens(tokens, token_count);
    return 0;
}

// Parse a command group (handles pipes and redirection)
int parse_group(token_t* tokens, int start, int end, cmd_group_t* group) {
    if (start > end) return -1;
    
    // Count pipes to determine number of commands
    group->command_count = 1;
    for (int i = start; i <= end; i++) {
        if (tokens[i].type == TOKEN_PIPE) {
            group->command_count++;
        }
    }
    
    group->commands = safe_malloc(group->command_count * sizeof(command_t));
    
    // Parse each command (separated by pipes)
    int cmd_idx = 0;
    int cmd_start = start;
    
    for (int i = start; i <= end + 1; i++) {
        if (i > end || tokens[i].type == TOKEN_PIPE) {
            int cmd_end = (i > end) ? end : i - 1;
            
            // Parse this single command
            if (parse_single_command(tokens, cmd_start, cmd_end, &group->commands[cmd_idx]) != 0) {
                return -1;
            }
            
            cmd_idx++;
            cmd_start = i + 1;
        }
    }
    
    return 0;
}

// Parse a single command (handles arguments and redirection)
int parse_single_command(token_t* tokens, int start, int end, command_t* cmd) {
    if (start > end) return -1;
    
    memset(cmd, 0, sizeof(command_t));
    
    // Count word tokens for args
    int word_count = 0;
    for (int i = start; i <= end; i++) {
        if (tokens[i].type == TOKEN_WORD) {
            word_count++;
        }
    }
    
    if (word_count == 0) return -1;
    
    // Allocate args array
    cmd->args = safe_malloc((word_count + 1) * sizeof(char*));
    cmd->argc = word_count;
    
    // Parse tokens
    int arg_idx = 0;
    for (int i = start; i <= end; i++) {
        switch (tokens[i].type) {
            case TOKEN_WORD:
                cmd->args[arg_idx] = safe_strdup(tokens[i].value);
                arg_idx++;
                break;
                
            case TOKEN_INPUT_REDIRECT:
                if (i + 1 <= end && tokens[i + 1].type == TOKEN_WORD) {
                    cmd->input_file = safe_strdup(tokens[i + 1].value);
                    i++; // Skip the filename
                }
                break;
                
            case TOKEN_OUTPUT_REDIRECT:
                if (i + 1 <= end && tokens[i + 1].type == TOKEN_WORD) {
                    cmd->output_file = safe_strdup(tokens[i + 1].value);
                    cmd->append_output = 0;
                    i++; // Skip the filename
                }
                break;
                
            case TOKEN_APPEND_REDIRECT:
                if (i + 1 <= end && tokens[i + 1].type == TOKEN_WORD) {
                    cmd->output_file = safe_strdup(tokens[i + 1].value);
                    cmd->append_output = 1;
                    i++; // Skip the filename
                }
                break;
                
            default:
                // Ignore other tokens
                break;
        }
    }
    
    cmd->args[word_count] = NULL; // Null terminate
    return 0;
}

// Free token array
void free_tokens(token_t* tokens, int count) {
    for (int i = 0; i < count; i++) {
        if (tokens[i].value) {
            free(tokens[i].value);
        }
    }
    free(tokens);
}

// Free shell command structure
void free_shell_cmd(shell_cmd_t* cmd) {
    for (int i = 0; i < cmd->group_count; i++) {
        for (int j = 0; j < cmd->groups[i].command_count; j++) {
            command_t* c = &cmd->groups[i].commands[j];
            if (c->args) {
                free_string_array(c->args, c->argc);
            }
            if (c->input_file) free(c->input_file);
            if (c->output_file) free(c->output_file);
        }
        if (cmd->groups[i].commands) {
            free(cmd->groups[i].commands);
        }
    }
    if (cmd->groups) {
        free(cmd->groups);
    }
}
