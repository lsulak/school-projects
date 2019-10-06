# Author			Ladislav Sulak (xsulak04@stud.fit.vutbr.cz)
# Date				2017-04
# File 				Makefile
# Description		Makefile for Project 1 in subject KRY (Cryptography),
#					Secure Channel (Tunel pro bezpečné zasílání zpráv).


CPP=g++
CPP_FLAGS=-std=c++11 -Wall

RM=rm -rf
TAR=tar czvf

APP_NAME=sec_channel

APP_SRC_FILES= main.cpp
LIB_SRC_FILES= Client.cpp Server.cpp
LIB_CPP_FLAGS= -lssl -lcrypto -lgmp -lm

PACKED = *.h *.cpp Makefile logs tests/* services/* doc.pdf

all:
	$(CPP) $(CPP_FLAGS) -o $(APP_NAME) $(APP_SRC_FILES) $(LIB_SRC_FILES) $(LIB_CPP_FLAGS)

debug: CPP_FLAGS += -g -D_DEBUG
debug: all

clean:
	$(RM) $(APP_NAME) $(APP_OBJ_FILES) logs/*

pack: clean
	$(TAR) xsulak04.tar.gz $(PACKED)

%.o: %.cpp
	$(CPP) $(CPP_FLAGS) -c -o $@ $<

.PHONY: all pack
