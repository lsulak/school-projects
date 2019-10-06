/**********************************************************|
|**        Projekt do predmetov IFJ a IAL 2014/2015      **|
|**Subor : psa_new.c                               	 	 **|
|**Popis : Obsahuje funkcie na vycislovanie vyrazov	 	 **|
|**Autori:                                               **|
|**                                                      **|
|**       Matej Muzila   xmuzil00                        **|
|**********************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "psa_new.h"
#include "scanner.h"
#include "parser.h"
#include "ilist.h"
#include "debug.h"
#include "str.h"

//                      *  /  +  -  <  >  <= >= =  <> ( 
const int priority[] = {3, 3, 2, 2, 1, 1, 1, 1, 1, 1, 0};

#if defined(PSADEBUG) || defined(DEBUG)
const char *OPER_STR[] =
{
	"O_MUL,",
	"O_DIV,",
	"O_ADD,",
	"O_SUB,",
	"O_LESS,",
	"O_MORE,",
	"O_LEQ,",
	"O_MEQ,",
	"O_EQ,",
	"O_NEQ,",
	"O_LBR,",
};
#endif

typedef enum
{
	O_MUL,
	O_DIV,
	O_ADD,
	O_SUB,
	O_LESS,
	O_MORE,
	O_LEQ,
	O_MEQ,
	O_EQ,
	O_NEQ,
	O_LBR,
	O_NOOP, 
}psaOperator_t;

/* Typ zaznamu, pouzite v liste a zasobniku  */
typedef enum 
{
	KEY,
	OPERATOR,
}psaOpType_t;

typedef enum 
{
	P_KEY,
	P_OPERATOR,
	P_LBRACKET,
	P_RBRACKET,
}psaPfxOp_t;

/* polozka zoznamu alebo zasobnika */
typedef struct psaOpItem
{
	psaOperator_t oper;
	string key;
	psaOpType_t type;
	struct psaOpItem * next;
}psaOpItem_t;

typedef struct
{
	psaOpItem_t * top;
}psaStack_t;



typedef struct
{
	psaOpItem_t * first;
	psaOpItem_t * last;
}psaList_t;

/* Deklaracie */

static ret_t psaPfxToInstr(htab_t * tGlob, htab_t * tLoc, tListOfInstr * iList,
		psaStack_t *  pStack, psaList_t * pQueue, type_t wantsType);
static ret_t psaQuAddKey(psaList_t * pList, const string * key);
static ret_t psaQuAddSign(psaList_t * pList, psaOperator_t operator);
static void psaQuInit(psaList_t * pList);
static ret_t psaQuPopKey(psaList_t * pList, string * key);
static psaOperator_t psaQuPopSign(psaList_t * pList);
static void psaStackInit(psaStack_t * pStack);
static ret_t psaStackPopKey(psaStack_t * pStack, string * key);
static psaOperator_t psaStackPopSign(psaStack_t * pStack);
static ret_t psaStackPushKey(psaStack_t * pStack, string * key);
static ret_t psaStackPushSign(psaStack_t * pStack, psaOperator_t operator);
static psaOperator_t psaStackTopSign(psaStack_t * pStack);
static psaOperator_t psaTokenToOperator(token_t tok);
static ret_t psaPfxToInstr(htab_t * tGlob, htab_t * tLoc, tListOfInstr * iList,
		psaStack_t *  pStack, psaList_t * pQueue, type_t wantsType);
static void psaStackClear(psaStack_t * pStack);
static bool psaTypeComp(type_t type1, type_t type2, type_t * pType3,
		psaOperator_t operator);

#ifdef DEBUG
static psaOperator_t psaQuTopSign(psaList_t * pQueue);
static string * psaQuTopKey(psaList_t * pQueue);
static ret_t psaQuPrint(psaList_t * pQueue);
#endif /* DEBUG */



/* Definicie */


/* Inicializacia Zasobnika */
static void psaStackInit(psaStack_t * pStack)
{
	pStack->top = NULL;
}


/* Prida key na vrchol zasobnika */
static ret_t psaStackPushKey(psaStack_t * pStack, string * key)
{
	psaOpItem_t  * tmp = NULL;

	if (pStack->top == NULL)
	{
		if ((pStack->top = malloc(sizeof(psaOpItem_t))) == NULL)
			return ERR_INTERN;
		pStack->top->next = NULL;
	}
	else
	{
		tmp = pStack->top;

		if ((pStack->top = malloc(sizeof(psaOpItem_t))) == NULL)
			return ERR_INTERN;

		pStack->top->next = tmp;
	}

	/* inicializujeme polozku key */
	if(strInit(&pStack->top->key) != STR_SUCCESS)
		return ERR_INTERN;

	/* skopirujeme kluc */
	if(strCopyString(&pStack->top->key, key) != STR_SUCCESS)
		return ERR_INTERN;

	pStack->top->type = KEY;

	return SUCCESS;
}

/* Prida operand na vrchol zasobnika */
static ret_t psaStackPushSign(psaStack_t * pStack, psaOperator_t operator)
{
	psaOpItem_t * tmp = NULL;

	if (pStack->top == NULL)
	{
		if ((pStack->top = malloc(sizeof(psaOpItem_t))) == NULL)
			return ERR_INTERN;
		pStack->top->next = NULL;
	}
	else
	{
		tmp = pStack->top;

		if ((pStack->top = malloc(sizeof(psaOpItem_t))) == NULL)
			return ERR_INTERN;

		pStack->top->next = tmp;
	}

	pStack->top->oper = operator;
	pStack->top->type = OPERATOR;

	return SUCCESS;
}


