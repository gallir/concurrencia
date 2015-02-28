CC=gcc
CFLAGS=-pthread

SOURCES=$(wildcard *.s)
PROGS=$(patsubst %.s,%,$(SOURCES))

all: $(PROGS)

%: %.s
	- $(CC) $(CFLAGS) -o $@ $<


EXES=$(basename $(wildcard *.s))

.PHONY: clean
clean:
	-rm -f $(PROGS) $(EXES)

print-%:
	@echo $* = $($*)
