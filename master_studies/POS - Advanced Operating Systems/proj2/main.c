/**
 *	@author			Ladislav Sulak (xsulak04@stud.fit.vutbr.cz)
 *	
 *	login 			xsulak04			
 *
 *	@date			2017-04
 *
 *	@file 			main.c
 *
 *	@brief			POS (Advanced Operating Systems): Project 2 - Shell.
 *
 *	@details		The aim of this project is to create a simple shell,
 *					which will execute programs with their parameters.
 *					There will be 2 threads, one for parsing and obtaining input,
 *					and the second one for executing commands.
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
#include <stdlib.h>
#include <stdint.h> 
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <pthread.h>
#include <fcntl.h> 
#include <unistd.h>
#include <signal.h>
#include <stdarg.h>

const int STD_IN_DESC = 0;
const int BUFF_LEN = 513;

typedef struct {
	char* argv[256];
	char* in_file; 
	char* out_file;
	
	int argc;
	int pid;
	bool background;
	pthread_mutex_t mutex_process;
} t_process_ctx;

t_process_ctx* process_ctx = NULL;

typedef enum {    
    START,
    BEGIN_ARG,
    ARG,
    BEGIN_REDIR_I,
    BEGIN_REDIR_O,
    REDIRECT_I,
    REDIRECT_O,
    BACKGROUND
} t_fsm_state;


pthread_mutex_t mutex;
pthread_cond_t condition;

static char* read_buff;


/*
 * 	Function for handling SIGINT signal, it passes interrupt to foreground process.
 */
void sighandler_int (int signum)
{
	if (process_ctx->pid != 0) {
		fprintf (stdout, "Killing process %u.\n", process_ctx->pid);
		kill (process_ctx->pid, SIGINT);
	}
}

/*
 * 	Function for handling SIGCHILD signal, it waits for completion of process.
 */
void sighandler_child (int signum)
{
	int return_code = 0;
	pid_t child_pid;

	if ((child_pid = wait (&return_code)) < 0)
		return;

	pthread_mutex_lock (&(process_ctx->mutex_process));
	if (child_pid == process_ctx->pid) {
		pthread_mutex_unlock (&(process_ctx->mutex_process));
		return;
	}
	pthread_mutex_unlock (&(process_ctx->mutex_process));

	fprintf (stdout, "Background process %u done.\n", child_pid);
}

/*
 * 	Function which basically represends monitor, the entry.
 */
void buff_wait ()
{
	pthread_mutex_lock (&mutex);
	pthread_cond_wait (&condition, &mutex);
	pthread_mutex_unlock (&mutex);
}

/*
 * 	Function which basically represends monitor, the exit.
 */
void buff_signal ()
{
	pthread_mutex_lock (&mutex);
	pthread_cond_signal (&condition); 
	pthread_mutex_unlock (&mutex);
}

/*
 * 	Function for printing help on standard output.
 */
void printHelp ()
{
	fprintf (stdout, "Description: \tProject 2 for subject POS - Simple Shell\n");
	fprintf (stdout, "Author: \tLadislav Sulak <xsulak04@stud.fit.vutbr.cz\n");
	fprintf (stdout, "Date: \t\t2017-04\n");
	fprintf (stdout, "\n");
	fprintf (stdout, "Usage:\t./simple_shell\n");
	fprintf (stdout, "Parameters:\n");
	fprintf (stdout,   
		"\t-h - optional parameter for printing help message on stdout\n");
}

/*
 * 	Function which reads maximum BUFF_LEN characters from STDIN and the result is in parameter line.
 */
int read_line (char* line) 
{
	int res = 0;
	int read_chars = read (STD_IN_DESC, line, BUFF_LEN);

	if (read_chars >= BUFF_LEN)
	{
		char c;
		while (c != '\n') 
			read (STD_IN_DESC, &c, 1);

		fprintf (stderr, "Too long input, it should be lower than 512 characters\n");
		res = -1;
	}
	else if (read_chars == 0)
	{
		res = 1;
	}
	else if (read_chars < 0)
	{
		fprintf (stderr, "An error during read()\n");
		res = -1;
	}
	else
		line[read_chars] = '\0';

	return res;
}

