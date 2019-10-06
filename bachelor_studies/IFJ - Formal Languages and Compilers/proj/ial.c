/**********************************************************|
|**        Projekt do predmetov IFJ a IAL 2014/2015      **|
|**Subor:    ial.c                                       **|
|**Popis:    Obsahuje hashovaciu tabulku, heapsort       **|
|**          a vyhladavaci algoritmus                    **|
|**Autori:                                               **|
|**                                                      **|
|**       Matej Muzila    xmuzil00                       **|
|**       Tomas Poremba   xporem00                       **|
|**********************************************************/ 


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ial.h"

const char *typesS[] = { "INT", "BOOL", "STRING", "REAL", "FUNC", "LABEL", "NOTYPE"};

static void htab_freeitem(htab_listitem  *pZaznam);
static int htab_copyfunc(func_t * pFuncNew, const func_t * pFuncIn,
        const string * funcId);
static int htab_copystring(string *pStringNew,const string * pStringIn,
        const long length);
static int htab_newref(
            htab_t *t,
            const string *key,
            const  type_t type,
            const long begin,
            const long length, 
            const bool literal, 
            const bool inited, 
            void *data,
            htab_listitem **ret);
static htab_listitem * htab_add(htab_t *t, const string *key);


//
//
// NASLEDUJE TABULKA
//
//

// Hashovacia funkcia
unsigned int hash_function(const char *str, unsigned htab_size) 
{
    unsigned int h=0;
    const unsigned char *p;
    for(p=(const unsigned char*)str; *p!='\0'; p++)
         h = 65599*h + *p;
  return h % htab_size;
}

hExit_t funcCompare(func_t * f1, func_t * f2)
{
    paramEnt * p1 = f1->nextParam;
    paramEnt * p2 = f2->nextParam;
    
    if(f1->rettype != f2->rettype)
        return HTAB_SYM_EXISTS;
    
    for(; p1 != NULL && p2 != NULL;)
    {
    if(p1->paramType != p2->paramType)
        return HTAB_SYM_EXISTS;
    if(strcmp(p1->paramKey.str, p2->paramKey.str) != 0)
        return HTAB_SYM_EXISTS;
    p1 = p1->nextParam;
    p2 = p2->nextParam;
    }

    if(p1 != p2)
        return HTAB_SYM_EXISTS;

    return HTAB_SUCCESS;

}

static void htab_freeitem(htab_listitem  *pZaznam)
{
    if(pZaznam->entry.type == STRING)
    {
        string * pString = (string * ) pZaznam->entry.data;
        for(long j=0; j<pZaznam->entry.length; j++)
            strFree(&pString[j]);
    }

    else if(pZaznam->entry.type == FUNC)
    {
        funcFree(pZaznam->entry.data);
        pZaznam->entry.data = NULL;
    }

    strFree(&(pZaznam->key));
    free(pZaznam->entry.data);
    free(pZaznam);
}

// Zmaže všetko čo je v tabulke ale ponecha tabulku
void htab_clear(htab_t *t)
{
    if(t == NULL)
        return;
    for(unsigned i  = 0; i < t->size; i++)
    {    
        for(htab_listitem *pZaznam = t->htab_ind[i]; pZaznam != NULL;)
        {
            htab_listitem * pZaznamDal = pZaznam->next;
            htab_freeitem(pZaznam);
            pZaznam = pZaznamDal;
        }

        t->htab_ind[i] = NULL;
    }
}

/* zisti ci je funkcia inicializovana alebo nie */
int funcIsInited(string str, entry_t * entry)
{
    UNUSED(str);
    if(entry->type == FUNC && entry->inited == false) 
        return 1;
    else 
        return 0;
}

// Zavolá parametrom predanu funkciu na kazdy prvok tabulky
int htab_foreach(const htab_t *t, int (*fc)(const string, entry_t *))
{

    for(unsigned i = 0; i < t->size; i++)
    {
        for( htab_listitem * pZaznam = t->htab_ind[i];
                pZaznam != NULL;
                pZaznam = pZaznam->next)
        {
            if((*fc)(pZaznam->key, &(pZaznam->entry)) != 0)
                return 1;
        }
    }
    return 0;
}


