/**
 *	@author			Ladislav Sulak (xsulak04@stud.fit.vutbr.cz)
 *	
 *	login 			xsulak04			
 *
 *	@date			2017-04
 *
 *	@file 			Client.cpp
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

#include "Client.h"



const std::string Client::s0 = "74332719096221206106095413109489442556412674825434719873207141231851147737"
								"1271973919067415983785044159596020755013357302898134363984897429956030688"
								"5407975240413862032370352304111724573281921388359950381559971993901111691"
								"4832748780029602379931307630902807316035557622033926441147418919531413079"
								"746799717078336";

const std::string Client::s1 = "34872423687895948428383046966107511012117343192669254962765777779167265970"
								"4387687204092916007648758020112581311187918259554446306250445073575024907"
								"1314862881364008861900410594785215922501938715944012641397064201180857928"
								"7084591173223680469549109603755406534478566040384522623608286101341827824"
								"102274449010401";

const std::string Client::s2 = "15487416395674465481822317114750371988100524382522281085528192558064572600"
								"2292528891363223824988118532934654806025539745793215266303695143314960370"
								"2814707325549583484951537227628424595786892701682484099506724816621029305"
								"3231699718107610114359379466173856887366105660651586232385165524496605875"
								"9536498402546110";

const std::string Client::s3 = "90761665601646844651874079550419223979545228246924857894861745496960497633"
								"1168031793992355488234096203294902859822815514982566015932095685473042582"
								"6177311571992780055308405949071400047024391203452563143614655932799744530"
								"4567545288585238956417308223117366103775979382748152531523918526831273669"
								"131563912108610";

const std::string Client::s4 = "70591149646013650282127363446502672482449011466152863456069689438100848192"
								"3639652212995052455579486408253631642095774816010612538694312812028797321"
								"2032802372706520754695761809707950719745322365785191949752281313561225317"
								"1142504691590575945034609152321017730419383304128475930585712721782107349"
								"425188100936001";


Client::Client (
	const std::string socket_file, 
	const std::string pub_prime_mod, 
	const std::string pub_prime_base, 
	const std::string pub_FFS_mod,
	bool testing) : 

	CryptoService(socket_file, pub_prime_mod, pub_prime_base, pub_FFS_mod, "logs/logs_client.out", testing) {}

Client::~Client () {}

void Client::DiffieHellman ()
{
	std::string pre_shared_secret_key, send_public_key, recv_public_key;
	std::string hello_msg;

	hello_msg = m_sock_serv->Receive ();
	m_log_serv->Display ("[Client::DiffieHellman] Received: ", hello_msg);

	if (hello_msg != HELLO_MSG)
		m_log_serv->Display ("[Client::DiffieHellman] Received BAD hello message, continuing despite of it.");

	ComputePrivateKey ();
	m_log_serv->Display ("[Client::DiffieHellman] Private key generated "/*, GetPrivateKey ()*/);

	recv_public_key = m_sock_serv->Receive ();
	m_log_serv->Display ("[Client::DiffieHellman] Server's public key received "/*, recv_public_key*/);

	send_public_key = ComputePublicKey ();
	m_sock_serv->Send (send_public_key);
	m_log_serv->Display("[Client::DiffieHellman] Client's public key generated and sent "/*, send_public_key*/);

	pre_shared_secret_key = ComputeSharedKey (recv_public_key);
	m_shared_secret_key = RunSHA256 (pre_shared_secret_key);
}

void Client::SendMessageAndReceiveHash (const std::string message)
{
	std::string response, hash_original;

	SendSecuredText (m_sock_serv, message);
	m_log_serv->Display ("[Client::SendMessageAndReceiveHash] Sending: ", message);
	
	hash_original = RunSHA256 (message);
	response = ReceiveEncryptedText(m_sock_serv);

	if (response != hash_original) {
		m_log_serv->Display (response);
		m_log_serv->Display (hash_original);
		throw KRYException("[Client::SendMessageAndReceiveHash] An error - integrity check failed, hash differs");
	}
	else
		m_log_serv->Display ("[Client::SendMessageAndReceiveHash] Received message hash, it's correct: ", response);
}

