# Author			Ladislav Sulak (xsulak04@stud.fit.vutbr.cz)
# Date				2/2016
# File 				Makefile
# Description		Makefile for subject KKO (Kodovani a komprese dat), 
#					project GIF to BMP conversion.


NAME=gif2bmp

FILES=gif2bmp.c main.c gif2bmp.h

OBJ=gif2bmp.o main.o

DEBFLAGS=-ggdb3 -DDEBUG

CC=gcc
CFLAGS=-std=c99 -Wformat -Wsequence-point -Wswitch -Wall -Wextra -pedantic

CPP=g++
CPPFLAGS=-std=c++98 -Wformat -Wsequence-point -Wswitch -Wall -Wextra -pedantic

ZIPPED = Makefile gif2bmp.c gif2bmp.h main.c gif2bmp.pdf

$(NAME): main.c gif2bmp.c
	$(CPP) $(CPPFLAGS) $(FILES) -o $@

debug:
	$(CPP) $(CPPFLAGS) $(DEBFLAGS) $(FILES) -o $(NAME)

clean:
	rm -f $(OBJ) $(NAME)

zip: clean
	zip kko.proj3.xsulak04.zip $(ZIPPED)