// Funkcia slúžiaca na zmazanie celej tabuľky (a teda aj
//            uvoľnenie pamäti)
void htab_free(htab_t *t)
{
    htab_clear(t);
    free(t);
}

// Funkcia slúžiaca na inicializáciu hashovacej tabulky
htab_t * htab_init(const unsigned int size)
{
    if (size < 2)
        return NULL;

    htab_t* htab =  malloc(sizeof(htab_t) + size*sizeof(htab_listitem *));
    if (htab == NULL)
        return NULL;
    
    htab->size = size;
    htab->litIdCnt = 0;

    for(unsigned i = 0; i < size; i++)
        htab->htab_ind[i] = NULL;
    
    return htab;
} 

// Vracia pointer na zaznam v tabulke. Ak takyto zaznam neexistuje 
// vrati NULL
htab_listitem * htab_get(const htab_t *t, const string *key)
{
    if (t == NULL)
        return NULL;
    unsigned int index = hash_function(key->str, t->size);
    
    // ak existuje na danom indexe zoznam
    if (t->htab_ind[index] != NULL)
    {
        

        for(htab_listitem * pZaznam = t->htab_ind[index];
                pZaznam != NULL;
                pZaznam = pZaznam->next)
        {
            if(strCmpString(&(pZaznam->key), key) == 0)
                return pZaznam;
        }

    }
    return NULL;
}


int htab_newLabel(
        htab_t *t,
        tListItem **data,
        const string *key,
        htab_listitem **ret)
{
    string nkey;
    if(strInit(&nkey) != STR_SUCCESS)
        return HTAB_ERR_MALLOC;

    if(key == NULL)
    {
        char str[INT_MAX_C+1];
        sprintf(str, "%d@", t->litIdCnt++);
        if(strAddConstString(&nkey, str) != STR_SUCCESS)
        {
            strFree(&nkey);
            return HTAB_ERR_MALLOC;
        }
    }
    else
    {
        if(strCopyString(&nkey, key) != STR_SUCCESS 
                || strAddChar(&nkey, '@') != STR_SUCCESS)
        {
            strFree(&nkey);
            return HTAB_ERR_MALLOC;
        }

    }
    int retval =  htab_new(t, &nkey, LABEL, ARRAY_BEG, 1, true, (void *) data, ret);
    
    strFree(&nkey);
    return retval;
}

int htab_newLiteral(
        htab_t *t,
        const type_t type,
        const void *data,
        htab_listitem **ret)
{
    // checkneme ci nam to uz nepreteka
    if (t->litIdCnt < 0)
        return HTAB_ERR_MALLOC;

    // prasarna 
    char str[INT_MAX_C+1];
    sprintf(str, "%d&", t->litIdCnt++);
    const string key = {str, INT_MAX_C+1};

    return htab_new(t, &key, type, ARRAY_BEG, 1, true, data, ret);
}

int htab_newTmp(
        htab_t *t,
        const type_t type,
        htab_listitem **ret)
{
    // checkneme ci nam to uz nepreteka
    if (t->litIdCnt < 0)
        return HTAB_ERR_MALLOC;

    // prasarna 
    char str[INT_MAX_C+1];
    sprintf(str, "%d&", t->litIdCnt++);
    const string key = {str, INT_MAX_C+1};

    return htab_new(t, &key, type, ARRAY_BEG, 1, false, NULL, ret);
}

static int htab_copyfunc(func_t * pFuncNew, const func_t * pFuncIn,
        const string * funcId)
{
    pFuncNew->nextParam = NULL;
    pFuncNew->rettype = pFuncIn->rettype;
    pFuncNew->defHtab = htab_init(HTAB_SIZE);
    if (pFuncNew->defHtab == NULL)
        return HTAB_ERR_MALLOC;

    /* pridame do tabulky novej funkcie navratovu premennu */
    if(htab_new(pFuncNew->defHtab, funcId, pFuncIn->rettype, 0, 1,
            false, NULL, NULL) != HTAB_SUCCESS)
    {
        return HTAB_ERR_MALLOC;
    }

    /* skopirujeme parametre */
    for(paramEnt * pEnt = pFuncIn->nextParam;
        pEnt !=NULL;
        pEnt = pEnt->nextParam)
    {
        if(funcAddParam(pFuncNew, pEnt->paramType, &(pEnt->paramKey))
            != HTAB_SUCCESS)
        {
            return HTAB_ERR_MALLOC;
        }
    }

    return HTAB_SUCCESS;
}

