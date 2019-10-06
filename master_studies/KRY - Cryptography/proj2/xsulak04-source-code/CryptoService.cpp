/**
 *	@author			Ladislav Sulak (xsulak04@stud.fit.vutbr.cz)
 *	
 *	login 			xsulak04			
 *
 *	@date			2017-05
 *
 *	@file 			CryptoService.cpp
 *
 *	@brief			Cryptography: Project 2 - Certificate Validator.
 *
 *	@details		Client application using OpenSSL for validating server's certificates.
 */

#include "CryptoService.h"

ValidStatus* CryptoService::validation_status = new ValidStatus;

CryptoService::CryptoService ()
{
	m_ssl_ctx = NULL;
	m_ssl = NULL;
	m_bio = NULL;
	m_out = NULL;

	SSL_library_init ();
	SSL_load_error_strings ();	
	ERR_load_BIO_strings ();

	if (!PRINT_SERVERS_INFO)
		std::cout.setstate(std::ios::failbit);

}

CryptoService::~CryptoService ()
{
	delete validation_status;

	if (!PRINT_SERVERS_INFO)
		std::cout.clear();
}

void CryptoService::init_comm (bool olderTLS)
{
	const SSL_METHOD* method = NULL;

	if (olderTLS)
		method = TLSv1_1_client_method ();
	else
		method = TLSv1_2_client_method ();

	if (method == NULL) err_handler_ssl ();

	m_ssl_ctx = SSL_CTX_new(method);  // client-mode initialization of CTX
	if (m_ssl_ctx == NULL) err_handler_ssl ();
}

void CryptoService::free_comm (void)
{
	if (m_out != NULL)
		BIO_free (m_out);

	if (m_bio != NULL)
		BIO_free_all (m_bio);

	if (m_ssl_ctx != NULL)
		SSL_CTX_free (m_ssl_ctx);
}

void CryptoService::err_handler_ssl ()
{
	unsigned long err = ERR_peek_error ();
	
	if (err != X509_V_OK)
		std::cerr << "[" << validation_status->hostname << "]::";

	ERR_print_errors_fp (stderr);
}

void CryptoService::err_handler_custom (std::string msg)
{
	std::cout << "[" << validation_status->hostname << "]::";
	std::cout << msg << std::endl;
}

// Inspirated from
// http://stackoverflow.com/questions/4138139/how-to-find-out-the-path-for-openssl-trusted-certificate
std::string CryptoService::get_sys_cert ()
{
	const char *dir;

	dir = getenv (X509_get_default_cert_dir_env ());

	if (!dir)
		dir = X509_get_default_cert_dir ();

	return std::string (dir) + CERT_SYSTEM;
}


void CryptoService::check_hash_alg (X509* cert, int depth)
{
	std::string err_message;
	int sign_alg;

	int err = OBJ_find_sigid_algs (OBJ_obj2nid(cert->sig_alg->algorithm), &sign_alg, NULL);
	if (err != 1)
		err_handler_ssl ();

	switch (sign_alg)
	{
		case NID_md5:
			err_handler_custom (M_INSECURED_HASH_ALG + " MD5");
			validation_status->report.insert (M_INSECURED_HASH_ALG + " MD5");
			validation_status->trust_level = TrustLevel::UNTRUSTED;
			break;

		// It is OK when the root CA is with SHA1 for now. On intermediate it can (?) be a problem...
		// https://superuser.com/questions/1122069/why-are-root-cas-with-sha1-signatures-not-a-risk
		case NID_sha1:
			err_message = M_INSECURED_HASH_ALG + " SHA1";

			if(depth == 0)
				validation_status->trust_level = TrustLevel::RELIABLE;
			else
				err_message += M_IN_HASH_ALG_ROOT_CA;

			err_handler_custom (err_message);
			validation_status->report.insert (err_message);
			break;

		// Keep it in here, in case of (possible) later utilization.
		case NID_sha224:
			break;
		case NID_sha256:
			break;
		case NID_sha384:
			break;
		case NID_sha512:
			break;
		default:
			break;
	}
}

