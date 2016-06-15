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
#include "task.h"
#include "FreeRTOS_CLI.h"
#include "debug.h"
#include "util.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

#define LCD_NIBBLE_DELAY					20
#define LCD_INSTRUCTION_DELAY				60
// LCD Pins
#define LCD_PIN_CLOCK						RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC
#define LCD_PIN_DB4_PORT					GPIOB
#define LCD_PIN_DB4_NUM						GPIO_Pin_5
#define LCD_PIN_DB5_PORT					GPIOB
#define LCD_PIN_DB5_NUM						GPIO_Pin_4
#define LCD_PIN_DB6_PORT					GPIOB
#define LCD_PIN_DB6_NUM						GPIO_Pin_10
#define LCD_PIN_DB7_PORT					GPIOA
#define LCD_PIN_DB7_NUM						GPIO_Pin_8
#define LCD_PIN_RS_PORT						GPIOA
#define LCD_PIN_RS_NUM						GPIO_Pin_9
#define LCD_PIN_EN_PORT						GPIOC
#define LCD_PIN_EN_NUM						GPIO_Pin_7
#define LCD_PIN_BL_PORT						GPIOB
#define LCD_PIN_BL_NUM						GPIO_Pin_6

#define LCD_PIN_DB4							LCD_PIN_DB4_PORT, LCD_PIN_DB4_NUM
#define LCD_PIN_DB5							LCD_PIN_DB5_PORT, LCD_PIN_DB5_NUM
#define LCD_PIN_DB6							LCD_PIN_DB6_PORT, LCD_PIN_DB6_NUM
#define LCD_PIN_DB7							LCD_PIN_DB7_PORT, LCD_PIN_DB7_NUM
#define LCD_PIN_RS							LCD_PIN_RS_PORT, LCD_PIN_RS_NUM
#define LCD_PIN_EN							LCD_PIN_EN_PORT, LCD_PIN_EN_NUM
#define LCD_PIN_BL							LCD_PIN_BL_PORT, LCD_PIN_BL_NUM

// HD44780 Commands
#define HD44780_CLEAR						0x01
#define HD44780_HOME						0x02
#define HD44780_ENTRY_MODE					0x04
#define HD44780_EM_SHIFT_CURSOR				0
#define HD44780_EM_SHIFT_DISPLAY			1
#define HD44780_EM_DECREMENT				0
#define HD44780_EM_INCREMENT				2

#define HD44780_DISPLAY_ONOFF				0x08
#define HD44780_DISPLAY_OFF					0
#define HD44780_DISPLAY_ON					4
#define HD44780_CURSOR_OFF					0
#define HD44780_CURSOR_ON					2
#define HD44780_CURSOR_NOBLINK				0
#define HD44780_CURSOR_BLINK				1

#define HD44780_DISPLAY_CURSOR_SHIFT		0x10
#define HD44780_SHIFT_CURSOR				0
#define HD44780_SHIFT_DISPLAY				8
#define HD44780_SHIFT_LEFT					0
#define HD44780_SHIFT_RIGHT					4

#define HD44780_FUNCTION_SET				0x20
#define HD44780_FONT5x7						0
#define HD44780_FONT5x10					4
#define HD44780_ONE_LINE					0
#define HD44780_TWO_LINE					8
#define HD44780_4_BIT						0
#define HD44780_8_BIT						16

#define HD44780_CGRAM_SET					0x40

#define HD44780_DDRAM_SET					0x80

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
 * @brief send nibble to LCD in 4bit mode
 * @param nibble
 */
void LCD_WriteNibble(unsigned char nibble)
{
	GPIO_SetBits(LCD_PIN_EN);
	GPIO_WriteBit(LCD_PIN_DB4, (BitAction) (nibble & BIT0));
	GPIO_WriteBit(LCD_PIN_DB5, (BitAction) (nibble & BIT1));
	GPIO_WriteBit(LCD_PIN_DB6, (BitAction) (nibble & BIT2));
	GPIO_WriteBit(LCD_PIN_DB7, (BitAction) (nibble & BIT3));

	udelay(LCD_NIBBLE_DELAY);
	GPIO_ResetBits(LCD_PIN_EN);
}

/**
 * @brief Write instruction
 * @param command
 */
void LCD_WriteInstruction(unsigned char cmd)
{
	GPIO_ResetBits(LCD_PIN_RS);
	LCD_WriteNibble(cmd >> 4);
	LCD_WriteNibble(cmd & 0x0f);
	udelay(LCD_INSTRUCTION_DELAY);
}

/**
 * @brief Write data
 * @param data
 */
void LCD_WriteData(unsigned char data)
{
	GPIO_SetBits(LCD_PIN_RS);
	LCD_WriteNibble(data >> 4);
	LCD_WriteNibble(data & 0x0f);
	udelay(LCD_INSTRUCTION_DELAY);
}


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

	GPIO_InitStructure.GPIO_Pin = LCD_PIN_DB4_NUM;
	GPIO_Init(LCD_PIN_DB4_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = LCD_PIN_DB5_NUM;
	GPIO_Init(LCD_PIN_DB5_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = LCD_PIN_DB6_NUM;
	GPIO_Init(LCD_PIN_DB6_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = LCD_PIN_DB7_NUM;
	GPIO_Init(LCD_PIN_DB7_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = LCD_PIN_RS_NUM;
	GPIO_Init(LCD_PIN_RS_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = LCD_PIN_EN_NUM;
	GPIO_Init(LCD_PIN_EN_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = LCD_PIN_BL_NUM;
	GPIO_Init(LCD_PIN_BL_PORT, &GPIO_InitStructure);

	GPIO_ResetBits(LCD_PIN_RS);
	GPIO_ResetBits(LCD_PIN_EN);
	GPIO_SetBits(LCD_PIN_BL);

	// Register CLI command
	FreeRTOS_CLIRegisterCommand( &xLcdLed );

	// wait 100ms for LCD module boot
	//udelay(100000);

	// Configure LCD to 4 bit mode
	LCD_WriteNibble(0x03);
	udelay(LCD_INSTRUCTION_DELAY);
	LCD_WriteNibble(0x03);
	udelay(LCD_INSTRUCTION_DELAY);
	LCD_WriteNibble(0x03);
	udelay(LCD_INSTRUCTION_DELAY);
	LCD_WriteNibble(0x02);
	udelay(LCD_INSTRUCTION_DELAY);

	LCD_WriteInstruction(0x28);	// set LCD to font 5x7, 2 line
	LCD_WriteInstruction(0x08); // Disable Display

	LCD_WriteInstruction(0x01); // Clear Display
	udelay(3000); // delay 3ms

	LCD_WriteInstruction(0x06); // set to cursor move mode
	LCD_WriteInstruction(0x00);
	LCD_WriteInstruction(0x0f); // Enable display cursor

	LCD_WriteInstruction(0x81); // goto 1, 0
	LCD_WriteData('H');
	LCD_WriteData('e');
	LCD_WriteData('l');
	LCD_WriteData('l');
	LCD_WriteData('o');

}
