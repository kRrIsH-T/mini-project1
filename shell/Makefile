# Makefile for OSN Shell

CC = gcc
CFLAGS = -std=c99 -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=700 -Wall -Wextra -Werror -Wno-unused-parameter -fno-asm
TARGET = shell.out
SRCDIR = src
INCDIR = include
OBJDIR = obj

# Source files
SOURCES = $(SRCDIR)/main.c \
          $(SRCDIR)/prompt.c \
          $(SRCDIR)/input.c \
          $(SRCDIR)/parser.c \
          $(SRCDIR)/builtins/builtins.c \
          $(SRCDIR)/builtins/hop.c \
          $(SRCDIR)/builtins/reveal.c \
          $(SRCDIR)/builtins/log.c \
          $(SRCDIR)/builtins/activities.c \
          $(SRCDIR)/builtins/ping.c \
          $(SRCDIR)/builtins/fg_bg.c \
          $(SRCDIR)/execution/executor.c \
          $(SRCDIR)/signals/signal_handlers.c \
          $(SRCDIR)/utils/path_utils.c \
          $(SRCDIR)/utils/string_utils.c \
          $(SRCDIR)/utils/process_utils.c

# Object files
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# Create object directory structure
$(shell mkdir -p $(OBJDIR)/builtins $(OBJDIR)/execution $(OBJDIR)/signals $(OBJDIR)/utils)

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(TARGET)

test: $(TARGET)
	@echo "Running shell tests..."
	@./tests/run_tests.sh

install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

.SUFFIXES: .c .o

# Scheduler selection
ifndef SCHEDULER
SCHEDULER := RR
endif

ifeq ($(SCHEDULER), FCFS)
CFLAGS += -DFCFS
endif

ifeq ($(SCHEDULER), CFS)
CFLAGS += -DCFS
endif