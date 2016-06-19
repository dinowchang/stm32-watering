/**
  ******************************************************************************
  * @file    clockmenu.c
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
#include <string.h>

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define CLOCK_UPDATE_TIME						100

#define CURSOR_POS_BACK						0
#define CURSOR_POS_HOUR						1
#define CURSOR_POS_MIN						2
#define CURSOR_POS_SEC						3


/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static int32_t m_lastUpdateTime;
static RTC_TimeTypeDef m_newTime;
static uint8_t m_curPos;

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

void clockMenu_Open(void)
{
	m_lastUpdateTime = (int32_t) xTaskGetTickCount();

	LCD_Display(DISABLE);
	LCD_Blink(DISABLE);
	LCD_Clear();

	LCD_SetLoc(0, 0);
	LCD_Print("TIME:");

	LCD_SetLoc(0, 1);
	LCD_WriteData(127);
	LCD_Print(" Back");

	LCD_SetLoc(11, 1);
	LCD_Print("Set ");
	LCD_WriteData(126);

	LCD_Display(ENABLE);
}

void clockMenu_Update(void)
{
	char buf[17];

	if ((((int32_t) xTaskGetTickCount()) - m_lastUpdateTime) > CLOCK_UPDATE_TIME)
	{
		RTC_TimeTypeDef RTC_TimeStruct;
		RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);

		sprintf(buf, "%2d:%02d:%02d", RTC_TimeStruct.RTC_Hours,
				RTC_TimeStruct.RTC_Minutes, RTC_TimeStruct.RTC_Seconds);
		LCD_SetLoc(8, 0);
		LCD_Print(buf);
	}
}

void clockMenu_setClock(void)
{
	MENU_SwitchMenu(&setClockMenu);
}

void clockMenu_Exit(void)
{
	MENU_SwitchMenu(&mainMenu);
}

Menu_t clockMenu =
{
	.open = clockMenu_Open,
	.close = NULL,
	.redraw = clockMenu_Update,
	.up = NULL,
	.down = NULL,
	.right = clockMenu_setClock,
	.left = clockMenu_Exit,
	.select = NULL,
};

void setClockMenu_UpdataCurPos(void)
{
	switch(m_curPos)
	{
		case CURSOR_POS_BACK:
			LCD_SetLoc(0, 0);
			break;

		case CURSOR_POS_HOUR:
			LCD_SetLoc(9, 0);
			break;

		case CURSOR_POS_MIN:
			LCD_SetLoc(12, 0);
			break;

		case CURSOR_POS_SEC:
			LCD_SetLoc(15, 0);
			break;

		default:
			break;
	}
}

void setClockMenu_UpdataTime(void)
{
	char buf[17];

	sprintf(buf, "%2d:%02d:%02d", m_newTime.RTC_Hours,
			m_newTime.RTC_Minutes, m_newTime.RTC_Seconds);

	LCD_Blink(DISABLE);
	LCD_SetLoc(8, 0);
	LCD_Print(buf);

	setClockMenu_UpdataCurPos();
	LCD_Blink(ENABLE);

}

void setClockMenu_Open(void)
{
	m_curPos = CURSOR_POS_BACK;
	RTC_GetTime(RTC_Format_BIN, &m_newTime);

	LCD_Display(DISABLE);
	LCD_Blink(DISABLE);
	LCD_Clear();

	LCD_SetLoc(1, 0);
	LCD_Print("CANCEL");

	setClockMenu_UpdataTime();

	LCD_Display(ENABLE);
}

void setClockMenu_Left(void)
{
	if ( m_curPos > CURSOR_POS_BACK ) m_curPos--;
	setClockMenu_UpdataCurPos();
}

void setClockMenu_Right(void)
{
	if ( m_curPos < CURSOR_POS_SEC ) m_curPos++;
	setClockMenu_UpdataCurPos();
}

void setClockMenu_Up(void)
{
	switch(m_curPos)
	{
		case CURSOR_POS_HOUR:
			m_newTime.RTC_Hours ++;
			if (m_newTime.RTC_Hours >= 24)
				m_newTime.RTC_Hours -= 24;
			break;

		case CURSOR_POS_MIN:
			m_newTime.RTC_Minutes ++;
			if (m_newTime.RTC_Minutes >= 60)
				m_newTime.RTC_Minutes -= 60;
			break;

		case CURSOR_POS_SEC:
			m_newTime.RTC_Seconds ++;
			if (m_newTime.RTC_Seconds >= 60)
				m_newTime.RTC_Seconds -= 60;
			break;

		default:
			break;
	}

	setClockMenu_UpdataTime();
}

void setClockMenu_Down(void)
{
	switch(m_curPos)
	{
		case CURSOR_POS_HOUR:
			if (m_newTime.RTC_Hours == 0)
				m_newTime.RTC_Hours = 23;
			else
				m_newTime.RTC_Hours --;
			break;

		case CURSOR_POS_MIN:
			if (m_newTime.RTC_Minutes == 0)
				m_newTime.RTC_Minutes = 59;
			else
				m_newTime.RTC_Minutes --;
			break;

		case CURSOR_POS_SEC:
			if (m_newTime.RTC_Seconds == 0)
				m_newTime.RTC_Seconds = 59;
			else
				m_newTime.RTC_Seconds --;
			break;

		default:
			break;
	}

	setClockMenu_UpdataTime();
}

void setClockMenu_Select(void)
{
	if(  m_curPos != CURSOR_POS_BACK)
	{
		RTC_SetTime(RTC_Format_BIN, &m_newTime);
	}
	MENU_SwitchMenu(&clockMenu);
}


Menu_t setClockMenu =
{
	.open = setClockMenu_Open,
	.close = NULL,
	.redraw = NULL,
	.up = setClockMenu_Up,
	.down = setClockMenu_Down,
	.right = setClockMenu_Right,
	.left = setClockMenu_Left,
	.select = setClockMenu_Select,
};


