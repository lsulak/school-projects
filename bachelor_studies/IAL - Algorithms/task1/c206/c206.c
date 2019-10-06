	
/* c206.c **********************************************************}
{* Téma: Dvousmìrnì vázaný lineární seznam
**
**                   Návrh a referenèní implementace: Bohuslav Køena, øíjen 2001
**                            Pøepracované do jazyka C: Martin Tuèek, øíjen 2004
**                                             Úpravy: Bohuslav Køena, øíjen 2013
**
** Implementujte abstraktní datový typ dvousmìrnì vázaný lineární seznam.
** U¾iteèným obsahem prvku seznamu je hodnota typu int.
** Seznam bude jako datová abstrakce reprezentován promìnnou
** typu tDLList (DL znamená Double-Linked a slou¾í pro odli¹ení
** jmen konstant, typù a funkcí od jmen u jednosmìrnì vázaného lineárního
** seznamu). Definici konstant a typù naleznete v hlavièkovém souboru c206.h.
**
** Va¹ím úkolem je implementovat následující operace, které spolu
** s vý¹e uvedenou datovou èástí abstrakce tvoøí abstraktní datový typ
** obousmìrnì vázaný lineární seznam:
**
**      DLInitList ...... inicializace seznamu pøed prvním pou¾itím,
**      DLDisposeList ... zru¹ení v¹ech prvkù seznamu,
**      DLInsertFirst ... vlo¾ení prvku na zaèátek seznamu,
**      DLInsertLast .... vlo¾ení prvku na konec seznamu, 
**      DLFirst ......... nastavení aktivity na první prvek,
**      DLLast .......... nastavení aktivity na poslední prvek, 
**      DLCopyFirst ..... vrací hodnotu prvního prvku,
**      DLCopyLast ...... vrací hodnotu posledního prvku, 
**      DLDeleteFirst ... zru¹í první prvek seznamu,
**      DLDeleteLast .... zru¹í poslední prvek seznamu, 
**      DLPostDelete .... ru¹í prvek za aktivním prvkem,
**      DLPreDelete ..... ru¹í prvek pøed aktivním prvkem, 
**      DLPostInsert .... vlo¾í nový prvek za aktivní prvek seznamu,
**      DLPreInsert ..... vlo¾í nový prvek pøed aktivní prvek seznamu,
**      DLCopy .......... vrací hodnotu aktivního prvku,
**      DLActualize ..... pøepí¹e obsah aktivního prvku novou hodnotou,
**      DLSucc .......... posune aktivitu na dal¹í prvek seznamu,
**      DLPred .......... posune aktivitu na pøedchozí prvek seznamu, 
**      DLActive ........ zji¹»uje aktivitu seznamu.
**
** Pøi implementaci jednotlivých funkcí nevolejte ¾ádnou z funkcí
** implementovaných v rámci tohoto pøíkladu, není-li u funkce
** explicitnì uvedeno nìco jiného.
**
** Nemusíte o¹etøovat situaci, kdy místo legálního ukazatele na seznam 
** pøedá nìkdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodnì komentujte!
**
** Terminologická poznámka: Jazyk C nepou¾ívá pojem procedura.
** Proto zde pou¾íváme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/

#include "c206.h"

int solved;
int errflg;

void DLError() {
/*
** Vytiskne upozornìní na to, ¾e do¹lo k chybì.
** Tato funkce bude volána z nìkterých dále implementovaných operací.
**/	
    printf ("*ERROR* This program has performed an illegal operation.\n");
    errflg = TRUE;             /* globální promìnná -- pøíznak o¹etøení chyby */
    return;
}

void DLInitList (tDLList *L)	{
/*
** Provede inicializaci seznamu L pøed jeho prvním pou¾itím (tzn. ¾ádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádìt nad ji¾ inicializovaným
** seznamem, a proto tuto mo¾nost neo¹etøujte. V¾dy pøedpokládejte,
** ¾e neinicializované promìnné mají nedefinovanou hodnotu.
**/
   L->First = NULL;	//Inicializacia zoznamu.Jedna sa o prazdny zoznam,takze vsetky ukazatele budu NULL
   L->Act = NULL;
   L->Last = NULL; 
}

