CC = gcc
CFLAGS = -std=c99 -D_POSIX_C_SOURCE=200809L -Wall -Wextra
SOURCES_SERVER = src/server.c src/utils/sham_protocol.c
SOURCES_CLIENT = src/client.c src/utils/sham_protocol.c
OBJECTS_SERVER = $(SOURCES_SERVER:.c=.o)
OBJECTS_CLIENT = $(SOURCES_CLIENT:.c=.o)

all: server client

server: $(OBJECTS_SERVER)
	$(CC) $(CFLAGS) -o $@ $^

client: $(OBJECTS_CLIENT)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -I. -c $< -o $@

clean:
	rm -f $(OBJECTS_SERVER) $(OBJECTS_CLIENT) server client

.PHONY: all clean
