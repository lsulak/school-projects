/**********************************************************|
|**        Projekt do predmetov IFJ a IAL 2014/2015      **|
|**Subor:    instructions.c                              **|
|**Popis:    Obsahuje funkcie jednotlivych instrukcii    **|
|**Autori:                                               **|
|**                                                      **|
|**       Tomas Poremba   xporem00                       **|
|**********************************************************/ 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "ilist.h"
#include "str.h"
#include "ial.h"
#include "interpret.h"
#include "instructions.h"

ret_t assign(htab_t *globTable, tabListItem_t *tab, tInstr *instr)
{
    entry_t *dest;
    entry_t *src;

    htab_t *locTable = NULL;

    if(tab != NULL)
        locTable = tab->table;
    
    if(locTable != NULL)
    {
        //hladam symbol v lokalnej tabulke
        if( (dest = htab_getentry(locTable,&(instr->addr1))) == NULL)
        //hladam symbol v globalnej tabulke
            if( (dest = htab_getentry(globTable,&(instr->addr1))) == NULL)
                //nenasiel som symbol ani v jednej
                return ERR_RT_OTHER;
        
        if( (src = htab_getentry(locTable,&(instr->addr3))) == NULL)
            if( (src = htab_getentry(globTable,&(instr->addr3))) == NULL)
                return ERR_RT_OTHER;
    }
    else
    {
        if( (dest = htab_getentry(globTable,&(instr->addr1))) == NULL)
            //nenasiel som symbol ani v jednej
            return ERR_RT_OTHER;

        if( (src = htab_getentry(globTable,&(instr->addr3))) == NULL)
            return ERR_RT_OTHER;
    }

    if(src->inited == false)
        return ERR_RT_UNINIT;

    if(src->type != dest->type)
        return ERR_RT_OTHER;

    if(src->type == INT)
    {
        if((dest->data = realloc(dest->data,sizeof(int)*(src->length))) == NULL)
            return ERR_INTERN;
    
        memcpy(dest->data,src->data,sizeof(int)*(src->length));
        dest->type = INT;
    }
    else if(src->type == REAL)
    {
        if((dest->data = realloc(dest->data,sizeof(double)*(src->length))) == NULL)
            return ERR_INTERN;
        
        memcpy(dest->data,src->data,sizeof(double)*(src->length));
        dest->type = REAL;
    }
    else if(src->type == BOOL)
    {
        if((dest->data = realloc(dest->data,sizeof(bool)*(src->length))) == NULL)
            return ERR_INTERN;
        
        memcpy(dest->data,src->data,sizeof(bool)*(src->length));
        dest->type = BOOL;
    }
    else if(src->type == STRING)
    {
        /*if(dest->inited == false)
        {
            strFree(dest->data);
            if((dest->data = realloc(dest->data,sizeof(string)*(src->length))) == NULL)
                return ERR_INTERN;
            
            if(strInit(dest->data) != STR_SUCCESS)
                return ERR_INTERN;
        }*/
        if(strCopyString(dest->data,src->data) != STR_SUCCESS)
            return ERR_INTERN;

        dest->type = STRING;
    }

    dest->inited = true;

    return SUCCESS;
}



ret_t plus(htab_t *globTable, tabListItem_t *tab, tInstr *instr)
{
    entry_t *opL;
    entry_t *opR;
    entry_t *vysl;
        
    htab_t *locTable = NULL;

    if(tab != NULL)
        locTable = tab->table;

    if(locTable != NULL)
    {
        if( (opL = htab_getentry(locTable,&(instr->addr1))) == NULL)
            if( (opL = htab_getentry(globTable,&(instr->addr1))) == NULL)
                return ERR_RT_OTHER;

        if( (opR = htab_getentry(locTable,&(instr->addr2))) == NULL)
            if( (opR = htab_getentry(globTable,&(instr->addr2))) == NULL)
                return ERR_RT_OTHER;

        if( (vysl = htab_getentry(locTable,&(instr->addr3))) == NULL)
            if( (vysl = htab_getentry(globTable,&(instr->addr3))) == NULL)
                return ERR_RT_OTHER;
    }
    else
    {
            if( (opL = htab_getentry(globTable,&(instr->addr1))) == NULL)
                return ERR_RT_OTHER;

            if( (opR = htab_getentry(globTable,&(instr->addr2))) == NULL)
                return ERR_RT_OTHER;

            if( (vysl = htab_getentry(globTable,&(instr->addr3))) == NULL)
                return ERR_RT_OTHER;
    }

    if(opL->inited == false || opR->inited == false)
        return ERR_RT_UNINIT;

    if(opL->type == opR->type)
    {
        if(opL->type == INT)
        {
            vysl->type = INT;
            if((vysl->data = realloc(vysl->data,sizeof(int)*(opL->length))) == NULL)
                    return ERR_INTERN;
            
            *((int *)vysl->data) = *((int *)opL->data) + *((int *)opR->data);
        }
        else if(opL->type == REAL)
        {
            vysl->type = REAL;
            if((vysl->data = realloc(vysl->data,sizeof(double)*(opL->length))) == NULL)
                return ERR_INTERN;
            
            *((double *)vysl->data) = *((double *)opL->data) + *((double *)opR->data);
        }
        else//if(opL->type == STRING)
        {
            vysl->type = STRING;
            /*strFree(vysl->data);
            if((vysl->data = realloc(vysl->data,sizeof(string)*(opL->length))) == NULL)
                return ERR_INTERN;

            if(strInit(vysl->data) != STR_SUCCESS)
                return ERR_INTERN;*/

            if(strCopyString(vysl->data,opL->data) != STR_SUCCESS)
                return ERR_INTERN;

            if(strAddConstString(vysl->data,((string *)opR->data)->str) != STR_SUCCESS)
                return ERR_INTERN;
        }
        
    }
    else//if(opL->type != opR->type)
    {
        if(opL->type == INT && opR->type == REAL)
        {
            vysl->type = REAL;
            if((vysl->data = realloc(vysl->data,sizeof(double)*(opR->length))) == NULL)
                return ERR_INTERN;
            
            *((double *)vysl->data) = *((int *)opL->data) + *((double *)opR->data);
        }
        else//if(opL->type == REAL && opR->type == INT)
        {
            vysl->type = REAL;
            if((vysl->data = realloc(vysl->data,sizeof(double)*(opR->length))) == NULL)
                return ERR_INTERN;
            
            *((double *)vysl->data) = *((double *)opL->data) + *((int *)opR->data);

        }

    }
    vysl->inited = true;
    return SUCCESS;
}


