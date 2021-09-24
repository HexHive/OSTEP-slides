CC=clang
CFLAGS=--std=gnu99 -Wall -Wextra -Wpedantic -lpthread
SRC=$(shell ls *.c)
DST=$(SRC:%.c=%.out)

.PHONY=all clean
all: $(DST)

%.out: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f ${DST}