/* Zisti co je na vrchole zasobnika */
inline static psaOpType_t psaStackFirstType(psaStack_t * pStack)
{
	return pStack->top->type;
}


static psaOperator_t psaStackTopSign(psaStack_t * pStack)
{
	if (pStack == NULL || pStack->top == NULL)
		return O_NOOP;

	return pStack->top->oper;
}

/* Vytiahne z vrchola zasobnika operator a zo zasobnika ho vymaze
 * musi tam byt operator */
static psaOperator_t psaStackPopSign(psaStack_t * pStack)
{
	psaOperator_t operator;

	if (pStack == NULL || pStack->top == NULL)
		return O_NOOP;

	psaOpItem_t * tmp = pStack->top->next;
	operator = pStack->top->oper;
	free(pStack->top);
	pStack->top = tmp;

	return operator;
}

/* Vytiahne zo zasobnika operand, a zo zasobnika ho odstrani. 
 * musi tam byt operand, iank pruser */
static ret_t psaStackPopKey(psaStack_t * pStack, string * key)
{
	if (pStack == NULL)
		return ERR_INTERN;

	psaOpItem_t * tmp = pStack->top->next;

	if (strCopyString(key, &(pStack->top->key)) != STR_SUCCESS)
		return ERR_INTERN;

	strFree(&(pStack->top->key));
	free(pStack->top);
	pStack->top = tmp;

	return SUCCESS;
}

/* Inicializacia fronty */
static void psaQuInit(psaList_t * pList)
{
	pList->first = NULL;
	pList->last = NULL;
}

/* Prida do fronty operator */
static ret_t psaQuAddSign(psaList_t * pList, psaOperator_t operator)
{
	if (pList->last == NULL)
	{
		if ((pList->last = malloc(sizeof(psaOpItem_t))) == NULL)
			return ERR_INTERN;
		pList->first = pList->last;
	}
	else
	{
		if((pList->last->next = malloc(sizeof(psaOpItem_t))) == NULL)
			return ERR_INTERN;
		pList->last = pList->last->next;
	}
	
	pList->last->oper = operator;
	pList->last->next = NULL;
	pList->last->type = OPERATOR;
	return SUCCESS;
}

/* Prida do fronty Kluc */
static ret_t psaQuAddKey(psaList_t * pList, const string * key)
{
	if (pList->last == NULL)
	{
		if ((pList->last = malloc(sizeof(psaOpItem_t))) == NULL)
			return ERR_INTERN;
		pList->first = pList->last;
	}
	else
	{
		if((pList->last->next = malloc(sizeof(psaOpItem_t))) == NULL)
			return ERR_INTERN;
		pList->last = pList->last->next;
	}

	/* Mali by sme inicializovat kluc */
	if (strInit(&(pList->last->key)) != STR_SUCCESS)
		return ERR_INTERN;

	/* A skopirujeme do neho hodnotu */
	if (strCopyString(&(pList->last->key), key) != STR_SUCCESS)
		return ERR_INTERN;

	pList->last->next = NULL;
	pList->last->type = KEY;
	return SUCCESS;
}

/* Zisti ci je na zaciatku fronty operator alebo operand */
inline static psaOpType_t psaQuFirstType(psaList_t * pList)
{
	return pList->first->type;
}

static psaOperator_t psaQuPopSign(psaList_t * pList)
{
	psaOperator_t operator;

	if (pList->first == NULL)
		return O_NOOP;

	psaOpItem_t * tmp = pList->first->next;

	operator = pList->first->oper;

	free(pList->first);
	pList->first = tmp;

	if (tmp == NULL)
		pList->last = tmp;

	return operator;
}

/* Vytiahne z fronty prvy zaznam, musi to byt kluc, vymaze ho */
static ret_t psaQuPopKey(psaList_t * pList, string * key)
{
	if (pList->first == NULL)
		return ERR_INTERN;

	psaOpItem_t * tmp = pList->first->next;

	if (strCopyString(key, &(pList->first->key)) != STR_SUCCESS)
		return ERR_INTERN;

	strFree(&(pList->first->key));
	free(pList->first);
	pList->first = tmp;

	if (tmp == NULL)
		pList->last = tmp;

	return SUCCESS;
}

static psaOperator_t psaTokenToOperator(token_t tok)
{
	switch(tok)
	{
			case T_PLUS:
				return O_ADD;

			case T_MINUS:
				return O_SUB;

			case T_MULTIPLICATION:
				return O_MUL;

			case T_DIVISION:
				return O_DIV;

			case T_MORE:
				return O_MORE;

			case T_MORE_EQUAL:
				return O_MEQ;

			case T_LESS:
				return O_LESS;

			case T_LESS_EQUAL:
				return O_LEQ;

			case T_EQUAL: /* T_EQUAL */
				return O_EQ;

            case T_NOT_EQUAL:
                return O_NEQ;

			default:
				return O_LBR;
	}
}

