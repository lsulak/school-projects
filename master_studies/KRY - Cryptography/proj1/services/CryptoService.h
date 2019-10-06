/**
 *	@author			Ladislav Sulak (xsulak04@stud.fit.vutbr.cz)
 *	
 *	login 			xsulak04			
 *
 *	@date			2017-04
 *
 *	@file 			CryptoService.h
 *
 *	@brief			Cryptography: Project 1 - Secure Channel.
 *	@details		Cryptography part of the application using Unix domain socket.
 					Communication goes via secured channel. 
 					It uses:
 						* Diffie-Hellman for secure key exchange,
 						
 						* AES for encrypting (without autentization),
 						
 						* Feige-Fiat-Shamir identification protocol,
 						
 						* SHA256 hash function for maintaining integrity. Server responds with 
 						responding msg + hash (of received msg) when the secured channel is established.
 */

#ifndef CRYPTOSERVICE_H
#define CRYPTOSERVICE_H

#include <stddef.h>
#include <gmp.h>
#include <openssl/aes.h> 
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <memory>

#include "SocketService.h"
#include "LoggerService.h"

#define KEY_SIZE 	32
#define BLOCK_SIZE 	16

#define K_FFS_SEC_PARAM 5
#define T_FFS_SEC_PARAM 4

typedef unsigned char byte;
using EVP_CIPHER_CTX_free_ptr = std::unique_ptr<EVP_CIPHER_CTX, decltype(&::EVP_CIPHER_CTX_free)>;


class CryptoService
{
public:

	CryptoService (
		const std::string socket_file, 
		const std::string pub_prime_mod, 
		const std::string pub_prime_base,
		const std::string pub_FFS_mod,
		const std::string log_file_name,
		bool testing) : 
	
		m_sock_serv(new SocketService(socket_file)),
		m_log_serv(new LoggerService(log_file_name, testing))

	{
		mpz_init (m_gmp_private_key);

		// Originally in std::string and hexadecimal/decimal.
		mpz_init_set_str (m_gmp_prime_modulo, pub_prime_mod.c_str(), 16);
		mpz_init_set_str (m_gmp_prime_base, pub_prime_base.c_str(), 10);
		mpz_init_set_str (m_gmp_pub_FFS_modulo, pub_FFS_mod.c_str(), 10);

		// Initialise the OpenSSL library for AES in CBC mode
		// Load the necessary cipher
		EVP_add_cipher(EVP_aes_256_cbc());
	}

	~CryptoService ()
	{
		// Clear memory for all variables used with GMP lib.
		mpz_clear (m_gmp_private_key);
		mpz_clear (m_gmp_prime_modulo);
		mpz_clear (m_gmp_prime_base);
		mpz_clear (m_gmp_pub_FFS_modulo);

		// Clean up OpenSSL library used for AES in CBC mode
		EVP_cleanup();
		ERR_free_strings();
	}

	/**
	 *	@synopsis	Method which obtains a private key for DH: just a generation, big random number
	 *				from 1 to prime_modulo-2 (inclusive), but for cryptographic purposes it will be
	 *				better not to generate smaller keys. It will be kept like this for a purposes 
	 *				of this project.
	 */
	void ComputePrivateKey ()
	{
		// Init state for a default algorithm in GMP lib.
		gmp_randstate_t rstate;
		gmp_randinit_default (rstate);

		mpz_t gmp_upper_key_max, gmp_seed;

		// Generator seed for the random engine to reference is obtained 
		// from /dev/urandom by OpenSSL library. It is in hexa format and std::string.
		mpz_init_set_str (gmp_seed, ComputeSeed(128).c_str(), 16);
		gmp_randseed (rstate, gmp_seed);

		// We want for DH a range: 1 <= key <= prime_mod-2
		// So firstly substract by 3 and then increment.
		mpz_init_set (gmp_upper_key_max, m_gmp_prime_modulo);  // just init and an assigment
		mpz_sub_ui (gmp_upper_key_max, gmp_upper_key_max, 3);  // gmp_upper_key_max -= 3

		// Generate private key.
		mpz_urandomm (m_gmp_private_key, rstate, gmp_upper_key_max);
		mpz_add_ui (m_gmp_private_key, m_gmp_private_key, 1);

		// Clear memory for the random generator state.
		gmp_randclear (rstate);
		mpz_clear (gmp_upper_key_max);
		mpz_clear (gmp_seed);
	}

	/**
	 *	@synopsis	Method computes public key which will be sent later for computing shared secret key.
	 *				It is done by following: "prime_base ^ private_key mod prime_modulo".
	 *
	 *	@return	public_key std::string representation of public key in hexadecimal.
	 */
	std::string ComputePublicKey ()
	{
		std::string str_public_key;

		mpz_t gmp_public_key;
		mpz_init (gmp_public_key);

		mpz_powm (gmp_public_key, m_gmp_prime_base, m_gmp_private_key, m_gmp_prime_modulo);

		str_public_key = mpz_get_str (NULL, 16, gmp_public_key);  // in hexadecimal

		mpz_clear (gmp_public_key);
		return str_public_key;
	}

