# Compiler and flags
CC=gcc
CFLAGS=-Wall -Wextra -Iinclude -g 

# Directories
SRCDIR=src
OBJDIR=obj

# Sources and objects
SRCS=$(shell find $(SRCDIR) -name "*.c")
OBJS=$(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))

# Target
TARGET=mysh

all: | $(OBJDIR) $(TARGET)

$(OBJDIR):
	mkdir -p $@

$(TARGET): $(OBJS)
	$(CC) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(OBJDIR)/* $(TARGET)

.PHONY: all clean