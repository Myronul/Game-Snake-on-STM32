/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "SpaceInv.h"
#include "st7735.h"
#include "GFX_FUNCTIONS.h"
#include "fonts.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

bool optiuneJoc=0;

//-----------------------------------------Jocul Snake------------------------------------------------------------------------------

typedef struct
{
  int16_t x;
  int16_t y;

}Sarpe;

Sarpe sarpe[255];
uint8_t temphr=0;//hrana spawn
//uint8_t uart_buf[1];

uint8_t latime=128;
uint8_t lungime=128;

uint8_t scor=2; //sarpele incepe cu 3 elemente adica 0 1 si 2
uint8_t randx=0;
uint8_t randy=0;
uint8_t ok1=0;
uint8_t game_over=0;

uint8_t apasat12=0;//sus
uint8_t apasat13=0;//jos
uint8_t apasat14=0;//dreapta
uint8_t apasat15=0;//stanga
uint8_t intrerupere=0;
uint16_t timp=0;

//------------------------------------------------------------------Jocul SpaceInvaders---------------------------------------------

uint8_t apasatDR=0;
uint8_t apasatSTG=0;


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart1;

PCD_HandleTypeDef hpcd_USB_FS;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM2_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USB_PCD_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim2)
{
	timp++;//timerul se apeleaza de 100 de ori pe secunda deci
		  //o secunda trece cand timp ajunge la 100 analog 50 pentru 500 ms
		 //Nu este cazul pentru Overflow deoarece variabila se incrementeaza doar de 100 de ori pe secunda
		//Pentru 30 de secunde brb am avea 3000 maximul pe 16 biti unsigned fiind 65.536-1
}

void alege_joc_interfata()
{
	//Ecranul e deja negru

	ST7735_WriteString(0, 8, " Selectati jocul!", Font_7x10, ST7735_WHITE, ST7735_BLACK);//18 spatii pe linie
	ST7735_WriteString( 49, 40, "Snake", Font_7x10, ST7735_WHITE, ST7735_BLACK);
	ST7735_WriteString( 21, 80, "Space Invaders", Font_7x10, ST7735_WHITE, ST7735_BLACK);

	while(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)!=0)
	{
		if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12)==1 && HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13)==0)//SUS-Selectat snake
		{
			ST7735_WriteString( 49, 40, "Snake", Font_7x10, ST7735_RED, ST7735_BLACK);
			ST7735_WriteString( 21, 80, "Space Invaders", Font_7x10, ST7735_WHITE, ST7735_BLACK);
			optiuneJoc=0;//snake
		}

		if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13)==1 && HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12)==0)//SUS-Selectat snake
		{
			ST7735_WriteString( 49, 40, "Snake", Font_7x10, ST7735_WHITE, ST7735_BLACK);
			ST7735_WriteString( 21, 80, "Space Invaders", Font_7x10, ST7735_BLUE, ST7735_BLACK);
			optiuneJoc=1;//SpaceInv
		}
	}

}

void pixel_sarpe(int16_t x,int16_t y)//deseneaza pixelul corpului sarpelui
{
   uint8_t i=0,j=0;

   for(i=x;i<(x+8);i++)
   {
      for(j=y;j<(y+8);j++)
      {
    	  ST7735_DrawPixel(i, j, ST7735_GREEN);
      }
   }
}


void pixel_hrana(int16_t x,int16_t y)//deseneaza pixelul corpului sarpelui
{
   uint8_t i=0,j=0;

   for(i=x;i<(x+8);i++)
   {
      for(j=y;j<(y+8);j++)
      {
    	  ST7735_DrawPixel(i, j, ST7735_RED);
      }
   }
}

