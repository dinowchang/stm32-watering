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
typedef struct
{
	char name[16];
	Menu_t *menu;
} MainMenuItem_t;

#define MAINMENU_LIST_LEN		( sizeof(m_menuList) / sizeof(MainMenuItem_t) )

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static MainMenuItem_t m_menuList[] =
{
		{ "-- Main Menu --", NULL},
		{ " Watering set  ", &waterMenu},
		{ " Moist. mon    ", &moistureMenu},
		{ " Clock         ", &clockMenu},
		{ "---   End   ---", NULL}
};

static uint8_t m_curDisplayPos = 0;
static uint8_t m_curListPos = 0;


/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
 *
 */
void mainMenu_Open(void)
{
	LCD_Display(DISABLE);
	LCD_Clear();
	LCD_SetLoc(1,0);
	LCD_Print(m_menuList[0].name);
	LCD_SetLoc(1,1);
	LCD_Print(m_menuList[1].name);

	m_curDisplayPos = 0;
	m_curListPos = 0;
	LCD_SetLoc(0,0);
	LCD_Blink(ENABLE);
	LCD_Display(ENABLE);
}

/**
 *
 */
void mainMenu_Up(void)
{
	if( m_curDisplayPos == 1 )
	{
		m_curDisplayPos = 0;
		m_curListPos--;
		LCD_SetLoc(0, m_curDisplayPos);
	}
	else
	{
		if( m_curListPos != 0 )
		{
			LCD_Blink(DISABLE);
			m_curListPos--;
			LCD_SetLoc(1,0);
			LCD_Print(m_menuList[m_curListPos].name);
			LCD_SetLoc(1,1);
			LCD_Print(m_menuList[m_curListPos + 1].name);
			LCD_SetLoc(0,0);
			LCD_Blink(ENABLE);
		}
	}
}

/**
 *
 */
void mainMenu_Down(void)
{
	if( m_curDisplayPos == 0 )
	{
		m_curDisplayPos = 1;
		m_curListPos++;
		LCD_SetLoc(0, m_curDisplayPos);
	}
	else
	{
		if( m_curListPos != (MAINMENU_LIST_LEN - 1) )
		{
			LCD_Blink(DISABLE);
			m_curListPos++;
			LCD_SetLoc(1,0);
			LCD_Print(m_menuList[m_curListPos - 1].name);
			LCD_SetLoc(1,1);
			LCD_Print(m_menuList[m_curListPos].name);
			LCD_SetLoc(0,1);
			LCD_Blink(ENABLE);
		}
	}
}

/**
 *
 */
void mainMenu_Select(void)
{
	if( m_menuList[m_curListPos].menu != NULL)
		MENU_SwitchMenu(m_menuList[m_curListPos].menu);
}

Menu_t mainMenu =
{
	.open = mainMenu_Open,
	.close = NULL,
	.redraw = NULL,
	.up = mainMenu_Up,
	.down = mainMenu_Down,
	.right = mainMenu_Select,
	.left = NULL,
	.select = mainMenu_Select,
};
