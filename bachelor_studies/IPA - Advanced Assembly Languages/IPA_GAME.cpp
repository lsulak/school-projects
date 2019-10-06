/*	Autor: Ladislav Šulák , 2BIT	
	Login: xsulak04
	Dátum: 24.12.2013
	Mail:  xsulak04@stud.fit.vutbr.cz
	
	Projekt do predmetu Pokrocile Asemblery 
	Varianta: jednoducha aplikace v asembleru
	Hra Ping Pong
*/

#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include <malloc.h>
#include "glut.h"


//Globalna struktura, najprv tu boli glob. premenne, ale tazko sa k nim pristupovalo v asemblere.
//Resp. nie tazko, ale nedali sa menit, iba pozerat ich obsah a porovnavat.
typedef struct {	
	int vyska_okna;
	int sirka_okna;  
	int interval;
	int speed;

	int my_x; 
	int my_y; 
	int my_tmp;
	int op_x;
	int op_y;
	int op_tmp;
	int dlzka;

	int ball_x;
	int ball_y;
	int ball_x_tmp;
	int ball_y_tmp;

	int scoreL;
	int scoreR;

	int game_mode;
} Tgame;
	
Tgame *g = NULL;

//Prototypy funkcii
void pociatocne_hodnoty(Tgame* g);

void draw_my();
void draw_op();
void draw_ball();
void draw_border();
void draw();
void draw_scoreL();
void draw_scoreR();

void move_my();
void move_op();
void ball();

void keyboard(unsigned char key, int x,int y);
void processSpecialKeys(int key, int x, int y);

void timing(int value);
void Initialization(int vyska, int sirka);

int main(int argc, char * argv[])
{ 
	 
	 g = (Tgame*) malloc(sizeof(Tgame));
	 if(g == NULL)
		 return 1;
	 pociatocne_hodnoty(g); //inicializujeme hodnoty premennych v strukture

     glutInit(&argc, argv);
     glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
	 glutInitWindowSize(g->vyska_okna, g->sirka_okna);
     glutInitWindowPosition(g->vyska_okna/4 , 0);
     glutCreateWindow("Ping Pong game, IPA 2013"); // vsetky funkcie inicializuju okno, poziciu, rozlisenie, atd.
    
	 glutDisplayFunc(draw); //nekonecne smycky pre vykreslovanie, casovanie, obsluhu klaves
     glutTimerFunc(g->interval,timing,0);

     glutKeyboardFunc(keyboard);
	 glutSpecialFunc(processSpecialKeys); // obsluha specialnych GLUT klaves, ako F1,F2,sipky

	 Initialization(g->vyska_okna,g->sirka_okna);
     glutMainLoop(); //hlavna smycka hry, je nekonecna az po stlaceni Escape alebo vypnuti hry
	 free(g);	//odalokovanie struktury
     system("pause >> NULL");
     return (0);
}

void pociatocne_hodnoty(Tgame* g)
{
	/////ECX BUDE POMOCNY REGISTER -> PRE KONSTANTY, EBX BUDE TIEZ POMOCNY REGISTER -> PRE SIRKU_OKNA//////////
	///// NA ZACIATKU VZDY VYMAZEME OBSAH REGISTROV,KTORE BUDEME POUZIVAT/////////
	__asm
	{
		xor eax,eax
		xor ebx,ebx
		xor ecx,ecx
		xor edx,edx

		mov edx, g		  ;do edx si dame odkaz na strukturu a potom pomocou posunov budeme k nej pristupovat

		mov ecx, 1024	  ;vyska_okna = 1024
		mov [edx],ecx
		xor ecx, ecx

		mov ecx, 860	  ;sirka_okna = 860
		mov [edx+4],ecx
		mov EBX, ecx
		xor ecx,ecx

		mov ecx, 3		  ;interval = 3
		mov [edx+8],ecx
		xor ecx,ecx

		mov ecx, 3		  ;speed = 3 - tato je optimalna pre start
		mov [edx+12],ecx
		xor ecx,ecx

		neg EBX			  ;my_x = -sirka_okna
		mov [edx+16],ebx
		
		xor ecx,ecx
		mov [edx+20],ecx  ;my_y = 0

		xor ecx,ecx
		mov [edx+24],ecx  ;my_tmp = 0

		neg EBX			  ;op_x = sirka_okna
		mov [edx+28],EBX
		xor EBX,EBX

		xor ecx,ecx
		mov [edx+32],ecx  ;op_y = 0
	
		xor ecx,ecx
		mov [edx+36],ecx  ;op_tmp = 0
		 
		mov ecx,260
		mov [edx+40],ecx  ;dlzka = 260 - dlzka rakety

		xor ecx,ecx
		mov [edx+44],ecx  ;ball_x = 0

		xor ecx,ecx
		mov [edx+48],ecx  ;ball_y = 0
		
		xor ecx,ecx
		mov ecx,-1
		mov [edx+52],ecx  ;ball_x_tmp = -1

		xor ecx,ecx
		mov [edx+56],ecx  ;ball_y_tmp = 0

		xor ecx,ecx
		mov [edx+60],ecx  ;scoreL = 0

		xor ecx,ecx
		mov [edx+64],ecx  ;scoreR = 0
		
		xor ecx,ecx
		mov ecx,2
		mov [edx+68],ecx  ;game_mode = 2 - hra je na zac. pre 2 hracov

		xor eax,eax
		xor ebx,ebx
		xor ecx,ecx
		xor edx,edx
	}
	/////PRE ISTOTU VYMAZANIE OBSAHU EAX,EBX,ECX ESTE AJ NA KONCI//////
}

