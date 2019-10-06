#!/usr/bin/python

"""
 @author		Ladislav Sulak (xsulak04@stud.fit.vutbr.cz)
 
 login 			xsulak04			
 
 @date			2017-04
 
 @file 			test.py
 
 @brief			Cryptography: Project 1 - Secure Channel.
 
 @details		Test part of the application using Unix domain socket. 
 				Communication goes via secured channel. 
 				It uses:
 					* Diffie-Hellman for secure key exchange,
 					
 					* AES for encrypting (without autentization),
 						
 					* Feige-Fiat-Shamir identification protocol,
 					
 					* SHA256 hash function for maintaining integrity. Server responds with 
 					responding msg + hash (of received msg) when the secured channel is established.
"""

import os
import re
import sys


##### Tests are focused on analyzing log files - 1 for client and 1 for server 
#####
##### PLEASE RUN IT WITH SCRIPT: 'run_test.sh'

# Processing file, name given from an input parameter. Function returns shared key and one or more hashes if found.  
def process_file(file_name):
	pattern_sk = re.compile("Shared secret key calculated:")
	pattern_hash = re.compile("message hash")
	pattern_auth = re.compile("WELCOME_YOU_ARE_AUTENTICATED")
	pattern_error = re.compile("(error|bad|except)")
	pattern_r_not_auth = re.compile("REFUSED_YOU_HAVE_NO_POWER_HERE")

	shared_key = str()
	hashes = list()
	authenticated = False
	round_not_auth = False

	with open(file_name, 'r') as server_file:
		for line in server_file:
			res_sk = re.search(pattern_sk, line)
			res_hash = re.search(pattern_hash, line)
			res_auth = re.search(pattern_auth, line)
			res_err = re.search(pattern_error, line)
			r_not_auth = re.search(pattern_r_not_auth, line)

			if res_sk:
				shared_key = line[line.rfind(' '):].strip()

			elif res_hash:
				hash_found = line[line.rfind(' '):].strip()
				hashes.append(hash_found)

			elif res_auth:
				authenticated = True

			elif r_not_auth:
				round_not_auth = True

			elif res_err:
				print("[Testing] Found error in log file ... FAIL:")
				print("\tIN: " + line)
				sys.exit(1)

	return shared_key, hashes, authenticated, round_not_auth


def main():
	
	if not os.path.exists("..logs/"):
		os.makedirs("..logs/")

	server_shared_key, server_hashes, autenticatedMsg, round_not_auth = process_file("../logs/logs_server.out")
	client_shared_key, client_hashes, isAutenticated, round_not_auth  = process_file("../logs/logs_client.out")

	# Comparison of shared secret keys.
	if (server_shared_key == client_shared_key):
		print("[Testing] Shared secret keys are equal ... OK")

	else:
		print("[Testing] Shared secret keys are not equal ... FAIL:")
		print(server_shared_key)
		print(client_shared_key)
		sys.exit(1)

	# Comparison of found hashes.
	if (client_hashes and server_hashes and set(client_hashes) == set(server_hashes) and
		len(client_hashes) == len(server_hashes)):
		print("[Testing] Message hashes are equal ... OK")

	else:
		print("[Testing] Message hashes are not equal ... FAIL:")
		print("Client: " + str(client_hashes))
		print("Server: " + str(server_hashes))
		sys.exit(1)

	# Client should be autenticated and Server should inform us in the log file.
	if (isAutenticated and autenticatedMsg):
		print("[Testing] Client is authenticated ... OK")

	else:
		print("[Testing] Client should be authenticated  ... FAIL:")
		sys.exit(1)

	if round_not_auth:
		sys.exit(2)

	print("[Testing] Tests passed")

if __name__ == "__main__":
	main()
