# OSN Mini Project

This project implements a comprehensive systems programming assignment with three components: a C shell, a custom networking protocol, and xv6 kernel modifications.

## Components

### Shell (C)
Located in `shell/`, a fully-featured interactive shell with:
- Custom prompt display
- Command parsing and execution
- Built-in commands: `hop`, `reveal`, `log`, `activities`, `ping`, `fg`, `bg`
- Pipes and I/O redirection
- Background job execution
- Signal handling

**Build:**
```bash
cd shell && make
```

### Networking (SHAM Protocol)
Located in `networking/`, a reliable UDP-based transport protocol featuring:
- Sequence numbers and acknowledgments
- Checksum validation (MD5)
- Server and client implementation
- Window-based flow control

**Build:**
```bash
cd networking && make
```

### xv6 OS Modifications
Located in `xv6_new/`, kernel enhancements including:
- `getreadcount()` system call
- FCFS scheduler (First Come First Serve)
- CFS scheduler (Completely Fair Scheduler)
- MLFQ scheduler (Multi-Level Feedback Queue)

## Building All Components

```bash
# Build shell
cd shell && make

# Build networking
cd networking && make

# Build xv6 (requires xv6 source)
cd xv6_new && make qemu SCHEDULER=FCFS
```

## Quick Start

### Run the Shell
```bash
cd shell
./shell.out
```

### Test Network Protocol
```bash
# Terminal 1
cd networking && ./server

# Terminal 2
cd networking && ./client 127.0.0.1
```

## Project Structure

```
.
├── shell/              # Shell implementation
│   ├── include/       # Header files
│   ├── src/          # Source files
│   └── Makefile
├── networking/         # Network protocol
│   ├── include/
│   ├── src/
│   └── Makefile
├── xv6_new/           # Kernel patches
│   ├── *.patch
│   └── README.md
├── .gitignore
└── README.md
```

## Standards

All code follows POSIX.1-2008 with compiler flags:
```bash
-std=c99 -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=700 -Wall -Wextra -Werror
```

## Documentation

- **RESUME.md** - Resume entries and project descriptions
- **Shell Architecture** - Modular design with separate compilation units for parsing, execution, builtins, signals, and job control
- **POSIX Compliance** - Strict adherence to POSIX.1-2008 standards

## Author

kRrIsH-T
