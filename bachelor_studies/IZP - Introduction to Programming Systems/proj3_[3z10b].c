/*
 * Subor:   proj3.c
 * Datum:   25.11.2012
 * Autor:   1BIB -  Ladislav Šulák, xsulak04stud.fit.vutbr.cz
 * Projekt: IZP č.3 - Riešenie osemsmerovky
 * Popis:   Program dokáže overit,či data zo vstupného súboru neobsahujú chybu
 *          ,dokáže nájst dané slovo v matici , a dokáže vylúštit osemsmerovku.
 */
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

enum errors
{
  EFORMAT,  //Zlý formát vstupných parametrov.
  ENUMBERS, //Príliš vela parametrov.
  ENUMBER,  //Príliš málo parametrov.
  ESIZE,    //Zlé rozmery matice.
  EOPEN,    //Chyba pri otvárani suboru.
  ECLOSE,   //Chyba pri zatváraní súboru.
  ESUBOR,   //Chyba vo vstupnom súbore.
  ESUBORC,  //V matici sa nachádza číslica.
  ESUBORP,  //V matici sa nachádza písmeno velkej abecedy.
  EMALLOC,  //Nepodarilo sa alokovat pamat.
  ESTRING   //Zadajte slovo zlozene z pismen malej abecedy.
};

const char *EMSG[]=
{
  "Zadali ste nesprávny parameter!\n",
  "Zadali ste príliš vela parametrov!\n",
  "Zadajte parameter!\n",
  "Zlé rozmery matice!\n",
  "Pri otváraní súboru došlo k chybe!\n",
  "Pri zatváraní súboru došlo k chybe!\n",
  "Vo vstupnom súbore je chyba!",
  "V Matici sa nachádza číslica!",
  "V Matici sa nachádza písmeno velkej abecedy!",
  "Nepodarilo sa alokovat potrebné množstvo pamate!",
  "Zadajte slovo zlozene z pismen malej abecedy!"
};


//Nápoveda
void printHelp(void)
{
  fprintf(stdout , "                                              \n"
  "Program: Riešenie osemsmerovky                                \n\n"
  "Autor:   Ladislav Šulák                                       \n\n"
  "Popis:   Program nájde zadané slová,a ostatné znaky vypíše,   \n"
  "         teda vylúšti 8-smerovku \n\n"
  "Popis parametrov:                                             \n"
  "         -h       Zobrazí nápovedu.                           \n\n"
  "         --test data.txt                                      \n"
  "           Skontroluje data zo suboru, ak nedojde k chybe,    \n"
  "           vytlačí maticu na stdout.                          \n\n"
  "         --search=slovo osmismerka.txt                        \n"
  "           Pokúsi sa nájst dané slovo zo vstupného súboru  v osemsmerovke \n\n"
  "         --solve osmismerka.txt slova.txt                     \n"
  "           Pokúsi sa vylúštit osemsmerovku vyškrtávaním slov, \n"
  "           ktoré sa nachádzajú v súbore slova.txt             \n\n"
  "Koniec nápovedy: stlač Ctrl+D v Linuxe                        \n"
  "                 alebo Ctrl+Z vo Windowse                     \n");
}

typedef struct tmatica
  {
  unsigned int R;  //pocet riadkov
  unsigned int S;  //pocet stlpcov
  char *matica; //matica
  } TMatica;

int tiskni(TMatica*mat);


void allocMatica(int R, int S, TMatica *mat) //Alokovanie matice na hromade
{
 if((mat->matica=malloc(sizeof(char)*R*S))==NULL)
    fprintf(stderr , "Chyba: %s\n" , EMSG[EMALLOC]);


   mat->S=S;
   mat->R=R;

}

void deallocMatica(TMatica *mat) //Dealokovanie matice
{
  free(mat->matica);
  mat->matica=NULL;
  //mat->matica=0;

}

int prvyZnak(TMatica *mat,  unsigned int dlzka, char *argv[]);  //Prototyp funkcie ktora najde prvy znak.