static int htab_copystring(string *pStringNew, const string * pStringIn,
        const long length)
{
    for(long i=0; i<length; i++)
    {
        // Inicializuju sa jednotilive stringy
        if(strInit(&pStringNew[i]) == STR_ERROR)
        {
            // Ked sa cosi nepodari tak sa vytvorene stringy povymazavaju
            for(long j=0; j<i; j++)
                strFree(&pStringNew[j]);
            return HTAB_ERR_MALLOC;
        }

        // Kopiruju sa stringy
        if(pStringIn!= NULL 
            && strCopyString(&pStringNew[i], &pStringIn[i]) == STR_ERROR)
        {
            // Ked sa cosi nepodari tak sa vytvorene stringy povymazavaju
            for(long j=0; j<i; j++)
                strFree(&pStringNew[j]);
            return HTAB_ERR_MALLOC;
        }
    }
    return HTAB_SUCCESS;
}


// Prida prvok do tabulky
// Navratova hodnota viz. EXIT_TYPES
// Berie:
//   -htab_t *t - pointer na tabulku
//   -string *key - pointer na string obsahujuci kluc. V ciel odkazu
//       sa skopiruje do tabulky
//   -type_t type - typ premennej (viz. type_t)
//   -long begin - zaciatocny index pola (pri obycajnej premennej 0)
//   -long length - dlzka pola (pri obycajnej premennej 1)
//   -bool literal - ak sa jedna o literal true, inak false
//   -void *data - pointer kde su data, ak NULL bude sa s premennou
//       nakladat ako s neinicializovanou, ak !NULL vytvori sa miesto
//       a data sa skopiruju do tabulky
//   -entry_t **ret - NULL alebo pointer na pointer na entry_t, tadeto sa
//       vracia ukazatel na novo vzniknuty prvok tabulky (viz. entry_t).
//       Ak NULL nevracia sa nic.
//         

int htab_new(
        htab_t *t,
        const string *key,
        const type_t type,
        const long begin,
        const long length,
        const bool literal,
        const void *data,
        htab_listitem **ret)
{
    size_t size;
    bool inited;
    
    if (data == NULL)
    {
        inited = false;
        if(type == FUNC)
            return HTAB_ERR_FUNC;
    }
    else
        inited = true;

    switch(type)
    {
        case LABEL:
            size = sizeof(tListItem *);
            break;
        case INT:
            size = sizeof(int);
            break;    
        case REAL:
            size = sizeof(double);
            break;
        case BOOL:
            size = sizeof(bool);
            break;
        case STRING:
            size = sizeof(string);
            break;
        case FUNC:
            if(length != 1)
                return HTAB_ERR_FUNC_ARRAY;
            size = sizeof(func_t );
            break;

        default:
            return HTAB_SYM_NFOUND;
    }
    
    // Spravime miesto pre data
    void * space = malloc(size * length);
    if (space == NULL)
        return HTAB_ERR_MALLOC;
    
    // Ak data niesu STRING tak sa natvrdo skopiruju
    if (data != NULL && type != STRING && type != FUNC)
        memcpy(space, data, size * length);

    // Ak data su STRING tak sa musia stringy inicializovat a bla bla bla
    else if (type == STRING)
    {
        if(htab_copystring(space, data, length) != HTAB_SUCCESS)
        {
            free(space);
            return HTAB_ERR_MALLOC;
        }
    }
    
    if(type == FUNC)
    {
        if(htab_copyfunc(space, data, key) != HTAB_SUCCESS)
        {
            free(space);
            return HTAB_ERR_MALLOC;
        }

        inited = false;
    }

    // Zavola sa funkcia ktora zaradi zaznam do tabulky
    hExit_t retcode = htab_newref(t, key, type, begin, length,
            literal, inited, space, ret);

    if (retcode != HTAB_SUCCESS)
        free(space);
    return retcode;
}


