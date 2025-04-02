# Compiler and flags
CC = gcc
CFLAGS = -Wall -pthread -g

# Target programs
TARGETS = writer.out reader.out

# Default rule: Build all targets
all: $(TARGETS)

# Generic build rule
%.out: %.c common.h
	$(CC) $(CFLAGS) -o $@ $<

# Clean rule
clean:
	rm -f $(TARGETS)
