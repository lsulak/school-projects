/**********************************************************|
|**        Projekt do predmetov IFJ a IAL 2014/2015      **|
|**Subor : debug.h                                       **|
|**Autori: Matej Muzila (xmuzil00)                       **|
|**                                                      **|
|**********************************************************/
#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

#ifdef DEBUG
#define DEB_PRINTF    printf
#else
#define DEB_PRINTF(...)
#endif
#endif 

