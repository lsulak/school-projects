 #!/usr/bin/env python3

#SYN:xsulak04

import sys,re,codecs,operator

format_string = ""
input_string = ""
output_string = ""

#Name of output file
outputFile = ""
add_br = 0
isInputFile = 0
isOutputFile =  0

#2D list containing on each list: regexp command command...
format_list = []
format_list2 = []

#Print a help on stdout
def help():
	sys.stdout.write("Project 2. for subject IPP : variant SYN\n")
	sys.stdout.write('  Python script for highlighting syntax using HTML tags\n')
	sys.stdout.write('  Author: Ladislav Sulak\n')
	sys.stdout.write('  Login: xsulak04\n')
	sys.stdout.write("  Datum: 4/2014\n")
	sys.stdout.write('  Parameters:\n')
	sys.stdout.write("\t--help				\n")
	sys.stdout.write("\t--input=filename    if none, input from stdin\n")
	sys.stdout.write('\t--output=filename   if none, output on stdout\n')

#Print an err messages, and finish a program with error code
def errMsg(errNum):
	if(errNum == 1):
		sys.stderr.write("Parameter(s) has bad format\n")
		sys.stderr.write("Or some parameter(s) are presented multiple times\n")
		sys.stderr.write("Or you combined parameter '--help' with another\n")

	elif(errNum == 2):
		sys.stderr.write("Input file doesn't exist, or is corupted\n")

	elif(errNum == 3):
		sys.stderr.write("Error during opening output file for writing (rights for writing?)\n")

	elif(errNum == 4):
		sys.stderr.write("Input file doesn't have correct format (or encoding - must be UTF-8)\n")

	sys.exit(errNum)

#Parameter handling; function which checks parameters, opens an input file and input format file,
# finds a name of output file, prints help, or applies </ br> rule
def paramProcessing():
	global format_string
	global input_string
	global output_string
	
	global outputFile
	global add_br
	global isInputFile 
	global isOutputFile

	if(("--help" in sys.argv) and len(sys.argv) > 2):
		errMsg(1)

	#Compare 2 lists of arguments: list without duplicated elements, and full list
	if(len(list(set(sys.argv))) < len(sys.argv)):
		errMsg(1)

	if((len(sys.argv) == 2) and (sys.argv[1] == '--help')):
		help()
		sys.exit(0)

	position = 1
	while(position < len(sys.argv)):
		if(sys.argv[position][:8] == '--input='):
			isInputFile = 1
			try:
				inputFile = open(sys.argv[position][8:], "r", encoding = 'UTF-8') #input file must be in UTF-8 encoding
				input_string = inputFile.read()
				inputFile.close()
			except:
				errMsg(2)

		elif(sys.argv[position][:9] == '--format='):
			try:
				with open(sys.argv[position][9:], "r", encoding = 'UTF-8') as formatFile: #input file must be in UTF-8 encoding
					format_string = formatFile.read()
				formatFile.close()
			except:
				format_string = ""

		elif(sys.argv[position][:9] == '--output='):
			outputFile = sys.argv[position][9:]
			isOutputFile = 1

		elif(sys.argv[position][:4] == '--br'):
			add_br = 1
		
		else:
			errMsg(1)

		position = position + 1

	#If there is no parameter '--input=' then read from std input
	if(isInputFile == 0):
		input_string = sys.stdin.read()
	#At first, in output string will be input we just read
	output_string = input_string

#Save each line from format file to 1 list, separate it from tabs
def formatProcessing():
    for i in format_string.split('\n'):
      tmp =  i.split('\t' , 1)  

      #Substitute regexp from input to python's regexp
      tmp[0] = RegexpProcessing(tmp[0])	
      tmp2 = [tmp[0]]
      check = tmp

   	  #Check, if 'tag type' or regexp is missing
      if(len(check) == 2 and ((check[0] == "" and check[1] != "") or (check[0] != "" and check[1] == "")) ):
      	errMsg(4)
      #Check if 'tag type' is missing 
      elif(len(check) == 1 and check[0] != ""):
      	errMsg(4)

      format_list_tmp = []
      format_list_tmp.append(tmp2)
      del tmp[0]
      for j in tmp:
      	#Separate all rules in every line - with comma, space or tab 
      	tmp3 = re.split('[, \t]+' , j)
      	format_list_tmp.append(tmp3)
      	#Save to global list 
      	format_list.append(format_list_tmp)
 	
