#ifndef SIGNALS_H
#define SIGNALS_H

#include "shell.h"
#include "execution.h"

// Signal handling functions
void setup_signal_handlers(void);
void sigint_handler(int sig);    // Ctrl-C
void sigtstp_handler(int sig);   // Ctrl-Z
void sigchld_handler(int sig);   // Child process termination

// Job control functions
void bring_to_foreground(process_t* proc);
void send_to_background(process_t* proc);
void stop_foreground_process(void);
void terminate_foreground_process(void);

// Current foreground process tracking
extern pid_t foreground_pid;
extern process_t* foreground_proc;

#endif
