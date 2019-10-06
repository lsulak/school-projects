/**
 *	@author			Ladislav Sulak (xsulak04@stud.fit.vutbr.cz)
 *	
 *	login 			xsulak04			
 *
 *	@date			2017-05
 *
 *	@file 			main.cpp
 *
 *	@brief			Cryptography: Project 2 - Certificate Validator.
 *
 *	@details		Client application using OpenSSL for validating server's certificates.
 */

#include <string.h>
#include <iostream>

#include "Client.h"

const std::string CSV_FILENAME = "xsulak04-domains.csv";

/**
 *	@synopsis	Function for printing help on standard output.
 */
void printHelp ()
{
	std::cout
		<< "Description: \tProject 2 for subject KRY - Certificate Validator\n"
		<< "Author: \tLadislav Sulak <xsulak04@stud.fit.vutbr.cz\n"
		<< "Date: \t\t2017-05\n"
		<< "Usage: \t\t./cert_validate [options]\n"
		<< "\n"
		<< "Options:\n"
		<< "   -h 		Prints this help message.\n"
		<< std::endl;
}

int main (int argc, char *argv[])
{
	if (argc > 2)
	{
		std::cerr << "An error, too many arguments\n" << std::endl;
		printHelp ();
		return 1;
	}

	else if (argc == 2) 
	{
		int arg_len = strlen (argv[1]);
		if (strncmp (argv[1], "-h", arg_len) == 0 || strncmp (argv[1], "--help", arg_len) == 0) {
			printHelp ();
			return 0;
		}

		else 
		{
			std::cerr << "An error, no argument is needed except '-h or --help'\n" << std::endl;
			printHelp ();
			return 1;
		}
	}

	else
		Client(CSV_FILENAME).Run();

	return 0;
}
