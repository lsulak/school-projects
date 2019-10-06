#!/usr/bin/env bash

# =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
# IPP - syn - veřejné testy - 2013/2014 + mala zmena - treba vytvorit zlozky moje,diff,public
#                do public vlozit vsetky testy, aj zdrojove subory. Skript kontroluje ci sa navratove hodnoty zhoduju
#                 a do priecinku diff uklada rozdiely medzi vystupmi
# =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
# Činnost: 
# - vytvoří výstupy studentovy úlohy v daném interpretu na základě sady testů
# =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

TASK=syn
#INTERPRETER="php -d open_basedir=\"\""
#EXTENSION=php
INTERPRETER=python3
EXTENSION=py

# cesty ke vstupním a výstupním souborům
LOCAL_IN_PATH="./"
#LOCAL_IN_PATH="" #Alternative 1
#LOCAL_IN_PATH=`pwd`"/" #Alternative 2
LOCAL_OUT_PATH="../moje/"
#LOCAL_OUT_PATH="" #Alternative 1
#LOCAL_OUT_PATH=`pwd`"/" #Alternative 2
# cesta pro ukládání chybového výstupu studentského skriptu
LOG_PATH="../moje/"
DIFF_PATH="../diff/"    

# test01: Argument error; Expected output: test01; Expected return code: 1
$INTERPRETER $TASK.$EXTENSION --error 2> ${LOG_PATH}test01.err
echo -n $? > ${LOG_PATH}test01.!!!

if [ 1 == $(cat ${LOG_PATH}test01.!!!) ] ; then
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>" T01 OK
else
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>" T01 BAD
fi

# test02: Input error; Expected output: test02; Expected return code: 2
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}nonexistent --output=${LOCAL_OUT_PATH}test02.out 2> ${LOG_PATH}test02.err
echo -n $? > ${LOG_PATH}test02.!!!
if [ 2 == $(cat ${LOG_PATH}test02.!!!) ] ; then
    #diff ./test02.out ${LOCAL_OUT_PATH}test02.out > ${DIFF_PATH}02.out
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>" T02 OK
else
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>" T02 BAD
fi

# test03: Output error; Expected output: test03; Expected return code: 3
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}empty --output=nonexistent/${LOCAL_OUT_PATH}test03.out 2> ${LOG_PATH}test03.err
echo -n $? > ${LOG_PATH}test03.!!!
if [ 3 == $(cat ${LOG_PATH}test03.!!!) ] ; then
    #diff ./test03.out ${LOCAL_OUT_PATH}test03.out > ${DIFF_PATH}03.out
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>" T03 OK
else
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>" T03 BAD
fi

# test04: Format table error - nonexistent parameter; Expected output: test04; Expected return code: 4
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}empty --output=${LOCAL_OUT_PATH}test04.out --format=${LOCAL_IN_PATH}error-parameter.fmt 2> ${LOG_PATH}test04.err
echo -n $? > ${LOG_PATH}test04.!!!
if [ 4 == $(cat ${LOG_PATH}test04.!!!) ] ; then
    #diff ./test04.out ${LOCAL_OUT_PATH}test04.out > ${DIFF_PATH}04.out
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>" T04 OK
else
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>" T04 BAD
fi

# test05: Format table error - size; Expected output: test05; Expected return code: 4
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}empty --output=${LOCAL_OUT_PATH}test05.out --format=${LOCAL_IN_PATH}error-size.fmt 2> ${LOG_PATH}test05.err
echo -n $? > ${LOG_PATH}test05.!!!
if [ 4 == $(cat ${LOG_PATH}test05.!!!) ] ; then
   # diff ./test05.out ${LOCAL_OUT_PATH}test05.out > ${DIFF_PATH}05.out
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>" T05 OK
else
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>" T05 BAD
fi

# test06: Format table error - color; Expected output: test06; Expected return code: 4
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}empty --output=${LOCAL_OUT_PATH}test06.out --format=${LOCAL_IN_PATH}error-color.fmt 2> ${LOG_PATH}test06.err
echo -n $? > ${LOG_PATH}test06.!!!
if [ 4 == $(cat ${LOG_PATH}test06.!!!) ] ; then
    #diff ./test06.out ${LOCAL_OUT_PATH}test06.out > ${DIFF_PATH}06.out
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>" T06 OK
else
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>" T06 BAD
fi

