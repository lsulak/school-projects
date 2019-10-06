/**
 *	@author			Ladislav Sulak (xsulak04@stud.fit.vutbr.cz)
 *	
 *	login 			xsulak04			
 *
 *	@date			2017-04
 *
 *	@file 			KRYException.h
 *
 *	@brief			Cryptography: Project 1 - Secure Channel.
 *
 *	@details		Client/Server application using Unix domain socket. 
 					Communication goes via secured channel. 
 					It uses:
 						* Diffie-Hellman for secure key exchange,
 						
 						* AES for encrypting (without autentization),
 						
 						* Feige-Fiat-Shamir identification protocol,
 						
 						* SHA256 hash function for maintaining integrity. Server responds with 
 						responding msg + hash (of received msg) when the secured channel is established.
 */

#ifndef KRY_EXCEPTION_H
#define KRY_EXCEPTION_H

#include <exception>


/* Inspirated by http://tldp.org/LDP/LG/issue74/misc/tougher/SocketException.h.txt */
class KRYException : public std::exception
{
public:
	KRYException (const std::string& msg) : m_msg (msg) { };

	~KRYException () { };

	const std::string what() noexcept
	{ 
		return m_msg;
	}

private:
	std::string m_msg;

};

#endif //KRY_EXCEPTION_H