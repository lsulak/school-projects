/**********************************************************|
|**        Projekt do predmetov IFJ a IAL 2013/2014      **|
|**Subor : ilist.c                                       **|
|**Autori:                                               **|
|**       prevzate z ukazkoveho interpretu               **|
|**       upravili Matej Muzila  xmuzil00                **|
|**                Tomas Poremba xporem00                **|
|**********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "interpret.h"
#include "ilist.h"
#include "debug.h"

const char * iTypeChar[] = {"I_WRITE","I_READ","I_JUMP","I_JUMP_ELSE","I_RET","I_LAB","I_PLUS","I_MINUS","I_MULTIPLICATION","I_DIVISION","I_MORE","I_LESS","I_MORE_EQUAL","I_LESS_EQUAL","I_EQUAL","I_NOT_EQUAL","I_ASSIGN","I_CALL", "I_NOP", "I_PUSH", "I_SORT", "I_FIND", "I_COPY", "I_LENGTH"};

const string sNULL = {"#NULL", 6 };

int AddInstr(tListOfInstr *IList,instType_t instType, const string * addr1,
        const string *addr2, const string * addr3)
{
   DEB_PRINTF("INST: %s\n", iTypeChar[instType]);
   tInstr I;
    
   if(strInit(&(I.addr1)) != STR_SUCCESS)
       return ERR_INTERN;

   if( strCopyString(&(I.addr1), addr1) != STR_SUCCESS)
       goto err1;

   if(strInit(&(I.addr2)) != STR_SUCCESS)
       goto err1;

   if( strCopyString(&(I.addr2), addr2) != STR_SUCCESS)
       goto err2;

   if(strInit(&(I.addr3)) != STR_SUCCESS)
       goto err2;

   if( strCopyString(&(I.addr3), addr3) != STR_SUCCESS)
       goto err3;

   I.instType = instType;
   if(listInsertLast(IList, I) == SUCCESS)
       return SUCCESS;

    err3:
           strFree(&(I.addr3));
    err2:
           strFree(&(I.addr2));
    err1:
           strFree(&(I.addr1));
        return ERR_INTERN;
}



void listInit(tListOfInstr *L)
// funkce inicializuje seznam instrukci
{
  if(L == NULL)
      return;
  L->first  = NULL;
  L->last   = NULL;
  L->active = NULL;
  L->preactive = NULL;
}
  
void listFree(tListOfInstr *L)
// funkce dealokuje seznam instrukci
{
  
  while (L->first != NULL)
  {
    tListItem *tmp;
    tmp=L->first->nextItem;
    strFree(&(L->first->Instruction.addr1));
    strFree(&(L->first->Instruction.addr2));
    strFree(&(L->first->Instruction.addr3));
    free(L->first);
    L->first = tmp;
  }
}

int listInsertLast(tListOfInstr *L, const tInstr I)
// vlozi novou instruci na konec seznamu
{
  tListItem *newItem;
  if((newItem = malloc(sizeof (tListItem))) == NULL)
          return ERR_INTERN;
  newItem->Instruction = I;
  newItem->nextItem = NULL;
  if (L->first == NULL)
     L->first = newItem;
  else
     L->last->nextItem=newItem;
  L->last=newItem;
  return SUCCESS;
}

void listFirst(tListOfInstr *L)
// zaktivuje prvni instrukci
{
  L->preactive = NULL;
  L->active = L->first;
}

void listNext(tListOfInstr *L)
// aktivni instrukci se stane nasledujici instrukce
{
  if (L->active != NULL)
  {
    L->preactive = L->active;
    L->active = L->active->nextItem;
  }
}

void listGoto(tListOfInstr *L, void *gotoInstr)
// nastavime aktivni instrukci podle zadaneho ukazatele
// POZOR, z hlediska predmetu IAL tato funkce narusuje strukturu
// abstraktniho datoveho typu
{
  L->preactive = L->active;
  L->active = *(tListItem**) gotoInstr;
}

tListItem * listGetPointerLast(tListOfInstr *L)
// vrati ukazatel na posledni instrukci
// POZOR, z hlediska predmetu IAL tato funkce narusuje strukturu
// abstraktniho datoveho typu
{
  return L->last;
}

tInstr *listGetData(tListOfInstr *L)
// vrati aktivni instrukci
{
  if (L->active == NULL)
  {
    printf("Chyba, ziadna instrukcia nieje aktivna");
    return NULL;
  }
  else return &(L->active->Instruction);
}

