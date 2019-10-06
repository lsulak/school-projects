/**********************************************************|
|**        Projekt do predmetov IFJ a IAL 2014/2015      **|
|**Subor : parser.c                                      **|
|**Popis : Obsahuje funkcie pre syntakticku analyzu      **|
|**Autori:                                               **|
|**       Matej Muzila      xmuzil00                     **|
|**       Milos Volny       xvolny02                     **|
|**       Marek Minda       xminda00                     **|
|**********************************************************/


#include <stdio.h>
#include <stdlib.h>
#include "interpret.h"
#include "parser.h"
#include "str.h"
#include "psa_new.h"
#include "ilist.h"
#include "debug.h"

token_t token;
string attr;

static ret_t command(htab_t * tGlob, htab_t * tLoc, tListOfInstr * iList);
static ret_t block(htab_t * tGlob, htab_t * tLoc, tListOfInstr * iList);
static ret_t fundef(htab_t *symbolTable,tListOfInstr *IList);
static ret_t vardef(htab_t *fSymbolTable);
static ret_t parsWhile(htab_t * tGlob, htab_t * tLoc, tListOfInstr * iList);
static ret_t parsIf(htab_t * tGlob, htab_t * tLoc, tListOfInstr * iList);
static ret_t parsIdent(htab_t * tGlob, htab_t * tLoc, tListOfInstr * iList);
static ret_t parsWrite(htab_t * tGlob, htab_t * tLoc, tListOfInstr * iList);


#ifdef DEBUG
void prInstList(tListOfInstr *L)
{
    tListItem *tmp = L->first;
    
    fprintf(stderr, "-------------------------------------------------------------+\n");
    while(tmp != NULL)
    {
        fprintf(stderr, "%11s |%10s |%10s |%10s |%10lu |\n", iTypeChar[tmp->Instruction.instType],
                                                tmp->Instruction.addr1.str,
                                                tmp->Instruction.addr2.str,
                                                tmp->Instruction.addr3.str,
                                                (long unsigned)tmp);
        tmp = tmp->nextItem;
        fprintf(stderr,"------------------------------------------------------------+\n");
    }
}
#endif

ret_t parse(htab_t *tGlob, tListOfInstr *IList)
{
    ret_t result = ERR_INTERN; 

    /* inicializacia stringu pre atribut tokenu */
    if (strInit(&attr) != STR_SUCCESS)
        return ERR_INTERN;

    if(strAddConstString(&attr, RET_BOOL) != STR_SUCCESS
            || htab_new(tGlob, &attr, BOOL, ARRAY_BEG, 1, false, NULL, NULL) != HTAB_SUCCESS)
        goto End;

    strClear(&attr);
    if(strAddConstString(&attr, RET_INT) != STR_SUCCESS
            || htab_new(tGlob, &attr, INT, ARRAY_BEG, 1, false, NULL, NULL) != HTAB_SUCCESS)
        goto End;

    strClear(&attr);
    if(strAddConstString(&attr, RET_REAL) != STR_SUCCESS
            || htab_new(tGlob, &attr, REAL, ARRAY_BEG, 1, false, NULL, NULL) != HTAB_SUCCESS)
        goto End;

    strClear(&attr);
    if(strAddConstString(&attr, RET_STR) != STR_SUCCESS
            || htab_new(tGlob, &attr, STRING, ARRAY_BEG, 1, false, NULL, NULL) != HTAB_SUCCESS)
        goto End;


    strClear(&attr);
    
    if ((token = getToken(&attr)) == T_ERR_LEX)
    {
        result = ERR_LEX;
        goto End;
    }

    result = program(tGlob, IList);

End:
    strFree(&attr);

    DEB_PRINTF("PARSER KONIEC: %d\n\n\n", result);
#ifdef DEBUG
    DEB_PRINTF(">>>>>>>>>>>>>>GLOBALNA_TABULKA\n");
    htab_foreach(tGlob, &htab_print);    
#endif
    return result;    
}

