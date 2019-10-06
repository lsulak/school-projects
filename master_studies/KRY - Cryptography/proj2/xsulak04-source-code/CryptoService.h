/**
 *	@author			Ladislav Sulak (xsulak04@stud.fit.vutbr.cz)
 *	
 *	login 			xsulak04			
 *
 *	@date			2017-05
 *
 *	@file 			CryptoService.h
 *
 *	@brief			Cryptography: Project 2 - Certificate Validator.
 *
 *	@details		Client application using OpenSSL for validating server's certificates.
 */

#ifndef CRYPTOSERVICE_H
#define CRYPTOSERVICE_H

#include <iostream>
#include <fstream>
#include <string.h>
#include <string>
#include <set>
#include <stddef.h>
#include <openssl/opensslv.h>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/x509_vfy.h>
#include <openssl/buffer.h>
#include <openssl/objects.h>


#if (SSLEAY_VERSION_NUMBER >= 0x0907000L)
# include <openssl/conf.h>
#endif


const bool PRINT_CERT					= false;
const bool PRINT_SERVERS_INFO			= false;
const bool ANALYZE_GET_REQ				= false;


const std::string HOST_NAME				= "minotaur.fi.muni.cz";
const std::string HOST_PORT_UNSECURD 	= "80";
const std::string HOST_PORT_SECURD		= "443";
const std::string CERT_MUNI				= "cert/crocs-ca.pem";
const std::string CERT_SYSTEM			= "/ca-bundle.trust.crt";
const ushort SUBDOMAIN_AMOUNT			= 100; 


const std::string M_HOST_SUBJECT_DIFFER	= "subject and hostname differ";
const std::string M_INSECURED_HASH_ALG	= "insecured hash algorithm";
const std::string M_IN_HASH_ALG_ROOT_CA	= " - but on root/intermediate CA its not such a risk";
const std::string M_SHORT_PUB_KEY		= "public key size is too short";
const std::string M_ISSUER_NAME_MISSING	= "issuer name is missing";
const std::string M_SUBJ_NAME_MISSING	= "subject name is missing";
const std::string M_SUBJ_WILDCARD		= "subject name has too general wildcard";


enum class TrustLevel
{
	TRUSTED		= 1,
	RELIABLE	= 2,
	SUSPICIOUS	= 3,
	UNTRUSTED	= 4
};

struct ValidStatus {
	std::string hostname;
	std::set<std::string> report;
	TrustLevel trust_level;
};


class CryptoService
{
	
public:
	static ValidStatus* validation_status;

	CryptoService ();
	~CryptoService ();

	void init_comm (bool olderTLS);
	void free_comm (void);
	static void err_handler_ssl ();
	static void err_handler_custom (std::string msg);

	std::string get_sys_cert ();

	static void check_hash_alg (X509* cert, int depth);
	static void check_pub_key_len (X509* cert);
	static void print_iname_sname (X509* cert);
	static void check_san_name (X509* cert);
	static int verify_callback (int preverify, X509_STORE_CTX *ctx);

	void analyse_get_request ();
	int init_conn_and_ctx ();
	int certif_handler ();

protected:
	SSL_CTX *m_ssl_ctx;  	// all options which are dealing with SSL connection in general
	SSL* m_ssl; 			// it contains all details about 1 particular connection, based on CTX struct
	BIO* m_bio, *m_out; 	// a layer which can be used for accessing files/memory/sockets
};

#endif // CRYPTOSERVICE_H