#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fitkitlib.h>
#include <keyboard/keyboard.h>
#include <lcd/display.h>

#include "constants.h"
#include "VGA_graphics.h"

//Odstartovanie hry bez pripojeneho terminalu
extern bool zaciatok_hry;

//Zmeni hodnotu vo VideoRAM na urcity pixel
void VGA_SetPixelXY(const coord_t x, const coord_t y, color_t color) {
  //ak niektore hodnoty x alebo y budu siahat mimo pamat, preskoci ich
  if (x >= VGA_COLUMNS || y >= VGA_ROWS) {
      return;
  }

  unsigned short vga_pos = y*VGA_STRIDE + x;
  FPGA_SPI_RW_AN_DN(SPI_FPGA_ENABLE_WRITE, BASE_ADDR_VRAM /*base*/ + vga_pos, &color, 2, 1);
}

 //Donutra rámu vytlaci pixel ,suradnice a farby su ako parametre
void VGA_SetBoardPointXY(const coord_t x, const coord_t y, const color_t color) {
    coord_t bx = VGA_COL_START+1+2*x;
    coord_t by = VGA_ROW_START+1+2*y;

    VGA_SetPixelXY(bx,   by,   color);
    VGA_SetPixelXY(bx+1, by,   color);
    VGA_SetPixelXY(bx,   by+1, color);
    VGA_SetPixelXY(bx+1, by+1, color);
}

//Vykreslí rám , ktorý bude po obvode obrazovky
void VGA_DrawScene(void) {
  short i;

  for (i=0;i<VGA_COLUMNS;i++) {
      VGA_SetPixelXY(i,0, 1);
      VGA_SetPixelXY(i,VGA_ROWS-1, 1);
  }
  for (i=0;i<VGA_ROWS;i++) {
      VGA_SetPixelXY(0,i, 1);
      VGA_SetPixelXY(VGA_COLUMNS-1,i, 1);
  }
}

//Funkcia ktora vykresli lode invaderov  - cervene kocky
void printInvaders(void) {
//Konstanty pre farby - cierna,cervena
#define S0 VGA_BLACK
#define S1 VGA_COLOR

#define SPD_WIDTH  61 //   Pocet Invaderov: (61-1)/(3+1)= 15 .....   -1  a  +1 kvoli medzeram na zac. aj na konci
#define SPD_HEIGHT 3

    const color_t speedDrawing[SPD_HEIGHT][SPD_WIDTH] = {
        {S0, S1, S1, S1, S0, S1, S1, S1, S0, S1, S1, S1, S0, S1, S1, S1, S0, S1, S1, S1, 
         S0, S1, S1, S1, S0, S1, S1, S1, S0, S1, S1, S1, S0, S1, S1, S1, S0, S1, S1, S1,
         S0, S1, S1, S1, S0, S1, S1, S1, S0, S1, S1, S1, S0, S1, S1, S1, S0, S1, S1, S1,S0}, 

        {S0, S1, S1, S1, S0, S1, S1, S1, S0, S1, S1, S1, S0, S1, S1, S1, S0, S1, S1, S1, 
         S0, S1, S1, S1, S0, S1, S1, S1, S0, S1, S1, S1, S0, S1, S1, S1, S0, S1, S1, S1,
         S0, S1, S1, S1, S0, S1, S1, S1, S0, S1, S1, S1, S0, S1, S1, S1, S0, S1, S1, S1,S0}, 

        {S0, S1, S1, S1, S0, S1, S1, S1, S0, S1, S1, S1, S0, S1, S1, S1, S0, S1, S1, S1, 
         S0, S1, S1, S1, S0, S1, S1, S1, S0, S1, S1, S1, S0, S1, S1, S1, S0, S1, S1, S1, 
         S0, S1, S1, S1, S0, S1, S1, S1, S0, S1, S1, S1, S0, S1, S1, S1, S0, S1, S1, S1,S0}, 
          
    };

    coord_t y;
    coord_t x;

    //Vykreslenie lode invaderov
    for (y = 0; y < SPD_HEIGHT; y++) {
        for (x = 0; x < SPD_WIDTH; x++) {
            VGA_SetPixelXY(VGA_SPEED_COL_START+x+8, VGA_SPEED_ROW_START+y+13, speedDrawing[y][x]);
        }
    }
}

///Konstanty pre klavesy
#define UNPRESSED_KEY 0x0000
#define PRESSED_LONG_ENOUGH 0x0851

 //Nastavi hodnotu klavesy do aktualnej hodnoty TARu, parameter since- cas do obnovy
static inline void setPressedTime(uint16_t *since) {
    *since = TAR;
    if (*since == UNPRESSED_KEY) {
        *since += 1;
    }
}

 //Funkcia pre obsluhu klavesnice
 //keyboardState-vektor stlacenych klaves
 //action-vysledna akcia, ak je stlacena klavesa
 //since-posledny krat co bola nastavena klavesa
static inline unsigned char handleKey(const unsigned short keyboardState, const unsigned short key, const unsigned char action, uint16_t *since) {
    if (keyboardState & key) { //stlacena klavesa
        
        if (*since == UNPRESSED_KEY) { //prvy krat stlacena klavesa
            setPressedTime(since);
            return ACT_NONE;
        }
        else if (TAR - *since > PRESSED_LONG_ENOUGH) {
            //klavesa je stlacena prilis dlho
            setPressedTime(since);
            return action;
        }
    }
    else if (*since != UNPRESSED_KEY && TAR - *since > PRESSED_LONG_ENOUGH) {
        //klavesa nieje stlacena , ale bola prilis dlho drzana
        *since = UNPRESSED_KEY;
        return action;
    }
    else {
        //klavesa nieje stlacena a ani nebola dlho drzana
        *since = UNPRESSED_KEY;
        return ACT_NONE;
    }

    return ACT_NONE;
}

//Ziska aktualny status klavesnice, vracia bitovu masku aktivnych klaves
unsigned char kbstate(void) {
    static uint16_t pressedSince[10] = {UNPRESSED_KEY, UNPRESSED_KEY, UNPRESSED_KEY, UNPRESSED_KEY, UNPRESSED_KEY};

    unsigned short keyboardState = read_word_keyboard_4x4();
    unsigned char result = ACT_NONE;

    if (!zaciatok_hry) // stisk hociakej klavesy zahaji hru
    {
        zaciatok_hry = keyboardState;
        return ACT_NONE;
    }
    
    result |= handleKey(keyboardState, KEY_2, ACT_UP, &pressedSince[4]);
    result |= handleKey(keyboardState, KEY_4, ACT_LEFT, &pressedSince[0]);
    result |= handleKey(keyboardState, KEY_5, ACT_FIRE, &pressedSince[3]);
    result |= handleKey(keyboardState, KEY_6, ACT_RIGHT, &pressedSince[1]);
    result |= handleKey(keyboardState, KEY_8, ACT_DOWN, &pressedSince[2]);
    

    return result;
}