ret_t minus(htab_t *globTable, tabListItem_t *tab, tInstr *instr)
{
    entry_t *opL;
    entry_t *opR;
    entry_t *vysl;
        
    htab_t *locTable = NULL;

    if(tab != NULL)
        locTable = tab->table;

    
    if(locTable != NULL)
    {
        if( (opL = htab_getentry(locTable,&(instr->addr1))) == NULL)
            if( (opL = htab_getentry(globTable,&(instr->addr1))) == NULL)
                return ERR_RT_OTHER;

        if( (opR = htab_getentry(locTable,&(instr->addr2))) == NULL)
            if( (opR = htab_getentry(globTable,&(instr->addr2))) == NULL)
                return ERR_RT_OTHER;

        if( (vysl = htab_getentry(locTable,&(instr->addr3))) == NULL)
            if( (vysl = htab_getentry(globTable,&(instr->addr3))) == NULL)
                return ERR_RT_OTHER;
    }
    else
    {
            if( (opL = htab_getentry(globTable,&(instr->addr1))) == NULL)
                return ERR_RT_OTHER;

            if( (opR = htab_getentry(globTable,&(instr->addr2))) == NULL)
                return ERR_RT_OTHER;

            if( (vysl = htab_getentry(globTable,&(instr->addr3))) == NULL)
                return ERR_RT_OTHER;
    }

    if(opL->inited == false || opR->inited == false)
        return ERR_RT_UNINIT;

    if(opL->type == opR->type)
    {
        if(opL->type == INT)
        {
            vysl->type = INT;
            if((vysl->data = realloc(vysl->data,sizeof(int)*(opL->length))) == NULL)
                    return ERR_INTERN;
    
            *((int *)vysl->data) = *((int *)opL->data) - *((int *)opR->data);
        }
        else//if(opL->type == REAL)
        {
            vysl->type = REAL;
            if((vysl->data = realloc(vysl->data,sizeof(double)*(opL->length))) == NULL)
                return ERR_INTERN;
            
            *((double *)vysl->data) = *((double *)opL->data) - *((double *)opR->data);
        }
        
    }
    else//if(opL->type != opR->type)
    {
        if(opL->type == INT && opR->type == REAL)
        {
            vysl->type = REAL;
            if((vysl->data = realloc(vysl->data,sizeof(double)*(opR->length))) == NULL)
                return ERR_INTERN;
            
            *((double *)vysl->data) = *((int *)opL->data) - *((double *)opR->data);
        }
        else//if(opL->type == REAL && opR->type == INT)
        {
            vysl->type = REAL;
            if((vysl->data = realloc(vysl->data,sizeof(double)*(opR->length))) == NULL)
                return ERR_INTERN;
            
            *((double *)vysl->data) = *((double *)opL->data) - *((int *)opR->data);

        }

    }
    vysl->inited = true;
    return SUCCESS;
}

ret_t multiply(htab_t *globTable, tabListItem_t *tab, tInstr *instr)
{
    entry_t *opL;
    entry_t *opR;
    entry_t *vysl;
        
    htab_t *locTable = NULL;

    if(tab != NULL)
        locTable = tab->table;

    
    if(locTable != NULL)
    {
        if( (opL = htab_getentry(locTable,&(instr->addr1))) == NULL)
            if( (opL = htab_getentry(globTable,&(instr->addr1))) == NULL)
                return ERR_RT_OTHER;

        if( (opR = htab_getentry(locTable,&(instr->addr2))) == NULL)
            if( (opR = htab_getentry(globTable,&(instr->addr2))) == NULL)
                return ERR_RT_OTHER;

        if( (vysl = htab_getentry(locTable,&(instr->addr3))) == NULL)
            if( (vysl = htab_getentry(globTable,&(instr->addr3))) == NULL)
                return ERR_RT_OTHER;
    }
    else
    {
            if( (opL = htab_getentry(globTable,&(instr->addr1))) == NULL)
                return ERR_RT_OTHER;

            if( (opR = htab_getentry(globTable,&(instr->addr2))) == NULL)
                return ERR_RT_OTHER;

            if( (vysl = htab_getentry(globTable,&(instr->addr3))) == NULL)
                return ERR_RT_OTHER;
    }

    if(opL->inited == false || opR->inited == false)
        return ERR_RT_UNINIT;

    if(opL->type == opR->type)
    {
        if(opL->type == INT)
        {
            vysl->type = INT;
            if((vysl->data = realloc(vysl->data,sizeof(int)*(opL->length))) == NULL)
                    return ERR_INTERN;
    
            *((int *)vysl->data) = *((int *)opL->data) * *((int *)opR->data);
        }
        else//if(opL->type == REAL)
        {
            vysl->type = REAL;
            if((vysl->data = realloc(vysl->data,sizeof(double)*(opL->length))) == NULL)
                return ERR_INTERN;
            
            *((double *)vysl->data) = *((double *)opL->data) * *((double *)opR->data);
        }
        
    }
    else//if(opL->type != opR->type)
    {
        if(opL->type == INT && opR->type == REAL)
        {
            vysl->type = REAL;
            if((vysl->data = realloc(vysl->data,sizeof(double)*(opR->length))) == NULL)
                return ERR_INTERN;
            
            *((double *)vysl->data) = *((int *)opL->data) * *((double *)opR->data);
        }
        else//if(opL->type == REAL && opR->type == INT)
        {
            vysl->type = REAL;
            if((vysl->data = realloc(vysl->data,sizeof(double)*(opR->length))) == NULL)
                return ERR_INTERN;
            
            *((double *)vysl->data) = *((double *)opL->data) * *((int *)opR->data);

        }

    }
    vysl->inited = true;
    return SUCCESS;


}