ret_t program(htab_t *symbolTable,tListOfInstr *IList)
{
    tListItem * pIns;

    int result = SUCCESS;

    // Globalne premenne
    if (token == T_VAR)
    {
        result = vardef(symbolTable);
        if (result != SUCCESS)
            return result;
    }

    // Funkcie
    while(token == T_FUNCTION)
    {
        result = fundef(symbolTable,IList);
        if (result != SUCCESS)
            return result;
    }

    
    /* vlozime navestie na zaciatok mainu */
    if (AddInstr(IList, I_LAB, &sNULL, &sNULL, &sNULL) != SUCCESS)
        return ERR_INTERN;

    /* Zistime adresu instrukcie zaciatku mainu */
    if ((pIns = listGetPointerLast(IList)) == NULL)
        return ERR_INTERN;

    /* zariadime aby bol bola aktivna prva instrukcia mainu programu */
    IList->preactive = NULL;
    IList->active = pIns;

    /* Ak secko ide ok skonci ; */
    if(token != T_BEGIN)
        return ERR_SYNT;

    if((result = block(symbolTable, symbolTable, IList)) != SUCCESS)
        return result;
    
    if ((token = getToken(&attr)) == T_ERR_LEX)
        return ERR_LEX;
    // Program musi koncit bodkou
    if(token != T_POINT)
        return ERR_SYNT;
        
    if ((token = getToken(&attr)) == T_ERR_LEX)
        return ERR_LEX;

    if(token != T_EOF_S)
        return ERR_SYNT;

#ifdef DEBUG  
    prInstList(IList);
#endif

    /* checkneme ci su secky funkcie definovane */
    if (htab_foreach(symbolTable, &funcIsInited))
        return ERR_SEM_UNDEF;

    return SUCCESS;
}



type_t typeDecode(token_t token)
{
    switch (token)
    {
        case T_INTEGER:
            return INT;
        case T_REAL:
            return REAL;
        case T_BOOLEAN:
            return BOOL;
        case T_STRING:
            return STRING;
        default:
            return NOTYPE;
    }
}