void draw_my() // vykreslenie lavej rakety
{ 
	glVertex2f(g->my_x+10, g->my_y+130); 
	glVertex2f(g->my_x+10, g->my_y-130);
	glVertex2f(g->my_x-10, g->my_y-130);
	glVertex2f(g->my_x-10, g->my_y+130);
}

void draw_op() // vykreslenie pravej rakety
{ 
	glVertex2f(g->op_x+10, g->op_y+130);
	glVertex2f(g->op_x+10, g->op_y-130);
	glVertex2f(g->op_x-10, g->op_y-130);
	glVertex2f(g->op_x-10, g->op_y+130);
}

void draw_ball() // vykreslenie lopticky
{ 
	glVertex2f(g->ball_x+15 , g->ball_y+15);
	glVertex2f(g->ball_x+15 , g->ball_y-10);
	glVertex2f(g->ball_x-10 , g->ball_y-10);
	glVertex2f(g->ball_x-10 , g->ball_y+15);
}

void draw_border() // vykreslenie ramov, a strednej ciary , v strede x-ovej suradnice
{
	glColor3f(1,0,0);
	glVertex2f(g->vyska_okna   , g->vyska_okna);
	glVertex2f(g->vyska_okna-4 , g->vyska_okna);
	glVertex2f(g->vyska_okna-4 ,-g->vyska_okna);
	glVertex2f(g->vyska_okna   ,-g->vyska_okna);
	glColor3f(1,1,1);
 
	glColor3f(1,0,0);
	glVertex2f(g->vyska_okna   , g->vyska_okna);
	glVertex2f(g->vyska_okna   , g->vyska_okna-4);
	glVertex2f(-g->vyska_okna  , g->vyska_okna-4);
	glVertex2f(-g->vyska_okna  , g->vyska_okna);
	glColor3f(1,0,0);
 
	glColor3f(1,0,0);
	glVertex2f( 2,-g->vyska_okna);
	glVertex2f( 2, g->vyska_okna);
	glVertex2f(-2, g->vyska_okna);
	glVertex2f(-2,-g->vyska_okna);
	glColor3f(1,1,1);

	glColor3f(1,0,0);
	glVertex2f(-g->vyska_okna  , -g->vyska_okna);
	glVertex2f(-g->vyska_okna  ,  g->vyska_okna);
	glVertex2f(-g->vyska_okna-4,  g->vyska_okna);
	glVertex2f(-g->vyska_okna-4, -g->vyska_okna);
	glColor3f(1,1,1);
 
	glColor3f(1,0,0);
	glVertex2f(-g->vyska_okna ,  -g->vyska_okna);
	glVertex2f(g->vyska_okna  ,  -g->vyska_okna);
	glVertex2f(g->vyska_okna  ,  -g->vyska_okna-4);
	glVertex2f(-g->vyska_okna ,  -g->vyska_okna-4);
	glColor3f(1,1,1);
}

void draw_scoreR() //vykreslenie skora pre hraca
{ 
	glColor3f(1,1,1);
	for(int i = 0; i < g->scoreR; i++)
		{
		   glVertex2f(-20 -i*50, g->vyska_okna-20);
		   glVertex2f(-12 -i*50, g->vyska_okna-20);
		   glVertex2f(-12 -i*50, g->vyska_okna-30);
		   glVertex2f(-20 -i*50, g->vyska_okna-30);
        } 
}

