CC=gcc
CFLAGS=-pthread
LIBS=stack_llsc.s

SOURCES=$(wildcard *.c)
PROGS=$(patsubst %.c,%,$(SOURCES))

all: $(PROGS)

%: %.c $(LIBS)
	$(CC) $(CFLAGS) $(LIBS) -o $@ $<

.PHONY: clean
clean:
	-rm -f $(PROGS) 

