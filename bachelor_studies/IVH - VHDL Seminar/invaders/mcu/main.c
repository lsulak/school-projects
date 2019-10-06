#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <fitkitlib.h>
#include <lcd/display.h>
#include <thermometer/thermometer.h>

#include "constants.h"
#include "VGA_graphics.h"

//Globalne premenne,ktore sa tykaju samotneho behu hry
unsigned int moving; //rychlost pohybu lode
bool moveORnot = false; //T/F premenna, ktora urcuje ci sa ma pohyb vykonat alebo nie
bool shooting = false;
bool koniec_hry = true;
bool zaciatok_hry = false;

//Help
void print_user_help(void) {
    term_send_str_crlf("RESTART ........ pre restartovanie hry");
}

//Generuje lod , ktora sa bude moct ovladat - funkcia, ktora dodava parametre, kde lod zacne- na predposlednom riadku(posledny je rám), uprostred X-ovej osy.
static inline void ship(coord_t *x, coord_t *y) {
    *x = 17; 
    *y = 27;
}

//Beh hry, vykreslenie objektov, casovanie, klavesy z FITKITu maju pridelene akcie
void play(void) {

    coord_t x;
    coord_t y;
    color_t color = 0xFFFF; //Lod bude seda

    ship(&x, &y); 
    printShip(x,y, color,false);
    printInvaders();
   
    koniec_hry = false;
    zaciatok_hry = false;

    moving = 0xFFFF;
    //Povolit casovac
    CCTL0 = CCIE;
    CCR0 = moving;
    TACTL = TASSEL_1 + MC_2;

    set_led_d5(0);

    //Zaciatok hry
    while (!koniec_hry)  {
        terminal_idle();

        //FITKIT klavesy-pridelenie akcii
        unsigned short actions = kbstate();
        if (actions != ACT_NONE) { 
            eraseShip(x, y); //Pri kazdom pohybe sa vymazu predchadzajuce bloky (zmenia farbu na ciernu)

            if (actions & ACT_LEFT) { //Ak sa stlaci 4, posunie sa objekt dolava
                if (isPossiblePlace(x-1, y, true)) { 
                    x -= 1;
                }
            }

            if (actions & ACT_RIGHT) { //Ak sa stlaci 6, posunie sa objekt doprava
                if (isPossiblePlace(x+1, y, true)) {
                    x += 1;
                }
            }

            if (actions & ACT_UP) {
                if (isPossiblePlace(x, y-1, true)) { //Ak sa stlaci 2, posunie sa objekt hore
                    y -= 1;
                }
            }
            if (actions & ACT_DOWN) {
                if (isPossiblePlace(x, y+1, true)) { //Ak sa stlaci 8, posunie sa objekt dolu
                    y += 1;
                }
            }

            if (actions & ACT_FIRE) { //Ak sa stlaci 5, vysle sa strela, ktora si pameta suradnice objektu s ktorym pohybujeme   
               shooting = true;
               printShip(x,y,color,shooting);    
               shooting = false;  
            }
        }

            printShip(x,y, color, false);

            if (moveORnot) {
            eraseShip(x, y);

            if (isPossiblePlace(x, y-1, true)) {
                y--;
            }
            else { //lod je pri rame
            }

            moveORnot = false;
            printShip(x, y, color, false);
        }
    }
    eraseShip(x, y); //Ak hra skonci-napriklad ak do konzoly napiseme RESTART, vymaze sa nasa "lod" , resp. bloky kde sa nachadzala sa zmenia na cierne.
}

//Komunikacia s terminalom QdevKitu - pri napisani RESTART sa hra restartuje, inak sa vypise napoveda 
unsigned char decode_user_cmd(char *cmd_ucase, char *cmd) {
 
    if (strcmp7(cmd_ucase, "RESTART")) {
        koniec_hry = true;
        zaciatok_hry = true;

        term_send_str_crlf("Nova hra");
        return USER_COMMAND;
    }
    else {
        term_send_str_crlf(" RESTART ........ restartovat hru");
    }

    return CMD_UNKNOWN;
}

//Vykresli obrazovku a inicializuje LCD.je zavolana po inicializacii FPGA
void fpga_initialized(void) {
    VGA_DrawScene();
    LCD_init();
    LCD_write_string("Space Invaders"); //Na LCD FITITU napise tuto hlasku
}


int main(void) {
   //Inicializacia
    initialize_hardware();
    thermometer_init_rand();
    WDG_stop();
    set_led_d5(0);
    term_send_str_crlf("Pre restartovani hry stlacte RESTART");
    //Beh samotnej hry   
    while (1) {
        terminal_idle();

        if (zaciatok_hry) {
            VGA_DrawScene();
            play();
        } else kbstate();
    }
} 