static ret_t psaCallFunc(htab_t * tGlob, htab_t * tLoc, tListOfInstr * iList,
        htab_listitem * hItem)
{
	paramEnt * pEnt = ((func_t *)(hItem->entry.data))->nextParam;
    tListItem * pIns = NULL;
    ret_t ret;
    
    /* ak nemame ziadne parametre tak zozerieme lavu zatvotku */
    if(pEnt == NULL)
    {
        GET_TOKEN;
        
        switch(token)
        {
            case T_IDENTIFIER:
            case T_STR_LIT:
            case T_INTEGER_NUM:
            case T_DOUBLE_NUM:
            case T_FALSE:
            case T_TRUE:
                return ERR_SEM_TYPE;

            case T_R_BRACKET:
                break;

            default:
                return ERR_SYNT;
        }
    }

    /* na kazdy parameter si zavolame psa */
    while (pEnt != NULL && token != T_R_BRACKET)
    {
        GET_TOKEN;
        
        /* ak je funkcia pustene bez parametrov a ma mat parametre */
        if(token == T_R_BRACKET)
            return ERR_SEM_TYPE;

        if ((ret = PSA(tGlob, tLoc, iList, pEnt->paramType)) != SUCCESS)
            return ret;

        /* ak sa nam nepodari poslednu instrukciu vytiahnut */
        if ((pIns = listGetPointerLast(iList)) == NULL)
            return ERR_INTERN;

        /* skusime pushnut */
        if (AddInstr(iList, I_PUSH, &(pIns->Instruction.addr3), &sNULL, &sNULL))
            return ERR_INTERN;

        /* ak je za parametrom ciarka, skusime nacitat dalsi */
        if (token == T_COMMA || token == T_R_BRACKET)
            pEnt = pEnt->nextParam;

        /* aj pride ; musi sa jednat o posledny parameter */
        else 
            return ERR_SYNT;
    }

    /* parametrov bolo zadanych malo */
    if (pEnt != NULL)
        return ERR_SEM_TYPE;

    /* parametrov bolo zadane vela */
    if (token == T_COMMA)
        return ERR_SEM_TYPE;

    /* ak sa vyskytka nejaka synt chyba */
    if (token != T_R_BRACKET)
        return ERR_SYNT;

    /* skusime zavolat funkciu */
    if((ret = AddInstr(iList, I_CALL, &(hItem->key) , &sNULL, &sNULL)) != SUCCESS)
        return ERR_INTERN;

    /* pre spravnost sa zbavime zatvaracej zatvorky */
    GET_TOKEN;
    
    return SUCCESS;
}

inline static ret_t getRetKey(type_t type, string * key)
{
    int strRet = STR_ERROR;
    
    strClear(key);

    switch(type)
    {
        case BOOL:
            strRet = strAddConstString(key, RET_BOOL);
            break;

        case INT:
            strRet = strAddConstString(key, RET_INT);
            break;

        case REAL:
            strRet = strAddConstString(key, RET_REAL);
            break;

        case STRING:
            strRet = strAddConstString(key, RET_STR);
            break;

        default:
            return ERR_SEM_TYPE;
    }

    if (strRet != STR_SUCCESS)
        return ERR_INTERN;

    return SUCCESS;
}

static ret_t psaFuncLength(htab_t * tGlob, htab_t * tLoc,
        tListOfInstr * iList, string ** ppStr)
{
    ret_t ret;
    tListItem * pIns = NULL;
    htab_listitem * hItem = NULL;


    GET_TOKEN;
    RET_TOKEN_NEQAL(T_L_BRACKET, ERR_SYNT);
    GET_TOKEN;

    /* musime nejaky parameter priat */
    if (token == T_R_BRACKET)
        return ERR_SEM_TYPE;

    if ((ret = PSA(tGlob, tLoc, iList, STRING)) != SUCCESS)
            return ret;

    /* musime koncit zatvorkou */
    RET_TOKEN_NEQAL(T_R_BRACKET, ERR_SEM_TYPE);

    /* vytiahneme si posôednu instrukciu */
    if ((pIns = listGetPointerLast(iList)) == NULL)
        return ERR_INTERN;

    /* skusime si vytvorit pomocnu premennu */
    if (htab_newTmp(tLoc, INT, &hItem) != HTAB_SUCCESS)
        return ERR_INTERN;

    /* zavolame si instrukciu sort */
    if (AddInstr(iList, I_LENGTH, &(pIns->Instruction.addr3), 
            &sNULL, &(hItem->key)) != SUCCESS)
        return ERR_INTERN;

    *ppStr = &(hItem->key);

    return SUCCESS;
}

static ret_t psaFuncSort(htab_t * tGlob, htab_t * tLoc,
        tListOfInstr * iList, string ** ppStr)
{
    ret_t ret;
    tListItem * pIns = NULL;
    htab_listitem * hItem = NULL;


    GET_TOKEN;
    RET_TOKEN_NEQAL(T_L_BRACKET, ERR_SYNT);
    GET_TOKEN;

    /* musime nejaky parameter priat */
    if (token == T_R_BRACKET)
        return ERR_SEM_TYPE;

    if ((ret = PSA(tGlob, tLoc, iList, STRING)) != SUCCESS)
            return ret;

    /* musime koncit zatvorkou */
    RET_TOKEN_NEQAL(T_R_BRACKET, ERR_SEM_TYPE);

    /* vytiahneme si posôednu instrukciu */
    if ((pIns = listGetPointerLast(iList)) == NULL)
        return ERR_INTERN;

    /* skusime si vytvorit pomocnu premennu */
    if (htab_newTmp(tLoc, STRING, &hItem) != HTAB_SUCCESS)
        return ERR_INTERN;

    /* zavolame si instrukciu sort */
    if (AddInstr(iList, I_SORT, &(pIns->Instruction.addr3), 
            &sNULL, &(hItem->key)) != SUCCESS)
        return ERR_INTERN;

    *ppStr = &(hItem->key);

    return SUCCESS;
}

