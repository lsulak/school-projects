#!/bin/bash

# @author	Ladislav Sulak (xsulak04@stud.fit.vutbr.cz)
#
# @login 	xsulak04			
#
# @date		4/2016
#
# @file 	test.sh
#
# @brief	The third project for subject PRL - Carry Look Ahead Parallel Binary Adder. 
# @details	Carry Look Ahead Parallel Binary Adder is a paralell algorithm for 
# 			addition of 2 binary numbers using the Open MPI library. 
#			Numbers are placed in the file 'numbers'. 
#			Please run with bash script ./test numberOfDigits
#			The input numbers are added and outputted in format from specification 
#			(each digit on one line together with proc ID).

if [ $# -ne 1 ]; then
	echo "Please provide 1 argument, \
a number of bits in the input numbers for computing."
	exit 1
fi;

if [[ $1 =~ [0-9]+$ ]]; then
	if [ $1 -ge 0 ]; then
		digits=$1
		processors=$((($digits * 2) - 1));  # Binary tree - the number of digits in the input number (1 pair of digits for 1 processor). 
	else
		echo "The input argument has to be potitive number."
		exit 1
	fi
else
	echo "The input argument must be integer."
    exit 1
fi;

mpic++ -o clapba clapba.cpp # --prefix /usr/local/share/OpenMPI on merlin
mpirun -np $processors clapba $digits # --prefix /usr/local/share/OpenMPI on merlin
