/**********************************************************|
|**        Projekt do predmetov IFJ a IAL 2014/2015      **|
|**Subor:    ial.c                                       **|
|**Popis:    Hlavickovy subor pre modul interpret.c      **|
|**Autori:                                               **|
|**                                                      **|
|**       Matej Muzila    xmuzil00                       **|
|**       Tomas Poremba   xporem00                       **|
|**********************************************************/ 

#ifndef INTERPRET_H
#define INTERPRET_H

#include "ial.h"
#include "ilist.h"



typedef enum enum_t
{
    SUCCESS = 0,
    ERR_LEX,
    ERR_SYNT,
    ERR_SEM_UNDEF,
    ERR_SEM_TYPE,
    ERR_SEM_OTHER,
    ERR_RT_READ,
    ERR_RT_UNINIT,
    ERR_RT_ZEROD,
    ERR_RT_OTHER,
    ERR_INTERN = 99,
}ret_t;

typedef struct tabListItem {
    htab_t *table;
    struct tabListItem *nextTab;
} tabListItem_t;

typedef struct {
    tabListItem_t *first;
} tabList_t;

typedef struct parListItem {
    string key;
    struct parListItem *nextPar;
} parListItem_t;

typedef struct {
    parListItem_t *first;
} paramList_t;

typedef struct retAddress {
    tListItem *inst;
    struct retAddress *nextAdd;
} retAddress_t;

typedef struct {
    retAddress_t *first;
} addStack_t;

void addStackInit(addStack_t *Stack);
ret_t addAddress(addStack_t *Stack, tListItem* inst);
tListItem* removeAddress(addStack_t *Stack);


void tabListInit(tabList_t *L);
ret_t addTabToList(tabList_t *L, htab_t *table);
void removeTab(tabList_t *L);

void paramListInit(paramList_t *L);
ret_t addParam(paramList_t *L, const string key);
string * getParam(paramList_t *L);

ret_t interpret(htab_t *symbolTable,tListOfInstr *IList);

#endif
