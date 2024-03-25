/*********************************************************************************
 *    Description:
 *    version: V1.0.0
 *    Author:  �Ŵ��� <zhangdalin@dtusystem.com>
 *    LastEditors: �Ŵ��� <zhangdalin@dtusystem.com>
 *    Date: 2023-11-24 16:49:02
 *    LastEditTime: 2023-12-08 09:58:54
*********************************************************************************/

#ifndef _LIB_UART_H_
#define _LIB_UART_H_

#include <stdbool.h>
#include <stdint.h>
#include "main.h"
#include "libfifo.h"
#define UART_TX_BUF_SIZE           64
#define UART_RX_BUF_SIZE           64
#define UART_DMA_RX_BUF_SIZE       64
#define UART_DMA_TX_BUF_SIZE       64

typedef struct
{
    UART_HandleTypeDef* huart;
    DMA_HandleTypeDef* hdma_rx;
    DMA_HandleTypeDef* hdma_tx;
    uint8_t status;     /* ����״̬ */
    _fifo_t tx_fifo;    /* ����fifo */
    _fifo_t rx_fifo;    /* ����fifo */
    uint8_t* dmarx_buf; /* dma���ջ��� */
    uint16_t dmarx_buf_size;/* dma���ջ����С*/
    uint8_t* dmatx_buf; /* dma���ͻ��� */
    uint16_t dmatx_buf_size;/* dma���ͻ����С */
    uint16_t last_dmarx_size;/* dma��һ�ν������ݴ�С */
} UartDeviceImpl;

typedef struct
{
    UART_HandleTypeDef* huart;
    bool isInited;
    uint8_t uart_tx_buf[UART_TX_BUF_SIZE];
    uint8_t uart_rx_buf[UART_RX_BUF_SIZE];
    uint8_t uart_dmatx_buf[UART_TX_BUF_SIZE];
    uint8_t uart_dmarx_buf[UART_RX_BUF_SIZE];
    UartDeviceImpl uartDeviceImpl;
} UartDevice;   /* ���ڶ�UartDevice���г�ʼ�� */

void UartDeviceInit(UART_HandleTypeDef *huart);
uint16_t UartWrite(UART_HandleTypeDef *huart, const uint8_t* buf, uint16_t size);
uint16_t UartRead(UART_HandleTypeDef *huart, uint8_t* buf, uint16_t size);
void UartPollDmaTx(UART_HandleTypeDef *huart);
void USER_UART_IRQHandler(UART_HandleTypeDef *huart);
void uart_dmarx_idle_isr(UartDevice *uartDevice);
void uart_dmatx_done_isr(UartDevice *uartDevice);
void uart_dmarx_half_done_isr(UartDevice *uartDevice);
void uart_dmarx_done_isr(UartDevice *uartDevice);
uint16_t uart_get_dmarx_buf_remain_size(UartDevice *uartDevice);

void UartTest(void);
#endif
