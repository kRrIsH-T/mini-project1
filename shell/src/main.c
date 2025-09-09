#include "../include/shell.h"

int main() {
    Command cmd;
    char *input;
    
    while (1) {
        prompt();
        input = read_input();
        
        if (input == NULL) {
            break;
        }
        
        parse_input(input, &cmd);
        
        if (cmd.argc > 0) {
            execute_command(&cmd);
        }
        
        free(input);
    }
    
    return 0;
}