static ret_t psaFuncFind(htab_t * tGlob, htab_t * tLoc,
        tListOfInstr * iList, string ** ppStr)
{
    ret_t ret;
    tListItem * pIns1 = NULL;
    tListItem * pIns2 = NULL;
    htab_listitem * hItem = NULL;


    GET_TOKEN;
    RET_TOKEN_NEQAL(T_L_BRACKET, ERR_SYNT);
    GET_TOKEN;

    /* musime nejaky parameter priat */
    if (token == T_R_BRACKET)
        return ERR_SEM_TYPE;

    /* na prvy parameter si zavolame psa -*/
    if ((ret = PSA(tGlob, tLoc, iList, STRING)) != SUCCESS)
            return ret;

    /* musime koncit ciarkou */
    RET_TOKEN_NEQAL(T_COMMA, ERR_SEM_TYPE);
    GET_TOKEN;

    /* vytiahneme si poslednu instrukciu */
    if ((pIns1 = listGetPointerLast(iList)) == NULL)
        return ERR_INTERN;


    /* na druhy pdarameter si zavolame psa */
    if ((ret = PSA(tGlob, tLoc, iList, STRING)) != SUCCESS)
            return ret;

    RET_TOKEN_NEQAL(T_R_BRACKET, ERR_SEM_TYPE);

    /* vytiahneme si poslednu instrukciu */
    if ((pIns2 = listGetPointerLast(iList)) == NULL)
        return ERR_INTERN;


    /* skusime si vytvorit pomocnu kam budeme vracat */
    if (htab_newTmp(tLoc, INT, &hItem) != HTAB_SUCCESS)
        return ERR_INTERN;

    /* zavolame si instrukciu sort */
    if (AddInstr(iList, I_FIND, &(pIns1->Instruction.addr3), 
            &(pIns2->Instruction.addr3), &(hItem->key)) != SUCCESS)
        return ERR_INTERN;

    *ppStr = &(hItem->key);

    return SUCCESS;
}

