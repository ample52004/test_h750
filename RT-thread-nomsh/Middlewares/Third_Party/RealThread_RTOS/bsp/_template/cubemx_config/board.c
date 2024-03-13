/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-24                  the first version
 */

#include <rthw.h>
#include <rtthread.h>

#include "main.h"
#include "main.h"
#include "dma.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"
#include "dev_uart.h"
#include "bsp_uart.h"
#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
/*
 * Please modify RT_HEAP_SIZE if you enable RT_USING_HEAP
 * the RT_HEAP_SIZE max value = (sram size - ZI size), 1024 means 1024 bytes
 */
#define RT_HEAP_SIZE (15*1024)
static rt_uint8_t rt_heap[RT_HEAP_SIZE];

RT_WEAK void *rt_heap_begin_get(void)
{
    return rt_heap;
}

RT_WEAK void *rt_heap_end_get(void)
{
    return rt_heap + RT_HEAP_SIZE;
}
#endif

void SysTick_Handler(void)
{
    rt_interrupt_enter();
    
    rt_tick_increase();

    rt_interrupt_leave();
}

/**
 * This function will initial your board.
 */
void rt_hw_board_init(void)
{
    extern void SystemClock_Config(void);
    LL_APB4_GRP1_EnableClock(LL_APB4_GRP1_PERIPH_SYSCFG);

		/* System interrupt init*/
		NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

		/* USER CODE BEGIN Init */

		/* USER CODE END Init */

		/* Configure the system clock */
		SystemClock_Config();

	  
		MX_GPIO_Init();
		//usart1_dma_init();
		uart_device_init(DEV_UART1);

//		MX_DMA_Init();
//		MX_USART1_UART_Init();
		MX_SPI5_Init();


    /* Call components board initial (use INIT_BOARD_EXPORT()) */
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif

#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
    rt_system_heap_init(rt_heap_begin_get(), rt_heap_end_get());
#endif
}

#ifdef RT_USING_CONSOLE

static int uart_init(void)
{

}
INIT_BOARD_EXPORT(uart_init);

void rt_hw_console_output(const char *str)
{
	  rt_size_t i = 0, size = 0;
    char a = '\r';
    // ??USART?????
    //LL_USART_EnableIT_TXE(USART1); // ??????????,????????????
    size = rt_strlen(str);
			//uart_write(DEV_UART1, str, size);
    for (i = 0; i < size; i++)
    {
        while(!LL_USART_IsActiveFlag_TXE(USART1))
        {
        }
        if (*(str + i) == '\n')
        {
            LL_USART_TransmitData8(USART1, a);
            while(!LL_USART_IsActiveFlag_TXE(USART1))
            {
            }
        }
        LL_USART_TransmitData8(USART1, *(str + i));
    }
}
#endif

#ifdef RT_USING_FINSH
char rt_hw_console_getchar(void)
{
    /* Note: the initial value of ch must < 0 */
    int ch = -1;

    if (__HAL_UART_GET_FLAG(&UartHandle, UART_FLAG_RXNE) != RESET)
    {
        ch = UartHandle.Instance->DR & 0xff;
    }
    else
    {
        rt_thread_mdelay(10);
    }
    return ch;
}
#endif
