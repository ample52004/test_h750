/*********************************************************************************
 *    Description:
 *    version: V1.0.0
 *    Author:  �Ŵ��� <zhangdalin@dtusystem.com>
 *    LastEditors: �Ŵ��� <zhangdalin@dtusystem.com>
 *    Date: 2023-12-01 16:18:48
 *    LastEditTime: 2023-12-08 10:12:17
*********************************************************************************/
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "usart.h"
#include "libfifo.h"
#include "libuart.h"
#include "main.h"

#define LIBUART_PRINT_DEBUG_LOG 1
typedef enum
{
    UART_IDLE = 0,
    UART_BUSY = 1
}uart_trans_status;

/* fifo�������� */
static void fifo_lock(void)
{
    __disable_irq(); //�ر������ж�,arm�ں�ͨ��
}

/* fifo�������� */
static void fifo_unlock(void)
{
    __enable_irq();//���������ж�
}

UartDevice uart1_device;
UartDevice uart3_device;

UartDevice *GetUartDeviceByUartDef(UART_HandleTypeDef *huart){
    if(huart->Instance == USART1){
        return &uart1_device;
    }
    else if(huart->Instance == USART3){
        return &uart3_device;
    }
    else{
        return NULL;
    }
}


void UartDeviceImplInit(UartDeviceImpl *uartDeviceImpl, UART_HandleTypeDef *huart,
                    uint8_t *uart_tx_buf, uint16_t uart_tx_buf_size,
                    uint8_t *uart_rx_buf, uint16_t uart_rx_buf_size,
                    uint8_t *dmatx_buf, uint16_t dmatx_buf_size,
                    uint8_t *dmarx_buf, uint16_t dmarx_buf_size)
{
    /* ���ô����շ�fifo */
    fifo_register(&uartDeviceImpl->tx_fifo, uart_tx_buf,
        uart_tx_buf_size, fifo_lock, fifo_unlock);
    fifo_register(&uartDeviceImpl->rx_fifo, uart_rx_buf,
        uart_rx_buf_size, fifo_lock, fifo_unlock);

    /* ���ô��� DMA�շ�buf */
    uartDeviceImpl->huart = huart;
    uartDeviceImpl->hdma_rx = huart->hdmarx;
    uartDeviceImpl->hdma_tx = huart->hdmatx;
    uartDeviceImpl->dmarx_buf = dmarx_buf;
    uartDeviceImpl->dmarx_buf_size = dmarx_buf_size;
    uartDeviceImpl->dmatx_buf = dmatx_buf;
    uartDeviceImpl->dmatx_buf_size = dmatx_buf_size;
    __HAL_UART_ENABLE_IT(huart, UART_IT_RXNE);
    __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);
    HAL_UART_Receive_DMA(uartDeviceImpl->huart, uartDeviceImpl->dmarx_buf, uart_rx_buf_size);
    uartDeviceImpl->status = UART_IDLE;

}

/**
 * @brief �����豸��ʼ��
 * @param
 * @retval
 */
void UartDeviceInit(UART_HandleTypeDef *huart)
{   
    UartDevice *uartDevice = GetUartDeviceByUartDef(huart);
    assert_param(uartDevice != NULL);
    uartDevice->huart = huart;
    UartDeviceImplInit(&uartDevice->uartDeviceImpl, uartDevice->huart,
                       uartDevice->uart_tx_buf, sizeof(uartDevice->uart_tx_buf),
                       uartDevice->uart_rx_buf, sizeof(uartDevice->uart_rx_buf),
                       uartDevice->uart_dmatx_buf, sizeof(uartDevice->uart_dmatx_buf),
                       uartDevice->uart_dmarx_buf, sizeof(uartDevice->uart_dmarx_buf));
    uartDevice->isInited = true;
}

/**
 * @brief  ���ڷ������ݽӿڣ�ʵ����д�뷢��fifo��������dma����
 * @param
 * @retval
 */