#Returns Python's regexp. Its finally saved into global list 'format_list'. This function represends Final state machine     								
def RegexpProcessing(regexpIN):
	#Checking if input expression is invalid
	if(re.search("[.]{2}|[|]{2}|[!]{2}", regexpIN)):
		errMsg(4)
	
	regexpOut = ""
	#Variables for skipping current character from input string
	skipChar = 1
	skip2 = 1
	#FSM - reads from input character by character
	for i in range(len(regexpIN)):
		#Character '.' in beginning of regexp, or in the end is not correct
 		if((i == 0 or i+1 == len(regexpIN)) and regexpIN[i] == "."):
 			if(regexpIN[i-1] != "%"):
 				errMsg(4)

 		if(regexpIN[i] == "." and i != 0 and regexpIN[i-1] != "%"):
 			skipChar = 0
 		#Negation - it's substituted !A into ![^A], and skip next character
 		if(regexpIN[i] == "!"):
 			if(i+1 < len(regexpIN) and re.search("[\"]|[a-z]|[A-Z]|[0-9]|\s" , regexpIN[i+1])):
 				if(regexpIN[i+1] == "\""):
 					regexpOut = regexpOut + ''.join("[^\\\"]")
 					skipChar = 0
 				else:
 					regexpOut = regexpOut + ''.join("[^" + regexpIN[i+1] + "]")
 					skipChar = 0
 			#if expression hasn't form !%s , !%a, !%d....  it's incorrect
 			elif(i+1 < len(regexpIN) and regexpIN[i+1] == "%" and i+2 < len(regexpIN)):
 				if(not re.search("s|a|d|l|L|w|W|t|n|[.]|[!]|[|]|[*]|[+]|[%]" ,regexpIN[i+2])):
 					errMsg(4)
 				if(regexpIN[i+2] == "a"):   # expression with form !%a will represends nothing ( !%a =>  [.])
 					regexpOut = "[.]"
 					skipChar = 0 
 					skip2 = 0
 			else:
 				skipChar = 0
 		elif(regexpIN[i] == "\\"):  # '\' -> '\\'
 			regexpOut =  regexpOut + "\\\\"

 		elif(regexpIN[i] == "(" and (i+1 != len(regexpIN)) and regexpIN[i+1] == ")"): # "()"  - empty inside is an incorrect input regexp
 	 		errMsg(4) 

 	 	#These special characters will be transformed:   A  => \A   (in case that prevorious char is not '%')
 		elif(regexpIN[i] == "$" or regexpIN[i] == "^" or regexpIN[i] == "[" or regexpIN[i] == "]" or regexpIN[i] == "?" or regexpIN[i] == "{" or regexpIN[i] == "}" or regexpIN[i] == "/" ):
 			if(i == 0):
 				regexpOut = regexpOut + "\\" + regexpIN[i]
 			elif(i >= 1 and regexpIN[i-1] != "%"):
 				regexpOut = regexpOut + "\\" + regexpIN[i]
 		#Another special kind of regexp - '%A' will be transfered, next character is skipped
 		elif(regexpIN[i] == "%" and i+1 < len(regexpIN)):
 			if(regexpIN[i+1] == "s"):		# %s => \s   , !%s => [^\s]
 				if(i != 0  and regexpIN[i-1] == "!"):
 					regexpOut = regexpOut + ''.join('[^\s]')
 				else:
 					regexpOut = regexpOut + ''.join('\s')
 				skipChar = 0

 			elif(regexpIN[i+1] == "a"):		# %a => .   , !%a => [^.]
 				if(i != 0  and regexpIN[i-1] == "!"):
 					regexpOut = regexpOut + ''.join('[^.]')
 				else:
 					regexpOut = regexpOut + ''.join('.')
 				skipChar = 0

 			elif(regexpIN[i+1] == "d"):		# %d => \d   , !%d => [^\d]
 				if(i != 0  and regexpIN[i-1] == "!"):
 					regexpOut = regexpOut + ''.join('[^\d]')
 				else:
 					regexpOut = regexpOut + ''.join('\d')
 				skipChar = 0
 
 			elif(regexpIN[i+1] == "l"):		# %l => [a-z]   , !%l => [^[a-z]]
 				if(i != 0  and regexpIN[i-1] == "!"):
 					regexpOut = regexpOut + ''.join('[^[a-z]]')
 				else:
 					regexpOut = regexpOut + ''.join('[a-z]')
 				skipChar = 0
 				
 			elif(regexpIN[i+1] == "L"):		# %L => [A-Z]  , !%L => [^[A-Z]]
 				if(i != 0  and regexpIN[i-1] == "!"):
 					regexpOut = regexpOut + ''.join('[^[A-Z]]')
 				else:
 					regexpOut = regexpOut + ''.join('[A-Z]')
 				skipChar = 0
 				
 			elif(regexpIN[i+1] == "w"):		# %w => [a-z][A-Z]  , !%w => [^[a-z][A-Z]]
 				regexpOut = regexpOut + ''.join('[a-z]|[A-Z]')
 				skipChar = 0
 				
 			elif(regexpIN[i+1] == "W"):		#  %W => [a-z]|[A-Z]|[0-9]  , !%w => [^[a-z]|[A-Z]|[0-9]]
 				if(i != 0  and regexpIN[i-1] == "!"):
 					regexpOut = regexpOut + ''.join('[^[a-z]|[A-Z]|[0-9]]')
 				else:
 					regexpOut = regexpOut + ''.join('[a-z]|[A-Z]|[0-9]')
 				skipChar = 0
 				
 			elif(regexpIN[i+1] == "t"):		# %t => \t  , !%t  => [^\t]
 				if(i != 0  and regexpIN[i-1] == "!"):
 					regexpOut = regexpOut + ''.join('[^\t]')
 				else:
 					regexpOut = regexpOut + ''.join('\t')
 				skipChar = 0
 				
 			elif(regexpIN[i+1] == "n"):		# %n => \n , !%n  => [^\n]
 				if(i != 0  and regexpIN[i-1] == "!"):
 					regexpOut = regexpOut + ''.join('[^\n]')
 				else:
 					regexpOut = regexpOut + ''.join('\n')
 				skipChar = 0
 				
 			elif(regexpIN[i+1] == "."):		# %. => [.] , !%.  => [^.]
 				if(i != 0  and regexpIN[i-1] == "!"):
 					regexpOut = regexpOut + ''.join('[^[.]]')
 				else:
 					regexpOut = regexpOut + ''.join('[.]')
 				skipChar = 0
 				
 			elif(regexpIN[i+1] == "|"):		# %| => [|] , !%|  => [^[|]]
 				if(i != 0  and regexpIN[i-1] == "!"):
 					regexpOut = regexpOut + ''.join('[^[|]]')
 				else:
 					regexpOut = regexpOut + ''.join('[|]')
 				skipChar = 0
 				
 			elif(regexpIN[i+1] == "!"):		# %! => [!] , !%!  => [^[|]]
 				if(i != 0  and regexpIN[i-1] == "!"):
 					regexpOut = regexpOut + ''.join('[^[!]]')
 				else:
 					regexpOut = regexpOut + ''.join('[!]')
 				skipChar = 0
 				
 			elif(regexpIN[i+1] == "*"):		# %* => [*] , !%*  => [^[*]]
 				if(i != 0  and regexpIN[i-1] == "!"):
 					regexpOut = regexpOut + ''.join('[^[*]]')
 				else:
 					regexpOut = regexpOut + ''.join('[*]')
 				skipChar = 0
 				
 			elif(regexpIN[i+1] == "+"):		# %+ => [+] , !%+  => [^[+]]
 				if(i != 0  and regexpIN[i-1] == "!"):
 					regexpOut = regexpOut + ''.join('[^[+]]')
 				else:
 					regexpOut = regexpOut + ''.join('[+]')
 				skipChar = 0
 				
 			elif(regexpIN[i+1] == "("):		# %( => [(] , !%(  => [^[(]]
 				if(i != 0  and regexpIN[i-1] == "!"):
 					regexpOut = regexpOut + ''.join('[^[(]]')
 				else:
 					regexpOut = regexpOut + ''.join('[(]')
 				skipChar = 0
 				
 			elif(regexpIN[i+1] == ")"):		# %) => [)] , !%) => [^[)]]
 				if(i != 0  and regexpIN[i-1] == "!"):
 					regexpOut = regexpOut + ''.join('[^[)]]')
 				else:
 					regexpOut = regexpOut + ''.join('[)]')
 				skipChar = 0
 			
 			elif(regexpIN[i+1] == "%"):		# %% => [%] , !%%  => [^[%]]
 				if(i != 0  and regexpIN[i-1] == "!"):
 					regexpOut = regexpOut + ''.join('[^[%]]')
 				else:
 					regexpOut = regexpOut + ''.join('[%]')
 				skipChar = 0
 			
 			else:
 				errMsg(4)
 		#Rozsireni -  bonus extension - NQS - just a 4 rules for reduction, its checks next char from input, and last char in output regexp.
 		#Characters '+' and '*' can be multiple times. After reduction there is everytime '*', just in 1 case is '+' - if there is no '*', and 1+ chars '+'
 		elif(regexpIN[i] == "+" and len(regexpOut) > 0):
 			if(regexpOut[len(regexpOut)-1] == "+"):		# A++ => A+
 				pass
 			elif(regexpOut[len(regexpOut) - 1] == "*"):	# A*+ => A*
 				regexpOut[len(regexpOut)-1] == "*"
 			else:
 				regexpOut = regexpOut + "+" 

 		elif(regexpIN[i] == "*" and len(regexpOut) > 0):
 			if(regexpOut[len(regexpOut)-1] == "*"):		# A** => A*
 				pass
 			elif(regexpOut[len(regexpOut) - 1] == "+"):
 				regexpOut[len(regexpOut)-1] == "*"		# A+* => A*
 			else:
 				regexpOut = regexpOut + "*"
 	 	#Add to output string next character, or skip him
 		else:
 			if(skipChar == 1 and ord(regexpIN[i]) >= 32):
 				regexpOut = regexpOut + ''.join(regexpIN[i])
 			
 			elif(skipChar == 0 and skip2 == 0 and regexpIN[i] == "a"):
 				skipChar = 1
 				skip2 = 1

 			else:
 				skipChar = 1
	#print(regexpOut)
	return regexpOut

