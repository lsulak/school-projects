
/* c401.c: **********************************************************}
{* Téma: Rekurzivní implementace operací nad BVS
**                                         Vytvoøil: Petr Pøikryl, listopad 1994
**                                         Úpravy: Andrea Nìmcová, prosinec 1995
**                                                      Petr Pøikryl, duben 1996
**                                                   Petr Pøikryl, listopad 1997
**                                  Pøevod do jazyka C: Martin Tuèek, øíjen 2005
**                                         Úpravy: Bohuslav Køena, listopad 2009
**                                         Upravy: Karel Masarik, rijen 2013
**
** Implementujte rekurzivním zpùsobem operace nad binárním vyhledávacím
** stromem (BVS; v angliètinì BST - Binary Search Tree).
**
** Klíèem uzlu stromu je jeden znak (obecnì jím mù¾e být cokoliv, podle
** èeho se vyhledává). U¾iteèným (vyhledávaným) obsahem je zde integer.
** Uzly s men¹ím klíèem le¾í vlevo, uzly s vìt¹ím klíèem le¾í ve stromu
** vpravo. Vyu¾ijte dynamického pøidìlování pamìti.
** Rekurzivním zpùsobem implementujte následující funkce:
**
**   BSTInit ...... inicializace vyhledávacího stromu
**   BSTSearch .... vyhledávání hodnoty uzlu zadaného klíèem
**   BSTInsert .... vkládání nové hodnoty
**   BSTDelete .... zru¹ení uzlu se zadaným klíèem
**   BSTDispose ... zru¹ení celého stromu
**
** ADT BVS je reprezentován koøenovým ukazatelem stromu (typ tBSTNodePtr).
** Uzel stromu (struktura typu tBSTNode) obsahuje klíè (typu char), podle
** kterého se ve stromu vyhledává, vlastní obsah uzlu (pro jednoduchost
** typu int) a ukazatel na levý a pravý podstrom (LPtr a RPtr). Pøesnou definici typù 
** naleznete v souboru c401.h.
**
** Pozor! Je tøeba správnì rozli¹ovat, kdy pou¾ít dereferenèní operátor *
** (typicky pøi modifikaci) a kdy budeme pracovat pouze se samotným ukazatelem 
** (napø. pøi vyhledávání). V tomto pøíkladu vám napoví prototypy funkcí.
** Pokud pracujeme s ukazatelem na ukazatel, pou¾ijeme dereferenci.
**/

#include "c401.h"
int solved;

void BSTInit (tBSTNodePtr *RootPtr) {
/*   -------
** Funkce provede poèáteèní inicializaci stromu pøed jeho prvním pou¾itím.
**
** Ovìøit, zda byl ji¾ strom pøedaný pøes RootPtr inicializován, nelze,
** proto¾e pøed první inicializací má ukazatel nedefinovanou (tedy libovolnou)
** hodnotu. Programátor vyu¾ívající ADT BVS tedy musí zajistit, aby inicializace
** byla volána pouze jednou, a to pøed vlastní prací s BVS. Provedení
** inicializace nad neprázdným stromem by toti¾ mohlo vést ke ztrátì pøístupu
** k dynamicky alokované pamìti (tzv. "memory leak").
**	
** V¹imnìte si, ¾e se v hlavièce objevuje typ ukazatel na ukazatel.	
** Proto je tøeba pøi pøiøazení pøes RootPtr pou¾ít dereferenèní operátor *.
** Ten bude pou¾it i ve funkcích BSTDelete, BSTInsert a BSTDispose.
**/

	*RootPtr = NULL; 										//Inicializujeme tak ze Korenovy bude mat hodnotu NULL,nebude mat ziadne kluce, data atd.
		return;
	
}	

