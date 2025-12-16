# OSN Mini Project 1

This project implements a comprehensive Operating Systems and Networks assignment consisting of three major components: a C shell with advanced features, a custom reliable networking protocol, and xv6 operating system kernel modifications. The project demonstrates systems programming concepts including process management, file I/O, networking protocols, and kernel-level scheduling.

**Assignment Deadline**: September 11, 2025 23:59:59 IST  
**Total Marks**: 865 (Shell: 645, Networking: 80, xv6: 140)


## Project Structure

```
.
├── networking/
│   ├── client
│   ├── client_log.txt
│   ├── file.txt
│   ├── hash.txt
│   ├── include/
│   ├── Makefile
│   ├── obj/
│   ├── output.txt
│   ├── received_file
│   ├── server
│   ├── server_log.txt
│   ├── src/
│   ├── test_logs/
│   └── verify.sh
├── shell/
│   ├── .test/
│   ├── include/
│   ├── Makefile
│   ├── obj/
│   ├── src/
│   │   ├── builtins/
│   │   ├── execution/
│   │   ├── input.c
│   │   ├── main.c
│   │   ├── parser.c
│   │   ├── prompt.c
│   │   ├── signals/
│   │   └── utils/
│   ├── tests/
├── xv6_new/
│   ├── report.md
│   └── xv6_modifications.patch
├── README.md
├── .gitignore
```

## Quick Start

### Prerequisites
- GCC with POSIX support
- Make build system  
- OpenSSL development libraries (for networking MD5)
- QEMU (for xv6 testing)
- Expect/TCL (for shell testing)

### Build All Components
```bash
# From project root
make all              # Builds shell and networking components
make setup           # Makes scripts executable
```

### Individual Component Builds
```bash
# Shell (creates shell.out in shell/ directory)
cd shell && make all

# Networking (creates client and server binaries)  
cd networking && make

# xv6 (applies patches and builds kernel)
cd xv6 && make qemu SCHEDULER=FCFS    # Options: FCFS, CFS, MLFQ
```

## Component Details

### C Shell (645 marks)

A fully-featured shell implementing a Context-Free Grammar parser with comprehensive built-in commands and job control.

**Part A: Shell Input (65 marks)**
- Custom prompt: `<Username@SystemName:current_path>`
- Home directory handling with `~` expansion
- CFG-based input parsing and syntax validation

**Part B: Shell Intrinsics (70 marks)**
- `hop` - Directory navigation with relative/absolute paths
- `reveal` - Enhanced file listing with `-a` and `-l` flags  
- `log` - Command history with purge and execute functionality

**Part C: File Redirection and Pipes (200 marks)**
- Input redirection: `command < file`
- Output redirection: `command > file`, `command >> file`
- Pipeline support: `cmd1 | cmd2 | cmd3`
- Proper file descriptor management

**Part D: Sequential and Background Execution (200 marks)**  
- Sequential commands: `cmd1 ; cmd2 ; cmd3`
- Background processes: `command &`
- Process management and status tracking

**Part E: Exotic Shell Intrinsics (110 marks)**
- `activities` - List all active background processes
- `ping` - Send signals to processes by PID or job number
- `fg`/`bg` - Foreground/background job control
- Signal handling (Ctrl+C, Ctrl+Z, Ctrl+D)

**Compilation Requirements:**
```bash
gcc -std=c99 -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=700 \
    -Wall -Wextra -Werror -Wno-unused-parameter -fno-asm
```

### Networking - S.H.A.M. Protocol (80 marks)

A custom reliable transport protocol implemented over UDP, featuring TCP-like reliability mechanisms.

**Key Features:**
- **Reliable Transfer**: Sequence numbers, acknowledgments, retransmission
- **Flow Control**: Sliding window protocol implementation  
- **Dual Modes**: File transfer and real-time chat functionality
- **Error Handling**: Checksum validation, packet loss simulation
- **Performance**: Configurable timeouts and window sizes

**Protocol Details:**
- Custom packet format with headers for sequence/ack numbers
- MD5 checksums for data integrity
- Adaptive retransmission timers
- Window-based congestion control

### xv6 Operating System (140 marks)

Kernel modifications to the xv6 educational operating system, implementing new system calls and scheduling algorithms.

**System Call Implementation:**
- `getreadcount()` - Tracks total bytes read by all processes
- Kernel integration with proper syscall number assignment
- User-space library integration

**Scheduler Implementations:**
- **FCFS (First Come First Serve)**: Non-preemptive scheduling
- **CFS (Completely Fair Scheduler)**: Virtual runtime-based fair scheduling  
- **MLFQ (Multi-Level Feedback Queue)**: Priority-based preemptive scheduling with aging

**Build Options:**
```bash
make qemu SCHEDULER=FCFS     # First Come First Serve
make qemu SCHEDULER=CFS      # Completely Fair Scheduler  
make qemu SCHEDULER=MLFQ     # Multi-Level Feedback Queue (bonus)
```

## Testing Framework

### Shell Testing
```bash
cd shell
chmod +x shell[1].tcl
./shell[1].tcl           # Run full test suite
VERBOSE=1 ./shell[1].tcl # Verbose output
```

### Networking Testing  
```bash
cd networking
./test_networking.sh    # Automated protocol testing
./verify.sh             # Verification scripts
```

### xv6 Testing
```bash  
cd xv6
python3 test-xv6.py     # Scheduler performance analysis
```

## Compliance and Standards

**POSIX Compliance**: All code strictly follows POSIX.1-2008 standards with specified compiler flags.

**LLM Usage Policy**: Any LLM-generated code is clearly marked:
```c
############## LLM Generated Code Begins ##############
// LLM-generated implementation
############## LLM Generated Code Ends ################
```
Corresponding prompts and responses are documented in `llm_completions/` with image files.

**File Organization**: Modular design with separate `.c` and `.h` files based on functionality. Monolithic implementations are heavily penalized.

## Evaluation Criteria

**I/O Evaluation (Automated)**:
- Automated testing environment similar to online judges
- Strict input/output format compliance required
- Sample test cases provided for validation

**Code Evaluation (Manual)**:  
- In-person viva with TAs
- Code understanding and design decisions
- Higher weightage than automated evaluation

## Troubleshooting

**Common Build Issues:**
- Ensure POSIX compliance flags are used
- Check OpenSSL development packages for networking
- Verify `shell.out` is created in `shell/` directory (not root)

**Runtime Issues:**
- Shell prompt format must match exactly: `<user@host:path> `
- Error messages must match specification exactly
- Background process handling requires proper signal management

## Performance Benchmarks

The xv6 schedulers are evaluated on:
- **Turnaround Time**: Time from process arrival to completion
- **Response Time**: Time from arrival to first CPU allocation  
- **Fairness**: CPU time distribution among processes
- **Throughput**: Number of processes completed per time unit

Detailed performance analysis and comparisons are included in the final report.

## Documentation

- **Design Decisions**: Architecture choices and implementation rationale
- **Performance Analysis**: Scheduler comparison with graphs and metrics  
- **Test Results**: Comprehensive test case coverage and results
- **LLM Documentation**: Complete prompt/response history for any AI assistance
