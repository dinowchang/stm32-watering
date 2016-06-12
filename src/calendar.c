/**
  ******************************************************************************
  * @file    calendar.c
  * @author  dinow
  * @version V0.0.1
  * @date    2016-06-10
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

/* Private macro -------------------------------------------------------------*/
/************************* PLL Parameters *************************************/
#if defined(STM32F40_41xxx) || defined(STM32F427_437xx) || defined(STM32F429_439xx) || defined(STM32F401xx) || defined(STM32F469_479xx)
 /* PLL_VCO = (HSE_VALUE or HSI_VALUE / PLL_M) * PLL_N */
 #define PLL_M      25
#elif defined (STM32F446xx)
 #define PLL_M      8
#elif defined (STM32F410xx) || defined (STM32F411xE)
 #if defined(USE_HSE_BYPASS)
  #define PLL_M      8
 #else /* !USE_HSE_BYPASS */
  #define PLL_M      16
 #endif /* USE_HSE_BYPASS */
#else
#endif /* STM32F40_41xxx || STM32F427_437xx || STM32F429_439xx || STM32F401xx || STM32F469_479xx */

/* USB OTG FS, SDIO and RNG Clock =  PLL_VCO / PLLQ */
#define PLL_Q      7

#if defined(STM32F446xx)
/* PLL division factor for I2S, SAI, SYSTEM and SPDIF: Clock =  PLL_VCO / PLLR */
#define PLL_R      7
#endif /* STM32F446xx */

#if defined(STM32F40_41xxx) || defined(STM32F427_437xx) || defined(STM32F429_439xx) || defined(STM32F446xx) || defined(STM32F469_479xx)
#define PLL_N      360
/* SYSCLK = PLL_VCO / PLL_P */
#define PLL_P      2
#endif /* STM32F40_41xxx || STM32F427_437x || STM32F429_439xx || STM32F446xx || STM32F469_479xx */

#if defined(STM32F401xx)
#define PLL_N      336
/* SYSCLK = PLL_VCO / PLL_P */
#define PLL_P      4
#endif /* STM32F401xx */

#if defined(STM32F410xx) || defined(STM32F411xE)
#define PLL_N      400
/* SYSCLK = PLL_VCO / PLL_P */
#define PLL_P      4
#endif /* STM32F410xx || STM32F411xE */

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
static BaseType_t CALENDAR_GetTimeCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
	sprintf(pcWriteBuffer, "System Time: %02d:%02d:%02d\n",
			RTC_TimeStruct.RTC_Hours, RTC_TimeStruct.RTC_Minutes,
			RTC_TimeStruct.RTC_Seconds);
	return pdFALSE;
}

static const CLI_Command_Definition_t xCalendarGetTime =
{
	"get_time", /* The command string to type. */
	"get_time:\n    Displays current time of system\n",
	CALENDAR_GetTimeCommand, /* The function to run. */
	0 /* No parameters are expected. */
};

static BaseType_t CALENDAR_SetTimeCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	uint32_t hours, minutes, seconds;

	const char *parameterPtr;
	int32_t paramterLen;

	parameterPtr = FreeRTOS_CLIGetParameter(pcCommandString, 1, &paramterLen);
	if (paramterLen > 3 && parameterPtr[0] == '0' && parameterPtr[1] == 'x')
		hours = HexToInt((char *) parameterPtr, paramterLen);
	else
		hours = DecToInt((char *) parameterPtr, paramterLen);

	parameterPtr = FreeRTOS_CLIGetParameter(pcCommandString, 2, &paramterLen);
	if (paramterLen > 3 && parameterPtr[0] == '0' && parameterPtr[1] == 'x')
		minutes = HexToInt((char *) parameterPtr, paramterLen);
	else
		minutes = DecToInt((char *) parameterPtr, paramterLen);

	parameterPtr = FreeRTOS_CLIGetParameter(pcCommandString, 3, &paramterLen);
	if (paramterLen > 3 && parameterPtr[0] == '0' && parameterPtr[1] == 'x')
		seconds = HexToInt((char *) parameterPtr, paramterLen);
	else
		seconds = DecToInt((char *) parameterPtr, paramterLen);

	sprintf(pcWriteBuffer, "Set system time to %02ld:%02ld:%02ld\n", hours, minutes,
			seconds);

	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_TimeStruct.RTC_H12 = RTC_H12_AM;
	RTC_TimeStruct.RTC_Hours = hours;
	RTC_TimeStruct.RTC_Minutes = minutes;
	RTC_TimeStruct.RTC_Seconds = seconds;
	RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);

	return pdFALSE;
}

