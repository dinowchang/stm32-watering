/**
  ******************************************************************************
  * @file    moisturemenu.c
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
#include "soil-moisture.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define MOIST_UPDATE_TIME					1000

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
uint8_t m_liveCnt = 0;
char m_live[4] = { '-', '_'};
static int32_t m_lastUpdateTime;

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/


void moistMenu_Open(void)
{
	uint16_t data;
	char buf[17];

	SOIL_Open();
	vTaskDelay(100 / portTICK_PERIOD_MS); // wait sensor stable

	m_liveCnt = 0;
	m_lastUpdateTime = (int32_t) xTaskGetTickCount();
	data = SOIL_Read();

	LCD_Display(DISABLE);
	LCD_Blink(DISABLE);
	LCD_Clear();
	LCD_SetLoc(1, 0);
	sprintf(buf, "Mositure   %c", m_live[m_liveCnt]);
	LCD_Print(buf);

	LCD_SetLoc(5, 1);
	sprintf(buf, "%4d", data);
	LCD_Print(buf);

	LCD_Display(ENABLE);
}

void moistMenu_Update(void)
{
	if ((((int32_t) xTaskGetTickCount()) - m_lastUpdateTime) > MOIST_UPDATE_TIME)
	{
		uint16_t data;
		char buf[17];

		m_liveCnt++;
		if (m_liveCnt == 2)
			m_liveCnt = 0;
		m_lastUpdateTime = (int32_t) xTaskGetTickCount();
		data = SOIL_Read();

		LCD_SetLoc(1, 0);
		sprintf(buf, "Mositure   %c", m_live[m_liveCnt]);
		LCD_Print(buf);

		LCD_SetLoc(5, 1);
		sprintf(buf, "%4d", data);
		LCD_Print(buf);
	}
}

void moistMenu_Close(void)
{
	SOIL_Close();
}

void moistMenu_Exit(void)
{
	MENU_SwitchMenu(&mainMenu);
}


Menu_t moistureMenu =
{
	.open = moistMenu_Open,
	.close = moistMenu_Close,
	.redraw = moistMenu_Update,
	.up = NULL,
	.down = NULL,
	.right = NULL,
	.left = moistMenu_Exit,
	.select = moistMenu_Exit,
};
