src = $(wildcard *.c)
obj = $(src:.c=.o)

LDFLAGS =

.PHONY: all
all: fenmesh-server

.PHONY: all-debug
all-debug: override CFLAGS += -g
all-debug: all

fenmesh-server: main.c log.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) fenmesh-server