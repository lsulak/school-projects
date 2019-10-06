CC =g++
CFLAGS = -Wall -Wextra -pedantic
NAME = ftpclient
ZIPPED = Makefile ftpclient.cpp

$(NAME): $(NAME).cpp
	$(CC) $(CFLAGS) $(NAME).cpp -o $(NAME)

tar: clean
	tar -cf xsulak04.tar $(ZIPPED)
	gzip xsulak04.tar