int BSTSearch (tBSTNodePtr RootPtr, char K, int *Content)	{
/*  ---------
** Funkce vyhledá uzel v BVS s klíèem K.
**
** Pokud je takový nalezen, vrací funkce hodnotu TRUE a v promìnné Content se
** vrací obsah pøíslu¹ného uzlu.´Pokud pøíslu¹ný uzel není nalezen, vrací funkce
** hodnotu FALSE a obsah promìnné Content není definován (nic do ní proto
** nepøiøazujte).
**
** Pøi vyhledávání v binárním stromu bychom typicky pou¾ili cyklus ukonèený
** testem dosa¾ení listu nebo nalezení uzlu s klíèem K. V tomto pøípadì ale
** problém øe¹te rekurzivním volání této funkce, pøièem¾ nedeklarujte ¾ádnou
** pomocnou funkci.
**/
							   
	if(RootPtr == NULL)										//Ak je korenovy NULL, tak sa nic neda najst, teda vracia FALSE
		return FALSE;
	
	else
	{

		if(RootPtr->Key == K) 								//Ak sa kluce rovnaju, je zhoda, nasli sme, vrat TRUE a este daj do premennej Content to co si nasiel
		{
			*Content = RootPtr->BSTNodeCont;
			return TRUE;
		}

		else
		{
			if(RootPtr->Key > K)							//Ak je kluc mensi tak sa posun dolava a rekurzivne volaj tuto funkciu...vlastne az potial ked nenajdes zhodu
			{ 
				RootPtr=RootPtr->LPtr;
				return BSTSearch(RootPtr, K, Content);	
			}
			else											//Ak teda (RootPtr->Key < K) -->> Ak je kluc vacsi tak sa posun dolava a rekurzivne volaj tuto funkciu...vlastne az potial ked nenajdes zhodu	
			{
				RootPtr=RootPtr->RPtr;							 
				return BSTSearch(RootPtr, K, Content);
			}
		}
	}
} 


void BSTInsert (tBSTNodePtr* RootPtr, char K, int Content)	{	
/*   ---------
** Vlo¾í do stromu RootPtr hodnotu Content s klíèem K.
**
** Pokud ji¾ uzel se zadaným klíèem ve stromu existuje, bude obsah uzlu
** s klíèem K nahrazen novou hodnotou. Pokud bude do stromu vlo¾en nový
** uzel, bude vlo¾en v¾dy jako list stromu.
**
** Funkci implementujte rekurzivnì. Nedeklarujte ¾ádnou pomocnou funkci.
**
** Rekurzivní implementace je ménì efektivní, proto¾e se pøi ka¾dém
** rekurzivním zanoøení ukládá na zásobník obsah uzlu (zde integer).
** Nerekurzivní varianta by v tomto pøípadì byla efektivnìj¹í jak z hlediska
** rychlosti, tak z hlediska pamì»ových nárokù. Zde jde ale o ¹kolní
** pøíklad, na kterém si chceme ukázat eleganci rekurzivního zápisu.
**/
		
	if (*RootPtr == NULL)								//Vlozime do prazdneho stromu
	{
		tBSTNodePtr PomVar = malloc(sizeof(struct tBSTNode));

		if(PomVar == NULL)								//Ak sa nepodari alokacia tak nepokracuj, vyjdi z funkcie
			return;

		else if(PomVar != NULL)							//Ak sa podari, tak do prazdneho stromu priradime to co sme alokovali, a predali ako parametry funkcie
		{
			(*RootPtr) = PomVar;
			(*RootPtr)->Key = K;
			(*RootPtr)->BSTNodeCont = Content;
			(*RootPtr)->LPtr = NULL;					//Co lavych a pravych uzlov budu mat hodnotu NULL, lebo niesu vytvorene
			(*RootPtr)->RPtr = NULL;
		}
	}
	else
	{
		if((*RootPtr)->Key > K)							//Ak strom nieje prazdny tak rekurzivne volam tuto funkciu, pricom "korenovy" pointer bude zmeneny, bud nalavo alebo napravo, podla kluca
			BSTInsert(&(*RootPtr)->LPtr, K, Content);

		else if((*RootPtr)->Key < K)
			BSTInsert(&(*RootPtr)->RPtr, K, Content);

		else if((*RootPtr)->Key == K)					//Ak sa kluce rovnaju, tak iba prepis obsah uzlu
			(*RootPtr)->BSTNodeCont = Content;

	}

}

void ReplaceByRightmost (tBSTNodePtr PtrReplaced, tBSTNodePtr *RootPtr) {
/*   ------------------
** Pomocná funkce pro vyhledání, pøesun a uvolnìní nejpravìj¹ího uzlu.
**
** Ukazatel PtrReplaced ukazuje na uzel, do kterého bude pøesunuta hodnota
** nejpravìj¹ího uzlu v podstromu, který je urèen ukazatelem RootPtr.
** Pøedpokládá se, ¾e hodnota ukazatele RootPtr nebude NULL (zajistìte to
** testováním pøed volání této funkce). Tuto funkci implementujte rekurzivnì. 
**
** Tato pomocná funkce bude pou¾ita dále. Ne¾ ji zaènete implementovat,
** pøeètìte si komentáø k funkci BSTDelete(). 
**/
	if((*RootPtr)->RPtr == NULL)						//Ako 2. parameter sme dostali lavy uzol, a teraz hladame najpravsi. Najdeme ho tak, ze ukazatel na jeho pravy uzol bude NULL, cize je najpravsi
	{
		tBSTNodePtr PomUk = (*RootPtr);					//vytvorime si novy pomocny ukazatel, priradime donho koren

		PtrReplaced->Key = (*RootPtr)->Key;				//korenovy kluc aj obsah dame do 1.parametru, teda v podstate nahradime najpravsi co sme nasli za ten co neskor zrusime. 
		PtrReplaced->BSTNodeCont = (*RootPtr)->BSTNodeCont;

		(*RootPtr) = PomUk->LPtr;

		free(PomUk);									//zrusime najpravsi. uz nieje treba, presunul sa do toho ktory budeme rusit

	}

	else
		ReplaceByRightmost(PtrReplaced, &(*RootPtr)->RPtr);	//Popisane v 1. riadku, rekurzivne volam tuto funkciu a parameter bude vzdy dalsi pravy uzol (bude sa ukladat ako root)


}