# test07: Format table error - RE syntax; Expected output: test07; Expected return code: 4
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}empty --output=${LOCAL_OUT_PATH}test07.out --format=${LOCAL_IN_PATH}error-re.fmt 2> ${LOG_PATH}test07.err
echo -n $? > ${LOG_PATH}test07.!!!
if [ 4 == $(cat ${LOG_PATH}test07.!!!) ] ; then
    #diff ./test07.out ${LOCAL_OUT_PATH}test07.out > ${DIFF_PATH}07.out
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>" T07 OK
else
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>" T07 BAD
fi

# test08: Empty files; Expected output: test08; Expected return code: 0
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}empty --output=${LOCAL_OUT_PATH}test08.out --format=${LOCAL_IN_PATH}empty 2> ${LOG_PATH}test08.err
echo -n $? > ${LOG_PATH}test08.!!!
if [ 0 == $(cat ${LOG_PATH}test08.!!!) ] ; then
    diff ./test08.out ${LOCAL_OUT_PATH}test08.out > ${DIFF_PATH}08.out
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>" T08 OK
else
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>" T08 BAD
fi

# test09: Format parameters; Expected output: test09; Expected return code: 0
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}basic-parameter.in --format=${LOCAL_IN_PATH}basic-parameter.fmt > ${LOCAL_OUT_PATH}test09.out 2> ${LOG_PATH}test09.err
echo -n $? > ${LOG_PATH}test09.!!!
if [ 0 == $(cat ${LOG_PATH}test09.!!!) ] ; then
    diff ./test09.out ${LOCAL_OUT_PATH}test09.out > ${DIFF_PATH}09.out
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>" T09 OK
else
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>" T09 BAD
fi

# test10: Argument swap; Expected output: test10; Expected return code: 0
$INTERPRETER $TASK.$EXTENSION --format=${LOCAL_IN_PATH}basic-parameter.fmt --output=${LOCAL_OUT_PATH}test10.out --input=${LOCAL_IN_PATH}basic-parameter.in 2> ${LOG_PATH}test10.err
echo -n $? > ${LOG_PATH}test10.!!!
if [ 0 == $(cat ${LOG_PATH}test10.!!!) ] ; then
    diff ./test10.out ${LOCAL_OUT_PATH}test10.out > ${DIFF_PATH}10.out
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>" T10 OK
else
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>" T10 BAD
fi

# test11: Standard input/output; Expected output: test11; Expected return code: 0
$INTERPRETER $TASK.$EXTENSION --format=${LOCAL_IN_PATH}basic-parameter.fmt >${LOCAL_OUT_PATH}test11.out < ${LOCAL_IN_PATH}basic-parameter.in 2> ${LOG_PATH}test11.err
echo -n $? > ${LOG_PATH}test11.!!!
if [ 0 == $(cat ${LOG_PATH}test11.!!!) ] ; then
    diff ./test11.out ${LOCAL_OUT_PATH}test11.out > ${DIFF_PATH}11.out
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>" T11 OK
else
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>" T11 BAD
fi

# test12: Basic regular expressions; Expected output: test12; Expected return code: 0
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}basic-re.in --output=${LOCAL_OUT_PATH}test12.out --format=${LOCAL_IN_PATH}basic-re.fmt 2> ${LOG_PATH}test12.err
echo -n $? > ${LOG_PATH}test12.!!!
if [ 0 == $(cat ${LOG_PATH}test12.!!!) ] ; then
    diff ./test12.out ${LOCAL_OUT_PATH}test12.out > ${DIFF_PATH}12.out
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>" T12 OK
else
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>" T12 BAD
fi

# test13: Special regular expressions; Expected output: test13; Expected return code: 0
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}special-re.in --output=${LOCAL_OUT_PATH}test13.out --format=${LOCAL_IN_PATH}special-re.fmt 2> ${LOG_PATH}test13.err
echo -n $? > ${LOG_PATH}test13.!!!
if [ 0 == $(cat ${LOG_PATH}test13.!!!) ] ; then
    diff ./test13.out ${LOCAL_OUT_PATH}test13.out > ${DIFF_PATH}13.out
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>" T13 OK
else
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>" T13 BAD
fi

# test14: Special RE - symbols; Expected output: test14; Expected return code: 0
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}special-symbols.in --output=${LOCAL_OUT_PATH}test14.out --format=${LOCAL_IN_PATH}special-symbols.fmt 2> ${LOG_PATH}test14.err
echo -n $? > ${LOG_PATH}test14.!!!
if [ 0 == $(cat ${LOG_PATH}test14.!!!) ] ; then
    diff ./test14.out ${LOCAL_OUT_PATH}test14.out > ${DIFF_PATH}14.out
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>" T14 OK
else
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>" T14 BAD
fi

