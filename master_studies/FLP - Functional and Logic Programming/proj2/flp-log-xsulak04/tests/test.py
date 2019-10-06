#!/usr/bin/python
# -*- coding: utf-8 -*-

"""
Title:			Test file for project Rubik's Cube for subject FLP (Functional and Logic Programming), 
				FIT VUT in Brno.

Author: 		Ladislav Sulak,  xsulak04@stud.fit.vutbr.cz
Data: 			4/2016

Description:  	Testing tool for program written in Prolog, which solves Rubik's Cube by searching the state space.
				This script requieres no arguments, and you can change name of the input and output files 
				for prolog program (fINPUT, OUTPUT) and also tINPUT (text input) from Cube here:
					http://thecube.guru/online-3d-rubiks-cube/ 
"""

import os
import sys
import string

# Testing scenarios done with this app: http://thecube.guru/online-3d-rubiks-cube/
# For your tests, please follow this steps:
#	Scramble cube -> export -> copy & paste into variable tINPUT 
# They use different representation, so there is function 'convertInput' for converting
# This script will convert it to our representation and save the result into text file, 
# with name from variable fINPUT, and then executes rubikCube program with this file as an input parameter.
tINPUT = "7,7,5,4,2,2,4,2,2,2,3,6,3,3,6,3,3,6,4,4,7,4,4,2,4,4,2,4,5,5,6,5,7,6,5,7,2,2,3,5,6,5,6,6,5,7,7,3,7,7,3,5,6,3"

fINPUT = "input.txt" 	# This is for text input in proper format
OUTPUT = "output.txt"	# Output of the program, in testing scenarios together with list of moves at the end of txt file


def convertInput(inputText):
	inputTextTmp = ""
	inputText  = inputText.replace(",", "")

	for num in inputText:
		inputTextTmp += str(int(num) - 1)

	result = inputTextTmp[-9:-6] + "\n" + inputTextTmp[-6:-3] + "\n" + inputTextTmp[-3:] + "\n" + \
				inputTextTmp[35] + inputTextTmp[34] + inputTextTmp[33] + " " + inputTextTmp[26] + inputTextTmp[25] + inputTextTmp[24] + " " + \
				inputTextTmp[17] + inputTextTmp[16] + inputTextTmp[15] + " " + inputTextTmp[8] + inputTextTmp[7] + inputTextTmp[6] + "\n" + \
				inputTextTmp[32] + inputTextTmp[31] + inputTextTmp[30] + " " + inputTextTmp[23] + inputTextTmp[22] + inputTextTmp[21] + " " + \
				inputTextTmp[14] + inputTextTmp[13] + inputTextTmp[12] + " " + inputTextTmp[5] + inputTextTmp[4] + inputTextTmp[3] + "\n" + \
				inputTextTmp[29] + inputTextTmp[28] + inputTextTmp[27] + " " + inputTextTmp[20] + inputTextTmp[19] + inputTextTmp[18] + " " + \
				inputTextTmp[11] + inputTextTmp[10] + inputTextTmp[9]  + " " + inputTextTmp[2] + inputTextTmp[1] + inputTextTmp[0] + "\n" + \
				inputTextTmp[36:39] + "\n" + inputTextTmp[39:42] + "\n" + inputTextTmp[42:45] + "\n"

	return result


def main(argv):
	listOfMoves = []
	print("Converting input....\n")
	print(tINPUT)

	print("\n..to:\n")
	result = convertInput(tINPUT)

	# Print converted file to stdout as well as to file
	with open(fINPUT, "w") as f:
		for i in result:
			sys.stdout.write(i)
			f.write(i)

	print("\nRunning....\n")
	os.system("../flp16-log <" + fINPUT + " > " + OUTPUT)

	# rubikCube program can print steps in list [line 394: ,writeln(MoveSeq)].
	# For outputting this steps please uncomment it.
	# Find and print these steps to representation from app mentioned before
	with open(OUTPUT) as file:
		lines = file.readlines()
		for line in lines:
			if(line[0] == "["):
				listOfMoves = line[1:(len(line)-2)].replace(",", "") # Prolog list --> seq of steps
				break

	print("Solution:")
	cnt = 0

	# Print result as a steps. Cubes during processing are not interesting in our testing scenarios,
	# but are available in text file with name in variable OUTPUT
	for i in listOfMoves:
		if i.isalpha():
			sys.stdout.write(i.upper())
			sys.stdout.write(listOfMoves[cnt-1])
		cnt += 1

	print("\n\nCheck on http://thecube.guru/online-3d-rubiks-cube/")


if __name__ == "__main__":
	main(sys.argv[1:])
