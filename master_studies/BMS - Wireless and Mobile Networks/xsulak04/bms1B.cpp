/* 
 * Author			Ladislav Sulak
 * Contact			xsulak04@stud.fit.vutbr.cz
 * Date				30.10.2015
 *
 * File 			bms1B.cpp
 * Description		Project 1, part B - Demodulation of signal represented as a wav file with QPSK method.
 * 										Output will be printed as a binary data to a txt file.
 */

#include <cstdlib>
#include <cmath>  
#include <math.h>
#include <string.h>

#include "sndfile.hh"
#include <fstream>

using namespace std;

// Value taken from provided skeleton
#define AMPLITUDE (1.0 * 0x7F000000)

// Values on the Unit Circle will be handy for determining a shift in sinus.
#define shift00 (3.0/4.0)*M_PI
#define shift01 (1.0/4.0)*M_PI
#define shift10 (5.0/4.0)*M_PI
#define shift11 (7.0/4.0)*M_PI


// Place for saving stuff for demodulation process, such as buffer, number of frames or frequency.
typedef struct DemodulData {
   double frequency; 	// Frequency calculated as 1/samplerate.
   int frames; 			// Number of frames in the input wav file.
   int* buffer;			// Buffer which will contain sinus values for later processing.
} DemodulationData;


// Prototypes
char* getOutputFName(char* inputFName);
int demodulation (ofstream& outputFile, DemodulationData* demodulData);
int getSamplesPerSymbol(DemodulationData* demodulData);


int main (int argc, char** argv) 
{
	SndfileHandle inputFile = SndfileHandle(argv[1]);

	int sampleRate;			// Calculated from samplerate method.

	// Name of output file is the same as the input file with different extension [txt-wav].
	char* outputFileName = getOutputFName(argv[1]);
	if(outputFileName == NULL) {
		delete outputFileName;
		return 1;
	}

	DemodulationData* demodulData = new DemodulationData;

	if(argc == 1) {
		fprintf(stderr, "Error, no input argument. Please specify name of the input file. \n");
		return 1;
	}

	else if(argc > 2) {
		fprintf(stderr, "Error, number of input arguments has to be just 1. Please specify name of the input file. \n");
		return 1;
	}
	
	sampleRate = inputFile.samplerate();
	if(sampleRate == 0) {
		fprintf(stderr, "Error, unable to open wav file [ompty, not existing or broken]. \n");
		delete outputFileName;
		return 1;
	}
	
	demodulData->frequency = 1000.0 / sampleRate;
	demodulData->frames = inputFile.frames();
	
	demodulData->buffer = new int[demodulData->frames];
	inputFile.read(demodulData->buffer, demodulData->frames);

	ofstream outputFile(outputFileName);

	if(demodulation(outputFile, demodulData) != 0) {
		outputFile.close();
		delete [] demodulData->buffer;
		delete demodulData;
		delete outputFileName;
		return 1;
	}

	outputFile << "\n"; // Before EOF.
	outputFile.close();
	delete [] demodulData->buffer;
	delete demodulData;
	delete outputFileName;
	return 0;
}


//
// Function which determines name of the output file according to the first argument.
//
char* getOutputFName(char* inputFName)
{
	char* tmpOutFName = NULL;	// Name of output file is the same as the input file with different extension [txt-wav].
	char* tmpStrExtension = strrchr(inputFName, '.');	// For checking an extension of input file.
	int outputFileNameLen = strlen(inputFName);			// Length of the name of input file.

	if(tmpStrExtension == NULL or strcmp(tmpStrExtension, ".wav")) {
		fprintf(stderr, "Error, input file has to be wav file [*.wav]. \n");
		return NULL;
	}

	tmpOutFName = new char[outputFileNameLen + 1];

	for (int i = 0; i < outputFileNameLen - 3; i++)
		tmpOutFName[i] = inputFName[i];

	tmpOutFName[outputFileNameLen - 3] = 't'; // Change the extension from txt to wav
	tmpOutFName[outputFileNameLen - 2] = 'x';
	tmpOutFName[outputFileNameLen - 1] = 't';
	tmpOutFName[outputFileNameLen] = '\0';

	return tmpOutFName;
}


