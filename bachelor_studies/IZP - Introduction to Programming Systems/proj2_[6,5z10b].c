/*
 * Subor:   proj2.c
 * Datum:   11.11.2012
 * Autor:   1BIB -  Ladislav Šulák, xsulak04stud.fit.vutbr.cz
 * Projekt: IZP č.2 - Iterační výpočty
 * Popis:   Vypocet 3 matematických funkcií pomocou iteračných algoritmov.
 */

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define MYABS(x) (x<0 ? -x : x)               //Absolútna hodnota
//#define NAN = 0.0/0.0                         //Not a Number
//#define INFINITY = 1.0/0.0                    //Infinity

const double IZP_E = 2.7182818284590452354;        // e
const double IZP_PI = 3.14159265358979323846;      // pi
const double IZP_2PI = 6.28318530717958647692;     // 2*pi
const double IZP_PI_2 = 1.57079632679489661923;    // pi/2
const double IZP_PI_4 = 0.78539816339744830962;    // pi/4
const double IZP_LN2  = 0.693147180559945309417;    //Ln(2)

enum errors
{
  EFORMAT,  //Zlý formát vstupných parametrov.
  ENUMBERS, //Príliš vela parametrov.
  ENUMBER,  //Príliš málo parametrov.
  EUNKNOWN //neznáma chyba.
};

const char *EMSG[]=
{
  "Zadali ste nesprávny parameter!\n",
  "Zadali ste príliš vela parametrov!\n",
  "Zadajte parameter!\n",
  "Neznáma chyba\n"
};

//Nápoveda
void printHelp(void)
{
  printf("                                                      \n"
  "Program: Iterační výpočty                                  \n\n"
  "Autor:   Ladislav Šulák                                    \n\n"
  "Popis: Program vypočíta:                                     \n"
  "Mocninovú funkciu (s reálným exponentom)                     \n"
  "Arcus tangens                                                \n"
  "Argument hyperbolického sinu                               \n\n"
  "Popis parametrov:                                            \n"
  "    -h  alebo --help  Zobrazí nápovedu.                      \n"
  "    --powxa sigdig a  sigdig-presnost zadana ako pocet platných číslic. a-exponent.   \n"
  "    --arctg sigdig                                           \n"
  "    --argsinh sigdig                                       \n\n"
  "Koniec nápovedy: stlač Ctrl+D v Linuxe                       \n"
  "                 alebo Ctrl+Z vo Windowse                    \n");
}

double myPowK(double x , double a);  //Prototyp mocninovej funkcie.Exponent može byt iba kladné,celé čislo.

double myArctg(int sigdig, double x) //Funkcia pre výpočet Arctg.
{                                    //Počítam ju vdaka Taylorovmu rozvoju

  double vypocet = x;
  double p_vypocet = vypocet;
  double k = 3.0;
  double p_cislo=1.0;
  double epsylon;
  epsylon=myPowK(0.1 , sigdig); //pocet platných cislic vypocitam umocnenim 0.1 ^ sigdig

  if(x<1 && x>=0)              //Rozsah <0,1)
  {
    while (MYABS(p_vypocet) > epsylon*vypocet)
    {
      p_vypocet = -((p_vypocet*x*x)*(p_cislo/k));
      k+= 2.0;
      vypocet = vypocet + p_vypocet;
      p_cislo+=2.0;

    }
      return vypocet;
  }

  else if(x>1)               //heuristika pre x>1
  {
    return(IZP_PI_2-myArctg(sigdig , 1/x));
  }
  else if(x<0)                //heuristika pre x<0
  {
    return(-IZP_PI_2+myArctg(sigdig , 1/MYABS(x)));
  }
  else if(x==1)
    return IZP_PI_4;

  else if(x==-1)
    return -IZP_PI_4;

  else if(x==NAN)
    return NAN;

  return 0;
}