static ret_t fundef(htab_t *tGlob,tListOfInstr *IList)
{
    int result = SUCCESS;
    hExit_t  hRet;
    htab_listitem * hItem = NULL;
    tListItem * pIns = NULL;
    func_t * func = NULL;
    string  retKey;
    string id;
    string funcId;

    if(strInit(&id) != STR_SUCCESS)
        return ERR_INTERN;

    if(strInit(&funcId) != STR_SUCCESS)
    {
        result = ERR_INTERN;
        goto strEnd;
    }

    if(strInit(&retKey) != STR_SUCCESS)
    {
        result = ERR_INTERN;
        goto strEnd2;
    }

    func = funcInit();
    if (func == NULL)
    {
        result = ERR_INTERN;
        goto fmalEnd;
    }


    stateFnc_t state = S_FNC_START;
    type_t type;


    while(result == SUCCESS)
    {
        if ((token=getToken(&attr)) == T_ERR_LEX)
        {
            result = ERR_LEX;
            goto End;
        }

        /* musime ocheckovat ci neprislo copy || length, lebo tento ultra
         * super jazyk ich nema medzi klucovymi slovami */
        if (token == T_COPY || token == T_LENGTH)
        {
            result = ERR_SEM_UNDEF;
            goto End;
        }

        switch(state)
        {
            case S_FNC_START:
                if (token != T_IDENTIFIER)
                {
                    result = ERR_SYNT;
                    goto End;
                }

                if (strCopyString(&funcId, &attr) != STR_SUCCESS)
                {
                    result = ERR_INTERN;
                    goto End;
                }
                
                state = S_FNC_ID;
                break;

            case S_FNC_ID:
                if (token != T_L_BRACKET)
                {
                    result =  ERR_SYNT;
                    goto End;
                }
                else 
                    state = S_FNC_LB;

                break;

            case S_FNC_LB:
                
                // funkcia je bez parametrov
                if (token == T_R_BRACKET)    
                    state = S_FNC_RB;

                // prisiel nam nazov parametra
                else if (token == T_IDENTIFIER)
                {
                    if(strCopyString(&id, &attr) != STR_SUCCESS)
                    {
                        result = ERR_INTERN;
                        goto End;
                    }
                    state = S_FNC_PARAM_ID;
                }

                else
                {
                    result = ERR_SYNT;
                    goto End;
                }
                break;

            case S_FNC_PARAM_ID:
                    if(token != T_COLON)
                    {
                        result = ERR_SYNT;
                        goto End;
                    }
                    state = S_FNC_PARAM_COL;
                    break;

            case S_FNC_PARAM_COL:
                // ma parameter validny typ?
                if((type = typeDecode(token)) == NOTYPE)
                {
                    result = ERR_SYNT;
                    goto End;
                }

                 // vopchame parameter do funkcie
                else if((hRet = funcAddParam(func, type, &id) != HTAB_SUCCESS))
                {
                    if(hRet == HTAB_SYM_EXISTS)
                        result =  ERR_SEM_UNDEF;
                    else
                        result = ERR_INTERN;
                    goto End;
                }
                state = S_FNC_PARAM_TYPE;
                break;
                
            case S_FNC_PARAM_TYPE:
                if (token == T_SEMICOLON)
                    state = S_FNC_LB;
                else if (token == T_R_BRACKET)
                    state = S_FNC_RB;
                else 
                {
                    result = ERR_SYNT;
                    goto End;
                }
                break;

            case S_FNC_RB:
                if(token != T_COLON)
                {
                    result = ERR_SYNT;
                    goto End;
                }
                state = S_FNC_COL;
                break;

            case S_FNC_COL:
                if((type = typeDecode(token)) == NOTYPE) // ma funkcia validny typ?
                {
                    result = ERR_SYNT;
                    goto End;
                }

                else
                {
                    funcAddType(func, type);

                    /* pridanie navratovej navratovej premennej
                     * do loc tab funkcie */
                    if((hRet = htab_new(func->defHtab, &funcId, type, 0, 1,
                                false, NULL, NULL)) != HTAB_SUCCESS) // pridame do tab. premennu
                    {
                        if(hRet == HTAB_SYM_EXISTS)
                            result = ERR_SEM_UNDEF;
                        else
                            result = ERR_INTERN;
                        goto End;
                    }

                }
                state = S_FNC_TYPE;
                break;

            case S_FNC_TYPE:
                if(token != T_SEMICOLON)
                {
                    result = ERR_SYNT;
                    goto End;
                }

                state = S_FNC_SCOL;
                break;

            case S_FNC_FWD:
                if (token != T_SEMICOLON)
                {
                    /* za forward musi byt semicolon */
                    result =  ERR_SYNT;
                    goto End;
                }
                DEB_PRINTF(">>>>>>>>>>>>>>>>> DEKLARACIA: %s\n", funcId.str);

                /* skusime ci takato funkcia uz nahodou v tabulke neni */
                if ((hItem = htab_get(tGlob, &funcId)) != NULL)
                {
                    result = ERR_SEM_UNDEF;
                    goto End;
                }
                
                /* vlozime funkciu do globalnej tabulky */
                if ((hRet = htab_new(tGlob, &funcId, FUNC, ARRAY_BEG, 1,
                                false, func, &hItem)) != HTAB_SUCCESS) 
                {
                    result = ERR_INTERN;
                    goto End;
                }

                if ((token = getToken(&attr)) == T_ERR_LEX)
                    result =  ERR_LEX;

                /* validny koniec */
                goto End;

            case S_FNC_SCOL:             // POZOR NA CHYBU, nekonci vzdy break;
                
                /* forward deklaracia */
                if (token == T_FORWARD)
                {
                    state = S_FNC_FWD;
                    break;
                }

                /* token neni ani var ani forward ani begin-> chyba */
                else if (token != T_VAR && token != T_BEGIN)
                {
                    result = ERR_SYNT;
                    goto End;
                }

                DEB_PRINTF(">>>>>>>>>>>>>>>>>>>>>>>>> DEFINICIA: %s\n", funcId.str);
                
                /* ak uz najde v tabulke tak isto nazvanu funkciu, porovna ich */
                if ((hItem = htab_get(tGlob, &funcId)) != NULL)
                {
                    /* zistime, ci bola uz vytvorena funkcia aj deklarovana */
                    if(hItem->entry.inited == true || hItem->entry.type != FUNC)
                    {
                        result = ERR_SEM_UNDEF;
                        hItem = NULL;
                        goto End;
                    }

                    /* ak su neni totozne -> sematicka chyba */
                    if(funcCompare(((func_t *)(hItem->entry.data)), func)
                            != HTAB_SUCCESS)
                    {
                        result = ERR_SEM_UNDEF;
                        goto End;
                    }
                }

                /* ak taka funkcia este neni deklarovana skusi ju vytvorit */
                else if ((hRet = htab_new(tGlob, &funcId, FUNC, ARRAY_BEG, 1,
                                false, func, &hItem)) != HTAB_SUCCESS) 
                {
                    result = ERR_INTERN;
                    goto End;
                }
                
                /* ak mame deklarovane lokalne premenne */
                if(token == T_VAR)
                /* zistime ake ma premenne  */
                    if ((result = vardef(((func_t *)(hItem->entry.data))->defHtab)) != SUCCESS)
                        goto End;

            //case S_FNC_BEGIN:
                if(token != T_BEGIN)
                {
                    result = ERR_SYNT;
                    goto End;
                }


                /* Vlozime vstupny bod, nemusi sa to podarit */
                if(AddInstr(IList, I_LAB, &sNULL, &sNULL, &sNULL) != SUCCESS)
                {
                    result = ERR_INTERN;
                    goto End;
                }

                /* vytiahneme si poslednu instrukciu */
                if ((pIns = listGetPointerLast(IList)) == NULL)
                {
                    result = ERR_INTERN;
                    goto End;
                }

                /* vopchame adr. tejto instrukcie ako vstupny bod do funkcie */
                funcAddAddress(hItem, pIns);

                /* spracujeme telo funkcie */
                if ((result = block(tGlob,
                                ((func_t *)(hItem->entry.data))->defHtab,
                                IList)) != SUCCESS)
                    goto End;

                /* vysledok vunkcie priradime do globalnej premennej, ale este
                 * nevieme do akej*/

                switch(((func_t *)(hItem->entry.data))->rettype)
                {
                    case BOOL:
                        if (strAddConstString(&retKey, RET_BOOL) != STR_SUCCESS)
                        {
                            result = ERR_INTERN;
                            goto End;
                        }
                        break;

                    case INT:
                        if (strAddConstString(&retKey, RET_INT) != STR_SUCCESS)
                        {
                            result = ERR_INTERN;
                            goto End;
                        }
                        break;

                    case REAL:
                        if (strAddConstString(&retKey, RET_REAL) != STR_SUCCESS)
                        {
                            result = ERR_INTERN;
                            goto End;
                        }
                        break;

                    case STRING:
                        if (strAddConstString(&retKey, RET_STR) != STR_SUCCESS)
                        {
                            result = ERR_INTERN;
                            goto End;
                        }
                        break;

                    default:
                        return ERR_INTERN;
                }
                    
                /* priradime vysledok funkcie */
                if(AddInstr(IList, I_ASSIGN, &retKey, &sNULL, &funcId) != SUCCESS)
                {
                    result = ERR_INTERN;
                    goto End;
                }

                /* za na koniec funkcie dame RET */
                if(AddInstr(IList, I_RET, &sNULL, &sNULL, &sNULL) != SUCCESS)
                {
                    result = ERR_INTERN;
                    goto End;
                }

                // nacitame dalsi token
                if((token =  getToken(&attr)) == (token_t)T_ERR_LEX)
                {
                    result = ERR_LEX;
                    goto End;
                }

                // ak za funkciu neni ; je to zle
                if (token != T_SEMICOLON)
                {
                    result = ERR_SYNT;
                    goto End;
                }

                if((token =  getToken(&attr)) == (token_t)T_ERR_LEX)
                    result = ERR_LEX;

                /* validne goto */    
                goto End;
            default: 
                result =  ERR_SYNT;

        }
    }

    

    End:

    DEB_PRINTF(">>>>>>>>>>>>>>END: %s\n", funcId.str);
#ifdef DEBUG
    DEB_PRINTF(">>>>>>>>>>>>>>TABULKA: %s\n", funcId.str);

    if (hItem != NULL)
        htab_foreach(((func_t *)(hItem->entry.data))->defHtab, &htab_print);

    DEB_PRINTF(">>>>>>>>>>TABULKA - KONIEC: %s\n\n", funcId.str);
#endif

        funcFree(func);
    fmalEnd:
        strFree(&retKey);
    strEnd2:
        strFree(&funcId);
    strEnd:
        strFree(&id);
        return result;
        

}

            
static ret_t parsWhile(htab_t * tGlob, htab_t * tLoc, tListOfInstr * iList)
{
    htab_listitem * l1 = NULL;
    htab_listitem * l2 = NULL;
    tListItem * pIns1 = NULL;
    tListItem * pIns2 = NULL;
    ret_t ret;
    
    if ((token=getToken(&attr)) == T_ERR_LEX)
        return ERR_LEX;

    /* priprava navesti */
    if(htab_newLabel(tLoc, &pIns1, NULL, &l1) != HTAB_SUCCESS)
        return ERR_INTERN;

    if(htab_newLabel(tLoc, &pIns2, NULL, &l2) != HTAB_SUCCESS)
        return ERR_INTERN;

    // navestie pred podmienkou
    if(AddInstr(iList, I_LAB, &sNULL, &sNULL, &sNULL) != SUCCESS)
        return ERR_INTERN;

    /* vytiahneme poslednu instrukciu  za ucelom zistenia adresy navestia */
    if ((pIns1 = listGetPointerLast(iList)) == NULL)
        return ERR_INTERN;

    /* vyhodnotenie podmienky  */
    if ((ret =PSA(tGlob, tLoc, iList, BOOL)) != SUCCESS)
        return ret;

    /* musi nasledovat token do */
    if(token != T_DO)
        return ERR_SYNT;
    
    // ak podmienka neplati skaceme
    if(AddInstr(iList, I_JUMP_ELSE, &(l1->key), &sNULL, &sNULL) != SUCCESS)
        return ERR_INTERN;

    if((token=getToken(&attr)) == T_ERR_LEX)
        return ERR_LEX;

    // Podmienka plati
    if((ret = block(tGlob, tLoc, iList)) != SUCCESS)
        return ret;

    // Skocit na podmienku
    if(AddInstr(iList, I_JUMP, &(l2->key), &sNULL, &sNULL) != SUCCESS)
        return ERR_INTERN;

    // navestie podmienka neplati
    if(AddInstr(iList, I_LAB, &sNULL, &sNULL, &sNULL) != SUCCESS)
        return ERR_INTERN;

    /* vytiahneme si poslednu instrukciu */
    if ((pIns2 = listGetPointerLast(iList)) == NULL)
        return ERR_INTERN;

    // nastavime JUMP instrukcie na spravne navestia
    *((tListItem **)(l1->entry.data)) = pIns2;
    *((tListItem **)(l2->entry.data)) = pIns1;

    if((token=getToken(&attr)) == T_ERR_LEX)
        return ERR_LEX;

    return SUCCESS;
}


