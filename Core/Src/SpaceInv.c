/*
 * SpaceInv.c
 *
 *  Created on: Jun 25, 2024
 *      Author: Miron Andrei
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "st7735.h"
#include "GFX_FUNCTIONS.h"
#include "fonts.h"
#include "main.h"
#include "SpaceInv.h"

typedef struct Entitate
{
	int16_t x;
	int16_t y;

}Entitate;

bool ok=0;


Entitate entitate[20];//entitate[0] va fi userul

void initializare_ecran()
{
	ST7735_FillScreenFast(ST7735_BLACK);
}

void initializare_racheta_jucator()
{
	uint8_t i;
	uint8_t j;
	//incepem de la 8 pixeli fata de jos

	for(i=0;i<8;i++)
	{
		for(j=0;j<8;j++)
		{
			if((i==0 && j==0) || (i==0 && j==1) || (i==0 && j==2) || (i==0 && j==3) || (i==0 && j==5) || (i==0 && j==6) ||
			   (i==1 && j==3) || (i==1 && j==5) || (i==2 && j==3) || (i==2 && j==5) || (i==2 && j==7) || (i==3 && j==1) ||
			   (i==3 && j==2) || (i==3 && j==3) || (i==3 && j==4) || (i==3 && j==5) || (i==3 && j==6) || (i==3 && j==7) ||
			   (i==4 && j==1) || (i==4 && j==2) || (i==4 && j==3) || (i==4 && j==4) || (i==4 && j==5) || (i==4 && j==6) ||
			   (i==4 && j==7) || (i==5 && j==3) || (i==5 && j==5) || (i==5 && j==7) || (i==6 && j==3) || (i==6 && j==5) ||
			   (i==7 && j==0) || (i==7 && j==1) || (i==7 && j==2) || (i==7 && j==3) || (i==7 && j==5) || (i==7 && j==6) )

			{
				//coloreaza pixelul albastru
				ST7735_DrawPixel(entitate[0].x+i,entitate[0].y+j, ST7735_BLUE);
			}

			else
			{
				//coloreaza negru
				ST7735_DrawPixel(entitate[0].x+i,entitate[0].y+j, ST7735_BLACK);
			}
		}
	}

}

void refresh_jucator(uint8_t x, uint8_t y)
{
	uint8_t i;
	uint8_t j;

	for(i=x;i<x+8;i++)
	{
		for(j=y;j<y+8;j++)
		{
			ST7735_DrawPixel(i,j, ST7735_BLACK);
		}
	}
}

void pixel_adversar(uint8_t x, uint8_t y)
{
	uint8_t i;
	uint8_t j;

	for(i=0;i<8;i++)
	{
		for(j=0;j<8;j++)
		{
			if( (i==2 && j==1) || (i==3 && j==1) || (i==4 && j==1) || (i==5 && j==1) || (i==1 && j==2) || (i==2 && j==2) ||
				(i==3 && j==2) || (i==4 && j==2) || (i==5 && j==2) || (i==6 && j==2) || (i==1 && j==3) || (i==3 && j==3) ||
				(i==4 && j==3) || (i==6 && j==3) || (i==1 && j==4) || (i==2 && j==4) || (i==3 && j==4) || (i==4 && j==4)
				|| (i==5 && j==4) || (i==6 && j==4) || (i==2 && j==5) || (i==3 && j==5) || (i==4 && j==5) || (i==5 && j==5)
				|| (i==1 && j==6) || (i==3 && j==6) || (i==4 && j==6) || (i==6 && j==6))
			{
				ST7735_DrawPixel(x+i,y+j, ST7735_WHITE);
			}

			else

			{
				ST7735_DrawPixel(x+i,y+j, ST7735_BLACK);
			}
		}
	}
}

void initializare_adversar_nivel1()
{
	uint8_t i;

	uint8_t nr_Entitati=4;

	entitate[1].x=8;
	entitate[1].y=8;

	entitate[2].x=40;
	entitate[2].y=8;

	entitate[3].x=72;
	entitate[3].y=8;

	entitate[4].x=104;
	entitate[4].y=8;

	for(i=1;i<=nr_Entitati;i++)
	{
		pixel_adversar(entitate[i].x,entitate[i].y);
	}
}


void initializare_nivel_1()
{
	//primul nivel va fi cu 4 adversari fiecare divizat la o distanta pe aceeasi linie 64,112
	entitate[0].x=64;
	entitate[0].y=112;

	initializare_racheta_jucator();
	initializare_adversar_nivel1();
}

void fizica_jucator()
{

	if((entitate[0].x=!0) && (entitate[0].x!=120))
	{

		if(apasatDR==1 && apasatSTG==0)
		{
			refresh_jucator(entitate[0].x,entitate[0].y);
			entitate[0].x=entitate[0].x+8;
			initializare_racheta_jucator();
		}

		if(apasatDR==0 && apasatSTG==1)
		{
			entitate[0].x=entitate[0].x-8;
			initializare_racheta_jucator();
			refresh_jucator(entitate[0].x+8,entitate[0].y);
		}

		apasatDR=0;
		apasatSTG=0;
	}

}

void motor_joc()
{
	if(ok==0)
	{
		initializare_nivel_1();
		ok=1;
	}

	fizica_jucator();
}

