int test(char *testFile, unsigned int v,char *argv[] ,unsigned int dlzka)       //Funkcia v ktorej sa otvára,zatvára súbor,alokuje a načítava sa matica.
{
  FILE *subor;

  unsigned int count1=0,count2=0;
  int male_CH,i=0,c;
  unsigned int S,R;

  TMatica x;

if((subor=fopen(testFile, "r")) == NULL)    //Kontroluje ,či sa podarilo otvorit súbor.
   {
     fprintf(stderr, "Chyba: %s\n" , EMSG[EOPEN]);
     return 1;
   }



if(fscanf(subor, "%u %u" ,&R ,&S)!=2)      //Kontroluje, či sú na prvých dvoch miestach čísla
  {
    fprintf(stderr, "Chyba: %s\n" , EMSG[ESUBOR]);
    return 1;
  }

  count1=R*S;                            //Zistujem sucin prvych dvoch cisel.Bude to potrebne pre test ci sa pocet pismen v subore rovna count1
  allocMatica(R,S,&x);                   //Maticu alokujem uz v teste z toho dovodu, ze test je pritomny aj v search aj v solve

  while((c=fgetc(subor)) != EOF)         //Nacitavanie matice.
  {
    if(isalpha(c))                       //Ak sa jedna o znak, count2 sa inkrementuje.
       count2++;                         //Porovna sa s count1 a ak je to zhodne ,znamena to ze je vstupny subor v pozadovanom formate.

    if(c>='a' && c<='z')                //Vo vstupnom subore mozu byt (okrem prvych 2 cisel) iba pismena malej abecedy.
    {
        if(c=='c')                      //Ak sa vyskytne v matici pismeno ch , nahradime ho znakom & , a pri vypise matice ho len zamenime za ch.
        {
            if((male_CH=fgetc(subor))=='h')
            {
                x.matica[i]='&';        //Spominana substitucia.
                i++;
            }
            else
            {
                x.matica[i]=c;
                i++;
            }
        }
        else
        {
            x.matica[i]=c;
            i++;
        }

    }
    else if(c!=32 && c!=10)
    {
    fprintf(stderr, "Chyba: %s\n",EMSG[ESUBOR]);
    return 1;
    }
  }


   if(count1!=count2)                   //Overenie, ci sa vo vstupnom usbore naozaj nachadza pozadovany pocet pismen.
   {
     fprintf(stdout, "Chyba: %s\n" ,EMSG[ESUBOR]);
     return 1;
   }
   switch (v) {                            //Premenna v urcuje aky parameter sa ma vykonat.
       case 1:                             //ak v=1, vykona sa iba test.
         tiskni(&x);
         deallocMatica(&x);
         break;
       case 2:                              //ak v=1, vykona sa search
         prvyZnak(&x ,dlzka,argv);
         tiskni(&x);
         deallocMatica(&x);
       break;

       case 3:                             //ak v=1, vykona sa solve
         tiskni(&x);
         deallocMatica(&x);
       break;
   }
   if(fclose(subor) == EOF)                //Kontroluje či sa súbor zatvoril korektne.
   {
     fprintf(stderr, "Chyba: %s \n" , EMSG[ECLOSE]);
     return 1;
   }


return 0;
}

int tiskni(TMatica*mat)  //Pomocou tejto funkcie sa tlačí matica.
{
  fprintf(stdout, "%u %u\n" , mat->R , mat->S);
  for(unsigned int i=0; i < mat->R ; i++)
  {
    for(unsigned int j=0; j< mat->S; j++)
    {
      if(mat->matica[i*mat->S+j]=='&')
        fprintf(stdout, "%c%c " ,'c','h');
      else
        fprintf(stdout, "%c " , mat->matica[i*mat->S+j]);
    }

    fprintf(stdout, "\n");
  }
return 0;
}

//Hladanie znaku pre 1 smer, Ak zmenime premenne R2 S2 , zmenime smer vyhladavania.Pouzita rekurzia.
int search(TMatica*mat , unsigned int dlzka,  unsigned int R , unsigned int S , unsigned int d , unsigned int R2 , unsigned int S2)
{
    // if(mat->matica[R2*mat->S+S2]==' ')
      if(d==dlzka-1)
         {
           mat->matica[R*mat->S+S]=toupper(mat->matica[R*mat->S+S2]);
             return 0;



          if(search(mat,dlzka,R+R2,S+S2, d+1, R2 ,S2)==0)
          {


            mat->matica[R2*mat->S+S2]=toupper(mat->matica[R2*mat->S+S2]);
            return 0;
          }
         }
return 1;
}


