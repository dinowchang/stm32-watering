/**
  ******************************************************************************
  * @file    mainmenu.c
  * @author  dinow
  * @version V0.0.1
  * @date    2016-06-18
  * @brief
  ******************************************************************************
  * @attention
  *
  *
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "menufunc.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

void mainMenu_Open(void)
{
	LCD_Clear();
	LCD_Print("   Welcome!!!");
}

void mainMenu_Right(void)
{
	LCD_SetLoc(0,1);
	LCD_Print("RIGHT");
}

void mainMenu_Left(void)
{
	LCD_SetLoc(0,1);
	LCD_Print("LEFT ");
}

void mainMenu_Up(void)
{
	LCD_SetLoc(0,1);
	LCD_Print("UP   ");
}

void mainMenu_Down(void)
{
	LCD_SetLoc(0,1);
	LCD_Print("DOWN ");
}



Menu_t mainMenu =
{
	.open = mainMenu_Open,
	.close = NULL,
	.redraw = NULL,
	.up = mainMenu_Up,
	.down = mainMenu_Down,
	.right = mainMenu_Right,
	.left = mainMenu_Left,
	.select = NULL,
};
