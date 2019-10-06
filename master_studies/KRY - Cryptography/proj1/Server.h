/**
 *	@author			Ladislav Sulak (xsulak04@stud.fit.vutbr.cz)
 *	
 *	login 			xsulak04			
 *
 *	@date			2017-04
 *
 *	@file 			Server.h
 *
 *	@brief			Cryptography: Project 1 - Secure Channel.
 *
 *	@details		Server part of the application using Unix domain socket. 
 					Communication goes via secured channel. 
 					It uses:
 						* Diffie-Hellman for secure key exchange,
 						
 						* AES for encrypting (without autentization),
 						
 						* Feige-Fiat-Shamir identification protocol,
 						
 						* SHA256 hash function for maintaining integrity. Server responds with 
 						responding msg + hash (of received msg) when the secured channel is established.
 */

#ifndef SERVER_H
#define SERVER_H

#include "services/CryptoService.h"


class Server : CryptoService
{
public:
	Server (
		const std::string socket_file, 
		const std::string pub_prime_mod, 
		const std::string pub_prime_base,
		const std::string pub_FFS_mod,
		bool testing);
	~Server();

	void Run ();

private:
	void DiffieHellman ();
	void ReceiveMessageAndSendHash ();

	void ReceiveFFSPublicKey (std::string ffs_publ_key[K_FFS_SEC_PARAM + 1]);
	bool HandleFFSProtocol ();
	bool Authenticate (std::string received_x_val, std::string received_y_val,
			std::string received_v_vec[K_FFS_SEC_PARAM + 1], int e_vec[K_FFS_SEC_PARAM]);

	SocketService* m_session_socket_serv;
};

#endif	/* SERVER_H */