char prve (char *argv[])  //Funkcia vracia prvy znak zo slova ktore sa ma hladat.
{
  int p;
  p=argv[1][9];

  return p;
}

int prvyZnak(TMatica *mat,  unsigned int dlzka , char *argv[])   //Hladanie prveho znaku,a nasledne dalsich.
{
    unsigned int k=0;

/*    for(unsigned int i=0;i< mat->R*mat->S ;i++)
*    {
*        if(mat->matica[i]==argv[1][9+k] && k<dlzka)
*        {
*          for(unsigned q=i; q< (mat->R*mat->S) ;q++)
*          {
*
*           if(mat->matica[q]==argv[1][9+k]) {
*             if((k-i!=dlzka) && (q% (mat->S)) !=0)
*             {
*               mat->matica[q]=toupper(mat->matica[q]);
*               k++;
*             } }
*            else break;
*
*          }
*        }
*    }
*/

    for(unsigned int i=0; i< mat->R*mat->S;i++)
    { if(((i)%(mat->R))!=0 && k!=dlzka)
     if(mat->matica[i]==argv[1][9+k])
      {
       //  if(k!=dlzka)
       //  {
           //if((i%(mat->S))!=0)
           mat->matica[i]=toupper(mat->matica[i]);
           k++;
         //}
       //  else break;
      }
    }
    return 0;

}
int main(int argc, char *argv[] )  //Funkcia ktorá spracúva parametre príkazového riadku.
{
  int i=9;
  unsigned int dlzka=0;

if(argc > 1 && argc < 5)
{
  if(argc==2)
  {
    if(strcmp("-h" , argv[1]) == 0)       //Zobrazí nápovedu.
    {
      printHelp();
      return 0;
    }
    else
    {
      fprintf(stderr, "Chyba: %s\n" , EMSG[EFORMAT]);
      return 1;
    }
  }
  else if(argc==3)
  {
   if(strcmp("--test" , argv[1]) == 0)    //Skontroluje či je matica v správnom tvare, ak áno, vytlačí ju.
   {

      if((test(argv[2],1,argv,0))==0)
        return 0;
   }

  if(strncmp("--search=" , argv[1] ,9) == 0) //Pokusi sa najst v 8-smerovke slovo ktore nasleduje za parametrom --search=
  {

       if(argv[1][i]=='\0')
        {
            fprintf(stderr, "Chyba: %s\n", EMSG[ESTRING]);
            return 1;
        }

       else if(argv[1][i]>='a' && argv[1][i]<='z')  //Retazec moze byt iba pismeno malej abecedy
        {
          dlzka=strlen(argv[1])-9;     //Urci dlzku retazca,asi vymazat

          if((test(argv[2],2,argv,dlzka))==0)

            while(argv[1][i]!='\0')  //Vymazat!!!!!!!!!!!!!!!!!!!!!!!!!
            {
              fprintf(stdout, "%c" ,argv[1][i]);
              i++;
            }

          fprintf(stdout, "\n");
      return 0;
        }
   else
        {
        fprintf(stderr, "Chyba: %s\n" ,EMSG[ESTRING]);
        return 1;
        }
  }

  else
  {
    fprintf(stderr, "Chyba: %s\n" ,EMSG[EFORMAT]);
    return 1;
  }
  }
  else if(argc==4)
  {
    if(strcmp("--solve" , argv[1]) == 0)  //Pokúsi sa vyškrtávaním slov vylúštit osemsmerovku.
     {
         if((test(argv[2],3,argv,0))==0)

           return 0;

         else
         {
           fprintf(stderr, "Chyba: %s\n" , EMSG[EFORMAT]);
           return 1;
         }
     }

    else
    {
      fprintf(stderr, "Chyba: %s\n" ,EMSG[EFORMAT]);
      return 1;
    }
  }
}

    else if(argc==1)
    {
      fprintf(stderr, "Chyba: %s" ,EMSG[ENUMBER]);
      return 1;
    }
    else if(argc>=5)
    {
      fprintf(stderr, "Chyba: %s" ,EMSG[ENUMBERS]);
      return 1;
    }


  return 0;
}
