CC = gcc
CFLAGS = -std=c99 -D_POSIX_C_SOURCE=200809L -D_GNU_SOURCE -Wall -Wextra -Werror -Wno-unused-parameter -Wno-deprecated-declarations
LIBS = -lcrypto

SRCDIR = src
INCDIR = include
OBJDIR = obj

# Source files
SERVER_SOURCES = $(SRCDIR)/server.c \
                 $(SRCDIR)/utils/sham_protocol.c \
                 $(SRCDIR)/utils/network_utils.c \
                 $(SRCDIR)/utils/logging.c \
                 $(SRCDIR)/md5/md5_utils.c

CLIENT_SOURCES = $(SRCDIR)/client.c \
                 $(SRCDIR)/utils/sham_protocol.c \
                 $(SRCDIR)/utils/network_utils.c \
                 $(SRCDIR)/utils/logging.c \
                 $(SRCDIR)/md5/md5_utils.c

# Object files
SERVER_OBJECTS = $(SERVER_SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
CLIENT_OBJECTS = $(CLIENT_SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# Targets
SERVER_TARGET = server
CLIENT_TARGET = client

# Create object directory structure
$(shell mkdir -p $(OBJDIR)/utils $(OBJDIR)/md5)

.PHONY: all clean

all: $(SERVER_TARGET) $(CLIENT_TARGET)

$(SERVER_TARGET): $(SERVER_OBJECTS)
	$(CC) $(SERVER_OBJECTS) -o $(SERVER_TARGET) $(LIBS)

$(CLIENT_TARGET): $(CLIENT_OBJECTS)
	$(CC) $(CLIENT_OBJECTS) -o $(CLIENT_TARGET) $(LIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(SERVER_TARGET) $(CLIENT_TARGET) *.txt received_file

test: all
	@echo "Running networking tests..."
	@echo "Test with: ./server 8080"
	@echo "Test with: ./client 127.0.0.1 8080 --chat"

.SUFFIXES: .c .o