CC = g++
CFLAGS = -Wall -Wextra -pedantic

ZIPPED = Makefile client.cpp server.cpp

all: server client

client: client.cpp
	$(CC) $(CFLAGS) -o $@ client.cpp

server: server.cpp
	$(CC) $(CFLAGS) -o $@ server.cpp
	
clean:
	rm -f *.o *.out *~ client server *.core

tar: clean
	tar -cf xsulak04.tar $(ZIPPED)
	gzip xsulak04.tar
