/**
  ******************************************************************************
  * @file    lcd1602-test.c
  * @author  dinow
  * @version V0.0.1
  * @date    2016-06-15
  * @brief   
  ******************************************************************************
  * @attention
  *
  *
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOS_CLI.h"
#include "debug.h"
#include "util.h"
#include "lcd1602.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
 * @brief	Switch LCD backlight LED
 * @param	pcWriteBuffer
 * @param	xWriteBufferLen
 * @param	pcCommandString
 * @return
 */
static BaseType_t LCD_LedCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	const char *parameterPtr;
	int32_t paramterLen;

	parameterPtr = FreeRTOS_CLIGetParameter(pcCommandString, 1, &paramterLen);

	if (paramterLen == 1)
	{
		if (parameterPtr[0] == '0')
			GPIO_ResetBits(GPIOB, GPIO_Pin_6);
		else if (parameterPtr[0] == '1')
			GPIO_SetBits(GPIOB, GPIO_Pin_6);

	}
	else if (paramterLen == 2)
	{
		if (parameterPtr[0] == 'o' && parameterPtr[1] == 'n')
			GPIO_SetBits(GPIOB, GPIO_Pin_6);//GPIO_SetBits(LCD_PIN_BL);
	}
	else if (paramterLen == 3)
	{
		if (parameterPtr[0] == 'o' && parameterPtr[1] == 'f'
				&& parameterPtr[2] == 'f')
			GPIO_ResetBits(GPIOB, GPIO_Pin_6);//GPIO_ResetBits(LCD_PIN_BL);
	}

	sprintf(pcWriteBuffer, "\n"); // Clean Message

	return pdFALSE;

}

static const CLI_Command_Definition_t xLcdLed =
{
	"lcd_led",
	"lcd_led:\n    Switch LCD backlight LED\n",
	LCD_LedCommand,
	1
};

/**
 * @param	pcWriteBuffer
 * @param	xWriteBufferLen
 * @param	pcCommandString
 * @return
 */
static BaseType_t LCD_ClearCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	LCD_Clear();
	sprintf(pcWriteBuffer, "LCD clear display\n"); // Clean Message
	return pdFALSE;

}

static const CLI_Command_Definition_t xLcdClear =
{
	"lcd_clear",
	"lcd_clear:\n    Clear display\n",
	LCD_ClearCommand,
	0
};

/**
 * @param	pcWriteBuffer
 * @param	xWriteBufferLen
 * @param	pcCommandString
 * @return
 */
static BaseType_t LCD_HomeCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	LCD_Home();
	sprintf(pcWriteBuffer, "LCD move cursor home\n"); // Clean Message
	return pdFALSE;

}

static const CLI_Command_Definition_t xLcdHome =
{
	"lcd_home",
	"lcd_home:\n    Clear display\n",
	LCD_HomeCommand,
	0
};
/**
 * @brief	Register CLI command
 */
void LCD_Test(void)
{
	FreeRTOS_CLIRegisterCommand(&xLcdLed);
	FreeRTOS_CLIRegisterCommand(&xLcdClear);
	FreeRTOS_CLIRegisterCommand(&xLcdHome);

}