static ret_t parsIf(htab_t * tGlob, htab_t * tLoc, tListOfInstr * iList)
{
    
                // zariadi:
                // 
                // PSA - zavola psa
                // JUMP_ELSE - vlozi jump else instrukciu
                // BLOCK/COMMAND - spusti rutinu pre spracovanie 1 prikazu alebo
                //         bloku (begin ... end) za podmienky TRUE
                // JUMP - instrukcia skoku za blok  FALSE
                // BLOCK/COMMAND - spusti rutinu pre spracovanie 1 prikazu alebo
                //         bloku (begin ... end) za podmienky FALSE
                // LAB - vlozi instrukciu navestia aby bolo kam skakat
    htab_listitem * l1 = NULL;
    htab_listitem *l2 = NULL;
    tListItem * pIns1 = NULL;
    tListItem * pIns2 = NULL;
    ret_t  ret;


    if ((token=getToken(&attr)) == T_ERR_LEX)
        return ERR_LEX;

    /* zavolame si PSA na vyhodnotenie podmienky */
    if ((ret =PSA(tGlob, tLoc, iList, BOOL)) != SUCCESS)
        return ret;

    if(token != T_THEN)
        return ERR_SYNT;
    
    if(htab_newLabel(tLoc, &pIns1, NULL, &l1) != HTAB_SUCCESS)
        return ERR_INTERN;

    if(htab_newLabel(tLoc, &pIns2, NULL, &l2) != HTAB_SUCCESS)
        return ERR_INTERN;
    
    if(AddInstr(iList, I_JUMP_ELSE, &(l1->key), &sNULL, &sNULL) != SUCCESS)
        return ERR_INTERN;

    if((token=getToken(&attr)) == T_ERR_LEX)
        return ERR_LEX;

    // Podmienka plati
    if((ret = block(tGlob, tLoc, iList)) != SUCCESS)
        return ret;

    /// ->>>> JUMP_ELSE
    // instrukcia skocit za false
    if(AddInstr(iList, I_JUMP, &(l2->key), &sNULL, &sNULL) != SUCCESS)
        return ERR_INTERN;

    // Navestie na false
    if(AddInstr(iList, I_LAB, &sNULL, &sNULL, &sNULL) != SUCCESS)
        return ERR_INTERN;

    /* Vytiahneme si poslednu instrukciu */
    if ((pIns1 = listGetPointerLast(iList)) == NULL)
        return ERR_INTERN;

    if((token=getToken(&attr)) == T_ERR_LEX)
        return ERR_LEX;

    // Za blokom true musi nasledovat else
    if(token != T_ELSE) return ERR_SYNT;

    if((token=getToken(&attr)) == T_ERR_LEX)
        return ERR_LEX;
    
    // podmienka neplati
    if((ret = block(tGlob, tLoc, iList)) != SUCCESS)
        return ret;
    
    // navestie za podmienkou
    if(AddInstr(iList, I_LAB, &sNULL, &sNULL, &sNULL) != SUCCESS)
        return ERR_INTERN;

    /* vytiahneme si poslednu instrukciu */
    if ((pIns2 = listGetPointerLast(iList)) == NULL)
        return ERR_INTERN;

    *((tListItem **)(l1->entry.data)) = pIns1;
    *((tListItem **)(l2->entry.data)) = pIns2;

    if((token=getToken(&attr)) == T_ERR_LEX)
        return ERR_LEX;

    return SUCCESS;
}


