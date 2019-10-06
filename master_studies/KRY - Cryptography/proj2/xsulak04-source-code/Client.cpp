/**
 *	@author			Ladislav Sulak (xsulak04@stud.fit.vutbr.cz)
 *	
 *	login 			xsulak04			
 *
 *	@date			2017-05
 *
 *	@file 			Client.cpp
 *
 *	@brief			Cryptography: Project 2 - Certificate Validator.
 *
 *	@details		Client application using OpenSSL for validating server's certificates.
 */

#include "Client.h"


Client::Client (const std::string csv_filename) : CryptoService () 
{
	m_csv_filename = csv_filename;
}

Client::~Client () {}

void Client::print_to_csv (ValidStatus validation_stats[])
{
	std::set<std::string>::iterator it;

	std::ofstream csv_file;
	csv_file.open (m_csv_filename);

	for (ushort i = 0; i < SUBDOMAIN_AMOUNT; i++) 
	{
		csv_file << validation_stats[i].hostname + ", ";
		csv_file << static_cast<int>(validation_stats[i].trust_level) << ", ";

		for (it = validation_stats[i].report.begin(); it != validation_stats[i].report.end();) 
		{
			csv_file << *it;
			if (++it != validation_stats[i].report.end())
				csv_file << " AND ";
		}

		if (i != SUBDOMAIN_AMOUNT - 1)
			csv_file << std::endl;
	}
	csv_file.close ();
}

void Client::analyze_get_request ()
{
	const std::string message = 
		"GET / HTTP/1.1\r\n"
		"Host: " + validation_status->hostname + "\r\n"
		"Connection: close\r\n\r\n";

	BIO_puts (m_bio, message.c_str ());
	BIO_puts (m_out, "\n");

	int len = 0;
	do
	{
		char buff[1536] = {};
		len = BIO_read (m_bio, buff, sizeof (buff));

		if(len > 0)
			BIO_write (m_out, buff, len);

	} while (len > 0 || BIO_should_retry (m_bio));
}

void Client::Run ()
{
	std::string subdomain;

	ValidStatus validation_stats[SUBDOMAIN_AMOUNT];

	int res = 0;
	bool fallback = false;
	
	for (ushort i = 0; i < SUBDOMAIN_AMOUNT; i++) 
	{
		// We need full domain name.
		subdomain = i < 10 ? "0" + std::to_string (i) : std::to_string (i);
		validation_status->hostname = subdomain + "." + HOST_NAME;

		do {
			init_comm (fallback);

			if(!fallback) {
				validation_status->report.clear ();
				validation_status->trust_level = TrustLevel::TRUSTED;
			}

			res = certif_handler ();
			if (res != 0) {
				std::cerr << "Falling back to older TLS 1.1" << std::endl;

				validation_status->trust_level = TrustLevel::RELIABLE;
				validation_status->report.insert ("older TLS/SSL version");

				if (fallback) break;  // only 1 fallback available

				fallback = true;
			}
			else
				fallback = false;

			validation_stats[i] = *validation_status;

			if (ANALYZE_GET_REQ)
				analyze_get_request ();
			free_comm ();
		} while (fallback);
	}
	print_to_csv (validation_stats);
}
