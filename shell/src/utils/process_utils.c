#include "../../include/utils.h"
#include "../../include/shell.h"

// System information
void get_username_hostname(void) {
    // Get username
    struct passwd* pw = getpwuid(getuid());
    if (pw) {
        strncpy(username, pw->pw_name, sizeof(username) - 1);
        username[sizeof(username) - 1] = '\0';
    } else {
        strcpy(username, "unknown");
    }
    
    // Get hostname
    if (gethostname(hostname, sizeof(hostname)) != 0) {
        strcpy(hostname, "unknown");
    }
}

char* get_current_time_string(void) {
    time_t now = time(NULL);
    char* time_str = ctime(&now);
    
    // Remove newline from ctime
    if (time_str) {
        char* newline = strchr(time_str, '\n');
        if (newline) *newline = '\0';
    }
    
    return safe_strdup(time_str);
}

// Process utilities
int get_process_count(void) {
    // Simple implementation - count processes in our list
    // This would be more complex in a real shell
    return 0;
}

char* get_process_name(pid_t pid) {
    // Simple implementation - return pid as string
    char* name = safe_malloc(32);
    snprintf(name, 32, "process_%d", pid);
    return name;
}

int is_process_running(pid_t pid) {
    return kill(pid, 0) == 0;
}

void send_signal_to_process(pid_t pid, int signal) {
    kill(pid, signal);
}

// File utilities
int file_exists(char* filename) {
    return access(filename, F_OK) == 0;
}

long get_file_size(char* filename) {
    struct stat st;
    if (stat(filename, &st) == 0) {
        return st.st_size;
    }
    return -1;
}

int copy_file(char* src, char* dest) {
    FILE* source = fopen(src, "rb");
    if (!source) return -1;
    
    FILE* target = fopen(dest, "wb");
    if (!target) {
        fclose(source);
        return -1;
    }
    
    char buffer[4096];
    size_t bytes;
    
    while ((bytes = fread(buffer, 1, sizeof(buffer), source)) > 0) {
        if (fwrite(buffer, 1, bytes, target) != bytes) {
            fclose(source);
            fclose(target);
            return -1;
        }
    }
    
    fclose(source);
    fclose(target);
    return 0;
}
