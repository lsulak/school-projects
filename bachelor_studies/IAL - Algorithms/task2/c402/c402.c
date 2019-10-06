
/* c402.c: ********************************************************************}
{* Téma: Nerekurzivní implementace operací nad BVS 
**                                     Implementace: Petr Pøikryl, prosinec 1994
**                                           Úpravy: Petr Pøikryl, listopad 1997
**                                                     Petr Pøikryl, kvìten 1998
**			  	                        Pøevod do jazyka C: Martin Tuèek, srpen 2005
**                                         Úpravy: Bohuslav Køena, listopad 2009
**                                         Upravy: Karel Masarik, rijen 2013
**
** S vyu¾itím dynamického pøidìlování pamìti, implementujte NEREKURZIVNÌ
** následující operace nad binárním vyhledávacím stromem (pøedpona BT znamená
** Binary Tree a je u identifikátorù uvedena kvùli mo¾né kolizi s ostatními
** pøíklady):
**
**     BTInit .......... inicializace stromu
**     BTInsert ........ nerekurzivní vlo¾ení nového uzlu do stromu
**     BTPreorder ...... nerekurzivní prùchod typu pre-order
**     BTInorder ....... nerekurzivní prùchod typu in-order
**     BTPostorder ..... nerekurzivní prùchod typu post-order
**     BTDisposeTree ... zru¹ v¹echny uzly stromu
**
** U v¹ech funkcí, které vyu¾ívají nìkterý z prùchodù stromem, implementujte
** pomocnou funkci pro nalezení nejlevìj¹ího uzlu v podstromu.
**
** Pøesné definice typù naleznete v souboru c402.h. Uzel stromu je typu tBTNode,
** ukazatel na nìj je typu tBTNodePtr. Jeden uzel obsahuje polo¾ku int Cont,
** která souèasnì slou¾í jako u¾iteèný obsah i jako vyhledávací klíè 
** a ukazatele na levý a pravý podstrom (LPtr a RPtr).
**
** Pøíklad slou¾í zejména k procvièení nerekurzivních zápisù algoritmù
** nad stromy. Ne¾ zaènete tento pøíklad øe¹it, prostudujte si dùkladnì
** principy pøevodu rekurzivních algoritmù na nerekurzivní. Programování
** je pøedev¹ím in¾enýrská disciplína, kde opìtné objevování Ameriky nemá
** místo. Pokud se Vám zdá, ¾e by nìco ¹lo zapsat optimálnìji, promyslete
** si v¹echny detaily Va¹eho øe¹ení. Pov¹imnìte si typického umístìní akcí
** pro rùzné typy prùchodù. Zamyslete se nad modifikací øe¹ených algoritmù
** napøíklad pro výpoèet poètu uzlù stromu, poètu listù stromu, vý¹ky stromu
** nebo pro vytvoøení zrcadlového obrazu stromu (pouze popøehazování ukazatelù
** bez vytváøení nových uzlù a ru¹ení starých).
**
** Pøi prùchodech stromem pou¾ijte ke zpracování uzlu funkci BTWorkOut().
** Pro zjednodu¹ení práce máte pøedem pøipraveny zásobníky pro hodnoty typu
** bool a tBTNodePtr. Pomocnou funkci BTWorkOut ani funkce pro práci
** s pomocnými zásobníky neupravujte 
** Pozor! Je tøeba správnì rozli¹ovat, kdy pou¾ít dereferenèní operátor *
** (typicky pøi modifikaci) a kdy budeme pracovat pouze se samotným ukazatelem 
** (napø. pøi vyhledávání). V tomto pøíkladu vám napoví prototypy funkcí.
** Pokud pracujeme s ukazatelem na ukazatel, pou¾ijeme dereferenci.
**/

#include "c402.h"
int solved;

