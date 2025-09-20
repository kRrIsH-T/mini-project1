# Shell Features Documentation

## Built-in Commands

### Navigation
- **hop [path]** - Change directory. Defaults to home if no path given.

### File Operations
- **reveal [-a] [-l]** - List files with optional flags for all files and long format.

### History
- **log** - Display command history from `.shell_history` file.

## Job Control

### Process Management
- **activities** - List all active background processes with job numbers.
- **ping <signal> <job|pid>** - Send signal to process by job number or PID.
- **fg <job>** - Bring background job to foreground.
- **bg <job>** - Resume stopped job in background.

## Advanced Features

### Pipes
```bash
cat file.txt | grep pattern | sort
```

### Redirection
```bash
command < input.txt      # Input redirection
command > output.txt     # Output redirection (truncate)
command >> output.txt    # Output redirection (append)
```

### Background Execution
```bash
long_running_command &   # Run in background
```

### Signal Handling
- **Ctrl+C** (SIGINT) - Interrupt current process
- **Ctrl+Z** (SIGTSTP) - Stop current process
- **Ctrl+D** (EOF) - Exit shell

## Architecture

The shell is organized into modular components:

```
shell/
├── include/
│   ├── shell.h           # Core definitions
│   ├── builtins.h        # Built-in command interface
│   ├── job_control.h     # Job control interface
│   ├── signals.h         # Signal handling
│   ├── background.h      # Background execution
│   ├── redirection.h     # Pipes and redirection
│   └── parser.h          # Parsing utilities
├── src/
│   ├── main.c            # Main shell loop
│   ├── prompt.c          # Prompt generation
│   ├── input.c           # Input reading
│   ├── parser.c          # Command parsing
│   ├── execution.c       # Command execution
│   ├── utils.c           # Utility functions
│   ├── builtins/         # Built-in commands
│   ├── signals/          # Signal handlers
│   ├── job_control/      # Job control implementation
│   ├── background/       # Background execution
│   └── redirection/      # Pipe and redirect handlers
└── Makefile
```

## Compilation

```bash
gcc -std=c99 -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=700 \
    -Wall -Wextra -Werror -Wno-unused-parameter
```

All source files compile with strict POSIX compliance and no warnings.
