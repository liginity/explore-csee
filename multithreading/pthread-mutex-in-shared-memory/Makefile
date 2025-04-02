# Compiler and flags
CC = gcc
CFLAGS = -Wall -pthread -g

# Target programs
TARGETS = writer reader

# Default rule: Build all targets
all: $(TARGETS)

# Generic build rule
%: %.c common.h
	$(CC) $(CFLAGS) -o $@ $<

# Clean rule
clean:
	rm -f $(TARGETS)
