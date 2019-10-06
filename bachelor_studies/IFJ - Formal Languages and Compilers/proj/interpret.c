/**********************************************************|
|**        Projekt do predmetov IFJ a IAL 2014/2015      **|
|**Subor:    interpret.c                                 **|
|**Popis:    Obsahuje interpretacnu cast interpretu      **|
|**Autori:                                               **|
|**                                                      **|
|**       Ladislav Sulak  xsulak04                       **|
|**       Tomas Poremba   xporem00                       **|
|**********************************************************/ 
#include <stdio.h>
#include <stdlib.h>

#include "interpret.h"
#include "instructions.h"
#include "debug.h"

ret_t interpret(htab_t *globTable,tListOfInstr *IList)
{
    addStack_t addStack;
    tabList_t tabList;
    paramList_t paramList;
    string *par = NULL;

    addStackInit(&addStack);
    tabListInit(&tabList);
    paramListInit(&paramList);

//    listFirst(IList);
    tInstr *Instr;
    ret_t err = SUCCESS;
    if (IList->active == NULL)
    {
        printf("ziadne instrukcie na vykonanie\n");
        return 99;
    }
    while(IList->active != NULL)
    {
        Instr = listGetData(IList);
        switch (Instr->instType)
        {

          case I_WRITE:
            err = write(globTable,tabList.first,Instr);
              DEB_PRINTF("I_WRITE \tadr1: %s\tadr2: %s\tadr3: %s\n",Instr->addr1.str,Instr->addr2.str,Instr->addr3.str);
              break;

        case I_READ:
            err = readln(globTable,tabList.first,Instr);
            DEB_PRINTF("I_READ \tadr1: %s\tadr2: %s\tadr3: %s\n",Instr->addr1.str,Instr->addr2.str,Instr->addr3.str);
            break;

          case I_LAB:
              DEB_PRINTF("I_LAB \tadr1: %s\tadr2: %s\tadr3: %s\n",Instr->addr1.str,Instr->addr2.str,Instr->addr3.str);
              break;

          case I_RET:
            err = ret(    &tabList,
                        IList,
                        &addStack);
              DEB_PRINTF("I_RET \tadr1: %s\tadr2: %s\tadr3: %s\n",Instr->addr1.str,Instr->addr2.str,Instr->addr3.str);
            if(err != SUCCESS)
                goto Error;
              continue;

        case I_JUMP:
            err = jump(globTable,tabList.first,Instr,IList);
            if(err != SUCCESS)
                goto Error;
              DEB_PRINTF("I_JUMP \tadr1: %s\tadr2: %s\tadr3: %s\n",Instr->addr1.str,Instr->addr2.str,Instr->addr3.str);
              continue;

        case I_JUMP_ELSE:
            err = j_else(globTable,tabList.first,Instr,IList);
            if(err != SUCCESS)
                goto Error;
              DEB_PRINTF("I_JUMP_ELSE \tadr1: %s\tadr2: %s\tadr3: %s\n",Instr->addr1.str,Instr->addr2.str,Instr->addr3.str);
              continue;
       
        case I_PLUS: //OR CONCATENATION!
          err = plus(globTable,tabList.first,Instr);
          DEB_PRINTF("I_PLUS \tadr1: %s\tadr2: %s\tadr3: %s\n",Instr->addr1.str,Instr->addr2.str,Instr->addr3.str);
          break;

        case I_MINUS:
          err = minus(globTable,tabList.first,Instr);
          DEB_PRINTF("I_MINUS \tadr1: %s\tadr2: %s\tadr3: %s\n",Instr->addr1.str,Instr->addr2.str,Instr->addr3.str);
          break;
        
        case I_MULTIPLICATION:
          err = multiply(globTable,tabList.first,Instr);
          DEB_PRINTF("I_MULTIPLICATION \tadr1: %s\tadr2: %s\tadr3: %s\n",Instr->addr1.str,Instr->addr2.str,Instr->addr3.str);
          break;
        
        case I_DIVISION:
          err = divide(globTable,tabList.first,Instr);
          DEB_PRINTF("I_DIVISION \tadr1: %s\tadr2: %s\tadr3: %s\n",Instr->addr1.str,Instr->addr2.str,Instr->addr3.str);
          break;
        
        case I_MORE:
          err = more(globTable,tabList.first,Instr);
          DEB_PRINTF("I_MORE \tadr1: %s\tadr2: %s\tadr3: %s\n",Instr->addr1.str,Instr->addr2.str,Instr->addr3.str);
          break;
        
        case I_LESS:
          err = less(globTable,tabList.first,Instr);
          DEB_PRINTF("I_LESS \tadr1: %s\tadr2: %s\tadr3: %s\n",Instr->addr1.str,Instr->addr2.str,Instr->addr3.str);
          break;
          
        case I_MORE_EQUAL:
          err = more_equal(globTable,tabList.first,Instr);
          DEB_PRINTF("I_MORE_EQUAL \tadr1: %s\tadr2: %s\tadr3: %s\n",Instr->addr1.str,Instr->addr2.str,Instr->addr3.str);
          break;
        
        case I_LESS_EQUAL:
          err = less_equal(globTable,tabList.first,Instr);
          DEB_PRINTF("I_LESS_EQUAL \tadr1: %s\tadr2: %s\tadr3: %s\n",Instr->addr1.str,Instr->addr2.str,Instr->addr3.str);
          break;
        
        case I_EQUAL:
          err = equal(globTable,tabList.first,Instr);
          DEB_PRINTF("I_EQUAL \tadr1: %s\tadr2: %s\tadr3: %s\n",Instr->addr1.str,Instr->addr2.str,Instr->addr3.str);
          break;
        
        case I_NOT_EQUAL:
          err = not_equal(globTable,tabList.first,Instr);
          DEB_PRINTF("I_NOT_EQUAL \tadr1: %s\tadr2: %s\tadr3: %s\n",Instr->addr1.str,Instr->addr2.str,Instr->addr3.str);
          break;

        case I_ASSIGN:
          err = assign(globTable,tabList.first,Instr);
          DEB_PRINTF("I_ASSIGN \tadr1: %s\tadr2: %s\tadr3: %s\n",Instr->addr1.str,Instr->addr2.str,Instr->addr3.str);
          break;

        case I_CALL:
          err = call(globTable,
                      tabList.first,
                      Instr,
                    &tabList,
                      &paramList,
                    IList,
                    &addStack);
          DEB_PRINTF("I_CALL \tadr1: %s\tadr2: %s\tadr3: %s\n",Instr->addr1.str,Instr->addr2.str,Instr->addr3.str);

          if(err != SUCCESS)
              goto Error;
          continue;

        case I_NOP:
          DEB_PRINTF("I_NOP \tadr1: %s\tadr2: %s\tadr3: %s\n",Instr->addr1.str,Instr->addr2.str,Instr->addr3.str);
          break;

        case I_PUSH:
          err = addParam(&paramList,Instr->addr1);
          DEB_PRINTF("I_PUSH \tadr1: %s\tadr2: %s\tadr3: %s\n",Instr->addr1.str,Instr->addr2.str,Instr->addr3.str);
          break;

        case I_SORT:
          err = sort(globTable,tabList.first,Instr);
          DEB_PRINTF("I_SORT \tadr1: %s\tadr2: %s\tadr3: %s\n",Instr->addr1.str,Instr->addr2.str,Instr->addr3.str);
          break;

        case I_LENGTH:
          err = len(globTable,tabList.first,Instr);
          DEB_PRINTF("I_LENGTH \tadr1: %s\tadr2: %s\tadr3: %s\n",Instr->addr1.str,Instr->addr2.str,Instr->addr3.str);
          break;

        case I_COPY:
          err = copy(globTable,tabList.first,Instr);
          DEB_PRINTF("I_COPY \tadr1: %s\tadr2: %s\tadr3: %s\n",Instr->addr1.str,Instr->addr2.str,Instr->addr3.str);
          break;

        case I_FIND:
          err = find(globTable, tabList.first, Instr);
          DEB_PRINTF("I_FIND \tadr1: %s\tadr2: %s\tadr3: %s\n",Instr->addr1.str,Instr->addr2.str,Instr->addr3.str);
          break;

          }

        if(err != SUCCESS)
            goto Error;

          listNext(IList);
    }
    return SUCCESS;

    Error:
    while(tabList.first != NULL)
    {
        removeTab(&tabList);
    }
    while(paramList.first != NULL)
    {
        par = getParam(&paramList);
        strFree(par);
        free(par);
    }
    while(addStack.first != NULL)
    {
        removeAddress(&addStack);
    }
    return err;
}

