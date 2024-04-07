/** 
  ******************************************************************************
  * @file    BSP_Init.c 
  * @author  ample
  * @version V0.0.1
  * @date    2024��3��15��(V0.0.1)����ʼ�汾   
  * @brief   �弶֧�ְ���������
  ******************************************************************************
**/ 
/* 1.Include -------------------------------------------------------------------*/
#include "BSP_Init.h"
#include "main.h"
#include "lwip.h"
/* 2.Private typedef -----------------------------------------------------------*/
/* 3.Private define ------------------------------------------------------------*/
#define  TIM2_ic1_START_CNT         10
#define  TIM2_ic1_STOP_CNT 				  1010				// ������1010-10��

#define  TIM2_ic2_START_CNT         10
#define  TIM2_ic2_STOP_CNT 				  1010				// ������1010-10��
/* 4.Private macro -------------------------------------------------------------*/
/* 5.Global variables ---------------------------------------------------------*/
/* 6.Private variables ---------------------------------------------------------*/
uint32_t tim2_ic1_start_cnt_value;							// TIM2ͨ��1����ʼʱ��������ֵ
uint32_t tim2_ic1_stop_cnt_value;								// TIM2ͨ��1�������ʱ��������ֵ
double tim2_c1_frequency;	  											// TIM2ͨ��1�������õ���Ƶ��
uint8_t tim2_ic1_complete_flag = 0;	  					// TIM2ͨ��1����״̬��1Ϊ������ɣ�0Ϊ����δ���
uint16_t tim2_ic1_count = 0;										// TIM2ͨ��1�������

uint32_t tim2_ic2_start_cnt_value;							// TIM2ͨ��2����ʼʱ��������ֵ
uint32_t tim2_ic2_stop_cnt_value;								// TIM2ͨ��2�������ʱ��������ֵ
double tim2_c2_frequency;	  											// TIM2ͨ��2�������õ���Ƶ��
uint8_t tim2_ic2_complete_flag = 0;	  					// TIM2ͨ��2����״̬��1Ϊ������ɣ�0Ϊ����δ���
uint16_t tim2_ic2_count = 0;										// TIM2ͨ��2�������
extern uint16_t  PwmNum_Channel1;
extern uint16_t  PwmNum_Channel2;

extern SUASRT uart4;
extern SUASRT usart2;
extern uint8_t TIM2CH1_CAPTURE_STA;
extern uint16_t TIM2CH1_CAPTURE_VAL;
extern uint8_t TIM2CH2_CAPTURE_STA;
extern uint16_t TIM2CH2_CAPTURE_VAL;

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_uart4_rx;
DMA_HandleTypeDef hdma_uart4_tx;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;

ETH_BufferTypeDef Txbuffer[ETH_TX_DESC_CNT * 2U];
ETH_TxPacketConfig TxConfig;

ETH_HandleTypeDef heth;
/* 7.Global function prototypes -----------------------------------------------*/
/* 8.Private function prototypes -----------------------------------------------*/
/*******************************************************************************
** ��    �� �� BSP_Init
** ��    �� �� �弶֧�ְ���ʼ��
** ��ڲ��� �� ��
** ���ڲ��� �� ��
*******************************************************************************/
void BSP_Init(void)
{
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_UART4_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  //MX_ETH_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM5_Init();
  MX_TIM8_Init();
  MX_LWIP_Init();
  SCB_CleanInvalidateDCache();
}

