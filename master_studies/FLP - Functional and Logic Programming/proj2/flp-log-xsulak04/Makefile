#################################################################################
#
# Name of the project		Rubikova kostka
# Author					Ladislav Sulak
# Login 					xsulak04
# Date						4/2016
# File 						Makefile
# Description				Makefile for project in Prolog for subject FLP 
#							(Functional and Logic Programming)
#
#################################################################################

NAME=flp16-log

ZIPPED = Makefile rubikCube.pl README tests/* alg/rubikCubeAlg.pl

all:
	swipl -q -g main -o $(NAME) -c rubikCube.pl 

clean:
	rm -f $(NAME) *.out

zip: clean
	zip flp-log-xsulak04.zip $(ZIPPED)
