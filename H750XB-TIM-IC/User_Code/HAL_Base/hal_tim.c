
/* Includes ------------------------------------------------------------------*/
#include "hal_tim.h"
#include "hal_usart.h"


TIM_STATUS_TypeDef TIM_STATUS_Struct;

#if  1
/**
  * @brief  TIM�����жϻص�����
  * @note   ��
  * @param  htim��TIM�ṹ��
  * @retval ��
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM_PERIPHERAL) 
	{
		/* UART����һ֡��ɱ�־��λ */
		UART_Transfer_Struct.Transfer_Status |= UART_TRANSFER_RXEND;
		
		/* TIM����״̬ */
		TIM_STATUS_Struct.TIM_Status = TIM_STATUS_IDLE;
		
		/* �رն�ʱ�� */
		__HAL_TIM_DISABLE(&TIM_HAL_STRUCT);
	}
	
	else
	{
		/* ������ʱ�� */
	}
}
#endif


/**
  * @brief  ���³�ʼ��TIM
  * @note   ��
  * @param  Prescaler����Ƶϵ��
  * @param  Period����װ��ֵ
  * @retval ��
  */
void TIM_HAL_Init(uint16_t Prescaler, uint16_t Period)
{
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	
	/* TIM clock enable */
  TIM_PERIPHERAL_RCC_ENABLE();
		
	TIM_HAL_STRUCT.Instance = TIM_PERIPHERAL;
	TIM_HAL_STRUCT.Init.Prescaler = Prescaler;
	TIM_HAL_STRUCT.Init.CounterMode = TIM_COUNTERMODE_UP;
	TIM_HAL_STRUCT.Init.Period = Period;
	TIM_HAL_STRUCT.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&TIM_HAL_STRUCT) != HAL_OK)
	{
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&TIM_HAL_STRUCT, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
	
	/* �򿪸����ж� */
	__HAL_TIM_ENABLE_IT(&TIM_HAL_STRUCT, TIM_IT_UPDATE);
}


