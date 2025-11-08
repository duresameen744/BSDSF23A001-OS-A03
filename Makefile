# Compiler and flags
CC = gcc
CFLAGS = -Wall -g -Iinclude

# Check if readline is available
HAS_READLINE := $(shell pkg-config --exists readline && echo 1 || echo 0)

ifeq ($(HAS_READLINE),1)
    CFLAGS += -DHAS_READLINE
    LDFLAGS = -lreadline
else
    $(warning Readline not found. Using fallback input method.)
    LDFLAGS = 
endif

TARGET = bin/myshell
SRCDIR = src
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:.c=.o)

# Default target
all: $(TARGET)

# Create target executable
$(TARGET): $(OBJECTS)
	@mkdir -p bin
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

# Compile source files to object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET)

# Install dependencies
deps:
	sudo apt update
	sudo apt install -y libreadline-dev build-essential pkg-config

.PHONY: all clean deps