static inline bool isLiteral(token_t token)
{
    switch(token)
    {
        case T_TRUE:
        case T_FALSE:
        case T_INTEGER_NUM:
        case T_DOUBLE_NUM:
        case T_STR_LIT:
            return true;

        default:
            return false;
    }
}


static ret_t parsIdent(htab_t * tGlob, htab_t * tLoc, tListOfInstr * iList)
{
    htab_listitem * hItemL = NULL;;
    tListItem * pInsLast = NULL;
    ret_t ret;

    /* skusime najst to kam priradujeme v lokalnej premennej */
    if ((hItemL = htab_get(tLoc, &attr)) == NULL)
    {
        /* Pripadne v globalnej */
        if ((hItemL = htab_get(tGlob, &attr)) == NULL)
        {
            /* ak sme symbol nenasli an v globalnej tabulke */
            return ERR_SEM_UNDEF;
        }
    }
    
    /* priradit do funkcie je tiez sprostost */
    if (hItemL->entry.type == FUNC)
        return ERR_SEM_TYPE;

    /* musi nasledovat operator priradenia */
    GET_TOKEN;
    RET_TOKEN_NEQAL(T_ASSIGN, ERR_SYNT);

    GET_TOKEN;

    /* zavolame si na vyraz PSA */
    if ((ret = PSA(tGlob, tLoc, iList, hItemL->entry.type)) != SUCCESS)
        return ret;

    /* vytiahneme si poslednu instrukciu */
    if ((pInsLast = listGetPointerLast(iList)) == NULL)
        return ERR_INTERN;

    /* Pridame samotne priradenie */
    if(AddInstr(iList, I_ASSIGN, &(hItemL->key), &sNULL,
                &(pInsLast->Instruction.addr3)) != SUCCESS)
        return ERR_INTERN;


    // Prikaz musi koncit ; alebo end
    if(token == T_END || token == T_SEMICOLON)
        return SUCCESS;

    return ERR_SYNT;

}


