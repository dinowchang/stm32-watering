/**
  ******************************************************************************
  * @file    water.c
  * @author  dinow
  * @version V0.0.1
  * @date    2016-06-20
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
#include "type.h"
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOS_CLI.h"
#include "debug.h"
#include "util.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define SUPPORT_WATER_TEST_COMMAND				1 // debug command for FreeRTOS-CLI

#define WATER_GPIO_CLOCK_PORT					RCC_AHB1Periph_GPIOC
#define WATER_PIN_PORT							GPIOC
#define WATER_PIN_NUM							GPIO_Pin_13

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
 * @brief	Open value for x millisecond
 * @param period
 */
void WATER_OpenValve(uint32_t ms)
{
	GPIO_ResetBits(WATER_PIN_PORT, WATER_PIN_NUM);
	vTaskDelay(ms);
	GPIO_SetBits(WATER_PIN_PORT, WATER_PIN_NUM);
}

#if SUPPORT_WATER_TEST_COMMAND
/**
 * @param	pcWriteBuffer
 * @param	xWriteBufferLen
 * @param	pcCommandString
 * @return
 */
static BaseType_t WATER_OpenCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	const char *parameterPtr;
	int32_t paramterLen;
	uint32_t time;

	parameterPtr = FreeRTOS_CLIGetParameter(pcCommandString, 1, &paramterLen);
	time = DecToInt((char *) parameterPtr, paramterLen);
	WATER_OpenValve(time);
	pcWriteBuffer[0] = '\0'; // clean Write Buffer

	return pdFALSE;
}

static const CLI_Command_Definition_t xWaterOpen =
{
	"wopen",
	"wopen <x>:\n    Open valve for x ms\n",
	WATER_OpenCommand,
	1
};
#endif

/**
 * @brief Configure gpio
 */
void WATER_Init(void)
{
	// Enable GPIO clock
	RCC_AHB1PeriphClockCmd(WATER_GPIO_CLOCK_PORT, ENABLE);

	// Configure key pin to analog input
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = WATER_PIN_NUM;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(WATER_PIN_PORT, &GPIO_InitStructure);

	GPIO_SetBits(WATER_PIN_PORT, WATER_PIN_NUM);
#if SUPPORT_WATER_TEST_COMMAND
	FreeRTOS_CLIRegisterCommand(&xWaterOpen);
#endif
}
