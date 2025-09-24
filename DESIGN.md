# Shell Design Document

## Overview

This shell implements a fully-featured interactive command interpreter with support for pipes, redirection, job control, and signal handling. The design prioritizes modularity and POSIX compliance.

## Design Decisions

### Modular Architecture
The codebase is split into focused modules to maintain clarity and testability:
- **Parsing**: Tokenization and command structure validation
- **Execution**: Process creation and management via fork/execvp
- **Built-ins**: Shell-internal commands
- **Job Control**: Background process tracking and signaling
- **Signals**: Interrupt and suspension handling
- **Redirection**: Pipe and file I/O management

### Process Model
- Uses `fork()` for process creation
- Uses `execvp()` for command execution
- Uses `waitpid()` for synchronous process management
- Implements non-blocking job reaping via SIGCHLD

### Data Structures
```c
typedef struct {
    int argc;
    char *argv[ARGS_MAX];
} Command;

typedef struct {
    pid_t pid;
    char *command;
    int status;
} Job;

typedef struct {
    char *input_file;
    char *output_file;
    int append_output;
    int pipe_count;
    Command pipes[ARGS_MAX];
} PipeCommand;
```

## Key Implementation Details

### Pipe Implementation
- Creates intermediate pipes between all commands
- Duplicates file descriptors to connect stdin/stdout
- Parent process manages all child process cleanup
- Uses blocking `wait()` for synchronization

### Redirection
- Supports `<` for stdin, `>` for truncating output, `>>` for append
- Uses `open()` with appropriate flags and modes
- Closes file descriptors after `dup2()`

### Job Control
- SIGCHLD handler performs non-blocking reaping
- Job table tracks background processes
- `activities` command displays all running jobs
- `fg`/`bg` commands manipulate job state

### Signal Handling
- SIGINT (Ctrl+C) interrupts foreground process only
- SIGTSTP (Ctrl+Z) suspends foreground process
- SIGCHLD automatically reaps zombie processes

## Memory Management
- Allocates memory for parsed arguments
- Frees input buffers after processing
- Manages job history in memory
- Command history persisted to `.shell_history` file

## Performance Considerations
- O(n) job lookup for small n (typical shell usage)
- Single-threaded event loop
- Efficient pipe implementation with minimal syscalls

## Future Enhancements
- Implement history search (Ctrl+R)
- Add tab completion
- Support environment variable expansion
- Implement command aliases
- Add configurable prompt format
