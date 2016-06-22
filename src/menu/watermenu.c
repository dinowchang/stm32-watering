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
#define CURSOR_FIELD_SAVE				3
#define CURSOR_FIELD_TEST				4
#define CURSOR_FIELD_BACK				5

#define WATER_MENU_LENGTH				(sizeof(waterMenu_Print) / sizeof(waterMenu_Print[0]))
#define WATER_MENU_TIME_STEP			30

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static uint8_t m_cursorField;
static uint8_t m_cursorLine;
static RTC_TimeTypeDef m_newWaterTime;
static uint32_t m_period;
static uint16_t m_moisture;
static bool m_modified;

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
	LCD_Print("- Test -       ");
}

static void waterMenu_PrintSave(void)
{
	if( m_modified )
		LCD_Print("- Save -  *    ");
	else
		LCD_Print("- Save -       ");

}

static void waterMenu_PrintBack(void)
{
	LCD_Print("- Back -       ");
}

void (*waterMenu_Print[])(void) =
{
	waterMenu_PrintTime,
	waterMenu_PrintPeriod,
	waterMenu_PrintMoisture,
	waterMenu_PrintSave,
	waterMenu_PrintTest,
	waterMenu_PrintBack
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
	m_modified = FALSE;

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
	switch(m_cursorField)
	{
		case  CURSOR_FIELD_SAVE:
			WATER_SetWaterTime(&m_newWaterTime);
			WATER_SetPeriod(m_period * 100);
			WATER_SetThreshold(m_moisture);

			m_modified = FALSE;
			waterMenu_Show();
			break;

		case  CURSOR_FIELD_TEST:
			WATER_SendRequest();
			break;

		case  CURSOR_FIELD_BACK:
			MENU_SwitchMenu(&mainMenu);
			break;

		default:
			break;
	}
}

/**
 *
 */
static void waterMenu_Left(void)
{
	switch(m_cursorField)
	{
		case CURSOR_FIELD_TIME:
			m_modified = TRUE;

			if( m_newWaterTime.RTC_Minutes >= WATER_MENU_TIME_STEP )
			{
				m_newWaterTime.RTC_Minutes -= WATER_MENU_TIME_STEP;
			}
			else
			{
				m_newWaterTime.RTC_Minutes += (60 - WATER_MENU_TIME_STEP);

				if( m_newWaterTime.RTC_Hours > 0 )
					m_newWaterTime.RTC_Hours--;
				else
					m_newWaterTime.RTC_Hours = 23;
			}

			break;

		case CURSOR_FIELD_PERIOD:
			m_modified = TRUE;

			if( m_period > 0 ) m_period --;
			break;

		case CURSOR_FIELD_MOISTURE:
			m_modified = TRUE;

			if( m_moisture >= 10 )
				m_moisture -= 10;
			else
				m_moisture = 0;
			break;

		default:
			break;
	}

	waterMenu_Show();
}

/**
 *
 */
static void waterMenu_Right(void)
{
	switch(m_cursorField)
	{
		case CURSOR_FIELD_TIME:
			m_modified = TRUE;

			m_newWaterTime.RTC_Minutes += WATER_MENU_TIME_STEP;

			if( m_newWaterTime.RTC_Minutes >=60 )
			{
				m_newWaterTime.RTC_Minutes -= 60;
				m_newWaterTime.RTC_Hours++;
				if( m_newWaterTime.RTC_Hours == 24 ) m_newWaterTime.RTC_Hours = 0;
			}

			break;

		case CURSOR_FIELD_PERIOD:
			m_modified = TRUE;

			if( m_period < 300 ) m_period ++;
			break;

		case CURSOR_FIELD_MOISTURE:
			m_modified = TRUE;

			if( m_moisture < 250 )
				m_moisture += 10;
			break;

		case  CURSOR_FIELD_SAVE:
		case  CURSOR_FIELD_TEST:
		case  CURSOR_FIELD_BACK:
			waterMenu_Select();
			return;
			break;

		default:
			break;
	}

	waterMenu_Show();
}


Menu_t waterMenu =
{
	.open = waterMenu_Open,
	.close = NULL,
	.redraw = NULL,
	.up = waterMenu_Up,
	.down = waterMenu_Down,
	.right = waterMenu_Right,
	.left = waterMenu_Left,
	.select = waterMenu_Select,
};
