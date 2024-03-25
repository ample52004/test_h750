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
#include "board.h"
#include "stm32h750xx.h"
#include "gpio.h"
#include "usart.h"
#include <stdint.h>
#include "ringbuffer.h"
#include "dma.h"
struct rt_ringbuffer  uart_rxcb;         /* 定义一个 ringbuffer cb */
static struct rt_semaphore shell_rx_sem; /* 定义一个静态信号量 */
#define UART_RX_BUF_LEN 16
rt_uint8_t uart_rx_buf[UART_RX_BUF_LEN] = {0};
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
    //HAL_Init();
    SystemClock_Config();
    SystemCoreClockUpdate();
    /* 
     * 1: OS Tick Configuration
     * Enable the hardware timer and call the rt_os_tick_callback function
     * periodically with the frequency RT_TICK_PER_SECOND. 
     */
    SysTick_Config(LL_RCC_GetFMCClockFreq(LL_RCC_FMC_CLKSOURCE)/RT_TICK_PER_SECOND);
	  MX_GPIO_Init();
		MX_DMA_Init();
		MX_USART1_UART_Init();
		MX_USART2_UART_Init();
    rt_ringbuffer_init(&uart_rxcb, uart_rx_buf, UART_RX_BUF_LEN);
	/* 初始化串口接收数据的信号量 */
    rt_sem_init(&(shell_rx_sem), "shell_rx", 0, 0);
    MX_USART2_UART_Init();
    /* Call components board initial (use INIT_BOARD_EXPORT()) */
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif

#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
    rt_system_heap_init(rt_heap_begin_get(), rt_heap_end_get());
#endif
}

#ifdef RT_USING_CONSOLE
// 初始化UART
static int uart_init(void)
{
    MX_USART1_UART_Init();
	
}
INIT_BOARD_EXPORT(uart_init);

// 输出字符串到控制台
void rt_hw_console_output(const char *str)
{
    size_t i = 0, size = 0;
    char a = '\r';
    // 确保USART外设已解锁
    //LL_USART_EnableIT_TXE(USART1); // 如果需要使用中断发送，需要使能发送缓冲区空中断
    size = rt_strlen(str);
    /*******************
     * 轮询收发
    *********************/
//     for (i = 0; i < size; i++)
//     {
//         if (*(str + i) == '\n')
//         {
//             while(1){
//                 if (LL_USART_IsActiveFlag_TXE(USART1))
//                 {
//                     LL_USART_TransmitData8(USART1, (uint8_t)a);
//                     break;
//                 }
//             }
//         }
//         while(1){
//             if (LL_USART_IsActiveFlag_TXE(USART1))
//             {
//                 LL_USART_TransmitData8(USART1, *(str + i));
//                 break;
//             }
//         }
// }
    /*******************
     * 轮询收发
     * 未经过测试原理上应该可以
    *********************/

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
// 从控制台获取字符
// char rt_hw_console_getchar(void)
// {
//     int ch = -1;
//     if(LL_USART_IsActiveFlag_RXNE(USART1))
//     {
//         ch = LL_USART_ReceiveData8(USART1);
//     }
//     else
//     {
//         rt_thread_mdelay(10);
//     }
//     return ch;
// }

/***********************
 * SHELL接收中断
***********************/ 
char rt_hw_console_getchar(void)
{
    
    char ch = 0;
    /* 从 ringbuffer 中拿出数据 */
    while (rt_ringbuffer_getchar(&uart_rxcb, (rt_uint8_t *)&ch) != 1)
    {
        rt_sem_take(&shell_rx_sem, RT_WAITING_FOREVER);
    } 
    return ch;  
}

#endif
/***********************
 * 中断服务程序
***********************/
void USART1_IRQHandler(void)                	//串口1中断服务程序
{ 
    int ch = -1;
    /* enter interrupt */
    rt_interrupt_enter();          //在中断中一定要调用这对函数，进入中断
    if (LL_USART_IsActiveFlag_RXNE(USART1) && LL_USART_IsEnabledIT_RXNE(USART1))
    {
        while (LL_USART_IsActiveFlag_RXNE(USART1))
        {
            ch = LL_USART_ReceiveData8(USART1) & 0xFF; // Read received data
            if (ch == -1)
            { 
                break;
            }              /* 读取到数据，将数据存入 ringbuffer */
            rt_ringbuffer_putchar(&uart_rxcb, ch);
        }        
        rt_sem_release(&shell_rx_sem);
    }
    /* leave interrupt */
    rt_interrupt_leave();    //在中断中一定要调用这对函数，离开中断
}