# test15: Negation; Expected output: test15; Expected return code: 0
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}negation.in --output=${LOCAL_OUT_PATH}test15.out --format=${LOCAL_IN_PATH}negation.fmt 2> ${LOG_PATH}test15.err
echo -n $? > ${LOG_PATH}test15.!!!
if [ 0 == $(cat ${LOG_PATH}test15.!!!) ] ; then
    diff ./test15.out ${LOCAL_OUT_PATH}test15.out > ${DIFF_PATH}15.out
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>" T15 OK
else
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>" T15 BAD
fi

# test16: Multiple format parameters; Expected output: test16; Expected return code: 0
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}multiple.in --output=${LOCAL_OUT_PATH}test16.out --format=${LOCAL_IN_PATH}multiple.fmt 2> ${LOG_PATH}test16.err
echo -n $? > ${LOG_PATH}test16.!!!
if [ 0 == $(cat ${LOG_PATH}test16.!!!) ] ; then
    diff ./test16.out ${LOCAL_OUT_PATH}test16.out > ${DIFF_PATH}16.out
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>" T16 OK
else
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>" T16 BAD
fi

# test17: Spaces/tabs in format parameters; Expected output: test17; Expected return code: 0
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}multiple.in --output=${LOCAL_OUT_PATH}test17.out --format=${LOCAL_IN_PATH}spaces.fmt 2> ${LOG_PATH}test17.err
echo -n $? > ${LOG_PATH}test17.!!!
if [ 0 == $(cat ${LOG_PATH}test17.!!!) ] ; then
    diff ./test17.out ${LOCAL_OUT_PATH}test17.out > ${DIFF_PATH}17.out
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>" T17 OK
else
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>" T17 BAD
fi

# test18: Line break tag; Expected output: test18; Expected return code: 0
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}newlines.in --output=${LOCAL_OUT_PATH}test18.out --format=${LOCAL_IN_PATH}empty --br 2> ${LOG_PATH}test18.err
echo -n $? > ${LOG_PATH}test18.!!!
if [ 0 == $(cat ${LOG_PATH}test18.!!!) ] ; then
    diff ./test18.out ${LOCAL_OUT_PATH}test18.out > ${DIFF_PATH}18.out
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>" T18 OK
else
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>" T18 BAD
fi

# test19: Overlap; Expected output: test19; Expected return code: 0
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}overlap.in --output=${LOCAL_OUT_PATH}test19.out --format=${LOCAL_IN_PATH}overlap.fmt 2> ${LOG_PATH}test19.err
echo -n $? > ${LOG_PATH}test19.!!!
if [ 0 == $(cat ${LOG_PATH}test19.!!!) ] ; then
    diff ./test19.out ${LOCAL_OUT_PATH}test19.out > ${DIFF_PATH}19.out
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>" T19 OK
else
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>" T19 BAD
fi

# test20: Perl RE; Expected output: test20; Expected return code: 0
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}special-symbols.in --output=${LOCAL_OUT_PATH}test20.out --format=${LOCAL_IN_PATH}re.fmt 2> ${LOG_PATH}test20.err
echo -n $? > ${LOG_PATH}test20.!!!
if [ 0 == $(cat ${LOG_PATH}test20.!!!) ] ; then
    diff ./test20.out ${LOCAL_OUT_PATH}test20.out > ${DIFF_PATH}20.out
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>" T20 OK
else
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>" T20 BAD
fi

# test21: Example; Expected output: test21; Expected return code: 0
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}example.in --br --format=${LOCAL_IN_PATH}example.fmt > ${LOCAL_OUT_PATH}test21.out 2> ${LOG_PATH}test21.err
echo -n $? > ${LOG_PATH}test21.!!!
if [ 0 == $(cat ${LOG_PATH}test21.!!!) ] ; then
    diff ./test21.out ${LOCAL_OUT_PATH}test21.out > ${DIFF_PATH}21.out
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>" T21 OK
else
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>" T21 BAD
fi

# test22: Simple C program; Expected output: test22; Expected return code: 0
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}cprog.c --br --format=${LOCAL_IN_PATH}c.fmt > ${LOCAL_OUT_PATH}test22.out 2> ${LOG_PATH}test22.err
echo -n $? > ${LOG_PATH}test22.!!!
if [ 0 == $(cat ${LOG_PATH}test22.!!!) ] ; then
    diff ./test22.out ${LOCAL_OUT_PATH}test22.out > ${DIFF_PATH}22.out
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>" T22 OK
else
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>" T22 BAD
fi