void Client::InitPrivateKeyFFS ()
{
	mpz_init_set_str (m_s_vector_ffs[0], s0.c_str(), 10);
	mpz_init_set_str (m_s_vector_ffs[1], s1.c_str(), 10);
	mpz_init_set_str (m_s_vector_ffs[2], s2.c_str(), 10);
	mpz_init_set_str (m_s_vector_ffs[3], s3.c_str(), 10);
	mpz_init_set_str (m_s_vector_ffs[4], s4.c_str(), 10);
}

void Client::FreeFFSKeys ()
{
	// Clear memory for all variables used with GMP lib.
	for (short i = 0; i < K_FFS_SEC_PARAM; i++) {
		mpz_clear (m_s_vector_ffs[i]);
		mpz_clear (m_v_vector_ffs[i]);
	}
	mpz_clear (m_v_vector_ffs[K_FFS_SEC_PARAM]);
}

void Client::InitPublicKeyFFS ()
{
	mpz_t tmp_s_square_vector_ffs [K_FFS_SEC_PARAM];

	for (short i = 0; i < K_FFS_SEC_PARAM; i++) {
		mpz_init (tmp_s_square_vector_ffs[i]);
		mpz_init (m_v_vector_ffs[i]);

		// compute s^2 vector
		mpz_mul (tmp_s_square_vector_ffs[i], m_s_vector_ffs[i], m_s_vector_ffs[i]);
		mpz_mod (tmp_s_square_vector_ffs[i], tmp_s_square_vector_ffs[i], m_gmp_pub_FFS_modulo);

		// compute vector v (public key): (-1)^bi * (si^2)(^INV) mod n (public modulo)
		mpz_invert (m_v_vector_ffs[i], tmp_s_square_vector_ffs[i], m_gmp_pub_FFS_modulo);
	}

	// Last item in vector v (public key) is private modulo.
	mpz_init_set (m_v_vector_ffs[K_FFS_SEC_PARAM], m_gmp_pub_FFS_modulo); 

	// Generate 'vector' of random values from set {0,1} and complete a calculation, part vi = (-1)^bi * ...
	for (short i = 0; i < K_FFS_SEC_PARAM; i++)
		if (( atoi (ComputeSeed(1).c_str()) % 2 ) == 1)
			mpz_sub (m_v_vector_ffs[i], m_gmp_pub_FFS_modulo, m_v_vector_ffs[i]);

	m_log_serv->Display ("[Client::InitPublicKeyFFS] Vector v (public key) for FFS calculated");

	for (short i = 0; i < K_FFS_SEC_PARAM; i++) {
		mpz_clear (tmp_s_square_vector_ffs[i]);
	}
}

void Client::SendPublicKeyFFS ()
{
	std::string response;

	for (short i = 0; i < K_FFS_SEC_PARAM + 1; i++) {
		SendSecuredText (m_sock_serv, mpz_get_str (NULL, 10, m_v_vector_ffs[i]));
		m_log_serv->Display ("[Client::SendPublicKeyFFS] Sending v vector (public key) for FFS ", i);
		
		response = ReceiveEncryptedText(m_sock_serv);
		m_log_serv->Display ("[Client::SendPublicKeyFFS] Received: ", response);
	}
}

void Client::HandleFFSProtocol ()
{
	mpz_t r_number, r_number_tmp, tmp_number, x_number;

	mpz_init_set_str (r_number, ComputeSeed(256).c_str(), 16);

	// We can do this because the public key is always > 0. ;-) ehm, should be
	mpz_init(x_number);
 	mpz_init_set (tmp_number, m_gmp_pub_FFS_modulo);
 	mpz_sub_ui (tmp_number, tmp_number, 1);

	// Number in range <0, n-2>, we need <1, n-1> so just increment
	mpz_mod (r_number, r_number, tmp_number);


	mpz_add_ui (r_number, r_number, 1);
	mpz_init_set (r_number_tmp, r_number);

	mpz_mul (r_number, r_number, r_number);
	mpz_mod (x_number, r_number, m_gmp_pub_FFS_modulo);

	if ((atoi(ComputeSeed(1).c_str()) % 2 ) == 1)
		mpz_sub (x_number, m_gmp_pub_FFS_modulo, x_number);

	//gmp_printf ("%s is an mpz %Zd\n", "here", x_number);
	SendSecuredText (m_sock_serv, mpz_get_str (NULL, 10, x_number));
	m_log_serv->Display ("[Client::HandleFFSProtocol] Sending X val "/*, mpz_get_str (NULL, 10, x_number)*/);


	HandleEVector(&r_number_tmp);

	mpz_clear (tmp_number);
	mpz_clear (x_number);
	mpz_clear (r_number);
	mpz_clear (r_number_tmp);
}

