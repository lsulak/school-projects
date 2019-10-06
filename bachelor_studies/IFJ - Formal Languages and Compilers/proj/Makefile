NAME=interpret
DEBNAME=test test2
OBJ=str.o scanner.o main.o ial.o psa.o ilist.o interpret.o parser.o instructions.o
DEBOBJ=test.o test2.o
DEBFLAGS=-ggdb  -DDEBUG
LDFLAGS=-lunwind
ifndef DEBUG
DEBFLAGS=
endif

CFLAGS=-std=c11 -Wformat -Wsequence-point -Wswitch -Wall -Wextra -pedantic  $(DEBFLAGS)
CC=gcc
RM=rm -f -v

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

test2: test2.o ial.o str.o ilist.o instructions.o
	$(CC) $(LDFAGS)  -o test2 test2.o ial.o str.o ilist.o instructions.o

test: ial.o test.o str.o
	$(CC) $(LDFLAGS) -o test test.o ial.o str.o

main.o: main.c str.h scanner.h parser.h ial.h ilist.h interpret.h debug.h

test2.o: test_inst.c test2.h str.h str.c ial.h ial.c ilist.h ilist.c instructions.h instructions.c
	$(CC) $(CFLAGS) -c -o $@ $<

test.o: test.c str.h ial.h ilist.h
	$(CC) $(CFLAGS) -c -o $@ $<

ial.o: ial.c ial.h str.h ilist.h
	$(CC) $(CFLAGS) -c -o $@ $<

str.o: str.c str.h
	$(CC) $(CFLAGS) -c -o $@ $<

psa.o: psa_new.c psa_new.h scanner.h parser.h ilist.h debug.h interpret.h ial.h ilist.h
	$(CC) $(CFLAGS) -c -o $@ $<

ilist.o: ilist.c ilist.h interpret.h str.h debug.h
	$(CC) $(CFLAGS) -c -o $@ $<

parser.o: parser.c parser.h interpret.h scanner.h str.h psa_new.h ial.h ilist.h debug.h
	$(CC) $(CFLAGS) -c -o $@ $<

interpret.o: interpret.c ilist.h interpret.h ial.h debug.h
	$(CC) $(CFLAGS) -c -o $@ $<

scanner.o: scanner_new.c str.h interpret.h scanner.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) $(OBJ) $(DEBOBJ) $(NAME) $(DEBNAME)
