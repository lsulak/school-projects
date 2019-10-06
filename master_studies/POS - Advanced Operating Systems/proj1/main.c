/**
 *	@author			Ladislav Sulak (xsulak04@stud.fit.vutbr.cz)
 *	
 *	login 			xsulak04			
 *
 *	@date			2017-04
 *
 *	@file 			main.c
 *
 *	@brief			POS (Advanced Operating Systems): Project 1 - Ticket Algorithm.
 *
 *	@details		The aim of this project is to create a simple program,
 *					which will execute and manage certain amount of threads.
 *					Those threads will be synchronized so that always only 1 will be in Critical Section (CS).
 *					Threads receives unique tickets and based on this order they are allowed to CS or not.
 */


#define _POSIX_C_SOURCE 199506L

#define _XOPEN_SOURCE 500
#define _XOPEN_SOURCE_EXTENDED 1

/*
#ifndef _REENTRANT
#define _REENTRANT

#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 600
*/

#include <stdio.h>
#include <stdint.h> 
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>

// Number ('id') of current ticket.
static int ticket = 0;
static int num_of_cs_accesses = 0;

// Number (id) of a current ticket which is going to critical section right now.
static int ticket_to_cs = 0;

// Static initialization, good for global synch. resources - use when manipulating and protecting shared resources (variables)
// BTW: _POSIX_THREAD_PRIO_INHERIT  - sometimes it can be useful, for function pthread_mutexattr_setprotocol()
pthread_mutex_t mutex_ticket=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_cs=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_ticket=PTHREAD_COND_INITIALIZER;  // fifo of waiting threads


/*
 *	The return value is uniq ticket number, which determines which ticket is going to be in critical section
 *	as a next. First ticket is 0, then 1, 2, ...
 */
int getticket (void)
{
	pthread_mutex_lock(&mutex_ticket);
	int curr_ticket = ticket++;
	pthread_mutex_unlock(&mutex_ticket);

	return curr_ticket;
}

/*
 *	Begin/in to the critical section, param 'aenter' is a number of attached ticket, so the result of function getticket(). 
 *	First thread with ticket 0 and there can be only 1 thread in CS simultaneously.
 */
void await (int aenter)
{
	pthread_mutex_lock(&mutex_cs);
	while(aenter != ticket_to_cs) 
		pthread_cond_wait(&cond_ticket, &mutex_cs); 
}


/*
 *	End/out from the critical section, thanks to this function, another thread can access the critical
 *	section via function await() with incremented ticket. 
 */
void advance (void) 
{
	++ticket_to_cs;
	pthread_cond_broadcast(&cond_ticket); 
	pthread_mutex_unlock(&mutex_cs);
}


/*
 * 	Function for printing help on standard output.
 */
void printHelp ()
{
	fprintf(stdout, "Description: \tProject 1 for subject POS - Ticket Algorithm\n");
	fprintf(stdout, "Author: \tLadislav Sulak <xsulak04@stud.fit.vutbr.cz\n");
	fprintf(stdout, "Date: \t\t2017-04\n");
	fprintf(stdout, "\n");
	fprintf(stdout, "Usage:\t./ticket_alg N M\n");
	fprintf(stdout, "Parameters:\n");
	fprintf(stdout, 
		"\t N - mandatory parameter, number of threads\n"
		"\t M - mandatory parameter, number of accesses to critical section\n");
	fprintf(stdout,   
		"\t-h - optional parameter for printing help message on stdout\n");
}


 
/*
	Local parameters, reentrant function(s). Newly created thread will start executing its flow by this function.
	Every use of global or static or dynamic vars from multiple threads has to be protected as Critical Section.
*/
static void* handle_thread_func (void* arg)
{
	const int id = *((int*)arg);
	int curr_ticket_m = 0;
	
	struct timeval curr_time; 
	struct timespec pause;
	pause.tv_sec = 0;
	gettimeofday(&curr_time, NULL);

	unsigned int seed = curr_time.tv_sec*(uint64_t)1000000+curr_time.tv_usec + id;
	
	while ((curr_ticket_m = getticket()) < num_of_cs_accesses) { /* Přidělení lístku */

		pause.tv_nsec = rand_r(&seed)%500000000L;
		nanosleep(&pause, NULL);	// 0 - 0.5 sec
		
		await(curr_ticket_m);	// Entering CS

		// pthread_t pthread_self(void); - identification of a current thread, but type pthread_t!
		// int pthread_equal(pthread_t t1, pthread_t t2); - the only operation on this type! Cannot use it for our purposes.

		fprintf(stdout, "%d\t(%d)\n", curr_ticket_m, id); // Identifikace id by měla být číslo vlákna v intervalu <1, N>, nie cez pthread_self()!
		fflush(stdout);

		advance();		// Leaving CS

		pause.tv_nsec = rand_r(&seed)%500000000L;
		nanosleep(&pause, NULL);	// 0 - 0.5 sec
	}
	return 0;
}

int main (int argc, char *argv[])
{
	int *stat = NULL;  
	int res = 0;

	pthread_attr_t attr;

	if (argc == 2 && (strncmp(argv[1], "-h", 2) == 0)) {
		printHelp();
		return 0;
	}
	else if (argc != 3) {
		fprintf(stderr, "Please provide 2 inputs arguments.\n");
		printHelp();
		return 1;
	}

	const int num_of_threads = atoi(argv[1]);
	pthread_t thread[num_of_threads];
	num_of_cs_accesses= atoi(argv[2]);
	
	int threadId[num_of_threads];

	if (num_of_threads == 0 || num_of_cs_accesses == 0) {
		fprintf(stderr, "The input arguments have to be > 0.\n");
		printHelp();
		return 1;
	}

	res = pthread_attr_init(&attr);
	if (res != 0) {
		fprintf(stderr, "An error during pthread_attr_init\n");
		return 1;
	}

	// PTHREAD_CREATE_JOINABLE (it is possible to wait for such a thread),
	// PTHREAD_CREATE_DETACHED (detached - lonely thread - default).
	// int pthread_detach(pthread_t thread) - not detached thread is detached by the use of this
	res = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE); // type of thread
	if (res != 0) {
		fprintf(stderr, "An error during pthread_attr_setdetachstate\n");
		return 1;
	}

	for (int i = 0; i < num_of_threads; i++) {
		threadId[i] = i+1;
		pthread_create(&thread[i], &attr, handle_thread_func, &threadId[i]);
		if (res != 0) {
			fprintf(stderr, "An error during pthread_create\n");
			return 1;
		}
	}

	res = pthread_attr_destroy(&attr);
	if (res != 0) {
		fprintf(stderr, "An error during pthread_attr_destroy\n");
		return 1;
	}

	for (int i = 0; i < num_of_threads; i++) {
		res = pthread_join(thread[i], (void *)&stat);  // waiting for thread's end
		if (res != 0) {
			fprintf(stderr, "An error during pthread_join\n");
			return 1;
		}
	}

	free(stat);
	return 0;
}
