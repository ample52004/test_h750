/** 
  ******************************************************************************
  * @file    BSP_Init.h 
  * @author  ample
  * @version V0.0.1
  * @date    2024-3-15
  * @brief   鏉跨骇鏀寔鍖呴┍鍔ㄧ▼搴忓ご鏂囦欢
  ******************************************************************************
**/ 

#ifndef _CONFIG_H
#define _CONFIG_H
/* 1.Include ------------------------------------------------------------------*/
#include "main.h"
#include "string.h"
#include "stm32h7xx_hal.h"
#include "stm32h7xx.h"

/* 2.export typedef -----------------------------------------------------------*/
/* 3.export define ------------------------------------------------------------*/
#define  TIM2_ic1_START_CNT         10

#define  TIM2_ic1_STOP_CNT 				  1010				// 捕获了1010-10次

#define  TIM2_ic2_START_CNT         10

#define  TIM2_ic2_STOP_CNT 				  1010				// 捕获了1010-10次
/* 4.export macro -------------------------------------------------------------*/

/* 5.export variables ---------------------------------------------------------*/
extern UART_HandleTypeDef huart4;

extern UART_HandleTypeDef huart1;

extern UART_HandleTypeDef huart2;


extern TIM_HandleTypeDef htim1;

extern TIM_HandleTypeDef htim2;

extern TIM_HandleTypeDef htim3;

extern TIM_HandleTypeDef htim4;

extern TIM_HandleTypeDef htim5;

extern TIM_HandleTypeDef htim8;

extern ETH_HandleTypeDef heth;

/* 6.export function prototypes -----------------------------------------------*/
extern void BSP_Init(void);

void MX_ETH_Init(void);
void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
double TIM_IC1_Frequency(void);

double TIM_IC2_Frequency(void);
#endif
/* end------------------------------------------------------------------------*/
