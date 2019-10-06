/**********************************************************|
|**        Projekt do predmetov IFJ a IAL 2014/2015      **|
|**Subor : psa_new.h                               	 	 **|
|**Popis : Hlavickovy subor k modulu psa_new.h       	 **|
|**Autori:                                               **|
|**                                                      **|
|**       Matej Muzila   xmuzil00                        **|
|**********************************************************/

#ifndef PSA_H
#define PSA_H
#include "ial.h"
#include "ilist.h"
#include "scanner.h"
#include "interpret.h"



ret_t PSA(htab_t * tGlob, htab_t * tLoc, tListOfInstr * iList, type_t wantsType);

#endif
