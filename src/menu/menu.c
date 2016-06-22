/**
  ******************************************************************************
  * @file    menu.c
  * @author  dinow
  * @version V0.0.1
  * @date    2016-06-17
  * @brief
  ******************************************************************************
  * @attention
  *
  *
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "menufunc.h"
#include "keypad.h"
#include "water.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define MENU_TASK_PRIORITY					( tskIDLE_PRIORITY + 1UL )
#define MENU_TASK_STACK						( 2048/4 ) 							// 2048 bytes

#define MENU_KEY_POLLING_DELAY				(10 / portTICK_PERIOD_MS)
#define MENU_KEY_DETECTION_TIMEOUT			(100 / portTICK_PERIOD_MS)
#define MENU_KEY_SLEEP_TIMEOUT				(30000 / portTICK_PERIOD_MS)

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static Menu_t *m_currentMenu;
static uint32_t m_wakeupEvent = 0;

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

void MENU_SetWakeupEvent(uint32_t wakeupEvent)
{
	m_wakeupEvent |= wakeupEvent;
}


void MENU_ClrWakeupEvent(uint32_t wakeupEvent)
{
	taskENTER_CRITICAL();
	m_wakeupEvent &= ~wakeupEvent;
	taskEXIT_CRITICAL();
}

/**
 * @brief	switch menu
 * @param	newMenu
 */
void MENU_SwitchMenu(Menu_t *newMenu)
{
	if( m_currentMenu->close != NULL)
		m_currentMenu->close();

	m_currentMenu = newMenu;

	if( m_currentMenu->open != NULL )
		m_currentMenu->open();
}

/**
 * @brief	Get the new key pressed with debounce
 * @param	timeout :
 * @return	key pressed
 */
Key_t MENU_GetNewKey(int16_t timeout)
{
	Key_t ret = Key_None;
	Key_t key;

	bool newKey = FALSE;
	int8_t pressCount = 0;
	int32_t startTime = (int32_t) xTaskGetTickCount();

	while ((((int32_t) xTaskGetTickCount()) - startTime) < timeout)
	{
		key = KEY_GetKey();

		if (key == Key_None)
		{
			pressCount = 0;
			newKey = TRUE;
			ret = Key_None;
		}
		else
		{
			if (newKey == TRUE)
			{
				newKey = FALSE;
				ret = key;
				pressCount = 1;
			}
			else
			{
				if (ret != Key_None)
				{
					if (ret == key)
						pressCount++;
					else
					{
						ret = key;
						pressCount = 1;
					}
				}

				if (pressCount == 2)
					return ret;
			}

		}

		vTaskDelay(MENU_KEY_POLLING_DELAY);
	}

	return Key_None;
}

void MENU_Sleep(void)
{
	LCD_Sleep(ENABLE);
	KEY_SetIntrMode(ENABLE);

	RTC_TimeTypeDef RTC_SleepTime, RTC_WakeupTime;
	RTC_GetTime(RTC_Format_BIN, &RTC_SleepTime);

	// Disable systick interrupt to prevent that wake up immediately
	SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;

	// Enter stop mode
	PWR_EnterSTOPMode(PWR_MainRegulator_ON, PWR_STOPEntry_WFI);

	// clock is changed when exit stop mode, reset again
	extern void ResetSysClock(void);
	ResetSysClock();

	// Enable systick interrupt
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;

	RTC_GetTime(RTC_Format_BIN, &RTC_WakeupTime);

	DEBUG_printf(DBG_MENU, "Sleep @ %02d:%02d:%02d, Wake up @ %02d:%02d:%02d\n",
			RTC_SleepTime.RTC_Hours, RTC_SleepTime.RTC_Minutes,
			RTC_SleepTime.RTC_Seconds, RTC_WakeupTime.RTC_Hours,
			RTC_WakeupTime.RTC_Minutes, RTC_WakeupTime.RTC_Seconds);

	KEY_SetIntrMode(DISABLE);
	LCD_Sleep(DISABLE);
}

/**
 *
 * @param pvParameters
 */
static void MENU_Task( void *pvParameters )
{
	Key_t key;
	int32_t idleStartTime = (int32_t) xTaskGetTickCount();

	while(1)
	{

		// Check watering event
		if( m_wakeupEvent & MENU_WAKEUP_ALARM)
		{
			MENU_ClrWakeupEvent(MENU_WAKEUP_ALARM);
			WATER_SendRequest();
		}

		// Check keypad event

		// Reset Menu
		KEY_Enable();
		idleStartTime = (int32_t) xTaskGetTickCount();

		m_currentMenu = &MENU_DEFAULT_MENU;
		if( m_currentMenu->open != NULL )
			m_currentMenu->open();

		// Start menu loop
		while(1)
		{
			key = MENU_GetNewKey(MENU_KEY_DETECTION_TIMEOUT);

			if( key != Key_None )
			{
				idleStartTime = (int32_t) xTaskGetTickCount();
			}
			else
			{
				if ((((int32_t) xTaskGetTickCount()) - idleStartTime) > MENU_KEY_SLEEP_TIMEOUT)
				{
					if( m_currentMenu->close != NULL)
						m_currentMenu->close();

					break;
				}
			}

			switch(key)
			{
				case Key_None:
					if( m_currentMenu->redraw != NULL )
						m_currentMenu->redraw();
					break;

				case Key_Left:
					if( m_currentMenu->left != NULL )
						m_currentMenu->left();
					break;

				case Key_Right:
					if( m_currentMenu->right != NULL )
						m_currentMenu->right();
					break;

				case Key_Up:
					if( m_currentMenu->up != NULL )
						m_currentMenu->up();
					break;

				case Key_Down:
					if( m_currentMenu->down != NULL )
						m_currentMenu->down();
					break;

				case Key_Select:
					if( m_currentMenu->select != NULL )
						m_currentMenu->select();
					break;

				default:
					break;
			}
		}

		// Enter Sleep mode
		if( WATER_Lock() )
		{
			MENU_Sleep();
			WATER_Unlock();
		}
		else
		{
			vTaskDelay(100 / portTICK_PERIOD_MS);
		}

	}
}

/**
 * @brief	Menu task initialize flow before schedule start
 */
void MENU_Init(void)
{
	xTaskCreate( MENU_Task, "MENU", MENU_TASK_STACK, NULL, MENU_TASK_PRIORITY, NULL );
}
