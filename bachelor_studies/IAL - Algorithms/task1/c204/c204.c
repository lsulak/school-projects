
/* ******************************* c204.c *********************************** */
/*  Pøedmìt: Algoritmy (IAL) - FIT VUT v Brnì                                 */
/*  Úkol: c204 - Pøevod infixového výrazu na postfixový (s vyu¾itím c202)     */
/*  Referenèní implementace: Petr Pøikryl, listopad 1994                      */
/*  Pøepis do jazyka C: Luká¹ Mar¹ík, prosinec 2012                           */
/*  Upravil: Bohuslav Køena, øíjen 2013                                       */
/* ************************************************************************** */
/*
** Implementujte proceduru pro pøevod infixového zápisu matematického
** výrazu do postfixového tvaru. Pro pøevod vyu¾ijte zásobník (tStack),
** který byl implementován v rámci pøíkladu c202. Bez správného vyøe¹ení
** pøíkladu c202 se o øe¹ení tohoto pøíkladu nepokou¹ejte.
**
** Implementujte následující funkci:
**
**    infix2postfix .... konverzní funkce pro pøevod infixového výrazu 
**                       na postfixový
**
** Pro lep¹í pøehlednost kódu implementujte následující funkce:
**    
**    untilLeftPar .... vyprázdnìní zásobníku a¾ po levou závorku
**    doOperation .... zpracování operátoru konvertovaného výrazu
**
** Své øe¹ení úèelnì komentujte.
**
** Terminologická poznámka: Jazyk C nepou¾ívá pojem procedura.
** Proto zde pou¾íváme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**
**/

#include "c204.h"

int solved;


/*
** Pomocná funkce untilLeftPar.
** Slou¾í k vyprázdnìní zásobníku a¾ po levou závorku, pøièem¾ levá závorka
** bude také odstranìna. Pokud je zásobník prázdný, provádìní funkce se ukonèí.
**
** Operátory odstraòované ze zásobníku postupnì vkládejte do výstupního pole
** znakù postExpr. Délka pøevedeného výrazu a té¾ ukazatel na první volné
** místo, na které se má zapisovat, pøedstavuje parametr postLen.
**
** Aby se minimalizoval poèet pøístupù ke struktuøe zásobníku, mù¾ete zde
** nadeklarovat a pou¾ívat pomocnou promìnnou typu.
*/
void untilLeftPar ( tStack* s, char* postExpr, unsigned* postLen ) {
	char PomRet = '\0';							//Pomocna premenna, na zac. ma hodnotu '\0' 
	if(!stackEmpty(s))		
	{
	
		while(s->arr[s->top] != '(')			//Az po lavu zatvorku vkladame prvky z vrcholu zasobniku do vystupneho retazca
		{
			stackTop(s, &PomRet);
			stackPop(s);
			postExpr[*postLen] = PomRet;		//priradenie do vystupneho retazca
			(*postLen)++ ; 						//inkrementacia indexu na kt. bude vystupny znak retazca

		}
	stackPop(s);								//Lavu zatvorku odstranime zo zasobniku, ale neukladame ju do vystupneho retazca.
	}
}

/*
** Pomocná funkce doOperation.
** Zpracuje operátor, který je pøedán parametrem c po naètení znaku ze
** vstupního pole znakù.
**
** Dle priority pøedaného operátoru a pøípadnì priority operátoru na
** vrcholu zásobníku rozhodneme o dal¹ím postupu. Délka pøevedeného 
** výrazu a takté¾ ukazatel na první volné místo, do kterého se má zapisovat, 
** pøedstavuje parametr postLen, výstupním polem znakù je opìt postExpr.
*/
void doOperation ( tStack* s, char c, char* postExpr, unsigned* postLen ) {
	
		if(c == '+' || c == '-' ) 						 //Funkcia sa deli na 2 podobne casti, pre prehladnost. Tato, prva , spracuva operatory + a - , lebo maju nizsiu prioritu
		{			
			if(stackEmpty(s)) 							//Ak je zasobnik prazdny nic sa nemusi kontrolovat, jednoducho sa operator vlozi na zasobnik
				stackPush(s, c);

			else
			{
				if(s->arr[s->top] == '(') 				//Ak je na vrchole zasobniku lava zatvorka, tiez sa len vlozi na zasobnik
					stackPush(s, c);

				else if(s->arr[s->top] == '+' || s->arr[s->top] == '-' || s->arr[s->top] == '*' || s->arr[s->top] == '/')
				{										//Ak su na zasobniku operatory s rovnakou prioritou alebo vyssou, cize * / + - ,
					stackTop(s, &postExpr[*postLen]);	//tak sa z vrcholu zasobnika ulozi operator do vystupneho retazca
					(*postLen)++;						//inkrementuj index kde sa bude zapisovat dalsia polozka
					stackPop(s);						//vrchol sa posunie dalej
					stackPush(s, c);				
				}
			}

		}

		else if(c == '/' || c == '*')						//Funkcia sa deli na 2 podobne casti, pre prehladnost. Tato, druha , spracuva operatory / a * , lebo maju vacsiu prioritu
		{

			if(stackEmpty(s))								//Ak je zasobnik prazdny nic sa nemusi kontrolovat, jednoducho sa operator vlozi na zasobnik
				stackPush(s, c);

			else
			{ 
				if(s->arr[s->top] == '(' || s->arr[s->top] == '+' || s->arr[s->top] == '-')  //Ak je na vrchole zasobniku lava zatvorka,
					stackPush(s, c);														//alebo operatory s nizsou prioritou teda + a -, tiez sa len vlozi na zasobnik

				else if(s->arr[s->top] == '*' || s->arr[s->top] == '/')						//Ak su na zasobniku operatory s rovnakou prioritou , cize * a / ,
				{
					stackTop(s, &postExpr[*postLen]);										//tak sa z vrcholu zasobnika ulozi operator do vystupneho retazca
					(*postLen)++;															//inkrementuj index kde sa bude zapisovat dalsia polozka
					stackPop(s);															//vrchol sa posunie dalej
					stackPush(s, c);
				}
			}

		}
}

