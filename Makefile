# Compiler and flags
CC = gcc
CFLAGS = -Wall -g -Iinclude
LDFLAGS = 
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

# Install dependencies (for later features)
deps:
	sudo apt update
	sudo apt install -y libreadline-dev

.PHONY: all clean deps