void DLDisposeList (tDLList *L)	{
/*
** Zru¹í v¹echny prvky seznamu L a uvede seznam do stavu, v jakém
** se nacházel po inicializaci. Ru¹ené prvky seznamu budou korektnì
** uvolnìny voláním operace free. 
**/
	tDLElemPtr PomUk = NULL;  //Vytvorenie pomocneho ukazatela, na zaciatku bude NULL
	while (L->First != NULL) //Rus postupne prvky , az pokial nebude prvy NULL
		{
			PomUk = L->First; //Zaciatok si ulozime do pomocneho ukazatela
			L->First = L->First->rptr; //nasledujuci prvok (od prveho) sa premiestni do prveho
						
			free(PomUk); //zrusi sa v podstate prvy prvok..a takto v cykle postupne
		}
	L->Last = NULL;
	L->Act = NULL;	
	
 
}

void DLInsertFirst (tDLList *L, int val)	{
/*
** Vlo¾í nový prvek na zaèátek seznamu L.
** V pøípadì, ¾e není dostatek pamìti pro nový prvek pøi operaci malloc,
** volá funkci DLError().
**/
	tDLElemPtr PomUk = NULL;  //Vytvorenie pomocneho ukazatela, na zaciatku bude NULL
	PomUk = (tDLElemPtr)malloc(sizeof(struct tDLElem));	

	if(PomUk == NULL) //Ak sa alokacia nepodari, zavolaj danu funkciu
	{
		DLError();
		return;
	}

	PomUk->data = val; //parameter val bude priradeny do data
	PomUk->lptr = NULL; //Predchadzajuci prvok neexistuje, pretoze tato funkcia vklada prvy prvok
	PomUk->rptr = L->First; //Nasledujuci prvok je vlastne prvym alebo NULL

	if(L->First != NULL) //Ak prvy prvok nieje NULL - to znamena ze sa nejedna o prazdny zoznam
		L->First->lptr = PomUk;
	else				//Ak sa jedna o prazdny zoznam , tak prvy a posledny prvok su rovnake
		L->Last = PomUk; 

		L->First = PomUk; //Prvemu prvku priradim hodnotu ktora ma byt na prvom mieste v zozname
	
}

void DLInsertLast(tDLList *L, int val)	{
/*
** Vlo¾í nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
** V pøípadì, ¾e není dostatek pamìti pro nový prvek pøi operaci malloc,
** volá funkci DLError().
**/ 	
	tDLElemPtr PomUk = NULL;  //Vytvorenie pomocneho ukazatela, na zaciatku bude NULL
	PomUk = (tDLElemPtr)malloc(sizeof(struct tDLElem));	

	if(PomUk == NULL) //Ak sa alokacia nepodari, zavolaj danu funkciu
	{
		DLError();
		return;
	}

	PomUk->data = val; //parameter val bude priradeny do data
	PomUk->lptr = L->Last; //Predchadzajuci prvok 
	PomUk->rptr = NULL; //Nasledujuci prvok neexistuje, lebo vkladame posledny prvok.

	if((L->First != NULL) && (L->Last != NULL)) //Ak prvy a posledny prvok nieje NULL - to znamena ze sa nejedna o prazdny zoznam
		L->Last->rptr = PomUk;
	else
		L->First = PomUk; //ak sa jedna o prazdny zoznam, posledny bude prvy

		L->Last = PomUk; //poslednemu prvku priradim hodnotu ktora ma byt na poslednom mieste v zozname
}



void DLFirst (tDLList *L)	{
/*
** Nastaví aktivitu na první prvek seznamu L.
** Funkci implementujte jako jediný pøíkaz (nepoèítáme-li return),
** ani¾ byste testovali, zda je seznam L prázdný.
**/
	
	L->Act = L->First;
}

void DLLast (tDLList *L)	{
/*
** Nastaví aktivitu na poslední prvek seznamu L.
** Funkci implementujte jako jediný pøíkaz (nepoèítáme-li return),
** ani¾ byste testovali, zda je seznam L prázdný.
**/
	
	L->Act = L->Last;
}

