/**
 *	@author			Ladislav Sulak (xsulak04@stud.fit.vutbr.cz)
 *	
 *	login 			xsulak04			
 *
 *	@date			2017-04
 *
 *	@file 			main.cpp
 *
 *	@brief			Cryptography: Project 1 - Secure Channel.
 *
 *	@details		Client/Server application using Unix domain socket. 
 					Communication goes via secured channel. 
 					It uses:
 						* Diffie-Hellman for secure key exchange,
 						
 						* AES for encrypting (without autentization),
 						
 						* Feige-Fiat-Shamir identification protocol,
 						
 						* SHA256 hash function for maintaining integrity. Server responds with 
 						responding msg + hash (of received msg) when the secured channel is established.
 */

#include <getopt.h>

#include "Client.h"
#include "Server.h"


// Testing flag, it means that stdout will be redirected to file(s), 
// client's log file is separated from server's log file.  
bool TESTING = false;

enum class ArgType
{
	ARGS_CLIENT		= 1,
	ARGS_SERVER		= 2,
	ARGS_HELP		= 3,
	ARGS_INVALID	= 4
};

const std::string socket_file = "/tmp/secured_kry1-xsulak04.socket";


/*
 	Obtained from rfc3526 - 2048-bit MODP group.
 	It should be 'safe prime', which means that (p-1)/2 is also a prime.
 	It is recommented that it should change more often, but for the purpose of this
 	project it is sufficient. In some big projects, like OpenVPN, the prime numbers are generated
 	once and they are not changing oftenly. 

 	https://www.ietf.org/rfc/rfc3526.txt

	2 <= prime_base <= prime_mod 
*/
const std::string pub_prime_base = "2";

const std::string pub_prime_mod = "FFFFFFFFFFFFFFFFC90FDAA22168C234C4C6628B80DC1CD1"
								  "29024E088A67CC74020BBEA63B139B22514A08798E3404DD"
								  "EF9519B3CD3A431B302B0A6DF25F14374FE1356D6D51C245"
								  "E485B576625E7EC6F44C42E9A637ED6B0BFF5CB6F406B7ED"
								  "EE386BFB5A899FA5AE9F24117C4B1FE649286651ECE45B3D"
								  "C2007CB8A163BF0598DA48361C55D39A69163FA8FD24CF5F"
								  "83655D23DCA3AD961C62F356208552BB9ED529077096966D"
								  "670C354E4ABC9804F1746C08CA18217C32905E462E36CE3B"
								  "E39E772C180E86039B2783A2EC07A28FB5C55DF06F4C52C9"
								  "DE2BCBF6955817183995497CEA956AE515D2261898FA0510"
								  "15728E5A8AACAA68FFFFFFFFFFFFFFFF";

/* From the subject's assignment */
const std::string pub_FFS_mod = "76896662714508286744734522772713649137443706128590068779597"
								"45371785637987457481498414328462251004415926559654387970412"
								"85206147536141111189006057233704081795093856350260711717537"
								"53971595311787642734631255964171862122769944457180726400998"
								"94312769931280376743471528836172408766824676501451745920484"
								"93842891094009455019701200795712422922630035806939436754211"
								"03055474447477714088715152187480821234096948376237000274125"
								"43688341294152014919041062895021897004625831652681996972503"
								"72146744961820201717959144053979785645512775525862781097716"
								"86568007143134450111699204213342342246127354528257251352614"
								"071191397288071566525113181";

/**
 *	@synopsis	Function for printing help on standard output.
 */
void printHelp ()
{
	std::cout
		<< "Description: \tProject 1 for subject KRY - Secure Channel\n"
		<< "Author: \tLadislav Sulak <xsulak04@stud.fit.vutbr.cz\n"
		<< "Date: \t\t2017-04\n"
		<< "Usage: \t\t./secure_channel [options]\n"
		<< "\n"
		<< "Options:\n"
		<< "   -c | -s 	Specifies a client or server mode of the application.\n"
		<< "   -h 		Prints this help message.\n"
		<< std::endl;
}

/**
 *	@synopsis	Function for parsing the input arguments using getopt.
 *
 * 	@param	argc number of the input arguments.
 *	@param 	argv aray contaning input arguments.
 *	
 *	@return	ArgType Which one, that specific depends on input argument.
 */
ArgType parseArgs (int argc, char *argv[])
{
	int opt;
	ArgType usedArg;

	while ((opt = getopt(argc, argv, "csht")) != -1)
	{
		switch (opt)
		{
			case 't':
				TESTING = true;
				continue;
			case 'c':
				usedArg = ArgType::ARGS_CLIENT;
				break;
			case 's':
				usedArg = ArgType::ARGS_SERVER;
				break;
			case 'h':
				usedArg = ArgType::ARGS_HELP;
				break;
			default:
				usedArg = ArgType::ARGS_INVALID;
		}
	}
	return usedArg;
}


int main (int argc, char *argv[])
{
	ArgType argType = parseArgs(argc, argv);
	
	if ((TESTING && argc != 3) || 
		(!TESTING && (argc != 2 || argType == ArgType::ARGS_INVALID)))
	{
		printHelp();
		return 1;
	}

	else if (argType == ArgType::ARGS_HELP)	 printHelp();

	else if (argType == ArgType::ARGS_CLIENT) 
		Client(socket_file, pub_prime_mod, pub_prime_base, pub_FFS_mod, TESTING).Run();

	else if (argType == ArgType::ARGS_SERVER) 
		Server(socket_file, pub_prime_mod, pub_prime_base, pub_FFS_mod, TESTING).Run();

	return 0;
}
