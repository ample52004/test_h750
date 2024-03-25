#ifndef __BOARD_H__
#define __BOARD_H__

#include "stm32h7xx.h"
#include "main.h"
#include "string.h"
#include <rthw.h>
#include "gpio.h"
#include "usart.h"
void rt_hw_board_init(void);
void SysTick_Handler(void);
void USART1_IRQHandler(void);

#endif /* __BOARD_H__ */