/*
 * Function runs with thread which processes STDIN and prints prompt. 
 */
static void* read_input_thread_func (void* arg)
{
	bool exit = false;
	while (1)
	{
		char* line = malloc (BUFF_LEN * sizeof(char)); 
		memset (line, 0, BUFF_LEN);
		
		fprintf (stdout, "~$ ");
		fflush (stdout);

		int res = read_line (line);
		if (res < 0) 
		{
			free (line);
			continue;  // long input or error, just skipping such input
		}
		else if (res == 1)
		{
			free (line);
			break;
		}

		if (strcmp(line, "exit\n") == 0)
			exit = true;

		read_buff = line;
		
		buff_signal ();  // buffer is ready for processing and execution its commands
		buff_wait ();    // wait for next input reading

		free (line);

		if (exit)
			break;
	}

	return 0;
}

/*
 * Function processes already read input from terminal.
 * Our simple shell supports redirection STDIN/STDOUT -> file
 * and the process can run in background.
 *
 * This processing is implemented as finite state machine.
 * If there is an error, there will be information on stderr and such input will be skipped.
 */
bool process_params ()
{
	char c;
	int tmp_len = 0;
	char* tmp_arg = NULL;
	t_fsm_state state = START;

	while (*read_buff != '\0') {

		c = *(read_buff++);

		switch(state) 
		{
			case START:
			{
				if (isspace(c))
					break;

				else if (c == '&' || c == '<' || c == '>') {
					fprintf (stderr, "There has to be a program first.\n");
					return false;
				}

				else {
					tmp_arg = read_buff;
					tmp_len++;
					state = ARG;
					break;
				}
			}
			case ARG:
			{
				if (isspace(c))
					state = BEGIN_ARG;

				else if (c == '&') {
					process_ctx->background = true;
					state = BACKGROUND;
				}

				else if (c == '<')
					state = BEGIN_REDIR_I;

				else if (c == '>')
					state = BEGIN_REDIR_O;

				else {
					tmp_arg++;
					tmp_len++;
					break;
				}

				*tmp_arg = '\0';
				process_ctx->argv[process_ctx->argc] = tmp_arg-tmp_len;
				process_ctx->argc++;
				tmp_len = 0;
				tmp_arg = read_buff;
				break;
			}
			case BEGIN_ARG:
			{
				if (isspace(c))
					break;

				else if (c == '&') {
					process_ctx->background = true;
					state = BACKGROUND;
					break;
				}

				else if (c == '<') {
					state = BEGIN_REDIR_I;
					break;
				}

				else if (c == '>') {
					state = BEGIN_REDIR_O;
					break;
				}

				else {
					tmp_len = 1;
					tmp_arg = read_buff;
					state = ARG;
					break;
				}
			}

			case BACKGROUND:
			{
				if (isspace(c))
					break;

				else {
					fprintf (stderr, "In this simple shell, symbol '&' can ");
					fprintf (stderr,	"be used only during putting program to ");
					fprintf (stderr, "background, so on the end of sequence.\n");
					return false;
				}
			}

			case BEGIN_REDIR_I:
			{
				if (isspace(c))
					break;

				else if (c == '&' || c == '<' || c == '>') {
					fprintf (stderr, "There has to be a file to redirect.\n");
					return false;
				}

				else {
					tmp_len = 1;
					tmp_arg = read_buff;
					state = REDIRECT_I;
					break;
				}
			}

			case REDIRECT_I:
			{
				if (isspace(c)) {
					*tmp_arg = '\0';
					process_ctx->in_file = tmp_arg-tmp_len;
					tmp_len = 0;
					tmp_arg = read_buff;
				}

				else if (c == '<') {
					fprintf(stderr, "Bad syntax, you should provide input file.\n");
					return false;
				}

				else if (c == '&') {
					if (process_ctx->in_file == NULL) {
						fprintf (stderr, "Bad syntax, you should provide input file.\n");
						return false;
					}
					process_ctx->background = true;
					state = BACKGROUND;
					break;
				}

				else if (c == '>') {
					if (process_ctx->in_file == NULL || process_ctx->out_file != NULL) {
						fprintf (stderr, "Bad syntax, you should provide input file or use 1 type of redirection only once.\n");
						return false;
					}
					state = BEGIN_REDIR_O;
					break;
				}

				else {
					tmp_arg++;
					tmp_len++;
					break;
				}
			}

			case BEGIN_REDIR_O:
			{
				if (isspace(c))
					break;

				else if (c == '&' || c == '<' || c == '>') {
					fprintf (stderr, "There has to be a file to redirect.\n");
					return false;
				}

				else {
					tmp_len = 1;
					tmp_arg = read_buff;
					state = REDIRECT_O;
					break;
				}
			}

			case REDIRECT_O:
			{
				if (isspace(c)) {
					*tmp_arg = '\0';
					process_ctx->out_file = tmp_arg-tmp_len;
					tmp_len = 0;
					tmp_arg = read_buff;
				}

				else if (c == '>') {
					fprintf(stderr, "Bad syntax, you should provide input file.\n");
					return false;
				}

				else if (c == '&') {
					if (process_ctx->in_file == NULL) {
						fprintf (stderr, "Bad syntax, you should provide input file.\n");
						return false;
					}
					process_ctx->background = true;
					state = BACKGROUND;
					break;
				}

				else if (c == '<') {
					if (process_ctx->out_file == NULL || process_ctx->in_file != NULL) {
						fprintf (stderr, "Bad syntax, you should provide input file or use 1 type of redirection only once.\n");
						return false;
					}
					state = BEGIN_REDIR_I;
					break;
				}

				else {
					tmp_arg++;
					tmp_len++;
					break;
				}
			}
		}
	}
	return true;
}

