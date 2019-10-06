CC=g++
CFLAGS=-std=c++11 -g -Wall -pedantic -O2
LIBS=-lm
ZIPPED = Makefile bms1A.cpp bms1B.cpp sndfile.h sndfile.hh

all: bms1A bms1B


bms1A: bms1A.cpp
	$(CC) $(CFLAGS) $(LIBS) $^ -o $@ libsndfile.a

bms1B: bms1B.cpp
	$(CC) $(CFLAGS) $(LIBS) $^ -o $@ libsndfile.a

clean:
	rm -f *.o bms1A bms1B

zip: clean
	zip xsulak04.zip $(ZIPPED)