htab_t * htab_copy(const htab_t * src)
{
    htab_t * t =  htab_init(HTAB_SIZE);
    if(t == NULL)
        return NULL;
    
    int ret;
    for(unsigned i = 0; i < src->size; i++)
    {
        for( htab_listitem * pZaznam = src->htab_ind[i];
                pZaznam != NULL;
                pZaznam = pZaznam->next)
        {
            ret = htab_new(
                    t, //nova tabulka
                    &(pZaznam->key), //kluc
                    pZaznam->entry.type,
                    pZaznam->entry.begin,
                    pZaznam->entry.length,
                    pZaznam->entry.literal,
                    pZaznam->entry.type == FUNC || pZaznam->entry.inited ?
                        pZaznam->entry.data : NULL,
                    NULL);
            if (ret != HTAB_SUCCESS)
            {
                htab_free(t);
                return NULL;
            }
        }
    }
    return t;
}

// Pridá symbol do tabulky. Vracia HTAB_SYM_EXISTS ak dany symbol uz existuje,
// HTAB_ERR_MALLOC ak nebolo mozne zaznam vytvorit a HTAB_SUCCESS ak vsetko prebehlo 
// spravne. Ak je ako parameter ret NULL vrati sa v nom NULL, ak sa jedna
// o je ret nenulovy vracia sa v nom pointer na zaznam;
static int htab_newref(
        htab_t *t,
        const string *key,
        const type_t type,
        const long begin,
        const long length,
        const bool literal,
        const bool inited,
        void *data,
        htab_listitem **ret)
{
    if(literal == true && data == NULL)
        return HTAB_LITERAL_UNDEF;

    if(ret != NULL)
        *ret = NULL;

    htab_listitem * pZaznam = htab_get(t, key);
    if(pZaznam != NULL)
        return HTAB_SYM_EXISTS;

    pZaznam = htab_add(t, key);
    if (pZaznam == NULL)
        return HTAB_ERR_MALLOC;


    pZaznam->entry.literal = literal;
    pZaznam->entry.inited = inited;
    pZaznam->entry.begin = begin;
    pZaznam->entry.length = length;
    pZaznam->entry.type = type;
    pZaznam->entry.data = data;

    if(ret != NULL)
        *ret = pZaznam;
    return HTAB_SUCCESS;
}

// Vrati ukazatel na entry v tabulke. AK dany kluc v tabulke neni vrati NULL
entry_t * htab_getentry(const htab_t *t, const string *key)
{
    htab_listitem *listItem = htab_get(t, key);
    return listItem ? &(listItem->entry) : NULL;
}

// Pridá do tabuľky prvok; Nekontroluje či tam už taký prvok 
// náhodou neni. 
static htab_listitem * htab_add(htab_t *t, const string *key)
{
    if (t == NULL)
        return NULL;
    unsigned int index = hash_function(key->str, t->size);

    htab_listitem * pZaznam = malloc(sizeof(htab_listitem));
    if (pZaznam == NULL)
        return NULL;

    if(strInit(&(pZaznam->key)) != STR_SUCCESS)
    {
        free(pZaznam);
        return NULL;
    }

    if (strCopyString(&(pZaznam->key), key) != STR_SUCCESS)
    {
        free(pZaznam);
        return NULL;
    }
    
    //pZaznam->data = 0;
    pZaznam->next = t->htab_ind[index];
    t->htab_ind[index] = pZaznam;

    return pZaznam;
}


// Odstrani prvok z tabulky 
int htab_remove(htab_t *t, const string *key)
{
    if( t == NULL)
        return 1;
    unsigned int index = hash_function(key->str, t->size);
    
    // ak existuje na danom indexe zoznam
    if (t->htab_ind[index] != NULL)
    {
        
        htab_listitem **ppZaznam = &(t->htab_ind[index]);
        for(htab_listitem * pZaznam = t->htab_ind[index];
                pZaznam != NULL;
                pZaznam = pZaznam->next)
        {
            if (strCmpString(key, &(pZaznam->key)) == 0)
            {
                *ppZaznam = pZaznam->next;

                htab_freeitem(pZaznam);

                return HTAB_SUCCESS;
            }
            
            ppZaznam = &pZaznam;
        }
    }
    return HTAB_SYM_NFOUND;
}