double log_natural(int sigdig , double x)  //Funkcia pre výpočet prirodzeného logaritmu.
{                                          //Počítam ho vdaka Taylorovmu rozvoju

  if(x<0)
    return NAN;

  if(x==0)
    return -INFINITY;

  if(x==NAN)
    return NAN;

  if(x==INFINITY)
    return INFINITY;
  int epsylon;
  int spresni=0;

  while(2<x)  // Heuristika pre x>2 . Vychádzam zo základných vzorcov
  {           // pre počítanie s logaritmami. Príklad: Ln(24)=Ln(8*3)=Ln(8)+Ln(3)=Ln(2^3)+Ln(3)=3*ln(2)+ln(3)
    x=x/2;    //Bez tejto heuristiky by som pri vyšších x dostaval nepresnejsie čísla.
    spresni++;
  }

  while(x<1) //Heuristika pre x<2. Vychádzam z podobného princípu ako pre x>2.
  {          // Využívam základné vztahy pre vypočet algoritmov.
    x=x*2;
    spresni--;
  }          //Koniec heuristiky.

  double cislo=3.0;             //Pre násobenie menovatela.
  double vysledok=0.0;
  double p_vysledok=0.0;
  double p_citatel=(x-1);       //Pomocná premenná pre citatel.
  double p_menovatel=(x+1);     //Pomocna premenna pre menovatel.
  double p_cislo=1.0;           //Pre delenie menovatela.
  p_vysledok=p_citatel/p_menovatel;
  epsylon = myPowK(0.1 , sigdig);  //Výpočet relatívnej presnosti.

  while (MYABS(p_vysledok) > epsylon*vysledok) //Cyklus sa zastaví, ked bude prírastok vačší než relatívna presnost.

  {
   p_vysledok=p_citatel/p_menovatel;

   vysledok+=p_vysledok;

   p_citatel=p_citatel*(x-1)*(x-1);
   p_menovatel=cislo*p_menovatel*(x+1)*(x+1)/p_cislo;

   cislo+=2.0;
   p_cislo+=2.0;
  }
  vysledok*=2;
  return vysledok+spresni*IZP_LN2; //Vracia výsledok + spresni*ln(2) , vid heuristika
}

double myPowK(double x , double a)  //Funkcia, ktorá vypočíta mocninu čísla x^a
{                                   //pre a > 0 &&  musí byt celé čislo.
  int i=0;                          //Využijem ju na výpočet relatívnej presnosti
  double vysledok=1.0;              //    ,kedže sigdig je cele kladné čislo.

  for(i=0; i<a; i++)
    vysledok*=x;

  return vysledok;
}


double myPow(int sigdig , double x, double a) //Funkcia ,ktorá vypočíta mocninu kladného čísla pri reálnom exponente.
{
  double epsylon;
  double vysledok=1.0;
  double p_vysledok=1.0;
  int p_menovatel=1;
  double pomA;
  double pomB;
  pomA=log_natural(sigdig , x);
  pomB=a;
  epsylon=myPowK(0.1 , sigdig);             //Vypocet relatívnej presnosti.

  if(x==0)
    return x;
  if(x>0 && x<1e-2)
  {
  if(a>0)
     return myPowK(x,trunc(a));
  }
  if(x<0)
  return NAN;

  while(MYABS(p_vysledok) > epsylon*vysledok)   //Cyklus sa zastaví, ked bude prírastok vačší než relatívna presnost.

  {
    p_vysledok*=(pomB*pomA)/p_menovatel;
    p_menovatel++;
    vysledok+=p_vysledok;
  }
    return vysledok;
}

double myArgsinh(int sigdig , double x)  //Funkcia pre výpočet argsinh.
{                                        //Vychádzal som zo vzorca argsinh=ln(x+sqrt(x*x+1)
  double odmocnina=x*x+1;
  const double exp=0.5;
  double vypocet = 0.0;
  vypocet = log_natural(sigdig , x+(myPow(sigdig, odmocnina,exp)));
 if(x>0)
 {
  if(x<=1e-15)   //Heuristika pre čísla ktoré konvergujú k nule.
    return x;

   if(x>1e-15)
    return vypocet;
  }
else if(x<0)
  {
   if(x>=-1e-15)
    return x;
   else
   return vypocet;
  }
 return 0;
}