void update_sarpe()//il stergem pt un scurt moment pe tot pentru a redesena cu coorodnatele actualizate
{
  uint8_t i,j,z;

  for(i=0;i<=scor;i++)//lungimea curenta a sarpelui
  {
    for(j=sarpe[i].x;j<(sarpe[i].x+8);j++)
   {
      for(z=sarpe[i].y;z<(sarpe[i].y+8);z++)
      {
    	  ST7735_DrawPixel(j, z, ST7735_BLACK);
      }
   }

  }

}

void deseneaza_sarpe()
{

  for(uint8_t i=0;i<=scor;i++)
  {

	if(i==0) //Daca desenam capul, ii punem ochi :)
	{
		uint8_t k=0;
		uint8_t l=0;

		for(k=sarpe[0].x;k<(sarpe[0].x+8);k++)
		{
			for(l=sarpe[0].y;l<(sarpe[0].y+8);l++)
			{
				if( (k==1+sarpe[0].x && l==1+sarpe[0].y) || (k==1+sarpe[0].x && l==2+sarpe[0].y) ||
					(k==2+sarpe[0].x && l==1+sarpe[0].y) || (k==2+sarpe[0].x && l==2+sarpe[0].y) ||
					(k==5+sarpe[0].x && l==1+sarpe[0].y) || (k==6+sarpe[0].x && l==1+sarpe[0].y) ||
					(k==5+sarpe[0].x && l==2+sarpe[0].y) || (k==6+sarpe[0].x && l==2+sarpe[0].y))
				{
					ST7735_DrawPixel(k, l, ST7735_RED);
				}

				else
				{
					ST7735_DrawPixel(k, l, ST7735_GREEN);
				}
			}
		}
	}

	else
	{
		pixel_sarpe(sarpe[i].x,sarpe[i].y);//desenam sarpele in functie de coordonatele fiecarui corp(doar corpul fara cap)
	}


  }

}

void conversie_char(char *ScorString)
{

	uint8_t temp=scor-2;//facem scorul sa incepa de la 0

	if(temp==0)
	{
		ScorString[0]='0';
		ScorString[1]='\0';//functia afiseaza pana la endstirng \0
		return;

	}

	ScorString[0]=(temp/100 + '0');
	ScorString[1]=((temp/10)%10 + '0');//456
	ScorString[2]=(temp%10 + '0');
	ScorString[3]='\0';


}

void interfata_initiala(uint16_t *speed)
{
	ST7735_FillScreenFast(ST7735_RED);

	ST7735_WriteString(0, 8, "   Jocul Snake!     Selectati viteza ", Font_7x10, ST7735_BLACK, ST7735_RED);

	ST7735_WriteString( 7, lungime/2, "Mica", Font_7x10, ST7735_BLACK, ST7735_RED);
	ST7735_WriteString( latime-35 , lungime/2, "Mare", Font_7x10, ST7735_BLACK, ST7735_RED);

	while(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)!=0)//cat timp nu am apasat pe user si am selectat viteza, putem alege
	{

		if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15)==1)//am apasat stanga->selectam mica
		{
			ST7735_WriteString(7 , lungime/2, "Mica", Font_7x10, ST7735_WHITE, ST7735_RED);
			ST7735_WriteString(latime-35 , lungime/2, "Mare", Font_7x10, ST7735_BLACK, ST7735_RED);

			*speed=500;
		}

		if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14)==1)//am apasat dreapta->selectam mare
		{
			ST7735_WriteString(7, lungime/2, "Mica", Font_7x10, ST7735_BLACK, ST7735_RED);
			ST7735_WriteString(latime-35, lungime/2, "Mare", Font_7x10, ST7735_WHITE, ST7735_RED);

			*speed=250;

		}

	}


	ST7735_FillScreenFast(ST7735_BLACK);


}


