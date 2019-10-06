#include <string.h>
#include <iostream>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/stat.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#include <dirent.h>
#include <cerrno>

#include <sstream>
#include <fstream>
#include <time.h>

/* Author: Ladislav Sulak
   Login:  xsulak04
   Date:   4/2014
   mail:   xsulak04@stud.fit.vutbr.cz
   
   Run program:   ./server -d limit -p port
   or       	  ./server -p port -d limit
   Makefile : make, make client, make server, make tar, make clean
Communication protokol is based on form #MESSAGE#\r\n, but there are exceptions.

Runtime: (server runs, client has just connected)
	server sends welcome msg, it contains list of availible files , last symbol is '@'
	client sends msg:   '#DOWNLOAD:FILE#\r\n'  , in FILE there is requiered file to download
	server sends msg:   '#DCONFIRMED\r\n'   - confirm prevorious msg
	server sends msg:   '#SIZE!#'   - client must know exact size of file 
	client sends msg:   '#OK#\r\n'  - client is ready for receiving binary data
	server sends binary data, character by character, and client receives this data also character by character
	client sends msg:   '#QUIT#\r\n'   - transfer was complete, client can ends
	server sends msg:   '#QCONFIRMED#\r\n   - confirm prevorious msg, close client socket, communication ends
*/

//Function for checking if char* contains digits only
int digitCheck(char * string)
{
	for(unsigned int i = 0; i < strlen(string); i++)
	{
   		if(!isdigit(string[i]))
   		{
   			fprintf(stderr, "Parameter has wrong format!\n");
        	return 1;
        }
	}
	return 0;
}

//Function for parsing an input parameter - it can be in form ./server -p port -d limit or ./server -d limit -p port
int parameter_parsing(int argc, char *argv[], int* port, int* limit)
{   
    if(argc == 5)
    {
		if(digitCheck(argv[2]) != 0) //my function for checking if char* contains digits only
			return 1;
		
		if(digitCheck(argv[4]) != 0)
			return 1;
    
    	if(strcmp(argv[1], "-d") == 0 && strcmp(argv[3], "-p") == 0)
    	{
    		*limit = atoi(argv[2]); //need to save limit and port numbers into variable
    		*port = atoi(argv[4]);
		}
		else if(strcmp(argv[3], "-d") == 0 && strcmp(argv[1], "-p") == 0)
		{
			*port = atoi(argv[2]);  //need to save limit and port numbers into variable
			*limit = atoi(argv[4]);
		}
		else
		{
    		fprintf(stderr, "Parameter has wrong format!\n");
    		return 1;
		}
	}
	else
    {
        fprintf(stderr, "Too few or too much parameters!\n");
        return 1;
    }
    return 0;
}