/*
 * Function clears (not frees from memory) the process context structure (process_ctx).
 */
void clear_process_ctx ()
{
	for (int i=0; i < process_ctx->argc; i++)
		process_ctx->argv[i] = NULL;

	*process_ctx->argv = NULL;

	process_ctx->in_file = NULL;
	process_ctx->out_file = NULL;
	process_ctx->argc = 0;

	process_ctx->pid = 0;
	process_ctx->background = false;
}

/*
 * Function executes a command as a forked process.
 */
int execute_cmd ()
{
	pid_t  pid;

	if ((pid = fork()) < 0) {
		perror("fork");
		return 1;
	}
	else if (pid == 0)  // child
	{
		int input_fd = 0;
		int output_fd = 0;

		if (process_ctx->out_file != NULL) {
			if ((output_fd = open(process_ctx->out_file, O_CREAT|O_APPEND|O_TRUNC|O_WRONLY, 0644)) < 0)
				perror("open");

			if (dup2(output_fd, STDOUT_FILENO) < 0)
				perror("dup2");  // to the stdout
		}

		if (process_ctx->in_file != NULL) {
			if ((input_fd = open (process_ctx->in_file, O_RDONLY)) < 0) 
				perror("open"); // error, we will use stdin

			if (dup2(input_fd, STDIN_FILENO) < 0)
				perror("dup2");
		}

		if (execvp (*process_ctx->argv, process_ctx->argv) < 0)
			perror("execvp");

		if (process_ctx->out_file != NULL)
			if (close(output_fd)) perror("close");

		if (process_ctx->in_file != NULL)
			if (close(input_fd)) perror("close");

		clear_process_ctx ();
		exit(1);
	}
	else  // parent
	{
		if (!process_ctx->background) {
			process_ctx->pid = pid;
			waitpid (process_ctx->pid, NULL, 0);
			process_ctx->pid = 0;
		}
	}
	return 0;
}

