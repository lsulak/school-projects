/* 
 * IMP projekt, varianta MSP430: Otackomer s vystupom na VGA, B
 * Autor: Ladislav Sulak
 * Login: xsulak04 
 * Datum: 15.12.2014
 * Zmeny: 50% obsluha preruseni, klavesnice, VGA inspirovane aplikaciami zo SVN repozitaru
*/

#include <stdio.h>
#include <stdlib.h>

#include <fitkitlib.h>
#include <keyboard/keyboard.h>
#include <lcd/display.h>
#include <string.h>
#include "dataVGA.h"

/* Globalne premenne potrebne pre beh programu */
char last_ch;	
char char_cnt;
long otacky_max;
long otacky_max_tmp;
int otacky_cnt;

/* Hlavna funkcia. Nastavi prerusenia, inicializuje vsetko potrebne
 a spusti nekonecnu smycku ktora bude obsluhovat klavesnicu a terminal
  - budu prichadzat prerusenia: 
  		jedno bude sluzit ako timer kazdu sekundu
  		druhe bude vyuzivat prerusenie od P2 - ak magnet pride do styku so senzorom*/
int main(void)
{
	char_cnt = 0;
	last_ch = 0;
	otacky_cnt = 0;
	otacky_max = 10;			//Defaultna hodnota
	otacky_max_tmp = 0;

	WDTCTL = WDTPW + WDTHOLD;	// zastav watchdog              
	CCTL0 = CCIE;				// povolit prerusenia

	CCR0 = 0x8000;				// prve prerusenie za 1 s                        
	TACTL = TASSEL_1 + MC_2;	// ACKL, continuous mode

	P2OUT &=  0x00;
	P2DIR &=  0x00;

	P2DIR |=  BIT0;			// P2.0 na vystup
	P2OUT |=  BIT0;

	P2IE  |=  BIT3;			// Povolit prerusenie na P2.3  pin 22
	P2IES |=  BIT3;			// pri nastupnej hrane

	P2IFG &= ~BIT3;			// clear interrupt flag 
	_BIS_SR(GIE);

	//Inicializacia HW,FITKIT klavesnice, lcd, a VGA 
	initialize_hardware();
	keyboard_init();
	LCD_init();
	LCD_clear();
	VGA_Clear();
	VGA_Draw();

	LCD_append_string("MAX OTACKY:");
	print_user_help();
	term_send_str_crlf("\nMaximalne otacky:");
	term_send_num(otacky_max);

	while (1)
	{
		keyboard_idle();	// obsluha klavesnice
		terminal_idle();	// obsluha terminalu
	}
}

/* Vymaze VGA, resp prepise ich farbu na 0 - cierna */
void VGA_Clear(void) 
{
	unsigned short i, j;
	unsigned char color = 0;

	for(i=0; i< VGA_ROWS; i++) 
	{
		for(j = 0; j < VGA_COLUMNS; j++) 
		{
			FPGA_SPI_RW_AN_DN(SPI_FPGA_ENABLE_WRITE,
			BASE_ADDR_VRAM + i*VGA_COLUMNS + j, &color, 2, 1);
		}
	}
}

/* Vyfarbi okno aplikacie, farby ziska z pola drawing */
void VGA_Draw(void) 
{
	unsigned short i, j;
	unsigned short v_edge = (VGA_ROWS - DRAW_H)   /2;
	unsigned short h_edge = (VGA_COLUMNS - DRAW_W)/2;
	unsigned char color;

	for(i=0; i < DRAW_H; i++)
	{
		for(j = 0; j < DRAW_W; j++)
		{
			color = drawing[i][j];
			if(color != 0)		// barva ina nez cierna
				FPGA_SPI_RW_AN_DN(
				SPI_FPGA_ENABLE_WRITE,
				BASE_ADDR_VRAM + (i + v_edge) * 128 + j + h_edge,
				&color, 2, 1);
		}
	}
}

/* Funkcia podobna predchadzajucej, no vola sa pri zmene otacok,
 zmeni cislo v percentach a vyfarbi kurzor */
