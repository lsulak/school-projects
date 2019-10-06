/**********************************************************|
|**        Projekt do predmetov IFJ a IAL 2014/2015      **|
|**Subor : parser.h                                      **|
|**Popis : Hlavickovy subor k modulu parser.c            **|
|**Autori:                                               **|
|**       Matej Muzila      xmuzil00                     **|
|**       Milos Volny       xvolny02                     **|
|**********************************************************/

#ifndef PARSER_H
#define PARSER_H

#include "str.h"
#include "ial.h"
#include "ilist.h"
#include "interpret.h"
#include "scanner.h"


extern string attr;
extern token_t token;


typedef enum
{
    S_CALL_FUNC_ID,
    S_CALL_LB,
    S_CALL_PARAM,
    S_CALL_RB,
}stateFncCall_t;

typedef enum
{
    S_FNC_ID,
    S_FNC_START,
    S_FNC_LB,
    S_FNC_RB,
    S_FNC_PARAM_ID,
    S_FNC_PARAM_COL,
    S_FNC_PARAM_TYPE,
    S_FNC_COL,
    S_FNC_TYPE,
    S_FNC_SCOL,
    S_FNC_VAR,
    S_FNC_FWD,
    S_FNC_VAR_ID,
    S_FNC_VAR_COL,
    S_FNC_VAR_TYPE,
    S_FNC_VAR_SCOL,
    S_FNC_END,

}stateFnc_t;

typedef enum
{
    S_BL_BEGIN,
    S_BL_CLEAR_S,
    S_BL_CLEAR_E,
    S_BL_IF,
    S_BL_WHILE,
    S_BL_THEN,
}stateBl_t;

typedef enum
{
    S_WR_START,
    S_WR_LB,
    S_WR_AFTWR,
    S_WR_RB,
}stateWr_t;

ret_t parse(htab_t *symbolTable,tListOfInstr *IList);
ret_t program(htab_t *symbolTable,tListOfInstr *IList);
type_t typeDecode(token_t token);

#endif 
