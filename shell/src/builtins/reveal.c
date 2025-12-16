#include "../../include/builtins.h"
#include "../../include/utils.h"

int builtin_reveal(char** args) {
    int show_all = 0;      // -a flag
    int long_format = 0;   // -l flag
    char target_dir[MAX_PATH_LEN];
    
    // Parse flags and target directory
    int i = 1;
    while (args[i] != NULL && args[i][0] == '-' && strcmp(args[i], "-") != 0) {
        for (int j = 1; args[i][j] != '\0'; j++) {
            if (args[i][j] == 'a') {
                show_all = 1;
            } else if (args[i][j] == 'l') {
                long_format = 1;
            }
        }
        i++;
    }
    
    // Determine target directory
    if (args[i] == NULL) {
        strcpy(target_dir, current_dir);
    } else if (strcmp(args[i], "~") == 0) {
        strcpy(target_dir, home_dir);
    } else if (strcmp(args[i], ".") == 0) {
        strcpy(target_dir, current_dir);
    } else if (strcmp(args[i], "..") == 0) {
        char* last_slash = strrchr(current_dir, '/');
        if (last_slash && last_slash != current_dir) {
            strncpy(target_dir, current_dir, last_slash - current_dir);
            target_dir[last_slash - current_dir] = '\0';
        } else {
            strcpy(target_dir, "/");
        }
    } else if (strcmp(args[i], "-") == 0) {
        if (strlen(prev_dir) == 0) {
            printf("No such directory!\n");
            return 1;
        }
        strcpy(target_dir, prev_dir);
    } else {
        char* resolved = resolve_path(args[i]);
        if (resolved) {
            strcpy(target_dir, resolved);
            free(resolved);
        } else {
            printf("reveal: %s: No such file or directory\n", args[i]);
            return 1;
        }
    }
    
    // Check for too many arguments
    if (args[i] != NULL && args[i + 1] != NULL) {
        printf("reveal: Invalid Syntax!\n");
        return 1;
    }
    
    // Open directory
    DIR* dir = opendir(target_dir);
    if (!dir) {
        printf("No such directory!\n");
        return 1;
    }
    
    // Read directory entries
    struct dirent* entry;
    char** entries = safe_malloc(1000 * sizeof(char*));
    int count = 0;
    
    while ((entry = readdir(dir)) != NULL && count < 1000) {
        // Skip hidden files unless -a flag is set
        if (!show_all && entry->d_name[0] == '.') {
            continue;
        }
        
        entries[count] = safe_strdup(entry->d_name);
        count++;
    }
    
    closedir(dir);
    
    // Sort entries lexicographically
    qsort(entries, count, sizeof(char*), string_compare);
    
    // Display entries
    if (long_format) {
        // One entry per line
        for (int j = 0; j < count; j++) {
            printf("%s\n", entries[j]);
        }
    } else {
        // Space-separated format
        for (int j = 0; j < count; j++) {
            printf("%s", entries[j]);
            if (j < count - 1) {
                printf(" ");
            }
        }
        if (count > 0) {
            printf("\n");
        }
    }
    
    // Free memory
    free_string_array(entries, count);
    
    return 0;
}