ret_t divide(htab_t *globTable, tabListItem_t *tab, tInstr *instr)
{
    entry_t *opL;
    entry_t *opR;
    entry_t *vysl;
        
    htab_t *locTable = NULL;

    if(tab != NULL)
        locTable = tab->table;

    
    if(locTable != NULL)
    {
        if( (opL = htab_getentry(locTable,&(instr->addr1))) == NULL)
            if( (opL = htab_getentry(globTable,&(instr->addr1))) == NULL)
                return ERR_RT_OTHER;

        if( (opR = htab_getentry(locTable,&(instr->addr2))) == NULL)
            if( (opR = htab_getentry(globTable,&(instr->addr2))) == NULL)
                return ERR_RT_OTHER;

        if( (vysl = htab_getentry(locTable,&(instr->addr3))) == NULL)
            if( (vysl = htab_getentry(globTable,&(instr->addr3))) == NULL)
                return ERR_RT_OTHER;
    }
    else
    {
            if( (opL = htab_getentry(globTable,&(instr->addr1))) == NULL)
                return ERR_RT_OTHER;

            if( (opR = htab_getentry(globTable,&(instr->addr2))) == NULL)
                return ERR_RT_OTHER;

            if( (vysl = htab_getentry(globTable,&(instr->addr3))) == NULL)
                return ERR_RT_OTHER;
    }

    if(opL->inited == false || opR->inited == false)
        return ERR_RT_UNINIT;

    if(opL->type == opR->type)
    {
        if(opL->type == INT)
        {
            vysl->type = REAL;
            if((vysl->data = realloc(vysl->data,sizeof(double)*(opL->length))) == NULL)
                    return ERR_INTERN;

            if(*((int *)opR->data) == 0)
                return ERR_RT_ZEROD;
            
            *((double *)vysl->data) = (double) *((int *)opL->data) / (double)*((int *)opR->data);
        }
        else//if(opL->type == REAL)
        {
            vysl->type = REAL;
            if((vysl->data = realloc(vysl->data,sizeof(double)*(opL->length))) == NULL)
                return ERR_INTERN;
            
            if(*((double *)opR->data) == 0)
                return ERR_RT_ZEROD;

            *((double *)vysl->data) = *((double *)opL->data) / *((double *)opR->data);
        }
        
    }
    else//if(opL->type != opR->type)
    {
        if(opL->type == INT && opR->type == REAL)
        {
            vysl->type = REAL;
            if((vysl->data = realloc(vysl->data,sizeof(double)*(opR->length))) == NULL)
                return ERR_INTERN;
            
            if(*((double *)opR->data) == 0)
                return ERR_RT_ZEROD;

            *((double *)vysl->data) = (double) *((int *)opL->data) / *((double *)opR->data);
        }
        else//if(opL->type == REAL && opR->type == INT)
        {
            vysl->type = REAL;
            if((vysl->data = realloc(vysl->data,sizeof(double)*(opR->length))) == NULL)
                return ERR_INTERN;
            
            if(*((int *)opR->data) == 0)
                return ERR_RT_ZEROD;

            *((double *)vysl->data) = *((double *)opL->data) / (double) *((int *)opR->data);
        }

    }
    vysl->inited = true;
    return SUCCESS;
}

ret_t write(htab_t *globTable, tabListItem_t *tab, tInstr *instr)
{
    entry_t *entry;

    htab_t *locTable = NULL;

    if(tab != NULL)
        locTable = tab->table;
    
    if(locTable != NULL)
    {
        if( (entry = htab_getentry(locTable,&(instr->addr1))) == NULL)
            if( (entry = htab_getentry(globTable,&(instr->addr1))) == NULL)
                return ERR_RT_OTHER;
    }
    else
    {
        if( (entry = htab_getentry(globTable,&(instr->addr1))) == NULL)
            return ERR_RT_OTHER;
    }
    
    if(entry->inited == false)
        return ERR_RT_UNINIT;

    switch(entry->type)
    {
        case(INT):
            {
                printf("%d",*((int *)entry->data));
            }
            break;

        case(REAL):
            {
                printf("%g",*((double *)entry->data));
            }
            break;

        case(BOOL):
            {
                *((bool *)entry->data) ? printf("TRUE") : printf("FALSE");
            }
            break;
            
        case(STRING):
            {
                printf("%s",((string *)entry->data)->str);
            }
            break;
        default:
            break;
    }
    return SUCCESS;
}

//PO TEJTO INSTRUKCII MUSI V SLUCKE WHILE NASLEDOVAT CONTINUE!!!
ret_t j_else(htab_t *globTable, tabListItem_t *tab, tInstr *instr, tListOfInstr *L)
{
    entry_t *entry;
    entry_t *cond;

    htab_t *locTable = NULL;

    if(tab != NULL)
        locTable = tab->table;
    
    if(locTable != NULL)
    {
        if( (entry = htab_getentry(locTable,&(instr->addr1))) == NULL)
            if( (entry = htab_getentry(globTable,&(instr->addr1))) == NULL)
                return ERR_RT_OTHER;
    }
    else
    {
        if( (entry = htab_getentry(globTable,&(instr->addr1))) == NULL)
            return ERR_RT_OTHER;
    }


    tListItem *tmp = L->preactive;    //predchadzajuca instrukcia
    if(tmp == NULL)
        return ERR_INTERN;

    if(locTable != NULL)
    {
        if( (cond = htab_getentry(locTable,&(tmp->Instruction.addr3))) == NULL)
            if( (cond = htab_getentry(globTable,&(tmp->Instruction.addr3))) == NULL)
                return ERR_RT_OTHER;
    }
    else
    {
        if( (cond = htab_getentry(globTable,&(tmp->Instruction.addr3))) == NULL)
            return ERR_RT_OTHER;
    }
    if(cond->inited == false)
        return ERR_RT_UNINIT;
    
    if(entry->type == LABEL)
    {
        if(*((bool *)cond->data) == false)
        {
            //jumpni
            listGoto(L,entry->data);
        }    
        else
        {
            listNext(L);
        }
    }
    else
        return ERR_RT_OTHER;
    
    return SUCCESS;
}

//PO TEJTO INSTRUKCII MUSI NASLEDOVAT V SLUCKE INTERPRETU WHILE NASLEDOVAT CONTINUE!!!
ret_t jump(htab_t *globTable, tabListItem_t *tab, tInstr *instr, tListOfInstr *L)
{
    entry_t *entry;

    htab_t *locTable = NULL;

    if(tab != NULL)
        locTable = tab->table;
    
    if(locTable != NULL)
    {
        if( (entry = htab_getentry(locTable,&(instr->addr1))) == NULL)
            if( (entry = htab_getentry(globTable,&(instr->addr1))) == NULL)
                return ERR_RT_OTHER;
    }
    else
    {
        if( (entry = htab_getentry(globTable,&(instr->addr1))) == NULL)
            return ERR_RT_OTHER;
    }


    if(entry->inited == false)
        return ERR_RT_OTHER;

    if(entry->type == LABEL)
        listGoto(L,entry->data);
    else
        return ERR_RT_OTHER;

    return SUCCESS;
}

