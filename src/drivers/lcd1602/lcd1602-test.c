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
static BaseType_t LCD_SleepCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	const char *parameterPtr;
	int32_t paramterLen;

	parameterPtr = FreeRTOS_CLIGetParameter(pcCommandString, 1, &paramterLen);

	if (paramterLen == 1)
	{
		if (parameterPtr[0] == '0')
			LCD_Sleep(ENABLE);
		else if (parameterPtr[0] == '1')
			LCD_Sleep(DISABLE);
	}
	else if (paramterLen == 2)
	{
		if (parameterPtr[0] == 'o' && parameterPtr[1] == 'n')
			LCD_Sleep(DISABLE);
	}
	else if (paramterLen == 3)
	{
		if (parameterPtr[0] == 'o' && parameterPtr[1] == 'f'
				&& parameterPtr[2] == 'f')
			LCD_Sleep(ENABLE);
	}

	sprintf(pcWriteBuffer, "\n"); // Clean Message

	return pdFALSE;

}

static const CLI_Command_Definition_t xLcdSleep =
{
	"lcd_sleep",
	"lcd_sleep <on/off>:\n    Control LCD backlight and display\n",
	LCD_SleepCommand,
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
	sprintf(pcWriteBuffer, "LCD move cursor home\n");
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
 * @param	pcWriteBuffer
 * @param	xWriteBufferLen
 * @param	pcCommandString
 * @return
 */
static BaseType_t LCD_CursorCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	const char *parameterPtr;
	int32_t paramterLen;
	uint32_t mode;

	parameterPtr = FreeRTOS_CLIGetParameter(pcCommandString, 1, &paramterLen);
	mode = DecToInt((char *) parameterPtr, paramterLen);

	LCD_Cursor(mode & BIT0);
	LCD_Blink(mode & BIT1);

	sprintf(pcWriteBuffer, "Cursor set to mode: %d\n", (uint16_t)(mode & 0x03));

	return pdFALSE;

}

static const CLI_Command_Definition_t xLcdCursor =
{
	"lcd_cur",
	"lcd_cur <mode>:\n    Cursor mode 0~3\n",
	LCD_CursorCommand,
	1
};

/**
 * @brief	Register CLI command
 */
void LCD_Test(void)
{
	FreeRTOS_CLIRegisterCommand(&xLcdSleep);
	FreeRTOS_CLIRegisterCommand(&xLcdClear);
	FreeRTOS_CLIRegisterCommand(&xLcdHome);
	FreeRTOS_CLIRegisterCommand(&xLcdCursor);
}
