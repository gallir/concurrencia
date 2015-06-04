CC=gcc
CFLAGS=-pthread -fgnu-tm

STM=tinySTM/lib/libstm.a

UNAME := $(shell uname -a)
ifeq ($(findstring x86_64,$(UNAME)),x86_64)
    CFLAGS += -mcx16
endif

ifeq ($(findstring armv7l,$(UNAME)),armv7l)
    CFLAGS += -march=armv7-a
endif

SOURCES=$(wildcard *.c)
HEADERS=$(wildcard *.h)
PROGS=$(patsubst %.c,%,$(SOURCES))


all: $(PROGS) intel

%: %.c $(STM) $(HEADERS)
	- $(CC) $(CFLAGS) -o $@ $< $(STM)

$(STM):
	mkdir -p tinySTM/lib
	$(MAKE) -C tinySTM

intel:
	$(MAKE) -C intel clean

EXES=$(basename $(wildcard *.c ))

.PHONY: clean
clean:
	-rm -f $(PROGS) $(EXES) *.class *~
	$(MAKE) -C tinySTM clean
	$(MAKE) -C intel clean

print-%:
	@echo $* = $($*)
