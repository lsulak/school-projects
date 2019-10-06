#include <string.h>
#include <iostream>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>

#include <arpa/inet.h> 
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#include <sstream>
#include <fstream>

/* Author: Ladislav Sulak
   Login:  xsulak04
   Date:   4/2014
   mail:   xsulak04@stud.fit.vutbr.cz
   
   Run program:   ./client host:port/filename
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


//Global structure - for saving parts of input argument
struct p_struct { 
    std::string p_host;
           long p_port;
    std::string p_file;
} param;
p_struct parsed_struct;

//Function for parameter parsing. There should be 1 parameter in form: host:port/file
int parameter_parsing(int argc, std::string parameter)
{   
    unsigned long position,position2;
    std::string tmp = "";
    position = 0;
    position2 = 0;
    if(argc == 2)
    {
        //Find position of ':' and '/' .... and we know positions of every three elements of parameter
        position = parameter.find_first_of(":");
        position2 = parameter.find_first_of("/");
        if(position == std::string::npos || position2 == std::string::npos || position > position2 || ((position2+1) == parameter.length()))
        {
            fprintf(stderr, "Parameter has wrong format!\n");
            return 1; //port, or file is missing, or host 
        }
        //Save host into global structure
        parsed_struct.p_host = parameter.substr(0, position);
        tmp = parameter.substr(position+1, position2-position-1);

        const char* port_str = tmp.c_str();
        for(unsigned int i = 0; i < tmp.length(); i++)
        {
            if(!isdigit(port_str[i]))
            {
                fprintf(stderr, "Parameter has wrong format!\n");
                return 1;
            }
        }
        //Save port and filename into global structure
        parsed_struct.p_port = atoi(port_str);
        parsed_struct.p_file = parameter.substr(position2+1);
    }
    else 
    {
        fprintf(stderr, "Too few or too much parameters!\n");
        return 1;
    }
    return 0;
}

//Function for connecting client to server on specific port given from the parameter
int connection(int* m_socket)
{
    int port;
    std::string hostname ("");
    sockaddr_in server;
    hostent *get_ip;

    hostname = parsed_struct.p_host; //add host from structure (achieved from parameter)
    port = parsed_struct.p_port;

    //CREATE SOCKET
    *m_socket = socket(AF_INET , SOCK_STREAM , 0);
    if(*m_socket < 0) 
    {
        fprintf(stderr, "Socket couldn't be created!\n");
        close(*m_socket);
        return 1;
    }
    //GET IP FROM HOSTNAME
    get_ip = gethostbyname(hostname.c_str());
    if(get_ip == NULL)
    {
        fprintf(stderr, "Error while getting hostname!\n");
        close(*m_socket);
        return 1;
    }
    //CONNECT SOCKET TO SPECIFIC SERVER
    server.sin_family = AF_INET;               //Ipv4
    memcpy(&(server.sin_addr), get_ip->h_addr, get_ip->h_length); //IP adres                    
    server.sin_port = htons((u_short) port);   //Port
    
    if(connect(*m_socket , (struct sockaddr *)&server , sizeof(server)) < 0) //socket, socket structure, size 
    {
        fprintf(stderr, "Connection error!\n");
        close(*m_socket);
        return 1;
    }
    return 0;
}

//Function for sending command from socket descriptor
int send_data(int socket, std::string message)
{  
    if(send(socket , message.c_str() , message.length() , 0) < 0)
    {
        fprintf(stderr, "Send failed!\n");
        close(socket);
        return 1;
    }
    return 0;
}

//Function for receiving and printing welcome message. It's send by server, on connect
int receive_welcome(int socket)
{
    std::string text("");  //final message
    int position = 0;
    char server_msg[5000];
    while(1) //The cycle ends when receiving is broken, or we found Bye msg
    {
        if(recv(socket, server_msg, 5000, 0) == -1)
        {
            fprintf(stderr, "Error while receiving data!\n");
            close(socket);
            return 1;
        }
        text += server_msg;
        //Message ends with '$'. Everything except '$' is printed - message + list of files 
        if((position = text.find_first_of("$")) != 0)
        {
           text.erase(position);
           fprintf(stdout, "%s\n", text.c_str());
           return 0;
        } 
    }
    return 0;
}

//Function for receiving (but not printing) message from server, in form '#QCONFIRMED' = kind of goodbye msg,
//                                  size of file we want to download, or '#DCONFIRMED' = before download binary file
long receive_msg(int socket)
{   
    std::string text("");
    char server_msg[200]; //tmp buffer
    long position, position2, size = 0;
    
    while(1) //The cycle ends when receiving is broken, or we received '#QCONFIRMED\r\n' or '#DCONFIRMED\r\n', or #fileSize!#
    {
        if(recv(socket, server_msg, 200, 0) == -1)
        {
            fprintf(stderr, "Kicked from server (or server shutdown).\n");
            close(socket);
            exit(1);
        } 
        text += server_msg;
        //printf("%s\n", text.c_str());
        if(strcmp(text.c_str(),"#QCONFIRMED#\r\n") == 0 || strcmp(text.c_str(),"#DCONFIRMED#\r\n") == 0) //#QCONFIRMED# when communication between server and client ends
            return 0;
       
        else if((position2 = text.find_first_of("!")) != 0 ) //#size_of_file!# ...so when "!#" are found, 
        {                    //everything (except first char '#') is number = size of file from parameter
            position = text.find_first_of("#");
            size = atoi(text.substr(position+1, position2-position-1).c_str());
            return size;
        }
    }
    return 0;
}

//Function for downloading binary data - they are send character by character, and also received in this way
int download_file(int socket)
{
    std::ofstream myFile(parsed_struct.p_file.c_str());
    std::string text("");
    std::string text2("");
    //Open a file 
    FILE *fr = fopen(parsed_struct.p_file.c_str(), "w");
    // 1 Character -> received from server
    char character[1];
    bzero(character, 1); 

    int size;
    int received = 0;
    //Receiving message from server in form '#SIZE!#' . if SIZE == 0, its empty file - an error, no binary data will be send
    size = receive_msg(socket);
    if(size == 0)
    {
        fprintf(stderr, "An input file is empty!\n");
        close(socket);
        return 1;
    }
    send_data(socket, "#OK#\r\n"); //Send message '#OK#\r\n' to server - we know size of file, and server now can send binary data
    //Receiving binary data (downloading), character one by one.
    while(size >= 1) //cycle runs in length of file
    {
        if((received = recv(socket, character, 1, 0)) == -1)
        {
            close(socket);
            return 1;
        } 
        else if(received == 1)
        {
            fwrite(character, sizeof(char), received, fr); //write to file immediately 
            size -= 1;  //decrement size of file.
        }
        else
            break;
        bzero(character, 1); //in next iteration we need to clear buffer of size 1
    }
    return 0;
}

int communication(int* m_socket)
{
    //Commands we need for sending to server 
    std::string quit ("#QUIT#\r\n");
    std::string download ("#DOWNLOAD:");
    download = download + parsed_struct.p_file + "#\r\n";

    if(receive_welcome(*m_socket) != 0) //receive welcome message 
        return 1;

    if(send_data(*m_socket, download) != 0) //send message in form '#DOWNLOAD:FILE#\r\n'
        return 1;

    receive_msg(*m_socket);      //receive message '#DCONFIRMED\r\n' - server responds to our prevorious message

    if(download_file(*m_socket) != 0) //downloading - receiving binary data
        return 1;

    if(send_data(*m_socket, quit) != 0) //send message in form '#QUIT#\r\n' 
        return 1;
    
    receive_msg(*m_socket); //receiving message '#QCONFIRMED#\r\n' . Now file is downloaded and client will be disconnected and client program ends.
    return 0;
}

//Main function, it just calls another 3 functions for parsing parameter, connecting and communication with server
int main(int argc , char *argv[])
{
    int ctrl_socket;

    if(parameter_parsing(argc, argv[1]) != 0) 
        return 1;

    if(connection(&ctrl_socket) != 0)
        return 1;

    if(communication(&ctrl_socket) != 0)
        return 1;

    close(ctrl_socket);
    return 0;
}