CC=gcc
CFLAGS=-pthread -fgnu-tm -mrtm

UNAME := $(shell uname -a)
ifeq ($(findstring x86_64,$(UNAME)),x86_64)
    CFLAGS += 
endif

ifeq ($(findstring armv7l,$(UNAME)),armv7l)
    CFLAGS += -march=armv7-a
endif

SOURCES=$(wildcard *.c)
HEADERS=$(wildcard *.h, ../*.h)
PROGS=$(patsubst %.c,%,$(SOURCES))

all: $(PROGS) 

array_rtm_tinystm: array_rtm_tinystm.c $(HEADERS)
	- $(CC) $(CFLAGS) -o $@ $< ../tinySTM/lib/libstm.a

%: %.c $(HEADERS)
	- $(CC) $(CFLAGS) -o $@ $< 

%.class: %.java
	- javac $<

%: %.go
	- go build $<

EXES=$(basename $(wildcard *.c *.go ))

.PHONY: clean
clean:
	-rm -f $(PROGS) $(EXES) *.class *~

print-%:
	@echo $* = $($*)
