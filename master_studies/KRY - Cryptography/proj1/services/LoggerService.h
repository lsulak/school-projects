/**
 *	@author			Ladislav Sulak (xsulak04@stud.fit.vutbr.cz)
 *	
 *	login 			xsulak04			
 *
 *	@date			2017-04
 *
 *	@file 			LoggerService.h
 *
 *	@brief			Cryptography: Project 1 - Secure Channel.
 *	@details		Logger service part of the application using Unix domain socket.
 					Communication goes via secured channel. 
 					It uses:
 						* Diffie-Hellman for secure key exchange,
 						
 						* AES for encrypting (without autentization),
 						
 						* Feige-Fiat-Shamir identification protocol,
 						
 						* SHA256 hash function for maintaining integrity. Server responds with 
 						responding msg + hash (of received msg) when the secured channel is established.
 */

#ifndef LOGGERSERVICE_H
#define LOGGERSERVICE_H

#include <fstream>
#include <iostream>


class LoggerService
{
public:
	LoggerService(const std::string log_file_name, bool testing)
	{
		m_ostream = testing ? new std::ofstream(log_file_name) : &std::cout;
	}

	~LoggerService() {}

	void Display(const std::string msg) 
	{
		*m_ostream << msg << std::endl;
	}

	void Display(const std::string msg, const std::string additional_msg) 
	{
		*m_ostream << msg << additional_msg << std::endl;
	}

	void Display(const std::string msg, const int additional_msg) 
	{
		*m_ostream << msg << "[" << additional_msg << "]" << std::endl;
	}

private:
	std::ostream* m_ostream;
};

#endif // LOGGERSERVICE_H