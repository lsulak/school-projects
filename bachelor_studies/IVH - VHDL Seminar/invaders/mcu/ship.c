#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include <fitkitlib.h>
#include <lcd/display.h>
#include <thermometer/thermometer.h>

#include "constants.h"
#include "VGA_graphics.h"


//Staticka premenna pre rám
static boardline_t board[10] = {0};
#define FULL_LINE 0xFFF

//Struktura pre blok
typedef struct {
    unsigned char shape[2];
} block_definition_t;

//Bitmapova maska bloku, je zakodovana do hexadecimalneho kodu.Tento Blok sluzi ako nasa "lod" , s ktorou sa budeme vediet pohybovat
const block_definition_t blocks[1] = { //Spravene v podstate pole, pretoze je mozne ze este budem pridavat nejake objekty.
   { {0x4E}, 1 }, 
};


//Umiestni nasu lod na suradniciach ktore su dodane ako parametre, s farbou ktora je tiez ako parameter.....bude seda
void printShip(const coord_t x, const coord_t y, const color_t color, bool shooting) {
    coord_t by, bx;

  if(!shooting) {
    for (by = 0; by < 4; by++) {
        for (bx = 0; bx < 4; bx++) {
            unsigned short index = by < 2 ? 0 : 1;
            unsigned short bit_index = ( by == 0 || by == 2 ) ? 4 : 0;
            bit_index += 4-bx-1;

            if ( (blocks[0].shape[index] >> bit_index) & 1 ) 
            {
                VGA_SetBoardPointXY(x+bx, y+by, color);
            }
        }
    }
   } 
    else
    {
        color_t color =  0xFFFD;
        shoot(x,y,color);
   // VGA_SetBoardPointXY(x+1, y-1, VGA_BLACK); 
    }
}

//Funkcia ktora je taka ista ako predosla, resp. printShip - len s tym rozielom, ze vytvara cierne bloky, inymi slovami ich maze-hodi sa pri pohybe, alebo restarte
void eraseShip(const coord_t x, const coord_t y) {
            printShip(x,y, VGA_BLACK,false);
}

//Kontroluje, ci je mozny posuv po X-ovej ose - aby sme sa nedostali "lodou" mimo obrazovky alebo ramu
bool isPossiblePlace(const coord_t x, const coord_t y,bool fireORmove) {
    coord_t by, bx;
    if(fireORmove) { //Pre pohyb lode
        for (bx = 0; bx < 4; bx++) {
            if ( x + bx >= 39 || y>27 || y<15) { //kontrola, ci je mozny posuv po xovej ose - aby sme nesli "lodou" mimo obrazovky alebo ramu
                return false;                    //posledna podmienka, y<15 znamena, ze sa nemozem posunut do prvych 15 blokov, pretoze tam su 
            }                                    //umiestnene lode invaderov a nebolo by vhodne aby som si tam poletoval
        }
        return true;

    }
    else 
    {                                     //Pre strelbu- strela sa pohybuje po celej obrazovke nie len po 15 bloku
        for (bx = 0; bx < 4; bx++) {
            if ( x + bx >= 39 || y>27 ) { //kontrola, ci je mozny posuv po xovej ose - aby sme nesli "lodou" mimo obrazovky alebo ramu
                return false;                     
            }                                    
        }
        return true;
    }
}

//Strela nieje spravena ako na vacsine hier, teda jeden pohybujuci sa objekt smerom hore, na zaklade nejakeho casoveho kvanta, ale jeden objekt,resp.blok ktory sa 
//cyklom dostane uplne hore az po rám, a dalsim cyklom su od dola hore vymazane jednotlive pixely.Dovod-dizan....namiesto jednej strely to vyzera ako paprsok
void shoot(const coord_t x,const coord_t y, const coord_t color) {
   x=x+1;
   while(isPossiblePlace(x, y, false)) {  //Cyklus , ktory vysle luc, od lode ktoru sa da ovladat
        y=y-1;
        VGA_SetBoardPointXY(x, y, color);

    }

    int pom;
    for(pom=0;pom<10000;pom++); //V podstate sa nic nestane, iba sa trosku zdrzi cas, na odkreslenie ciernych blokov- strela vyzera ako mocny paprsok
   
    int pomoc;
    y=27;
  
   for(pomoc=0; pomoc <= 27; pomoc++) //cyklus, ktory zaisti, ze sa po sebe bude "cistit" , teda aby luc v predchadzajucom cykle neostal na obrazovke, aby tam bolo znovu cierna plocha
        VGA_SetBoardPointXY(x, y--, VGA_BLACK);
}