void BSTDelete (tBSTNodePtr *RootPtr, char K) 		{
/*   ---------
** Zru¹í uzel stromu, který obsahuje klíè K.
**
** Pokud uzel se zadaným klíèem neexistuje, nedìlá funkce nic. 
** Pokud má ru¹ený uzel jen jeden podstrom, pak jej zdìdí otec ru¹eného uzlu.
** Pokud má ru¹ený uzel oba podstromy, pak je ru¹ený uzel nahrazen nejpravìj¹ím
** uzlem levého podstromu. Pozor! Nejpravìj¹í uzel nemusí být listem.
**
** Tuto funkci implementujte rekurzivnì s vyu¾itím døíve deklarované
** pomocné funkce ReplaceByRightmost.
**/
	
	if((*RootPtr) == NULL)								//Korenovy je prazdny, nerob nic vyjdi z funkcie
		return;
	else
	{
		if((*RootPtr)->Key < K)							//musime najst ten co chceme rusit, najdeme podla kluca, princip podobny ako v BSTSearch, porovnavanim klucov.
		{
			BSTDelete(&(*RootPtr)->RPtr, K);			//ak je kluc vacsi tak zavolame rekurzivne tuto funkciu a korenovy bude ten co sa nachadzal napravo 
		}
		else if((*RootPtr)->Key > K)
		{
			BSTDelete(&(*RootPtr)->LPtr, K);			//symetricke s ((*RootPtr)->Key < K)
		}
		else if((*RootPtr)->Key == K)					//kluce sa zhoduju, nasli sme prvok ktory ideme odstranit
		{
			tBSTNodePtr PomUk = (*RootPtr);				//definujeme si pomocnu premennu a vlozime do nej sucasny korenovy uzol

			if((*RootPtr)->RPtr == NULL)				//Ak existuje iba jeden podstrom, lavy, tak ho zdedi otec ruseneho , a nasledne mozeme rusit
			{
				(*RootPtr) = PomUk->LPtr;
				free(PomUk);
			}
			else if((*RootPtr)->LPtr == NULL) 			//symetricke k predchadzajucej podmienke
			{
				(*RootPtr) = PomUk->RPtr;
				free(PomUk);
			}
			else
				ReplaceByRightmost((*RootPtr), &(*RootPtr)->LPtr);  //Ak ma ruseny 2 podstromy, tak sa postupuje trosku inak, vyhlada sa najpravsi z laveho podstromu a ten sa nahradi za ruseny
		}
	}

} 

void BSTDispose (tBSTNodePtr *RootPtr) {	
/*   ----------
** Zru¹í celý binární vyhledávací strom a korektnì uvolní pamì».
**
** Po zru¹ení se bude BVS nacházet ve stejném stavu, jako se nacházel po
** inicializaci. Tuto funkci implementujte rekurzivnì bez deklarování pomocné
** funkce.
**/
	
	if((*RootPtr) == NULL)								//Ak je strom prazdny tak sa nic  nedeje
		return;
	else 												//ale ak nieje, tak ideme rusit :)
	{
		if((*RootPtr)->LPtr != NULL)					//ak ma lavy podstrom tak rekurzivne zavolame tuto funkciu, pricom bude ako parameter lavy podstrom, teda koren sa posunie dolava
		{
			(*RootPtr)=(*RootPtr)->LPtr;
			BSTDispose(RootPtr);
		}
		else if(((*RootPtr)->RPtr != NULL))				//symetricke s predchadzajucou podmienkou
		{
			(*RootPtr)=(*RootPtr)->RPtr;
			BSTDispose(RootPtr);
		}	
	}
	free(*RootPtr);										//Korenovy (v tomto pripade korenove) sa uvolnuju rekurzivne..pokial uz nieje ziadny podstrom 
	(*RootPtr) = NULL;									//nakoniec dame strom do pociatocneho stavu, teda ako sme ho inicializovali. Korenovy = NULL.
}

/* konec c401.c */

