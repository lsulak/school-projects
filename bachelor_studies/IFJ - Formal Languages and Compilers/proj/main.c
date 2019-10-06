/**********************************************************|
|**        Projekt do predmetov IFJ a IAL 2014/2015      **|
|**Subor:    ial.c                                       **|
|**Popis:    Obsahuje hashovaciu tabulku, heapsort       **|
|**          a vyhladavaci algoritmus                    **|
|**Autori:                                               **|
|**                                                      **|
|**       Matej Muzila    xmuzil00                       **|
|**       Tomas Poremba   xporem00                       **|
|**       Marek Minda     xminda00                       **|
|**********************************************************/ 
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "str.h"
#include "scanner.h"
#include "parser.h"
#include "ial.h"
#include "ilist.h"
#include "interpret.h"
#include "debug.h"


void signal_handler()
{
    fprintf(stderr, "S-I-G-S-E-G-V\n"
                    "Dovolali ste sa do odkazovej schranky.\n");
    exit(99);
}

int main(int argc, char* argv[])
{
    signal(SIGSEGV,signal_handler);
        
    int result = 0;

    if (argc != 2)
        return 99; 
  
    if (openFile(argv[1]) != 0)
        return 99;    


    htab_t *symbolTable;
    symbolTable = htab_init(HTAB_SIZE);
      
    tListOfInstr IList;
    listInit(&IList);

    if (symbolTable ==NULL)
    {
        return 99;
    }
    if (htab_newLabel(symbolTable, NULL, &sNULL, NULL) != HTAB_SUCCESS)
    {
        return 99;
    }

    result = parse(symbolTable,&IList);
    if (result != SUCCESS)
    {
        fprintf(stderr,"parser returned with err %d\n", result);
        fprintf(stderr, "Error on line: %d\n", lineNumber);
        listFree(&IList);
        closeFile();
        htab_free(symbolTable);
        return result;
    }

    DEB_PRINTF("\n>>>>>>>>>INTERPRET:\n");

    result = interpret(symbolTable,&IList);
    
      
    htab_free(symbolTable);
    
    listFree(&IList);  
    closeFile();
    
    return result;

}