void BTWorkOut (tBTNodePtr Ptr)		{
/*   ---------
** Pomocná funkce, kterou budete volat pøi prùchodech stromem pro zpracování
** uzlu urèeného ukazatelem Ptr. Tuto funkci neupravujte.
**/
			
	if (Ptr==NULL) 
    printf("Chyba: Funkce BTWorkOut byla volána s NULL argumentem!\n");
  else 
    printf("Výpis hodnoty daného uzlu> %d\n",Ptr->Cont);
}
	
/* -------------------------------------------------------------------------- */
/*
** Funkce pro zásobník hotnot typu tBTNodePtr. Tyto funkce neupravujte.
**/

void SInitP (tStackP *S)  
/*   ------
** Inicializace zásobníku.
**/
{
	S->top = 0;  
}	

void SPushP (tStackP *S, tBTNodePtr ptr)
/*   ------
** Vlo¾í hodnotu na vrchol zásobníku.
**/
{ 
                 /* Pøi implementaci v poli mù¾e dojít k pøeteèení zásobníku. */
  if (S->top==MAXSTACK) 
    printf("Chyba: Do¹lo k pøeteèení zásobníku s ukazateli!\n");
  else {  
		S->top++;  
		S->a[S->top]=ptr;
	}
}	

tBTNodePtr STopPopP (tStackP *S)
/*         --------
** Odstraní prvek z vrcholu zásobníku a souèasnì vrátí jeho hodnotu.
**/
{
                            /* Operace nad prázdným zásobníkem zpùsobí chybu. */
	if (S->top==0)  {
		printf("Chyba: Do¹lo k podteèení zásobníku s ukazateli!\n");
		return(NULL);	
	}	
	else {
		return (S->a[S->top--]);
	}	
}

bool SEmptyP (tStackP *S)
/*   -------
** Je-li zásobník prázdný, vrátí hodnotu true.
**/
{
  return(S->top==0);
}	

/* -------------------------------------------------------------------------- */
/*
** Funkce pro zásobník hotnot typu bool. Tyto funkce neupravujte.
*/

void SInitB (tStackB *S) {
/*   ------
** Inicializace zásobníku.
**/

	S->top = 0;  
}	

void SPushB (tStackB *S,bool val) {
/*   ------
** Vlo¾í hodnotu na vrchol zásobníku.
**/
                 /* Pøi implementaci v poli mù¾e dojít k pøeteèení zásobníku. */
	if (S->top==MAXSTACK) 
		printf("Chyba: Do¹lo k pøeteèení zásobníku pro boolean!\n");
	else {
		S->top++;  
		S->a[S->top]=val;
	}	
}

bool STopPopB (tStackB *S) {
/*   --------
** Odstraní prvek z vrcholu zásobníku a souèasnì vrátí jeho hodnotu.
**/
                            /* Operace nad prázdným zásobníkem zpùsobí chybu. */
	if (S->top==0) {
		printf("Chyba: Do¹lo k podteèení zásobníku pro boolean!\n");
		return(NULL);	
	}	
	else {  
		return(S->a[S->top--]); 
	}	
}

bool SEmptyB (tStackB *S) {
/*   -------
** Je-li zásobník prázdný, vrátí hodnotu true.
**/
  return(S->top==0);
}

/* -------------------------------------------------------------------------- */
/*
** Následuje jádro domácí úlohy - funkce, které máte implementovat. 
*/

void BTInit (tBTNodePtr *RootPtr)	{
/*   ------
** Provede inicializaci binárního vyhledávacího stromu.
**
** Inicializaci smí programátor volat pouze pøed prvním pou¾itím binárního
** stromu, proto¾e neuvolòuje uzly neprázdného stromu (a ani to dìlat nemù¾e,
** proto¾e pøed inicializací jsou hodnoty nedefinované, tedy libovolné).
** Ke zru¹ení binárního stromu slou¾í procedura BTDisposeTree.
**	
** V¹imnìte si, ¾e zde se poprvé v hlavièce objevuje typ ukazatel na ukazatel,	
** proto je tøeba pøi práci s RootPtr pou¾ít dereferenèní operátor *.
**/
	
	(*RootPtr) = NULL;	////Inicializujeme tak ze Korenovy bude mat hodnotu NULL

}

