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
void WATER_SendRequest(void);
void WATER_SetPeriod(uint32_t newPeriod);
uint32_t WATER_GetPeriod(void);
void WATER_SetWaterTime(RTC_TimeTypeDef *RTC_TimeStruct);
void WATER_GetWaterTime(RTC_TimeTypeDef *RTC_TimeStruct);
void WATER_SetThreshold(uint16_t newThreshold);
uint16_t WATER_GetThreshold(void);
bool WATER_Lock(void);
void WATER_Unlock(void);

#endif /* __INC_WATER_H_ */
