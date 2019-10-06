#!/bin/bash
# Author: Martin Sakin 
# Date:	  2014-03-20
# Nezarucuju korektnost !!! Skript pouze porovnava tvoje vysledky s mojmi :)

# Do slozky "testovac" pridej svuj program "ftpclient" (tam kde je "testovac.sh")
# Spusteni > sh testovac.sh

NAMEFILE="./ftpclient"
NEWFILES="./temp/"
ORGFILES="./outputs/"

#Barvy
c_red=`tput setaf 1`
c_green=`tput setaf 2`
c_normal=`tput sgr0`

#porovnani a tisk vysledku
kontrola () {
	diff ${ORGFILES}test${TEST}.out ${NEWFILES}test${TEST}.out > /dev/null 2> /dev/null
	if [ $? -eq 0 ] ; then
	 res="${c_green}pass$c_normal"
	else
	 res="${c_red}FAIL$c_normal"
	fi
	 printf "   Test $TEST                  $res\n";
}

#vytvoreni pomocne slozky, vyprazdneni, pokud uz existuje
mkdir $NEWFILES > /dev/null 2> /dev/null
rm -f $NEWFILES*.out $NEWFILES*.err

#Kontrola existence souboru
if [ -f $NAMEFILE ] ; then
  printf "File $NAMEFILE exist\n"
else
  printf "File $NAMEFILE not exist !\n"
  exit
fi

# ======================= TESTOVANI =======================

printf "Chybne parametry (nemelo by se pripojovat)\n"

TEST="01"
$NAMEFILE ftp://:secret@ftp.fit.vutbr.cz:21/pub/ > ${NEWFILES}test${TEST}.out 2> ${NEWFILES}test${TEST}.err
kontrola

TEST="02"
$NAMEFILE ftp://anonymous:secret@:21/pub/ > ${NEWFILES}test${TEST}.out 2> ${NEWFILES}test${TEST}.err
kontrola

TEST="03"
$NAMEFILE ftp://ftp.fit.vutbr.cz/pub/ 21 > ${NEWFILES}test${TEST}.out 2> ${NEWFILES}test${TEST}.err
kontrola

TEST="04"
$NAMEFILE > ${NEWFILES}test${TEST}.out 2> ${NEWFILES}test${TEST}.err
kontrola

TEST="05"
$NAMEFILE anonymous:secret@ftp.fit.vutbr.cz/pub/usenet/comp/sources/games/v14/scrabble2/ > ${NEWFILES}test${TEST}.out 2> ${NEWFILES}test${TEST}.err
kontrola

TEST="06"
$NAMEFILE anonymous@ftp.fit.vutbr.cz/pub/usenet/comp/sources/ > ${NEWFILES}test${TEST}.out 2> ${NEWFILES}test${TEST}.err
kontrola

TEST="07"
$NAMEFILE ftp://@ftp.fit.vutbr.cz:21/pub/ > ${NEWFILES}test${TEST}.out 2> ${NEWFILES}test${TEST}.err
kontrola

TEST="08"
$NAMEFILE @ftp.fit.vutbr.cz:21/pub/usenet/comp/sources/games/v14/scrabble2 > ${NEWFILES}test${TEST}.out 2> ${NEWFILES}test${TEST}.err
kontrola

TEST="09"
$NAMEFILE @ftp.fit.vutbr.cz:/pub/usenet/comp/sources/games/v14/scrabble2 > ${NEWFILES}test${TEST}.out 2> ${NEWFILES}test${TEST}.err
kontrola

TEST="10"
$NAMEFILE ptf://ftp.fit.vutbr.cz/ > ${NEWFILES}test${TEST}.out 2> ${NEWFILES}test${TEST}.err
kontrola


printf "Spravnost vypisu - pripojovani k ftp\n"

TEST="11"
$NAMEFILE ftp://ftp.fit.vutbr.cz > ${NEWFILES}test${TEST}.out 2> ${NEWFILES}test${TEST}.err
kontrola

TEST="12"
$NAMEFILE ftp://anonymous:secret@ftp.fit.vutbr.cz:21/pub/systems/centos > ${NEWFILES}test${TEST}.out 2> ${NEWFILES}test${TEST}.err
kontrola

TEST="13"
$NAMEFILE ftp.linux.cz/pub/local/ > ${NEWFILES}test${TEST}.out 2> ${NEWFILES}test${TEST}.err
kontrola

TEST="14"
$NAMEFILE ftp://anonymous:secret@ftp.fit.vutbr.cz:21/etc/ > ${NEWFILES}test${TEST}.out 2> ${NEWFILES}test${TEST}.err
kontrola

TEST="15"
$NAMEFILE ftp://anonymous:secret@ftp.fit.vutbr.cz/bin/ > ${NEWFILES}test${TEST}.out 2> ${NEWFILES}test${TEST}.err
kontrola

TEST="16"
$NAMEFILE ftp://ftp.fit.vutbr.cz/pub/usenet/comp/sources/games/v14/scrabble2/ > ${NEWFILES}test${TEST}.out 2> ${NEWFILES}test${TEST}.err
kontrola

TEST="17"
$NAMEFILE ftp://anonymous:secret@ftp.fit.vutbr.cz/pub/usenet/comp/sources/ > ${NEWFILES}test${TEST}.out 2> ${NEWFILES}test${TEST}.err
kontrola

TEST="18"
$NAMEFILE ftp.fit.vutbr.cz/pub/usenet/comp/ > ${NEWFILES}test${TEST}.out 2> ${NEWFILES}test${TEST}.err
kontrola

TEST="19"
$NAMEFILE ftp://ftp.fit.vutbr.cz:21/pub/usenet/comp/sources/misc/v39/cwish > ${NEWFILES}test${TEST}.out 2> ${NEWFILES}test${TEST}.err
kontrola

TEST="20"
$NAMEFILE ftp://anonymous:secret@ftp.fit.vutbr.cz:21/pub/ > ${NEWFILES}test${TEST}.out 2> ${NEWFILES}test${TEST}.err
kontrola

TEST="21"
$NAMEFILE http://fit.vutbr.cz/ > ${NEWFILES}test${TEST}.out 2> ${NEWFILES}test${TEST}.err
kontrola

TEST="22"
$NAMEFILE ftp://ftp.mgo.opava.cz/ > ${NEWFILES}test${TEST}.out 2> ${NEWFILES}test${TEST}.err
kontrola

TEST="23"
$NAMEFILE ftp.altap.cz > ${NEWFILES}test${TEST}.out 2> ${NEWFILES}test${TEST}.err
kontrola

TEST="24"
$NAMEFILE ftp.altap.cz/pub/altap/salamand/ > ${NEWFILES}test${TEST}.out 2> ${NEWFILES}test${TEST}.err
kontrola

printf "KONEC TESTU \n"

