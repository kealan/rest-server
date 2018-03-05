TARGETS=main

CC=gcc

CCOPTS=-static -Wall -Wextra

# Project version
VERSION=$(shell cat VERSION)

# ServiceProject version
SERVICE=SERVICE-TEST

.PHONY: all clean format

all: $(TARGETS)

# Format the source code
format:
	astyle --style=allman --recursive --suffix=none '*.c'

clean:
	rm -f $(TARGETS) *~

%: %.c
	$(CC) $(CCOPTS) -D DEBUG -D VERSION="\"${VERSION}\"" -D SERVICE="\"${SERVICE}\"" -o $@ $<
