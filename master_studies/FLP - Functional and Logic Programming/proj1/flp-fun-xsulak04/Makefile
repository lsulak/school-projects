#################################################################################
#
# Name of the project		rka-2-dka
# Author					Ladislav Sulak
# Login 					xsulak04
# Date						4/2016
# File 						Makefile
# Description				Makefile for project in Haskell for subject FLP 
#							(Funkcionální a logické programování)
#
#################################################################################

NAME=rka-2-dka

ZIPPED = Makefile FsmMain.hs FsmDataTypes.hs FsmProcessing.hs README

all:
	ghc --make FsmMain.hs -o $(NAME)

clean:
	rm -f $(NAME) *.hi *.o

zip: clean
	zip flp-fun-xsulak04.zip $(ZIPPED)