void Client::HandleEVector (mpz_t *r_number)
{
	bool e_vector_ffs [K_FFS_SEC_PARAM];
	std::string tmp_e_vector, final_y_number;

	tmp_e_vector = ReceiveEncryptedText (m_sock_serv);
	m_log_serv->Display ("[Client::HandleEVector] Received E vector: ", tmp_e_vector);

	//SendSecuredText (m_sock_serv, FFS_E_VEC_RESPOND);
	//m_log_serv->Display ("[Client::HandleEVector] Sending: ", FFS_E_VEC_RESPOND);

	for(std::string::size_type i = 0; i < tmp_e_vector.size(); i++) {
		if (tmp_e_vector[i] == '1')
			e_vector_ffs[i] = true;

		else if (tmp_e_vector[i] == '0')
			e_vector_ffs[i] = false;

		else {
			e_vector_ffs[i] = false;
			m_log_serv->Display ("[Client::HandleEVector] E vector was broken!");
		}
	}
	// Generate 'vector' of random values from set {0,1} and complete a calculation
	for (short i = 0; i < K_FFS_SEC_PARAM; i++)
		if (e_vector_ffs[i]) {
			mpz_mul (*r_number, *r_number, m_s_vector_ffs[i]);
			mpz_mod (*r_number, *r_number, m_gmp_pub_FFS_modulo);
		}

	// Now we have Y in our r_number
	mpz_mod (*r_number, *r_number, m_gmp_pub_FFS_modulo);

	final_y_number = mpz_get_str (NULL, 10, *r_number);
	SendSecuredText (m_sock_serv, final_y_number);
	m_log_serv->Display ("[Client::HandleEVector] Sending Y value "/*, final_y_number*/);
}

void Client::Run ()
{
	try
	{
		std::string auth_message = "";

		m_sock_serv->Open ();
		m_sock_serv->Connect ();
		m_log_serv->Display ("[Client::Run] Client started");
		
		// Diffie-Hellman
		DiffieHellman ();
		m_log_serv->Display ("[Client::Run] Shared secret key calculated: ", m_shared_secret_key);

		SendMessageAndReceiveHash (PING_MSG);

		for (int i = 0; i < T_FFS_SEC_PARAM; i++) 
		{
			// Feige-Fiat-Shamir identification
			InitPrivateKeyFFS ();
			InitPublicKeyFFS ();
			SendPublicKeyFFS ();

			HandleFFSProtocol ();

			// If we have been accepted for this round or not.
			auth_message = ReceiveEncryptedText (m_sock_serv);
			m_log_serv->Display ("[Client::Run] Received AUTH msg: ", auth_message);

			if (auth_message.compare(ACCEPT_RND_MSG) != 0) {
				m_log_serv->Display ("[Client::Run] Not authenticated");
			}
		}

		// Last message - if we have been accepted or not.
		auth_message = ReceiveEncryptedText (m_sock_serv);
		m_log_serv->Display ("[Client::Run] Received AUTH msg: ", auth_message);

		// Sending message, receives response as hash and check the integrity.
		SendMessageAndReceiveHash (CLIENT_ENDS);

		// Communication ends.
		m_log_serv->Display ("[Client::Run] Client ends");

	}
	catch (KRYException& ex)
	{
		m_log_serv->Display ("[Client::Run] KRYException: ", ex.what());
	}
	FreeFFSKeys ();
}
