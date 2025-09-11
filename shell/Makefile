CC = gcc
CFLAGS = -std=c99 -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=700 -Wall -Wextra -Werror -Wno-unused-parameter
SOURCES = src/main.c src/prompt.c src/input.c src/parser.c src/execution.c src/utils.c src/builtins/builtins.c src/builtins/hop.c src/builtins/reveal.c src/builtins/log.c
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
