#include "../include/shell.h"
#include "../include/utils.h"


char *read_input(void)
{
    char *input = safe_malloc(MAX_INPUT_SIZE);

    if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL)
    {

        free(input);
        return NULL;
    }

    // Remove trailing newline
    size_t len = strlen(input);
    if (len > 0 && input[len - 1] == '\n')
    {
        input[len - 1] = '\0';
    }

    return input;
}
