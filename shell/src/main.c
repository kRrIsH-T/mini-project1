#include "../include/shell.h"
#include "../include/background.h"
#include "../include/signals.h"

int main() {
    Command cmd;
    char *input;

    setup_signals();

    while (1) {
        prompt();
        input = read_input();

        if (input == NULL) {
            break;
        }

        parse_input(input, &cmd);

        if (cmd.argc > 0) {
            if (check_background_flag(&cmd)) {
                execute_background(&cmd);
            } else {
                execute_command(&cmd);
            }
        }

        free(input);
    }

    return 0;
}
