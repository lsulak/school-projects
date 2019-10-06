#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#include <stdbool.h>

//Konstanty, typy, prototypy funkcii- setrenie miesta, prehladnost.
typedef unsigned char coord_t;
typedef unsigned char color_t;
typedef uint16_t boardline_t;
typedef unsigned short block_t;

//Rozmery
#define BASE_ADDR_VRAM 0x8000
#define SPI_FPGA_ENABLE_READ  0x02
#define VGA_COLUMNS 80
#define VGA_ROWS    60
#define VGA_STRIDE  128

//Prototypy
void printShip(const coord_t x, const coord_t y, const color_t color, bool shoot);   
void eraseShip(const coord_t x, const coord_t y);
bool isPossiblePlace(const coord_t x, const coord_t y, bool fireORmove);  
void shoot(const coord_t x,const coord_t y, const coord_t color);      

#endif
