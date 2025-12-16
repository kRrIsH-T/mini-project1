#include "../../include/sham.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

static FILE* log_file = NULL;
static int logging_enabled = 0;

void init_logging(const char* filename) {
    char* env_log = getenv("RUDP_LOG");
    if (env_log && strcmp(env_log, "1") == 0) {
        logging_enabled = 1;
        log_file = fopen(filename, "w");
        if (!log_file) {
            perror("Failed to open log file");
            logging_enabled = 0;
        }
    }
}

void log_event(const char* format, ...) {
    if (!logging_enabled || !log_file) return;
    
    char time_buffer[30];
    struct timeval tv;
    time_t curtime;
    
    gettimeofday(&tv, NULL);
    curtime = tv.tv_sec;
    
    // Format the time part
    strftime(time_buffer, 30, "%Y-%m-%d %H:%M:%S", localtime(&curtime));
    
    // Write timestamp and LOG prefix
    fprintf(log_file, "[%s.%06ld] [LOG] ", time_buffer, tv.tv_usec);
    
    // Write the actual message
    va_list args;
    va_start(args, format);
    vfprintf(log_file, format, args);
    va_end(args);
    
    fprintf(log_file, "\n");
    fflush(log_file);
}

void cleanup_logging(void) {
    if (log_file) {
        fclose(log_file);
        log_file = NULL;
    }
    logging_enabled = 0;
}