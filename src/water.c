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
#include "soil-moisture.h"

/* Private typedef -----------------------------------------------------------*/
#define WATER_TASK_PRIORITY					( tskIDLE_PRIORITY + 3UL )
#define WATER_TASK_STACK					( 256/4 ) 							// 2048 bytes


/* Private define ------------------------------------------------------------*/
#define SUPPORT_WATER_TEST_COMMAND				0 // debug command for FreeRTOS-CLI

#define WATER_DEFAULT_HOUR						8
#define WATER_DEFAULT_MINUTE					0
#define WATER_DEFAULT_SECOND					0
#define WATER_DEFAULT_PERIOD					5000
#define WATER_DEFAULT_MOISTURE_THRESHOLD		200

#define WATER_GPIO_CLOCK_PORT					RCC_AHB1Periph_GPIOC
#define WATER_PIN_PORT							GPIOC
#define WATER_PIN_NUM							GPIO_Pin_13

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static TaskHandle_t m_waterTask;
static SemaphoreHandle_t m_waterLock;

static uint32_t m_period;
static uint16_t m_moistureThreshold;
static RTC_AlarmTypeDef m_waterTime;


/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
 * @brief	Set the period for valve open
 */
void WATER_SetPeriod(uint32_t newPeriod)
{
	vTaskSuspendAll();
	m_period = newPeriod;
	xTaskResumeAll();
}

/**
 * @brief	Get the poriod for valve open
 */
uint32_t WATER_GetPeriod(void)
{
	return m_period;
}

/**
 * @brief	Set the moisture thresold to water
 */
void WATER_SetThreshold(uint16_t newThreshold)
{
	vTaskSuspendAll();
	m_moistureThreshold = newThreshold;
	xTaskResumeAll();
}

/**
 * @brief	Get the moisture threshold to water
 */
uint16_t WATER_GetThreshold(void)
{
	return m_moistureThreshold;
}

/**
 * @brief	Set the time of valve open
 */
void WATER_SetWaterTime(RTC_TimeTypeDef *RTC_TimeStruct)
{
	vTaskSuspendAll();
	RTC_AlarmCmd(RTC_Alarm_A, DISABLE);

	m_waterTime.RTC_AlarmTime.RTC_H12 = RTC_TimeStruct->RTC_H12;
	m_waterTime.RTC_AlarmTime.RTC_Hours = RTC_TimeStruct->RTC_Hours;
	m_waterTime.RTC_AlarmTime.RTC_Minutes = RTC_TimeStruct->RTC_Minutes;
	m_waterTime.RTC_AlarmTime.RTC_Seconds = RTC_TimeStruct->RTC_Seconds;
	m_waterTime.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;
	RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &m_waterTime);

	RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
	xTaskResumeAll();
}

/**
 * @brief	Get the time of valve open
 */
void WATER_GetWaterTime(RTC_TimeTypeDef *RTC_TimeStruct)
{
	RTC_TimeStruct->RTC_H12 = m_waterTime.RTC_AlarmTime.RTC_H12;
	RTC_TimeStruct->RTC_Hours = m_waterTime.RTC_AlarmTime.RTC_Hours;
	RTC_TimeStruct->RTC_Minutes = m_waterTime.RTC_AlarmTime.RTC_Minutes;
	RTC_TimeStruct->RTC_Seconds = m_waterTime.RTC_AlarmTime.RTC_Seconds;
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
	xTaskNotifyGive(m_waterTask);

	pcWriteBuffer[0] = '\0'; // clean Write Buffer

	return pdFALSE;
}

static const CLI_Command_Definition_t xWaterOpen =
{
	"water",
	"water :\n    Open valve for set period\n",
	WATER_OpenCommand,
	0
};
#endif

/**
 * @brief	Request a watering process
 */
void WATER_SendRequest(void)
{
	xTaskNotifyGive(m_waterTask);
}

bool WATER_Lock(void)
{
	if( xSemaphoreTake(m_waterLock, 0) )
		return TRUE;
	else
		return FALSE;
}

void WATER_Unlock(void)
{
	xSemaphoreGive(m_waterLock);
}


/**
 * @brief Water task for valve controlling
 * @param pvParameters
 */
static void WATER_Task( void *pvParameters )
{
	uint16_t moisture;
	while(1)
	{
		ulTaskNotifyTake( pdTRUE, portMAX_DELAY );

		xSemaphoreTake(m_waterLock, portMAX_DELAY);

		SOIL_Open();
		vTaskDelay(100 / portTICK_PERIOD_MS); // wait sensor stable
		moisture = SOIL_Read();
		vTaskDelay(100 / portTICK_PERIOD_MS); // wait sensor stable
		moisture = SOIL_Read();
		SOIL_Close();

		if( moisture < m_moistureThreshold)
		{
			GPIO_ResetBits(WATER_PIN_PORT, WATER_PIN_NUM);
			vTaskDelay(m_period / portTICK_PERIOD_MS);
			GPIO_SetBits(WATER_PIN_PORT, WATER_PIN_NUM);
		}

		xSemaphoreGive(m_waterLock);
	}
}

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

	// set default water time
	m_waterTime.RTC_AlarmTime.RTC_H12 = RTC_H12_AM;
	m_waterTime.RTC_AlarmTime.RTC_Hours = WATER_DEFAULT_HOUR;
	m_waterTime.RTC_AlarmTime.RTC_Minutes = WATER_DEFAULT_MINUTE;
	m_waterTime.RTC_AlarmTime.RTC_Seconds = WATER_DEFAULT_SECOND;
	m_waterTime.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;
	RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &m_waterTime);
	RTC_AlarmCmd(RTC_Alarm_A, ENABLE);

	m_period = WATER_DEFAULT_PERIOD;
	m_moistureThreshold = WATER_DEFAULT_MOISTURE_THRESHOLD;
	m_waterLock = xSemaphoreCreateMutex();

	// Create that task that handles the console itself.
	xTaskCreate( 	WATER_Task,					/* The task that implements the command console. */
					"WATER",					/* Text name assigned to the task.  This is just to assist debugging.  The kernel does not use this name itself. */
					WATER_TASK_STACK,			/* The size of the stack allocated to the task. */
					NULL,						/* The parameter is not used, so NULL is passed. */
					WATER_TASK_PRIORITY,		/* The priority allocated to the task. */
					&m_waterTask );				/* A handle is not required, so just pass NULL. */


#if SUPPORT_WATER_TEST_COMMAND
	FreeRTOS_CLIRegisterCommand(&xWaterOpen);
#endif
}