/*******************************************************************************
** ��    �� �� MX_GPIO_Init
** ��    �� �� ����֧�ְ���ʼ��
** ��ڲ��� �� ��
** ���ڲ��� �� ��
*******************************************************************************/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOJ_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}
/*******************************************************************************
** ��    �� �� HAL_TIM_PWM_PulseFinishedCallback
** ��    �� �� PWM����жϻص�����
** ��ڲ��� �� htim��TIM�ṹ��
** ���ڲ��� �� ��
*******************************************************************************/
 
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
  static uint16_t cnt_channle1 = 0;    
  static uint16_t cnt_channle2 = 0;    
  if (htim->Instance == TIM1)
  {
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
    {
      cnt_channle1 ++;
      if(cnt_channle1 == PwmNum_Channel1)
			{
        rt_kprintf("PwmNum_Channel1 = %d\n",PwmNum_Channel1);
        HAL_TIM_PWM_Stop_IT(&htim1,TIM_CHANNEL_1);
      }                    
    }
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
    {
      cnt_channle2 ++;
      if(cnt_channle2 == PwmNum_Channel2)
			{
        rt_kprintf("PwmNum_Channel2 = %d\n",PwmNum_Channel2);
        HAL_TIM_PWM_Stop_IT(&htim1,TIM_CHANNEL_2);
      }                    
    }
  }
}
/*******************************************************************************
** ��    �� �� HAL_TIM_PeriodElapsedCallback
** ��    �� �� PWM�ſ��жϻص�����
** ��ڲ��� �� htim��TIM�ṹ��
** ���ڲ��� �� ��
*******************************************************************************/
void  HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim->Instance == TIM2)                      // �ж϶�ʱ��2�Ƿ����ж�
  {
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
    { 
      if((TIM2CH1_CAPTURE_STA & 0X80) == 0)           // ��δ�ɹ�����
      {
        if(TIM2CH1_CAPTURE_STA & 0X40)              // �Ѿ����񵽸ߵ�ƽ
        {		
            if((TIM2CH1_CAPTURE_STA & 0X3F) == 0X3F)// �ߵ�ƽʱ��̫���ˣ����������
            {	
                TIM2CH1_CAPTURE_STA |= 0X80;        // ���Ϊ���һ�β���
                TIM2CH1_CAPTURE_VAL = 0XFFFF;       // ������ֵ
            }
            else
            {
                TIM2CH1_CAPTURE_VAL++;              // ��û���������ֻ��TIM5CH1_CAP_STA�Լ�
            }                
        }	
      }
    }
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
    {
      if((TIM2CH2_CAPTURE_STA & 0X80) == 0)           // ��δ�ɹ�����
      {
        if(TIM2CH2_CAPTURE_STA & 0X40)              // �Ѿ����񵽸ߵ�ƽ
        {		
            if((TIM2CH1_CAPTURE_STA & 0X3F) == 0X3F)// �ߵ�ƽʱ��̫���ˣ����������
            {	
                TIM2CH1_CAPTURE_STA |= 0X80;        // ���Ϊ���һ�β���
                TIM2CH1_CAPTURE_VAL = 0XFFFF;       // ������ֵ
            }
            else
            {
                TIM2CH1_CAPTURE_VAL++;              // ��û���������ֻ��TIM5CH1_CAP_STA�Լ�
            }                
        }	
      }
    }
  }
  if(htim == &htim4)
  {
      if(__HAL_TIM_GET_FLAG(&htim4, TIM_FLAG_CC1) != RESET)  //�ж��Ƿ񴥷��ж�   
      {
          __HAL_TIM_CLEAR_FLAG(&htim4, TIM_FLAG_CC1);      //����жϱ�־
          HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);   //�ر�����ʱ��
          HAL_TIM_Base_Stop_IT(&htim4);         //�رմӶ�ʱ��
      }
  }
}
/*******************************************************************************
** ��    �� �� HAL_TIM_IC_CaptureCallback
** ��    �� �� TIM���벶���жϻص�����
** ��ڲ��� �� htim��TIM�ṹ��
** ���ڲ��� �� ��
*******************************************************************************/
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  if((TIM2CH1_CAPTURE_STA & 0X80) == 0)               // ��δ�ɹ�����
  {
		if(TIM2CH1_CAPTURE_STA & 0X40)                  // ����һ���½���
    {		
			TIM2CH1_CAPTURE_STA |= 0X80;	            // ��ǳɹ�����һ�θߵ�ƽ����
			TIM2CH1_CAPTURE_VAL = HAL_TIM_ReadCapturedValue(&htim2, TIM_CHANNEL_1); // ��ȡ��ǰ�ļ�����ֵ
			TIM_RESET_CAPTUREPOLARITY(&htim2, TIM_CHANNEL_1);	                // ���ԭ��������		
			TIM_SET_CAPTUREPOLARITY(&htim2,TIM_CHANNEL_1, TIM_ICPOLARITY_RISING);// ���������ز���
		}
		else
    {
			TIM2CH1_CAPTURE_STA = 0;                    // ����Զ����״̬�Ĵ���
			TIM2CH1_CAPTURE_VAL = 0;                    // ��ղ���ֵ
			TIM2CH1_CAPTURE_STA |= 0X40;	            // ��ǲ���������
			__HAL_TIM_DISABLE(&htim2);              // �رն�ʱ��
			__HAL_TIM_SET_COUNTER(&htim2, 0);       // ������ֵ����
			TIM_RESET_CAPTUREPOLARITY(&htim2,TIM_CHANNEL_1);	// һ��Ҫ�����ԭ��������	����			
			TIM_SET_CAPTUREPOLARITY(&htim2,TIM_CHANNEL_1,TIM_ICPOLARITY_RISING);   // �����½��ز���
			__HAL_TIM_ENABLE(&htim2);               // ʹ�ܶ�ʱ��		
		}	
	}
  if((TIM2CH2_CAPTURE_STA & 0X80) == 0)               // ��δ�ɹ�����
  {
		if(TIM2CH2_CAPTURE_STA & 0X40)                  // ����һ���½���
    {		
			TIM2CH2_CAPTURE_STA |= 0X80;	            // ��ǳɹ�����һ�θߵ�ƽ����
			TIM2CH2_CAPTURE_VAL = HAL_TIM_ReadCapturedValue(&htim2, TIM_CHANNEL_2); // ��ȡ��ǰ�ļ�����ֵ
			TIM_RESET_CAPTUREPOLARITY(&htim2, TIM_CHANNEL_2);	                // ���ԭ��������		
			TIM_SET_CAPTUREPOLARITY(&htim2,TIM_CHANNEL_2, TIM_ICPOLARITY_RISING);// ���������ز���
		}
		else
    {
			TIM2CH2_CAPTURE_STA = 0;                    // ����Զ����״̬�Ĵ���
			TIM2CH2_CAPTURE_VAL = 0;                    // ��ղ���ֵ
			TIM2CH2_CAPTURE_STA |= 0X40;	            // ��ǲ���������
			__HAL_TIM_DISABLE(&htim2);              // �رն�ʱ��
			__HAL_TIM_SET_COUNTER(&htim2, 0);       // ������ֵ����
			TIM_RESET_CAPTUREPOLARITY(&htim2,TIM_CHANNEL_2);	// һ��Ҫ�����ԭ��������	����			
			TIM_SET_CAPTUREPOLARITY(&htim2,TIM_CHANNEL_2,TIM_ICPOLARITY_FALLING);   // �����½��ز���
			__HAL_TIM_ENABLE(&htim2);               // ʹ�ܶ�ʱ��		
		}	
	}
  // if (htim->Instance == TIM2)
  // {
  //   if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
  //   {
  //     		/* ������� */
  //     tim2_ic1_count++;
  //     /* ����ʼ */
  //     if (tim2_ic1_count == TIM2_ic1_START_CNT)
  //     {
  //         /* ȡ����ʼʱ��������ֵ */
  //         tim2_ic1_start_cnt_value = TIM2->CNT;
  //       }
        
  //       /* ������� */
  //       if (tim2_ic1_count == TIM2_ic1_STOP_CNT)
  //       {	
  //         /* ȡ�������ʱ��������ֵ */
  //         tim2_ic1_stop_cnt_value = TIM2->CNT;
          
  //         /* ������ɱ�־��λ */
  //         tim2_ic1_complete_flag = 1;
  //     }
  //   }
  //   if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
  //   {
  //         /* ������� */
  //     tim2_ic2_count++;
      
  //     /* ����ʼ */
  //     if (tim2_ic2_count == TIM2_ic2_START_CNT)
  //     {
  //       /* ȡ����ʼʱ��������ֵ */
  //       tim2_ic2_start_cnt_value = TIM2->CNT;
  //     }
  //     /* ������� */
  //     if (tim2_ic2_count == TIM2_ic2_STOP_CNT)
  //     {	
  //       /* ȡ�������ʱ��������ֵ */
  //       tim2_ic2_stop_cnt_value = TIM2->CNT;
  //       /* ������ɱ�־��λ */
  //       tim2_ic2_complete_flag = 1;
  //     }
  //   }
  // }
}
/*******************************************************************************
** ��    �� �� TIM_IC1_Frequency
** ��    �� �� TIM2 ͨ��1���벶��
** ��ڲ��� �� ��
** ���ڲ��� �� Ƶ��
*******************************************************************************/
double TIM_IC1_Frequency(void)
{
	uint16_t count = 0;
	
	/* ������ɱ�־��λ */
	tim2_ic1_complete_flag = 0;
	
	/* �����벶�� */
	HAL_TIM_IC_Start(&htim2, TIM_CHANNEL_1);
	/* �����벶���ж� */
	__HAL_TIM_ENABLE_IT(&htim2, TIM_IT_CC1);

	/* �ȴ�������ɱ�־��λ */
	while (tim2_ic1_complete_flag != 1)
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
	HAL_TIM_IC_Stop(&htim2, TIM_CHANNEL_1);
	/* �ر����벶���ж� */
	__HAL_TIM_DISABLE_IT(&htim2, TIM_IT_CC1);
	
	/* ����Ƶ�� */
	tim2_c1_frequency = 200000000.0
										/
			((double)(tim2_ic1_stop_cnt_value - tim2_ic1_start_cnt_value) / (TIM2_ic1_STOP_CNT - TIM2_ic1_START_CNT));

	/* ������ɱ�־��λ */
	tim2_ic1_complete_flag = 0;

	/* TIM2���������0���ȴ��´βɼ� */
	tim2_ic1_count = 0;

	return tim2_c1_frequency;
}
/*******************************************************************************
** ��    �� �� TIM_IC2_Frequency
** ��    �� �� TIM2 ͨ��2���벶��
** ��ڲ��� �� ��
** ���ڲ��� �� Ƶ��
*******************************************************************************/
double TIM_IC2_Frequency(void)
{
	uint16_t count = 0;
	
	/* ������ɱ�־��λ */
	tim2_ic2_complete_flag = 0;
	
	/* �����벶�� */
	HAL_TIM_IC_Start(&htim2, TIM_CHANNEL_2);
	/* �����벶���ж� */
	__HAL_TIM_ENABLE_IT(&htim2, TIM_IT_CC2);
	
	/* �ȴ�������ɱ�־��λ */
	while (tim2_ic2_complete_flag != 1)
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
	HAL_TIM_IC_Stop(&htim2, TIM_CHANNEL_2);
	/* �ر����벶���ж� */
	__HAL_TIM_DISABLE_IT(&htim2, TIM_IT_CC2);
	
	/* ����Ƶ�� */
	tim2_c2_frequency = 200000000.0
										/
			((double)(tim2_ic2_stop_cnt_value - tim2_ic2_start_cnt_value) / (TIM2_ic2_STOP_CNT - TIM2_ic2_START_CNT));

	/* ������ɱ�־��λ */
	tim2_ic2_complete_flag = 0;

	/* TIM2���������0���ȴ��´βɼ� */
	tim2_ic2_count = 0;

	return tim2_c2_frequency;
}
/* USER CODE END 0 */

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim8;

