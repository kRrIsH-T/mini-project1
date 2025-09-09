#include "../include/shell.h"

void prompt() {
    char hostname[256];
    char cwd[PATH_MAX];
    struct passwd *pw;
    
    pw = getpwuid(getuid());
    gethostname(hostname, sizeof(hostname));
    getcwd(cwd, sizeof(cwd));
    
    printf("<%s@%s:%s> ", pw->pw_name, hostname, cwd);
    fflush(stdout);
}