ret_t equal(htab_t *globTable, tabListItem_t *tab, tInstr *instr)
{

    entry_t *opL;
    entry_t *opR;
    entry_t *vysl;

    htab_t *locTable = NULL;

    if(tab != NULL)
        locTable = tab->table;
    
    if(locTable != NULL)
    {
        if( (opL = htab_getentry(locTable,&(instr->addr1))) == NULL)
            if( (opL = htab_getentry(globTable,&(instr->addr1))) == NULL)
                return ERR_RT_OTHER;

        if( (opR = htab_getentry(locTable,&(instr->addr2))) == NULL)
            if( (opR = htab_getentry(globTable,&(instr->addr2))) == NULL)
                return ERR_RT_OTHER;

        if( (vysl = htab_getentry(locTable,&(instr->addr3))) == NULL)
            if( (vysl = htab_getentry(globTable,&(instr->addr3))) == NULL)
                return ERR_RT_OTHER;
    }
    else
    {
        if( (opL = htab_getentry(globTable,&(instr->addr1))) == NULL)
            return ERR_RT_OTHER;

        if( (opR = htab_getentry(globTable,&(instr->addr2))) == NULL)
            return ERR_RT_OTHER;

        if( (vysl = htab_getentry(globTable,&(instr->addr3))) == NULL)
            return ERR_RT_OTHER;
    }
    
    if(opL->inited == false || opR->inited == false)
        return ERR_RT_UNINIT;

    if((vysl->data = realloc(vysl->data,sizeof(bool)*(opL->length))) == NULL)
        return ERR_INTERN;

    vysl->inited = true;
    vysl->type = BOOL;

    if(opL->type == INT && opR->type == INT)
    {
        *((int *)opL->data) == *((int *)opR->data) ? (*((bool *)vysl->data) = true) :
                                                     (*((bool *)vysl->data) = false);
    }
    else if(opL->type == REAL && opR->type == REAL)
    {
        *((double *)opL->data) == *((double *)opR->data) ? (*((bool *)vysl->data) = true) :
                                                           (*((bool *)vysl->data) = false);
    }
    else if(opL->type == BOOL && opR->type == BOOL)
    {
        *((bool *)opL->data) == *((bool *)opR->data) ? (*((bool *)vysl->data) = true) :
                                                       (*((bool *)vysl->data) = false);
    }
    else if(opL->type == STRING && opR->type == STRING)
    {
        strCmpString((string *)opL->data, (string *)opR->data) == 0 ?
                                    (*((bool *)vysl->data) = true) :
                                    (*((bool *)vysl->data) = false);
    }
    return SUCCESS;
}

ret_t less(htab_t *globTable, tabListItem_t *tab, tInstr *instr)
{

    entry_t *opL;
    entry_t *opR;
    entry_t *vysl;

    htab_t *locTable = NULL;

    if(tab != NULL)
        locTable = tab->table;
    
    if(locTable != NULL)
    {
        if( (opL = htab_getentry(locTable,&(instr->addr1))) == NULL)
            if( (opL = htab_getentry(globTable,&(instr->addr1))) == NULL)
                return ERR_RT_OTHER;

        if( (opR = htab_getentry(locTable,&(instr->addr2))) == NULL)
            if( (opR = htab_getentry(globTable,&(instr->addr2))) == NULL)
                return ERR_RT_OTHER;

        if( (vysl = htab_getentry(locTable,&(instr->addr3))) == NULL)
            if( (vysl = htab_getentry(globTable,&(instr->addr3))) == NULL)
                return ERR_RT_OTHER;
    }
    else
    {
        if( (opL = htab_getentry(globTable,&(instr->addr1))) == NULL)
            return ERR_RT_OTHER;

        if( (opR = htab_getentry(globTable,&(instr->addr2))) == NULL)
            return ERR_RT_OTHER;

        if( (vysl = htab_getentry(globTable,&(instr->addr3))) == NULL)
            return ERR_RT_OTHER;
    }
    
    if(opL->inited == false || opR->inited == false)
        return ERR_RT_UNINIT;

    if((vysl->data = realloc(vysl->data,sizeof(bool)*(opL->length))) == NULL)
        return ERR_INTERN;

    vysl->inited = true;
    vysl->type = BOOL;

    if(opL->type == INT && opR->type == INT)
    {
        *((int *)opL->data) < *((int *)opR->data) ? (*((bool *)vysl->data) = true) :
                                                     (*((bool *)vysl->data) = false);
    }
    else if(opL->type == REAL && opR->type == REAL)
    {
        *((double *)opL->data) < *((double *)opR->data) ? (*((bool *)vysl->data) = true) :
                                                           (*((bool *)vysl->data) = false);
    }
    else if(opL->type == BOOL && opR->type == BOOL)
    {
        *((bool *)opL->data) < *((bool *)opR->data) ? (*((bool *)vysl->data) = true) :
                                                       (*((bool *)vysl->data) = false);
    }
    else if(opL->type == STRING && opR->type == STRING)
    {
        strCmpString((string *)opL->data, (string *)opR->data) < 0 ?
                                    (*((bool *)vysl->data) = true) :
                                    (*((bool *)vysl->data) = false);
    }
    return SUCCESS;
}

ret_t more(htab_t *globTable, tabListItem_t *tab, tInstr *instr)
{

    entry_t *opL;
    entry_t *opR;
    entry_t *vysl;

    htab_t *locTable = NULL;

    if(tab != NULL)
        locTable = tab->table;
    
    if(locTable != NULL)
    {
        if( (opL = htab_getentry(locTable,&(instr->addr1))) == NULL)
            if( (opL = htab_getentry(globTable,&(instr->addr1))) == NULL)
                return ERR_RT_OTHER;

        if( (opR = htab_getentry(locTable,&(instr->addr2))) == NULL)
            if( (opR = htab_getentry(globTable,&(instr->addr2))) == NULL)
                return ERR_RT_OTHER;

        if( (vysl = htab_getentry(locTable,&(instr->addr3))) == NULL)
            if( (vysl = htab_getentry(globTable,&(instr->addr3))) == NULL)
                return ERR_RT_OTHER;
    }
    else
    {
        if( (opL = htab_getentry(globTable,&(instr->addr1))) == NULL)
            return ERR_RT_OTHER;

        if( (opR = htab_getentry(globTable,&(instr->addr2))) == NULL)
            return ERR_RT_OTHER;

        if( (vysl = htab_getentry(globTable,&(instr->addr3))) == NULL)
            return ERR_RT_OTHER;
    }
    
    if(opL->inited == false || opR->inited == false)
        return ERR_RT_UNINIT;

    if((vysl->data = realloc(vysl->data,sizeof(bool)*(opL->length))) == NULL)
        return ERR_INTERN;

    vysl->inited = true;
    vysl->type = BOOL;

    if(opL->type == INT && opR->type == INT)
    {
        *((int *)opL->data) > *((int *)opR->data) ? (*((bool *)vysl->data) = true) :
                                                     (*((bool *)vysl->data) = false);
    }
    else if(opL->type == REAL && opR->type == REAL)
    {
        *((double *)opL->data) > *((double *)opR->data) ? (*((bool *)vysl->data) = true) :
                                                           (*((bool *)vysl->data) = false);
    }
    else if(opL->type == BOOL && opR->type == BOOL)
    {
        *((bool *)opL->data) > *((bool *)opR->data) ? (*((bool *)vysl->data) = true) :
                                                       (*((bool *)vysl->data) = false);
    }
    else if(opL->type == STRING && opR->type == STRING)
    {
        strCmpString((string *)opL->data, (string *)opR->data) > 0 ?
                                    (*((bool *)vysl->data) = true) :
                                    (*((bool *)vysl->data) = false);
    }
    return SUCCESS;
}