/* TIM1 init function */
/*******************************************************************************
** ��    �� �� MX_TIM1_Init
** ��    �� �� TIM1��ʼ��
** ��ڲ��� �� ��
** ���ڲ��� �� ��
*******************************************************************************/
void MX_TIM1_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 1000-1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 1000-1;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 500;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_TIM_MspPostInit(&htim1);

}
/* TIM2 init function */
/*******************************************************************************
** ��    �� �� MX_TIM2_Init
** ��    �� �� TIM2��ʼ��
** ��ڲ��� �� ��
** ���ڲ��� �� ��
*******************************************************************************/
void MX_TIM2_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 100-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 65535;
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
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICFilter = 8;
  if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_TIM_Base_Start_IT(&htim2);  		//������ʱ��2����ж�
  __HAL_TIM_ENABLE_IT(&htim2,TIM_IT_UPDATE);  // һ��Ҫ����TIM2�ĸ����ж�
  HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);    //�������벶���ж�
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_2);    //�������벶���ж�
}
/* TIM3 init function */
/*******************************************************************************
** ��    �� �� MX_TIM3_Init
** ��    �� �� TIM3��ʼ��
** ��ڲ��� �� ��
** ���ڲ��� �� ��
*******************************************************************************/
void MX_TIM3_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 400-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 1000-1;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 500;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}
/* TIM4 init function */
/*******************************************************************************
** ��    �� �� MX_TIM4_Init
** ��    �� �� TIM4��ʼ��
** ��ڲ��� �� ��
** ���ڲ��� �� ��
*******************************************************************************/
void MX_TIM4_Init(void)
{
  TIM_SlaveConfigTypeDef sSlaveConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 0;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 65535;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_DISABLE;
  sSlaveConfig.InputTrigger = TIM_TS_ITR2;
  if (HAL_TIM_SlaveConfigSynchro(&htim4, &sSlaveConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
}
/*******************************************************************************
** ��    �� �� MX_TIM5_Init
** ��    �� �� TIM5��ʼ��
** ��ڲ��� �� ��
** ���ڲ��� �� ��
*******************************************************************************/
/* TIM5 init function */
void MX_TIM5_Init(void)
{


  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 0;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 4294967295;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim5) != HAL_OK)
  {
    Error_Handler();       
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim5, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }

}
/*******************************************************************************
** ��    �� �� MX_TIM8_Init
** ��    �� �� TIM8��ʼ��
** ��ڲ��� �� ��
** ���ڲ��� �� ��
*******************************************************************************/
/* TIM8 init function */
void MX_TIM8_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  htim8.Instance = TIM8;
  htim8.Init.Prescaler = 0;
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim8.Init.Period = 65535;
  htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim8.Init.RepetitionCounter = 0;
  htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim8) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim8, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }


}
/*******************************************************************************
** ��    �� �� HAL_TIM_Base_MspInit
** ��    �� �� TIM�����ʼ��
** ��ڲ��� �� tim_baseHandle
** ���ڲ��� �� ��
*******************************************************************************/
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(tim_baseHandle->Instance==TIM1)
  {
    /* TIM1 clock enable */
    __HAL_RCC_TIM1_CLK_ENABLE();

    /* TIM1 interrupt Init */
    HAL_NVIC_SetPriority(TIM1_CC_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);
  }
  else if(tim_baseHandle->Instance==TIM2)
  {
    /* TIM2 clock enable */
    __HAL_RCC_TIM2_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**TIM2 GPIO Configuration
    PA15 (JTDI)     ------> TIM2_CH1
    PB3 (JTDO/TRACESWO)     ------> TIM2_CH2
    */
    GPIO_InitStruct.Pin = GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* TIM2 interrupt Init */
    HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
  }
  else if(tim_baseHandle->Instance==TIM3)
  {
    /* TIM3 clock enable */
    __HAL_RCC_TIM3_CLK_ENABLE();
  }
  else if(tim_baseHandle->Instance==TIM4)
  {
    /* TIM4 clock enable */
    __HAL_RCC_TIM4_CLK_ENABLE();
  }
  else if(tim_baseHandle->Instance==TIM5)
  {
    /* TIM5 clock enable */
    __HAL_RCC_TIM5_CLK_ENABLE();
  }
  else if(tim_baseHandle->Instance==TIM8)
  {
    /* TIM8 clock enable */
    __HAL_RCC_TIM8_CLK_ENABLE();
  }
}
/*******************************************************************************
** ��    �� �� HAL_TIM_MspPostInit
** ��    �� �� ����PWM
** ��ڲ��� �� tim_baseHandle
** ���ڲ��� �� ��
*******************************************************************************/
void HAL_TIM_MspPostInit(TIM_HandleTypeDef* timHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(timHandle->Instance==TIM1)
  {
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOJ_CLK_ENABLE();
    /**TIM1 GPIO Configuration
    PA8     ------> TIM1_CH1
    PJ11     ------> TIM1_CH2
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
    HAL_GPIO_Init(GPIOJ, &GPIO_InitStruct);
  }
  else if(timHandle->Instance==TIM3)
  {
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**TIM3 GPIO Configuration
    PB5     ------> TIM3_CH2
    PB4 (NJTRST)     ------> TIM3_CH1
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  }

}
/*******************************************************************************
** ��    �� �� HAL_TIM_Base_MspDeInit
** ��    �� �� TIM����ע��
** ��ڲ��� �� tim_baseHandle
** ���ڲ��� �� ��
*******************************************************************************/
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM1)
  {
    /* Peripheral clock disable */
    __HAL_RCC_TIM1_CLK_DISABLE();

    /* TIM1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM1_CC_IRQn);

  }
  else if(tim_baseHandle->Instance==TIM2)
  {
    /* Peripheral clock disable */
    __HAL_RCC_TIM2_CLK_DISABLE();

    /**TIM2 GPIO Configuration
    PA15 (JTDI)     ------> TIM2_CH1
    PB3 (JTDO/TRACESWO)     ------> TIM2_CH2
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_15);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3);

    /* TIM2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM2_IRQn);
  }
  else if(tim_baseHandle->Instance==TIM3)
  {
    __HAL_RCC_TIM3_CLK_DISABLE();
  }
  else if(tim_baseHandle->Instance==TIM4)
  {
    /* Peripheral clock disable */
    __HAL_RCC_TIM4_CLK_DISABLE();
  }
  else if(tim_baseHandle->Instance==TIM5)
  {
    /* Peripheral clock disable */
    __HAL_RCC_TIM5_CLK_DISABLE();
  }
  else if(tim_baseHandle->Instance==TIM8)
  {
    /* Peripheral clock disable */
    __HAL_RCC_TIM8_CLK_DISABLE();
  }
}


