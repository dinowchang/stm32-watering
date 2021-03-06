/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.6.0
  * @date    04-September-2015
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "config.h"
#include "stm32f4xx_it.h"
#include "debug.h"
#include "command.h"
#include "menu.h"

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

#if 0	// SVC_Handler used by FreeRTOS
/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}
#endif

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

#if DEBUG_PORT == DEBUG_PORT_USART2
/**
 * @brief  USART2 interrupt handler
 */
void USART2_IRQHandler(void)
{
	char data;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	while (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
#if TEST_USART_RX_ECHO
		data = USART_ReceiveData(USART2);
		USART_SendData(USART2, data);
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
#else
		data = USART_ReceiveData(USART2);
		xQueueSendFromISR(xCommandQueue, &data, &xHigherPriorityTaskWoken);
#endif
	}
}

#elif DEBUG_PORT == DEBUG_PORT_USART3
/**
 * @brief  USART3 interrupt handler
 */
void USART3_IRQHandler(void)
{
	while (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
#if TEST_USART_RX_ECHO
		char data = USART_ReceiveData(USART3);
		USART_SendData(USART3, data);
		while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
#else
		USART_ReceiveData(USART3);
#endif
	}
}
#endif

/**
 * @brief  EXTI0 interrupt handler
 */
void EXTI0_IRQHandler(void)
{
	if ( EXTI_GetITStatus(EXTI_Line0) )
	{
		/* Clear EXTI line0 pending bit */
		EXTI_ClearITPendingBit(EXTI_Line0);
		MENU_SetWakeupEvent(MENU_WAKEUP_KEYPAD);
	}
}

/**
 * @brief IRQ handler of RTC alarm
 */
void RTC_Alarm_IRQHandler(void)
{
	if ( EXTI_GetITStatus(EXTI_Line17) )
	{
		/* Clear EXTI line17 pending bit */
		EXTI_ClearITPendingBit(EXTI_Line17);

		if (RTC_GetITStatus(RTC_IT_ALRA) != RESET)
		{
			/* Clear the Alarm A Pending Bit */
			RTC_ClearITPendingBit(RTC_IT_ALRA);

			MENU_SetWakeupEvent(MENU_WAKEUP_ALARM);
		}
	}
}



/**
  * @}
  */ 


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
