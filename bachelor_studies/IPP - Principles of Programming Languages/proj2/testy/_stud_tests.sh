#!/usr/bin/env bash

# =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
# IPP - syn - veřejné testy - 2013/2014
# =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
# Činnost: 
# - vytvoří výstupy studentovy úlohy v daném interpretu na základě sady testů
# =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

TASK=syn
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


# test01: Argument error; Expected output: test01; Expected return code: 1
$INTERPRETER $TASK.$EXTENSION --error 2> ${LOG_PATH}test01.err
echo -n $? > test01.!!!

# test02: Input error; Expected output: test02; Expected return code: 2
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}nonexistent --output=${LOCAL_OUT_PATH}test02.out 2> ${LOG_PATH}test02.err
echo -n $? > test02.!!!

# test03: Output error; Expected output: test03; Expected return code: 3
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}empty --output=nonexistent/${LOCAL_OUT_PATH}test03.out 2> ${LOG_PATH}test03.err
echo -n $? > test03.!!!

# test04: Format table error - nonexistent parameter; Expected output: test04; Expected return code: 4
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}empty --output=${LOCAL_OUT_PATH}test04.out --format=${LOCAL_IN_PATH}error-parameter.fmt 2> ${LOG_PATH}test04.err
echo -n $? > test04.!!!

# test05: Format table error - size; Expected output: test05; Expected return code: 4
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}empty --output=${LOCAL_OUT_PATH}test05.out --format=${LOCAL_IN_PATH}error-size.fmt 2> ${LOG_PATH}test05.err
echo -n $? > test05.!!!

# test06: Format table error - color; Expected output: test06; Expected return code: 4
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}empty --output=${LOCAL_OUT_PATH}test06.out --format=${LOCAL_IN_PATH}error-color.fmt 2> ${LOG_PATH}test06.err
echo -n $? > test06.!!!

# test07: Format table error - RE syntax; Expected output: test07; Expected return code: 4
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}empty --output=${LOCAL_OUT_PATH}test07.out --format=${LOCAL_IN_PATH}error-re.fmt 2> ${LOG_PATH}test07.err
echo -n $? > test07.!!!

# test08: Empty files; Expected output: test08; Expected return code: 0
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}empty --output=${LOCAL_OUT_PATH}test08.out --format=${LOCAL_IN_PATH}empty 2> ${LOG_PATH}test08.err
echo -n $? > test08.!!!

# test09: Format parameters; Expected output: test09; Expected return code: 0
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}basic-parameter.in --format=${LOCAL_IN_PATH}basic-parameter.fmt > ${LOCAL_OUT_PATH}test09.out 2> ${LOG_PATH}test09.err
echo -n $? > test09.!!!

# test10: Argument swap; Expected output: test10; Expected return code: 0
$INTERPRETER $TASK.$EXTENSION --format=${LOCAL_IN_PATH}basic-parameter.fmt --output=${LOCAL_OUT_PATH}test10.out --input=${LOCAL_IN_PATH}basic-parameter.in 2> ${LOG_PATH}test10.err
echo -n $? > test10.!!!

# test11: Standard input/output; Expected output: test11; Expected return code: 0
$INTERPRETER $TASK.$EXTENSION --format=${LOCAL_IN_PATH}basic-parameter.fmt >${LOCAL_OUT_PATH}test11.out < ${LOCAL_IN_PATH}basic-parameter.in 2> ${LOG_PATH}test11.err
echo -n $? > test11.!!!

# test12: Basic regular expressions; Expected output: test12; Expected return code: 0
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}basic-re.in --output=${LOCAL_OUT_PATH}test12.out --format=${LOCAL_IN_PATH}basic-re.fmt 2> ${LOG_PATH}test12.err
echo -n $? > test12.!!!

# test13: Special regular expressions; Expected output: test13; Expected return code: 0
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}special-re.in --output=${LOCAL_OUT_PATH}test13.out --format=${LOCAL_IN_PATH}special-re.fmt 2> ${LOG_PATH}test13.err
echo -n $? > test13.!!!

# test14: Special RE - symbols; Expected output: test14; Expected return code: 0
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}special-symbols.in --output=${LOCAL_OUT_PATH}test14.out --format=${LOCAL_IN_PATH}special-symbols.fmt 2> ${LOG_PATH}test14.err
echo -n $? > test14.!!!

# test15: Negation; Expected output: test15; Expected return code: 0
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}negation.in --output=${LOCAL_OUT_PATH}test15.out --format=${LOCAL_IN_PATH}negation.fmt 2> ${LOG_PATH}test15.err
echo -n $? > test15.!!!

# test16: Multiple format parameters; Expected output: test16; Expected return code: 0
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}multiple.in --output=${LOCAL_OUT_PATH}test16.out --format=${LOCAL_IN_PATH}multiple.fmt 2> ${LOG_PATH}test16.err
echo -n $? > test16.!!!

# test17: Spaces/tabs in format parameters; Expected output: test17; Expected return code: 0
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}multiple.in --output=${LOCAL_OUT_PATH}test17.out --format=${LOCAL_IN_PATH}spaces.fmt 2> ${LOG_PATH}test17.err
echo -n $? > test17.!!!

# test18: Line break tag; Expected output: test18; Expected return code: 0
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}newlines.in --output=${LOCAL_OUT_PATH}test18.out --format=${LOCAL_IN_PATH}empty --br 2> ${LOG_PATH}test18.err
echo -n $? > test18.!!!

# test19: Overlap; Expected output: test19; Expected return code: 0
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}overlap.in --output=${LOCAL_OUT_PATH}test19.out --format=${LOCAL_IN_PATH}overlap.fmt 2> ${LOG_PATH}test19.err
echo -n $? > test19.!!!

# test20: Perl RE; Expected output: test20; Expected return code: 0
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}special-symbols.in --output=${LOCAL_OUT_PATH}test20.out --format=${LOCAL_IN_PATH}re.fmt 2> ${LOG_PATH}test20.err
echo -n $? > test20.!!!

# test21: Example; Expected output: test21; Expected return code: 0
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}example.in --br --format=${LOCAL_IN_PATH}example.fmt > ${LOCAL_OUT_PATH}test21.out 2> ${LOG_PATH}test21.err
echo -n $? > test21.!!!

# test22: Simple C program; Expected output: test22; Expected return code: 0
$INTERPRETER $TASK.$EXTENSION --input=${LOCAL_IN_PATH}cprog.c --br --format=${LOCAL_IN_PATH}c.fmt > ${LOCAL_OUT_PATH}test22.out 2> ${LOG_PATH}test22.err
echo -n $? > test22.!!!

