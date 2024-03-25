
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HAL_USART_H__
#define __HAL_USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "usart.h"


#define USART_USE_TIM_REVEICE

typedef struct
{
#define UART_DATALEN_MAX		   1024		  	// UART������󳤶�
	uint8_t  TxData[UART_DATALEN_MAX];			// UART���ͻ�����
	uint16_t TxData_Len;										// UART���ͻ���ʵ�ʳ���
	uint8_t  RxData[UART_DATALEN_MAX];			// UART���ջ�����
	uint16_t RxData_Len;										// UART���ջ���ʵ�ʳ���
	uint8_t  Transfer_Status;								// UART����״̬
#define UART_TRANSFER_IDLE		 0x00				// UART״̬����
#define UART_TRANSFER_TXEND		 0x01				// UART����һ֡���
#define UART_TRANSFER_RXEND		 0x02				// UART����һ֡���
} UART_Transfer_TypeDef;


/* ����ʵ��ʹ�ô���ѡ�� */
#define DEBUG_UART_STRUCT 		huart4

#define UART_HAL_STRUCT				huart4

#define UART_PERIPHERAL				UART4									// UART

#define UART_TXDATA_REGISTER	UART_PERIPHERAL->TDR			// UART�������ݼĴ���
#define UART_RXDATA_REGISTER	UART_PERIPHERAL->RDR			// UART�������ݼĴ���
#define UART_IRQN 						UART_PERIPHERAL_IRQn		// UART_IRQn

void UART_HAL_Init(uint32_t BaudRate);
void UART_HAL_TransmitData(uint8_t *Data, uint16_t Length);
void UART_HAL_RxCpltCallback(void);
void UART_HAL_TxCpltCallback(void);

extern UART_Transfer_TypeDef  UART_Transfer_Struct;

#ifdef __cplusplus
}
#endif

#endif

