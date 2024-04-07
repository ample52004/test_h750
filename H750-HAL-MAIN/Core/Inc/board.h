#ifndef __BOARD_H__
#define __BOARD_H__

#include "stm32h7xx.h"
#include "main.h"
#include "string.h"
#include <rthw.h>
#include "BSP_Init.h"
#include "ringbuffer.h"
void rt_hw_board_init(void);
void SysTick_Handler(void);
extern struct rt_ringbuffer  uart_rxcb;         /* ???? ringbuffer cb */
extern struct rt_semaphore shell_rx_sem; /* ????????? */
#endif /* __BOARD_H__ */
