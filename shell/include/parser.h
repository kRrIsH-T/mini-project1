#ifndef PARSER_H
#define PARSER_H

#include "shell.h"

int has_pipe(const char *input);
int has_redirect(const char *input);
char *trim_whitespace(char *str);

#endif
