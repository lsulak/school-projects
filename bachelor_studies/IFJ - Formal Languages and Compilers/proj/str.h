/**********************************************************|
|**        Projekt do predmetov IFJ a IAL 2014/2015      **|
|**Subor : str.c                                         **|
|**Autori:                                               **|
|**       prevzate z ukazkoveho interpretu               **|
|**       upravil Matej Muzila xmuzil00                  **|
|**********************************************************/
//hlavickovy soubor pro praci s nekonecne dlouhymi retezci
#ifndef STR_H
#define STR_H

#define STR_LEN_INC 8
// konstanta STR_LEN_INC udava, na kolik bytu provedeme pocatecni alokaci pameti
// pokud nacitame retezec znak po znaku, pamet se postupne bude alkokovat na
// nasobky tohoto cisla 

#define STR_ERROR   99
#define STR_SUCCESS 1

typedef struct
{
    char* str;        // misto pro dany retezec ukonceny znakem '\0'
    size_t allocSize;    // velikost alokovane pameti
} string;


int strInit(string *s);
void strFree(string *s);

void strClear(string *s);
int strAddChar(string *s1, char c);
int strCopyString(string *s1, const string *s2);
int strCmpString(const string *s1, const string *s2);
int strCmpConstStr(const string *s1, const char *s2);
int strAddConstString(string * s1, const char * src);

char *strGetStr(string *s);
size_t strLength(const string *s);


#endif