void BTInsert (tBTNodePtr *RootPtr, int Content) {
/*   --------
** Vlo¾í do stromu nový uzel s hodnotou Content.
**
** Z pohledu vkládání chápejte vytváøený strom jako binární vyhledávací strom,
** kde uzly s hodnotou men¹í ne¾ má otec le¾í v levém podstromu a uzly vìt¹í
** le¾í vpravo. Pokud vkládaný uzel ji¾ existuje, neprovádí se nic (daná hodnota
** se ve stromu mù¾e vyskytnout nejvý¹e jednou). Pokud se vytváøí nový uzel,
** vzniká v¾dy jako list stromu. Funkci implementujte nerekurzivnì.
**/
	
	if((*RootPtr) == NULL) 									//Vkladame do prazdneho stromu
	{
		tBTNodePtr PomVar = malloc(sizeof(struct tBTNode));	//alokujeme novy prvok
		
		if(PomVar == NULL)									//ak sa nepodari alokacia
			return;

		(*RootPtr) = PomVar;								//prave vytvorena hodnota bude korenovy uzol, nacita sa donho obsah
		(*RootPtr)->Cont = Content;
		(*RootPtr)->LPtr = NULL;							//Co lavych a pravych uzlov budu mat hodnotu NULL, lebo niesu vytvorene
		(*RootPtr)->RPtr = NULL;
			
	}
	
	else													//Vkladame do neprazdneho stromu
	{
		tBTNodePtr OUzol = NULL;							//OUzol je otcovsky uzol, na zac. hodnota NULL
		tBTNodePtr PomUz = (*RootPtr);						//PomUz - v nom bude ulozeny korenovy uzol, ten sa nezmeni, a s korenovym ulozenym v pom. premennej mozeme dalej pracovat

		while(PomUz != NULL)								
		{
			if(PomUz->Cont > Content)						//Ak bude sucasny korenovy vacsi ako obsah co hladame...
			{	
				OUzol = PomUz;								//Ulozime si sucasny korenovy do Otcovskeho, a zmenime korenovy na ten co je vlavo
				PomUz = PomUz->LPtr;
			}

			else if(PomUz->Cont < Content)					//Symetricke s predch.
			{
				OUzol = PomUz;
				PomUz = PomUz->RPtr;
			}
			else return;									//Inak ukonci
		}
		
	
		tBTNodePtr PomVar = malloc(sizeof(struct tBTNode));	//Alokujeme novy prvok

		if(PomVar == NULL)
			return;

		PomVar->Cont = Content;								//ulozime si do nove vytvoreneho obsah
		PomVar->LPtr = NULL;								//Co lavych a pravych uzlov budu mat hodnotu NULL, lebo niesu vytvorene
		PomVar->RPtr = NULL;
		
		if(OUzol->Cont < Content)							//Ak otcovsky obsah je mensi , novu hodnotu vlozime do toho kt. je pravym poduzlom otcovskeho
			OUzol->RPtr	= PomVar;

		else if(OUzol->Cont > Content)						//symetricke s predchadzajucim
			OUzol->LPtr	= PomVar;
	}

}

/*                                  PREORDER                                  */

void Leftmost_Preorder (tBTNodePtr ptr, tStackP *Stack)	{
/*   -----------------
** Jde po levì vìtvi podstromu, dokud nenarazí na jeho nejlevìj¹í uzel.
**
** Pøi prùchodu Preorder nav¹tívené uzly zpracujeme voláním funkce BTWorkOut()
** a ukazatele na nì is ulo¾íme do zásobníku.
**/

	while(ptr != NULL)			//pokial nebude nalavo NULL, teda az po najlavejsi
	{
		BTWorkOut(ptr);			//Postupne vypisuj a ukladaj na zasobnik.
		SPushP(Stack, ptr);
		ptr = ptr->LPtr; 		//Posun sa o dalsi lavu poduzol, znovu vypis pushni atd.

	}
	
}