static ret_t psaFuncCopy(htab_t * tGlob, htab_t * tLoc,
        tListOfInstr * iList)
{
    ret_t ret;
    tListItem * pIns1 = NULL;
    tListItem * pIns2 = NULL;
    tListItem * pIns3 = NULL;


    GET_TOKEN;
    RET_TOKEN_NEQAL(T_L_BRACKET, ERR_SYNT);
    GET_TOKEN;

    /* musime nejaky parameter priat */
    if (token == T_R_BRACKET)
        return ERR_SEM_TYPE;

    /* na prvy parameter si zavolame psa -*/
    if ((ret = PSA(tGlob, tLoc, iList, STRING)) != SUCCESS)
            return ret;

    /* vytiahneme si poslednu instrukciu */
    if ((pIns1 = listGetPointerLast(iList)) == NULL)
        return ERR_INTERN;

    /* musime koncit ciarkou */
    RET_TOKEN_NEQAL(T_COMMA, ERR_SEM_TYPE);
    GET_TOKEN;

    /* na druhy pdarameter si zavolame psa */
    if ((ret = PSA(tGlob, tLoc, iList, INT)) != SUCCESS)
            return ret;

    /* vytiahneme si poslednu instrukciu */
    if ((pIns2 = listGetPointerLast(iList)) == NULL)
        return ERR_INTERN;

    /* musime koncit ciarkou */
    RET_TOKEN_NEQAL(T_COMMA, ERR_SEM_TYPE);
    GET_TOKEN;

    /* na treti pdarameter si zavolame psa */
    if ((ret = PSA(tGlob, tLoc, iList, INT)) != SUCCESS)
            return ret;

    RET_TOKEN_NEQAL(T_R_BRACKET, ERR_SEM_TYPE);

    /* vytiahneme si poslednu instrukciu */
    if ((pIns3 = listGetPointerLast(iList)) == NULL)
        return ERR_INTERN;

    /* zavolame si instrukciu sort */
    if (AddInstr(iList, I_COPY, &(pIns1->Instruction.addr3), 
            &(pIns2->Instruction.addr3), &(pIns3->Instruction.addr3))
            != SUCCESS)
        return ERR_INTERN;

    return SUCCESS;
}
static ret_t psaGetToPfx(htab_t * tGlob, htab_t * tLoc, tListOfInstr * iList,
        psaStack_t * pStack, psaList_t * pQueue)
{

	psaPfxOp_t last = P_OPERATOR;
	psaOperator_t operator;
	int inteeger;
	double real;
	bool boolean;
    string str;
    string pomStr;
    string * pStr;
	htab_listitem * tabItem;
    ret_t ret;
    int brcCnt = 0;
    type_t funcType;
    type_t pomType;

	do
	{
		switch(token)
		{

			case T_FALSE:
			case T_TRUE:

				/* Ak posledny token nebol kluc, je to chyba */
				if (last == P_KEY || last == P_RBRACKET)
					return ERR_SYNT;
				
				/* Skonvertujeme si token  na bool */
				boolean = token == T_TRUE ? true : false;

				/* Skusime vlozit do tabulky */
				if (htab_newLiteral(tLoc, BOOL, &boolean, &tabItem)
						!= HTAB_SUCCESS)
					return ERR_INTERN;

				/* Skusime do fronty vlozit */
				if (psaQuAddKey(pQueue, &(tabItem->key)) != SUCCESS)
					return ERR_INTERN;
					
				/* Posledny bol kluc */
				last = P_KEY;
				break;
				
			case T_INTEGER_NUM:

				/* Ak posledny token nebol kluc, je to chyba */
				if (last == P_KEY || last == P_RBRACKET)
					return ERR_SYNT;
				
				/* Skonvertujeme si int literalu na int */
				inteeger = atoi(attr.str);

				/* Skusime vlozit do tabulky */
				if (htab_newLiteral(tLoc, INT, &inteeger, &tabItem)
						!= HTAB_SUCCESS)
					return ERR_INTERN;

				/* Skusime do fronty vlozit */
				if (psaQuAddKey(pQueue, &(tabItem->key)) != SUCCESS)
					return ERR_INTERN;
					
				/* Posledny bol kluc */
				last = P_KEY;
				break;

			case T_DOUBLE_NUM:

				/* Ak posledny token nebol kluc, je to chyba */
				if (last == P_KEY || last == P_RBRACKET)
					return ERR_SYNT;	
				
				/* Skonvertujeme si double literalu na double */
				real = strtod(attr.str, NULL);

				/* Skusime vlozit do tabulky */
				if (htab_newLiteral(tLoc, REAL, &real, &tabItem)
						!= HTAB_SUCCESS)
					return ERR_INTERN;

				/* Skusime do fronty vlozit */
				if (psaQuAddKey(pQueue, &(tabItem->key)) != SUCCESS)
					return ERR_INTERN;
					
				/* Posledny bol kluc */
				last = P_KEY;
				break;

			case T_STR_LIT:

				/* Ak posledny token nebol kluc, je to chyba */
				if (last == P_KEY || last == P_RBRACKET)
					return ERR_SYNT;
				
				/* Skusime vlozit do tabulky */
				if (htab_newLiteral(tLoc, STRING, &attr, &tabItem)
						!= HTAB_SUCCESS)
					return ERR_INTERN;

				/* Skusime do fronty vlozit */
				if (psaQuAddKey(pQueue, &(tabItem->key)) != SUCCESS)
					return ERR_INTERN;
					
				/* Posledny bol kluc */
				last = P_KEY;
				break;



			case T_IDENTIFIER:

				/* Ak posledny token nebol kluc, je to chyba */
				if (last == P_KEY || last == P_RBRACKET)
					return ERR_SYNT;

                if(strInit(&pomStr) != STR_SUCCESS)
                    return ERR_INTERN;

                if(strCopyString(&pomStr, &attr) != STR_SUCCESS)
                    return ERR_INTERN;

                if ((token = getToken(&attr)) == T_ERR_LEX)
                {
                    strFree(&pomStr);
                    return ERR_LEX;
                }

                /* narazili sme na zatvorku a teda to musi byt funkcia */
                if(token == T_L_BRACKET)
                {
                    /* skusime si z tabulky vytiahnut funkciu */
                    if ((tabItem = htab_get(tGlob, &pomStr)) == NULL)
                    {
                        strFree(&pomStr);
                        return ERR_SEM_UNDEF;
                    }

                    /* zistime ci ide fakt o funkciu */
                    if (tabItem->entry.type != FUNC)
                    {
                        strFree(&pomStr);
                        return ERR_SEM_TYPE;
                    }

                    /* zistime aky typ ma funkcia vracat */
                    funcType = ((func_t *)(tabItem->entry.data))->rettype;

                    /* zavolame si funkciu */
                    if ((ret = psaCallFunc(tGlob, tLoc, iList, tabItem)) != SUCCESS)
                    {
                        strFree(&pomStr);
                        return ret;
                    }
                     
                    if (strInit(&str) != STR_SUCCESS)
                    {
                        strFree(&pomStr);
                        return ERR_INTERN;
                    }

                    /* zistime kde hladat vysledok funkcie */
                    if (getRetKey(funcType, &str) != SUCCESS)
                    {
                        strFree(&str);
                        strFree(&pomStr);
                        return ERR_INTERN;
                    }

                    /* vytvorime si premennu v lokalnej tabulke */
                    if (htab_newTmp(tLoc, funcType, &tabItem) != HTAB_SUCCESS)
                    {
                        strFree(&str);
                        strFree(&pomStr);
                        return ERR_INTERN;
                    }

                    if (AddInstr(iList, I_ASSIGN, &(tabItem->key),
                                &sNULL, &str) != SUCCESS)
                    {
                        strFree(&str);
                        strFree(&pomStr);
                        return ERR_INTERN;
                    }
                    strFree(&str);

                    /* a pridame do fronty */
                    if (psaQuAddKey(pQueue, &(tabItem->key)) != SUCCESS)
                    {
                        strFree(&pomStr);
                        return ERR_INTERN;
                    }
                }

                else
                {
                    pomType = NOTYPE;
                    /* skusime si premennu z lokalnej tabulky vytiahnut */
                    if ((tabItem = htab_get(tLoc, &pomStr)) == NULL
                            || (pomType =  tabItem->entry.type) == FUNC)
                    {
                        if ((tabItem = htab_get(tGlob, &pomStr)) == NULL)
                        {
                            strFree(&pomStr);

                            if(pomType == FUNC)
                                return ERR_SEM_TYPE;
                            return ERR_SEM_UNDEF;
                        }
                    }

                    /* zistime ci mame ozaj premennu */
                    if (psaQuAddKey(pQueue, &pomStr) != SUCCESS)
                    {
                        strFree(&pomStr);
                        return ERR_INTERN;
                    }
                }
                strFree(&pomStr);

				last = P_KEY;
				continue;

            case T_COPY:

				/* Ak posledny token nebol kluc, je to chyba */
				if (last == P_KEY || last == P_RBRACKET)
					return ERR_SYNT;

                if ((ret = psaFuncCopy(tGlob, tLoc, iList)) != SUCCESS)
                    return ret;


                /* vytvorime si premennu v lokalnej tabulke */
                if (htab_newTmp(tLoc, STRING, &tabItem) != HTAB_SUCCESS)
                    return ERR_INTERN;

                if (strInit(&str) != STR_SUCCESS)
                    return ERR_INTERN;

                if (strAddConstString(&str, RET_STR) != STR_SUCCESS)
                {
                    strFree(&str);
                    return ERR_INTERN;
                }

                /* vysledok copy priradime do nejakej docasnej premennej */
                if (AddInstr(iList, I_ASSIGN, &(tabItem->key), &sNULL,
                            &str) != SUCCESS)
                {
                    strFree(&str);
                    return ERR_INTERN;
                }
                strFree(&str);


                /* pridame do fronty */
                if (psaQuAddKey(pQueue, &(tabItem->key)) != SUCCESS)
                    return ERR_INTERN;
                
				last = P_KEY;
				break;

            case T_FIND:

				/* Ak posledny token nebol kluc, je to chyba */
				if (last == P_KEY || last == P_RBRACKET)
					return ERR_SYNT;

                if ((ret = psaFuncFind(tGlob, tLoc, iList, &pStr)) != SUCCESS)
                    return ret;

                /* pridame do fronty */
                if (psaQuAddKey(pQueue, pStr) != SUCCESS)
                    return ERR_INTERN;
                
				last = P_KEY;
				break;

            case T_LENGTH:

				/* Ak posledny token nebol kluc, je to chyba */
				if (last == P_KEY || last == P_RBRACKET)
					return ERR_SYNT;

                if ((ret = psaFuncLength(tGlob, tLoc, iList, &pStr)) != SUCCESS)
                    return ret;

                /* pridame do fronty */
                if (psaQuAddKey(pQueue, pStr) != SUCCESS)
                    return ERR_INTERN;
                
				last = P_KEY;
				break;

            case T_SORT:

				/* Ak posledny token nebol kluc, je to chyba */
				if (last == P_KEY || last == P_RBRACKET)
					return ERR_SYNT;

                if ((ret = psaFuncSort(tGlob, tLoc, iList, &pStr)) != SUCCESS)
                    return ret;

                /* pridame do fronty */
                if (psaQuAddKey(pQueue, pStr) != SUCCESS)
                    return ERR_INTERN;
                
				last = P_KEY;
				break;

			case T_PLUS:
			case T_MINUS:
			case T_MULTIPLICATION:
			case T_DIVISION:
			case T_MORE:
			case T_MORE_EQUAL:
			case T_LESS:
			case T_LESS_EQUAL:
			case T_EQUAL:
            case T_NOT_EQUAL:
				/* Ak bol aj predchadzajuci token operator, tak je to zle */
				if (last == P_LBRACKET || last == P_OPERATOR)
					return ERR_SYNT;

				/* nastavime, ze last bol operator */
				last = P_OPERATOR;

				/* Zachovame sa podla priority operatorov */
				while((operator = psaStackTopSign(pStack)) != O_NOOP)
				{
					/* ak ma operator na vrchole zasobnika >= prioritu, dame 
					 * ho do fronty */
					if(priority[operator] >= priority[(int)psaTokenToOperator(token)])
					{
						/* pridanie do fronty, nemusi sa podarit */
						if (psaQuAddSign(pQueue, operator) != SUCCESS)
							return ERR_INTERN;

						/* popneme ho zo zasobnika, jedno kam */
						psaStackPopSign(pStack);
					}
					else 
						break;
				}
				
				/* ak ma vyssiu prioritu ako posledny v zasobniku PUSH */
				if (psaStackPushSign(pStack, psaTokenToOperator(token)) != SUCCESS)
					return ERR_INTERN;

				/* nastavime, ze posledny token bol operator */
				break;


			/* Ak pride lava zatvorka, natvrdo ju pushneme do zasobnika symbolov */
			case T_L_BRACKET:

                /* zvysime pocitadlo zatvoriek */
                brcCnt++;
				
				if (last == P_RBRACKET || last == P_KEY)
					return ERR_SYNT;

				if (psaStackPushSign(pStack, psaTokenToOperator(token)) != SUCCESS)
					return ERR_INTERN;

				last = P_LBRACKET;
				break;

			case T_R_BRACKET:

                /* pred prvou zatvorkou moze ist len ) alebo clen */
				if (last != P_RBRACKET && last != P_KEY )
					return ERR_SYNT;


                if (--brcCnt >=  0)
                {

                    while((operator = psaStackTopSign(pStack)) != O_LBR)
                    {
                        /* ak nenarzi na ziadnu lavu zatvorku, je to zle */
                        if(operator == O_NOOP)
                            return ERR_SYNT;

                        /* prida operator do fronty, nemusi sa podarit */
                        if (psaQuAddSign(pQueue, operator) != SUCCESS)
                            return ERR_INTERN;
                        psaStackPopSign(pStack);
                    }
                    
                    /* vyhodi zo zasobnika lavu zatvorku */
                    psaStackPopSign(pStack);

                    last = P_RBRACKET;
                    break;
                }

            /* POZOR NA ZRADU, NENI TU BREAK !!!! */

            default:

				/* ked je posledny vo vyraze operator, je to zle */
				if(last == P_OPERATOR || last == P_LBRACKET)
					return ERR_SYNT;

				/* povkladame do front zbytok zasobnika */
				while((operator = psaStackTopSign(pStack)) != O_NOOP)
				{
					/* v stacku po vycisleni vyrazu zatvorky nemaju co hladat */
					if(operator == O_LBR)
						return ERR_SYNT;

					/* pridanie do fronty, nemusi sa podarit */
					if (psaQuAddSign(pQueue, operator) != SUCCESS)
						return ERR_INTERN;

					/* popneme ho zo zasobnika, jedno kam */
					psaStackPopSign(pStack);
				}
#ifdef DEBUG
				psaQuPrint(pQueue);
#endif
				return SUCCESS;
		}
		
		token = getToken(&attr);
			if (token == T_ERR_LEX)
				return ERR_LEX;

	}while(true);


	return ERR_SYNT;
}

