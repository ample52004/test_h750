/*********************************************************************************
 *    Description:
 *    version: V1.0.0
 *    Author:  张达麟 <zhangdalin@dtusystem.com>
 *    LastEditors: 张达麟 <zhangdalin@dtusystem.com>
 *    Date: 2023-11-24 16:49:02
 *    LastEditTime: 2023-12-08 09:58:54
*********************************************************************************/

#ifndef _LIB_UART_H_
#define _LIB_UART_H_

#include <stdbool.h>
#include <stdint.h>
#include "main.h"
#include "libuart_fifo.h"
#define UART_TX_BUF_SIZE           64
#define UART_RX_BUF_SIZE           64
#define UART_DMA_RX_BUF_SIZE       64
#define UART_DMA_TX_BUF_SIZE       64

typedef struct
{
    UART_HandleTypeDef* huart;
    DMA_HandleTypeDef* hdma_rx;
    DMA_HandleTypeDef* hdma_tx;
    uint8_t status;     /* 发送状态 */
    _fifo_t tx_fifo;    /* 发送fifo */
    _fifo_t rx_fifo;    /* 接收fifo */
    uint8_t* dmarx_buf; /* dma接收缓存 */
    uint16_t dmarx_buf_size;/* dma接收缓存大小*/
    uint8_t* dmatx_buf; /* dma发送缓存 */
    uint16_t dmatx_buf_size;/* dma发送缓存大小 */
    uint16_t last_dmarx_size;/* dma上一次接收数据大小 */
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
} UartDevice;   /* 便于对UartDevice进行初始化 */

void UartDeviceInit(UART_HandleTypeDef *huart);
uint16_t UartWrite(UART_HandleTypeDef *huart, const uint8_t* buf, uint16_t size);
uint16_t UartRead(UART_HandleTypeDef *huart, uint8_t* buf, uint16_t size);
void UartPollDmaTx(UART_HandleTypeDef *huart);
void USER_UART_IRQHandler(UART_HandleTypeDef *huart);
void UartTest(void);
#endif
