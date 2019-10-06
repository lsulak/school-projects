/* 
 * Author			Ladislav Sulak
 * Contact			xsulak04@stud.fit.vutbr.cz
 * Date				30.10.2015
 *
 * File 			bms1A.cpp
 * Description		Project 1, part A - Modulation of binary data (from txt file) with QPSK method.
 */

#include <cstdlib>
#include <math.h>
#include <string.h>

#include "sndfile.hh"
#include <fstream>

using namespace std;

#define CHANELS 1
#define FORMAT (SF_FORMAT_WAV | SF_FORMAT_PCM_24)

// Values taken from provided skeleton
#define AMPLITUDE (1.0 * 0x7F000000)

#define SAMPLE_RATE 18000
#define FREQ (1000.0 / SAMPLE_RATE)

// Sampling freq = 18 000. If bitrate 1200 b/s (1000 and more required), 
// then 1 bit = 1/1200 s, 1 symbol = 1/600s. 18 000 * (1/600) = 30 samples per each symbol.
#define valuesPerSymbol 30


// Values on the Unit Circle will be handy for shifting the sinus.
#define shift00 (3.0/4.0)*M_PI
#define shift01 (1.0/4.0)*M_PI
#define shift10 (5.0/4.0)*M_PI
#define shift11 (7.0/4.0)*M_PI


// Place for saving stuff for binary data, such as buffer and its length etc.
typedef struct BinData {
   long sizeOfBinData; 	// Amount of sequence of binary data in the input file - according to this value we will create buffer for modulation.
   long bufferLength; 	// Length of buffer to output, actually number of 'symbols * values' = total amount of sinus values
   int* buffer4sinus;	// Buffer for containing sinus values.
} BinaryData;


// Prototypes
char* getOutputFName(char* inputFName);
int modulation (ifstream& inputFileHandle, int* buffer4sinus);
int generateSin (int* buffer4sinus, int* x, int b1, int b2);


int main (int argc, char** argv) 
{
	SndfileHandle outputFile;

	// Name of output file is the same as the input file with different extension [txt-wav].
	char* outputFileName = getOutputFName(argv[1]);
	if(outputFileName == NULL) {
		delete [] outputFileName;
		return 1;
	}

	BinaryData* binData = new BinaryData;
	int ret = 0; 	// Return values.

	if(argc == 1) {
		fprintf(stderr, "Error, no input argument. Please specify name of the input file. \n");
		return 1;
	}

	else if(argc > 2) {
		fprintf(stderr, "Error, number of input arguments has to be just 1. Please specify name of the input file. \n");
		return 1;
	}
	
	// Number of symbols (bits) in input file needed for determining a buffer size
	ifstream inputFile(argv[1]);

	if (!inputFile.is_open()) {
		fprintf(stderr, "Error, unable to open txt file. \n");
		delete [] outputFileName;
		return 1;
	}

	inputFile.seekg(0, ios_base::end); // Move the position of the file handle to the end - for obtaining number of bits in file
	binData->sizeOfBinData = inputFile.tellg(); // Save the amount of characters.
	inputFile.seekg(0, ios_base::beg); // Move the position of the file handle back to the start

	// division by 2 - each symbol is represented by 2 bits
	// add 4 is for 4 symbols means sequence for demodulator and 
	// multiply by valuesPerSymbol means number of actual points returned by sinus function in sequence.
	binData->sizeOfBinData = (binData->sizeOfBinData/2 + 4) * valuesPerSymbol; 

	binData->buffer4sinus = new int[binData->sizeOfBinData];
	binData->bufferLength = modulation(inputFile, binData->buffer4sinus);

	if (binData->bufferLength > 0) { // No error during modulation
		outputFile = SndfileHandle(outputFileName, SFM_WRITE, FORMAT, CHANELS, SAMPLE_RATE);
		outputFile.write(binData->buffer4sinus, binData->bufferLength);
		ret = 0;
	}

	else if(binData->bufferLength == 0) {
		fprintf(stderr, "Input file is empty. \n");
		ret = 1;
	}

	else {
		fprintf(stderr, "Incomplete modulation, writing to file despite of it. \n"); // Impair amount of bits / incorrect value
		outputFile = SndfileHandle(outputFileName, SFM_WRITE, FORMAT, CHANELS, SAMPLE_RATE);
		outputFile.write(binData->buffer4sinus, binData->bufferLength);
		ret = 1;
	}

	delete [] binData->buffer4sinus;
	delete binData;
	delete [] outputFileName;

	return ret;
}