//
// Function for demodulation the input wav file. We always consider 2 consecutive values of sinus, because of accuracy - 
// It is rare, but there can occur 1 sinus value which belongs to 2 symbols.
//
int demodulation (ofstream& outputFile, DemodulationData* demodulData)
{
	double ourSinValue, ourSinValue2, shift = 0.0;

	int samples = getSamplesPerSymbol(demodulData);
	if (samples == -1) // Error in synchronization sequence.
		return -1;

	for (int i =  samples * 4; i < demodulData->frames * 2; i += samples) { // Begin after a synchronization sequence, iterate through each symbol
		// Every iteration tries to figure out the phase shift by calculating supposed value for the current value of sinus.
		for (int j = 0; j < 4; j++) {
			if (j == 0)
				shift = shift00;
			else if (j == 1)
				shift = shift01;
			else if (j == 2)
				shift = shift10;
			else if (j == 3)
				shift = shift11;

			ourSinValue = sin (i * 2.0 *  M_PI * demodulData->frequency + shift);
			ourSinValue2 = sin ((i + 1) * 2.0 *  M_PI * demodulData->frequency + shift);

			if ((abs(ourSinValue - (demodulData->buffer[i] / AMPLITUDE)) < 0.01) and // Match! Write to the file and move to the next symbol.
				 abs(ourSinValue2 - (demodulData->buffer[i+1] / AMPLITUDE)) < 0.01) {
					if (j == 0)
						outputFile << "00";
					else if (j == 1)
						outputFile << "01";
					else if (j == 2)
						outputFile << "10";
					else if (j == 3)
						outputFile << "11";

					break;
			}
			else {
				// End of wav file, there are many 0.0 values in the buffer, because we didn't know the exact number of samples.
				if (j == 3 and demodulData->buffer[i] == 0.0 and demodulData->buffer[i+1] == 0.0 and 
				demodulData->buffer[i+samples] == 0.0 and demodulData->buffer[i+samples+1] == 0.0)
					return 0;
			}
		}
	}
	
	return 0;
}


//
// Function for determining how many values represend 1 symbol on the Unit Circle. 
// This processing is based on a synchronization sequence 00110011. 
// The idea is to generate an'ideal' value with expected value and to compare it with the current one. 
// If they are very similar, we can assume that we are still on the same symbol, 
// otherwise there is change (another symbol), and we have number of iterations (samples per symbol)/
//
int getSamplesPerSymbol(DemodulationData* demodulData)
{
	double ourSinValue, idealSinValue;
	double diff = 0.0;
	double shift = 0.0;
	int samples = 0;

	for (int i = 0; i < 3; i++) {
		if(i == 0 or i == 2)
			shift = shift00; // symbol '00' - first or third iteration must have this phase shift - synch. seq. is known.
		else
			shift = shift11; // symbol '11' - second or fourth iteration must have this phase shift - synch. seq. is known.

		// If we want to calculate a number of values per each symbol, it is sufficient to iterate only once. 
		// But synchronization sequence can be corrupted, therefore we will check all 4 symbols if they have the same amount.
		for (int x = samples * i; x < demodulData->frames; x++) { 
			ourSinValue = demodulData->buffer[x] / AMPLITUDE; // Normalizing a sin value obtained from input wav file
			idealSinValue = sin(x * 2.0 *  M_PI * demodulData->frequency + shift); 
			
			diff = idealSinValue - ourSinValue;

			// In other words, there is sinus with different shift. Of course, value for comparison (0.01) can vary,
			// because if there is sinus with another shift, values are (should be) very different.
			if (abs(diff) > 0.01) {
				if(i == 0) {
					samples = x; // First iteration = we just found the amount of values.
					break;
				}
				
				if (samples == x - (samples) * i) // Next 3 iterations = checking if everything is correct. 
					break;
				else {
					fprintf(stderr, "Error, synchronization sequence is incorrect, maybe corrupted wav file? \n"); // Error, not all 4 symbols in synch. sequence have the same number of samples.
					return -1;
				}
			}
		}
	}

	if (samples == demodulData->frames) { // Means that we haven't found 4 symbols in synch. sequence.
		fprintf(stderr, "Error, synchronization sequence is incorrect, maybe corrupted wav file? \n");
		return -1;
	}
	return samples;
}