void paramListInit(paramList_t *L)
{
    L->first = NULL;
}

ret_t addParam(paramList_t *L, const string key)
{
    parListItem_t * new = NULL;

    if((new = malloc(sizeof(parListItem_t))) == NULL)
        return ERR_INTERN;
    
    if(strInit(&(new->key)) != STR_SUCCESS)
    {
        free(new);
        return ERR_INTERN;
    }
    
    if(strCopyString(&(new->key),&key) != STR_SUCCESS)
    {
        strFree(&(new->key));
        free(new);
        return ERR_INTERN;
    }


    new->nextPar = L->first;
    L->first = new;

    return SUCCESS;
}

string * getParam(paramList_t *L)
{
    string * ret_val;
    parListItem_t * tmp = L->first;

    if(tmp == NULL)
        return NULL;

    if((ret_val = malloc(sizeof(string))) == NULL)
        return NULL;

    if(strInit(ret_val) != STR_SUCCESS)
    {
        strFree(ret_val);
        return NULL;
    }
    if(strCopyString(ret_val,&(tmp->key)) != STR_SUCCESS)
    {
        strFree(ret_val);
        return NULL;
    }

    L->first = tmp->nextPar;
    strFree(&(tmp->key));
    free(tmp);

    return ret_val;
}

void tabListInit(tabList_t *L)
{
    L->first = NULL;
}

ret_t addTabToList(tabList_t *L, htab_t *src)
{
    tabListItem_t *new;

    if((new = malloc(sizeof(tabListItem_t))) == NULL)
        return ERR_INTERN;

    if((new->table = htab_copy(src)) == NULL)
    {
        free(new);
        return ERR_INTERN;
    }
    
    new->nextTab = L->first;
    L->first = new;

    return SUCCESS;
}

void removeTab(tabList_t *L)
{
    tabListItem_t *tmp = L->first;
    if(tmp == NULL)
        return;

    L->first = tmp->nextTab;
    htab_free(tmp->table);
    free(tmp);
}
void addStackInit(addStack_t *Stack)
{
    Stack->first = NULL;
}

ret_t addAddress(addStack_t *Stack, tListItem* inst)
{
    retAddress_t *new;
    if((new = malloc(sizeof(retAddress_t))) == NULL)
        return ERR_INTERN;

    new->inst = inst;

    new->nextAdd = Stack->first;
    Stack->first = new;

    return SUCCESS;
}

tListItem* removeAddress(addStack_t *Stack)
{
    retAddress_t *tmp = Stack->first;
    tListItem *ret;
    
    if(tmp == NULL)
        return NULL;

    Stack->first = tmp->nextAdd;
    ret = tmp->inst;
    free(tmp);
    return ret;
}