// Funkcia vypisujúca štatistické informácie o tabuľke
void htab_statistics(const htab_t *t)
{
    if (t == NULL)
        return;
    long unsigned min = ~(0);
    long unsigned max = 0;
    double avg = 0;

    for(unsigned i = 0; i < t->size; i++)
    {
        unsigned long locval = 0;
        for(htab_listitem * pZaznam = t->htab_ind[i];
                pZaznam != NULL;
                pZaznam = pZaznam->next)
        {
            locval++;
        }

        avg += locval;
        if (locval < min)
            min = locval;
        if (locval > max)
            max = locval;
    }
    
    avg /= t->size - 1;

    printf("List length:\n"\
           "\t min: %lu\n"\
           "\t max: %lu\n"\
           "\t avg: %.3f\n",
           min, max, avg);
}

int htab_print(const string s, entry_t * entry)
{
    for(long  i=0; i<entry->length; i++)
    {
        if(entry->inited)
        {
            switch (entry->type)
            {
                case INT:
                    printf("|  INT   |     Inited | %11s[%ld] | %10d |\n",
                            s.str, i+entry->begin, ((int *)entry->data)[i]);
                    printf("-----------------------------------------------------\n");
                    break;
                case LABEL:
                    printf("| LABEL  |     Inited | %11s[%ld] | %10d |\n",
                            s.str, i+entry->begin, ((int *)entry->data)[i]);
                    printf("-----------------------------------------------------\n");
                    break;
                case REAL:
                    printf("|  REAL  |     Inited | %11s[%ld] | %10.5lf |\n",
                            s.str, i+entry->begin, ((double *)entry->data)[i]);
                    printf("-----------------------------------------------------\n");
                    break;
                case BOOL:
                    printf("|  BOOL  |     Inited | %11s[%ld] | %10s |\n", 
                            s.str, i+entry->begin, 
                            ((bool *)entry->data)[i]?"TRUE":"FALSE");
                    printf("-----------------------------------------------------\n");
                    break;
                case STRING:
                    printf("| STRING |     Inited | %11s[%ld] = \"%8s\" |\n", s.str,
                            i+entry->begin,
                            ((string *)entry->data)[i].str);
                    printf("-----------------------------------------------------\n");
                    break;
                case FUNC:
                    printf("FUNC I %s ",s.str);
                    funcPrint((func_t *)entry->data);
                    printf("-----------------------------------------------------\n");
                    break;

                default:
                    break;

            }
        }
        else
        {
            switch (entry->type)
            {
                case INT:
                    printf("|  INT   | Not-Inited | %11s[%ld] |            |\n", s.str,
                            i+entry->begin);
                    printf("-----------------------------------------------------\n");
                    break;
                case REAL:
                    printf("|  REAL  | Not-Inited | %11s[%ld] |            |\n", s.str,
                            i+entry->begin);
                    printf("-----------------------------------------------------\n");
                    break;
                case BOOL:
                    printf("|  BOOL  | Not-Inited | %11s[%ld] |            |\n", s.str,
                            i+entry->begin);
                    printf("-----------------------------------------------------\n");
                    break;
                case STRING:
                    printf("| STRING | Not-Inited | %11s[%ld] |            |\n", s.str,
                            i+entry->begin);
                    printf("-----------------------------------------------------\n");
                    break;
                case FUNC:            
                    printf("|  FUNC  | Not-Inited | %11s | ",s.str);
                    funcPrint((func_t *)entry->data);
                    printf("-----------------------------------------------------\n");
                    break;

                default:
                    break;
            }
        }
    }
    return 0;
}

func_t * funcInit()
{
    func_t *f = malloc(sizeof(func_t));
    if (f == NULL)
        return NULL;
    f->defHtab = htab_init(HTAB_SIZE);
    if (f->defHtab == NULL)
    {
        free(f);
        return NULL;
    }
    f->nextParam = NULL;
    f->lastParam = NULL;
    return f;
}

void funcAddType(func_t * f, type_t ret)
{
    f->rettype = ret;
}

void funcAddAddress(htab_listitem * pTab, tListItem * iAddress)
{
    ((func_t *)(pTab->entry.data))->pStartInst = iAddress;
    pTab->entry.inited = true;
}