static void psaStackClear(psaStack_t * pStack)
{
	if (pStack == NULL || pStack->top == NULL)
		return;

	psaOpItem_t * tmp = NULL;
	
	while(pStack->top != NULL)
	{

		/* prva polozka v zasobniku je kluci -> treba ho uvolnit */
		if (psaStackFirstType(pStack) == KEY)
			strFree((&pStack->top->key));

		tmp = pStack->top->next;
		free(pStack->top);
		pStack->top = tmp;
	}
}

static void psaQuClear(psaList_t * pQueue)
{
	if (pQueue == NULL || pQueue->first == NULL)
		return;

	psaOpItem_t * tmp = NULL;
	
	while(pQueue->first != NULL)
	{

		/* prva polozka v zasobniku je kluci -> treba ho uvolnit */
		if (psaQuFirstType(pQueue) == KEY)
			strFree((&pQueue->first->key));

		tmp = pQueue->first->next;
		free(pQueue->first);
		pQueue->first = tmp;
	}
	pQueue->last = NULL;
}

static bool psaTypeComp(type_t type1, type_t type2, type_t * pType3,
		psaOperator_t operator)
{
	switch(operator)
	{	
		case O_ADD:
			if (type1 == STRING && type2 == STRING)
			{
				*pType3 = STRING;
				return true;
			}

		case O_MUL:
		case O_SUB:
			if ((type1 == INT && type2 == REAL) || (type1 == REAL  && type2 == INT))
			{
				*pType3 = REAL;
				return true;
			}
			if (type1 == INT && type2 == INT)
			{
				*pType3 = INT;
				return true;
			}
			if (type1 == REAL && type2 == REAL)
			{
				*pType3 = REAL;
				return true;
			}

			return false;

        /* Delenie je bohová super ultra špecialita tohto jazyka (vracia iba real) */
		case O_DIV:
			if ((type1 == INT && type2 == REAL) || (type1 == REAL  && type2 == INT))
			{
				*pType3 = REAL;
				return true;
			}
			if (type1 == INT && type2 == INT)
			{
				*pType3 = REAL;
				return true;
			}
			if (type1 == REAL && type2 == REAL)
			{
				*pType3 = REAL;
				return true;
			}

			return false;

		case O_LESS:
		case O_MORE:
		case O_LEQ:
		case O_MEQ:
		case O_EQ:
		case O_NEQ:

			/* ak porovnavaci operator tak vzdy vracia BOOL */
			*pType3 = BOOL;
			if ((type1 == INT && type2 == REAL) || (type1 == REAL  && type2 == INT))
				return true;
			if (type1 == INT && type2 == INT)
				return true;
			if (type1 == REAL && type2 == REAL)
				return true;
			if (type1 == BOOL && type2 == BOOL)
				return true;
			if (type1 == STRING && type2 == STRING)
				return true;

			return false;

		default:
			return false;

	}
	
}