	/**
	 *	@synopsis	Method computes a final, secret shared key from the obtained public key,
	 *				given in a parameter. It is done by following:
	 *				"public_key ^ private_key mod prime_modulo"
	 *
	 *	@return	secret_shared_key std::string representation of shared key in hexadecimal.
	 */
	std::string ComputeSharedKey (std::string str_pub_key)
	{
		std::string str_secret_shared_key;

		mpz_t gmp_pub_key, gmp_secret_shared_key;
		mpz_init (gmp_secret_shared_key);
		mpz_init_set_str (gmp_pub_key, str_pub_key.c_str(), 16);

		mpz_powm (gmp_secret_shared_key, gmp_pub_key, m_gmp_private_key, m_gmp_prime_modulo);

		str_secret_shared_key = mpz_get_str (NULL, 16, gmp_secret_shared_key);  // in hexadecimal

		mpz_clear (gmp_secret_shared_key);
		return str_secret_shared_key;
	}

	/**
	 *	@synopsis	Method obtains string representation of private key used for DH, in hexadecimal. 
	 *
	 *	@return	string representation of private key in hexadecimal.
	 */
	std::string GetPrivateKey ()
	{
		return mpz_get_str (NULL, 16, m_gmp_private_key);
	}

	/**
	 *	@synopsis	Method hashes a string given in the input argument. 
	 *
	 *	@return	string representation of resulting hash.
	 */
	std::string RunSHA256 (std::string keystream)
	{

		char result_stream [SHA256_DIGEST_LENGTH*2];

		u_int8_t results[SHA256_DIGEST_LENGTH]; 

		SHA256_CTX sha256_ctx;

		SHA256_Init (&sha256_ctx);
		SHA256_Update (&sha256_ctx, (u_int8_t *)keystream.c_str(), keystream.length());
		SHA256_Final (results, &sha256_ctx);

		//printf("SHA256 for %s\n\nIS: ", keystream.c_str ());

		for (short n = 0; n < SHA256_DIGEST_LENGTH; n++) {
		//	printf("%02x", results[n]); 
			sprintf(result_stream + 2 * n, "%02x", results[n]);
		}
		//printf("\n");
		//printf("IS: %s\n", result_stream);

		return std::string (result_stream);
	}

	/**
	 *	@synopsis	Method receives and sends IV and also decrypts the incoming message. 
	 *  @param 		socket_serv which is the actual instance of socketservice library 
	 *
	 *	@return	string representation of decrypted text.
	 */
	std::string ReceiveEncryptedText (SocketService* socket_serv)
	{
		std::string plaintext;
		
		std::string iv = socket_serv->Receive();  // receive IV
		m_log_serv->Display ("[CryptoService::ReceiveEncryptedText] Received IV");

		socket_serv->Send(IV_RESPOND); 
		m_log_serv->Display ("[CryptoService::ReceiveEncryptedText] Sending: ", IV_RESPOND);

		std::string ciphertext = socket_serv->Receive();  // receiving ciphertext
		AESDecrypt ((const byte*)m_shared_secret_key.c_str(), 
			(const byte*)iv.c_str(), ciphertext, plaintext);

		return plaintext;
	}

	/**
	 *	@synopsis	Method sends receives IV and also encrypts the plaintext from second argument.
	 *  @param 		socket_serv which is the actual instance of socketservice library
	 *  @param 		plaintext string to send
	 *
	 *	@return	string representation of decrypted text.
	 */
	void SendSecuredText (SocketService* socket_serv, const std::string plaintext)
	{
		std::string ciphertext, respond_msg;
		byte iv[BLOCK_SIZE];

		const RAND_METHOD* rm = RAND_get_rand_method();
		if (rm != RAND_SSLeay ()) {
			m_sock_serv->Close();
			throw KRYException ("[CryptoService::SendSecuredText] RAND_METHOD mismatch");
		}

		//RAND_SSLeay ();

		if (RAND_bytes(iv, BLOCK_SIZE) != 1) {
			m_sock_serv->Close();
			throw KRYException(
				"[CryptoService::SendSecuredText] An error during seed generation: " + 
				std::string(ERR_error_string(ERR_get_error(), NULL)));
		}

		//iv_str = std::string(reinterpret_cast<const char*>(iv));

		AESEncrypt ((const byte*)m_shared_secret_key.c_str(), 
			iv, plaintext, ciphertext);
		
		socket_serv->Send((const char*)iv, BLOCK_SIZE);  // sending IV
		m_log_serv->Display ("[CryptoService::SendSecuredText] Sending IV");

		respond_msg = socket_serv->Receive();
		m_log_serv->Display ("[CryptoService::SendSecuredText] Received: ", respond_msg);

		socket_serv->Send (ciphertext);  // sending ciphertext

		OPENSSL_cleanse (iv, BLOCK_SIZE);
	}


protected:
	/**
	 *	@synopsis	Method which obtains random bytes from /dev/urandom
	 *
	 *	@return	keystring string representation of key returned from OpenSSL lib
	 */
	std::string ComputeSeed (unsigned int buffer_len)
	{
		byte buffer[buffer_len];
		char keystring[buffer_len * 2 + 1];

		const RAND_METHOD* rm = RAND_get_rand_method();
		if (rm != RAND_SSLeay ()) {
			m_sock_serv->Close();
			throw KRYException("[CryptoService::ComputeSeed] RAND_METHOD mismatch");
		}

		//RAND_SSLeay ();

		if (RAND_bytes(buffer, buffer_len) != 1) {
			m_sock_serv->Close();
			throw KRYException(
				"[CryptoService::ComputeSeed] An error during seed generation: " + 
				std::string(ERR_error_string(ERR_get_error(), NULL)));
		}

		for (unsigned short i = 0; i < sizeof (buffer); i++)
			sprintf (keystring + 2*i, "%02X", buffer[i]);

		OPENSSL_cleanse (buffer, buffer_len);

		return keystring;
	}