ret_t parsWrite(htab_t * tGlob, htab_t * tLoc, tListOfInstr * iList)
{
    htab_listitem * hItem = NULL;
    stateWr_t  state = S_WR_START;
    
    while(true)
    {
        if((token=getToken(&attr)) == (token_t)T_ERR_LEX)
            return ERR_LEX;
        switch(state)
        {
            case S_WR_START:
                if (token != T_L_BRACKET)
                    return ERR_SYNT;
                state = S_WR_LB;
                break;
            
            case S_WR_LB:
                if (token == T_STR_LIT
                    || token == T_INTEGER_NUM
                    || token == T_DOUBLE_NUM
                    || token == T_FALSE
                    || token == T_TRUE)
                {
                    if (htab_newLiteral(tLoc, STRING, &attr, &hItem)
                            != HTAB_SUCCESS)
                        return ERR_INTERN;

                    if (AddInstr(iList, I_WRITE, &hItem->key, &sNULL, &sNULL)
                            != SUCCESS)
                        return ERR_INTERN;

                }
                else if (token == T_IDENTIFIER)
                {
                    // Je v lokalnej premennej?
                    if ((hItem = htab_get(tLoc, &attr)) != NULL)
                    {
                        if (AddInstr(iList, I_WRITE, &hItem->key, &sNULL, &sNULL)
                                != SUCCESS)
                            return ERR_INTERN;
                    }

                    // Je v globalnej premennej?
                    else if ((hItem = htab_get(tGlob, &attr)) != NULL)
                    {
                        if (AddInstr(iList, I_WRITE, &hItem->key, &sNULL, &sNULL)
                                != SUCCESS)
                            return ERR_INTERN;
                    }

                    // Neni nikde
                    else
                        return ERR_SEM_UNDEF;
                }
                else 
                    return ERR_SYNT;
                state = S_WR_AFTWR;
                break;

            case S_WR_AFTWR:
                if (token == T_COMMA)
                    state = S_WR_LB;
                else if (token == T_R_BRACKET)
                    state = S_WR_RB;
                else
                    return ERR_SYNT;
                break;

            case S_WR_RB:
                if (token == T_SEMICOLON || token == T_END)
                    return SUCCESS;
                return ERR_SYNT;
        }
    }
    return ERR_SYNT;
}


