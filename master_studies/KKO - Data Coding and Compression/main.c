/**
 *	@author			Ladislav Sulak (xsulak04@stud.fit.vutbr.cz)
 *	
 *	login 			xsulak04			
 *
 *	@date			2/2016
 *
 *	@file 			main.c
 *
 *	@brief			Project KKO - GIF to BMP conversion.
 *	@details		Application which handles input and output files, 
 *					parameters and also calls a library function 'gif2bmp' 
 *					which serves for conversion itself.
 */


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "gif2bmp.h"


/* 
 * 	Function for printing help on standard output.
 */
void helpPrint()
{
	fprintf(stdout, "Description: Project for subject KKO - conversion GIF to BMP image format.\n");
	fprintf(stdout, "Author: Ladislav Sulak\n");
	fprintf(stdout, "Login: xsulak04\n");
	fprintf(stdout, "Datum: 2/2016\n");
	fprintf(stdout, "-------------\n");
	fprintf(stdout, "Parameters (all are optional):\n");
	fprintf(stdout, "\t-i <ifile> | -o <ofile> | -l <logfile>  "
		"\n\t\t-i must be followed by name of input file,"
		"\n\t\t-o must be followed by name of output file,"
		"\n\t\t-l must be followed by name of log file.\n\n");
	fprintf(stdout, "\t-h"   
		" \tFor printing help message on stdout.\n");
}


/* 
 * 	Main function firstly handles arguments and then it calls a function gif2bmp for conversion 
 *	of image in format GIF (supported GIF89a in LZW compression) to BMP (without any compression).
 *	It returns 0 on success and -1 otherwise.
 */
int main (int argc, char** argv) 
{
	// Name of files taken from input arguments.
	const char *inputFile = NULL;
	const char *outputFile = NULL;
	const char *logFile = NULL;

	int c = 0; 
	int result = 0;
	bool removeOutFile = false;

	tGIF2BMP gif2bmpStruct;

	FILE *iFilePtr, *oFilePtr, *logFilePtr; // Input, Output and Log file pointers.

	/* ------------ Arguments handling ------------ */
	while ((c = getopt(argc, argv, "ho:i:l:")) != -1) {

		switch (c) {
			case 'i':
				if(inputFile == NULL)
					inputFile = optarg;
				else {
					fprintf(stderr, "There was an error in input arguments - '-i' duplicity!\n");
					return -1;
				}
				break;

			case 'o':
				if(outputFile == NULL)
					outputFile = optarg;
				else {
					fprintf(stderr, "There was an error in input arguments - '-o' duplicity!\n");
					return -1;
				}
				break;

			case 'h':
				helpPrint();
				return 0;

			case 'l':
				if(logFile == NULL)
					logFile = optarg;
				else {
					fprintf(stderr, "There was an error in input arguments - '-l' duplicity!\n");
					return -1;
				}
				break;

			default:
				fprintf(stderr, "Usage: %s [-i <inputFile>] [-o <outputFile>] [-h] [-l <logfile>] \n",
				argv[0]);
				return -1;
		}
	}

	/* ------------ Open all files needed later ------------ */
	if(inputFile != NULL) {
		iFilePtr = fopen(inputFile,"rb");

		if(iFilePtr == NULL) {
			fprintf(stderr, "There was an error during opening an input file!\n");
			result = -1;
		}
	}
	else
		iFilePtr = stdin;

	if(outputFile != NULL) {
		oFilePtr = fopen(outputFile,"wb");
		if(oFilePtr == NULL) {
			fprintf(stderr, "There was an error during opening an output file!\n");
			result = -1;
		}
	}
	else
		oFilePtr = stdout;

	/* ------------ Conversion of GIF to BMP and creating a log ------------ */
	if(result != -1 && gif2bmp(&gif2bmpStruct, iFilePtr, oFilePtr) == 0) {
		if (logFile != NULL) {
			logFilePtr = fopen(logFile,"w");

			if(logFilePtr == NULL) {
				fprintf(stderr, "There was an error during opening a log file!\n");
				result = -1;
			}

			else {
				fprintf(logFilePtr, "login = xsulak04\n");
				fprintf(logFilePtr, "uncodedSize = %ld\n", gif2bmpStruct.bmpSize);
				fprintf(logFilePtr, "codedSize = %ld\n", gif2bmpStruct.gifSize);
			}
		}
	}

	else {
		result = -1; // If there was an error during gif2bmp conversion.
		removeOutFile = true;
	}
	
	/* ------------ Close all opened files  ------------ */
	if(inputFile != NULL and iFilePtr != NULL) // It's not stdin and file has been opened successfuly.
		fclose(iFilePtr);
	
	if(outputFile != NULL and oFilePtr != NULL) // It's not stdout and file has been opened successfuly.
		fclose(oFilePtr);

	if(logFile != NULL) // File has been opened successfuly.
		fclose(logFilePtr);

	if(removeOutFile == true && outputFile != NULL)
		remove(outputFile); // Cleanup - remove the output file.

	return result;
}
