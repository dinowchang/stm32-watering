/**
  ******************************************************************************
  * @file    water.h
  * @author  hcchang
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

#ifndef __INC_WATER_H_
#define __INC_WATER_H_

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
void WATER_Init(void);
void WATER_Process(void);
void WATER_SetPeriod(uint32_t newPeriod);
uint32_t WATER_GetPeriod(void);
void WATER_SetWaterTime(RTC_TimeTypeDef *RTC_TimeStruct);
void WATER_GetWaterTime(RTC_TimeTypeDef *RTC_TimeStruct);

#endif /* __INC_WATER_H_ */
