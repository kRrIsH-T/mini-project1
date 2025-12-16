#include "../../include/utils.h"
#include "../../include/shell.h"

// Path utilities
char* get_absolute_path(char* path) {
    if (!path) return NULL;
    
    if (path[0] == '/') {
        // Already absolute
        return safe_strdup(path);
    }
    
    // Relative path - prepend current directory
    size_t current_len = strlen(current_dir);
    size_t path_len = strlen(path);
    size_t total_len = current_len + path_len + 2; // +1 for '/', +1 for '\0'
    
    if (total_len > MAX_PATH_LEN) {
        return NULL; // Path too long
    }
    
    char* full_path = safe_malloc(MAX_PATH_LEN);
    int result = snprintf(full_path, MAX_PATH_LEN, "%s/%s", current_dir, path);
    
    // Additional safety check (though this should never happen given our length check)
    if (result >= MAX_PATH_LEN || result < 0) {
        free(full_path);
        return NULL;
    }
    return full_path;
}

char* get_relative_to_home(char* path) {
    if (!path || strlen(home_dir) == 0) return safe_strdup(path);
    
    int home_len = strlen(home_dir);
    
    // Check if path starts with home directory
    if (strncmp(path, home_dir, home_len) == 0) {
        if (path[home_len] == '\0') {
            // Exact match - return ~
            return safe_strdup("~");
        } else if (path[home_len] == '/') {
            // Path is under home - return ~/rest
            char* result = safe_malloc(strlen(path) - home_len + 2);
            sprintf(result, "~%s", path + home_len);
            return result;
        }
    }
    
    // Not under home directory
    return safe_strdup(path);
}

int path_exists(char* path) {
    return access(path, F_OK) == 0;
}

int is_directory(char* path) {
    struct stat st;
    if (stat(path, &st) == 0) {
        return S_ISDIR(st.st_mode);
    }
    return 0;
}

char* resolve_path(char* path) {
    if (!path) return NULL;
    
    if (strcmp(path, "~") == 0) {
        return safe_strdup(home_dir);
    }
    
    if (path[0] == '~' && path[1] == '/') {
        // ~/something
        char* result = safe_malloc(strlen(home_dir) + strlen(path));
        sprintf(result, "%s%s", home_dir, path + 1);
        return result;
    }
    
    if (path[0] == '/') {
        // Absolute path
        return safe_strdup(path);
    }
    
    // Relative path
    return get_absolute_path(path);
}

int change_directory(char* path) {
    return chdir(path);
}