	SocketService* m_sock_serv;
	LoggerService* m_log_serv;

	std::string m_shared_secret_key;
	mpz_t m_gmp_pub_FFS_modulo;

private:

	/**
	 *	@synopsis	Method which encrypts data provided as plaintext from a given parameter. It uses AES in CBC256 mode
	 				without authentization.
	 	@param 	key for hashing, it is secret
	 	@param 	iv initiation vector, which is not secret, but it is good to change it very often, for example with every message
	 	@param  ptext means plaintext to encryption
	 	@param  ctext means ciphertext which is the result of this method
	 */
	int AESEncrypt (const byte key[KEY_SIZE], const byte iv[BLOCK_SIZE], const std::string& ptext, std::string& ctext)
	{
		EVP_CIPHER_CTX_free_ptr ctx (EVP_CIPHER_CTX_new(), ::EVP_CIPHER_CTX_free);

		int rc = EVP_EncryptInit_ex (ctx.get(), EVP_aes_256_cbc(), NULL, key, iv);
		if (rc != 1) {
			m_sock_serv->Close();
			throw KRYException ("[CryptoService:AESEncrypt] EVP_EncryptInit_ex failed");
		}

		// Recovered text expands upto BLOCK_SIZE.
		ctext.resize (ptext.size() + BLOCK_SIZE);
		int out_len1 = (int)ctext.size();

		rc = EVP_EncryptUpdate (ctx.get(), (byte*)&ctext[0], &out_len1, (const byte*)&ptext[0], (int)ptext.size());
		if (rc != 1) {
			m_sock_serv->Close();
			throw KRYException ("[CryptoService:AESEncrypt] EVP_EncryptUpdate failed");
		}

		int out_len2 = (int)ctext.size() - out_len1;

		rc = EVP_EncryptFinal_ex (ctx.get(), (byte*)&ctext[0]+out_len1, &out_len2);
		if (rc != 1) {
			m_sock_serv->Close();
			throw KRYException ("[CryptoService:AESEncrypt] EVP_EncryptFinal_ex failed");
		}

		// Set cipher text size now that we know it
		ctext.resize (out_len1 + out_len2);
		return out_len1 + out_len2;
	}

	/**
	 *	@synopsis	Method which decrypts data provided as ciphertext from a given parameter. It uses AES in CBC256 mode
	 				without authentization.
	 	@param 	key for hashing, it is secret
	 	@param 	iv initiation vector, which is not secret, but it is good to change it very often, for example with every message
	 	@param  ctext means ciphertext for decrypting
	 	@param  ptext means plaintext which is the result of this method
	 */
	int AESDecrypt (const byte key[KEY_SIZE], const byte iv[BLOCK_SIZE], const std::string& ctext, std::string& rtext)
	{
		EVP_CIPHER_CTX_free_ptr ctx (EVP_CIPHER_CTX_new(), ::EVP_CIPHER_CTX_free);

		int rc = EVP_DecryptInit_ex (ctx.get(), EVP_aes_256_cbc(), NULL, key, iv);
		if (rc != 1) {
			m_sock_serv->Close();
			throw KRYException ("[CryptoService:AESDecrypt] EVP_DecryptInit_ex failed");
		}

		// Recovered text contracts upto BLOCK_SIZE.
		rtext.resize (ctext.size());
		int out_len1 = (int)rtext.size();

		rc = EVP_DecryptUpdate (ctx.get(), (byte*)&rtext[0], &out_len1, (const byte*)&ctext[0], (int)ctext.size());
		if (rc != 1) {
			m_sock_serv->Close();
			throw KRYException ("[CryptoService:AESDecrypt] EVP_DecryptUpdate failed");
		}

		int out_len2 = (int)rtext.size() - out_len1;

		rc = EVP_DecryptFinal_ex (ctx.get(), (byte*)&rtext[0]+out_len1, &out_len2);
		if (rc != 1) {
			m_sock_serv->Close();
			throw KRYException ("[CryptoService:AESDecrypt] EVP_DecryptFinal_ex failed");
		}

		// Set recovered text size now that we know it
		rtext.resize (out_len1 + out_len2);
		return out_len1 + out_len2;
	}

	mpz_t m_gmp_private_key;
	mpz_t m_gmp_prime_modulo;
	mpz_t m_gmp_prime_base;
};

#endif // CRYPTOSERVICE_H