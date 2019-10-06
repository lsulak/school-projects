
/* c016.c: **********************************************************}
{* Téma:  Tabulka s Rozptýlenými Polo¾kami
**                      První implementace: Petr Pøikryl, prosinec 1994
**                      Do jazyka C prepsal a upravil: Vaclav Topinka, 2005
**                      Úpravy: Karel Masaøík, øíjen 2013
**
** Vytvoøete abstraktní datový typ
** TRP (Tabulka s Rozptýlenými Polo¾kami = Hash table)
** s explicitnì øetìzenými synonymy. Tabulka je implementována polem
** lineárních seznamù synonym.
**
** Implementujte následující procedury a funkce.
**
**  HTInit ....... inicializuje tabulku pøed prvním pou¾itím
**  HTInsert ..... vlo¾ení prvku
**  HTSearch ..... zji¹tìní pøítomnosti prvku v tabulce
**  HTDelete ..... zru¹ení prvku
**  HTRead ....... pøeètení hodnoty prvku
**  HTClearAll ... zru¹ení obsahu celé tabulky (inicializace tabulky
**                 poté, co ji¾ byla pou¾ita)
**
** Definici typù naleznete v souboru c016.h.
**
** Tabulka je reprezentována datovou strukturou typu tHTable,
** která se skládá z ukazatelù na polo¾ky, je¾ obsahují slo¾ky
** klíèe 'key', obsahu 'data' (pro jednoduchost typu float), a
** ukazatele na dal¹í synonymum 'ptrnext'. Pøi implementaci funkcí
** uva¾ujte maximální rozmìr pole HTSIZE.
**
** U v¹ech procedur vyu¾ívejte rozptylovou funkci hashCode.  Pov¹imnìte si
** zpùsobu pøedávání parametrù a zamyslete se nad tím, zda je mo¾né parametry
** pøedávat jiným zpùsobem (hodnotou/odkazem) a v pøípadì, ¾e jsou obì
** mo¾nosti funkènì pøípustné, jaké jsou výhody èi nevýhody toho èi onoho
** zpùsobu.
**
** V pøíkladech jsou pou¾ity polo¾ky, kde klíèem je øetìzec, ke kterému
** je pøidán obsah - reálné èíslo.
*/

#include "c016.h"

int HTSIZE = MAX_HTSIZE;
int solved;

/*          -------
** Rozptylovací funkce - jejím úkolem je zpracovat zadaný klíè a pøidìlit
** mu index v rozmezí 0..HTSize-1.  V ideálním pøípadì by mìlo dojít
** k rovnomìrnému rozptýlení tìchto klíèù po celé tabulce.  V rámci
** pokusù se mù¾ete zamyslet nad kvalitou této funkce.  (Funkce nebyla
** volena s ohledem na maximální kvalitu výsledku). }
*/

int hashCode ( tKey key ) {
	int retval = 1;
	int keylen = strlen(key);
	for ( int i=0; i<keylen; i++ )
		retval += key[i];
	return ( retval % HTSIZE );
}

/*
** Inicializace tabulky s explicitnì zøetìzenými synonymy.  Tato procedura
** se volá pouze pøed prvním pou¾itím tabulky.
*/

void htInit ( tHTable* ptrht ) {

	if(ptrht != NULL)						//Osetrenie , ci ptrht nieje nulovy , ak je, nic sa nevykona, takto budem zacinat kazdu funkciu.
		for(int i=0 ; i <= HTSIZE-1 ; i++) 	//cyklus kt. bude nulovat vsetky prvky hash. tabulky, teda vytvorime prazdnu tabulku o velkosti 0-HTSIZE-1 , teda HTSIZE
			(*ptrht)[i] = NULL; 
}

/* TRP s explicitnì zøetìzenými synonymy.
** Vyhledání prvku v TRP ptrht podle zadaného klíèe key.  Pokud je
** daný prvek nalezen, vrací se ukazatel na daný prvek. Pokud prvek nalezen není, 
** vrací se hodnota NULL.
**
*/

tHTItem* htSearch ( tHTable* ptrht, tKey key ) {

	if(ptrht != NULL)
	{
		int i = 0;
 		i = hashCode(key);				//Najdeme prislusny index kluca, ulozime ho do pomocnej premennej 
	
		tHTItem* PomUk = NULL;			//Vytvorime si pomocny ukazatel, na zac. ho inicializujeme na NULL
		PomUk = (*ptrht)[i];			
		if(PomUk == NULL)
			return NULL;

		else
		{		
			while((PomUk != NULL) && (PomUk->key != key)) //Ak nieje pomocny ukazatel hodny NULL, a kluce sa nezhoduju, pozri dalsi, a dalsi, a ked sa budu zhodovat tak ho returni
				PomUk = PomUk->ptrnext;

			return PomUk;
		}
	}

	else
		return NULL;
}

/* 
** TRP s explicitnì zøetìzenými synonymy.
** Tato procedura vkládá do tabulky ptrht polo¾ku s klíèem key a s daty
** data.  Proto¾e jde o vyhledávací tabulku, nemù¾e být prvek se stejným
** klíèem ulo¾en v tabulce více ne¾ jedenkrát.  Pokud se vkládá prvek,
** jeho¾ klíè se ji¾ v tabulce nachází, aktualizujte jeho datovou èást.
**
** Vyu¾ijte døíve vytvoøenou funkci htSearch.  Pøi vkládání nového
** prvku do seznamu synonym pou¾ijte co nejefektivnìj¹í zpùsob,
** tedy proveïte.vlo¾ení prvku na zaèátek seznamu.
**/

