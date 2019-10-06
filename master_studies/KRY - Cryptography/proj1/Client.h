/**
 *	@author			Ladislav Sulak (xsulak04@stud.fit.vutbr.cz)
 *	
 *	login 			xsulak04			
 *
 *	@date			2017-04
 *
 *	@file 			Client.h
 *
 *	@brief			Cryptography: Project 1 - Secure Channel.
 *
 *	@details		Client part of the application using Unix domain socket.
 					Communication goes via secured channel. 
 					It uses:
 						* Diffie-Hellman for secure key exchange,
 						
 						* AES for encrypting (without autentization),
 						
 						* Feige-Fiat-Shamir identification protocol,
 						
 						* SHA256 hash function for maintaining integrity. Server responds with 
 						responding msg + hash (of received msg) when the secured channel is established.
 */

#ifndef CLIENT_H
#define CLIENT_H

#include "services/CryptoService.h"


class Client : CryptoService
{
public:
	Client (
		const std::string socket_file, 
		const std::string pub_prime_mod, 
		const std::string pub_prime_base,
		const std::string pub_FFS_mod,
		bool testing);
	~Client ();

	void Run ();

private:
	void DiffieHellman ();

	void SendMessageAndReceiveHash (const std::string message);

	void InitPrivateKeyFFS ();
	void InitPublicKeyFFS ();
	void FreeFFSKeys ();
	void SendPublicKeyFFS ();
	
	void HandleFFSProtocol ();
	void HandleEVector (mpz_t *r_number);


	// Vector (private key) consisting of private items for FFS
	static const std::string s0;
	static const std::string s1;
	static const std::string s2;
	static const std::string s3;
	static const std::string s4;

	mpz_t m_s_vector_ffs [K_FFS_SEC_PARAM];
	mpz_t m_v_vector_ffs [K_FFS_SEC_PARAM + 1]; // Last item is public modulo

};

#endif	/* CLIENT_H */