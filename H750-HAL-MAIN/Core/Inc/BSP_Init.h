/** 
  ******************************************************************************
  * @file    BSP_Init.h 
  * @author  ample
  * @version V0.0.1
  * @date    2024-3-15
  * @brief   板级支持包驱动程序头文件
  ******************************************************************************
**/ 

#ifndef _CONFIG_H
#define _CONFIG_H
/* 1.Include ------------------------------------------------------------------*/
#include "main.h"
/* 2.export typedef -----------------------------------------------------------*/

/* 3.export define ------------------------------------------------------------*/
/* 4.export macro -------------------------------------------------------------*/
/* 5.export variables ---------------------------------------------------------*/
extern TIM_HandleTypeDef htim1;

extern TIM_HandleTypeDef htim2;

extern TIM_HandleTypeDef htim3;

extern TIM_HandleTypeDef htim4;

extern TIM_HandleTypeDef htim5;

extern TIM_HandleTypeDef htim8;

extern UART_HandleTypeDef huart4;

extern UART_HandleTypeDef huart1;

extern UART_HandleTypeDef huart2;

extern ETH_HandleTypeDef heth;


/* 6.export function prototypes -----------------------------------------------*/
extern void BSP_Init(void) ;

void MX_TIM1_Init(void);
void MX_TIM2_Init(void);
void MX_TIM3_Init(void);
void MX_TIM4_Init(void);
void MX_TIM5_Init(void);
void MX_TIM8_Init(void);


void MX_UART4_Init(void);
void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

void MX_ETH_Init(void);

void MX_GPIO_Init(void);

void MX_DMA_Init(void);
/* USER CODE BEGIN Prototypes */
double TIM_IC1_Frequency(void);
double TIM_IC2_Frequency(void);
#endif
/* end------------------------------------------------------------------------*/
