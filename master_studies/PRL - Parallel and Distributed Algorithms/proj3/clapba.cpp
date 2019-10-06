/**
 *	@author		Ladislav Sulak (xsulak04@stud.fit.vutbr.cz)
 *	
 *	login 		xsulak04			
 *
 *	@date		4/2016
 *
 *  @file 		clapba.cpp
 *
 *	@brief		The third project for subject PRL - Carry Look Ahead Parallel Binary Adder. 
 *	@details	Carry Look Ahead Parallel Binary Adder is a paralell algorithm for 
 * 				addition of 2 binary numbers using the Open MPI library. 
 *				Numbers are placed in the file 'numbers'. 
 *				Please run with bash script ./test numberOfDigits
 *				The input numbers are added and outputted in format from specification 
 *				(each digit on one line together with proc ID).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <mpi.h>

/* Only for debugging purposes, if you want to see coloured logs, please set DEBUG to 1 */
#define RED		"\x1B[31m"
#define GREEN	"\x1B[32m"
#define RESET	"\033[0m"

#define DEBUG 				0
#define TIME_MEASUREMENT 	0

#if(DEBUG)
 	#define DEBUG_LOG(...)		fprintf(stderr, __VA_ARGS__)
 	#define DEBUG_LOG_SEND(...)	fprintf(stderr, RED); 	fprintf(stderr, __VA_ARGS__); fprintf(stderr, RESET)
 	#define DEBUG_LOG_RECV(...)	fprintf(stderr, GREEN); fprintf(stderr, __VA_ARGS__); fprintf(stderr, RESET)
#else
 	#define DEBUG_LOG(...)
	#define DEBUG_LOG_SEND(...)
	#define DEBUG_LOG_RECV(...)
#endif

#define INPUTFILE "numbers"

#define MASTER 	 0
#define TAG		 0
#define STOP 	-1

#define D_STOP 		0
#define D_PROPAGATE 1
#define D_GENERATE 	2

using namespace std;


/* Used in scan operation on initial D values. */
int scan_table[3][3] = 
{	/* STOP 		PROPAGATE 		GENERATE */
	{D_STOP, 		D_STOP, 		D_STOP,}, 	 /* STOP */
	{D_STOP, 		D_PROPAGATE, 	D_GENERATE,}, /* PROPAGATE */
	{D_GENERATE, 	D_GENERATE, 	D_GENERATE,}, /* GENERATE */
};

/* 
	Function for determining if a number a power of two - the input argument 
	represent number of digits in the input numbers. 
*/
bool isPowerOfTwo(int number) 
{
	while(number != 1) {
		if(number % 2 == 1) {
			return false;
		}
		number /= 2;
	}
	return true;
}


