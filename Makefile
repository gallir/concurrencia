CC=gcc
CFLAGS=-pthread
SOURCES=$(wildcard *.c)
PROGS=$(patsubst %.c,%,$(SOURCES))

GO_SOURCES=$(wildcard *.go)
GO_PROGS=$(patsubst %.go,%,$(GO_SOURCES))

all: $(PROGS) $(GO_PROGS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $<

%: %.go
	go build $<

EXES=$(basename $(wildcard *.c *.go ))

.PHONY: clean
clean:
	-rm -f $(PROGS) $(EXES) *.class *~

print-%:
	@echo $* = $($*)