#From global list 'format_list' we will make another, 'format_list2', in which there will be regexp, start and end tags, 
#          and number - for save order of tags from input format file (from top to bottom, from left to right)
def commandProcessing():
	count = 1
	tmp = []
	tmp2 = []
	notDuplicitList = []
	#If there are some commands same, remove all duplicities , for example: bold,bold,bold -> bold
	for j in format_list:
		tmp2.append(j[0])
		for k in j[1]:
			# color:000000, color:111111, color:333333 -> color:333333, so only last color
			if(re.search('color:', k[:6]) ):
				cnt = 0
				for t in tmp:
					if(re.search("color:", t)):
						del tmp[cnt]
					cnt = cnt + 1
			# same with 'size:'
			elif(re.search('size:', k[:5])): 
				cnt = 0
				for t in tmp:
					if(re.search('size:', t)):
						del tmp[cnt]
					cnt = cnt + 1
			if k not in tmp:
				tmp.append(k)

		tmp2.append(tmp)
		notDuplicitList.append(tmp2)
		tmp2 = []
		tmp = []
	for i in notDuplicitList:
		count = 1
		for p1 in i[1]:
			if(p1 == "bold" or p1 == "italic" or p1 == "underline"):
				p1 = "<" + p1[0] + ">"
				p2 = "</" + p1[1] + ">"
			elif(p1 == "teletype"):
				p1 = "<tt>"
				p2  = "</tt>"
			elif(p1[:6] == "color:"):
				if(re.search( '^[0-9A-Fa-f]{6}' , p1[6:])):
					if(len(p1[6:]) != 6):
						errMsg(4)
					p1 = "<font " + p1[:5] + "=#" + p1[6:] + ">"
					p2 = "</font>" 
				else:
					errMsg(4)
			elif(p1[:5] == "size:"):
				if(re.search('^[1-7]{1}', p1[5:])):
					p1 = "<font " + p1[:4] + "=" + p1[5:] + ">"
					p2 = "</font>"
				else:
					errMsg(4)
			elif(p1 == ""):
				continue
			else:
				errMsg(4)

			addPositions(i[0][0], p1, p2, count)
			count = count + 1

