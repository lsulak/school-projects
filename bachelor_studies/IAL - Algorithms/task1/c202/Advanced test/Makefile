
PRJ=c202
#
PROGS=$(PRJ)-aastandard-test $(PRJ)-advanced-test
CC=gcc
CFLAGS=-Wall -std=c99 -pedantic -lm

all: $(PROGS)

$(PRJ)-aastandard-test: $(PRJ).c $(PRJ)-aastandard-test.c
	$(CC) $(CFLAGS) -o $@ $(PRJ).c $(PRJ)-aastandard-test.c

$(PRJ)-advanced-test: $(PRJ).c $(PRJ)-advanced-test.c
	$(CC) $(CFLAGS) -o $@ $(PRJ).c $(PRJ)-advanced-test.c

clean:
	rm -f *.o *.out $(PROGS)
#