/* prevedie operator na instrukciu */
static instType_t psaOpToInst(psaOperator_t operator)
{
	switch(operator)
	{
		case O_MUL:
			return I_MULTIPLICATION;
		case O_DIV:
			return I_DIVISION;
		case O_ADD:
			return I_PLUS;
		case O_SUB:
			return I_MINUS;
		case O_LESS:
			return I_LESS;
		case O_MORE:
			return I_MORE;
		case O_LEQ:
			return I_LESS_EQUAL;
		case O_MEQ:
			return I_MORE_EQUAL;
		case O_EQ:
			return I_EQUAL;
		case O_NEQ:
			return I_NOT_EQUAL;
		default:
			return I_NOP;
	}
}

static ret_t psaPfxToInstr(htab_t * tGlob, htab_t * tLoc, tListOfInstr * iList,
		psaStack_t *  pStack, psaList_t * pQueue, type_t wantsType)
{
	ret_t ret = SUCCESS;
	type_t type1;
	type_t type2;
	type_t type3;
	entry_t * entry = NULL;
	string key1;
	string key2;
	psaOperator_t operator;
	htab_listitem * htItem = NULL;;


	if (strInit(&key1) != STR_SUCCESS)
		return ERR_INTERN;

	if (strInit(&key2) != STR_SUCCESS)
		goto EndStr2;

	/* Ak je vo fronte len jeden operand tak ho rovno priradime a koniec */
	if(pQueue->first != NULL && pQueue->first->next == NULL)
	{
		if((ret = psaQuPopKey(pQueue, &key1)) != SUCCESS)
			goto End;

        if ((entry = htab_getentry(tLoc, &key1)) == NULL)
        {
            if ((entry = htab_getentry(tGlob, &key1)) == NULL)
            {
                ret = ERR_SEM_UNDEF;
                goto End;
            }
        }

        /* musime mat rovnake typy */
        if (entry->type != wantsType)
        {
            ret = ERR_SEM_TYPE;
            goto End;
        }
		ret = AddInstr(iList, I_NOP, &sNULL, &sNULL, &key1);
		goto End;
	}


	while(pQueue->first != NULL)
	{
		/* ak je vo fronte operand */
		if (psaQuFirstType(pQueue) == KEY)
		{
			/* vytiahneme ho z fronty */
			if((ret = psaQuPopKey(pQueue, &key1)) != SUCCESS)
				goto End;
			/* vopchame ho do zasobnika */
			if ((ret = psaStackPushKey(pStack, &key1)) != SUCCESS)
				goto End;
		}

		/* ak je vo fronte operator */
		else
		{
			
			if ((operator = psaQuPopSign(pQueue)) == O_NOOP)
			{
				ret = ERR_INTERN; /* SKONTROLUJ */
				goto End;
			}

			/* vytiahneme zo zasobnika prvy operand */
			if ((ret = psaStackPopKey(pStack, &key1)) != SUCCESS)
				goto End;
			
			/* vytiahneme zo zasobnika druhy operand */
			if ((ret = psaStackPopKey(pStack, &key2)) != SUCCESS)
				goto End;

			/* skusime najst operand1 v tabulkach */
			if ((entry = htab_getentry(tLoc, &key1)) == NULL)
			{
				if ((entry = htab_getentry(tGlob, &key1)) == NULL)
					return ERR_SEM_UNDEF;
			}

			/* typ prveho operandu */
			type1 = entry->type;

			/* skusime najst operand2 v tabulkach */
			if ((entry = htab_getentry(tLoc, &key2)) == NULL)
			{
				if ((entry = htab_getentry(tGlob, &key2)) == NULL)
					return ERR_SEM_UNDEF;
			}
			
			/* typ druheho operandu */
			type2 = entry->type;


			/* zistime ci su dane typy pri konkretnej instrukcii 
			 * navzajom kompatibilne a aky typ nam vznikne*/
			if (psaTypeComp(type1, type2, &type3, operator))
			{

				/* zisiel by sa mi nejaky docasny vysledok */
				if(htab_newTmp(tLoc, type3,  &htItem) != HTAB_SUCCESS)
				{
					ret = ERR_INTERN;
					goto End;
				}

				/* Pridame instrukciu */
				if(AddInstr(iList, psaOpToInst(operator), &key2, &key1, 
							&(htItem->key)) != SUCCESS)
					goto End;

				if ((ret = psaStackPushKey(pStack, &(htItem->key))) != SUCCESS)
					goto End;
			}

			/* Ak su neni kompatibilne zahlasime ERR_SEM_TYPE */
			else
			{
				ret = ERR_SEM_TYPE;
				goto End;
			}
		}
	}

    /* porovname to co nam vzniklo s tym co cakame */
    if (type3 != wantsType)
    {
        ret = ERR_SEM_TYPE;
        goto End;
    }

	ret = psaStackPopKey(pStack, &key1);
	if (pStack->top != NULL)
		ret = ERR_SYNT;

End:
	strFree(&key2);
EndStr2:
	strFree(&key1);
	return ret;
}