// Inspirated from https://zakird.com/2013/10/13/certificate-parsing-with-openssl
// and http://fm4dd.com/openssl/certpubkey.htm
void CryptoService::check_pub_key_len (X509* cert)
{
	//err_handler_custom ("unable to find specified public key algorithm name");
	EVP_PKEY* pkey = X509_get_pubkey(cert);
	if (pkey == NULL){
		err_handler_custom ("errror getting public key from certificate");
		return;
	}

	switch (pkey->type) {
		case EVP_PKEY_RSA:
		case EVP_PKEY_DSA:
			if (EVP_PKEY_bits (pkey) < 2048) {
				err_handler_custom ("RSA/DSA public key is too short");

				validation_status->report.insert ("RSA/DSA " + M_SHORT_PUB_KEY);
				validation_status->trust_level = TrustLevel::SUSPICIOUS;
			}
			break;
		case EVP_PKEY_EC:
			if (EVP_PKEY_bits (pkey) < 256) {
				err_handler_custom ("EC public key is too short");

				validation_status->report.insert ("EC " + M_SHORT_PUB_KEY);
				validation_status->trust_level = TrustLevel::SUSPICIOUS;
			}
			break;
		default:
			err_handler_custom ("non-RSA/DSA/EC Key");
			break;
	}
	EVP_PKEY_free(pkey);
}

// Inspirated from https://wiki.openssl.org/index.php/SSL/TLS_Client function print_san_name
void CryptoService::check_san_name (X509* const cert)
{
	int success = 0;
	GENERAL_NAMES* names = NULL;
	unsigned char* utf8 = NULL;

	std::string wildcard_hostname = "*." + HOST_NAME;

	do
	{
		if(!cert) break;

		names = (GENERAL_NAMES*) X509_get_ext_d2i (cert, NID_subject_alt_name, 0, 0);
		if(!names) break;

		int i = 0, count = sk_GENERAL_NAME_num (names);
		if(!count) break;

		for(i = 0; i < count; ++i)
		{
			GENERAL_NAME* entry = sk_GENERAL_NAME_value (names, i);
			if(!entry) continue;

			if(GEN_DNS == entry->type)
			{
				int len1 = 0, len2 = -1;

				len1 = ASN1_STRING_to_UTF8 (&utf8, entry->d.dNSName);
				if(utf8)
					len2 = (int)strlen ((const char*)utf8);

				if(len1 != len2)
					err_handler_custom ("Strlen and ASN1_STRING size do not match (embedded null?)");

				/* If there's a problem with string lengths, then     */
				/* we skip the candidate and move on to the next.     */
				/* Another policy would be to fails since it probably */
				/* indicates the client is under attack.              */
				if(utf8 && len1 && len2 && (len1 == len2)) {
					if (strncmp ((const char*)utf8, "*", strlen( (const char*)utf8)) == 0) {
						err_handler_custom (M_SUBJ_WILDCARD);

						validation_status->report.insert (M_SUBJ_WILDCARD);
						validation_status->trust_level = TrustLevel::SUSPICIOUS;
					}
					else if (0 == strncmp ((const char*)utf8,
											wildcard_hostname.c_str(),
											strlen((const char*)utf8))) 
					{
						;
					}
					else if (validation_status->hostname.compare ((const char*)utf8) != 0) {
						err_handler_custom (M_HOST_SUBJECT_DIFFER);

						validation_status->report.insert (M_HOST_SUBJECT_DIFFER);
						validation_status->trust_level = TrustLevel::SUSPICIOUS;
					}
					success = 1;
				}
				if(utf8)
					OPENSSL_free (utf8), utf8 = NULL;
			}
		}
	} while (0);

	if(names)
		GENERAL_NAMES_free (names);

	if(utf8)
		OPENSSL_free (utf8);

	if(!success) {
		err_handler_custom (M_SUBJ_NAME_MISSING);

		validation_status->report.insert(M_SUBJ_NAME_MISSING);
		validation_status->trust_level = TrustLevel::SUSPICIOUS;
	}
}

void CryptoService::print_iname_sname (X509* cert)
{
	X509_NAME* iname = cert ? X509_get_issuer_name (cert) : NULL;
	X509_NAME* sname = cert ? X509_get_subject_name (cert) : NULL;

	char* x = X509_NAME_oneline (iname, NULL, 0);
	char* y = X509_NAME_oneline (sname, NULL, 0);

	std::cout << x << std::endl;
	std::cout << y << std::endl;
}

