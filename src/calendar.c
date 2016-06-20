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

/**
 * @brief Show RTC time
 * @param pcWriteBuffer
 * @param xWriteBufferLen
 * @param pcCommandString
 * @return
 */

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

/**
 * @brief Set RTC time
 * @param pcWriteBuffer
 * @param xWriteBufferLen
 * @param pcCommandString
 * @return
 */
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

/**
 * @brief Show alarm
 * @param pcWriteBuffer
 * @param xWriteBufferLen
 * @param pcCommandString
 * @return
 */
static BaseType_t CALENDAR_GetAlarmCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	RTC_AlarmTypeDef RTC_AlarmStruct;
	RTC_GetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStruct);

	sprintf(pcWriteBuffer, "System Time: %02d:%02d:%02d\n",
			RTC_AlarmStruct.RTC_AlarmTime.RTC_Hours, RTC_AlarmStruct.RTC_AlarmTime.RTC_Minutes,
			RTC_AlarmStruct.RTC_AlarmTime.RTC_Seconds);

	return pdFALSE;

}

static const CLI_Command_Definition_t xCalendarGetAlarm =
{
	"get_alarm", /* The command string to type. */
	"get_alarm:\n    Displays alarm time of system\n",
	CALENDAR_GetAlarmCommand, /* The function to run. */
	0 /* No parameters are expected. */
};

/**
 * @brief set alarm
 * @param pcWriteBuffer
 * @param xWriteBufferLen
 * @param pcCommandString
 * @return
 */
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

/**
 * @brief force system to enter stop mode
 * @param pcWriteBuffer
 * @param xWriteBufferLen
 * @param pcCommandString
 * @return
 */
static BaseType_t CALENDAR_SleepCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	RTC_TimeTypeDef RTC_SleepTime, RTC_WakeupTime;
	RTC_GetTime(RTC_Format_BIN, &RTC_SleepTime);

	// Disable systick interrupt to prevent that wake up immediately
	SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;

	// Enter stop mode
	PWR_EnterSTOPMode(PWR_MainRegulator_ON, PWR_STOPEntry_WFI);

	// clock is changed when exit stop mode, reset again
	extern void SetSysClock(void);
	SetSysClock();

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

/**
 * @brief start RTC calibration mode
 * @param pcWriteBuffer
 * @param xWriteBufferLen
 * @param pcCommandString
 * @return
 */
static BaseType_t CALENDAR_RtcCalibrationCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	uint32_t mode;

	const char *parameterPtr;
	int32_t paramterLen;

	parameterPtr = FreeRTOS_CLIGetParameter(pcCommandString, 1, &paramterLen);
	mode = DecToInt((char *) parameterPtr, paramterLen);

	if( mode == 1 )
	{
		// Enable RTC 512Hz clock output
		RTC_CalibOutputConfig(RTC_CalibOutput_512Hz);
		RTC_CalibOutputCmd(ENABLE);

		sprintf(pcWriteBuffer, "Enable RTC 512Hz clock output\n");
		return pdFALSE;
	}
	else if( mode == 2 )
	{
		// Enable RTC 1Hz clock output
		RTC_CalibOutputConfig(RTC_CalibOutput_1Hz);
		RTC_CalibOutputCmd(ENABLE);

		sprintf(pcWriteBuffer, "Enable RTC 512Hz clock output\n");
		return pdFALSE;
	}
	else
	{
		// Disable RTC clock output
		RTC_CalibOutputCmd(DISABLE);

		sprintf(pcWriteBuffer, "Disable RTC clock output\n");
		return pdFALSE;
	}

	return pdFALSE;
}

static const CLI_Command_Definition_t xRtcCalibration =
{
	"rtc_cal", /* The command string to type. */
	"rtc_cal <mode>:\n    enter RTC calibration mode 0: off, 1: 512Hz, 2: 1Hz\n",
	CALENDAR_RtcCalibrationCommand, /* The function to run. */
	1 /* No parameters are expected. */
};

/**
 * @brief Initialize RTC
 */
void CALENDAR_Init(void)
{
	// Enable the PWR clock
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	// Allow access to RTC
	PWR_BackupAccessCmd(ENABLE);

	// Reset RTC Domain
	RCC_BackupResetCmd(ENABLE);
	RCC_BackupResetCmd(DISABLE);

	// Enable the LSE OSC
	RCC_LSEConfig(RCC_LSE_ON);

	// Wait until LSE is ready
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);

	// Select the RTC Clock Source
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

	// Enable RTC Clock
	RCC_RTCCLKCmd(ENABLE);

	// Configure RTC Clock divider
    RTC_InitTypeDef RTC_InitStructure;
    RTC_StructInit(&RTC_InitStructure);
    RTC_Init(&RTC_InitStructure);

    // Enable SYSCFG clock for EXTI
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    // Configure EXTI Line 17 for RTC_Alarm
	EXTI_InitTypeDef EXTI_InitStruct;
	EXTI_StructInit(&EXTI_InitStruct);
	EXTI_InitStruct.EXTI_Line = EXTI_Line17;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruct);

	// Configure NVIC RTC_Alarm interrupt
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// Enable RTC_Alarm interrupt
	RTC_ITConfig(RTC_IT_ALRA, ENABLE);

	// Register CLI command
	FreeRTOS_CLIRegisterCommand( &xCalendarGetTime );
	FreeRTOS_CLIRegisterCommand( &xCalendarSetTime );
	FreeRTOS_CLIRegisterCommand( &xCalendarGetAlarm );
	FreeRTOS_CLIRegisterCommand( &xCalendarSetAlarm );
	FreeRTOS_CLIRegisterCommand( &xCalendarSleep );
	FreeRTOS_CLIRegisterCommand( &xRtcCalibration );
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
		}
	}

}