ret_t not_equal(htab_t *globTable, tabListItem_t *tab, tInstr *instr)
{

    entry_t *opL;
    entry_t *opR;
    entry_t *vysl;

    htab_t *locTable = NULL;

    if(tab != NULL)
        locTable = tab->table;
    
    if(locTable != NULL)
    {
        if( (opL = htab_getentry(locTable,&(instr->addr1))) == NULL)
            if( (opL = htab_getentry(globTable,&(instr->addr1))) == NULL)
                return ERR_RT_OTHER;

        if( (opR = htab_getentry(locTable,&(instr->addr2))) == NULL)
            if( (opR = htab_getentry(globTable,&(instr->addr2))) == NULL)
                return ERR_RT_OTHER;

        if( (vysl = htab_getentry(locTable,&(instr->addr3))) == NULL)
            if( (vysl = htab_getentry(globTable,&(instr->addr3))) == NULL)
                return ERR_RT_OTHER;
    }
    else
    {
        if( (opL = htab_getentry(globTable,&(instr->addr1))) == NULL)
            return ERR_RT_OTHER;

        if( (opR = htab_getentry(globTable,&(instr->addr2))) == NULL)
            return ERR_RT_OTHER;

        if( (vysl = htab_getentry(globTable,&(instr->addr3))) == NULL)
            return ERR_RT_OTHER;
    }
    
    if(opL->inited == false || opR->inited == false)
        return ERR_RT_UNINIT;

    if((vysl->data = realloc(vysl->data,sizeof(bool)*(opL->length))) == NULL)
        return ERR_INTERN;

    vysl->inited = true;
    vysl->type = BOOL;

    if(opL->type == INT && opR->type == INT)
    {
        *((int *)opL->data) != *((int *)opR->data) ? (*((bool *)vysl->data) = true) :
                                                     (*((bool *)vysl->data) = false);
    }
    else if(opL->type == REAL && opR->type == REAL)
    {
        *((double *)opL->data) != *((double *)opR->data) ? (*((bool *)vysl->data) = true) :
                                                           (*((bool *)vysl->data) = false);
    }
    else if(opL->type == BOOL && opR->type == BOOL)
    {
        *((bool *)opL->data) != *((bool *)opR->data) ? (*((bool *)vysl->data) = true) :
                                                       (*((bool *)vysl->data) = false);
    }
    else if(opL->type == STRING && opR->type == STRING)
    {
        strCmpString((string *)opL->data, (string *)opR->data) != 0 ?
                                    (*((bool *)vysl->data) = true) :
                                    (*((bool *)vysl->data) = false);
    }
    return SUCCESS;
}

ret_t less_equal(htab_t *globTable, tabListItem_t *tab, tInstr *instr)
{

    entry_t *opL;
    entry_t *opR;
    entry_t *vysl;

    htab_t *locTable = NULL;

    if(tab != NULL)
        locTable = tab->table;
    
    if(locTable != NULL)
    {
        if( (opL = htab_getentry(locTable,&(instr->addr1))) == NULL)
            if( (opL = htab_getentry(globTable,&(instr->addr1))) == NULL)
                return ERR_RT_OTHER;

        if( (opR = htab_getentry(locTable,&(instr->addr2))) == NULL)
            if( (opR = htab_getentry(globTable,&(instr->addr2))) == NULL)
                return ERR_RT_OTHER;

        if( (vysl = htab_getentry(locTable,&(instr->addr3))) == NULL)
            if( (vysl = htab_getentry(globTable,&(instr->addr3))) == NULL)
                return ERR_RT_OTHER;
    }
    else
    {
        if( (opL = htab_getentry(globTable,&(instr->addr1))) == NULL)
            return ERR_RT_OTHER;

        if( (opR = htab_getentry(globTable,&(instr->addr2))) == NULL)
            return ERR_RT_OTHER;

        if( (vysl = htab_getentry(globTable,&(instr->addr3))) == NULL)
            return ERR_RT_OTHER;
    }
    
    if(opL->inited == false || opR->inited == false)
        return ERR_RT_UNINIT;

    if((vysl->data = realloc(vysl->data,sizeof(bool)*(opL->length))) == NULL)
        return ERR_INTERN;

    vysl->inited = true;
    vysl->type = BOOL;

    if(opL->type == INT && opR->type == INT)
    {
        *((int *)opL->data) <= *((int *)opR->data) ? (*((bool *)vysl->data) = true) :
                                                     (*((bool *)vysl->data) = false);
    }
    else if(opL->type == REAL && opR->type == REAL)
    {
        *((double *)opL->data) <= *((double *)opR->data) ? (*((bool *)vysl->data) = true) :
                                                           (*((bool *)vysl->data) = false);
    }
    else if(opL->type == BOOL && opR->type == BOOL)
    {
        *((bool *)opL->data) <= *((bool *)opR->data) ? (*((bool *)vysl->data) = true) :
                                                       (*((bool *)vysl->data) = false);
    }
    else if(opL->type == STRING && opR->type == STRING)
    {
        strCmpString((string *)opL->data, (string *)opR->data) <= 0 ?
                                    (*((bool *)vysl->data) = true) :
                                    (*((bool *)vysl->data) = false);
    }
    return SUCCESS;
}