void interfata_game_over()
{


	uint8_t Xi=0;
	uint8_t Yj=0;

	while(Xi<128 && Yj<128) //facem efectul vizual pentru interfata de culoarea rosie-Background gameover
	{

	for(uint8_t i=Xi;i<Xi+8;i++)
	{
		for(uint8_t j=Yj;j<Yj+8;j++)
		{
			ST7735_DrawPixel(i, j, ST7735_RED);//(x,y)
		}

	}

	Xi=Xi+8;

	if(Xi>120)
	{
		Yj=Yj+8;
		Xi=0;
	}


	}

	ST7735_WriteString(0, 30, " Game over!     :( ", Font_11x18, ST7735_BLACK, ST7735_RED);
	//convertim la char scorul

	char ScorString[4];
	conversie_char(ScorString);

	ST7735_WriteString(56, 90, ScorString, Font_11x18, ST7735_BLACK, ST7735_RED);


}

void verifica_coliziune()
{
  //verifica coliziunea cu el insusi

  for(uint8_t i=1;i<=scor;i++)
  {
    if(sarpe[0].x==sarpe[i].x && sarpe[0].y==sarpe[i].y)
    {

      game_over=1;
      interfata_game_over();
    }
  }

  /*
  //verificam coliziunea cu peretii ecranului: se verifica doar capul

  if(sarpe[0].x>=128 || sarpe[0].y>=128 || sarpe[0].x<0 || sarpe[0].y<0)//HAHA... bufferOverflow
  {

     game_over=1;
     interfata_game_over();
  }
  */

}

//--------------------------------------deplasari in functie de inputul dat de user-------------------------------------------------------------------------

void deplasare_dreapta()
{

    for(uint8_t i=scor;i>0;i--)
    {
    sarpe[i].x=sarpe[i-1].x;
    sarpe[i].y=sarpe[i-1].y;
    }

    if((sarpe[0].x)+8==128)
    {
    	sarpe[0].x=0;//deplasare prin perete
    }

    else
    {
    	sarpe[0].x=sarpe[0].x+8;//deplasam capul spre dreapta
    }

}

void deplasare_stanga()
{

    for(uint8_t i=scor;i>0;i--)
    {
    sarpe[i].x=sarpe[i-1].x;
    sarpe[i].y=sarpe[i-1].y;
    }

    if((sarpe[0].x)-8<0)
    {
        sarpe[0].x=120;
    }

    else
    {
    	sarpe[0].x=sarpe[0].x-8;//deplasam capul spre stanga
    }


}

void deplasare_jos()
{

    for(uint8_t i=scor;i>0;i--)
    {
    sarpe[i].x=sarpe[i-1].x;
    sarpe[i].y=sarpe[i-1].y;
    }

    if((sarpe[0].y)+8==128)
    {
        sarpe[0].y=0;
    }

    else
    {
    	sarpe[0].y=sarpe[0].y+8;//deplasam capul spre sus
    }

}


void deplasare_sus()
{

    for(uint8_t i=scor;i>0;i--)
    {
    sarpe[i].x=sarpe[i-1].x;
    sarpe[i].y=sarpe[i-1].y;
    }

    if((sarpe[0].y)-8<0)
    {
        sarpe[0].y=120;
    }

    else
    {
    	sarpe[0].y=sarpe[0].y-8;//deplasam capul spre jos
    }


}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) //functie pentru intreruperi de sistem
{

	if(timp>=15)//Un mic Delay pentru schimbarea directiei pentru a nu parea prea instant si distopic fata de viteza de joc
	{

	intrerupere=1;//o variabila pentru a verifica daca a avut loc intreruperea

	if(GPIO_Pin == GPIO_PIN_12 && apasat13==0)//Deplasare Sus
		{
			apasat12=1;
			apasat13=0;
			apasat14=0;
			apasat15=0;


		}

	if(GPIO_Pin == GPIO_PIN_13 && apasat12==0)//Deplasare jos
		{
			apasat12=0;
			apasat13=1;
			apasat14=0;
			apasat15=0;

		}

	if(GPIO_Pin == GPIO_PIN_14 && apasat15==0)//Deplasare dreapta
		{
			apasat12=0;
			apasat13=0;
			apasat14=1;
			apasat15=0;

			apasatDR=1;
			apasatSTG=0;

		}

	if(GPIO_Pin == GPIO_PIN_15 && apasat14==0)//Deplasare Stanga
		{
			apasat12=0;
			apasat13=0;
			apasat14=0;
			apasat15=1;

			apasatDR=0;
			apasatSTG=1;

		}

		timp=0;//resetam timpul

	}


}

