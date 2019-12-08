CC=gcc
CFLAGS ?= -g -O2
XCFLAGS =
override CFLAGS += -Wall -D_GNU_SOURCE
SRCS=io_interface.c main.c

all:
	$(QUIET_CC)$(CC) $(CFLAGS) -o app $(SRCS) -luring  $(XCFLAGS)

build: all

clean:
	rm -f app