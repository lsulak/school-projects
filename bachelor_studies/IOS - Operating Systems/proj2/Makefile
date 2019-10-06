# Makefile pre 2. projekt z predmetu Operacne systemy
# Autor: Ladislav Sulak
# Login: xsulak04
# Datum: april 2013
# Pouzitie: make        --> preklad programu
#           make clean  --> vymazanie suborov kt. vznikli prekladom
#           make pack   --> zabali subory potrebne pre odovzdanie
#                           do xsulak04.zip , zabali *.c *.h Makefile

NAME = main

CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -pedantic -Werror
LFLAGS = -lrt -lpthread

ALLFILES = *.c *.h Makefile

$(NAME): $(NAME).c
	$(CC) $(CFLAGS) $(LFLAGS) $(NAME).c -o santa
	
pack: 
	zip xsulak04.zip $(ALLFILES) 

clean: 
	rm -f *.o
	
cleanA:
	rm -f *.o santa
