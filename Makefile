CC=gcc
CFLAGS=-pthread
SOURCES=$(wildcard *.c)
PROGS=$(patsubst %.c,%,$(SOURCES))

all: $(PROGS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $<


EXES=$(basename $(wildcard *.c *.go ))

.PHONY: clean
clean:
	-rm -f $(PROGS) $(EXES) *.class *~

print-%:
	@echo $* = $($*)
