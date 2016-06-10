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

	FreeRTOS_CLIRegisterCommand( &xCalendarGetTime );
	FreeRTOS_CLIRegisterCommand( &xCalendarSetTime );
}