ret_t more_equal(htab_t *globTable, tabListItem_t *tab, tInstr *instr)
{

    entry_t *opL;
    entry_t *opR;
    entry_t *vysl;

    htab_t *locTable = NULL;

    if(tab != NULL)
        locTable = tab->table;
    
    if(locTable != NULL)
    {
        if( (opL = htab_getentry(locTable,&(instr->addr1))) == NULL)
            if( (opL = htab_getentry(globTable,&(instr->addr1))) == NULL)
                return ERR_RT_OTHER;

        if( (opR = htab_getentry(locTable,&(instr->addr2))) == NULL)
            if( (opR = htab_getentry(globTable,&(instr->addr2))) == NULL)
                return ERR_RT_OTHER;

        if( (vysl = htab_getentry(locTable,&(instr->addr3))) == NULL)
            if( (vysl = htab_getentry(globTable,&(instr->addr3))) == NULL)
                return ERR_RT_OTHER;
    }
    else
    {
        if( (opL = htab_getentry(globTable,&(instr->addr1))) == NULL)
            return ERR_RT_OTHER;

        if( (opR = htab_getentry(globTable,&(instr->addr2))) == NULL)
            return ERR_RT_OTHER;

        if( (vysl = htab_getentry(globTable,&(instr->addr3))) == NULL)
            return ERR_RT_OTHER;
    }
    
    if(opL->inited == false || opR->inited == false)
        return ERR_RT_UNINIT;

    if((vysl->data = realloc(vysl->data,sizeof(bool)*(opL->length))) == NULL)
        return ERR_INTERN;

    vysl->inited = true;
    vysl->type = BOOL;

    if(opL->type == INT && opR->type == INT)
    {
        *((int *)opL->data) >= *((int *)opR->data) ? (*((bool *)vysl->data) = true) :
                                                     (*((bool *)vysl->data) = false);
    }
    else if(opL->type == REAL && opR->type == REAL)
    {
        *((double *)opL->data) >= *((double *)opR->data) ? (*((bool *)vysl->data) = true) :
                                                           (*((bool *)vysl->data) = false);
    }
    else if(opL->type == BOOL && opR->type == BOOL)
    {
        *((bool *)opL->data) >= *((bool *)opR->data) ? (*((bool *)vysl->data) = true) :
                                                       (*((bool *)vysl->data) = false);
    }
    else if(opL->type == STRING && opR->type == STRING)
    {
        strCmpString((string *)opL->data, (string *)opR->data) >= 0 ?
                                    (*((bool *)vysl->data) = true) :
                                    (*((bool *)vysl->data) = false);
    }
    return SUCCESS;
}

//PO TEJTO INSTRUKCII MUSI NASLEDOVAT CONTINUE
ret_t call(    htab_t *globTable,
            tabListItem_t *tab,
            tInstr *instr,
            tabList_t *tabStack,
            paramList_t *paramList,
            tListOfInstr *instList,
            addStack_t *addStack)
{
    entry_t *func = NULL;
    entry_t *parFromTable = NULL;
    entry_t *parToTable = NULL;
    htab_t *funcTab = NULL;
    htab_t *locTable = NULL;
    paramEnt *parameter = NULL;
    string *parFromStack = NULL;
    ret_t ret = SUCCESS;

    if(tab != NULL)
        locTable = tab->table;
    //najdi meno funkcie
    if((func = htab_getentry(globTable,&(instr->addr1))) == NULL)
        return ERR_INTERN;

    //vytvor tabulku
    if((funcTab = htab_copy(((func_t *)func->data)->defHtab)) == NULL)
        return ERR_INTERN;

    //najdi prvy parameter zo zoznamu parametrov
    parameter = ((func_t *)func->data)->lastParam;
    
    //nacitaj parametre
    //   uloz do novej tabulky na miesta parametrov parametre z predchadzajucej tabulky
    //   predchadzajuca tabulka je globalna (ak sa vola funkcia z programu)
    //   alebo je L->first->next.table (L->first je vrchol zasobnika) - tzn ze je
    //   tabulka funkcie z ktorej sa volalo

    if(locTable == NULL)//novu funkciu vola main
    {
        //hladaj parametre v globTable
        while(parameter != NULL)
        {
            parFromStack = getParam(paramList);
            //najdi ho v tabulke
            if ((parFromTable = htab_getentry(globTable,parFromStack)) == NULL)
                return ERR_INTERN;
            //prirad ho do novej tabulky

            if ((parToTable = htab_getentry(funcTab,&(parameter->paramKey))) == NULL)
                return ERR_INTERN;

            if(parFromTable->inited == false)
                goto Err_rt_uninit;
            
            if(parToTable->type == parFromTable->type)
            {
                parToTable->inited = true;
                parToTable->literal = parFromTable->literal;
                parToTable->begin = parFromTable->begin;
                parToTable->length = parFromTable->length;
                switch(parFromTable->type)
                {
                    case INT:
                        memcpy(parToTable->data,parFromTable->data,sizeof(int)*parFromTable->length);
                        break;
                    case REAL:
                        memcpy(parToTable->data,parFromTable->data,sizeof(double)*parFromTable->length);
                        break;
                    case BOOL:
                        memcpy(parToTable->data,parFromTable->data,sizeof(bool)*parFromTable->length);
                        break;
                    case STRING:
                        /*if((parToTable->data = realloc(parToTable->data,sizeof(string)*(parFromTable->length))) == NULL)
                            return ERR_INTERN;
                        if(strInit(parToTable->data) != STR_SUCCESS)
                            return ERR_INTERN;*/
                        if(strCopyString(parToTable->data,parFromTable->data) != STR_SUCCESS)
                            return ERR_INTERN;
                        break;
                    default:
                        return ERR_SEM_TYPE;
                }
            }
            else 
                return ERR_SEM_TYPE;

            parameter = parameter->prevParam;

            strFree(parFromStack);
            free(parFromStack);
        }
    }
    else//novu funkciu vola ina funkcia
    {
        //hladaj parametre v locTable
        while(parameter != NULL)
        {
            parFromStack = getParam(paramList);
            //najdi ho v tabulke
            if ((parFromTable = htab_getentry(locTable,parFromStack)) == NULL)
                if ((parFromTable = htab_getentry(globTable,parFromStack)) == NULL)
                    return ERR_INTERN;

            if ((parToTable = htab_getentry(funcTab,&(parameter->paramKey))) == NULL)
                return ERR_INTERN;
            //prirad ho do novej tabulky

            if(parFromTable->inited == false)
                goto Err_rt_uninit;

            if(parToTable->type == parFromTable->type)
            {
                parToTable->inited = true;
                parToTable->literal = parFromTable->literal;
                parToTable->begin = parFromTable->begin;
                parToTable->length = parFromTable->length;
                switch(parFromTable->type)
                {
                    case INT:
                        memcpy(parToTable->data,parFromTable->data,sizeof(int)*parFromTable->length);
                        break;
                    case REAL:
                        memcpy(parToTable->data,parFromTable->data,sizeof(double)*parFromTable->length);
                        break;
                    case BOOL:
                        memcpy(parToTable->data,parFromTable->data,sizeof(bool)*parFromTable->length);
                        break;
                    case STRING:
                        /*if((parToTable->data = realloc(parToTable->data,sizeof(string)*(parFromTable->length))) == NULL)
                            return ERR_INTERN;
                        if(strInit(parToTable->data) != STR_SUCCESS)
                            return ERR_INTERN;*/
                        if(strCopyString(parToTable->data,parFromTable->data) != STR_SUCCESS)
                            return ERR_INTERN;
                        break;
                    default:
                        return ERR_SEM_TYPE;
                }
            }
            else 
                return ERR_SEM_TYPE;
            parameter = parameter->prevParam;

            strFree(parFromStack);
            free(parFromStack);
        }
    }

    //uloz navratovu adresu na stack
    if((ret = addAddress(addStack,instList->active->nextItem)) != SUCCESS)
        return ret;

    //skoc na prvu instrukciu danej funkcie

    instList->preactive = instList->active;
    instList->active = ((func_t *)func->data)->pStartInst;

    //pushni tabulku na stack tabuliek
    if(addTabToList(tabStack, funcTab) != SUCCESS)
        return ERR_INTERN;
    
    htab_free(funcTab);
    return SUCCESS;

    Err_rt_uninit:
    htab_free(funcTab);
    strFree(parFromStack);
    free(parFromStack);
    return ERR_RT_UNINIT;
}

