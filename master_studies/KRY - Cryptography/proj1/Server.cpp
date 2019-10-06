/**
 *	@author			Ladislav Sulak (xsulak04@stud.fit.vutbr.cz)
 *	
 *	login 			xsulak04			
 *
 *	@date			2017-04
 *
 *	@file 			Server.cpp
 *
 *	@brief			Cryptography: Project 1 - Secure Channel.
 *	@details		Server part of the application using Unix domain socket.
 					Communication goes via secured channel. 
 					It uses:
 						* Diffie-Hellman for secure key exchange,
 						
 						* AES for encrypting (without autentization),
 						
 						* Feige-Fiat-Shamir identification protocol,
 						
 						* SHA256 hash function for maintaining integrity. Server responds with 
 						responding msg + hash (of received msg) when the secured channel is established.
 */

#include "Server.h"

Server::Server (
	const std::string socket_file, 
	const std::string pub_prime_mod, 
	const std::string pub_prime_base,
	const std::string pub_FFS_mod,
	bool testing) :

	CryptoService(socket_file, pub_prime_mod, pub_prime_base, pub_FFS_mod, "logs/logs_server.out", testing) {}

Server::~Server() { }

void Server::DiffieHellman ()
{
	std::string pre_shared_secret_key, send_public_key, recv_public_key;

	m_session_socket_serv = m_sock_serv->Accept ();
	m_session_socket_serv->Send (HELLO_MSG);

	m_log_serv->Display ("[Server::DiffieHellman] Sending: ", HELLO_MSG);

	ComputePrivateKey();

	// m_log_serv->Display ("[Server::DiffieHellman] Private key generated: ", GetPrivateKey ());

	send_public_key = ComputePublicKey ();
	m_session_socket_serv->Send (send_public_key);
	m_log_serv->Display ("[Server::DiffieHellman] Servers' public key generated and sent "/*, send_public_key*/);

	recv_public_key = m_session_socket_serv->Receive();
	m_log_serv->Display ("[Server::DiffieHellman] Client's public key received "/*, recv_public_key*/);

	pre_shared_secret_key = ComputeSharedKey (recv_public_key);
	m_shared_secret_key = RunSHA256 (pre_shared_secret_key);
}

void Server::ReceiveMessageAndSendHash ()
{
	std::string hash, message;

	message = ReceiveEncryptedText(m_session_socket_serv);
	m_log_serv->Display ("[Server::ReceiveMessageAndSendHash] Received msg: ", message);

	hash = RunSHA256 (message);
	m_log_serv->Display ("[Server::ReceiveMessageAndSendHash] Sending message hash: ", hash);

	SendSecuredText(m_session_socket_serv, hash);
}

void Server::ReceiveFFSPublicKey (std::string ffs_publ_key[K_FFS_SEC_PARAM + 1])
{
	for (short i = 0; i < K_FFS_SEC_PARAM + 1; i++) {
		ffs_publ_key[i] = ReceiveEncryptedText(m_session_socket_serv);
		m_log_serv->Display ("[Server::ReceiveFFSPublicKey] Received FFS publ key "/*, ffs_publ_key[i]*/);
		
		SendSecuredText (m_session_socket_serv, FFS_KEY_RESPOND + " [" + std::to_string(i) + "]");
		m_log_serv->Display ("[Server::ReceiveFFSPublicKey] Send FFS publ key respond: ",  FFS_KEY_RESPOND);
	}
}

bool Server::HandleFFSProtocol ()
{
	std::string received_x_val, received_y_val, e_vec_to_send("");
	int e_vec[K_FFS_SEC_PARAM];
	int generated_num = 0;
	bool autenticated = false;

	std::string ffs_publ_key[K_FFS_SEC_PARAM + 1];  // Last item is public modulo 'n'

	ReceiveFFSPublicKey (ffs_publ_key);

	received_x_val = ReceiveEncryptedText(m_session_socket_serv);
	m_log_serv->Display ("[Server::HandleFFSProtocol] Received X val "/*, received_x_val*/);

	// Generate 'vector' of random values from set {0,1} and complete a calculation.
	for (short i = 0; i < K_FFS_SEC_PARAM; i++)
	{
		generated_num = atoi(ComputeSeed(1).c_str()) % 2;
		e_vec_to_send += std::to_string(generated_num);
		e_vec[i] = generated_num;
	}
	SendSecuredText (m_session_socket_serv, e_vec_to_send);
	m_log_serv->Display ("[Server::HandleFFSProtocol] Send E vector: ", e_vec_to_send);

	received_y_val = ReceiveEncryptedText (m_session_socket_serv);
	m_log_serv->Display ("[Server::HandleFFSProtocol] Received Y value "/*, received_y_val*/);

	// Last check :-)
	if (not Authenticate (received_x_val, received_y_val, ffs_publ_key, e_vec)) {
		m_log_serv->Display ("[Server::HandleFFSProtocol] Client not autenticated, keys were wrong");
		SendSecuredText (m_session_socket_serv, REFUSE_MSG);
		m_log_serv->Display ("[Server::HandleFFSProtocol] Send AUTH message: ", REFUSE_MSG);
	}

	else {
		SendSecuredText (m_session_socket_serv, ACCEPT_RND_MSG);
		m_log_serv->Display ("[Server::HandleFFSProtocol] Send AUTH message: ", ACCEPT_RND_MSG);
		autenticated = true;
	}
	return autenticated;
}

