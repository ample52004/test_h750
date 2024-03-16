/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Mechine1_X_E_Pin GPIO_PIN_0
#define Mechine1_X_E_GPIO_Port GPIOF
#define Mechine1_Y_E_Pin GPIO_PIN_1
#define Mechine1_Y_E_GPIO_Port GPIOF
#define Mechine2_X_E_Pin GPIO_PIN_2
#define Mechine2_X_E_GPIO_Port GPIOF
#define Mechine2_Y_E_Pin GPIO_PIN_3
#define Mechine2_Y_E_GPIO_Port GPIOF
#define Mechine3_X_E_Pin GPIO_PIN_4
#define Mechine3_X_E_GPIO_Port GPIOF
#define Mechine3_Y_E_Pin GPIO_PIN_5
#define Mechine3_Y_E_GPIO_Port GPIOF
#define Mechine4_X_E_Pin GPIO_PIN_6
#define Mechine4_X_E_GPIO_Port GPIOF
#define Mechine4_Y_E_Pin GPIO_PIN_7
#define Mechine4_Y_E_GPIO_Port GPIOF
#define Mechine4_PWM_X_Pin GPIO_PIN_1
#define Mechine4_PWM_X_GPIO_Port GPIOA
#define Mechine4_PWM_Y_Pin GPIO_PIN_2
#define Mechine4_PWM_Y_GPIO_Port GPIOA
#define Mechine2_PWM_X_Pin GPIO_PIN_6
#define Mechine2_PWM_X_GPIO_Port GPIOA
#define Mechine2_PWM_Y_Pin GPIO_PIN_7
#define Mechine2_PWM_Y_GPIO_Port GPIOA
#define Mechine1_PWM_Y_Pin GPIO_PIN_10
#define Mechine1_PWM_Y_GPIO_Port GPIOB
#define Mechine1_PWM_X_Pin GPIO_PIN_11
#define Mechine1_PWM_X_GPIO_Port GPIOB
#define Mechine3_PWM_X_Pin GPIO_PIN_12
#define Mechine3_PWM_X_GPIO_Port GPIOD
#define Mechine3_PWM_Y_Pin GPIO_PIN_13
#define Mechine3_PWM_Y_GPIO_Port GPIOD
#define Mechine1_Dir_X_Pin GPIO_PIN_2
#define Mechine1_Dir_X_GPIO_Port GPIOG
#define Mechine1_Dir_Y_Pin GPIO_PIN_3
#define Mechine1_Dir_Y_GPIO_Port GPIOG
#define Mechine2_Dir_X_Pin GPIO_PIN_4
#define Mechine2_Dir_X_GPIO_Port GPIOG
#define Mechine2_Dir_Y_Pin GPIO_PIN_5
#define Mechine2_Dir_Y_GPIO_Port GPIOG
#define Mechine3_Dir_X_Pin GPIO_PIN_6
#define Mechine3_Dir_X_GPIO_Port GPIOC
#define Mechine3_Dir_Y_Pin GPIO_PIN_7
#define Mechine3_Dir_Y_GPIO_Port GPIOC
#define Mechine4_Dir_X_Pin GPIO_PIN_8
#define Mechine4_Dir_X_GPIO_Port GPIOC
#define Mechine4_Dir_Y_Pin GPIO_PIN_9
#define Mechine4_Dir_Y_GPIO_Port GPIOC

/* USER CODE BEGIN Private defines */
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
