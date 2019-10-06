/**
 *	@author		Ladislav Sulak (xsulak04@stud.fit.vutbr.cz)
 *	
 *	login 		xsulak04			
 *
 *	@date			3/2016
 *
 * @file 		mes.c
 *
 *	@brief		The second project for subject PRL - Minimum Extraction sort. 
 *	@details		Minimum Extraction sort paralell algorithm for sorting the input
 *					data using the Open MPI library. 
 *					Numbers are being generated from /dev/urandom. 
 *					Please run with bash script ./test numbersAmount processorsAmount
 *					The input sequence is sorted and outputted in the same format on stdout.
 */

#include <stdio.h>
#include <string.h>
#include <fstream>
#include <mpi.h>

/* Only for debugging purposes, if you want to see colourec logs, please set DEBUG to 1 */
#define RED			"\x1B[31m"
#define GREEN		"\x1B[32m"
#define RESET		"\033[0m"

#define DEBUG 		0

#if(DEBUG)
 	#define DEBUG_LOG(...)			fprintf(stderr, __VA_ARGS__)
 	#define DEBUG_LOG_SEND(...)	fprintf(stderr, RED); 	fprintf(stderr, __VA_ARGS__); fprintf(stderr, RESET)
 	#define DEBUG_LOG_RECV(...)	fprintf(stderr, GREEN); fprintf(stderr, __VA_ARGS__); fprintf(stderr, RESET)
#else
 	#define DEBUG_LOG(...)
	#define DEBUG_LOG_SEND(...)
	#define DEBUG_LOG_RECV(...)
#endif

#define MASTER 0
#define TAG 0
#define NaN -1
#define STOP -2
#define INPUTFILE "numbers"

using namespace std;


