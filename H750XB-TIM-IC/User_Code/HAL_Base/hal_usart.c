
/* Includes ------------------------------------------------------------------*/
#include "hal_usart.h"
#include "hal_tim.h"
#include "hal_base.h"


UART_Transfer_TypeDef UART_Transfer_Struct;


/**
  * @brief  初始化UART
  * @note   无
  * @param  BaudRate：波特率
  * @retval 无
  */
void UART_HAL_Init(uint32_t BaudRate)
{
	UART_HAL_STRUCT.Instance = UART_PERIPHERAL;
	UART_HAL_STRUCT.Init.BaudRate = BaudRate;
	UART_HAL_STRUCT.Init.WordLength = UART_WORDLENGTH_8B;
	UART_HAL_STRUCT.Init.StopBits = UART_STOPBITS_1;
	UART_HAL_STRUCT.Init.Parity = UART_PARITY_NONE;
	UART_HAL_STRUCT.Init.Mode = UART_MODE_TX_RX;
	UART_HAL_STRUCT.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	UART_HAL_STRUCT.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&UART_HAL_STRUCT) != HAL_OK)
	{
		Error_Handler();
	}
	
	/* 打开串口中断 */
	__HAL_UART_ENABLE_IT(&UART_HAL_STRUCT, UART_IT_RXNE);
	
	/* 配置定时器，超过10个字节的时间下没接收到数据则触发更新中断 */
	TIM_HAL_Init(71, 10 * (1000000 / (BaudRate / 10)));
}


/**
  * @brief  通过UART发送一帧数据
  * @note   无
  * @param  Data：数据缓冲区
  * @param  Length：数据缓冲区长度
  * @retval 无
  */
void UART_HAL_TransmitData(uint8_t *Data, uint16_t Length)
{
	HAL_UART_Transmit(&UART_HAL_STRUCT, (uint8_t *)Data, Length, 10 * Length);
}


/**
  * @brief  UART接收中断回调函数
  * @note   UART接收完成在TIM的中断里
  * @param  无
  * @retval 无
  */
void UART_HAL_RxCpltCallback(void)
{
	/* 读取UART接收数据寄存器中1Byte的数据 */
	UART_Transfer_Struct.RxData[UART_Transfer_Struct.RxData_Len] = UART_RXDATA_REGISTER;
	UART_Transfer_Struct.RxData_Len++;
	
	/* 是否一帧数据过长 */
	if (UART_Transfer_Struct.RxData_Len == UART_DATALEN_MAX - 1)
	{
		UART_Transfer_Struct.RxData_Len = 0;
	}
	
	/* TIM状态忙 */
	TIM_STATUS_Struct.TIM_Status = TIM_STATUS_BUSY;
	
	/* TIM计数器归0，重新计时 */
	TIM_CNT_REGISTER = 0;
		
	/* 使能定时器 */
	__HAL_TIM_ENABLE(&TIM_HAL_STRUCT);
}


/**
  * @brief  UART发送完成中断回调函数
  * @note   该函数由UART_IRQHandler调用
  * @param  无
  * @retval 无
  */
void UART_HAL_TxCpltCallback(void)
{
  /* UART发送一帧完成标志置位 */
  UART_Transfer_Struct.Transfer_Status |= UART_TRANSFER_TXEND;
}


/* 重定向printf函数到串口 */
int fputc(int ch, FILE *f)
{
	HAL_UART_Transmit(&DEBUG_UART_STRUCT, (uint8_t *)&ch, 1, 100);
	
	return ch;
}



