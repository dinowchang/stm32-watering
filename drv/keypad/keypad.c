/**
  ******************************************************************************
  * @file    keypad.c
  * @author  Dinow
  * @version V0.0.1
  * @date    2016-6-14
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
#include "stm32f4xx.h"
#include "keypad.h"

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
	KeyPadButton key;
	uint16_t threshold;
} KeyVoltage_t;

/* Private define ------------------------------------------------------------*/
#define SUPPORT_KEYPAD_TEST_COMMAND			1 // debug command for FreeRTOS-CLI

#define KEY_GPIO_CLOCK_PORT					RCC_AHB1Periph_GPIOA
#define KEY_PIN_PORT						GPIOA
#define KEY_PIN_NUM							GPIO_Pin_0

#define KEY_ADC_CLOCK_PORT					RCC_APB2Periph_ADC1
#define KEY_ADC_PORT						ADC1
#define KEY_ADC_CHANNEL						ADC_Channel_0

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
KeyVoltage_t m_KeyVoltageTable[] =
{
	{eKeyRight,		18	},
	{eKeyUp,		59	},
	{eKeyDown,		104	},
	{eKeyLeft,		155	},
	{eKeySelect,	220	},
	{eKeyNone,		256	},
};

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
uint16_t KEY_GetValue(void)
{
	ADC_ClearFlag(KEY_ADC_PORT, ADC_FLAG_EOC); //Clear EOC flag
	while(ADC_GetFlagStatus(KEY_ADC_PORT, ADC_FLAG_EOC) == RESET); //Wail for conversion complete

	return ADC_GetConversionValue(KEY_ADC_PORT);
}

KeyPadButton KEY_GetKey(void)
{
	uint16_t voltage;

	voltage = KEY_GetValue();

	uint8_t i = 0;

	while( m_KeyVoltageTable[i].threshold  < voltage) i++;

	return m_KeyVoltageTable[i].key;
}

/**
 * @brief Enable key detection
 */
void KEY_Enable(void)
{
	ADC_InitTypeDef ADC_InitStruct;
	ADC_StructInit(&ADC_InitStruct);
	ADC_InitStruct.ADC_Resolution = ADC_Resolution_6b;
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Left;
	ADC_Init(KEY_ADC_PORT, &ADC_InitStruct);

	ADC_RegularChannelConfig(KEY_ADC_PORT, KEY_ADC_CHANNEL, 1, ADC_SampleTime_84Cycles);

	ADC_Cmd(KEY_ADC_PORT, ENABLE);

	ADC_SoftwareStartConv(KEY_ADC_PORT); //Start ADC1 software conversion
}

/**
 * @brief Disable key detection
 */
void KEY_Disable(void)
{
	ADC_Cmd(KEY_ADC_PORT, DISABLE);
}


/**
 * @brief Configure clock and pins for keypad
 */
void KEY_Init(void)
{
	// Enable GPIO and ADC clock
    RCC_APB2PeriphClockCmd(KEY_ADC_CLOCK_PORT, ENABLE);
	RCC_AHB1PeriphClockCmd(KEY_GPIO_CLOCK_PORT, ENABLE);

	// Configure key pin to analog input
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = KEY_PIN_NUM;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(KEY_PIN_PORT, &GPIO_InitStructure);

	ADC_CommonInitTypeDef ADC_CommonInitStruct;
	ADC_CommonStructInit(&ADC_CommonInitStruct);
	ADC_CommonInit(&ADC_CommonInitStruct);

#if SUPPORT_KEYPAD_TEST_COMMAND
	KEY_Enable();
	KEY_Test();
#endif
}
