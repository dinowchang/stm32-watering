/**
  ******************************************************************************
  * @file    watermenu.c
  * @author  dinow
  * @version V0.0.1
  * @date    2016-06-21
  * @brief
  ******************************************************************************
  * @attention
  *
  *
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stdio.h"
#include "menufunc.h"
#include "water.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define CURSOR_FIELD_TIME				0
#define CURSOR_FIELD_PERIOD				1
#define CURSOR_FIELD_MOISTURE			2
#define CURSOR_FIELD_TEST				3
#define CURSOR_FIELD_SAVE				4
#define CURSOR_FIELD_CANCEL				5

#define WATER_MENU_LENGTH				(sizeof(waterMenu_Print) / sizeof(waterMenu_Print[0]))

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static uint8_t m_cursorField;
static uint8_t m_cursorLine;
static RTC_TimeTypeDef m_newWaterTime;
static uint32_t m_period;
static uint16_t m_moisture;

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
static void waterMenu_PrintTime(void)
{
	char buf[17];
	sprintf(buf, "Time      %2d:%02d", m_newWaterTime.RTC_Hours,
			m_newWaterTime.RTC_Minutes);
	LCD_Print(buf);
}

static void waterMenu_PrintPeriod(void)
{
	char buf[17];
	sprintf(buf, "Period(s)  %2ld.%1ld", (m_period / 10), (m_period % 10));
	LCD_Print(buf);
}

static void waterMenu_PrintMoisture(void)
{
	char buf[17];
	sprintf(buf, "Moisture   %4d", m_moisture);
	LCD_Print(buf);
}

static void waterMenu_PrintTest(void)
{
	LCD_Print("Test           ");
}

static void waterMenu_PrintSave(void)
{
	LCD_Print("Save           ");
}

static void waterMenu_PrintCancel(void)
{
	LCD_Print("Cancel         ");
}

void (*waterMenu_Print[])(void) =
{
	waterMenu_PrintTime,
	waterMenu_PrintPeriod,
	waterMenu_PrintMoisture,
	waterMenu_PrintTest,
	waterMenu_PrintSave,
	waterMenu_PrintCancel
};

static void waterMenu_Show(void)
{
	LCD_Blink(DISABLE);

	LCD_SetLoc(0, m_cursorLine);
	waterMenu_Print[m_cursorField]();

	if (m_cursorLine == 0)
	{
		LCD_SetLoc(0, 1);
		waterMenu_Print[m_cursorField + 1]();
	}
	else
	{
		LCD_SetLoc(0, 0);
		waterMenu_Print[m_cursorField - 1]();
	}

	LCD_SetLoc(15, m_cursorLine);
	LCD_Blink(ENABLE);
}

/**
 *
 */
static void waterMenu_Open(void)
{
	LCD_Display(DISABLE);
	LCD_Clear();

	m_cursorField = 0;
	m_cursorLine = 0;

	WATER_GetWaterTime(&m_newWaterTime);
	m_period = WATER_GetPeriod() / 100;
	m_moisture = WATER_GetThreshold();

	waterMenu_Show();

	LCD_Display(ENABLE);
}

/**
 *
 */
static void waterMenu_Up(void)
{
	if( m_cursorLine > 0 )
		m_cursorLine--;

	if( m_cursorField > CURSOR_FIELD_TIME )
			m_cursorField--;

	waterMenu_Show();
}

/**
 *
 */
static void waterMenu_Down(void)
{
	if( m_cursorLine < 1 )
		m_cursorLine++;

	if( m_cursorField < (WATER_MENU_LENGTH - 1) )
			m_cursorField++;

	waterMenu_Show();
}

/**
 *
 */
static void waterMenu_Select(void)
{

}

/**
 *
 */
static void waterMenu_Left(void)
{

}

Menu_t waterMenu =
{
	.open = waterMenu_Open,
	.close = NULL,
	.redraw = NULL,
	.up = waterMenu_Up,
	.down = waterMenu_Down,
	.right = NULL,
	.left = waterMenu_Left,
	.select = waterMenu_Select,
};
