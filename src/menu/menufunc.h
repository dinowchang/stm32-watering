/**
  ******************************************************************************
  * @file    menufunc.h
  * @author  dinow
  * @version V0.0.1
  * @date    2016-06-17
  * @brief   header file for menu/
  ******************************************************************************
  * @attention
  *
  *
  *
  ******************************************************************************
  */

#ifndef __SRC_MENU_MENUFUNC_H_
#define __SRC_MENU_MENUFUNC_H_

/* Includes ------------------------------------------------------------------*/
#include "config.h"
#include "type.h"
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "debug.h"
#include "lcd1602.h"
#include "menu.h"

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
#define MENU_DEFAULT_MENU				(mainMenu)

#define DEBUG_MENU						1

/* Exported macro ------------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/
// declare menu here
extern Menu_t mainMenu;
extern Menu_t moistureMenu;

/* Exported functions ------------------------------------------------------- */
void MENU_SwitchMenu(Menu_t *newMenu);

#endif /* __SRC_MENU_MENUFUNC_H_ */