#Function for adding positions of tags - every tag has 2 position - begin and end positions, as we have start and end tags
def addPositions(regexp, begin, end, count):
	global output_string
	beginPos = 0
	#To global list 'format_list2' we will save everything neccessary: text, begin position, end position, and 'count' - a number from parameter
	i = re.compile(regexp, flags = re.DOTALL)
	for m in i.finditer(input_string):
		beginPos = m.start()
		tmp = [m.group(), beginPos, begin, end, count]
		if(tmp[0] != ""):
			format_list2.append(tmp)

#Add tags from global list 'format_list2', [0] - regexp, [1] and [2] - indexes , [3] and [4] - tags , [5] - number (of  position from input format file)
def addTags():
	global output_string
	global input_string
	global format_list2
	#Temporary variables for tagging
	reversed_commandList = []
	FINAL = []
	BUG = []
	tmp = ""
	tmp2 = ""
	counter = 1
	#Lets sort format list, according to 1.(second actually) element in list (its integer - begin position)
	format_list2 = sorted(format_list2, key=lambda x: x[1]) 
	#Cycle for saving 'begin/end position of tag, a number of tag in which order should be, and the tag itself'
	for i in format_list2:  
		if(i[4] == 1):
			for j in format_list2: 
				if(i[0] == j[0] and i[1] == j[1] ):
					tmp = tmp + j[2] 
					tmp2 =  j[3] + tmp2
			tmpList = [i[1], counter, tmp]
			counter = counter + 1
			tmpList2 = [len(i[0]) + i[1], counter, tmp2]
			reversed_commandList.append(tmpList)
			reversed_commandList.append(tmpList2)
			tmp = ""
			tmp2 = ""
			counter = counter + 1
	
	#Sort list according by 0. and 1. numbers from list in reverse order, so we will process from ending (offsets-positions will be unchanged)
	reversed_commandList = sorted(reversed_commandList, key=lambda x: x[:2], reverse = True)
	#To another list we will save begin position, and tag 
	for p in reversed_commandList:
		BUG.append([p[0],p[2]])
	#To another list we will save copy of prevorious list, but without duplicit elements
	for l in BUG:
		for m in reversed_commandList:
			if l not in FINAL:
				FINAL.append(l)
	#Convert output string to list, every character will be separately, and in cycle add tags to position (from ending)		
	output_toList = list(output_string)
	for k in FINAL:
		output_toList.insert(k[0], k[1])
	output_string = ''.join(output_toList) #convert back to string

#Function for writing to file, with UTF-8
def writeToFile():
	if(isOutputFile == 0):
		sys.stdout.write(output_string)
	else:
		try:
			output = codecs.open(outputFile, "w", "UTF-8")
			output.write(output_string)
		except:
			errMsg(3)

paramProcessing()
formatProcessing()
commandProcessing()
addTags()

#If there is parameter '--br', add <br /> tag on every EOL
if (add_br == 1):
	output_string = output_string.replace("\n", "<br />\n")

writeToFile()