static ret_t block(htab_t * tGlob, htab_t * tLoc, tListOfInstr * iList)
{
    ret_t result;

    if(token != T_BEGIN)
        return ERR_SYNT;


    while((result = command(tGlob, tLoc, iList)) == SUCCESS)
    {
        if (token == T_END)
        {
            return SUCCESS;
        }
        else if (token != T_SEMICOLON)
            return ERR_SYNT;
    }
    return result;
}

static ret_t parsReadln(htab_t * tGlob, htab_t * tLoc, tListOfInstr * iList)
{
    htab_listitem * hItem = NULL;

    if ((token=getToken(&attr)) == T_ERR_LEX)
        return ERR_LEX;

    /* za readln musi byt zatvorka */
    if (token != T_L_BRACKET)
        return ERR_SYNT;

    if ((token=getToken(&attr)) == T_ERR_LEX)
        return ERR_LEX;

    /* v zatvorke musi byt identifikator */
    if(token != T_IDENTIFIER)
        return ERR_SYNT;

    if((hItem = htab_get(tLoc, &attr)) == NULL)
    {
        /* symbol sa nepodarilo najst */
        if((hItem = htab_get(tGlob, &attr)) == NULL)
            return ERR_SEM_UNDEF;
    }

    /* symbol neni int real alebo string -> chyba */
    if(hItem->entry.type != INT
            &&  hItem->entry.type != REAL
            && hItem->entry.type != STRING)
        return ERR_SEM_TYPE;

    /* pridanie instrukcie sa nemusi podarit */
    if (AddInstr(iList, I_READ, &attr, &sNULL, &sNULL) != SUCCESS)
        return ERR_INTERN;

    if ((token=getToken(&attr)) == T_ERR_LEX)
        return ERR_LEX;

    /* musi to byt ozatvorkovane */
    if (token != T_R_BRACKET)
        return ERR_SYNT;

    if ((token=getToken(&attr)) == T_ERR_LEX)
        return ERR_LEX;

    /* mjusi to koncit end alebo ; */
    if (token == T_SEMICOLON || token == T_END)
        return SUCCESS;

    return ERR_SYNT;
}



