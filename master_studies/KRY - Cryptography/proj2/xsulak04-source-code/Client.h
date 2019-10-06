/**
 *	@author			Ladislav Sulak (xsulak04@stud.fit.vutbr.cz)
 *	
 *	login 			xsulak04			
 *
 *	@date			2017-05
 *
 *	@file 			Client.h
 *
 *	@brief			Cryptography: Project 2 - Certificate Validator.
 *
 *	@details		Client application using OpenSSL for validating server's certificates.
 */

#ifndef CLIENT_H
#define CLIENT_H

#include "CryptoService.h"

class Client : CryptoService
{
public:
	Client (const std::string csv_filename);
	~Client ();

	void Run ();

private:
	void analyze_get_request ();
	void print_to_csv (ValidStatus validation_stats[]);

	std::string m_csv_filename;
};

#endif	/* CLIENT_H */