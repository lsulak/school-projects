/**
 *	@author			Ladislav Sulak (xsulak04@stud.fit.vutbr.cz)
 *	
 *	login 			xsulak04			
 *
 *	@date			2017-04
 *
 *	@file 			main.cpp
 *
 *	@brief			Cryptography: Project 1 - Secure Channel.
 *	@details		Socket part of the application using Unix domain socket. 
 					Communication goes via secured channel. 
 					It uses:
 						* Diffie-Hellman for secure key exchange,
 						
 						* AES for encrypting (without autentization),
 						
 						* Feige-Fiat-Shamir identification protocol,
 						
 						* SHA256 hash function for maintaining integrity. Server responds with 
 						responding msg + hash (of received msg) when the secured channel is established.
 */

#ifndef SOCKETSERVICE_H
#define SOCKETSERVICE_H

#include <cstring>
#include <memory>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include "KRYException.h"

#define DEFAULT_BUFFER_LEN 	4096
#define INVALID_SOCKET 		-1


const std::string HELLO_MSG 		= "HELLO_MESSAGE";
const std::string CLIENT_ENDS		= "CLIENT_ENDS";
const std::string PING_MSG 			= "PING";
const std::string IV_RESPOND		= "AES_IV_THANKS";
const std::string FFS_KEY_RESPOND	= "FFS_PUB_KEY_THANKS";
const std::string FFS_E_VEC_RESPOND = "FFS_E_VECTOR_THANKS";
const std::string ACCEPT_MSG		= "WELCOME_YOU_ARE_AUTENTICATED";
const std::string ACCEPT_RND_MSG	= "THIS_AUTH_ROUND_IS_OK";
const std::string REFUSE_MSG		= "REFUSED_YOU_HAVE_NO_POWER_HERE";


class SocketService
{
public:
	SocketService (const std::string& socket_filepath) : 
		m_socket_addr(nullptr), 
		m_socket_fd(INVALID_SOCKET), 
		m_socket_filepath(socket_filepath)
	{
		m_socket_addr = new sockaddr_un;

		std::memset(m_socket_addr, 0, sizeof(sockaddr_un));
		std::memset(m_buffer, 0, DEFAULT_BUFFER_LEN);
	}

	SocketService (int session_fd) :  m_socket_fd(session_fd) {}

	~SocketService () {}

	void Open ()
	{

		if ((m_socket_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == INVALID_SOCKET) {
			Close();
			throw KRYException("[Socket::Open] Error during initializing a socket: " + std::string(strerror(errno)));
		}

		m_socket_addr->sun_family = AF_UNIX;

		std::memcpy(m_socket_addr->sun_path, m_socket_filepath.data(),
					std::min(m_socket_filepath.size(), sizeof(sockaddr_un::sun_path)));

		//strncpy(m_socket_addr->sun_path, m_socket_filepath.c_str(), sizeof(m_socket_addr->sun_path)-1);
	}

	void Close ()
	{
		shutdown(m_socket_fd, SHUT_RDWR);
		close(m_socket_fd);

		delete m_socket_addr;
	}

	void Connect ()
	{
		if (connect(m_socket_fd, (struct sockaddr*) m_socket_addr, sizeof(sockaddr_un)) == INVALID_SOCKET) {
			Close();
			throw KRYException("[Socket::Connect] Error during socket connecting: " + std::string(strerror(errno)));
		}
	}

	void Bind ()
	{
		unlink(m_socket_addr->sun_path); 
		if (bind(m_socket_fd, (struct sockaddr*) m_socket_addr, sizeof(sockaddr_un)) == INVALID_SOCKET) {
			Close();
			throw KRYException("[Socket::Bind] Error during binding to the selected socket: " + std::string(strerror(errno)));
		}
	}

	void Listen ()
	{
		// Max number it can be passed is 128 - limit of outstanding connections
		if (listen(m_socket_fd, SOMAXCONN) == INVALID_SOCKET) {
			Close();
			throw KRYException("[Socket::Listen] Error during listening on the selected socket: " + std::string(strerror(errno)));
		}
	}

	SocketService* Accept()
	{
		int session_fd;
		if ((session_fd = accept(m_socket_fd, NULL, NULL)) == INVALID_SOCKET) {
			Close();
			throw KRYException("[Socket::Accept] Error during socket accept");
		}
		
		return new SocketService(session_fd);
	}

	void Send(const std::string& payload) 
	{
		if(send(m_socket_fd, payload.data(), payload.size(), 0) == INVALID_SOCKET) {
			Close();
			throw KRYException("[Socket::Send] Unable to send a payload via the selected socket: " + std::string(strerror(errno)));
		}
	}

	void Send(const char* payload, int size) 
	{
		if(send(m_socket_fd, payload, size, 0) == INVALID_SOCKET) {
			Close();
			throw KRYException("[Socket::Send] Unable to send a payload via the selected socket: " + std::string(strerror(errno)));
		}
	}

	std::string Receive() 
	{
		char *recv_buffer = new char[DEFAULT_BUFFER_LEN];
		int64_t received_bytes = recv(m_socket_fd, recv_buffer, DEFAULT_BUFFER_LEN, 0);

		if (received_bytes == 0) {
			Close();
			throw KRYException("[Socket::Receive] Remote endpoint is disconnected");
		}

		else if(received_bytes == INVALID_SOCKET) {
			Close();
			throw KRYException("[Socket::Receive] Unable to receive a payload from the selected socket: " + std::string(strerror(errno)));
		}

		else
		{
			std::string received_string(recv_buffer, received_bytes);
			delete[] recv_buffer;
			return received_string;
		}
		return std::string();
	}

private:
	sockaddr_un* m_socket_addr;
	int m_socket_fd;

	const std::string m_socket_filepath;

	uint8_t m_buffer[DEFAULT_BUFFER_LEN];
};

#endif // SOCKETSERVICE_H