//
// Function which determines name of the output file according to the argument.
//
char* getOutputFName(char* inputFName)
{
	char* tmpOutFName = NULL;
	char* tmpStrExtension = strrchr(inputFName, '.');	// For checking an extension of input file.
	int outputFileNameLen = strlen(inputFName);			// Length of the name of input file.

	if(tmpStrExtension == NULL or strcmp(tmpStrExtension, ".txt")) {
		fprintf(stderr, "Error, input file has to be txt file [*.txt]. \n");
		return NULL;
	}

	tmpOutFName = new char[outputFileNameLen + 1];

	for (int i = 0; i < outputFileNameLen - 3; i++)
		tmpOutFName[i] = inputFName[i];

	tmpOutFName[outputFileNameLen - 3] = 'w'; // Change the extension from txt to wav
	tmpOutFName[outputFileNameLen - 2] = 'a';
	tmpOutFName[outputFileNameLen - 1] = 'v';
	tmpOutFName[outputFileNameLen ] = '\0';

	return tmpOutFName;
}


//
// Function for handling and processing input file. It calls function for generate sinus. 
//
int modulation (ifstream& inputFileHandle, int* buffer4sinus)
{
	int b1, b2;
	int index = 0; // Represends index in buffer and also 'x' in sinus function.

	// Generate synchronization sequence for demodulator
	generateSin(buffer4sinus, &index, '0', '0');
	generateSin(buffer4sinus, &index, '1', '1');
	generateSin(buffer4sinus, &index, '0', '0');
	generateSin(buffer4sinus, &index, '1', '1');

	while (1) {
		b1 = inputFileHandle.get();
		if(b1 == EOF)
			break;

		b2 = inputFileHandle.get();

		if (isspace(b1) and !isspace(b2)) { // Shift 
			b1 = b2;
			b2 = inputFileHandle.get();
			if(b2 == EOF) { 				// Impair amount of bits/characters in the input file.
				break;
			}
		}

		else if (!isspace(b1) and isspace(b2)) { // Get next character
			while(isspace(b2))
				b2 = inputFileHandle.get();
		}

		else if(isspace(b1) and isspace(b2))
			continue;

		if(generateSin(buffer4sinus, &index, b1, b2) != 0) {
			inputFileHandle.close();
			return index;
		}
	}
	inputFileHandle.close();
	return index;
}


//
// Function for generating sinus function based on input symbol (b1 and b2) which determines phase shift. 
//
int generateSin (int* buffer4sinus, int* x, int b1, int b2)
{
	double shift; 	// [a*sin(b*x+shift)], shift calculated from the Unit Circle, we can also map symbols in the reverse order.

	if(b2 == EOF) { // Impair amount of bits/characters in the input file.
		fprintf(stderr, "Bad/missing value in the input file occured, modulation ends. \n");
		return 1;
	}

	if(b1 == '0' and b2 == '0') {
		shift = shift00;
	}

	else if(b1 == '0' and b2 == '1') {
		shift = shift01;
	}

	else if(b1 == '1' and b2 == '0') {
		shift = shift10;
	}

	else if(b1 == '1' and b2 == '1') {
		shift = shift11;
	}

	else { // At least 1 of symbols is incorrect.
		fprintf(stderr, "Bad value in the input file occured, modulation endsss. \n"); 
		return 1;
	}

	for (int i = 0; i < valuesPerSymbol; i++) { // Need to fill buffer with multiple different values from period of each sinus. 
		buffer4sinus[*x] = AMPLITUDE * sin(FREQ * 2 *  M_PI * (*x) + shift);
		(*x)++;
	}

	return 0;
}