//PO TEJTO INSTRUKCII MUSI NASLEDOVAT CONTINUE
ret_t ret(    tabList_t *tabList, 
            tListOfInstr *instList,
            addStack_t *addStack)
{
    //zrus tabulku danej funkcie

    removeTab(tabList);
    //skoc na instrukciu nasledujucu po volani funkcie
    instList->preactive = instList->active;
    instList->active = removeAddress(addStack);
    
    if(instList->active == NULL)
        return ERR_INTERN;
    
    return SUCCESS;
}

ret_t readln(htab_t *globTable, tabListItem_t *tab, tInstr *instr)
{
    entry_t *read = NULL;
    string str;
    string number;
    int c;
    char *control;
    int line_index;
    read_state_t state;

    htab_t *locTable = NULL;

    if(tab != NULL)
        locTable = tab->table;
    

    if(strInit(&str) != STR_SUCCESS)
        return ERR_INTERN;

    if(strInit(&number) != STR_SUCCESS)
    {
        strFree(&str);
        return ERR_INTERN;
    }

    if(locTable != NULL)
    {
        if( (read = htab_getentry(locTable,&(instr->addr1))) == NULL)
            if( (read = htab_getentry(globTable,&(instr->addr1))) == NULL)
                goto Err_rt_other;
    }
    else
    {
        if( (read = htab_getentry(globTable,&(instr->addr1))) == NULL)
            goto Err_rt_other;
    }

    while(1)
    {
        c = fgetc(stdin);
        if(c == '\n' || c == EOF)
            break;
        if (strAddChar(&str,c) != STR_SUCCESS)
            goto Err_intern;
    }


    if(read->type == INT || read->type == REAL)
    {
        state = START;
        line_index = 0;
        //nacitaj len prvy token z riadku
        while((c = str.str[line_index]) != '\0')
        {
            switch(state)
            {
                case START:
                    if(isspace(c))
                    {
                        line_index++;
                    }
                    else
                    {
                        if(strAddChar(&number,str.str[line_index]) != STR_SUCCESS)
                            goto Err_intern;
                        line_index++;
                        state = TOKEN;
                    }
                    break;
                
                case TOKEN:
                    if(isspace(c))
                    {
                        line_index++;
                        state = END;
                    }
                    else
                    {
                        if(strAddChar(&number,str.str[line_index]) != STR_SUCCESS)
                            goto Err_intern;
                        line_index++;
                    }
                    break;

                case END:
                    line_index++;
                    break;
            }
        }
    }

    switch(read->type)
    {
        case INT:
            if((read->data = realloc(read->data,sizeof(int))) == NULL)
                goto Err_intern;

            *(int *)read->data = strtol(number.str,&control,10);
            
            if(*control != 0)
                goto Err_rt_read;

            break;

        case REAL:
            if((read->data = realloc(read->data,sizeof(double))) == NULL)
                goto Err_intern;

            *(double *)read->data = strtod(number.str,&control);

            if(*control != 0)
                goto Err_rt_read;

            break;

        case STRING:
            //if(read->inited == true)
                strFree((string *)read->data);
            
            if(strInit((string *)read->data) != STR_SUCCESS)
                goto Err_intern;
            
            if(strCopyString((string *)read->data, &str) != STR_SUCCESS)
                goto Err_intern;
            
            break;

        default:
            strFree(&str);
            strFree(&number);
            return ERR_SEM_TYPE;
            break;
    }
    read->inited = true;

    strFree(&str);
    strFree(&number);
    return SUCCESS;

    Err_intern:
        strFree(&str);
        strFree(&number);
        return ERR_INTERN;

    Err_rt_other:
        strFree(&str);
        strFree(&number);
        return ERR_RT_OTHER;

    Err_rt_read:
        strFree(&str);
        strFree(&number);
        return ERR_RT_READ;
}

ret_t sort(htab_t *globTable, tabListItem_t *tab, tInstr *instr)
{
    entry_t *dest;
    entry_t *src;

    htab_t *locTable = NULL;

    if(tab != NULL)
        locTable = tab->table;

    if(locTable != NULL)
    {
        if( (src = htab_getentry(locTable,&(instr->addr1))) == NULL)
            if( (src = htab_getentry(globTable,&(instr->addr1))) == NULL)
                return ERR_RT_OTHER;

        if( (dest = htab_getentry(locTable,&(instr->addr3))) == NULL)
            if( (dest = htab_getentry(globTable,&(instr->addr3))) == NULL)
                return ERR_RT_OTHER;
    }
    else
    {
        if( (src = htab_getentry(globTable,&(instr->addr1))) == NULL)
            return ERR_RT_OTHER;

        if( (dest = htab_getentry(globTable,&(instr->addr3))) == NULL)
            return ERR_RT_OTHER;
    }

    if(src->inited == false)
        return ERR_RT_UNINIT;

    /*if(dest->inited == false)
    {
        if((dest->data = realloc(dest->data,sizeof(string)*src->length)) == NULL)
            return ERR_INTERN;

        if(strInit(dest->data) != STR_SUCCESS)
            return ERR_INTERN;
    }*/

    if(strCopyString(dest->data,src->data) != STR_SUCCESS)
        return ERR_INTERN;

    hsort(((string *)dest->data)->str,strlen(((string *)dest->data)->str));
    dest->inited = true;
    return SUCCESS;
}

ret_t len(htab_t *globTable, tabListItem_t *tab, tInstr *instr)
{
    entry_t *str;
    entry_t *length;

    htab_t *locTable = NULL;

    if(tab != NULL)
        locTable = tab->table;

    if(locTable != NULL)
    {
        if( (str = htab_getentry(locTable,&(instr->addr1))) == NULL)
            if( (str = htab_getentry(globTable,&(instr->addr1))) == NULL)
                return ERR_RT_OTHER;

        if( (length = htab_getentry(locTable,&(instr->addr3))) == NULL)
            if( (length = htab_getentry(globTable,&(instr->addr3))) == NULL)
                return ERR_RT_OTHER;
    }
    else
    {
        if( (str = htab_getentry(globTable,&(instr->addr1))) == NULL)
            return ERR_RT_OTHER;

        if( (length = htab_getentry(globTable,&(instr->addr3))) == NULL)
            return ERR_RT_OTHER;
    }

    if(str->inited == false)
        return ERR_RT_UNINIT;

    if(str->type != STRING || length->type != INT)
        return ERR_SEM_TYPE;

    if((length->data = realloc((length->data),sizeof(int))) == NULL)
        return ERR_INTERN;
    
    *(int *)length->data = strLength((string *)str->data);
    length->inited = true;

    return SUCCESS;
}