void BTPreorder (tBTNodePtr RootPtr)	{
/*   ----------
** Prùchod stromem typu preorder implementovaný nerekurzivnì s vyu¾itím funkce
** Leftmost_Preorder a zásobníku ukazatelù. Zpracování jednoho uzlu stromu
** realizujte jako volání funkce BTWorkOut(). 
**/

	tStackP Stack;						//Pomocny zasobnik
	tBTNodePtr PomUz = RootPtr;
	SInitP(&Stack);

	Leftmost_Preorder(RootPtr, &Stack); //Napln zasobnik a vypis vsetky lave poduzly

	while(!SEmptyP(&Stack))				//Pokial nebude zasobnik prazdny....
	{
		PomUz = STopPopP(&Stack);		//z vrcholu odstran prvok a daj ho do pomocnej premennej
		PomUz = PomUz->RPtr;			//ulozime si pravy poduzol
		
		Leftmost_Preorder(PomUz, &Stack); //znovu (s pravym poduzlom) hladame najlavejsi, a postupne vypisujeme a ukladame na zasobnik...a takto dalej
	}

}


/*                                  INORDER                                   */ 

void Leftmost_Inorder(tBTNodePtr ptr, tStackP *Stack)		{
/*   ----------------
** Jde po levì vìtvi podstromu, dokud nenarazí na jeho nejlevìj¹í uzel.
**
** Pøi prùchodu Inorder ukládáme ukazatele na v¹echny nav¹tívené uzly do
** zásobníku. 
**/
		
	while(ptr != NULL)				//pokial nebude nalavo NULL, teda az po najlavejsi
	{
		SPushP(Stack, ptr);			//Uloz prvok na zasobnik, ale nevypisuj, to az neskor
		ptr = ptr->LPtr; 			//ulozime si dalsi lavy poduzol

	}
	
}

void BTInorder (tBTNodePtr RootPtr)	{
/*   ---------
** Prùchod stromem typu inorder implementovaný nerekurzivnì s vyu¾itím funkce
** Leftmost_Inorder a zásobníku ukazatelù. Zpracování jednoho uzlu stromu
** realizujte jako volání funkce BTWorkOut(). 
**/
	tStackP Stack;				     //Pomocny zasobnik...aj premenna v kt. bude ulozena hodnota korenoveho uzlu
	tBTNodePtr PomUz = RootPtr;

	SInitP(&Stack);
	Leftmost_Inorder(PomUz, &Stack); //Naplnime zasobnik lavymi poduzlami

	while(!SEmptyP(&Stack))			 //Pokial nebude zasobnik prazdny
	{
		PomUz = STopPopP(&Stack);	 //ulozime si do pomocnej premennej to co je na vrcholu zasobniku (prepiseme sucasne koren, kt. sa tam nachadzal)
		BTWorkOut(PomUz);			 //vypiseme uzol zo zasobniku
	
		if(PomUz->RPtr != NULL)		 //napln zasobnik najlevejsimi uzlami praveho podstromu
			Leftmost_Inorder(PomUz->RPtr, &Stack);
		
	}

}

/*                                 POSTORDER                                  */ 

void Leftmost_Postorder (tBTNodePtr ptr, tStackP *StackP, tStackB *StackB) {
/*           --------
** Jde po levì vìtvi podstromu, dokud nenarazí na jeho nejlevìj¹í uzel.
**
** Pøi prùchodu Postorder ukládáme ukazatele na nav¹tívené uzly do zásobníku
** a souèasnì do zásobníku bool hodnot ukládáme informaci, zda byl uzel
** nav¹tíven poprvé a ¾e se tedy je¹tì nemá zpracovávat. 
**/
	while(ptr != NULL) 				//pokial nebude najlevejsi uzol laveho podstrom nulovy...
	{
		SPushP(StackP, ptr);		//...ukladaj postupne na zasobnik..
		SPushB(StackB, FALSE);		//...a prirad k nim hodnotu FALSE - este ich teda nespracuvaj
		ptr = ptr->LPtr;
	}

}

