#!/usr/bin/python

import os
import subprocess

listProc = [3,7,15,31,63]


for proc in listProc:
	for x in range(0, 10):
		arg1 = str((proc + 1) / 2)
		arg2 = str(proc)
		os.system("./test " + arg1 + " " + arg2)
	f1=open('results.log', 'a+')
	f1.write('\nNEXT\n\n')
	f1.close()