void htInsert ( tHTable* ptrht, tKey key, tData data ) {
	if(ptrht != NULL)
	{
		tHTItem* PomUk = NULL; 					//Vytvorime si dva pomocne ukazatele, na zac. ich inicializujeme na NULL
		tHTItem* novy = NULL;
	
		PomUk = htSearch(ptrht, key); 			//pomocny ukazatel 'PomUk' je kvoli zapamataniu si indexu , ak nieje nulovy, znamena to ze tam nieco bolo, a iba sa prepise datova cast
		if (PomUk == NULL)
		{
		 	novy = malloc(sizeof(tHTable));		//pomocny ukazatel 'novy' - donho sa pamata alokovanie miesta v pamati , o velkosti Hash. tabulky
		 	if(novy != NULL)					//ak sa alokacia nepodari vrati null
			{
		 		novy->key=key;					//kluc, data sa vlozia do pomocneho ukazatela
		 		novy->data=data;
		 		novy->ptrnext = (*ptrht)[hashCode(key)]; //naplnime ukazatel na dalsi prvok, a pridame do tabulky
		 		(*ptrht)[hashCode(key)] = novy;

		 	}
		}
		else 
			PomUk->data = data;   				//prepisanie datova cast, vid IF
	}
}

/*
** TRP s explicitnì zøetìzenými synonymy.
** Tato funkce zji¹»uje hodnotu datové èásti polo¾ky zadané klíèem.
** Pokud je polo¾ka nalezena, vrací funkce ukazatel na polo¾ku
** Pokud polo¾ka nalezena nebyla, vrací se funkèní hodnota NULL
**
** Vyu¾ijte døíve vytvoøenou funkci HTSearch.
*/

tData* htRead ( tHTable* ptrht, tKey key ) {

	tHTItem* PomUk = NULL;
	if(ptrht != NULL)
	{
		PomUk = htSearch(ptrht, key);		//Najdeme poziciu pomocou hrSearch , ulozime do pomocneho retazca, ak nieje NULL tak funkcia vrati obsah z klucu (data), inak vzdy NULL
		
		if(PomUk != NULL)
			return &PomUk->data;
		else
			return NULL;
	}

	else 
		return NULL;

}

/*
** TRP s explicitnì zøetìzenými synonymy.
** Tato procedura vyjme polo¾ku s klíèem key z tabulky
** ptrht.  Uvolnìnou polo¾ku korektnì zru¹te.  Pokud polo¾ka s uvedeným
** klíèem neexistuje, dìlejte, jako kdyby se nic nestalo (tj. nedìlejte
** nic).
**
** V tomto pøípadì NEVYU®ÍVEJTE døíve vytvoøenou funkci HTSearch.
*/

void htDelete ( tHTable* ptrht, tKey key ) {
	if(ptrht == NULL)
		return;

	tHTItem* PomUk1 = NULL;				//vytvorime 2 ukazatele, na zac. budu mat rovnake hodnoty = NULL
	tHTItem* PomUk2 = NULL;
	unsigned int i = 0;
	i = hashCode(key);					//Najdeme a ulozime index na kt. sa bude ukladat
	PomUk1 = (*ptrht)[i];				//Obe ukazatele su zatial stejne
	PomUk2 = (*ptrht)[i];

	if(PomUk2 == NULL)					//usetrenie ci nieje jeden z nich nulovy - jeden staci preto lebo su rovnake
		return;

	if(PomUk2->key == key)				//ak je zhoda klucov, uloz do PomUk2, dalsi prvok bude v predchadzjucom, a odalokuj PomUk2
	{
		PomUk2=(*ptrht)[i];
		(*ptrht)[i] = (*ptrht)[i]->ptrnext;
		free(PomUk2);
	}
	else
	{
		while(PomUk2 != NULL)			//zastav az ked bude pomocny ukazatel NULL teda bude uvolneny z pamati
		{
			if(PomUk2 != (*ptrht)[i])	
			{
				PomUk2->ptrnext = PomUk1->ptrnext; 	//prepiseme ukazatele a prvy uvolnime
				free(PomUk1);
			}
			else
			{
				(*ptrht)[i] = PomUk2->ptrnext;		//dalsi prvok priradime k sucasnemu a uvolnime pomocny ukazatel
				free(PomUk2);
				return;
			}

		}
	}

}

/* TRP s explicitnì zøetìzenými synonymy.
** Tato procedura zru¹í v¹echny polo¾ky tabulky,          korektnì uvolní prostor,
** který tyto polo¾ky zabíraly, a uvede tabulku do poèáteèního stavu.
*/

void htClearAll ( tHTable* ptrht ) {
	if(ptrht == NULL) 
		return;

	tHTItem* PomUk = NULL; 						//Vytvorime si pomocny ukazatel typu hash.tabulky

	for(int i = 0; i <= HTSIZE -1 ; i++) {		//Budeme mazat postupne v cykle vsetky prvky tabulky
		while((*ptrht)[i] != NULL)				//Kazdy prvok musime nulovat a odalokovat, vyuzijeme na to pomocnu premennu
		{
			PomUk = (*ptrht)[i];				//Ulozime si obsah HT z daneho indexu do pomocnej premennej
			(*ptrht)[i] = (*ptrht)[i]->ptrnext;	//Nahradime dalsi sucasnym
			free(PomUk);						//Uvolnime pomocny retazec
		}
	}

}