static ret_t command(htab_t * tGlob, htab_t * tLoc, tListOfInstr * iList)
{
    int result = SUCCESS;
    token_t tmpTok = token;

    if ((token=getToken(&attr)) == T_ERR_LEX)
        return ERR_LEX;

    if (tmpTok == T_SEMICOLON && token == T_END)
        return ERR_SYNT;

        switch(token)
        {
            case T_BEGIN:
                if ((result = block(tGlob, tLoc, iList)) != SUCCESS)
                    return result;

                if(token == T_END)
                {
                    if ((token=getToken(&attr)) == T_ERR_LEX)
                        return ERR_LEX;
                }
                return SUCCESS;
                break;

            case T_END:
                return SUCCESS;

            case T_IDENTIFIER:
                return parsIdent(tGlob, tLoc, iList);


            case T_WHILE:
                return parsWhile(tGlob, tLoc, iList);

            case T_IF:
                return parsIf(tGlob, tLoc, iList);

            case T_WRITE:
                return parsWrite(tGlob, tLoc, iList);

            /*case T_FIND:
            case T_SORT: */
            case T_READLN:
                return parsReadln(tGlob, tLoc, iList);
            default:
                return ERR_SYNT;
        }
        return result;

}

#define RETURN(id, val) {strFree(id); return (val);}


static ret_t vardef(htab_t *fSymbolTable)
{

    string id;
    if(strInit(&id) != STR_SUCCESS)
        return ERR_INTERN;
    hExit_t hRet;
    stateFnc_t state = S_FNC_VAR;
    type_t type;

    while(true)
    {
        if ((token=getToken(&attr)) == T_ERR_LEX)
            RETURN(&id, ERR_LEX);

        /* musime ocheckovat ci neprislo copy || length, lebo tento ultra
         * super jazyk ich nema medzi klucovymi slovami */
        if (token == T_COPY || token == T_LENGTH)
            RETURN(&id, ERR_SEM_UNDEF);

        switch(state)
        {
            case S_FNC_VAR:
                if(token != T_IDENTIFIER)
                    RETURN(&id, ERR_SYNT)
                if(strCopyString(&id, &attr) != STR_SUCCESS)
                    RETURN(&id, ERR_INTERN)
                state = S_FNC_VAR_ID;
                break;
            
            case S_FNC_VAR_ID:
                if (token != T_COLON)
                    RETURN(&id, ERR_SYNT)
                state = S_FNC_VAR_COL;
                break;

            case S_FNC_VAR_COL:
                if((type = typeDecode(token)) == NOTYPE) // ma parameter validny typ?
                    RETURN(&id, ERR_SYNT)
                if((hRet = htab_new(fSymbolTable, &id, type, 0, 1,
                            false, NULL, NULL)) != HTAB_SUCCESS) // pridame do tab. premennu
                {
                    if(hRet == HTAB_SYM_EXISTS)
                        RETURN(&id, ERR_SEM_UNDEF)
                    else
                        RETURN(&id, ERR_INTERN)
                }
                state = S_FNC_VAR_TYPE;
                break;
            
            case S_FNC_VAR_TYPE:
                if(token != T_SEMICOLON)
                    RETURN(&id, ERR_SYNT)
                state = S_FNC_VAR_SCOL;
                break;
            
            case S_FNC_VAR_SCOL:
                if(token == T_IDENTIFIER)
                {
                    if(strCopyString(&id, &attr) != STR_SUCCESS)
                        RETURN(&id, ERR_INTERN)
                    state = S_FNC_VAR_ID;
                }
                else if (token == T_BEGIN || token == T_FUNCTION)
                    RETURN(&id, SUCCESS)
                else
                    RETURN(&id, ERR_SYNT)
                break;

            default:
                RETURN(&id, ERR_SYNT)

        }
    }
    RETURN(&id, ERR_SYNT)
}

