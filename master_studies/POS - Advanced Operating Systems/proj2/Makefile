# Author			Ladislav Sulak (xsulak04@stud.fit.vutbr.cz)
# Date				2017-04
# File 				Makefile
# Description		Makefile for Project 2 in subject POS (Advanced Operating Systems),
#					Simple Shell.

CPP=gcc
CPP_FLAGS=-std=c11 -g -Wall -pedantic -pthread -O 

RM=rm -f
TAR=tar czvf

APP_NAME=simple_shell

APP_SRC_FILES= main.c

PACKED = *.c Makefile

all:
	$(CPP) $(CPP_FLAGS) -o $(APP_NAME) $(APP_SRC_FILES)

clean:
	$(RM) $(APP_NAME) core*

pack: clean
	$(TAR) xsulak04.tar.gz $(PACKED)
