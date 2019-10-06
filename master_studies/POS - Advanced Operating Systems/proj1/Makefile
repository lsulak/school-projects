# Author			Ladislav Sulak (xsulak04@stud.fit.vutbr.cz)
# Date				2017-04
# File 				Makefile
# Description		Makefile for Project 1 in subject POS (Advanced Operating Systems),
#					Ticket Algorithm.

# -std=c11

CPP=gcc
CPP_FLAGS=-std=c11 -g -Wall -pedantic -pthread -O 

RM=rm -f
TAR=tar czvf

APP_NAME=ticket_alg

APP_SRC_FILES= main.cpp

PACKED = *.c Makefile

all:
	$(CPP) $(CPP_FLAGS) -o $(APP_NAME) main.c

clean:
	$(RM) $(APP_NAME) core*

pack: clean
	$(TAR) xsulak04.tar.gz $(PACKED)