static const CLI_Command_Definition_t xCalendarSetTime =
{
	"set_time", /* The command string to type. */
	"set_time: <hour> <minute> <second>\n    Set time of system\n",
	CALENDAR_SetTimeCommand, /* The function to run. */
	3 /* No parameters are expected. */
};

static BaseType_t CALENDAR_SetAlarmCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	RTC_AlarmCmd(RTC_Alarm_A, DISABLE);

	uint32_t hours, minutes, seconds;

	const char *parameterPtr;
	int32_t paramterLen;

	parameterPtr = FreeRTOS_CLIGetParameter(pcCommandString, 1, &paramterLen);
	if (paramterLen > 3 && parameterPtr[0] == '0' && parameterPtr[1] == 'x')
		hours = HexToInt((char *) parameterPtr, paramterLen);
	else
		hours = DecToInt((char *) parameterPtr, paramterLen);

	parameterPtr = FreeRTOS_CLIGetParameter(pcCommandString, 2, &paramterLen);
	if (paramterLen > 3 && parameterPtr[0] == '0' && parameterPtr[1] == 'x')
		minutes = HexToInt((char *) parameterPtr, paramterLen);
	else
		minutes = DecToInt((char *) parameterPtr, paramterLen);

	parameterPtr = FreeRTOS_CLIGetParameter(pcCommandString, 3, &paramterLen);
	if (paramterLen > 3 && parameterPtr[0] == '0' && parameterPtr[1] == 'x')
		seconds = HexToInt((char *) parameterPtr, paramterLen);
	else
		seconds = DecToInt((char *) parameterPtr, paramterLen);

	RTC_AlarmTypeDef RTC_AlarmStruct;
	RTC_AlarmStruct.RTC_AlarmTime.RTC_H12 = RTC_H12_AM;
	RTC_AlarmStruct.RTC_AlarmTime.RTC_Hours = hours;
	RTC_AlarmStruct.RTC_AlarmTime.RTC_Minutes = minutes;
	RTC_AlarmStruct.RTC_AlarmTime.RTC_Seconds = seconds;

	RTC_AlarmStruct.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;
	RTC_AlarmStruct.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_WeekDay;
	RTC_AlarmStruct.RTC_AlarmDateWeekDay = RTC_Weekday_Friday;

	RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStruct);

	RTC_AlarmCmd(RTC_Alarm_A, ENABLE);

	sprintf(pcWriteBuffer, "Set alarm to %02ld:%02ld:%02ld\n", hours, minutes,
			seconds);


	return pdFALSE;
}

static const CLI_Command_Definition_t xCalendarSetAlarm =
{
	"set_alarm", /* The command string to type. */
	"set_alarm: <hour> <minute> <second>\n    Set alarm of system\n",
	CALENDAR_SetAlarmCommand, /* The function to run. */
	3 /* No parameters are expected. */
};