bool Server::Authenticate (std::string received_x_val, std::string received_y_val, 
	std::string received_v_vec[K_FFS_SEC_PARAM + 1], int e_vec[K_FFS_SEC_PARAM])
{
	bool autenticated = false;

	// Init all necessary GMP numbers and structures
	mpz_t gmp_x_val, gmp_neg_x_val, gmp_y_val, public_modulo, gmp_z_val;
	mpz_t v_values_vec [K_FFS_SEC_PARAM];

	mpz_init (gmp_x_val);
	mpz_init (gmp_neg_x_val);
	mpz_init (gmp_y_val);
	mpz_init (gmp_z_val);

	mpz_init_set_str (gmp_x_val, received_x_val.c_str(), 10);
	mpz_init_set_str (gmp_y_val, received_y_val.c_str(), 10);

	for (short i = 0; i < K_FFS_SEC_PARAM; i++)
		mpz_init_set_str (v_values_vec[i], received_v_vec[i].c_str(), 10);

	mpz_init_set_str (public_modulo, received_v_vec[K_FFS_SEC_PARAM].c_str(), 10); // Modulo

	// Computation of z = y^2 * vi ^ ei mod n.

	// Compute y^2
	mpz_mul (gmp_z_val, gmp_y_val, gmp_y_val);
	mpz_mod (gmp_z_val, gmp_z_val, public_modulo);

	// Compute basically a sequence of multiplications.
	for (short i = 0; i < K_FFS_SEC_PARAM; i++) {
		if (e_vec[i] == 1) {
			mpz_mul (gmp_z_val, gmp_z_val, v_values_vec[i]);

			// Compute modulus, so we have a final -  Z
			mpz_mod (gmp_z_val, gmp_z_val, public_modulo);
		}
	}
	//mpz_mod (gmp_z_val, gmp_z_val, public_modulo);
	mpz_neg(gmp_neg_x_val, gmp_x_val);

	//gmp_printf ("%s X is an mpz %Zd\n", "here", gmp_x_val);
	//gmp_printf ("%s Z is an mpz %Zd\n", "here", gmp_z_val);


	if (mpz_cmp(gmp_z_val, gmp_x_val) == 0 || mpz_cmp(gmp_z_val, gmp_neg_x_val) == 0) {
		autenticated = true;
	}

	if (mpz_get_ui(gmp_z_val) == 0)
		autenticated = false;

	// Clear memory from GMP numbers and structures
	mpz_clear (gmp_x_val);
	mpz_clear (gmp_neg_x_val);
	mpz_clear (gmp_y_val);
	mpz_clear (gmp_z_val);
	mpz_clear (public_modulo);

	for (short i = 0; i < K_FFS_SEC_PARAM; i++)
		mpz_clear (v_values_vec[i]);

	return autenticated;
}

void Server::Run()
{	
	try
	{
		m_sock_serv->Open ();
		m_sock_serv->Bind ();
		m_sock_serv->Listen ();
		m_log_serv->Display ("[Server::Run] Server started");

		// Diffie-Hellman
		DiffieHellman();
		m_log_serv->Display ("[Server::Run] Shared secret key calculated: ", m_shared_secret_key);
		
		ReceiveMessageAndSendHash();

		for (int i = 0; i < T_FFS_SEC_PARAM; i++) 
		{
			// Feige-Fiat-Shamir identification
			if (not HandleFFSProtocol ()) {
				m_log_serv->Display ("[Server::Run] Client not autenticated");
			};
		}

		m_log_serv->Display ("[Server::Run] Client sucessfully autenticated");
		SendSecuredText (m_session_socket_serv, ACCEPT_MSG);
		m_log_serv->Display ("[Server::HandleFFSProtocol] Send AUTH message: ", ACCEPT_MSG);

		ReceiveMessageAndSendHash();

		m_log_serv->Display ("[Server::Run] Server ends");
	}
	catch (KRYException& ex)
	{
		m_log_serv->Display ("[Server::Run] KRYException: ", ex.what());
	}
}
