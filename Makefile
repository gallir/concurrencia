CC=gcc
CFLAGS=-pthread

ifeq ($(UNAME_P),x86_64)
    CCFLAGS += -mcx16
endif

SOURCES=$(wildcard *.c)
PROGS=$(patsubst %.c,%,$(SOURCES))

GO_SOURCES=$(wildcard *.go)
GO_PROGS=$(patsubst %.go,%,$(GO_SOURCES))

all: $(PROGS) $(GO_PROGS)

%: %.c
	- $(CC) $(CFLAGS) -o $@ $<

%: %.go
	- go build $<

EXES=$(basename $(wildcard *.c *.go ))

.PHONY: clean
clean:
	-rm -f $(PROGS) $(EXES) *.class *~

print-%:
	@echo $* = $($*)
