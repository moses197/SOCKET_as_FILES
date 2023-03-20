CC=gcc
CFLAGS=-g -Wall

all: countlines.o

%: %.c
	$(CC) (CFLAGS) -o $@ $^

clean:
	rm -rf *.dSYM countlines
