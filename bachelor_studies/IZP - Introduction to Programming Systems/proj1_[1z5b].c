/*
 * Subor:   proj1.c
 * Datum:   21.10.2012
 * Autor:   1BIB -  Ladislav Šulák, xsulak04stud.fit.vutbr.cz
 * Projekt: IZP č.1 - tvorba histogramu
 * Popis:   Program vytvára tabulku početnosti znakov zo vstupného textu.
 *          Znaky vytriedi podla početnosti,a ak bude mat niekolko znakov
 *          rovnakú početnost, budu zobrazené v poradí podla ich ordinálnych
 *          hodnot.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
//Nápoveda
void printHelp(void)
{
  printf("                                                      \n"
  "Program: Tvorba histogramu.                                \n\n"
  "Autor:   Ladislav Šulák                                    \n\n"
  "Popis    Program vytvorí tabulku početnosti znakov zo vstupného textu.\n"
  "         Znaky vytriedi podla ich početnosti.              \n\n"
  "Popis parametrov:                                            \n"
  "    -h   Zobrazí nápovedu.                                   \n"
  "     Bez parametrov - zobrazí všetky riadky histogramu.      \n"
  "     N   Cislo, ktoré udáva počet riadkov histogramu.        \n"
  "Koniec nápovedy: stlač Ctrl+D v Linuxe                       \n"
  "                 alebo Ctrl+Z vo Windowse                    \n");
}

//Ak bude zadaný parameter -h alebo --help , zobrazí nápovedu
void paramHelp(int argc, char *argv[])
{
   unsigned char pole[255]={0};
   int i=0;
   int max=0;
   int c=0;
   while(( c = getchar()) != EOF)
   {
     if(pole[c]<255)
    pole[c]++;
   }
  if(argc == 2)
  {
    if((strcmp("--help" , argv[1]) == 0) || (strcmp("-h" , argv[1]) == 0))
    {
      printHelp();
    }
    else if(paramChng(argv[1]==1))
            {
              for(int j;j<255;j++)
              if(pole[j]>max)
              max=j;

            }
   printf("%d '%c': %d", max , max, pole[max]);
}
}

int paramChng(char* h);
//Prevod stringu na cislo
int strtoint(char* g)
{
    int cislo=0;
    for(int i=0; i<strlen(g); i++)
{
    cislo=cislo*10+(g[i]-'0');
}
return cislo;
}

//Spracúva, triedi a zobrazuje početnost znakov zo vstupného textu
void vstupneUdaje(void)
{
  int c;
  int pocet=0;

  for(pocet=0; pocet<256 ;pocet++)
   {
    if(pole[pocet]>0 && pole[pocet]<255)
    //Ak je pocetnost znaku  od 0 do 255,vytlaci sa jeho početnost
     {
       if(isprint(pocet))
        printf("%d '%c': %d\n" ,pocet ,pocet ,pole[pocet]);
       else if(!isprint(pocet))
         printf("%d: %d\n" ,pocet ,pole[pocet]);
     }
    else if(pole[pocet]>=255)
    //Ak je pocetnost znaku vacsia alebo práve rovná 255,vytlaci sa NNN
         printf("%d '%c': NNN\n" ,pocet ,pocet);
   }
}

//Uprava parametrov - prevod retazca na číslo
int paramChng(char* h)
{

  int c=0;
  for(int i=0 ; i<strlen(h); i++)
  {
  if(isdigit(char[i]))
  c++;
  }
  if(c=strlen(h))
  return 1;
  else
  return 0;
}


//Hlavná funkcia, v nej sa volajú ostatné funkcie
int main (int argc, char *argv[])
{

  if(argc==1)
  vstupneUdaje();
  else
  paramHelp(argc, argv);

   return 0;
}
