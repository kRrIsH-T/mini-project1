CC = gcc
CFLAGS = -std=c99 -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=700 -Wall -Wextra -Werror -Wno-unused-parameter
SOURCES = src/main.c src/prompt.c src/input.c src/parser.c src/parser_utils.c src/execution.c src/utils.c src/builtins/builtins.c src/builtins/hop.c src/builtins/reveal.c src/builtins/log.c src/redirection/pipes.c src/redirection/redirect.c src/signals/signals.c src/job_control/activities.c src/job_control/ping.c src/job_control/fg_bg.c src/background/background.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = shell.out

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean
