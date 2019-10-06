#!/usr/bin/env bash

# =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
# IPP - xtd - veřejné testy - 2013/2014
# =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
# Činnost: 
# - vytvoří výstupy studentovy úlohy v daném interpretu na základě sady testů
# =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

TASK=xtd
INTERPRETER="php -d open_basedir=\"\""
EXTENSION=php
#INTERPRETER=python3
#EXTENSION=py

# cesty ke vstupním a výstupním souborům
LOCAL_IN_PATH="./"
#LOCAL_IN_PATH="" #Alternative 1
#LOCAL_IN_PATH=`pwd`"/" #Alternative 2
LOCAL_OUT_PATH="./"
#LOCAL_OUT_PATH="" #Alternative 1
#LOCAL_OUT_PATH=`pwd`"/" #Alternative 2
# cesta pro ukládání chybového výstupu studentského skriptu
LOG_PATH="./"


# test01: Zobrazi napovedu; Expected output: test01.out; Expected return code: 0
$INTERPRETER $TASK.$EXTENSION --help > ${LOCAL_OUT_PATH}test01.out 2> ${LOG_PATH}test01.err
echo -n $? > test01.!!!

# test02: Prazdny Root - vystupni soubor bude prazdny; Expected output: test02.out; Expected return code: 0
$INTERPRETER $TASK.$EXTENSION --output=${LOCAL_OUT_PATH}test02.out < ${LOCAL_IN_PATH}test02.in 2> ${LOG_PATH}test02.err
echo -n $? > test02.!!!

# test03: Generovani tabulek z plneho XML - neni omezeni na pocet sloupcu vzniklych ze stejnojmenych podelementu; Expected output: test03.out; Expected return code: 0
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}test03.in > ${LOCAL_OUT_PATH}test03.out 2> ${LOG_PATH}test03.err
echo -n $? > test03.!!!

# test04: Pocet sloupcu vzniklich ze stejnojmenych podelementu bude maximalne 2; Expected output: test04.out; Expected return code: 0
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}test04.in --output=${LOCAL_OUT_PATH}test04.out --etc=2 2> ${LOG_PATH}test04.err
echo -n $? > test04.!!!

# test05: Vsechny podelementy by meli vest na novy sloupec v tabulce s nazvem podelementu; Expected output: test05.out; Expected return code: 0
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}test05.in --output=${LOCAL_OUT_PATH}test05.out --etc=0 2> ${LOG_PATH}test05.err
echo -n $? > test05.!!!

# test06: Nejsou vytvareny sloupce z attributu; Expected output: test06.out; Expected return code: 0
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}test06.in --output=${LOCAL_OUT_PATH}test06.out -a 2> ${LOG_PATH}test06.err
echo -n $? > test06.!!!

# test07: Pokud se nejaky podelement objevi v danem elementu vicekrat, bude se chapat jako jeden; Expected output: test07.out; Expected return code: 0
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}test07.in --output=${LOCAL_OUT_PATH}test07.out -b 2> ${LOG_PATH}test07.err
echo -n $? > test07.!!!

# test08: Sloupce tabulek vznikaji inkrementalne podle obsahu stejnojmenych elementu. Pred vystup bude generovana zakomentovana hlavicka souboru; Expected output: test08.out; Expected return code: 0
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}test08.in --output=${LOCAL_OUT_PATH}test08.out --etc=2 --header='Takto pak vypadá hlavička výstupního souboru' 2> ${LOG_PATH}test08.err
echo -n $? > test08.!!!

# test09: Testovat se musi i spravnost zadanych parametru; Expected output: test09.out; Expected return code: 1
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}test09.in --output=${LOCAL_OUT_PATH}test09.out --etc=2 -b 2> ${LOG_PATH}test09.err
echo -n $? > test09.!!!

# test10: Vyhodi chybu, protoze dojde ke kolizi jmen attributu a subelementu; Expected output: test10.out; Expected return code: 90
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}test10.in --output=${LOCAL_OUT_PATH}test10.out --etc=2 2> ${LOG_PATH}test10.err
echo -n $? > test10.!!!

# test11: Generovani relaci z XML souboru; Expected output: test11.out; Expected return code: 0
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}test11.in --output=${LOCAL_OUT_PATH}test11.out -g 2> ${LOG_PATH}test11.err
echo -n $? > test11.!!!

# test12: Generovani relaci z XML souboru s parametrem --etc; Expected output: test12.out; Expected return code: 0
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}test12.in --output=${LOCAL_OUT_PATH}test12.out --etc=2 -g 2> ${LOG_PATH}test12.err
echo -n $? > test12.!!!

