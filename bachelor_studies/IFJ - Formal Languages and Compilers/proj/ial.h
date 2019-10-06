/**********************************************************|
|**        Projekt do predmetov IFJ a IAL 2014/2015      **|
|**Subor:    ial.h                                       **|
|**Popis:    Hlavickovy subor k modulu ial.c             **|
|**Autori:                                               **|
|**                                                      **|
|**       Matej Muzila    xmuzil00                       **|
|**       Tomas Poremba   xporem00                       **|
|**********************************************************/ 

#ifndef IAL_H
#define IAL_H

#include <limits.h>
#include <stdbool.h>
#include "str.h"
#include "ilist.h"

#define STRINGIFY(x)    #x                                                      
#define TOSTRING(x) STRINGIFY(x)
#define INT_MAX_C    sizeof(TOSTRING(INT_MAX))
#define IS_LESS(a, b) (a < b)

#define UNUSED(x) (void)(x)

#define HTAB_SIZE 193

typedef enum EXIT_TYPES
{
    HTAB_SUCCESS,
    HTAB_SYM_EXISTS,
    HTAB_SYM_NFOUND,
    HTAB_ERR_MALLOC,
    HTAB_LITERAL_UNDEF,
    HTAB_ERR_FUNC,
    HTAB_ERR_FUNC_ARRAY,
    HTAB_TAB_NOT_EXIST,
}hExit_t;


#define ARRAY_BEG 0

typedef enum
{
    INT, BOOL, STRING, REAL, FUNC, LABEL, NOTYPE
}type_t;


typedef struct paramEnt
{
    string paramKey;
    type_t paramType;
    struct paramEnt * prevParam;
    struct paramEnt * nextParam;
}paramEnt;

typedef struct entry_t
{
    bool inited;
    bool literal;
    long begin;
    long length;
    type_t type;
    void * data;
}entry_t;

typedef struct htab_listitem
{
    string key;
    entry_t entry;
    struct htab_listitem * next;
}htab_listitem;


typedef struct htab_t
{
    unsigned int size;
    int litIdCnt;
    htab_listitem *htab_ind[];
    
}htab_t;

typedef struct
{
    type_t rettype;
    htab_t * defHtab;
    tListItem * pStartInst;    

    paramEnt * nextParam;
    paramEnt * lastParam;
}func_t;

//htab_listitem * htab_lookup(htab_t *t, const string *key);
//
unsigned int hash_function(const char *str, unsigned htab_size);
void htab_clear(htab_t *t);
int htab_foreach(const htab_t *t, int (*fc)(const string, entry_t *));
void htab_free(htab_t *t);
htab_t * htab_init(const unsigned int size);
int htab_newLabel(htab_t *t, tListItem **data, const string *key, htab_listitem **ret);
int htab_newLiteral(
        htab_t *t,
        const type_t type,
        const void *data,
        htab_listitem **ret);
int htab_newTmp(
        htab_t *t,
        const type_t type,
        htab_listitem **ret);
int htab_new(
        htab_t *t,
        const string *key,
        const type_t type,
        const long begin,
        const long length,
        const bool literal,
        const void *data,
        htab_listitem **ret);
entry_t * htab_getentry(const htab_t *t, const string *key);
htab_listitem * htab_get(const htab_t *t, const string *key);
int htab_remove(htab_t *t, const string *key);
void htab_statistics(const htab_t *t);
int htab_print(const string s, entry_t * entry);
htab_t * htab_copy(const htab_t * src);
func_t * funcInit();
void funcAddType(func_t * f, type_t ret);
hExit_t funcCompare(func_t * f1, func_t * f2);
void funcAddAddress(htab_listitem * pTab, tListItem * iAddress);
int funcAddParam(func_t *f, type_t paramType, string * paramKey);
void funcPrint(func_t *f);
void funcFree(func_t * f);
//hExit_t htab_copyFuncParam(func_t *f, type_t paramType, string * paramKey);
int funcIsInited(string str, entry_t * entry);


void exch(char * a, char * b);
void fixDown(char * array, int start, int end);
void hsort(char * array, int number);
int kmp_find(string * text,string * pattern);
#endif