//----------------------------------------------------------------------------------------------------------------------------------------------------------

void spawn_mancare()
{
  uint8_t ok=0;

  //verificam sa nu spawunam in coordonatele curente ale sarpelui

  while(1)
  {

	ok=0;

	randx=(rand()%16)*8;
	randy=(rand()%16)*8;

   for(uint8_t i=0;i<=scor;i++)
   {

     if(sarpe[i].x==randx && sarpe[i].y==randy)
     {
       ok=1;
       break;
     }

   }

   if(ok==0)
   {

    break;

   }

  }


  //de testare! spawn initail
  if(temphr==0)
  {
    //test_sarpe
    randx=sarpe[0].x+32;
    randy=sarpe[0].y;
    //momentan hrana si sarpele arata la fel
    pixel_hrana(randx,randy);
    temphr=1;
  }

  //afisare seriala a coordonatelor

  uint8_t caracter;
  caracter=(randx/100)+'0';//456

  HAL_UART_Transmit(&huart1, &(caracter),1,HAL_MAX_DELAY);
  caracter=(randx%100)/10+'0';

  HAL_UART_Transmit(&huart1, &(caracter),1,HAL_MAX_DELAY);
  caracter=randx%10+'0';

  HAL_UART_Transmit(&huart1, &(caracter),1,HAL_MAX_DELAY);

  HAL_UART_Transmit(&huart1,(uint8_t*) " ",1,HAL_MAX_DELAY);
  HAL_UART_Transmit(&huart1,(uint8_t*) " ",1,HAL_MAX_DELAY);

}

