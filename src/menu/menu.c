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

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define MENU_TASK_PRIORITY					( tskIDLE_PRIORITY + 1UL )
#define MENU_KEY_DETECTION_TIMEOUT			100
#define MENU_KEY_POLLING_DELAY				10

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static Menu_t *m_currentMenu;

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

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

/**
 *
 * @param pvParameters
 */
static void MENU_Task( void *pvParameters )
{
	TickType_t xLastFlashTime;
	xLastFlashTime = xTaskGetTickCount();

	Key_t key;

	while(1)
	{
		// Reset Menu
		KEY_Enable();

		m_currentMenu = &MENU_DEFAULT_MENU;
		if( m_currentMenu->open != NULL )
			m_currentMenu->open();

		// Start menu loop
		while(1)
		{
			key = MENU_GetNewKey(MENU_KEY_DETECTION_TIMEOUT);

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
		vTaskDelayUntil( &xLastFlashTime, 4500 );

	}
}

/**
 * @brief	Menu task initialize flow before schedule start
 */
void MENU_Init(void)
{
	xTaskCreate( MENU_Task, "LED", configMINIMAL_STACK_SIZE, NULL, MENU_TASK_PRIORITY, NULL );
}
