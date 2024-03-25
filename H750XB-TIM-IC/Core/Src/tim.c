/**
  ******************************************************************************
  * @file    tim.c
  * @brief   This file provides code for the configuration
  *          of the TIM instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "tim.h"

/* USER CODE BEGIN 0 */


#define  TIM2_IC3_START_CNT         10
#define  TIM2_IC3_STOP_CNT 				  1010				// ������1010-10��

uint32_t tim2_ic3_start_cnt_value;							// TIM2����ʼʱ��������ֵ
uint32_t tim2_ic3_stop_cnt_value;								// TIM2�������ʱ��������ֵ
double tim2_frequency;	  											// TIM2�������õ���Ƶ��
uint8_t tim2_ic3_complete_flag = 0;	  					// TIM2����״̬��1Ϊ������ɣ�0Ϊ����δ���
uint16_t tim2_ic3_count = 0;										// TIM2�������



/**
  * @brief  TIM���벶���жϻص�����
  * @note   ��
  * @param  htim��TIM�ṹ��
  * @retval ��
  */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM2)
  {
		/* ������� */
		tim2_ic3_count++;
		
		/* ����ʼ */
		if (tim2_ic3_count == TIM2_IC3_START_CNT)
		{
			/* ȡ����ʼʱ��������ֵ */
			tim2_ic3_start_cnt_value = TIM2->CNT;
		}
		
		/* ������� */
		if (tim2_ic3_count == TIM2_IC3_STOP_CNT)
		{	
			/* ȡ�������ʱ��������ֵ */
			tim2_ic3_stop_cnt_value = TIM2->CNT;
			
			/* ������ɱ�־��λ */
			tim2_ic3_complete_flag = 1;
		}
  }
}


/**
  * @brief  TIM2���벶��
  * @note   ��
  * @param  ��
  * @retval Ƶ��
  */
double TIM_IC_Frequency(void)
{
	uint16_t count = 0;
	
	/* ������ɱ�־��λ */
	tim2_ic3_complete_flag = 0;
	
	/* �����벶�� */
	HAL_TIM_IC_Start(&htim2, TIM_CHANNEL_3);
	/* �����벶���ж� */
	__HAL_TIM_ENABLE_IT(&htim2, TIM_IT_CC3);
	
	/* �ȴ�������ɱ�־��λ */
	while (tim2_ic3_complete_flag != 1)
	{
		count++;
		/* ��ʱ */
		if (count > 2000)
		{
			printf("No frequency!\r\n");
			break;
		}
		HAL_Delay(1);
	}
	
	/* �ر����벶�� */
	HAL_TIM_IC_Stop(&htim2, TIM_CHANNEL_3);
	/* �ر����벶���ж� */
	__HAL_TIM_DISABLE_IT(&htim2, TIM_IT_CC3);
	
	/* ����Ƶ�� */
	tim2_frequency = 240000000.0
										/
			((double)(tim2_ic3_stop_cnt_value - tim2_ic3_start_cnt_value) / (TIM2_IC3_STOP_CNT - TIM2_IC3_START_CNT));

	/* ������ɱ�־��λ */
	tim2_ic3_complete_flag = 0;

	/* TIM2���������0���ȴ��´βɼ� */
	tim2_ic3_count = 0;

	return tim2_frequency;
}


/* USER CODE END 0 */

TIM_HandleTypeDef htim2;

/* TIM2 init function */
void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 0xFFFFFFFF;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(tim_baseHandle->Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspInit 0 */

  /* USER CODE END TIM2_MspInit 0 */
    /* TIM2 clock enable */
    __HAL_RCC_TIM2_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**TIM2 GPIO Configuration
    PA2     ------> TIM2_CH3
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* TIM2 interrupt Init */
    HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
  /* USER CODE BEGIN TIM2_MspInit 1 */

  /* USER CODE END TIM2_MspInit 1 */
  }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspDeInit 0 */

  /* USER CODE END TIM2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM2_CLK_DISABLE();

    /**TIM2 GPIO Configuration
    PA2     ------> TIM2_CH3
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2);

    /* TIM2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM2_IRQn);
  /* USER CODE BEGIN TIM2_MspDeInit 1 */

  /* USER CODE END TIM2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