ret_t copy(htab_t *globTable, tabListItem_t *tab, tInstr *instr)
{
    entry_t *src = NULL;
    entry_t *dest = NULL;
    entry_t *start_entry = NULL;
    entry_t *len_entry = NULL;
    string ret_val;
    int srcLen = 0;
    int end = 0;
    int start = 0;
    unsigned int paramLen = 0;

    htab_t *locTable = NULL;

    if(strInit(&ret_val) != STR_SUCCESS)
        return ERR_INTERN;

    if(strAddConstString(&ret_val,RET_STR) != STR_SUCCESS)
        goto Err_intern;
    

    if(tab != NULL)
        locTable = tab->table;

    if(locTable != NULL)
    {
        if( (src = htab_getentry(locTable,&(instr->addr1))) == NULL)
            if( (src = htab_getentry(globTable,&(instr->addr1))) == NULL)
                goto Err_rt_other;

        if( (start_entry = htab_getentry(locTable,&(instr->addr2))) == NULL)
            if( (start_entry = htab_getentry(globTable,&(instr->addr2))) == NULL)
                goto Err_rt_other;

        if( (len_entry = htab_getentry(locTable,&(instr->addr3))) == NULL)
            if( (len_entry = htab_getentry(globTable,&(instr->addr3))) == NULL)
                goto Err_rt_other;

        if( (dest = htab_getentry(locTable,&ret_val)) == NULL)
            if( (dest = htab_getentry(globTable,&ret_val)) == NULL)
                goto Err_rt_other;
    }
    else
    {
        if( (src = htab_getentry(globTable,&(instr->addr1))) == NULL)
            goto Err_rt_other;

        if( (start_entry = htab_getentry(globTable,&(instr->addr2))) == NULL)
            goto Err_rt_other;

        if( (len_entry = htab_getentry(globTable,&(instr->addr3))) == NULL)
            goto Err_rt_other;

        if( (dest = htab_getentry(globTable,&ret_val)) == NULL)
            goto Err_rt_other;
    }

    if(    src->inited == false ||
        start_entry->inited == false ||
        len_entry->inited == false)

        goto Err_rt_uninit;

    start = *((int *)start_entry->data) - 1; //zaciatocny index (znizeny o jedna Pascal -> C)
    if(start < 0)
        start = 0;

    paramLen = *((int *)len_entry->data);    //dlzka kopirovanej casti
    end = start + (paramLen - 1);        //koncovy index pola
    srcLen = strLength(src->data);
    
    if(start >= srcLen || end <= start)
    {
  /*      if(dest->inited == true)
            strClear(dest->data);
        else
        {
            if(strInit(dest->data) != STR_SUCCESS)
                goto Err_intern;*/
            if(strAddConstString(dest->data,"") != STR_SUCCESS)
                goto Err_intern;
       // }
        dest->inited = true;
    }
    else if(end <=srcLen)
    {
        //if(dest->inited == false)
          //  if(strInit(dest->data) != STR_SUCCESS)
            //    goto Err_intern;

        if(paramLen >= ((string *)dest->data)->allocSize)
        {
            if((((string *)dest->data)->str = 
                        realloc(((string *)dest->data)->str,paramLen + 1)) == NULL)
                goto Err_intern;

            ((string *)dest->data)->allocSize = paramLen + 1;
        }

            memcpy( ((string *)dest->data)->str, 
                    ((string *)src->data)->str + start, 
                    paramLen);

            ((string *)dest->data)->str[paramLen] ='\0';
            dest->inited = true;
    }
    else
    {
        paramLen = srcLen - start;

        if(paramLen >= ((string *)dest->data)->allocSize)
        {
            if((((string *)dest->data)->str = 
                        realloc(((string *)dest->data)->str,paramLen + 1)) == NULL)
                goto Err_intern;

            ((string *)dest->data)->allocSize = paramLen + 1;
        }

            memcpy( ((string *)dest->data)->str, 
                    ((string *)src->data)->str + start, 
                    paramLen);

            ((string *)dest->data)->str[paramLen] ='\0';
            dest->inited = true;
            
    }

    strFree(&ret_val);
    return SUCCESS;

    Err_rt_uninit:
        strFree(&ret_val);
        return ERR_RT_UNINIT;

    Err_rt_other:
        strFree(&ret_val);
        return ERR_RT_OTHER;

    Err_intern:
        strFree(&ret_val);
        return ERR_INTERN;
}

ret_t find(htab_t *globTable, tabListItem_t *tab, tInstr *instr)
{
    entry_t *text;
    entry_t *pattern;
    entry_t *index;
        
    int result;

    htab_t *locTable = NULL;

    if(tab != NULL)
        locTable = tab->table;

    if(locTable != NULL)
    {
        if( (text = htab_getentry(locTable,&(instr->addr1))) == NULL)
            if( (text = htab_getentry(globTable,&(instr->addr1))) == NULL)
                return ERR_RT_OTHER;

        if( (pattern = htab_getentry(locTable,&(instr->addr2))) == NULL)
            if( (pattern = htab_getentry(globTable,&(instr->addr2))) == NULL)
                return ERR_RT_OTHER;

        if( (index = htab_getentry(locTable,&(instr->addr3))) == NULL)
            if( (index = htab_getentry(globTable,&(instr->addr3))) == NULL)
                return ERR_RT_OTHER;
    }
    else
    {
        if( (text = htab_getentry(globTable,&(instr->addr1))) == NULL)
            return ERR_RT_OTHER;

        if( (pattern = htab_getentry(globTable,&(instr->addr2))) == NULL)
            return ERR_RT_OTHER;

        if( (index = htab_getentry(globTable,&(instr->addr3))) == NULL)
            return ERR_RT_OTHER;
    }

    if(text->inited == false || pattern->inited == false)
        return ERR_RT_UNINIT;

    if((index->data = realloc(index->data,sizeof(int)*index->length)) == NULL)
        return ERR_INTERN;

    result = kmp_find(text->data,pattern->data);
    if(result == -2)
        return ERR_INTERN;
    else
        result++; //do pascalovskeho indexovania

    *(int *)index->data = result;
    index->inited = true;

    return SUCCESS;
}