/* Master (proc with ID 0) distributes digits from input numbers to list processors. */
int valDistribution (int processCnt, int numberOfDigits, int listsRank)
{
	/* Flag indicating end of communication, processes will end afterwards. */
	bool endOfProcessing = false;

	/* For input file processing - numbers */
	string input;
	string num1, num2;
	int inputNumbersCnt = 0;
	int fillNum1, fillNum2;
	int numToSend;
	
	fstream fin;

	// Number of digits in input numbers has to be power of two
	if(!isPowerOfTwo(numberOfDigits)) {
		numToSend = STOP;
		for(int i = 1; i < processCnt; i++)
			MPI_Send(&numToSend, 1, MPI_INT, i, TAG, MPI_COMM_WORLD);
		fprintf(stderr, "The input argument has to be power of two.\n");
		return 1;
	}
	
	// Processing the input file
	fin.open(INPUTFILE, ios::in);

	while(getline(fin, input)) {
		inputNumbersCnt++;

		if(inputNumbersCnt == 1)
			num1 = input;
		else if(inputNumbersCnt == 2)
			num2 = input;	
	}

	fin.close();

	// There must be only 2 numbers
	if(inputNumbersCnt != 2) {
		fprintf(stderr, "An error, please provide just 2 input numbers.\n");
		endOfProcessing = true;
	}

	// Determining how many '0' will be added 
	fillNum1 = numberOfDigits - num1.length();
	fillNum2 = numberOfDigits - num2.length();

	// Filling '0' to the input numbers, if needed.
	if(fillNum1 >= 0)
		for(int i = 0; i < fillNum1; i++)
			num1 = "0" + num1;
	else {
		fprintf(stderr, "An error, the first input number has more digits than expected.\n");
		endOfProcessing = true;
	}

	if(fillNum2 >= 0)
		for(int i = 0; i < fillNum2; i++)
			num2 = "0" + num2;
	else {
		fprintf(stderr, "An error, the second input number has more digits than expected.\n");
		endOfProcessing = true;
	}

	// There has been an error - send STOP flag to every processor
	if(endOfProcessing) {
		numToSend = STOP;
		for(int i = 1; i < processCnt; i++)
			MPI_Send(&numToSend, 1, MPI_INT, i, TAG, MPI_COMM_WORLD);

		return 1;
	}

	// Only 1 digit and 1 processor
	if(processCnt == 1) {
		if( ((int)num1[0] - '0') == 1 and ((int)num2[0] - '0') == 1 ) {
			fprintf(stdout, "0:0\n");
			fprintf(stdout, "overflow\n");
		}
		else if( ((int)num1[0] - '0') == 0 and ((int)num2[0] - '0') == 0 ) {
			fprintf(stdout, "0:0\n");
		}
		else
			fprintf(stdout, "0:1\n");
		return 0;
	}

	// Send pair of digits from each input number to every list processor,
	// MSB will be list with the smallest ID, LSB with the last ID.
	for(int i = 0; i < numberOfDigits; i++) {
		numToSend = (int)num1[i] - '0';
		MPI_Send(&numToSend, 1, MPI_INT, listsRank, TAG, MPI_COMM_WORLD);

		numToSend = (int)num2[i] - '0';
		MPI_Send(&numToSend, 1, MPI_INT, listsRank, TAG, MPI_COMM_WORLD);
		listsRank++;	// Ranks of listsRank
	}

	DEBUG_LOG("%s\n", num1.c_str());
	DEBUG_LOG("%s\n", num2.c_str());
	return 0;
}


