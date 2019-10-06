#ifndef __VGA_GRAPHICS_H__
#define __VGA_GRAPHICS_H__

#include "constants.h"

//Rozmery VIDEORAM
#define VGA_COLUMNS 80
#define VGA_ROWS    60

#define VGA_BLACK 0 //Konstanta pre ciernu farbu pixelu
#define VGA_COLOR 2 //Konstanta pre cervenu farbu pixelu
    
//Zaciatok suradnicoveho systemu pre obrazovku
#define SCREEN_COL_START 0     //pociatocna poloha pre osu X , v podstate zaistuje posuv...1kvoli rámu
#define SCREEN_ROW_START 0     //pociatocna poloha pre osu Y , v podstate zaistuje posuv 

//Pre vykreslenie pixelov vnutri rámu
#define VGA_COL_START SCREEN_COL_START
#define VGA_COL_END VGA_COL_START

#define VGA_ROW_START SCREEN_ROW_START
#define VGA_ROW_END VGA_ROW_START

//Pre vykreslenie lode invaderov
#define VGA_SPEED_COL_START SCREEN_COL_START
#define VGA_SPEED_ROW_START SCREEN_ROW_START

//Prototypy funkcii
void VGA_SetPixelXY(const coord_t x, const coord_t y, const color_t color);
void VGA_SetBoardPointXY(const coord_t x, const coord_t y, const color_t color);
void VGA_DrawBoardLine(const coord_t y, const boardline_t boardline);
void VGA_DrawScene(void);
void printInvaders(void);

//Akcie pre Klavesy na fitkite
#define ACT_NONE  0x0
#define ACT_UP  0x1 
#define ACT_LEFT  0x2 
#define ACT_RIGHT 0x4
#define ACT_DOWN 0x8
#define ACT_FIRE 0x20

unsigned char kbstate(void);

#endif
