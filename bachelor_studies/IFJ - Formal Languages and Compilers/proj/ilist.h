/**********************************************************|
|**        Projekt do predmetov IFJ a IAL 2013/2014      **|
|**Subor : ilist.h                                       **|
|**Autori:                                               **|
|**       prevzate z ukazkoveho interpretu               **|
|**       upravili Matej Muzila  xmuzil00                **|
|**                Tomas Poremba xporem00                **|
|**********************************************************/


#ifndef ILIST_H
#define ILIST_H

extern const char * iTypeChar[];
#include "str.h"

extern const string sNULL;
#define    RET_BOOL    "#RET_BOOL"
#define RET_INT        "#RET_INT"
#define RET_REAL    "#RET_REAL"
#define RET_STR        "#RET_STR"

typedef enum INST_TYPES
{
    I_WRITE,    //vypis hodnoty (entry_t *) odkazovanej prvym operandom
    I_READ,        // nacita riadok vstupu do (entry_t *) v prvom operande
    I_JUMP,     // dumb nepodmieneny skok na instrukciu odkazovanu prvym parametrom
    I_JUMP_ELSE, // skoci na instrukciu odkazovanu v addr1 ak addr3 predchadzajucej instrukcie je false
      I_RET,        /*navratova hodnota odkazovana 3.parametrom*/
      I_LAB,        //navestie - ciel skoku

    I_PLUS,              //scitanie        
    I_MINUS,           //odcitanie
    I_MULTIPLICATION,        //nasobenie
    I_DIVISION,          //delenie

    I_MORE,         // '>'
    I_LESS,         // '<'
    I_MORE_EQUAL,    // '>='
    I_LESS_EQUAL,    // '<='
    I_EQUAL,        // '='  NOT ASSIGN! Use with comparison
    I_NOT_EQUAL,    // '<>'

    I_ASSIGN,          //priradenie hodnoty z predchadzajucej I_RET do (entry_t *) v addr1
    I_CALL,            //volanie funkcie
    I_NOP,
    I_PUSH,
    I_SORT,
    I_FIND,
    I_COPY,
    I_LENGTH
}instType_t;

typedef struct
{
  instType_t instType;  // typ instrukcie
  string addr1; // adresa 1
  string addr2; // adresa 2
  string addr3; // adresa 3
} tInstr;

typedef struct listItem
{
  tInstr Instruction;
  struct listItem *nextItem;
} tListItem;
    
typedef struct
{
  struct listItem *first;  // ukazatel na prvni prvek
  struct listItem *last;   // ukazatel na posledni prvek
  struct listItem *active; // ukazatel na aktivni prvek
  struct listItem *preactive; //ukazatel na prvok pred aktivnym prvkom
} tListOfInstr;

void listInit(tListOfInstr *L);
void listFree(tListOfInstr *L);
int listInsertLast(tListOfInstr *L, const tInstr I);
void listFirst(tListOfInstr *L);
void listNext(tListOfInstr *L);
void listGoto(tListOfInstr *L, void *gotoInstr);
tListItem * listGetPointerLast(tListOfInstr *L);
tInstr *listGetData(tListOfInstr *L);

int AddInstr(tListOfInstr *IList, instType_t instType, const string * addr1, 
        const string *addr2, const string *addr3);

#endif
