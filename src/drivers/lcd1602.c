/**
  ******************************************************************************
  * @file    lcd1602.c
  * @author  dinow
  * @version V0.0.1
  * @date    2016-06-14
  * @brief   
  ******************************************************************************
  * @attention
  *
  *
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "config.h"
#include "type.h"
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include "debug.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define LCD_PIN_CLOCK				RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC
#define LCD_PIN_DB4_PORT			GPIOB
#define LCD_PIN_DB4_NUM				GPIO_Pin_5
#define LCD_PIN_DB5_PORT			GPIOB
#define LCD_PIN_DB5_NUM				GPIO_Pin_4
#define LCD_PIN_DB6_PORT			GPIOB
#define LCD_PIN_DB6_NUM				GPIO_Pin_10
#define LCD_PIN_DB7_PORT			GPIOA
#define LCD_PIN_DB7_NUM				GPIO_Pin_8
#define LCD_PIN_RS_PORT				GPIOA
#define LCD_PIN_RS_NUM				GPIO_Pin_9
#define LCD_PIN_EN_PORT				GPIOC
#define LCD_PIN_EN_NUM				GPIO_Pin_7
#define LCD_PIN_BL_PORT				GPIOB
#define LCD_PIN_BL_NUM				GPIO_Pin_6

#define LCD_PIN_DB4					LCD_PIN_DB4_PORT, LCD_PIN_DB4_NUM
#define LCD_PIN_DB5					LCD_PIN_DB4_PORT, LCD_PIN_DB4_NUM
#define LCD_PIN_DB6					LCD_PIN_DB4_PORT, LCD_PIN_DB4_NUM
#define LCD_PIN_DB7					LCD_PIN_DB4_PORT, LCD_PIN_DB4_NUM
#define LCD_PIN_RS					LCD_PIN_RS_PORT, LCD_PIN_RS_NUM
#define LCD_PIN_EN					LCD_PIN_EN_PORT, LCD_PIN_EN_NUM
#define LCD_PIN_BL					LCD_PIN_BL_PORT, LCD_PIN_BL_NUM

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/**
 * @brief Switch LCD backlight LED
 * @param pcWriteBuffer
 * @param xWriteBufferLen
 * @param pcCommandString
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
			GPIO_ResetBits(LCD_PIN_BL);
		else if (parameterPtr[0] == '1')
			GPIO_SetBits(LCD_PIN_BL);

	}
	else if (paramterLen == 2)
	{
		if (parameterPtr[0] == 'o' && parameterPtr[1] == 'n')
			GPIO_SetBits(LCD_PIN_BL);
	}
	else if (paramterLen == 3)
	{
		if (parameterPtr[0] == 'o' && parameterPtr[1] == 'f'
				&& parameterPtr[2] == 'f')
			GPIO_ResetBits(LCD_PIN_BL);
	}

	sprintf(pcWriteBuffer, "\n"); // Clean Message

	return pdFALSE;

}

static const CLI_Command_Definition_t xLcdLed =
{
	"lcd_led", /* The command string to type. */
	"lcd_led:\n    Switch LCD backlight LED\n",
	LCD_LedCommand, /* The function to run. */
	1 /* No parameters are expected. */
};

/**
 * @brief Configure gpio and initialize LCD
 */
void LCD_Init(void)
{
	RCC_AHB1PeriphClockCmd(LCD_PIN_CLOCK, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;

	GPIO_InitStructure.GPIO_Pin = LCD_PIN_BL_NUM;
	GPIO_Init(LCD_PIN_BL_PORT, &GPIO_InitStructure);

	// Register CLI command
	FreeRTOS_CLIRegisterCommand( &xLcdLed );
}