void DLCopyFirst (tDLList *L, int *val)	{
/*
** Prostøednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/

	if(L->First == NULL) 	
	{
		DLError();
		return;
	}

	*val = L->First->data; //Do parametru val prirad hodnotu ktora sa nachadza v prvom prvku 
 
}

void DLCopyLast (tDLList *L, int *val)	{
/*
** Prostøednictvím parametru val vrátí hodnotu posledního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/

	if(L->First == NULL)	
	{	
		DLError();
		return;
	}
	*val = L->Last->data; //Symetricka funkcia s predchadzajucou, ibaze kopiruje poslednu prvok

}

void DLDeleteFirst (tDLList *L)	{
/*
** Zru¹í první prvek seznamu L. Pokud byl první prvek aktivní, aktivita 
** se ztrácí. Pokud byl seznam L prázdný, nic se nedìje.
**/
	tDLElemPtr PomUk = NULL;  //Vytvorenie pomocneho ukazatela, na zaciatku bude NULL
	
	if(L->First == NULL)	
	{	
		free(PomUk);
		return;
	}

	if(L->Act == L->First)	  //Ak je prvy aktivnym, nastav ukazatel aktivneho na NULL...teda zrus aktivitu
		L->Act = NULL;

	if(L->First == L->Last) //Ak je prvy zaroven poslednym, vynuluj posledny
		L->Last = NULL; 

	PomUk = L->First->rptr; //Ulozim si hodnotu dalsieho prvku do pomocneho ukazatela, vzapati sa z neho stane prvy prvok
	free(L->First);	//Prvy zrusime

	L->First = PomUk; //Ulozili sme si hodnotu Dalsieho prvku do pomocneho ukazatela , teraz sa z neho stane prvy prvok v zozname
	if(L->First != NULL)
		L->First->lptr = NULL; //Lavy vynulujeme, aby bol prvy naozaj prvym a nic nalavo 
}	

void DLDeleteLast (tDLList *L)	{
/*
** Zru¹í poslední prvek seznamu L. Pokud byl poslední prvek aktivní,
** aktivita seznamu se ztrácí. Pokud byl seznam L prázdný, nic se nedìje.
**/ 
	tDLElemPtr PomUk = NULL;  //Vytvorenie pomocneho ukazatela, na zaciatku bude NULL
	

	if (L->First == NULL)
	{
		free(PomUk);
		return;
	}

	PomUk=L->Last;
	
	if(L->Last == L->Act)	  //Ak je posledny aktivnym, nastav ukazatel aktivneho na NULL...teda zrus aktivitu
		L->Act = NULL;

	if(L->First == L->Last)
	{
		L->First = NULL; 
		L->Last = NULL;
	}
	else
	{
	L->Last = L->Last->lptr; //Ulozim si hodnotu predosleho prvku do pomocneho ukazatela, vzapati sa z neho stane prvy prvok
	
	L->Last->rptr = NULL;
	}
	free(PomUk);
}

void DLPostDelete (tDLList *L)	{
/*
** Zru¹í prvek seznamu L za aktivním prvkem.
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** posledním prvkem seznamu, nic se nedìje.
**/
	tDLElemPtr PomUk = NULL;

	if((L->Act == NULL) || (L->Act == L->Last))	//Presne podla zadania (neaktivita ....) -> zrus ukazatel, a nic nerob 
 		{
 			free(PomUk);
 			return ;
 		}

 	PomUk = L->Act->rptr; //ukazatel na prvok ktory sa nachadza napravo od aktivneho
 	
 	if(PomUk->rptr != NULL) //Moze byt nasledujuci nulovy alebo nie ....a na zaver zrus pomocny ukazatel
 		{
 			L->Act->rptr = PomUk->rptr;
 			PomUk->rptr->lptr = L->Act;
 		}
 	else
 		{
 			L->Act->rptr=NULL;
 			L->Last = L->Act;
 		}
 			free(PomUk);
}

void DLPreDelete (tDLList *L)	{
/*
** Zru¹í prvek pøed aktivním prvkem seznamu L .
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** prvním prvkem seznamu, nic se nedìje.
**/
	
	tDLElemPtr PomUk = NULL;
	
	if((L->Act == NULL) || (L->Act == L->First))	//Presne podla zadania (neaktivita ....) -> zrus ukazatel, a nic nerob	
 		{
 			free(PomUk);
 			return ;
 		}

 	PomUk = L->Act->lptr;  //ukazatel na prvok ktory sa nachadza nalavo od aktivneho

 	if(PomUk->lptr != NULL) //Moze byt predosly nulovy alebo nie ....a na zaver zrus pomocny ukazatel
 		{
 			L->Act->lptr = PomUk->lptr;
 			PomUk->lptr->rptr = L->Act;
 		}
 	else
 		{
 			L->Act->lptr=NULL;
 			L->First = L->Act;
 		}
 			free(PomUk);
}

