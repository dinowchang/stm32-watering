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

/* Exported macro ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
void MENU_Init(void);

#endif /* __INC_MENU_H_ */