static void RecoverSysClock(void)
{

#if defined(STM32F40_41xxx) || defined(STM32F427_437xx) || defined(STM32F429_439xx) || defined(STM32F401xx) || defined(STM32F446xx)|| defined(STM32F469_479xx)
/******************************************************************************/
/*            PLL (clocked by HSE) used as System clock source                */
/******************************************************************************/
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0;

  /* Enable HSE */
  RCC->CR |= ((uint32_t)RCC_CR_HSEON);

  /* Wait till HSE is ready and if Time out is reached exit */
  do
  {
    HSEStatus = RCC->CR & RCC_CR_HSERDY;
    StartUpCounter++;
  } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

  if ((RCC->CR & RCC_CR_HSERDY) != RESET)
  {
    HSEStatus = (uint32_t)0x01;
  }
  else
  {
    HSEStatus = (uint32_t)0x00;
  }

  if (HSEStatus == (uint32_t)0x01)
  {
    /* Select regulator voltage output Scale 1 mode */
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CR |= PWR_CR_VOS;

    /* HCLK = SYSCLK / 1*/
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

#if defined(STM32F40_41xxx) || defined(STM32F427_437xx) || defined(STM32F429_439xx) || defined(STM32F446xx) || defined(STM32F469_479xx)
    /* PCLK2 = HCLK / 2*/
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;

    /* PCLK1 = HCLK / 4*/
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;
#endif /* STM32F40_41xxx || STM32F427_437x || STM32F429_439xx  || STM32F446xx || STM32F469_479xx */

#if defined(STM32F401xx)
    /* PCLK2 = HCLK / 2*/
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;

    /* PCLK1 = HCLK / 4*/
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
#endif /* STM32F401xx */

#if defined(STM32F40_41xxx) || defined(STM32F427_437xx) || defined(STM32F429_439xx) || defined(STM32F401xx) || defined(STM32F469_479xx)
    /* Configure the main PLL */
    RCC->PLLCFGR = PLL_M | (PLL_N << 6) | (((PLL_P >> 1) -1) << 16) |
                   (RCC_PLLCFGR_PLLSRC_HSE) | (PLL_Q << 24);
#endif /* STM32F40_41xxx || STM32F401xx || STM32F427_437x || STM32F429_439xx || STM32F469_479xx */

#if defined(STM32F446xx)
    /* Configure the main PLL */
    RCC->PLLCFGR = PLL_M | (PLL_N << 6) | (((PLL_P >> 1) -1) << 16) |
                   (RCC_PLLCFGR_PLLSRC_HSE) | (PLL_Q << 24) | (PLL_R << 28);
#endif /* STM32F446xx */

    /* Enable the main PLL */
    RCC->CR |= RCC_CR_PLLON;

    /* Wait till the main PLL is ready */
    while((RCC->CR & RCC_CR_PLLRDY) == 0)
    {
    }

#if defined(STM32F427_437xx) || defined(STM32F429_439xx) || defined(STM32F446xx) || defined(STM32F469_479xx)
    /* Enable the Over-drive to extend the clock frequency to 180 Mhz */
    PWR->CR |= PWR_CR_ODEN;
    while((PWR->CSR & PWR_CSR_ODRDY) == 0)
    {
    }
    PWR->CR |= PWR_CR_ODSWEN;
    while((PWR->CSR & PWR_CSR_ODSWRDY) == 0)
    {
    }
    /* Configure Flash prefetch, Instruction cache, Data cache and wait state */
    FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_5WS;
#endif /* STM32F427_437x || STM32F429_439xx || STM32F446xx || STM32F469_479xx */

#if defined(STM32F40_41xxx)
    /* Configure Flash prefetch, Instruction cache, Data cache and wait state */
    FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_5WS;
#endif /* STM32F40_41xxx  */

#if defined(STM32F401xx)
    /* Configure Flash prefetch, Instruction cache, Data cache and wait state */
    FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_2WS;
#endif /* STM32F401xx */

    /* Select the main PLL as system clock source */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    /* Wait till the main PLL is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL);
    {
    }
  }
  else
  { /* If HSE fails to start-up, the application will have wrong clock
         configuration. User can add here some code to deal with this error */
  }
#elif defined(STM32F410xx) || defined(STM32F411xE)
#if defined(USE_HSE_BYPASS)
/******************************************************************************/
/*            PLL (clocked by HSE) used as System clock source                */
/******************************************************************************/
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0;

  /* Enable HSE and HSE BYPASS */
  RCC->CR |= ((uint32_t)RCC_CR_HSEON | RCC_CR_HSEBYP);

  /* Wait till HSE is ready and if Time out is reached exit */
  do
  {
    HSEStatus = RCC->CR & RCC_CR_HSERDY;
    StartUpCounter++;
  } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

  if ((RCC->CR & RCC_CR_HSERDY) != RESET)
  {
    HSEStatus = (uint32_t)0x01;
  }
  else
  {
    HSEStatus = (uint32_t)0x00;
  }

  if (HSEStatus == (uint32_t)0x01)
  {
    /* Select regulator voltage output Scale 1 mode */
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CR |= PWR_CR_VOS;

    /* HCLK = SYSCLK / 1*/
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

    /* PCLK2 = HCLK / 2*/
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;

    /* PCLK1 = HCLK / 4*/
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;

    /* Configure the main PLL */
    RCC->PLLCFGR = PLL_M | (PLL_N << 6) | (((PLL_P >> 1) -1) << 16) |
                   (RCC_PLLCFGR_PLLSRC_HSE) | (PLL_Q << 24);

    /* Enable the main PLL */
    RCC->CR |= RCC_CR_PLLON;

    /* Wait till the main PLL is ready */
    while((RCC->CR & RCC_CR_PLLRDY) == 0)
    {
    }

    /* Configure Flash prefetch, Instruction cache, Data cache and wait state */
    FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_2WS;

    /* Select the main PLL as system clock source */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    /* Wait till the main PLL is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL);
    {
    }
  }
  else
  { /* If HSE fails to start-up, the application will have wrong clock
         configuration. User can add here some code to deal with this error */
  }
#else /* HSI will be used as PLL clock source */
  /* Select regulator voltage output Scale 1 mode */
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;
  PWR->CR |= PWR_CR_VOS;

  /* HCLK = SYSCLK / 1*/
  RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

  /* PCLK2 = HCLK / 2*/
  RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;

  /* PCLK1 = HCLK / 4*/
  RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;

  /* Configure the main PLL */
  RCC->PLLCFGR = PLL_M | (PLL_N << 6) | (((PLL_P >> 1) -1) << 16) | (PLL_Q << 24);

  /* Enable the main PLL */
  RCC->CR |= RCC_CR_PLLON;

  /* Wait till the main PLL is ready */
  while((RCC->CR & RCC_CR_PLLRDY) == 0)
  {
  }

  /* Configure Flash prefetch, Instruction cache, Data cache and wait state */
  FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_2WS;

  /* Select the main PLL as system clock source */
  RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
  RCC->CFGR |= RCC_CFGR_SW_PLL;

  /* Wait till the main PLL is used as system clock source */
  while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL);
  {
  }