uint16_t UartWrite(UART_HandleTypeDef *huart, const uint8_t* buf, uint16_t size)
{
    UartDevice *uartDevice = GetUartDeviceByUartDef(huart);
    assert_param(uartDevice != NULL);
    UartDeviceImpl *uartDeviceImpl = &uartDevice->uartDeviceImpl;
    size = fifo_write(&uartDeviceImpl->tx_fifo, buf, size);
    #if LIBUART_PRINT_DEBUG_LOG
    if (size > 0)
        printf("write %d to tx_fifo, tx_fifo remain: %d\r\n", size, uartDeviceImpl->tx_fifo.occupy_size);
    #endif
    return size;
}

/**
 * @brief  ���ڶ�ȡ���ݽӿڣ�ʵ���Ǵӽ���fifo��ȡ
 * @param
 * @retval
 */
uint16_t UartRead(UART_HandleTypeDef *huart, uint8_t* buf, uint16_t size)
{
    UartDevice *uartDevice = GetUartDeviceByUartDef(huart);
    assert_param(uartDevice != NULL);
    UartDeviceImpl *uartDeviceImpl = &uartDevice->uartDeviceImpl;
    size = fifo_read(&uartDeviceImpl->rx_fifo, buf, size);
    #if LIBUART_PRINT_DEBUG_LOG
    if (size > 0)
        printf("read %d from rx_fifo, rx_fifo remain: %d\r\n", size, uartDeviceImpl->rx_fifo.occupy_size);
    #endif
    return size;
}

/**
 * @brief  ѭ���Ӵ��ڷ���fifo�������ݣ�������dma���ͻ��棬������dma����
 * @param
 * @retval
 */
void UartPollDmaTx(UART_HandleTypeDef *huart)
{
    UartDevice *uartDevice = GetUartDeviceByUartDef(huart);
    assert_param(uartDevice != NULL);
    UartDeviceImpl *uartDeviceImpl = &uartDevice->uartDeviceImpl;
    uint16_t size = 0;
    if (UART_BUSY == uartDeviceImpl->status) {
        return;
    }
    size = fifo_read(&uartDeviceImpl->tx_fifo, uartDeviceImpl->dmatx_buf, uartDeviceImpl->dmatx_buf_size);
    #if LIBUART_PRINT_DEBUG_LOG
    if (size > 0)
        printf("transmit size: %d\r\n", size);
    #endif
    if (size != 0) {
        uartDeviceImpl->status = UART_BUSY;  /* DMA����״̬ */
        HAL_UART_Transmit_DMA(uartDeviceImpl->huart, uartDeviceImpl->dmatx_buf, size);
    }
}



/**
 * @brief  ��ȡDMA����bufʣ��ռ�
 * @param
 * @retval
 */
uint16_t uart_get_dmarx_buf_remain_size(UartDevice *uartDevice)
{   
    UartDeviceImpl *uartDeviceImpl = &uartDevice->uartDeviceImpl;
    return __HAL_DMA_GET_COUNTER(uartDeviceImpl->hdma_rx);
}


/**
 * @brief  ����dma��������жϴ���
 * @param
 * @retval
 */
void uart_dmarx_done_isr(UartDevice *uartDevice)
{   
    UartDeviceImpl *uartDeviceImpl = &uartDevice->uartDeviceImpl;
    uint16_t recv_size; /* �����ж��½��յĳ��� */
    recv_size = uartDeviceImpl->dmarx_buf_size - uartDeviceImpl->last_dmarx_size;
    uint32_t size = fifo_write(&uartDeviceImpl->rx_fifo, (const uint8_t*)&(uartDeviceImpl->dmarx_buf[uartDeviceImpl->last_dmarx_size]), recv_size);
    #if LIBUART_PRINT_DEBUG_LOG
    if (size > 0)
        printf("done_isr: write %d to rx_fifo, rx_fifo remain: %d\r\n", size, uartDeviceImpl->rx_fifo.occupy_size);
    #endif
    uartDeviceImpl->last_dmarx_size = 0;
}

