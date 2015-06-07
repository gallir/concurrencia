CC=gcc
CFLAGS=-pthread -fgnu-tm 

STM=tinySTM/lib/libstm.a
SOURCES=$(wildcard *.c)
HEADERS=$(wildcard *.h)
PROGS=$(patsubst %.c,%,$(SOURCES))


UNAME := $(shell uname -a)
ifeq ($(findstring x86_64,$(UNAME)),x86_64)
    CFLAGS += -mcx16
    ARCH = intel
endif

ifeq ($(findstring armv7l,$(UNAME)),armv7l)
    CFLAGS += -march=armv7-a
endif


all: $(PROGS) $(ARCH)

mutex_tinystm: mutex_tinystm.c $(STM) $(HEADERS)
	- $(CC) $(CFLAGS) -o $@ $< $(STM)

array_tinystm: array_tinystm.c $(STM) $(HEADERS)
	- $(CC) $(CFLAGS) -o $@ $< $(STM)

rw_tinystm: rw_tinystm.c $(STM) $(HEADERS)
	- $(CC) $(CFLAGS) -o $@ $< $(STM)

%: %.c $(HEADERS)
	- $(CC) $(CFLAGS) -o $@ $< 

$(STM):
	mkdir -p tinySTM/lib
	$(MAKE) -C tinySTM

intel: $(HEADERS)
	$(MAKE) -C intel 

EXES=$(basename $(wildcard *.c ))

.PHONY: clean intel
clean:
	-rm -f $(PROGS) $(EXES) *.class *~
	$(MAKE) -C tinySTM clean
	$(MAKE) -C intel clean

print-%:
	@echo $* = $($*)