int main(int argc, char *argv[])
{
	/* For input file processing - numbers */
	int inputNumber;
	int inputNumbersCnt = 0;

	/* Number of processes */
	int processCnt;

	/* For saving ranks of processes */
	int myRank, listsRank, receiverRank, parentRank, leftChildRank, rightChildRank;

	/* For saving numbers to send/receive */
	int fromParent = 0, toParent = 0; // Value sended from a parent node or sent to parent node.
	int leftChildNumber = 0, rightChildNumber = 0;	// Values obtained from child nodes.
	int minNumber = 0, maxNumber = 0, minRank = 0, maxRank = 0;

	/* Flag indicating end of communication, processes will end afterwards. */
	bool endOfProcessing = false;

	MPI_Status stat;

	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&processCnt);	// Number of processes
	MPI_Comm_rank(MPI_COMM_WORLD,&myRank);			// Rank (ID) of our process

	// If there are 4 numbers, the last 4 processors are lists in binary tree.
	// From: numbers * 2 -  1 = processors ==> (processors + 1) / 2 = numbers 
	// (but we are indexing from 0, so we need to decrement that).
	listsRank = ((processCnt + 1) / 2) - 1;

	// Master distributes all values from the input file.
	// to processes, which in binary tree they represend lists.
	if(myRank == MASTER) {
		fstream fin;
		fin.open(INPUTFILE, ios::in);

		receiverRank = listsRank;

		/* Distribute all values to lists */
		while(fin.good()) {
			inputNumber = fin.get();

			if(!fin.good())
				break;

			cout << inputNumber << " ";

			if(processCnt == 1) {
				cout << endl << inputNumber;
				endOfProcessing = true;
			}
			else
				MPI_Send(&inputNumber, 1, MPI_INT, receiverRank, TAG, MPI_COMM_WORLD);

			receiverRank++; 		// Ranks of lists.
			inputNumbersCnt++; 	// The amount of numbers in the input file.
		}

		// Need to add NaN numbers (-1), because number of processors could be wrong sometimes.
		// It is always: processors = 2*inputNumbers - 1
		while (processCnt != ((2 * inputNumbersCnt) - 1)) {
			inputNumber = NaN;

			MPI_Send(&inputNumber, 1, MPI_INT, receiverRank, TAG, MPI_COMM_WORLD);
			inputNumbersCnt++;
			receiverRank++;
		}

		DEBUG_LOG("Together %d numbers.\n", inputNumbersCnt);
		cout << endl;
		fin.close();
	}
	
	// All lists processes receives a number.
	if(myRank >= listsRank && inputNumbersCnt != 1)
		MPI_Recv(&fromParent, 1, MPI_INT, MASTER, TAG, MPI_COMM_WORLD, &stat);

	/* Distribution ends, now processing! */
	while(!endOfProcessing) {
		/* List always commnunicate this way:
			* sends its value to the parent. At the very begin they have value from the MASTER.
			* receives another from the parent.
			* if parent sends STOP flag, list will end. 
		*/
		if(myRank >= listsRank) {

			if(myRank % 2 == 0)
				parentRank = (myRank - 2) / 2;
			else
				parentRank = (myRank - 1) / 2;

			DEBUG_LOG_SEND("[%d] -> %d\n", myRank, fromParent);

			MPI_Send(&fromParent, 1, MPI_INT, parentRank, TAG, MPI_COMM_WORLD);
			MPI_Recv(&fromParent, 1, MPI_INT, parentRank, TAG, MPI_COMM_WORLD, &stat);

			DEBUG_LOG_RECV("[%d] <- %d\n", myRank, fromParent);
			if(fromParent == STOP) {
				endOfProcessing = true;
			}
		}

		/* The other nodes including master. Communication is basically following: 
				* Node receives 2 numbers, from children. 
				
				* Compares these values and sends a MIN to the parent.
				
				* Receives a number from the parent and based on this value, a node will:
					- send a NaN to the child with an original MIN value ONLY if parent sends NaN,
					- send both values back to children, or
					- send a STOP flag to both children.
		*/
		else {
			/* Calculate a ranks of parent and both children. */
			if(myRank % 2 == 0)
				parentRank = (myRank - 2) / 2;
			else
				parentRank = (myRank - 1) / 2;

				leftChildRank = (2 * myRank) + 1;
				rightChildRank = (2 * myRank) + 2;

			/* Receives a number from left and right child. */
			MPI_Recv(&leftChildNumber,  1, MPI_INT, leftChildRank,  TAG, MPI_COMM_WORLD, &stat);
			MPI_Recv(&rightChildNumber, 1, MPI_INT, rightChildRank, TAG, MPI_COMM_WORLD, &stat);	

			DEBUG_LOG_RECV("[%d] <- %d a %d\n", myRank, leftChildNumber, rightChildNumber);

			/* Compare these 2 numbers and prepare the destination, MIN and MAX values.
				It the current Rank is MASTER (0), also prints a MIN value to STDOUT.
				There will be sorted values eventually. */
			if(leftChildNumber == NaN && rightChildNumber != NaN) {
				if(myRank == MASTER) {
					cout << rightChildNumber << endl;

					minNumber = NaN;
					maxNumber = NaN;
				}
				else {
					toParent = rightChildNumber;

					minNumber = rightChildNumber;
					maxNumber = leftChildNumber;
				}

				minRank = rightChildRank;
				maxRank = leftChildRank;
			}
			
			else if(leftChildNumber != NaN && rightChildNumber == NaN) {
				if(myRank == MASTER) {
					cout << leftChildNumber << endl;

					minNumber = NaN;
					maxNumber = NaN;
				}
				else {
					toParent = leftChildNumber;

					minNumber = leftChildNumber;
					maxNumber = rightChildNumber;
				}

				minRank = leftChildRank;
				maxRank = rightChildRank;
			}

			else if(leftChildNumber == NaN && rightChildNumber == NaN) {
				if(myRank == MASTER) {
					minNumber = STOP; // End MASTER and send a STOP flag to both children for ending.
					maxNumber = STOP; // They will send to their children etc. Everyone will finish.

					endOfProcessing = true; // MASTER will end as well.
				}
				else {
					toParent = NaN;

					minNumber = leftChildNumber;
					maxNumber = rightChildNumber;
				}

				minRank = leftChildRank;
				maxRank = rightChildRank;
			}

			else if(rightChildNumber >= leftChildNumber) {
				if(myRank == MASTER) {
					cout << leftChildNumber << endl;

					minNumber = NaN;
					maxNumber = rightChildNumber;
				}
				else {
					toParent = leftChildNumber;

					minNumber = leftChildNumber;
					maxNumber = rightChildNumber;
				}
				minRank = leftChildRank;
				maxRank = rightChildRank;
			}
			else if(rightChildNumber < leftChildNumber) {
				if(myRank == MASTER) {
					cout << rightChildNumber << endl;

				 	minNumber = NaN;
					maxNumber = leftChildNumber;
				}
				else {
				 	toParent = rightChildNumber;

				 	minNumber = rightChildNumber;
					maxNumber = leftChildNumber;
				}

			 	minRank = rightChildRank;
				maxRank = leftChildRank;
			}

			if (myRank != MASTER) {
				DEBUG_LOG_SEND("[%d] -> %d\n", myRank, toParent);

				MPI_Send(&toParent,	1, MPI_INT, parentRank, TAG, MPI_COMM_WORLD);
				MPI_Recv(&fromParent,1, MPI_INT, parentRank, TAG, MPI_COMM_WORLD, &stat);

				DEBUG_LOG_RECV("[%d] <- %d\n", myRank, fromParent);

				/* Parent send NaN (which means he has chosen our value as a MIN), 
					we will send NaN to the child with the original MIN value. */
				if(fromParent == NaN)
					minNumber = NaN;

				/* Send STOP flag to both children. Process will finish too. */
				else if(fromParent == STOP) {
					minNumber = STOP;
					maxNumber = STOP;
					endOfProcessing = true;
				}
			}
			MPI_Send(&minNumber,	1, MPI_INT, minRank, TAG, MPI_COMM_WORLD);
			MPI_Send(&maxNumber, 1, MPI_INT, maxRank, TAG, MPI_COMM_WORLD);

			DEBUG_LOG_SEND("[%d] -> %d a %d\n", myRank, minNumber, maxNumber);
		}
	}

	DEBUG_LOG("[%d] ends\n", myRank);
	MPI_Finalize(); 
	return 0;
}