/**
 * @brief  ����dma���ջ����Сһ�������жϴ���
 * @param
 * @retval
 */
void uart_dmarx_half_done_isr(UartDevice *uartDevice)
{   
    UartDeviceImpl *uartDeviceImpl = &uartDevice->uartDeviceImpl;
    uint16_t recv_total_size; //�����ܳ�
    uint16_t recv_size; /* �����ж��½��յĳ��� */
    recv_total_size = uartDeviceImpl->dmarx_buf_size - __HAL_DMA_GET_COUNTER(uartDeviceImpl->hdma_rx);
    recv_size = recv_total_size - uartDeviceImpl->last_dmarx_size;
    uint32_t size = fifo_write(&uartDeviceImpl->rx_fifo, (const uint8_t*)&(uartDeviceImpl->dmarx_buf[uartDeviceImpl->last_dmarx_size]), recv_size);
    #if LIBUART_PRINT_DEBUG_LOG
    if (size > 0)
        printf("half_done_isr: write %d to rx_fifo, rx_fifo remain: %d dma counter: %d\r\n", size, uartDeviceImpl->rx_fifo.occupy_size,  __HAL_DMA_GET_COUNTER(uartDeviceImpl->hdma_rx));
    #endif
    uartDeviceImpl->last_dmarx_size = recv_total_size;
}


/**
 * @brief  ���ڿ����жϴ���
 * @param
 * @retval
 */
void uart_dmarx_idle_isr(UartDevice *uartDevice)
{   
    UartDeviceImpl *uartDeviceImpl = &uartDevice->uartDeviceImpl;
    uint16_t recv_total_size;
    uint16_t recv_size;
    recv_total_size = uartDeviceImpl->dmarx_buf_size - __HAL_DMA_GET_COUNTER(uartDeviceImpl->hdma_rx);
    recv_size = recv_total_size - uartDeviceImpl->last_dmarx_size;
    uint32_t size = fifo_write(&uartDeviceImpl->rx_fifo, (const uint8_t*)&(uartDeviceImpl->dmarx_buf[uartDeviceImpl->last_dmarx_size]), recv_size);
    #if LIBUART_PRINT_DEBUG_LOG
    if (size > 0)
        printf("idle_isr: write %d to rx_fifo, rx_fifo remain: %d dma counter: %d\r\n", size, uartDeviceImpl->rx_fifo.occupy_size, __HAL_DMA_GET_COUNTER(uartDeviceImpl->hdma_rx));
    #endif
    uartDeviceImpl->last_dmarx_size = recv_total_size;
}

/**
 * @brief  ����dma��������жϴ���
 * @param
 * @retval
 */
void uart_dmatx_done_isr(UartDevice *uartDevice)
{   
    UartDeviceImpl *uartDeviceImpl = &uartDevice->uartDeviceImpl;
    uartDeviceImpl->status = UART_IDLE; /* DMA���Ϳ��� */
}



/**
 * @brief ����DMA����һ��Ļص�����.
 * @param None
 * @retval None
 */
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
    uart_dmarx_half_done_isr(GetUartDeviceByUartDef(huart));
}
/**
 * @brief ����DMA������ɵĻص�����.
 * @param None
 * @retval None
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    uart_dmatx_done_isr(GetUartDeviceByUartDef(huart));
}

void USER_UART_IRQHandler(UART_HandleTypeDef *huart)
{
  uint32_t temp;
  if (RESET != __HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE)) // �ж��Ƿ��ǿ����ж�
  {
    __HAL_UART_CLEAR_IDLEFLAG(huart); // ��������жϱ�־�������һֱ���Ͻ����жϣ�
    temp = huart->Instance->ISR;       // ���״̬�Ĵ���SR
    temp = huart->Instance->RDR;       // ��ȡ���ݼĴ���
    uart_dmarx_idle_isr(GetUartDeviceByUartDef(huart));
  }
}