//Function for binding server socket, and listening for connections
int connection(int *server_socket, sockaddr_in *server, socklen_t *sin_len, int port)
{

	if((*server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		fprintf(stderr, "Couldn't create socket!\n");
		return 1;
	}

	server->sin_family = AF_INET;
	server->sin_port = htons(port); 	
	server->sin_addr.s_addr = INADDR_ANY;
	
	if(bind(*server_socket, (struct sockaddr *)&(*server), sizeof(*server)) < 0)
	{
		fprintf(stderr, "Binding socket have failed!\n");
		return 1;		
	}
	//Listen 1 client
	if(listen(*server_socket, 1) < 0)
	{
		fprintf(stderr, "Listening on socket have failed!\n");
		return 1;				
	}

	*sin_len = sizeof(*server);
	return 0;
}

//Function for sending a size of file to client in form '#SIZE!#'
int sendLength(int socket, std::string fileName)
{
	struct stat fileLenght;
	std::string size ("");
	std::stringstream tmp;
	//find a length of binary file
	if(stat(fileName.c_str(), &fileLenght) != 0)
		return -1;
	//save length to temporary variable
	tmp.clear();
   	tmp.str("");
	tmp << fileLenght.st_size;
	size = "#" + tmp.str() + "!#"; //final form of message
	
	if(send(socket, size.c_str() , size.length(), 0) < 0) //msg is send
		return -1;

	return fileLenght.st_size; //function returns size of file
}

int sendFile(int socket, std::string fileName, int child_pid, int limit)
{
	std::ifstream fileStream(fileName.c_str());//, std::ios::in | std::ios::binary);
	
	char * fileBinary = NULL;
	long size = 0;
	//Variables for calculating downloaded time
	double tstart, tstop, ttime;
	double timeDownload = 0.0;
	long timeM = 0;
	//send length of file to client
	size = sendLength(socket, fileName);
	if(size < 0)
		return 1;
	//create a char[size] variable for whole binary file
	fileBinary = new char [size];
	fileStream.read(fileBinary,size);
	if(!fileStream)
	{
		fileStream.clear();
		return 1;
	}
	//Client is ready for incoming binary data if msg '#OK#\r\n' is received
 	std::string ok_msg("");
    char tmp2[100]; //tmp buffer
    if(recv(socket, tmp2, 100, 0) == -1)
        return 1;

    ok_msg += tmp2;

    if(strcmp(ok_msg.c_str(),"#OK#\r\n") != 0) 
        return 1;
    //Need to calculate the real time of downloading
    tstart = (double)clock()/CLOCKS_PER_SEC;
	
	//Sending binary data. Thats all! :) It is sending char by char
	for(int i=0; i < size; i++)
		if(send(socket, &fileBinary[i], 1, 0) == -1)
			return 1;

	tstop = (double)clock()/CLOCKS_PER_SEC;
	ttime= tstop-tstart; //'ttime' represends time of sending file (downloading)
	//Calculate time of downloading with download limit, and convert to miliseconds
	timeDownload = ((double)size/1000)/(double)limit;
	timeM = (timeDownload*1000000L - ttime*1000000L);
	if(timeDownload > ttime)
	{
		usleep(timeM); //Sleep process in miliseconds, so download time now fit
		fprintf(stdout, "Client:%d\tDownload in %f(%f - without limit) seconds.\n", child_pid, timeDownload, ttime);
	}
	else //Print msg, file downloaded....and in which time
		fprintf(stdout, "Client:%d\tDownload in %f seconds.\n", child_pid, ttime);
	
	fileStream.close();
	delete[] fileBinary;
	return 0;
} 

//Function for sending and receiving data with client
int communication(int child_pid, int client_socket, std::string files, int limit)
{
	sockaddr_in client;
	
	char str[INET_ADDRSTRLEN];
	char buff[1024];
	int received = 0;
	//Couple of messages, will be send to client
	std::string message ("WELCOME! THIS IS A VERY SIMPLE SERVER FOR SCHOOL PROJECT (IPK II.)\nYOU CAN DOWNLOAD THESE FILES:\n" + files + "$");
	std::string messageDown ("#DCONFIRMED#\r\n");
	std::string messageQuit ("#QCONFIRMED#\r\n");
	std::string tmp ("");
	//Client is connecter, just print info about this
	if(inet_ntop(AF_INET, &client.sin_addr, str, sizeof(str))) 
	{
		fprintf(stdout,"\nClient:%d\tIncoming on -> address:%s port:%d\n", child_pid, str, ntohs(client.sin_port));
		fflush(stdout);
	}
	//Send welcome message to client		
	send(client_socket, message.c_str(), strlen(message.c_str()), 0);
	while(1)		
	{	//Incoming message - message that client is ready for incoming binary data
		received = recv(client_socket, buff, 1024,0);
		if (received > 0)
		{
			buff[received] = '\0';
			
			fprintf(stdout, "Client:%d\t%s", child_pid, buff);
			//We knot name of file, because msg from client have form : '#DOWNLOAD:NAME#\r\n'
			if(strncmp(buff, "#DOWNLOAD:", 10) == 0)
			{
				send(client_socket, messageDown.c_str(), strlen(messageDown.c_str()), 0);
				tmp = buff;
				tmp = tmp.substr(10, tmp.length()-13); //parameter 2: -10 because of "#DOWNLOAD" -3 because of "#\r\n" - name of file is separated
				if(sendFile(client_socket, tmp, child_pid, limit) != 0) //send binary data with limit given from parameter
				{
					fprintf(stderr, "Client:%d\tError during sending file!\n", child_pid);
					break;
				}
				continue;
			}
			else if(strcmp(buff, "#QUIT#\r\n") == 0) //if client sends message '#QUIT#\r\n', respond on that, and finish client process, close his socket
			{
				send(client_socket, messageQuit.c_str(), strlen(messageQuit.c_str()), 0);
				break;
			}
		}
		else	// error or end of connection
			break;
	}
	fprintf(stdout, "Client:%d\tConnection closed\n", child_pid);
	close(client_socket);	
	return 0;
}
 
//Main function. Send list of availible files to client; parse an argument; bind socket and listen for clients;
//if there is a connected client, lets make another process (fork) and communication starts
int main (int argc, char *argv[])
{
	sockaddr_in server;
	socklen_t sin_len;

	int server_socket, client_socket;
	int port = 0;
	int limit = 0;
	int pid = 0;
	
	DIR* dir;
    dirent*  pdir;
    std::string files ("");
    dir = opendir(".");     // open current directory
    //save list of files into variable 'files', except '.' and '..' and source files 'server.cpp' 'client.cpp'
    while ((pdir = readdir(dir))) 
    {
    	if(!(!strcmp(pdir->d_name, ".") || !strcmp(pdir->d_name, "..") || !strcmp(pdir->d_name, "server.cpp") || !strcmp(pdir->d_name, "client.cpp")))
    		files = files + "   " + pdir->d_name + "\n";
    }
    closedir(dir);
    //parse an argument
    if(parameter_parsing(argc, argv, &port, &limit) != 0)
        return 1;
    //Connection handling 
	if(connection(&server_socket, &server, &sin_len, port) != 0)
		return 1;
	//Infinite cycle - server will always runs, and accept clients
	while(1)
	{
		if((client_socket = accept(server_socket, (struct sockaddr* )&server, &sin_len)) < 0 )
		{
			fprintf(stderr, "Connection wasn't accepted!\n");
			close(client_socket);
			return 1;	
		}
		//Fork process - another client - another runtime
		if((pid = fork()) >= 0)
		{ 	//if fork is succesfull, runtime for child process
			if(pid == 0)
				if(communication(getpid(), client_socket, files, limit) != 0)
					return 1;
		}
		else
		{
			fprintf(stderr, "Fork have failed!\n");
			close(client_socket);
			return 1;
		}
	}
    return 0;
}