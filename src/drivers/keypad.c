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
#include "config.h"
#include "type.h"
#include "stm32f4xx.h"
#include "debug.h"

/* Private typedef -----------------------------------------------------------*/
#define KEY_PORT		GPIOA
#define KEY_PORT_CLK	RCC_AHB1Periph_GPIOA
#define KEY_PIN			GPIO_Pin_0

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
void KEY_GetValue(void)
{
	ADC_ClearFlag(ADC1, ADC_FLAG_EOC); //Clear EOC flag
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET); //Wail for conversion complete

	printf("ADC = %04x\n",  ADC_GetConversionValue(ADC1) ); //Read ADC value
}

/**
 * @brief Enable key detection
 */
void KEY_Enable(void)
{
	ADC_InitTypeDef ADC_InitStruct;
	ADC_StructInit(&ADC_InitStruct);
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;
	ADC_Init(ADC1, &ADC_InitStruct);

	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_84Cycles);

	ADC_Cmd(ADC1, ENABLE);

	ADC_SoftwareStartConv(ADC1); //Start ADC1 software conversion
}

/**
 * @brief Disable key detection
 */
void KEY_Disable(void)
{
	ADC_Cmd(ADC1, DISABLE);
}


/**
 * @brief Configure clock and pins for keypad
 */
void KEY_Init(void)
{
	// Enable GPIO and ADC clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_AHB1PeriphClockCmd(KEY_PORT_CLK, ENABLE);

	// Configure key pin to analog input
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = KEY_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(KEY_PORT, &GPIO_InitStructure);

	ADC_CommonInitTypeDef ADC_CommonInitStruct;
	ADC_CommonStructInit(&ADC_CommonInitStruct);
	ADC_CommonInit(&ADC_CommonInitStruct);
}
