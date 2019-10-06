#!/bin/bash

# Author: Ladislav Sulak <xsulak04@stud.fit.vutbr.cz>, Cryptography, project 1 - Secured Channel

run_server() {
	echo "[Testing] Server started, see 'logs_server.out'"
	./sec_channel -s -t
	echo "[Testing] Server ends"
}

run_client() {
	echo "[Testing] Client started, see 'logs_client.out'"
	sleep 1.5; ./sec_channel -c -t
	echo "[Testing] Client ends"
}

##### RUNNING APP AND GENERATING LOGS #####
NUMBER_OF_TESTS=20
ROUNDS_SUCC=1

cd "../"

make clean; make all

if [ ! -d "logs" ]; then
	mkdir "logs"
fi

for i in $(seq 1 $NUMBER_OF_TESTS);
do
	echo "-------------- TEST NUMBER ${i} / $NUMBER_OF_TESTS --------------"
	run_server & run_client

	cd "tests"

	##### TESTS ARE WRITTEN IN PYTHON WITH ALL INFORMATION #####
	python test.py

	exit_code=$?
	if [[ $exit_code == 2 ]]; then
		ROUNDS_SUCC=0
		cd "../"
		continue

	elif [[ $exit_code == 0 ]]; then
		cd "../"
		continue

	else
		exit 1
	fi

	
done

if [[ $ROUNDS_SUCC == 0 ]]; then
	echo "----- AUTHENTIZATION HASN'T PASS AT LEAST FOR 1 ROUND -----"
fi
echo "-------------- ALL TESTS PASSED --------------"
