/**********************************************************|
|**        Projekt do predmetov IFJ a IAL 2014/2015      **|
|**Subor : str.c                                         **|
|**Autori:                                               **|
|**       prevzate z ukazkoveho interpretu               **|
|**       upravil Matej Muzila (xmuzil00)                **|
|**********************************************************/
//jednoducha knihovna pro praci s nekonecne dlouhymi retezci
#include <string.h>
#include <stdlib.h>
#include "str.h"


int strInit(string *s)
// funkce vytvori novy retezec
{
   if ((s->str = (char*) malloc(STR_LEN_INC)) == NULL)
      return STR_ERROR;
   memset(s->str, '\0', STR_LEN_INC);
   s->allocSize = STR_LEN_INC;
   return STR_SUCCESS;
}

void strFree(string *s)
// funkce uvolni retezec z pameti
{
   free(s->str);
}

void strClear(string *s)
// funkce vymaze obsah retezce
{
   s->str[0] = '\0';
}

int strAddChar(string *s1, char c)
// prida na konec retezce jeden znak
{
   if (strLength(s1) + 1 >= s1->allocSize)
   {
      // pamet nestaci, je potreba provest realokaci
      if ((s1->str = (char*) realloc(s1->str, 
                      strLength(s1) + STR_LEN_INC)) == NULL)
         return STR_ERROR;
      s1->allocSize = strLength(s1) + STR_LEN_INC;
   }
   size_t len = strLength(s1);
   s1->str[len] = c;
   s1->str[len+1]= '\0';
   return STR_SUCCESS;
}

int strAddConstString(string * s1, const char * src)
{
    for(unsigned i=0; i<strlen(src); i++)
        if(strAddChar(s1, src[i]) != STR_SUCCESS)
            return STR_ERROR;
    return STR_SUCCESS;
}

        

int strCopyString(string *s1, const string *s2)
// prekopiruje retezec s2 do s1
{
   const size_t newLength = strLength(s2);
   if (newLength >= s1->allocSize)
   {
      // pamet nestaci, je potreba provest realokaci
      if ((s1->str = (char*) realloc(s1->str, newLength + 1)) == NULL)
         return STR_ERROR;
      s1->allocSize = newLength + 1;
   }
   strcpy(s1->str, s2->str);
   return STR_SUCCESS;
}

int strCmpString(const string *s1, const string *s2)
// porovna oba retezce a vrati vysledek
{
   return strcmp(s1->str, s2->str);
}

int strCmpConstStr(const string *s1, const char* s2)
// porovna nas retezec s konstantnim retezcem
{
   return strcmp(s1->str, s2);
}

char *strGetStr(string *s)
// vrati textovou cast retezce
{
   return s->str;
}

size_t strLength(const string *s)
// vrati delku daneho retezce
{
    return strlen(s->str);
}