void fizica_sarpe()
{
    verifica_coliziune();

    if(intrerupere==1)
    {
    	intrerupere=0;//Daca a avut loc intreruperea o facem 0-
    	//Aceasta conditie este suplimentara, ajuta la debugging pentru a verifica daca a avut loc intreruperea
    }

    if(ok1==0)//Conditii initiale ale jocului-Spawn mancare+Reset initial date de intrerupere input
    {
      spawn_mancare();//spawn initial in care si coloreaza initial hrana
      apasat12=0;
      apasat13=0;
      apasat14=1;//Deplasare initiala dreapta
      apasat15=0;

      ok1=1;//Stop initializare

    }

    update_sarpe();//stergem pixeli curenti a sarpelui->ii facem negri ca backgroundul


    //pentru deplasareile automate dupa comenzile data ca input la fiecare ceas
    //automat se va considera doar o directie valida, conditie prelucrata anterior

    if(apasat15==1)
    {
      deplasare_stanga();
    }

    if(apasat12==1)
    {
      deplasare_sus();
    }

    if(apasat14==1)
    {
      deplasare_dreapta();
    }

    if(apasat13==1)
    {
      deplasare_jos();
    }


    deseneaza_sarpe();

    if(sarpe[0].x==randx && sarpe[0].y==randy)//capul a mancat bine->momentul in care sarpele a mancat hrana, urmeaza sa o adaugam
    {
      //Deoarece hrana actuala va fi suprapusa cu capul sarpelui nu va trebui sa o stergem/rectualizam
      scor++;

      //! Atentie, aici urmeaza in functie de directia de deplasareschimbarea codului->adaugarea noului corp
      //Pe scurt vom adauga mancarea in functie de ultimele coordonate (2) ale cozii ca sa aflam directia de deplasare in spatiu
      //sarpe[scor].x=sarpe[scor-1].x-8;
      //sarpe[scor].y=sarpe[scor-1].y;

      //deplasare catre dreapta-nou corp la coada unde vom face diferenta dintre ultimele 2 coordonate
      if(sarpe[scor-1].x+8==sarpe[scor-2].x && sarpe[scor-1].y==sarpe[scor-2].y)
      {
         sarpe[scor].x=sarpe[scor-1].x-8;
         sarpe[scor].y=sarpe[scor-1].y;
      }

      //deplasare catre stanga
      if(sarpe[scor-1].x-8==sarpe[scor-2].x && sarpe[scor-1].y==sarpe[scor-2].y)
      {
         sarpe[scor].x=sarpe[scor-1].x+8;
         sarpe[scor].y=sarpe[scor-1].y;
      }

      //deplasare catre sus
      if(sarpe[scor-1].x==sarpe[scor-2].x && sarpe[scor-1].y-8==sarpe[scor-2].y)
      {
         sarpe[scor].x=sarpe[scor-1].x;
         sarpe[scor].y=sarpe[scor-1].y+8;
      }

      //deplasare catre jos
      if(sarpe[scor-1].x==sarpe[scor-2].x && sarpe[scor-1].y+8==sarpe[scor-2].y)
      {
         sarpe[scor].x=sarpe[scor-1].x;
         sarpe[scor].y=sarpe[scor-1].y-8;
      }

      update_sarpe();
      deseneaza_sarpe();//Desenam sarpele pe ecran cu scor+

      spawn_mancare();//mutam aleator la alte coordonate hrana
      pixel_hrana(randx,randy);//desenam hrana


      }


}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  uint16_t speed=500;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_USB_PCD_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim2);//pornim counterul-ceasul

  ST7735_Init();

  ST7735_FillScreenFast(ST7735_BLACK);

  alege_joc_interfata();//userul alege jocul

  if(optiuneJoc==0)//Am selectat jocul snake
  {

  ST7735_FillScreenFast(ST7735_BLACK);

  interfata_initiala(&speed);

  pixel_sarpe(latime/2,lungime/2);

  sarpe[0].x=latime/2;
  sarpe[0].y=lungime/2;

  sarpe[1].x=sarpe[0].x-8;
  sarpe[1].y=sarpe[0].y;

  sarpe[2].x=sarpe[1].x-8;
  sarpe[2].y=sarpe[1].y;

  }

  if(optiuneJoc==1)//Am selectat spaceInvaders
  {
	 ST7735_FillScreenFast(ST7735_BLACK);

  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
   {
	  if(optiuneJoc==0)
	  {

	   if(game_over==0)

	   {

	   srand(HAL_GetTick());//Seed diferit pentru fiecare spawn->cat mai aleator
	   fizica_sarpe();
	   verifica_coliziune();

	   }

	   HAL_Delay(speed);

	  }

	  if(optiuneJoc==1)
	  {
		  motor_joc();
		  HAL_Delay(500);
	  }

  }


}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_1LINE;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 7199;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 100;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_OC_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_TIMING;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_OC_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USB Initialization Function
  * @param None
  * @retval None
  */
static void MX_USB_PCD_Init(void)
{

  /* USER CODE BEGIN USB_Init 0 */

  /* USER CODE END USB_Init 0 */

  /* USER CODE BEGIN USB_Init 1 */

  /* USER CODE END USB_Init 1 */
  hpcd_USB_FS.Instance = USB;
  hpcd_USB_FS.Init.dev_endpoints = 8;
  hpcd_USB_FS.Init.speed = PCD_SPEED_FULL;
  hpcd_USB_FS.Init.low_power_enable = DISABLE;
  hpcd_USB_FS.Init.lpm_enable = DISABLE;
  hpcd_USB_FS.Init.battery_charging_enable = DISABLE;
  if (HAL_PCD_Init(&hpcd_USB_FS) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_Init 2 */

  /* USER CODE END USB_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB4 PB5
                           PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB12 PB13 PB14 PB15 */
  GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PA8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