/* 
** Konverzní funkce infix2postfix.
** Ète infixový výraz ze vstupního øetìzce infExpr a generuje
** odpovídající postfixový výraz do výstupního øetìzce (postup pøevodu
** hledejte v pøedná¹kách nebo ve studijní opoøe). Pamì» pro výstupní øetìzec
** (o velikosti MAX_LEN) je tøeba alokovat. Volající funkce, tedy
** pøíjemce konvertovaného øetìzce, zajistí korektní uvolnìní zde alokované
** pamìti.
**
** Tvar výrazu:
** 1. Výraz obsahuje operátory + - * / ve významu sèítání, odèítání,
**    násobení a dìlení. Sèítání má stejnou prioritu jako odèítání,
**    násobení má stejnou prioritu jako dìlení. Priorita násobení je
**    vìt¹í ne¾ priorita sèítání. V¹echny operátory jsou binární
**    (neuva¾ujte unární mínus).
**
** 2. Hodnoty ve výrazu jsou reprezentovány jednoznakovými identifikátory
**    a èíslicemi - 0..9, a..z, A..Z (velikost písmen se rozli¹uje).
**
** 3. Ve výrazu mù¾e být pou¾it pøedem neurèený poèet dvojic kulatých
**    závorek. Uva¾ujte, ¾e vstupní výraz je zapsán správnì (neo¹etøujte
**    chybné zadání výrazu).
**
** 4. Ka¾dý korektnì zapsaný výraz (infixový i postfixový) musí být uzavøen 
**    ukonèovacím znakem '='.
**
** 5. Pøi stejné prioritì operátorù se výraz vyhodnocuje zleva doprava.
**
** Poznámky k implementaci
** -----------------------
** Jako zásobník pou¾ijte zásobník znakù tStack implementovaný v pøíkladu c202. 
** Pro práci se zásobníkem pak pou¾ívejte výhradnì operace z jeho rozhraní.
**
** Pøi implementaci vyu¾ijte pomocné funkce untilLeftPar a doOperation.
**
** Øetìzcem (infixového a postfixového výrazu) je zde my¹leno pole znakù typu
** char, jen¾ je korektnì ukonèeno nulovým znakem dle zvyklostí jazyka C.
**
** Na vstupu oèekávejte pouze korektnì zapsané a ukonèené výrazy. Jejich délka
** nepøesáhne MAX_LEN-1 (MAX_LEN i s nulovým znakem) a tedy i výsledný výraz
** by se mìl vejít do alokovaného pole. Po alokaci dynamické pamìti si v¾dycky
** ovìøte, ¾e se alokace skuteènì zdraøila. V pøípadì chyby alokace vra»te namísto
** øetìzce konstantu NULL.
*/
char* infix2postfix (const char* infExpr) {
	tStack s;
	int i = 0; 												//Pocitadlo pre umiestnenie vstupneho prvku
	unsigned j = 0;											//Pocitadlo pre umiestnenie vystupneho prvku

	char* postExpr = (char*) malloc(MAX_LEN*sizeof(char));	//Alokovanie retazca
	stackInit(&s);											//Inicializacia zasobnika

	if(postExpr == NULL)									//Ak sa nepodari alokacia...
	{
		return NULL;
	}

	while(infExpr[i] != '\0')								//Spracovavaj vstupny retazec az po posledny znak, teda ukoncovaciu \0
	{														//Ak je znak operator, spracuva sa tymto sposobom...funkciou doOperation
		
		if((infExpr[i] == '+') || (infExpr[i] == '-') || (infExpr[i] == '*')  ||(infExpr[i] == '/')) 
			{	
				doOperation(&s, infExpr[i], postExpr, &j);  
			} 

		else if(infExpr[i] == '(' )							//Ak je znak lava zatvorka, spracuva sa tymto sposobom...prida sa na vrchol zasobniku
			stackPush(&s, infExpr[i]);
				
		else if(infExpr[i] == '=')							//Ak je znak '=', spracuva sa tymto sposobom...
		{
			while(!stackEmpty(&s))			 
			{
				stackTop(&s, &postExpr[j]);  				//Vyberiem z vrchulu zasobniku znak
				stackPop(&s);								//Odstranim ho, a inkrementujem umiestnenie prvku vysl. retazca
				j++;
			}
			postExpr[j] = '=';								//Na uplny koniec vysl. retazca pridam '=' a potom '\0'
			j++;
			postExpr[j] = '\0';
		}
				
		else if(infExpr[i] == ')' )							//Ak je znak prava zatvorka, spracuva sa tymto sposobom...odstranuje sa zo zasobniku
		{	
			untilLeftPar(&s, postExpr, &j);

		}
		else if(((infExpr[i] >= 'a') && (infExpr[i])<= 'z') || ((infExpr[i] >= 'A') && (infExpr[i] <= 'Z')) || ((infExpr[i] >= '0') && (infExpr[i] <= '9'))) //Povoluju sa iba a-z , A-Z, 0-9 operandy
		{
			postExpr[j] = infExpr[i];						//Priradenie operandu na koniec retazca
			j++;
		}
		i++;
	}
   return postExpr;   										//Funkcia vracia Vysledny retazec
						       								
}

/* Konec c204.c */

