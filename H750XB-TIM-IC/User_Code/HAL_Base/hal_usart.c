
/* Includes ------------------------------------------------------------------*/
#include "hal_usart.h"
#include "hal_tim.h"
#include "hal_base.h"


UART_Transfer_TypeDef UART_Transfer_Struct;


/**
  * @brief  ��ʼ��UART
  * @note   ��
  * @param  BaudRate��������
  * @retval ��
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
	
	/* �򿪴����ж� */
	__HAL_UART_ENABLE_IT(&UART_HAL_STRUCT, UART_IT_RXNE);
	
	/* ���ö�ʱ��������10���ֽڵ�ʱ����û���յ������򴥷������ж� */
	TIM_HAL_Init(71, 10 * (1000000 / (BaudRate / 10)));
}


/**
  * @brief  ͨ��UART����һ֡����
  * @note   ��
  * @param  Data�����ݻ�����
  * @param  Length�����ݻ���������
  * @retval ��
  */
void UART_HAL_TransmitData(uint8_t *Data, uint16_t Length)
{
	HAL_UART_Transmit(&UART_HAL_STRUCT, (uint8_t *)Data, Length, 10 * Length);
}


/**
  * @brief  UART�����жϻص�����
  * @note   UART���������TIM���ж���
  * @param  ��
  * @retval ��
  */
void UART_HAL_RxCpltCallback(void)
{
	/* ��ȡUART�������ݼĴ�����1Byte������ */
	UART_Transfer_Struct.RxData[UART_Transfer_Struct.RxData_Len] = UART_RXDATA_REGISTER;
	UART_Transfer_Struct.RxData_Len++;
	
	/* �Ƿ�һ֡���ݹ��� */
	if (UART_Transfer_Struct.RxData_Len == UART_DATALEN_MAX - 1)
	{
		UART_Transfer_Struct.RxData_Len = 0;
	}
	
	/* TIM״̬æ */
	TIM_STATUS_Struct.TIM_Status = TIM_STATUS_BUSY;
	
	/* TIM��������0�����¼�ʱ */
	TIM_CNT_REGISTER = 0;
		
	/* ʹ�ܶ�ʱ�� */
	__HAL_TIM_ENABLE(&TIM_HAL_STRUCT);
}


/**
  * @brief  UART��������жϻص�����
  * @note   �ú�����UART_IRQHandler����
  * @param  ��
  * @retval ��
  */
void UART_HAL_TxCpltCallback(void)
{
  /* UART����һ֡��ɱ�־��λ */
  UART_Transfer_Struct.Transfer_Status |= UART_TRANSFER_TXEND;
}


/* �ض���printf���������� */
int fputc(int ch, FILE *f)
{
	HAL_UART_Transmit(&DEBUG_UART_STRUCT, (uint8_t *)&ch, 1, 100);
	
	return ch;
}