int main(int argc, char *argv[])
{
	int numberOfDigits = atoi(argv[1]);

	/* Number of processes */
	int processCnt;
	
	/* For saving ranks of processes */
	int myRank, listsRank, leftChildRank, rightChildRank, parentRank = 0;

	/* For saving numbers to send/receive */
	int num1, num2, numToSend;
	int leftChildNumber = 0;
	int rightChildNumber = 0;	// Values obtained from child nodes.
	int reduceValue = 0;
	double startTime, endTime;

	MPI_Status stat;
	FILE * pFile; // Time measurement results
	
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&processCnt);	// Number of processes
	MPI_Comm_rank(MPI_COMM_WORLD,&myRank);		// Rank (ID) of our process

	if(TIME_MEASUREMENT) {
		pFile = fopen("results.log", "a+");
		startTime  = MPI_Wtime(); // For measuring a time complexity
	}

	// If there are 4 numbers, the last 4 processors are lists in binary tree.
	// From: numbers * 2 -  1 = processors ==> (processors + 1) / 2 = numbers 
	// (but we are indexing from 0, so we need to decrement that).
	listsRank = ((processCnt + 1) / 2) - 1;

	if(myRank % 2 == 0)
		parentRank = (myRank - 2) / 2;
	else
		parentRank = (myRank - 1) / 2;

	leftChildRank = (2 * myRank) + 1;
	rightChildRank = (2 * myRank) + 2;

	// Master distributes all values from the input file
	// to processes, which in binary tree they represend lists.
	if(myRank == MASTER) {
		if(valDistribution(processCnt, numberOfDigits, listsRank) != 0) {
			MPI_Finalize();
			DEBUG_LOG("[%d] ends!\n", myRank);
			if(TIME_MEASUREMENT) fclose (pFile);
			return 1;
		}
	}

	if(processCnt == 1) {
		MPI_Finalize();
		DEBUG_LOG("[%d] ends\n", myRank);
		if(TIME_MEASUREMENT) fclose (pFile);
		return 0;
	}

	/****** INITIAL D field + UPSWEEP ******/

	// Lists just receives 2 values and based on them they send initial D-value
	if (myRank >= listsRank) {
		MPI_Recv(&num1, 1, MPI_INT, MASTER, TAG, MPI_COMM_WORLD, &stat);
		DEBUG_LOG_RECV("[%d] <- [%d]: %d\n", myRank, MASTER, num1);

		if(num1 == STOP) {
			MPI_Send(&num1, 1, MPI_INT, parentRank, TAG, MPI_COMM_WORLD);
			MPI_Finalize();
			DEBUG_LOG("[%d] ends!\n", myRank);
			if(TIME_MEASUREMENT) fclose (pFile);
			return 1;
		}
		else {
			MPI_Recv(&num2, 1, MPI_INT, MASTER, TAG, MPI_COMM_WORLD, &stat);
			DEBUG_LOG_RECV("[%d] <- [%d]: %d\n", myRank, MASTER, num2);
		}

		if(num1 == 1 and num2 == 1)
			numToSend = D_GENERATE;

		else if(num1 == 0 and num2 == 0)
			numToSend = D_STOP;

		else
			numToSend = D_PROPAGATE;

		// UPSWEEP part, lists send Initial D value
		MPI_Send(&numToSend, 1, MPI_INT, parentRank, TAG, MPI_COMM_WORLD);
		DEBUG_LOG_SEND("[%d] U-> [%d]: %d\n", myRank, parentRank, numToSend);
	}

	else {
		// UPSWEEP part for non-lists processors - receive two numbers from children and based on that
		// perform scan operation and send its result to the parent
		MPI_Recv(&leftChildNumber,  1, MPI_INT, leftChildRank, TAG, MPI_COMM_WORLD, &stat);
		DEBUG_LOG_RECV("[%d] U<- [%d]: %d\n", myRank, leftChildRank, leftChildNumber);

		MPI_Recv(&rightChildNumber, 1, MPI_INT, rightChildRank, TAG, MPI_COMM_WORLD, &stat);
		DEBUG_LOG_RECV("[%d] U<- [%d]: %d\n", myRank, rightChildRank, rightChildNumber);

		// If there was an error, send this flag to upper level
		if(leftChildNumber == STOP or rightChildNumber == STOP) {
			if (parentRank != MASTER) {
				numToSend = STOP;
				MPI_Send(&numToSend, 1, MPI_INT, parentRank, TAG, MPI_COMM_WORLD);
			}
			MPI_Finalize();
			DEBUG_LOG("[%d] ends!\n", myRank);
			if(TIME_MEASUREMENT) fclose (pFile);
			return 1;
		}
		
		numToSend = scan_table[leftChildNumber][rightChildNumber];
		if(myRank != MASTER) {
			MPI_Send(&numToSend, 1, MPI_INT, parentRank, TAG, MPI_COMM_WORLD);
			DEBUG_LOG_SEND("[%d] U-> [%d]: %d\n", myRank, parentRank, numToSend); 
		}
		else {
			// Only master remembers value after reduce pernamently, 
			// the other processors remember only to use in downsweep stage.
			reduceValue = numToSend;
			DEBUG_LOG("MASTER - REDUCE: %d from [%d] and [%d] \n" ,reduceValue, leftChildRank, rightChildRank);
		}
	}

	/****** DOWNSWEEP ******/

	// Lists just send its original D-value number to parent and receive number from parent (SCAN value) 
	if (myRank >= listsRank) {

		MPI_Send(&numToSend, 1, MPI_INT, parentRank, TAG, MPI_COMM_WORLD);
		DEBUG_LOG_SEND("[%d] D-> [%d]: %d\n", myRank, parentRank, numToSend);

		MPI_Recv(&numToSend,  1, MPI_INT, parentRank, TAG, MPI_COMM_WORLD, &stat);
		DEBUG_LOG_RECV("[%d] D<- [%d]: %d\n", myRank, parentRank, numToSend);

		// Shift right!
		if(myRank == listsRank) {
			MPI_Send(&numToSend, 1, MPI_INT, myRank+1, TAG, MPI_COMM_WORLD);
			MPI_Recv(&numToSend, 1, MPI_INT, MASTER, TAG, MPI_COMM_WORLD, &stat);
		}
		else if (myRank == processCnt -1) {
			MPI_Recv(&numToSend, 1, MPI_INT, myRank-1, TAG, MPI_COMM_WORLD, &stat);
		}
		else {
			MPI_Send(&numToSend, 1, MPI_INT, myRank+1, TAG, MPI_COMM_WORLD);
			MPI_Recv(&numToSend, 1, MPI_INT, myRank-1, TAG, MPI_COMM_WORLD, &stat);
		}

		if(numToSend == D_GENERATE) // There are not STOP, PROPAGATE and GENERATE values,
			numToSend = 1;			// but only 0 or 1 according to if there is generate or not - Carry field
		else
			numToSend = 0;

		// Shift left!
		if(myRank == listsRank) {
			// If is overflow - master just collects all values for printing in sorted order
			MPI_Send(&numToSend, 1, MPI_INT, MASTER, TAG, MPI_COMM_WORLD);

			MPI_Recv(&numToSend, 1, MPI_INT, myRank+1, TAG, MPI_COMM_WORLD, &stat);
		}
		else if (myRank == processCnt -1) {
			MPI_Send(&numToSend, 1, MPI_INT, myRank-1, TAG, MPI_COMM_WORLD);
			numToSend = 0;
		}
		else {
			MPI_Send(&numToSend, 1, MPI_INT, myRank-1, TAG, MPI_COMM_WORLD);
			MPI_Recv(&numToSend, 1, MPI_INT, myRank+1, TAG, MPI_COMM_WORLD, &stat);
		}
	}

	// Non-list processors send their value (from UPSWEEP) part to parent and receives a new value from DOWNSWEEP
	else {
		if(myRank == MASTER)
			numToSend = D_PROPAGATE; // Master has 'neutral' (initial) value - Propagate 
		else {
			MPI_Send(&numToSend, 1, MPI_INT, parentRank, TAG, MPI_COMM_WORLD);
			DEBUG_LOG_SEND("[%d] D-> [%d]: %d\n", myRank, parentRank, numToSend);
			
			MPI_Recv(&numToSend, 1, MPI_INT, parentRank, TAG, MPI_COMM_WORLD, &stat);
			DEBUG_LOG_RECV("[%d] D<- [%d]: %d\n", myRank, parentRank, numToSend);
		}

		MPI_Send(&numToSend, 1, MPI_INT, rightChildRank, TAG, MPI_COMM_WORLD);
		DEBUG_LOG_SEND("[%d] D-> [%d]: %d\n", myRank, rightChildRank, numToSend);

		MPI_Recv(&rightChildNumber,  1, MPI_INT, rightChildRank, TAG, MPI_COMM_WORLD, &stat);
		DEBUG_LOG_RECV("[%d] D<- [%d]: %d\n", myRank, rightChildRank, rightChildNumber);

		// Scan operation is not commutative, we have to calculate reversively,
		// from right to left, because we have LSB on highest proc ID 
		numToSend = scan_table[rightChildNumber][numToSend];
		MPI_Send(&numToSend, 1, MPI_INT, leftChildRank, TAG, MPI_COMM_WORLD);
		DEBUG_LOG_SEND("[%d] D-> [%d]: %d\n", myRank, leftChildRank, numToSend);

		if(myRank == MASTER) {
			MPI_Send(&reduceValue, 1, MPI_INT, listsRank, TAG, MPI_COMM_WORLD);
			DEBUG_LOG_SEND("[%d] SR!-> [%d]: %d\n", myRank, listsRank, reduceValue);
		}
	}

	/****** RESULTS calculating in lists. ******/

	// Result is sended to the MASTER and he prints sorted values
	if (myRank >= listsRank) {
		if(numToSend == 1){
			numToSend = (num1^num2) + 1;
			numToSend = numToSend % 2;
		}
		else 
			numToSend = num1^num2;  // XOR is great for this - there is '1' only if the values are different :-)

		MPI_Send(&numToSend, 1, MPI_INT, MASTER, TAG, MPI_COMM_WORLD);
		DEBUG_LOG_SEND("[%d] R-> [%d]: %d\n", myRank, MASTER, numToSend);
	}

	if (myRank == MASTER) {
		// Overflow flag, from the first lists processor
		MPI_Recv(&reduceValue, 1, MPI_INT, listsRank, TAG, MPI_COMM_WORLD, &stat);

		for(int i = listsRank; i < processCnt; i++) {
			MPI_Recv(&numToSend, 1, MPI_INT, i, TAG, MPI_COMM_WORLD, &stat);
			fprintf(stdout, "%d:%d\n", i, numToSend);
		}

		if(reduceValue == 1)
			fprintf(stdout, "overflow\n");
	}

	if(TIME_MEASUREMENT) {
		endTime  = MPI_Wtime();
		fprintf(pFile,"%f\n",endTime-startTime);
		fclose (pFile);
	}

	DEBUG_LOG("[%d] ends\n", myRank);
	MPI_Finalize(); 
	return 0;
}