void VGA_ReDraw(int num) 
{
	unsigned short i;
	unsigned short v_edge = (VGA_ROWS - DRAW_H)   /2;
	unsigned short h_edge = (VGA_COLUMNS - DRAW_W)/2;
	unsigned char color = 0;
	unsigned short colorCnt;

	//Na zaciatku vymaze (nastavi na 0-ciernu farbu) kurzor i cislo
	for(i = 22; i > 2; i--)
	{
		FPGA_SPI_RW_AN_DN(
		SPI_FPGA_ENABLE_WRITE,
		BASE_ADDR_VRAM + (i + v_edge) * 128 + 5 + h_edge,
		&color, 2, 1);

		FPGA_SPI_RW_AN_DN(
		SPI_FPGA_ENABLE_WRITE,
		BASE_ADDR_VRAM + (i + v_edge) * 128 + 4 + h_edge,
		&color, 2, 1);
	}
	Vga_Number(0); 

	if(num == 0)
		return;
	
	else
	{
		// zmeni - vyfarbi cislo podla pola prisluchajuceho danemu cislu
		if(num > 100)
		{
			Vga_Number(10); 
			colorCnt = 20;
			color = 13; 
		}
		else if(num <= 100 && num >= 95)
		{
			Vga_Number(10); 
			colorCnt = 20;
			color = 3; 
		}
		else if(num < 95 && num >= 85)
		{
			Vga_Number(9); 
			colorCnt = 18;
			color = 3; 
		}
		else if(num < 85 && num >= 75)
		{
			Vga_Number(8);
			colorCnt = 16;
			color = 3;
		}
		else if(num < 75 && num >= 65)
		{
			Vga_Number(7);
			colorCnt = 14;
			color = 3;
		}
		else if(num < 65 && num >= 55)
		{
			Vga_Number(6);
			colorCnt = 12;
			color = 3;
		}
		else if(num < 55 && num >= 45)
		{
			Vga_Number(5);
			colorCnt = 10;
			color = 9;
		}
		else if(num < 45 && num >= 35)
		{
			Vga_Number(4);
			colorCnt = 8;
			color = 9;
		}
		else if(num < 35 && num >= 25)
		{
			Vga_Number(3);
			colorCnt = 6;
			color = 9;
		}
		else if(num < 25 && num >= 15)
		{
			Vga_Number(2);
			colorCnt = 4;
			color = 9;
		}
		else if(num < 15 && num > 0)
		{
			Vga_Number(1);
			colorCnt = 2;
			color = 9;
		}
		
		// kurzor
		for(i = 22; i > 22-colorCnt; i--)
		{
			FPGA_SPI_RW_AN_DN(
			SPI_FPGA_ENABLE_WRITE,
			BASE_ADDR_VRAM + (i + v_edge) * 128 + 5 + h_edge,
			&color, 2, 1);

			FPGA_SPI_RW_AN_DN(
			SPI_FPGA_ENABLE_WRITE,
			BASE_ADDR_VRAM + (i + v_edge) * 128 + 4 + h_edge,
			&color, 2, 1);
		}
	}
}


/* Funkcia ktora zmeni cislo v percentach, 
   pocita sa podla vzorca aktualne (otacky/maximalne)*100 */
void Vga_Number(short num)
{
	unsigned short i, j;
	unsigned short v_edge = (VGA_ROWS - DRAW_H)   /2;
	unsigned short h_edge = (VGA_COLUMNS - DRAW_W)/2;
	unsigned char color;

	//Vykreslovanie cislice, teda zmena farby na danom pixelu, 
	//farby su ulozene v poliach num0-num10
	for(i = 10; i < 17; i++)
	{
		for(j = 14; j < 18; j++)
		{
			switch(num)
			{
				case 0:
					color = num0[i-10][j-14];
					break;
				case 1:
					color = num1[i-10][j-14];
					break;
				case 2:
					color = num2[i-10][j-14];
					break;
				case 3:
					color = num3[i-10][j-14];
					break;
				case 4:
					color = num4[i-10][j-14];
					break;
				case 5:
					color = num5[i-10][j-14];
					break;
				case 6:
					color = num6[i-10][j-14];
					break;
				case 7:
					color = num7[i-10][j-14];
					break;
				case 8:
					color = num8[i-10][j-14];
					break;
				case 9:
					color = num9[i-10][j-14];
					break;
				case 10:
					color = num10[i-10][j-14];
					break;
				default:
					break;
			}
			if(num == 0) //pri nulovych otackach treba nastavit cislo do 0 - najlavejsia cifra
				FPGA_SPI_RW_AN_DN(
				SPI_FPGA_ENABLE_WRITE,
				BASE_ADDR_VRAM + (i + v_edge) * 128 + j-5 + h_edge,
				&color, 2, 1);

			if(num == 10) //pri 100% otackach treba nastavit cislo do 1 - najlavejsia cifra, ostatne ostanu 0
				FPGA_SPI_RW_AN_DN(
				SPI_FPGA_ENABLE_WRITE,
				BASE_ADDR_VRAM + (i + v_edge) * 128 + j-5 + h_edge,
				&color, 2, 1);

			else
				FPGA_SPI_RW_AN_DN(
				SPI_FPGA_ENABLE_WRITE,
				BASE_ADDR_VRAM + (i + v_edge) * 128 + j + h_edge,
				&color, 2, 1);
		}
	}
}

