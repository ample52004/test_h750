/*********************************************************************************
 *    Description:
 *    version: V1.0.0
 *    Author:  张达麟 <zhangdalin@dtusystem.com>
 *    LastEditors: 张达麟 <zhangdalin@dtusystem.com>
 *    Date: 2023-12-01 16:18:48
 *    LastEditTime: 2023-12-08 10:12:17
*********************************************************************************/
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "BSP_Init.H"
#include "libuart_fifo.h"
#include "libuart.h"
#include "main.h"

#define LIBUART_PRINT_DEBUG_LOG 1
typedef enum
{
    UART_IDLE = 0,
    UART_BUSY = 1
}uart_trans_status;

/* fifo上锁函数 */
static void fifo_lock(void)
{
    __disable_irq(); //关闭所有中断,arm内核通用
}

/* fifo解锁函数 */
static void fifo_unlock(void)
{
    __enable_irq();//开启所有中断
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
    /* 配置串口收发fifo */
    fifo_register(&uartDeviceImpl->tx_fifo, uart_tx_buf,
        uart_tx_buf_size, fifo_lock, fifo_unlock);
    fifo_register(&uartDeviceImpl->rx_fifo, uart_rx_buf,
        uart_rx_buf_size, fifo_lock, fifo_unlock);

    /* 配置串口 DMA收发buf */
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
 * @brief 串口设备初始化
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
 * @brief  串口发送数据接口，实际是写入发送fifo，发送由dma处理
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
 * @brief  串口读取数据接口，实际是从接收fifo读取
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
 * @brief  循环从串口发送fifo读出数据，放置于dma发送缓存，并启动dma传输
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
        uartDeviceImpl->status = UART_BUSY;  /* DMA发送状态 */
        HAL_UART_Transmit_DMA(uartDeviceImpl->huart, uartDeviceImpl->dmatx_buf, size);
    }
}



/**
 * @brief  获取DMA接收buf剩余空间
 * @param
 * @retval
 */
uint16_t uart_get_dmarx_buf_remain_size(UartDevice *uartDevice)
{   
    UartDeviceImpl *uartDeviceImpl = &uartDevice->uartDeviceImpl;
    return __HAL_DMA_GET_COUNTER(uartDeviceImpl->hdma_rx);
}


/**
 * @brief  串口dma接收完成中断处理
 * @param
 * @retval
 */
void uart_dmarx_done_isr(UartDevice *uartDevice)
{   
    UartDeviceImpl *uartDeviceImpl = &uartDevice->uartDeviceImpl;
    uint16_t recv_size; /* 本次中断新接收的长度 */
    recv_size = uartDeviceImpl->dmarx_buf_size - uartDeviceImpl->last_dmarx_size;
    uint32_t size = fifo_write(&uartDeviceImpl->rx_fifo, (const uint8_t*)&(uartDeviceImpl->dmarx_buf[uartDeviceImpl->last_dmarx_size]), recv_size);
    #if LIBUART_PRINT_DEBUG_LOG
    if (size > 0)
        printf("done_isr: write %d to rx_fifo, rx_fifo remain: %d\r\n", size, uartDeviceImpl->rx_fifo.occupy_size);
    #endif
    uartDeviceImpl->last_dmarx_size = 0;
}

/**
 * @brief  串口dma接收缓存大小一半数据中断处理
 * @param
 * @retval
 */
void uart_dmarx_half_done_isr(UartDevice *uartDevice)
{   
    UartDeviceImpl *uartDeviceImpl = &uartDevice->uartDeviceImpl;
    uint16_t recv_total_size; //接收总长
    uint16_t recv_size; /* 本次中断新接收的长度 */
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
 * @brief  串口空闲中断处理
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
 * @brief  串口dma发送完成中断处理
 * @param
 * @retval
 */
void uart_dmatx_done_isr(UartDevice *uartDevice)
{   
    UartDeviceImpl *uartDeviceImpl = &uartDevice->uartDeviceImpl;
    uartDeviceImpl->status = UART_IDLE; /* DMA发送空闲 */
}



/**
 * @brief 串口DMA接收完成的回调函数.
 * @param None
 * @retval None
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    uart_dmarx_done_isr(GetUartDeviceByUartDef(huart));
}
/**
 * @brief 串口DMA接收一半的回调函数.
 * @param None
 * @retval None
 */
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
    uart_dmarx_half_done_isr(GetUartDeviceByUartDef(huart));
}
/**
 * @brief 串口DMA发送完成的回调函数.
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
  if (RESET != __HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE)) // 判断是否是空闲中断
  {
    __HAL_UART_CLEAR_IDLEFLAG(huart); // 清除空闲中断标志（否则会一直不断进入中断）
    temp = huart->Instance->ISR;       // 清除状态寄存器SR
    temp = huart->Instance->RDR;       // 读取数据寄存器
    uart_dmarx_idle_isr(GetUartDeviceByUartDef(huart));
  }
}