#endif /* USE_HSE_BYPASS */
#endif /* STM32F40_41xxx || STM32F427_437xx || STM32F429_439xx || STM32F401xx || STM32F469_479xx */

}

static BaseType_t CALENDAR_SleepCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	RTC_TimeTypeDef RTC_SleepTime, RTC_WakeupTime;
	RTC_GetTime(RTC_Format_BIN, &RTC_SleepTime);

	// Disable systick interrupt to prevent that wake up immediately
	SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;

	// Enter stop mode
	PWR_EnterSTOPMode(PWR_MainRegulator_ON, PWR_STOPEntry_WFI);

	// clock is changed when exit stop mode, reset again
	RecoverSysClock();

	// Enable systick interrupt
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;

	RTC_GetTime(RTC_Format_BIN, &RTC_WakeupTime);

	sprintf(pcWriteBuffer, "Sleep @ %02d:%02d:%02d, Wake up @ %02d:%02d:%02d\n",
			RTC_SleepTime.RTC_Hours, RTC_SleepTime.RTC_Minutes,
			RTC_SleepTime.RTC_Seconds, RTC_WakeupTime.RTC_Hours,
			RTC_WakeupTime.RTC_Minutes, RTC_WakeupTime.RTC_Seconds);

	return pdFALSE;
}

static const CLI_Command_Definition_t xCalendarSleep =
{
	"sleep", /* The command string to type. */
	"sleep:\n    enter stop mode\n",
	CALENDAR_SleepCommand, /* The function to run. */
	0 /* No parameters are expected. */

};

void CALENDAR_Init(void)
{
	// Enable the PWR clock
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	// Allow access to RTC
	PWR_BackupAccessCmd(ENABLE);

	/* Reset RTC Domain */
	RCC_BackupResetCmd(ENABLE);
	RCC_BackupResetCmd(DISABLE);

	/* Enable the LSE OSC */
	RCC_LSEConfig(RCC_LSE_ON);

	/* Wait until LSE is ready */
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);

	/* Enable RTC Clock */
	RCC_RTCCLKCmd(ENABLE);

	/* Select the RTC Clock Source */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

    RTC_InitTypeDef RTC_InitStructure;
    RTC_StructInit(&RTC_InitStructure);
    RTC_Init(&RTC_InitStructure);

    // Enable SYSCFG clock for EXTI
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    /* Configure EXTI Line 17 for RTC_Alarm */
	EXTI_InitTypeDef EXTI_InitStruct;
	EXTI_StructInit(&EXTI_InitStruct);
	EXTI_InitStruct.EXTI_Line = EXTI_Line17;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruct);

	/* Enable the RTC_Alarm interrupt */
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	RTC_ITConfig(RTC_IT_ALRA, ENABLE);

	FreeRTOS_CLIRegisterCommand( &xCalendarGetTime );
	FreeRTOS_CLIRegisterCommand( &xCalendarSetTime );
	FreeRTOS_CLIRegisterCommand( &xCalendarSetAlarm );
	FreeRTOS_CLIRegisterCommand( &xCalendarSleep );

}

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
		}
	}

}