ret_t PSA(htab_t * tGlob, htab_t * tLoc, tListOfInstr * iList, type_t wantsType)
{
	DEB_PRINTF("Nastupuje psa\n");
	string str;
	ret_t ret;
	psaList_t queue;
	psaStack_t stack;

	psaQuInit(&queue);
	psaStackInit(&stack);

	if(strInit(&str) != STR_SUCCESS)
        return ERR_INTERN;
	
	
	/* prevedieme vyraz do postfixu a checkneme ci sa to podarilo */
	if ((ret = psaGetToPfx(tGlob, tLoc, iList, &stack, &queue)) != SUCCESS)
		goto End;

	psaStackClear(&stack);
	DEB_PRINTF("Nastupuje psa generovanie instrukcii\n");

	/* pokusime sa dany vyraz vycislit (a previest na instrukcie) */
	ret = psaPfxToInstr(tGlob, tLoc, iList, &stack, &queue, wantsType);

End:
	/* zariadime aby sa uvolnila pamat */
	psaStackClear(&stack);
	psaQuClear(&queue);
	strFree(&str);
	
	DEB_PRINTF("psa konci: %d\n", ret);
	return ret;
}

#ifdef DEBUG

static string * psaQuTopKey(psaList_t * pQueue)
{
	if(pQueue == NULL || pQueue->first == NULL)
		return NULL;

	return &(pQueue->first->key);
}

static psaOperator_t psaQuTopSign(psaList_t * pQueue)
{
	if(pQueue == NULL || pQueue->first == NULL)
		return O_NOOP;
	
	return pQueue->first->oper;
}


/* Vytlaci celu frontu */
static ret_t psaQuPrint(psaList_t * pQueue)
{
	string * str;
	psaOperator_t operator;
	ret_t ret = SUCCESS;
	psaList_t tmpQueue = *pQueue;

	while(tmpQueue.first != NULL)
	{

		if(psaQuFirstType(&tmpQueue) == KEY)
		{
			str = psaQuTopKey(&tmpQueue);
			printf("psaQuPopKey: %s\n", str->str);

		}

		else
		{
			operator =  psaQuTopSign(&tmpQueue);
			printf("psaQuPopSign: %s\n", OPER_STR[operator]);
		}
	tmpQueue.first = tmpQueue.first->next;
	}

	return ret;
}

#endif /* DEBUG */

