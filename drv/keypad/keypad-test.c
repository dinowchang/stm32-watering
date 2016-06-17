/**
  ******************************************************************************
  * @file    keypad-test.c
  * @author  dinow
  * @version V0.0.1
  * @date    2016-06-16
  * @brief   
  ******************************************************************************
  * @attention
  *
  *
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "type.h"
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOS_CLI.h"
#include "keypad.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/**
 * @param	pcWriteBuffer
 * @param	xWriteBufferLen
 * @param	pcCommandString
 * @return
 */
static BaseType_t KEY_ReadCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	Key_t key;
	key = KEY_GetKey();

	switch(key)
	{
		case Key_None:
			sprintf(pcWriteBuffer, "Key_None\n");
			break;

		case Key_Left:
			sprintf(pcWriteBuffer, "Key_Left\n");
			break;

		case Key_Right:
			sprintf(pcWriteBuffer, "Key_Right\n");
			break;

		case Key_Up:
			sprintf(pcWriteBuffer, "Key_Up\n");
			break;

		case Key_Down:
			sprintf(pcWriteBuffer, "Key_Down\n");
			break;

		case Key_Select:
			sprintf(pcWriteBuffer, "Key_Select\n");
			break;

	}
	//sprintf(pcWriteBuffer, "Key ADC Value = %d , %04x\n", data, data);
	return pdFALSE;
}

static const CLI_Command_Definition_t xKeyRead =
{
	"key_read",
	"key_read:\n    Return ADV value\n",
	KEY_ReadCommand,
	0
};

void KEY_Test(void)
{
	FreeRTOS_CLIRegisterCommand(&xKeyRead);
}
