#include "../../include/builtins.h"
#include "../../include/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int builtin_hop(char** args) {
    char target[MAX_PATH_LEN];

    // If no args, treat as ~
    if (args[1] == NULL) {
        if (change_directory(home_dir) == 0) {
            if (strlen(prev_dir) == 0) {
                strncpy(prev_dir, current_dir, MAX_PATH_LEN - 1);
                prev_dir[MAX_PATH_LEN - 1] = '\0';
            }
            update_current_dir();
            return 0;
        } else {
            printf("No such directory!\n");
            return 1;
        }
    }

    // Process arguments sequentially
    for (int i = 1; args[i] != NULL; i++) {
        if (strcmp(args[i], "~") == 0) {
            strncpy(target, home_dir, MAX_PATH_LEN - 1);
            target[MAX_PATH_LEN - 1] = '\0';
        } else if (strcmp(args[i], ".") == 0) {
            // stay in current dir - this should not change anything
            continue;
        } else if (strcmp(args[i], "..") == 0) {
            // parent dir of current_dir
            char tmp[MAX_PATH_LEN];
            strncpy(tmp, current_dir, MAX_PATH_LEN - 1);
            tmp[MAX_PATH_LEN - 1] = '\0';
            char* last = strrchr(tmp, '/');
            if (last == NULL || last == tmp) {
                strncpy(target, "/", MAX_PATH_LEN - 1);
                target[MAX_PATH_LEN - 1] = '\0';
            } else {
                *last = '\0';
                strncpy(target, tmp, MAX_PATH_LEN - 1);
                target[MAX_PATH_LEN - 1] = '\0';
            }
        } else if (strcmp(args[i], "-") == 0) {
            if (strlen(prev_dir) == 0) {
                // do nothing if no previous dir
                continue;
            } else {
                strncpy(target, prev_dir, MAX_PATH_LEN - 1);
                target[MAX_PATH_LEN - 1] = '\0';
            }
        } else {
            char* resolved = resolve_path(args[i]);
            if (!resolved) {
                printf("No such directory!\n");
                return 1;
            }
            strncpy(target, resolved, MAX_PATH_LEN - 1);
            target[MAX_PATH_LEN - 1] = '\0';
            free(resolved);
        }

        // Attempt to change to target
        if (change_directory(target) != 0) {
            printf("No such directory!\n");
            return 1;
        }

        // Update prev_dir and current_dir
        if (strlen(prev_dir) == 0) {
            strncpy(prev_dir, current_dir, MAX_PATH_LEN - 1);
            prev_dir[MAX_PATH_LEN - 1] = '\0';
        } else {
            // shift current into prev
            strncpy(prev_dir, current_dir, MAX_PATH_LEN - 1);
            prev_dir[MAX_PATH_LEN - 1] = '\0';
        }
        update_current_dir();
    }

    return 0;
}
