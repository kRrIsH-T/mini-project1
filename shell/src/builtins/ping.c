#include "../../include/builtins.h"
#include "../../include/execution.h"
#include <signal.h>
#include <errno.h>

int builtin_ping(char** args) {
    if (args[1] == NULL || args[2] == NULL) {
        printf("Usage: ping <pid> <signal_number>\n");
        return 1;
    }
    
    pid_t pid = atoi(args[1]);
    int signal_number = atoi(args[2]);
    
    if (pid <= 0) {
        printf("Invalid PID\n");
        return 1;
    }
    
    // Take signal number modulo 32
    int actual_signal = signal_number % 32;
    
    // Send signal to process
    if (kill(pid, actual_signal) == -1) {
        if (errno == ESRCH) {
            printf("No such process found\n");
        } else {
            perror("kill");
        }
        return 1;
    }
    
    printf("Sent signal %d to process with pid %d\n", signal_number, pid);
    return 0;
}