void DLPostInsert (tDLList *L, int val) {
/*
** Vlo¾í prvek za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se nedìje.
** V pøípadì, ¾e není dostatek pamìti pro nový prvek pøi operaci malloc,
** volá funkci DLError().
**/
	
	tDLElemPtr PomUk = NULL;  //Vytvorenie pomocneho ukazatela, na zaciatku bude NULL
	PomUk = (tDLElemPtr)malloc(sizeof(struct tDLElem));	

	if(PomUk == NULL) //Ak sa alokacia nepodari, zavolaj danu funkciu
	{	
		DLError();
	}

	if(L->Act == NULL) //Ak je neaktivny nic sa nedeje
		return;

	if(L->Act->rptr == NULL) //Ak je napravo od aktivneho nulovy, tak sa z pomocneho ukazatela do posledneho vlozi prvok
		L->Last =PomUk;
	else
		L->Act->rptr->lptr = PomUk; 

	PomUk->rptr = L->Act->rptr;	//ukazatele na dalsi a predchadzajuci
	PomUk->lptr = L->Act;
	L->Act->rptr = PomUk;

	PomUk->data = val; //VLozime prvok do miesta kam ukazuje pomocny ukazatel
	
}

void DLPreInsert (tDLList *L, int val)		{
/*
** Vlo¾í prvek pøed aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se nedìje.
** V pøípadì, ¾e není dostatek pamìti pro nový prvek pøi operaci malloc,
** volá funkci DLError().
**/
	
	tDLElemPtr PomUk = NULL;  //Vytvorenie pomocneho ukazatela, na zaciatku bude NULL
	PomUk = (tDLElemPtr)malloc(sizeof(struct tDLElem));	

	if(PomUk == NULL) //Ak sa alokacia nepodari, zavolaj danu funkciu
	{
		DLError();
		return;
	}

	if(L->Act == NULL)	//Ak je neaktivny nic sa nedeje
		return;

	if(L->Act->lptr == NULL)	//Ak je naplavo od aktivneho nulovy, tak sa z pomocneho ukazatela do prveho vlozi prvok
		L->First = PomUk;

	else
		L->Act->lptr->rptr = PomUk;

	PomUk->lptr = L->Act->lptr; //ukazatele na predchadzajuci a dalsi
	PomUk->rptr = L->Act;
	L->Act->lptr = PomUk;

	PomUk->data = val;  //Vlozime prvok do miesta kam ukazuje pomocny ukazatel
}

void DLCopy (tDLList *L, int *val)	{
/*
** Prostøednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam L není aktivní, volá funkci DLError ().
**/
		
	if (L->Act == NULL) //Ak je zoznam neaktivny nic nerob resp. zavolaj iba funkciu ktora to oznami
	{
		DLError();
		return;
	}
	*val = L->Act->data;	//inak prirad do parametru val hodnotu z aktivneho prvku
	
 
}

void DLActualize (tDLList *L, int val) {
/*
** Pøepí¹e obsah aktivního prvku seznamu L.
** Pokud seznam L není aktivní, nedìlá nic.
**/
	
	if(L->Act == NULL) //Nic nerob ak je zoznam aktivny...inak prirad do aktivneho hodnotu z parametru val
		return;
	L->Act->data = val;
 
}

void DLSucc (tDLList *L)	{
/*
** Posune aktivitu na následující prvek seznamu L.
** Není-li seznam aktivní, nedìlá nic.
** V¹imnìte si, ¾e pøi aktivitì na posledním prvku se seznam stane neaktivním.
**/
	if (L->Act == NULL) 	//Nic nerob ak nieje zoznam aktivny
		return;

	if (L->Act == L->Last) //zrus aktivitu ak je posledny aktivny
	{
      L->Act = NULL;
      return;
  	}
  	L->Act = L->Act->rptr;
 }



void DLPred (tDLList *L)	{
/*
** Posune aktivitu na pøedchozí prvek seznamu L.
** Není-li seznam aktivní, nedìlá nic.
** V¹imnìte si, ¾e pøi aktivitì na prvním prvku se seznam stane neaktivním.
**/
	if(L->Act == NULL) //Nic nerob ak nieje zoznam aktivny
		return;

	if (L->Act == L->First) //zrus aktivitu ak je prvy aktivny
	{
      L->Act = NULL;
      return;
  	}
	L->Act = L->Act->lptr;

}

int DLActive (tDLList *L) {		
/*
** Je-li seznam aktivní, vrací true. V opaèném pøípadì vrací false.
** Funkci implementujte jako jediný pøíkaz.
**/
	
	return (L->Act != NULL) ? 1 : 0;	// 1 predstavuje TRUE, 0 predstavuje FALSE
 
}

/* Konec c206.c*/