int main(int argc , char *argv[])  //Hlavná funkcia.Riešim v nej spracovanie parametrov,a nacitavanie a upravovanie vstupu.
{
  //char *c=NULL;
  double x;

if(argc > 1 && argc < 5)
 {
  if(argc == 2)
  {
    if(strcmp("--help" , argv[1]) == 0 || strcmp("-h" , argv[1]) == 0)  //Pri splnenej podmienke sa zobrazí nápoveda.
    {
      printHelp();                                                      //Volanie funkcie pre nápoevdu.
      return 0;
    }
    else
    {
      fprintf(stderr , "Chyba : %s" , EMSG[EFORMAT]);                   //Chybové hlásenie-Ak zadáme zlý parameter.
      return 0;
    }
  }

  if(argc == 3)
  {
     if(strcmp("--arctg" , argv[1]) == 0)                                //Pri splnenej podmienke sa načíta premenná x, a vypočíta sa arctg.
    {
      fscanf(stdin , "%lf" ,&x);
      fprintf(stdout , "    Hodnota Arctg(%e) = %.10e\n\n" ,x ,myArctg(atoi(argv[2]), x) ); //argv[2] bude sigdig.
    //  fprintf(stdout , "TEST Math.h Arctg(%e) = %.10e\n\n", x, atan(x));
      return 0;
    }

   else if(strcmp("--argsinh" , argv[1]) == 0)                          //Pri splnenej podmienke sa načíta premenná x, a vypočíta sa argsinh-
   {
     fscanf(stdin , "%lf" ,&x);
    // fprintf(stdout ,"Iba pre Test, z Math.h Argsinh(%e) = %.10e\n" , x , asinh(x) );
     fprintf(stdout , "Hodnota Argsinh(%e) = %.10e\n\n" ,x , myArgsinh(atoi(argv[2]) , x) ); //argv[2] bude sigdig.
     return 0;
   }
   else
   {
     fprintf(stderr , "Chyba : %s" , EMSG[EFORMAT]);                   //Chybové hlásenie-Ak zadáme zlé parametre.
     return 0;
   }
  }

  if(argc == 4 )                                                        //Pri splnenej podmienke sa načíta x, a vypočíta sa mocninová funkcia powxa.
  {
    if(strcmp("--powxa" , argv[1]) == 0)
    {
      fscanf(stdin ,"%lf" ,&x);
     // fprintf(stdout , "TEST z math.h powxa   = %.10e\n\n" , pow(x,atof(argv[3]) ) ) ;
      fprintf(stdout , "Hodnota powxa %e ^ %e = %.10e\n\n" , x , atof(argv[3]) , myPow(atoi(argv[2]) , x , atof(argv[3]) )); //V tomto kroku sa načíta sigdig,aj exponent.
      return 0;
    }

    else
    {
      fprintf(stderr , "Chyba : %s" , EMSG[EFORMAT]);               //Chybové hlásenie: Ak zadáme zlé parametre.
      return 0;
    }
  }
  else
    {
      fprintf(stderr , "Chyba : %s" , EMSG[EFORMAT]);               //Chybové hlásenie: Ak zadáme zlé parametre.
    }
}

else
{
  if(argc==1)
  {
    fprintf(stderr ,"Chyba : %s" , EMSG[ENUMBER]);                  //Chybové hlásenie: Ak nezadáme žiadny parameter.
  }
  if(argc>=5)
  {
    fprintf(stderr , "Chyba: %s" , EMSG[ENUMBERS]);               //Chybové hlásenie: Ak zadáme viacej parametrov než máme dané.
  }
  return 0;
}
return 0;
}
