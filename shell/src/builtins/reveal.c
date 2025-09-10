#include "../../include/builtins.h"
#include <dirent.h>

void builtin_reveal(Command *cmd) {
    DIR *dir;
    struct dirent *entry;
    int show_hidden = 0;
    int long_format = 0;

    for (int i = 1; i < cmd->argc; i++) {
        if (strcmp(cmd->argv[i], "-a") == 0) {
            show_hidden = 1;
        } else if (strcmp(cmd->argv[i], "-l") == 0) {
            long_format = 1;
        }
    }

    dir = opendir(".");
    if (dir == NULL) {
        perror("reveal");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (!show_hidden && entry->d_name[0] == '.') {
            continue;
        }
        printf("%s\n", entry->d_name);
    }

    closedir(dir);
}
