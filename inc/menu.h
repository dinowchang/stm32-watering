/**
  ******************************************************************************
  * @file    menu.h
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

#ifndef __INC_MENU_H_
#define __INC_MENU_H_

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef struct
{
	void (*open)(void);
	void (*close)(void);
	void (*redraw)(void);
	void (*up)(void);
	void (*down)(void);
	void (*right)(void);
	void (*left)(void);
	void (*select)(void);
} Menu_t;

/* Exported constants --------------------------------------------------------*/
#define MENU_WAKEUP_ALARM					0x0001
#define MENU_WAKEUP_KEYPAD					0x0002


/* Exported macro ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
void MENU_Init(void);
void MENU_SetWakeupEvent(uint32_t wakeupEvent);

#endif /* __INC_MENU_H_ */