int funcAddParam(func_t *f, type_t paramType, string * paramKey)
{
    paramEnt * pEntNew;
    hExit_t ret = HTAB_ERR_MALLOC;

    if((pEntNew = malloc(sizeof(paramEnt))) == NULL)
        return HTAB_ERR_MALLOC;
    
    if(strInit(&(pEntNew->paramKey)) == STR_ERROR)
        goto errStrInit;

    if(strCopyString(&(pEntNew->paramKey), paramKey) == STR_ERROR)
        goto errStrCpy;

    pEntNew->paramType = paramType;
    pEntNew->nextParam = NULL;

    paramEnt * pEntEnd = f->lastParam;

    /* ak pridavame prvy parameter */
    if (pEntEnd == NULL)
    {
        f->nextParam = pEntNew;
        f->nextParam->prevParam = NULL;
    }
    
    /* ak pridavame nie 1. parameter */
    else
    {
        pEntEnd->nextParam = pEntNew;
        pEntNew->prevParam = pEntEnd;
    }
    f->lastParam = pEntNew;

    /* skusie do tabulky pridat parameter */
    if((ret = htab_new(f->defHtab, paramKey, paramType, 0, 1,
            false, NULL, NULL)) == HTAB_SUCCESS)
        return HTAB_SUCCESS;

    errStrCpy:
        strFree(&(pEntNew->paramKey));
    errStrInit:
        free(pEntNew);
        return ret;

}

void funcPrint(func_t *f)
{
    printf(" %s (", typesS[f->rettype]);
    
    for(paramEnt * pEnt = f->nextParam; pEnt !=NULL; pEnt = pEnt->nextParam)
        printf("%s %s,", typesS[pEnt->paramType], pEnt->paramKey.str);

    printf(")\n");

}

void funcFree(func_t * f)
{
    for(paramEnt * pEnt1 = f->nextParam; pEnt1 != NULL;)
    {
        paramEnt * pEnt2 = pEnt1->nextParam;
        strFree(&(pEnt1->paramKey));
        free(pEnt1);
        pEnt1 = pEnt2;
    }

    htab_free(f->defHtab);
    free(f);
}


//
//
// NASLEDUJE HEAPSORT
//
//


void exch(char * a, char * b)
{
    char tmp = *a;
    *a = *b;
    *b = tmp;
}

void fixDown(char * array, int start, int end)
{
    int parent = start, child;
    
    while(parent * 2 + 1 < end)
    {
        child = parent * 2 + 1;

        if((child + 1 < end) && IS_LESS(array[child], array[child+1]))
            child++;

        if(IS_LESS(array[parent], array[child]))
        {
            exch(&array[child],&array[parent]);
            parent = child;
        }
        else
            return;
    }
}

void hsort(char * array, int length)
{
    int start, end;

    for(start = (length-2)/2; start >=0; start --)
        fixDown(array,start,length);
    
    for(end = length - 1; end > 0; end--)
    {
        exch(&array[end], &array[0]);
        fixDown(array, 0, end);
    }
}

//
//
//    NASLEDUJE VYHLADAVACI ALGORITMUS
//
//

void kmp_table(string *text, int *arr)
{
    int pos = 2;
    int cnd = 0;
    int textLen = strLength(text);

    arr[0] = -1;
    arr[1] = 0;

    while(pos < textLen)
    {
        if(text->str[pos-1] == text->str[cnd])
            arr[pos++] = ++cnd;
        
        else if(cnd > 0)
            cnd = arr[cnd];
        
        else
            arr[pos++] = 0;
    }
}

int kmp_find(string *text, string *pattern)
{
    int m = 0;
    int i = 0;
    int textLen = strLength(text);
    int patLen = strLength(pattern);
    if (patLen == 0)
        return 0;
    
    int *arr = malloc(sizeof(int)*textLen);
    //interpretovane ako vnutorna chyba ERR_INTERN;
    if(arr == NULL)
        return -2;
    
    kmp_table(text, arr);

    while(m + i < textLen)
    {
        if(pattern->str[i] == text->str[m + i])
        {
            if(i == patLen - 1)
            {
                free(arr);
                return m;
            }
            i++;
        }
        else
        {
            if(arr[i] > -1)
            {
                m = m + i - arr[i];
                i = arr[i];
            }
            else
            {
                i = 0;
                m++;
            }
        }
    }
    free(arr);
    return -1;
}
