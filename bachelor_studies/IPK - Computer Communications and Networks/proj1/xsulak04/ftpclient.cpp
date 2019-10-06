#include <string.h>
#include <iostream>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>

#include <arpa/inet.h> 
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
//Structure for saving separate parts of input parameter
struct p_struct { 
    std::string p_user;
    std::string p_pass;
    std::string p_host;
            long p_port;
    std::string p_path;
} param;
p_struct parsed_struct;
//Function for receiving port number from extended passive mode
long receive_port(int socket)
{   
    std::string text("");
    std::string answer_port("");
    std::string final_port("");
    unsigned long position, position2;
    char server_msg[2000]; //buffer. Size 2000 because if there is f.e. 200, there would be wrong symbols sometimes
    do
    {
        if(recv(socket, server_msg, 2000, 0) == -1) //receive data from server (socket)
        {
            fprintf(stderr, "Error while receiving data!\n");
            return 1;
        } 
        text += server_msg; //buffer is added to string with whole message
    } while((position = text.find("229")) == std::string::npos); //229 means, that we just entered to passive mode

    if(position == std::string::npos) //if find returns npos -> error
    {
        fprintf(stderr, "Error while entering extended passive mode (or bad username/pass)!\n");
        return 1;
    }
    
    answer_port = text.substr(position+35); //+35, because of message which is received
    position = 0;
    if((position = answer_port.find('(')) != 0) //after 35 characters, the next should be '('
    {
        fprintf(stderr, "Error while entering extended passive mode (or bad username/pass)!\n");
        return 1;
    }
    position2 = answer_port.find(")."); //there should be present character ')'
    answer_port = answer_port.substr(position+1, position2-1); //and between '(' and ')' is port number

    for(std::string::size_type i = 0; i < answer_port.size(); ++i) 
        if(isdigit(answer_port[i]))
            final_port += answer_port[i];
    
    return atol(final_port.c_str()); //return port number, for the safety long
}
//Function for receiving data - list of files/folders
int receive_data(int socket)
{
    std::string text("");  //final message
    long read_bytes;       
    char character;  //we will read character one by one and save into string "text"

    while((read_bytes = read(socket, &character, 1))) //The cycle ends when receiving is broken, or message is in the end
    {
        if(read_bytes < 0)
        {
            fprintf(stderr, "Error while receiving data!\n");
            close(socket);
            return 1;
        }
        else if(read_bytes == 0)
            break;
       text.push_back(character); //on the end of string , one character will be added
    }
    std::cout << text; //print the whole message on stdout
    return 0;
}
//Function for receiving final goodbye (from server, after command QUIT)
int receive_bye(int socket)
{   
    std::string text("");
    char server_msg[2000]; //tmp buffer
    while(1) //The cycle ends when receiving is broken, or we found Bye msg
    {
        if(recv(socket, server_msg, 2000, 0) == -1)
        {
            fprintf(stderr, "Error while receiving data!\n");
            return 1;
        } 
        if(text.find("221")) //221 when communication between server and client ends
            break;
        text += server_msg;
    }
    return 0;
}
//Function for sending command from socket descriptor
int send_data(int socket, std::string message)
{  
    if(send(socket , message.c_str() , message.length() , 0) < 0)
    {
        fprintf(stderr, "Send failed");
        return 1;
    }
    return 0;
}
//Function for parsing parameter from command line
int parameter_parsing(std::string parameter)
{   
    unsigned long position,position2;

    //if there is "ftp://" at the beginning, then is will be removed. It is an arbitrary part of a parameter
    if(parameter.substr(0, 6) == "ftp://") 
    {
        parameter = parameter.substr(6); //cut "ftp://" 

        position = parameter.find_first_of(":");
        position2 = parameter.find_first_of("@");

        if(position < position2 && position2 != std::string::npos) //part "user:pass@" is in presence ( : before @)
        {
            parsed_struct.p_user = parameter.substr(0, position); //save user to structure
            parsed_struct.p_pass = parameter.substr(position+1, position2-position-1); //save pass to structure
            parameter = parameter.substr(position2+1); //cut everything including @
        }
        if(position == 0 || position2 == 0 || (position2 - position) == 1) //If there is no user, or pass
            return 1;
    }
    //User and/or Pass is in there, but prefix "ftp://" not
    if(parameter.find("://") != std::string::npos)
        return 1;
    position = parameter.find_first_of(":");
    position2 = parameter.find_first_of("@");
    if(position2 != std::string::npos)
        return 1;
    if(position2 > position && position2 != std::string::npos)
        return 1;
    //NOW, in variable parameter "host" starts
    position = 0;
    position2 = 0;
    position = parameter.find_first_of(":"); //begin of port...its an arbitrary part of a parameter
    position2 = parameter.find_first_of("/");//begin of path, or last character....both arbitrary parts of a parameter
   
    if(position == std::string::npos && position2 == std::string::npos) //save host..there is nothing more (port, path, '/' either) 
    {
        parsed_struct.p_host = parameter;
        return 0;
    }
    if(position != std::string::npos && position2 == std::string::npos) //save host...there is also port, but nothing more
    {
        parsed_struct.p_host = parameter.substr(0, position);
        parameter = parameter.substr(position+1); //begin of port
        parsed_struct.p_port = atoi(parameter.substr(0, position2).c_str()); //save port
        return 0;
    }
    if(position2 != std::string::npos && position == std::string::npos) //there is host, path, but no port
    {
        parsed_struct.p_host = parameter.substr(0, position2);//save host
        parsed_struct.p_path = parameter.substr(position2);   //save path
        return 0;
    }
    if(position != std::string::npos && position2 != std::string::npos) //There are host, port, and part
    {
        parsed_struct.p_host = parameter.substr(0, position); //save host
        parsed_struct.p_port = atoi(parameter.substr(position+1, position2-position-1).c_str()); //save port
        parsed_struct.p_path = parameter.substr(position2); //save path
        return 0;
    }
    return 0;
}
//Function for login, and entering to ext. passive mode
int login_epsv(int ctrl_socket)
{   //Commands we need
    std::string user ("USER ");
    std::string pass ("PASS ");
    std::string epsv ("EPSV\r\n");
    //If there is no username and password in parameter, we will choose anonymous user
    std::string username ("anonymous"); //login as anonym.
    std::string password ("passwd");

    if(!parsed_struct.p_user.empty()) //there is user:pass in parameter, or anonymous if otherwise
    {
        username = parsed_struct.p_user;
        password = parsed_struct.p_pass;
    }
    user += username;
    user += "\r\n";
    pass += password;
    pass += "\r\n";
    if(send_data(ctrl_socket, user) == 1) //send command USER
    {
        close(ctrl_socket);
        return 1;
    }
    if(send_data(ctrl_socket, pass) == 1) //send command PASS
    {
        close(ctrl_socket);
        return 1;
    }
    if(send_data(ctrl_socket, epsv) == 1) // with succesfull login we enter to passive mode - command EPSV
    {
        close(ctrl_socket);
        return 1;
    }
    return 0;
}
int main(int argc , char *argv[])
{
    //Sockets descriptors and struktures for TCP communication
    int ctrl_socket, data_socket;
	sockaddr_in server;
	hostent *get_ip;
    //Commands we need
    std::string hostname ("");
    std::string list ("LIST ");
    std::string quit ("QUIT\r\n");
    std::string ip ();
    std::string data ();
 
    int port = 21; //default port 21, however we can change this in parameter
    long epsv_port = 0; //port given from ext. passive mode

    //PARAMETERS HANDLING
    if(argc == 2)
    {
        if(parameter_parsing(argv[1]) != 0)
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
    hostname = parsed_struct.p_host; //add host from structure (achieved from parameter)
    if(hostname.empty())
    {
        fprintf(stderr, "Hostname missing!\n");
        return 1;
    }
    if(parsed_struct.p_port != 0) //Default port is 21, but in parameter we can choose different
        port = parsed_struct.p_port;

    if(!parsed_struct.p_path.empty()) //if there is any path in parameter, add it to command LIST
        list += parsed_struct.p_path;
    list += "\r\n"; 

	//CREATE FIRST SOCKET
    ctrl_socket = socket(AF_INET , SOCK_STREAM , 0);
    if(ctrl_socket < 0) 
    {
        fprintf(stderr, "Socket couldn't be created!\n");
    	close(ctrl_socket);
        return 1;
    }
    //GET IP FROM HOSTNAME
    get_ip = gethostbyname(hostname.c_str());
    if(get_ip == NULL)
    {
        close(ctrl_socket);
        fprintf(stderr, "Error while getting hostname");
        return 1;
    }
    //CONNECT FIRST SOCKET TO SPECIFIC SERVER
    server.sin_family = AF_INET;             //Ipv4
    memcpy(&(server.sin_addr), get_ip->h_addr, get_ip->h_length); //IP adres				    
    server.sin_port = htons((u_short) port);   //Port
    
    if(connect(ctrl_socket , (struct sockaddr *)&server , sizeof(server)) < 0) //socket, socket structure, size - parameters
    {
    	close(ctrl_socket);
        fprintf(stderr, "Connection error!\n");
        return 1;
    }
    //LOGIN, ENTER TO PASSIVE MODE
    if(login_epsv(ctrl_socket) != 0) 
        return 1;

    if((epsv_port = receive_port(ctrl_socket)) == 1) //receive number of port in which we will connect from next socket (call him data_socket)
    {
        close(ctrl_socket);
        return 1;
    }
    //CREATE SECOND SOCKET
    data_socket = socket(AF_INET , SOCK_STREAM , 0);
    if(data_socket < 0) 
    {
        fprintf(stderr, "Socket couldn't be created!\n");
        close(data_socket);
        close(ctrl_socket);
        return 1;
    }
    //CONNECT SECOND SOCKET TO THE SAME SERVER
    server.sin_family = AF_INET;                                  //Ipv4
    memcpy(&(server.sin_addr), get_ip->h_addr, get_ip->h_length); //IP adres                    
    server.sin_port = htons((u_short)epsv_port); //port number given from epsv mode
    if(connect(data_socket , (struct sockaddr *)&server , sizeof(server)) < 0) //socket, socket structure, size - parameters
    {
        close(data_socket);
        close(ctrl_socket);
        fprintf(stderr, "Connection error!\n");
        return 1;
    }
    if(send_data(ctrl_socket, list) == 1) //send command LIST
    {
        close(data_socket);
        close(ctrl_socket);
        return 1;
    }
    if(receive_data(data_socket) == 1) //receive list of folders and files
    {
        close(ctrl_socket);
        return 1;
    }
    if(send_data(ctrl_socket, quit) == 1) //send command QUIT
    {
        close(data_socket);
        close(ctrl_socket);
        return 1;
    }
    if(receive_bye(ctrl_socket) == 1) //if server responds (by number 221) communication ends
    {
        close(data_socket);
        close(ctrl_socket);
        return 1;
    }
    close(data_socket); 
    close(ctrl_socket);
    return 0;
}