/* Zobrazi napovedu, aj hned po zapnuti aplikacie */ 
void print_user_help(void) 
{
	term_send_str_crlf("		Aplikacia otackomer");
	term_send_str_crlf("	Frekvencia otacok bude zakreslena na VGA");
	term_send_str_crlf("	Pre zadanie maximalnej hodnoty:");
	term_send_str_crlf(" 		- maximalne 5-ciferne cislo,"); 
	term_send_str_crlf("		  zadajte pomocou klavesnice FITKIT");
	term_send_str_crlf(" 		- potvrdte znakom '#'");
	term_send_str_crlf(" 	Pripojte na JP9-port22, JP11-3.3V a JP1-GND");
}

unsigned char decode_user_cmd(char *cmd_ucase, char *cmd)
{
	return CMD_UNKNOWN;
}

void fpga_initialized() {}


/* Obsluha poziadaviek z klavesnice FITKITu */
int keyboard_idle()
{
	char ch;
	ch = key_decode(read_word_keyboard_4x4());
	// osetrenie drzania klavesy
	if (ch != last_ch)
	{
		last_ch = ch;
		if(char_cnt > 5 && ch != '#') //Pocet MAX otacok moze byt max 5-ciferny
		{
			LCD_clear();
			LCD_append_string("MAX OTACKY:");
			char_cnt = 0;
			otacky_max_tmp = 0;
		}

		// Zadanie hodnoty pre max otacky
		if((ch >= '1' && ch <= '9') || ch == '0')
		{
			if(char_cnt > 0) //Predoslu cislicu posunie do vyssieho radu a
				otacky_max_tmp *= 10;
			
			otacky_max_tmp += (ch - '0'); //Aktualnu cislicu pripocita, prevedie na long
			LCD_append_char(last_ch);
			char_cnt++;
		}
		// nastavenie max otacok
		if (ch == '#')
		{
			LCD_clear();
			LCD_append_string("MAX OTACKY:");
			char_cnt = 0;

			if(otacky_max_tmp == 0) //Defaultne bude MAX 10
				otacky_max = 10;
			else
				otacky_max = otacky_max_tmp;

			term_send_str_crlf("\nMaximalne otacky:");
			term_send_num(otacky_max);
			otacky_max_tmp = 0;
		}	
	}
	return 0;
}

/* Obsluha prerusenia od P2. Ak k nemu dojde, zvysi sa pocet otacok o 1 */
interrupt (PORT2_VECTOR) port2interrupt (void)
{
		otacky_cnt++;
		delay_ms(10);
		P2IFG &= ~BIT3;		// clear interrupt flag 
		
}

/* Obsluha prerusenia od casovaca, kazdu sekundu */
interrupt (TIMERA0_VECTOR) Timer_A (void)
{
	int percenta = ((int)(((double)otacky_cnt * 100)/ (double)otacky_max)+0.5);

	//Vypis aktualne otacky, ale iba ak nieje 0 - redukovanie dat
	if(otacky_cnt != 0)
	{
		term_send_str_crlf("\nOtacky:");
		term_send_num(otacky_cnt);
	}
	otacky_cnt = 0;
	CCR0 += 0x8000;
	VGA_ReDraw(percenta); //na VGA vykresli aktualny kurzor a cislo podla percenta
	delay_ms(10);
}