/*******************************************************************************
** ��    �� �� MX_UART4_UART_Init
** ��    �� �� UART4��ʼ��
** ��ڲ��� �� ��
** ���ڲ��� �� ��
*******************************************************************************/
void MX_UART4_Init(void)
{
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 9600;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart4.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart4, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart4, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */
	__HAL_UART_CLEAR_IT(&huart4, UART_CLEAR_IDLEF);
	__HAL_UART_ENABLE_IT(&huart4, UART_IT_IDLE);//����IDLE�ж�
	
  HAL_UART_Receive_DMA(&huart4,uart4.rxbuf,USART_BUF_LEN);//����DMA����
  /* USER CODE END UART4_Init 2 */

}
/* USART1 init function */
/*******************************************************************************
** ��    �� �� MX_USART1_UART_Init
** ��    �� �� UART1��ʼ��
** ��ڲ��� �� ��
** ���ڲ��� �� ��
*******************************************************************************/
void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */
	__HAL_UART_ENABLE_IT(&huart1,UART_IT_RXNE); 
  /* USER CODE END USART1_Init 2 */

}
/* USART2 init function */
/*******************************************************************************
** ��    �� �� MX_USART2_UART_Init
** ��    �� �� UART2��ʼ��
** ��ڲ��� �� ��
** ���ڲ��� �� ��
*******************************************************************************/
void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */
	
	__HAL_UART_CLEAR_IT(&huart2, UART_CLEAR_IDLEF);
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);//����IDLE�ж�
  HAL_UART_Receive_DMA(&huart2,usart2.rxbuf,USART_BUF_LEN);//����DMA����
	
  /* USER CODE END USART2_Init 2 */

}
/*******************************************************************************
** ��    �� �� HAL_UART_MspDeInit
** ��    �� �� UART�����ʼ��
** ��ڲ��� �� uartHandle
** ���ڲ��� �� ��
*******************************************************************************/
void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(uartHandle->Instance==UART4)
  {

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_UART4;
    PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* UART4 clock enable */
    __HAL_RCC_UART4_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**UART4 GPIO Configuration
    PA12     ------> UART4_TX
    PA11     ------> UART4_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF6_UART4;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* UART4 DMA Init */
    /* UART4_RX Init */
    hdma_uart4_rx.Instance = DMA2_Stream0;
    hdma_uart4_rx.Init.Request = DMA_REQUEST_UART4_RX;
    hdma_uart4_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_uart4_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_uart4_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_uart4_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_uart4_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_uart4_rx.Init.Mode = DMA_NORMAL;
    hdma_uart4_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_uart4_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_uart4_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_uart4_rx);

    /* UART4_TX Init */
    hdma_uart4_tx.Instance = DMA2_Stream1;
    hdma_uart4_tx.Init.Request = DMA_REQUEST_UART4_TX;
    hdma_uart4_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_uart4_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_uart4_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_uart4_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_uart4_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_uart4_tx.Init.Mode = DMA_NORMAL;
    hdma_uart4_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_uart4_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_uart4_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmatx,hdma_uart4_tx);

    /* UART4 interrupt Init */
    HAL_NVIC_SetPriority(UART4_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(UART4_IRQn);
  }
  else if(uartHandle->Instance==USART1)
  {
  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1;
    PeriphClkInitStruct.Usart16ClockSelection = RCC_USART16CLKSOURCE_D2PCLK2;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA10     ------> USART1_RX
    PA9     ------> USART1_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  }
  else if(uartHandle->Instance==USART2)
  {
  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART2;
    PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* USART2 clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();

    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**USART2 GPIO Configuration
    PD5     ------> USART2_TX
    PD6     ------> USART2_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* USART2 DMA Init */
    /* USART2_RX Init */
    hdma_usart2_rx.Instance = DMA1_Stream0;
    hdma_usart2_rx.Init.Request = DMA_REQUEST_USART2_RX;
    hdma_usart2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart2_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart2_rx.Init.Mode = DMA_NORMAL;
    hdma_usart2_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart2_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart2_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart2_rx);

    /* USART2_TX Init */
    hdma_usart2_tx.Instance = DMA1_Stream1;
    hdma_usart2_tx.Init.Request = DMA_REQUEST_USART2_TX;
    hdma_usart2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart2_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart2_tx.Init.Mode = DMA_NORMAL;
    hdma_usart2_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart2_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart2_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart2_tx);

    /* USART2 interrupt Init */
    HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspInit 1 */

  /* USER CODE END USART2_MspInit 1 */
  }
}
/*******************************************************************************
** ��    �� �� HAL_UART_MspDeInit
** ��    �� �� UART�������
** ��ڲ��� �� uartHandle
** ���ڲ��� �� ��
*******************************************************************************/
void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==UART4)
  {
    /* Peripheral clock disable */
    __HAL_RCC_UART4_CLK_DISABLE();

    /**UART4 GPIO Configuration
    PA12     ------> UART4_TX
    PA11     ------> UART4_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_12|GPIO_PIN_11);

    /* UART4 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);
    HAL_DMA_DeInit(uartHandle->hdmatx);

    /* UART4 interrupt Deinit */
    HAL_NVIC_DisableIRQ(UART4_IRQn);
  }
  else if(uartHandle->Instance==USART1)
  {
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA10     ------> USART1_RX
    PA9     ------> USART1_TX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_10|GPIO_PIN_9);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  }
  else if(uartHandle->Instance==USART2)
  {
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();

    /**USART2 GPIO Configuration
    PD5     ------> USART2_TX
    PD6     ------> USART2_RX
    */
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_5|GPIO_PIN_6);

    /* USART2 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);
    HAL_DMA_DeInit(uartHandle->hdmatx);

    /* USART2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART2_IRQn);
  }
}
/*******************************************************************************
** ��    �� �� MX_DMA_Init
** ��    �� �� DMA��ʼ��
** ��ڲ��� �� ��
** ���ڲ��� �� ��
*******************************************************************************/
void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
  /* DMA1_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
  /* DMA2_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);
}
/*******************************************************************************
** ��    �� �� MX_ETH_Init
** ��    �� �� ETH��ʼ��
** ��ڲ��� �� ��
** ���ڲ��� �� ��
*******************************************************************************/
void MX_ETH_Init(void)
{
   static uint8_t MACAddr[6];
  heth.Instance = ETH;
  MACAddr[0] = 0x00;
  MACAddr[1] = 0x80;
  MACAddr[2] = 0xE1;
  MACAddr[3] = 0x00;
  MACAddr[4] = 0x00;
  MACAddr[5] = 0x00;
  heth.Init.MACAddr = &MACAddr[0];
  heth.Init.MediaInterface = HAL_ETH_MII_MODE;
  heth.Init.TxDesc = DMATxDscrTab;
  heth.Init.RxDesc = DMARxDscrTab;
  heth.Init.RxBuffLen = 1524;

  if (HAL_ETH_Init(&heth) != HAL_OK)
  {
    Error_Handler();
  }

  memset(&TxConfig, 0 , sizeof(ETH_TxPacketConfig));
  TxConfig.Attributes = ETH_TX_PACKETS_FEATURES_CSUM | ETH_TX_PACKETS_FEATURES_CRCPAD;
  TxConfig.ChecksumCtrl = ETH_CHECKSUM_IPHDR_PAYLOAD_INSERT_PHDR_CALC;
  TxConfig.CRCPadCtrl = ETH_CRC_PAD_INSERT;
}
/*******************************************************************************
** ��    �� �� HAL_ETH_MspInit
** ��    �� �� ETH���ӳ�ʼ��
** ��ڲ��� �� ��
** ���ڲ��� �� ��
*******************************************************************************/
void HAL_ETH_MspInit(ETH_HandleTypeDef* ethHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(ethHandle->Instance==ETH)
  {

    /* ETH clock enable */
    __HAL_RCC_ETH1MAC_CLK_ENABLE();
    __HAL_RCC_ETH1TX_CLK_ENABLE();
    __HAL_RCC_ETH1RX_CLK_ENABLE();

    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**ETH GPIO Configuration
    PG11     ------> ETH_TX_EN
    PE2     ------> ETH_TXD3
    PG12     ------> ETH_TXD1
    PG13     ------> ETH_TXD0
    PC1     ------> ETH_MDC
    PC2     ------> ETH_TXD2
    PC3     ------> ETH_TX_CLK
    PH2     ------> ETH_CRS
    PA2     ------> ETH_MDIO
    PA1     ------> ETH_RX_CLK
    PH3     ------> ETH_COL
    PA7     ------> ETH_RX_DV
    PC4     ------> ETH_RXD0
    PB1     ------> ETH_RXD3
    PH6     ------> ETH_RXD2
    PC5     ------> ETH_RXD1
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4
                          |GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_1|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  }
}
/*******************************************************************************
** ��    �� �� HAL_ETH_MspDeInit
** ��    �� �� ETH����ע��
** ��ڲ��� �� ��
** ���ڲ��� �� ��
*******************************************************************************/
void HAL_ETH_MspDeInit(ETH_HandleTypeDef* ethHandle)
{

  if(ethHandle->Instance==ETH)
  {
  /* USER CODE BEGIN ETH_MspDeInit 0 */

  /* USER CODE END ETH_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ETH1MAC_CLK_DISABLE();
    __HAL_RCC_ETH1TX_CLK_DISABLE();
    __HAL_RCC_ETH1RX_CLK_DISABLE();

    /**ETH GPIO Configuration
    PG11     ------> ETH_TX_EN
    PE2     ------> ETH_TXD3
    PG12     ------> ETH_TXD1
    PG13     ------> ETH_TXD0
    PC1     ------> ETH_MDC
    PC2     ------> ETH_TXD2
    PC3     ------> ETH_TX_CLK
    PH2     ------> ETH_CRS
    PA2     ------> ETH_MDIO
    PA1     ------> ETH_RX_CLK
    PH3     ------> ETH_COL
    PA7     ------> ETH_RX_DV
    PC4     ------> ETH_RXD0
    PB1     ------> ETH_RXD3
    PH6     ------> ETH_RXD2
    PC5     ------> ETH_RXD1
    */
    HAL_GPIO_DeInit(GPIOG, GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13);

    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_2);

    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4
                          |GPIO_PIN_5);

    HAL_GPIO_DeInit(GPIOH, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_6);

    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_1|GPIO_PIN_7);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_1);


  }
}
