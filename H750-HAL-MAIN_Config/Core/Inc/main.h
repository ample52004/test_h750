/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdint.h"
#include "stdio.h"
#include "stdarg.h"
#include "string.h"
#include "rtthread.h"
#define    USART_BUF_LEN    128           // 定义缓存128字节                              // 
#define LED1_Toggle HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0)
#define LED2_Toggle HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_3)
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
// 串口设备数据结构
typedef struct
{
	unsigned char rxbuf[USART_BUF_LEN];
	unsigned char txbuf[USART_BUF_LEN];
	unsigned short rxlen;
	unsigned short txlen;
}SUASRT;
/* USER CODE BEGIN ET */

extern SUASRT usart2;
extern SUASRT uart4;
/* USER CODE END ET */

extern unsigned char Rx_Buf[USART_BUF_LEN];
//extern unsigned int reclen;
extern unsigned char flag;
/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

extern uint8_t rx_len; 
extern uint8_t recv_flag_usart2;
extern uint8_t recv_flag_uart4;

extern uint16_t PwmNum_Channel1;
extern uint16_t PwmNum_Channel2;
/* USER CODE END ET */


/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);
extern void MPU_Config(void);
/* USER CODE BEGIN EFP */
void delay_us(uint16_t nus);
void delay_s(uint16_t s);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */

#define TIM_HANDLE &htim5 //定义这个是为了方便移植
#define TIM_HANDLE_S &htim8 //定义这个是为了方便移植

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
