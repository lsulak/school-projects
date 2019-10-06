/**********************************************************|
|**        Projekt do predmetov IFJ a IAL 2014/2015      **|
|**Subor:    instructions.h                              **|
|**Popis:    Hlavickovy subor k modulu instructions.c    **|
|**Autori:                                               **|
|**                                                      **|
|**       Tomas Poremba   xporem00                       **|
|**********************************************************/ 
#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "ial.h"
typedef enum {
    START,
    TOKEN,
    END,
} read_state_t;

ret_t plus(htab_t *globTable, tabListItem_t *tab, tInstr *instr);
ret_t minus(htab_t *globTable, tabListItem_t *tab, tInstr *instr);
ret_t assign(htab_t *globTable, tabListItem_t *tab, tInstr *inst);
ret_t multiply(htab_t *globTable, tabListItem_t *tab, tInstr *instr);
ret_t divide(htab_t *globTable, tabListItem_t *tab, tInstr *instr);

ret_t write(htab_t *globTable, tabListItem_t *tab, tInstr *instr);

ret_t j_else(htab_t *globTable, tabListItem_t *tab, tInstr *instr, tListOfInstr *L);
ret_t jump(htab_t *globTable, tabListItem_t *tab, tInstr *instr, tListOfInstr *L);

ret_t equal(htab_t *globTable, tabListItem_t *tab, tInstr *instr);
ret_t less(htab_t *globTable, tabListItem_t *tab, tInstr *instr);
ret_t more(htab_t *globTable, tabListItem_t *tab, tInstr *instr);
ret_t less_equal(htab_t *globTable, tabListItem_t *tab, tInstr *instr);
ret_t more_equal(htab_t *globTable, tabListItem_t *tab, tInstr *instr);
ret_t not_equal(htab_t *globTable, tabListItem_t *tab, tInstr *instr);

ret_t call(    htab_t *globTable,
            tabListItem_t *tab, 
            tInstr *instr,
            tabList_t *tabList,
            paramList_t *paramList,
            tListOfInstr *instList,
            addStack_t *addStack);


ret_t ret(    tabList_t *tabList, 
            tListOfInstr *instList,
            addStack_t *addStack);

ret_t readln(htab_t *globTable, tabListItem_t *tab, tInstr *instr);
ret_t find(htab_t *globTable, tabListItem_t *tab, tInstr *instr);
ret_t copy(htab_t *globTable, tabListItem_t *tab, tInstr *instr);
ret_t len(htab_t *globTable, tabListItem_t *tab, tInstr *instr);
ret_t sort(htab_t *globTable, tabListItem_t *tab, tInstr *instr);
#endif