int CryptoService::verify_callback (int preverify, X509_STORE_CTX *ctx)
{
	int depth = X509_STORE_CTX_get_error_depth (ctx);
	int err = X509_STORE_CTX_get_error (ctx);
	X509* cert = X509_STORE_CTX_get_current_cert (ctx);

	std::string err_message = X509_verify_cert_error_string (err);
	
	if (err != X509_V_OK) 
	{
		err_handler_custom (err_message);

		validation_status->report.insert (err_message);
		validation_status->trust_level = TrustLevel::UNTRUSTED;
	}
	check_hash_alg (cert, depth);
	check_pub_key_len (cert);

	if (PRINT_SERVERS_INFO)
		print_iname_sname (cert);

	// If depth is 0, its the server's certificate. Check the SANs too
	if (depth == 0) {
		check_san_name(cert);
		if (PRINT_SERVERS_INFO)
			std::cout << "-------------------" << std::endl << std::endl;
	}

	return 1;
}

int CryptoService::init_conn_and_ctx ()
{
	long res = 1;

	const long flags = SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_COMPRESSION;
	SSL_CTX_set_options (m_ssl_ctx, flags);

	// Loads the certificate chain.
	res = SSL_CTX_load_verify_locations (m_ssl_ctx, CERT_MUNI.c_str(), NULL);
	if(res != 1) err_handler_ssl ();

	res = SSL_CTX_load_verify_locations (m_ssl_ctx, get_sys_cert ().c_str (), NULL);
	if(res != 1) err_handler_ssl ();

	m_bio = BIO_new_ssl_connect (m_ssl_ctx);  // creates a new BIO chain
	if (m_bio == NULL) err_handler_ssl ();

	std::string full_hostname = validation_status->hostname + ":" + HOST_PORT_SECURD;
	res = BIO_set_conn_hostname (m_bio, full_hostname.c_str());
	if (res != 1) err_handler_ssl ();

	BIO_get_ssl(m_bio, &m_ssl);  // fetch SSL conn object
	if (m_ssl == NULL) err_handler_ssl ();

	// Send/Recv msg through SSL can sometime fail because of some internal needs.
	// This is handled automatically With SSL_MODE_AUTO_RETRY.
	SSL_set_mode (m_ssl, SSL_MODE_AUTO_RETRY);

	const char* const PREFERRED_CIPHERS = "HIGH:!aNULL:!kRSA:!PSK:!SRP:!MD5:!RC4";
	res = SSL_set_cipher_list (m_ssl, PREFERRED_CIPHERS);
	if(res != 1) err_handler_ssl ();

	res = SSL_set_tlsext_host_name (m_ssl, validation_status->hostname.c_str());
	if(res != 1) err_handler_ssl ();

	m_out = BIO_new_fp (stdout, BIO_NOCLOSE);
	if(m_out == NULL) err_handler_ssl ();

	X509_STORE_new ();
	X509_STORE_CTX_new ();

	return 0;
}

int CryptoService::certif_handler ()
{
	long res = 1;

	// Sets SSL_VERIFY_PEER and a callback function. This ensures the chain is verified
	// according to RFC 4158 and Issuer and Subject info can be printed.
	SSL_CTX_set_verify (m_ssl_ctx, SSL_VERIFY_PEER, verify_callback);

	if (init_conn_and_ctx () != 0)
		return 1;

	// Connect to server - name lookup for the host and then 3-way handshake.
	res = BIO_do_connect (m_bio);
	if(res != 1) {
		err_handler_ssl ();
		return 1;
	}

	// TLS/SSL handshake ;)
	res = BIO_do_handshake (m_bio);
	if(res != 1) err_handler_ssl ();

	// Verify a server certificate was presented during the negotiation.
	X509* cert = SSL_get_peer_certificate (m_ssl);
	if (cert) 
	{
		if(PRINT_CERT) 
		{
			std::cout << "------------------------ CERT BEGIN ------------------------" << std::endl;
			X509_print_fp (stdout, cert);
			std::cout <<  "------------------------ CERT END ------------------------" << std::endl;
		}
		X509_free (cert); // free immediately
	}

	if (cert == NULL) 
		err_handler_ssl ();

	// Verify the result of chain verification, according to RFC 4158.
	// For more info see: https://wiki.openssl.org/index.php/Manual:Verify(1)
	res = SSL_get_verify_result (m_ssl);
	if(res != X509_V_OK)
		err_handler_ssl ();

	if(validation_status->report.size () == 0)
		validation_status->report.insert ("ok");

	return 0;
}
