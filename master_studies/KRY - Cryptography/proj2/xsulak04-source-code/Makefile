# Author			Ladislav Sulak (xsulak04@stud.fit.vutbr.cz)
# Date				2017-05
# File 				Makefile
# Description		Makefile for Project 2 in subject KRY (Cryptography),
#					Certificate Validator (Navazování SSL spojení a validace certifikátů knihovnou OpenSSL).


CPP=g++
CPP_FLAGS=-std=c++11 -Wall

RM=rm -rf
TAR=tar czvf

APP_NAME=validate_certs

APP_SRC_FILES= main.cpp
LIB_SRC_FILES= Client.cpp CryptoService.cpp
LIB_CPP_FLAGS= -lssl -lcrypto

PACKED = *.h *.cpp Makefile cert/*

all:
	$(CPP) $(CPP_FLAGS) -o $(APP_NAME) $(APP_SRC_FILES) $(LIB_SRC_FILES) $(LIB_CPP_FLAGS)

debug: CPP_FLAGS += -g -D_DEBUG
debug: all

clean:
	$(RM) $(APP_NAME) $(APP_OBJ_FILES)

pack: clean
	$(TAR) "xsulak04-source-code.tar.gz" $(PACKED)

%.o: %.cpp
	$(CPP) $(CPP_FLAGS) -c -o $@ $<

.PHONY: all pack