/*
 * Function runs with thread which performs execution of read command with its parameters. 
 */
static void* exec_commands_thread_func (void* arg)
{
	while (1) 
	{
		process_ctx->in_file = NULL;
		process_ctx->out_file = NULL;
		process_ctx->argc = 0;
		process_ctx->background = false;

		buff_wait ();  // wait for the data
		
		if (strcmp (read_buff, "exit\n") == 0) {
			clear_process_ctx ();
			buff_signal ();
			break;
		}

		if (process_params ()) {
			if (execute_cmd () != 0) {  // executes only if a syntax of our shell is supported
				clear_process_ctx ();
				buff_signal ();
				break;
			}
		}
		clear_process_ctx ();
		buff_signal (); // data are processed
	}
	return NULL;
}
 

int main (int argc, char *argv[])
{
	int res = 0;
	int *stat = NULL;  

	// SIGINT and SIGCHLD handler
	struct sigaction act_int, act_child;

	act_int.sa_handler = &sighandler_int;
	act_int.sa_flags = 0;
	if (sigemptyset(&act_int.sa_mask) != 0) {
		perror("sigemptyset");
		return -1;
	}

	act_child.sa_handler = &sighandler_child;
	act_child.sa_flags = 0; 
	if (sigemptyset(&act_child.sa_mask) != 0) {
		perror("sigemptyset");
		return -1;
	}

	if (sigaction(SIGCHLD, &act_child, NULL) != 0) {
		perror("sigaction");
		return -1;
	}

	if (sigaction(SIGINT, &act_int, NULL) != 0) {
		perror("sigaction");
		return -1;
	}

	read_buff = NULL;
	process_ctx = (t_process_ctx*)malloc (sizeof(t_process_ctx));
	pthread_mutex_init (&process_ctx->mutex_process, NULL);
	
	pthread_mutex_init (&mutex, NULL);
	pthread_cond_init (&condition, NULL);

	pthread_attr_t attr;

	if (argc == 2 && (strncmp(argv[1], "-h", 2) == 0)) {
		printHelp();
		return 0;
	}

	pthread_t thread_read_input, thread_exec_commands;
	
	res = pthread_attr_init (&attr);
	if (res != 0) {
		fprintf (stderr, "An error during pthread_attr_init\n");
		return 1;
	}

	// PTHREAD_CREATE_JOINABLE (it is possible to wait for such a thread),
	// PTHREAD_CREATE_DETACHED (detached - lonely thread - default).
	// int pthread_detach(pthread_t thread) - not detached thread is detached by the use of this
	res = pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_JOINABLE); // type of thread
	if (res != 0) {
		fprintf (stderr, "An error during pthread_attr_setdetachstate\n");
		return 1;
	}

	pthread_create (&thread_read_input, &attr, read_input_thread_func, NULL);
	if (res != 0) {
		fprintf (stderr, "An error during pthread_create\n");
		return 1;
	}

	pthread_create (&thread_exec_commands, &attr, exec_commands_thread_func, NULL);
	if (res != 0) {
		fprintf (stderr, "An error during pthread_create\n");
		return 1;
	}

	res = pthread_attr_destroy (&attr);
	if (res != 0) {
		fprintf (stderr, "An error during pthread_attr_destroy\n");
		return 1;
	}

	res = pthread_join (thread_read_input, (void *)&stat);  // waiting for thread's end
	if (res != 0) {
		fprintf (stderr, "An error during pthread_join\n");
		return 1;
	}

	res = pthread_join (thread_exec_commands, (void *)&stat);  // waiting for thread's end
	if (res != 0) {
		fprintf (stderr, "An error during pthread_join\n");
		return 1;
	}

	pthread_mutex_destroy (&mutex);
	pthread_cond_destroy (&condition);

	free (stat);
	pthread_mutex_destroy (&process_ctx->mutex_process);

	clear_process_ctx ();
	free (process_ctx);
	return 0;
}