void BTPostorder (tBTNodePtr RootPtr)	{
/*           -----------
** Prùchod stromem typu postorder implementovaný nerekurzivnì s vyu¾itím funkce
** Leftmost_Postorder, zásobníku ukazatelù a zásobníku hotdnot typu bool.
** Zpracování jednoho uzlu stromu realizujte jako volání funkce BTWorkOut(). 
**/
	tStackP StackP;					//2 pomocne zasobniky, jeden pre pravdivostne hodnoty - ci mame uz spracuvat alebo nie, a druhy pre prvky stromu (uzly)
	tStackB StackB;
	tBTNodePtr PomUz = RootPtr;

	SInitP(&StackP);
	SInitB(&StackB);
	Leftmost_Postorder(PomUz, &StackP, &StackB); //Naplnime zasobnik vsetkymi najlevejsimi uzlami laveho podstromu
	
	while(!SEmptyP(&StackP))					//Cyklus bezi az po vyprazdnenie zasobniku, teda az pokial nebude na vystupe posledna hodnota
	{
		PomUz = STopPopP(&StackP);				//uloz si hodnotu do pomocneho prvku. zial nemame oddelene implementovane funkcie pop a top...
		SPushP(&StackP, PomUz);					//...preto musime znovu vlozit prvok na zasobnik

		if(!STopPopB(&StackB))					//Ak sme este len raz navstivili uzol, teda vracia hodnotu FALSE , !FALSE = true, teda pojdeme k prvej casti podmien. prikazu
		{
			SPushB(&StackB, TRUE);				//vlozime hodnotu TRUE na bool zasobnik, to znamena ze uz ho najblizsie spracuvame , pretoze uz raz navstiveny bol 
			Leftmost_Postorder(PomUz->RPtr, &StackP, &StackB);	//Hned za tym vlozime na zasobnik prave uzly 
			continue;							//Cyklus ideme od zaciatku, znovu skontrolujeme uzol , ak mame spracuvat pokracujeme 2.vetvou, ak nie(napr. ze je na zas. pravy uzol) tak 1. vetvou
		}
			STopPopP(&StackP);					//uzol sme uz raz navstivili, takze ho dame pred zo zasobniku 
			BTWorkOut(PomUz);					//Vypiseme na vystup uzol , kt. sme naposledy pushli na zasobnik
	}
}


void BTDisposeTree (tBTNodePtr *RootPtr)	{
/*   -------------
** Zru¹í v¹echny uzly stromu a korektnì uvolní jimi zabranou pamì».
**
** Funkci implementujte nerekurzivnì s vyu¾itím zásobníku ukazatelù.
**/

	
	if((*RootPtr) == NULL)							//Ak uz je korenovy NULL, nic nerob
		return;			
	else
	{
		tStackP StackP;								//Pomocny zasobnik,pomocou neho sa bude rusit
		SInitP(&StackP);
		tBTNodePtr PomUz = (*RootPtr);				//pomocne premenne
		tBTNodePtr PomUz2 = NULL;

		while(1)									//bezi nekonecny cyklus
		{
			if(!SEmptyP(&StackP))	//ak zasobnik nieje prazdny
			{
				PomUz = STopPopP(&StackP);			//do pomocneho uzlu si vlozime hodnotu zo zasobniku , v cykle uz nic nerobime , pokracujeme dalsou iteraciou
				continue;
			}
			else if(PomUz == NULL) 					//cyklus sa ukonci ,az ked bude sucasny korenovy uzol prazdny
				break;

			PomUz2 = PomUz;
			PomUz = PomUz->LPtr;
			free(PomUz2);
		}

		(*RootPtr) = NULL; 							//Po zruseni celeho stromu proved to iste co pri inicializacii, teda nastav korenovy na NULL
	}
}

/* konec c402.c */

