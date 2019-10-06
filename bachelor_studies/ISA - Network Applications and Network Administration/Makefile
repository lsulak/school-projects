CC =g++
CFLAGS = -Wall -Wextra -pedantic
NAME = httphdrs
ZIPPED = Makefile README manual.pdf $(NAME).cpp
LIBS = -lpcap -lxml2 -I/usr/include/libxml2
$(NAME): $(NAME).cpp
	$(CC) $(CFLAGS) $(NAME).cpp -o $(NAME) $(LIBS)

tar: clean
	tar -cf xsulak04.tar $(ZIPPED)
	gzip xsulak04.tar