void draw_scoreL() //vykreslenie skora pre hraca
{ 
    glColor3f(1,1,1);
    for(int i = 0; i < g->scoreL; i++)
	{
        glVertex2f(20 + i*50, g->vyska_okna-20);
        glVertex2f(12 + i*50, g->vyska_okna-20);
        glVertex2f(12 + i*50, g->vyska_okna-30);
        glVertex2f(20 + i*50, g->vyska_okna-30);
        } 
}

void draw() 
{
	glClearColor(0.5, 0.5, 0.5 , 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
	
	glPushMatrix();
	glBegin(GL_QUADS);
  
    glColor3f(0,0,1); // nacerveno zafarbime (podla RGB) ramy
	draw_border();
    draw_my();		// rakety na bielo
	draw_op();

	glColor3f(1,1,0); //loptu na zlto
	draw_ball();

	draw_scoreL();  //skore tiez na bielo,
	draw_scoreR();  // bude vykreslovane iba ked sa lopta dotkne laveho/praveho okraju
	glEnd();
	glPopMatrix();

	glutSwapBuffers();
}

void move_my() //funkcia pre pohyb lavej rakety, je osetrene nech nejde mimo hracieho pola
{ 
	if((g->my_y >= -g->vyska_okna + g->dlzka/2)&&(g->my_y <= g->vyska_okna - g->dlzka/2))
	{	
		g->my_y += g->my_tmp;
	}
	if((g->my_y < -g->vyska_okna + g->dlzka/2))
	{
		g->my_y =-g->vyska_okna +10 + g->dlzka/2;
		g->my_tmp = 0;
	}

	if((g->my_y > g->vyska_okna - g->dlzka/2))
	{
		g->my_y = g->vyska_okna -10 - g->dlzka/2;
		g->my_tmp = 0;
	} 
}
 
void move_op()
{ 	
	if(g->game_mode == 2) //hraci mod pre 2 hracov, ovladanie popisane v dokumentacii
	{						//ovladanie implemenovane vo funkcii processSpecialKeys()
		if((g->op_y >= - g->vyska_okna + g->dlzka/2)&&(g->op_y <= g->vyska_okna - g->dlzka/2))
			g->op_y += g->op_tmp;

		if((g->op_y < - g->vyska_okna + g->dlzka/2))
		{
			g->op_y =- g->vyska_okna +10 + g->dlzka/2;
			g->op_tmp = 0;
		}

		if((g->op_y > g->vyska_okna - g->dlzka/2))
		{
			g->op_y = g->vyska_okna -10 - g->dlzka/2;
			g->op_tmp = 0;
		}
	}
	else if(g->game_mode == 1) //Hraci mod pre 1 hraca, druhym je PC, prava raketa sa nachadza vzdy tam, kde lopta,
		g->op_y = g->ball_y;   // teda je neporazitelna(testovanie aplikacie....)
}

void ball()
{ 
	g->ball_x +=g->ball_x_tmp*g->speed;
	g->ball_y +=g->ball_y_tmp*g->speed; 

	//Odraz od mojej rakety, teda lavej
	if((g->ball_x < g->my_x + 30) && 
		(fabs(double(g->ball_y - g->my_y)) <= g->dlzka/2))
	{
		g->ball_x_tmp = -g->ball_x_tmp;
	}
	else if((g->ball_x > g->op_x -30) && 
	   (fabs(double(g->ball_y - g->op_y)) <= g->dlzka/2))
	{
		g->ball_x_tmp = -g->ball_x_tmp;
	}
	else if(g->ball_y > g->vyska_okna-20)
	{
		g->ball_y_tmp = -fabs(double(g->ball_y_tmp));//odraz o hornu stenu!
	}
	else if(g->ball_y < -g->vyska_okna+20 )
	{
		g->ball_y_tmp =  fabs(double(g->ball_y_tmp));//odraz o dolnu stenu
	}
	else if(g->ball_x < -g->sirka_okna-150)	//lopticka sa dotkne laveho ramu
	{
		g->ball_x = 0;
		g->ball_y = 0;
		g->ball_y_tmp = 0;  //pozicie lopticky aj rakiet sa vynuluju
		if(g->scoreL == 20) //hra sa iba do 20, potom sa skore vynuluje, a rychlost nastavi na pociatocnu
		{
			g->scoreR = 0;
			g->scoreL = 0;
			g->speed = 3;
		}
		g->scoreL++;		//inak pri prehre sa skore aj speed inkrementuju
		if(g->speed == 26)	//rychlost lopticky je tiez obmedzena, inak by bola hra nehratelna
			g->speed = 3;
		else
			g->speed++;
		g->my_y = 0;
		g->op_y = 0;
	}
	else if(g->ball_x > g->sirka_okna+150) //lopticka sa dotkne praveho ramu
	{
		g->ball_x = 0;
		g->ball_y = 0;
		g->ball_y_tmp = 0;    //pozicie lopticky aj rakiet sa vynuluju
		if(g->scoreR == 20)   //hra sa iba do 20, potom sa skore vynuluje, a rychlost nastavi na pociatocnu
		{
			g->scoreR = 0;
			g->scoreL = 0;
			g->speed = 5;
		}
		g->scoreR++;       //inak pri prehre sa skore aj speed inkrementuju
		if(g->speed == 26) //rychlost lopticky je tiez obmedzena, inak by bola hra nehratelna
			g->speed = 3;
		else
			g->speed++;
		g->my_y = 0;
		g->op_y = 0;
	}
}

void keyboard(unsigned char key, int x,int y)
{
	///// NA ZACIATKU VZDY VYMAZEME OBSAH REGISTROV,KTORE BUDEME POUZIVAT/////////
	__asm
	{

		xor eax,eax
		xor edx,edx

		mov edx, g		   ;vlozenie odkazu na strukturu do registru EDX

		cmp [key],119      ;instrukcie pre pohyb lavou raketou hore, pri stlaceni W/w
		jne next_w
		mov eax,100 
		add [edx+20],eax   ;zvacsime premennu my_y, teda pohyb hore
		xor eax,eax
		
		mov EAX,[edx + 16] ;ak spravime pohyb raketkou hore pri dopadnuti lopty tak sa lopte zmeni uhol nahor
		add EAX,100
		cmp [edx + 44],EAX ;if(my_x +100 <= ball_x)
		jg next_w
		
		xor eax,eax
		mov eax, [edx+16]
		cmp [edx+44],eax   ;&& if(my_x > ball_x)
		jle next_w
		xor eax,eax
		
		mov eax,1
		add [edx + 56],eax ;ball_y_tmp++; teda lopticka bude po odrazeni stupat hore
		xor eax,eax		   ;riesene intervalom, pretoze inak by sme sa museli presne na milisekuntu trafit vtedy ked lopticka dopadne na raketu
	
	next_w:				   ;symetricky blok instrukcii k predchodziemu, moze byt stlacene W/w
		cmp [key], 87
		jne	move_s
		xor eax, eax
		mov eax,100 
		add [edx+20],eax
		xor eax,eax
		
		mov EAX,[edx + 16]
		add EAX,100
		cmp [edx + 44],EAX ;if(my_x +100 <= ball_x)
		jg move_s
		xor eax,eax
	
		mov eax, [edx+16]
		cmp [edx+44],eax   ;&& if(my_x > ball_x)
		jle move_s
		xor eax,eax
	
		mov eax,1
		add [edx + 56],eax   ;ball_y_tmp++; teda lopticka bude po odrazeni stupat hore
		xor eax,eax

	move_s:        ;symetricke k predchodzim 2 blokom, ibaze pre stlacenie s/S , a pohyb raketky smerom dolu
		cmp [key],115
		jne next_s
		xor eax, eax
		mov eax,100 
		sub [edx+20],eax
		xor eax,eax
		
		mov EAX,[edx + 16]
		add EAX,100
		cmp [edx + 44],EAX
		jg next_s
		xor eax,eax

		mov eax, [edx+16]
		cmp [edx+44],eax   ;&& if(my_x > ball_x)
		jle next_s
		xor eax,eax

		mov eax,-1
		add [edx + 56],eax
		xor eax,eax
	
	next_s:		;symetricky blok instrukcii k predchodziemu, moze byt stlacene S/s
		cmp [key], 83
		jne	plus
		xor eax, eax
		mov eax, 100 
		sub [edx+20],eax
		xor eax,eax

		mov EAX,[edx + 16]
		add EAX,100
		cmp [edx + 44],EAX
		jg plus
		xor eax,eax

		mov eax, [edx+16]
		cmp [edx+44],eax   ;&& if(my_x > ball_x)
		jle plus
		xor eax,eax

		mov eax,-1
		add [edx + 56],eax
		xor eax,eax

	plus:	;ak je stlacena klavesa '+' , inkrementuje sa premenna speed, teda zvysi sa rychlost pohybu lopty
		cmp [key],43
		jne minus
		xor eax,eax
		mov eax,30
		cmp [edx+12],eax ;no rychlost nemoze byt vacsia nez 30 - hra by bola nehratelna
		je minus		 ;ak je 30, nic sa nestane, ak nieje tak sa inkrementuje
		xor eax,eax
		mov eax,1
		add [edx+12],eax

	minus:				;ak je stlacena klavesa '-' , rychlost lopty sa dekrementuje
		cmp [key],45
		jne end
		xor eax,eax
		mov eax,1
		cmp [edx+12],eax	; rychlost nemoze byt mensia nez 1, teda zaporna alebo ziadna
		je end				; ak je , nic sa nestane, ak nieje dekrementuje sa
		xor eax,eax
		mov eax,-1
		add [edx+12],eax

	end:
		xor eax,eax ;vynulovanie pouzitych registrov
		xor edx,edx
	}
	
	if(key == 27)   //ak je klavesa 27 - Escape, hra sa ukonci.
		exit(0);	//najprv som to chcel robit v asemblere pomocou prerusenia ale takto je to univerzalnejsie a prehladnejsie
}

void processSpecialKeys(int key, int x, int y)
{ 
	__asm
	{
		xor eax,eax
		xor ebx,ebx
		xor edx,edx

		mov edx, g

		cmp [key],101		;glut_key_up
		jne glut_key_down
		mov eax,100
		add [edx + 32],eax	;op_y+=100 - pohyb hore
		xor eax,eax
		
		mov EAX,[edx + 28]	
		sub EAX,100
		cmp [edx + 44],EAX	; if(op_x-100 >= ball_x) 
		jl glut_key_down
		xor eax,eax
		mov eax, [edx+28]
		cmp [edx + 44], eax ; && if(op_x < ball_x)
		jg glut_key_down
		inc [edx + 56]		; then ball_y_tmp++;  teda lopticka bude po odrazeni stupat hore
		xor eax,eax			; riesene intervalom, pretoze inak by sme sa museli presne na milisekuntu trafit vtedy ked lopticka dopadne na raketu

	glut_key_down:
		cmp [key],103		;glut_key_down
		jne glut_key_f1
		xor eax,eax
		mov eax,100
		sub [edx + 32],eax  ;op_y-=100 - pohyb dolu
		xor eax,eax

		mov EAX,[edx + 28]
		sub EAX,100
		cmp [edx + 44],EAX    ;if(op_x-100 >= ball_x)
		jl glut_key_f1
		xor eax,eax
		mov eax, [edx+28]
		cmp [edx + 44], eax    ; && if(op_x < ball_x)
		jg glut_key_f1
		xor eax,eax
		mov eax,-1			   ; then ball_y_tmp--;  teda lopticka bude po odrazeni stupat dolu
		add [edx + 56], eax    ; riesene intervalom, pretoze inak by sme sa museli presne na milisekuntu trafit vtedy ked lopticka dopadne na raketu

	glut_key_f1:
		cmp [key],1				;ak je stlacena klavesa F1, premenna game_mode = 1
		jne glut_key_f2
		xor eax,eax
		mov eax,1
		mov [edx + 68],eax

	glut_key_f2:				;ak je stlacena klavesa F2, premenna game_mode = 2
		cmp [key],2
		jne end
		xor eax,eax
		mov eax,2
		mov [edx + 68],eax
	
	end: ;vynulovanie pouzitych registrov
		xor eax,eax
		xor ebx,ebx
		xor edx,edx
	}
}

void timing(int value) //funkcia z GLUT, podla intervalu obsluhuje funkcie pre pohyb rakiet a lopty
{ 
	move_my();
	move_op();
	ball();
	glutTimerFunc(g->interval,timing,0);
	glutPostRedisplay(); 
}

void Initialization(int vyska, int sirka)
{	//dalsia funkcia z OpenGL (GLUT), pre inicializovanie okna
	glClearColor(0,0,0,1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glScalef(0.8,0.8,0.8);
	glOrtho(-vyska,vyska,-sirka,sirka,-2,2);
}