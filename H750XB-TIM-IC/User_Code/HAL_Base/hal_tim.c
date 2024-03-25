
/* Includes ------------------------------------------------------------------*/
#include "hal_tim.h"
#include "hal_usart.h"


TIM_STATUS_TypeDef TIM_STATUS_Struct;

#if  1
/**
  * @brief  TIM更新中断回调函数
  * @note   无
  * @param  htim：TIM结构体
  * @retval 无
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM_PERIPHERAL) 
	{
		/* UART接收一帧完成标志置位 */
		UART_Transfer_Struct.Transfer_Status |= UART_TRANSFER_RXEND;
		
		/* TIM空闲状态 */
		TIM_STATUS_Struct.TIM_Status = TIM_STATUS_IDLE;
		
		/* 关闭定时器 */
		__HAL_TIM_DISABLE(&TIM_HAL_STRUCT);
	}
	
	else
	{
		/* 其他定时器 */
	}
}
#endif


/**
  * @brief  重新初始化TIM
  * @note   无
  * @param  Prescaler：分频系数
  * @param  Period：重装载值
  * @retval 无
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
	
	/* 打开更新中断 */
	__HAL_TIM_ENABLE_IT(&TIM_HAL_STRUCT, TIM_IT_UPDATE);
}


