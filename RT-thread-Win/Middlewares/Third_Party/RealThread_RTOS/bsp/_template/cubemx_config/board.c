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
#include "fmc.h"
#include "dev_uart.h"
#include "bsp_uart.h"
#include "nandflash.h"
#include "dev_i2c_s.h"
#include "dev_sdRam.h"
#include "dev_spiflash.h"
#include "i2c.h"
#include "spi.h"
struct rt_ringbuffer  uart_rxcb;         /* ???? ringbuffer cb */
static struct rt_semaphore shell_rx_sem; /* ????????? */
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
	
    /* Enable I-Cache---------------------------------------------------------*/
    SCB_EnableICache();

    /* Enable D-Cache---------------------------------------------------------*/
    SCB_EnableDCache();
		HAL_Init();
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
		uart_device_init(DEV_UART1);
//		MX_DMA_Init();
//		MX_USART1_UART_Init();
		MX_FMC_Init();
    rt_ringbuffer_init(&uart_rxcb, uart_rx_buf, UART_RX_BUF_LEN);
	/* ????????????? */
    rt_sem_init(&(shell_rx_sem), "shell_rx", 0, 0);
    MX_UART4_Init();
    MX_SPI4_Init();
    //MX_I2C1_Init();
    MX_SPI5_Init();
		bsp_analog_i2c_init();
		
    /* Call components board initial (use INIT_BOARD_EXPORT()) */
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif

#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
    rt_system_heap_init(rt_heap_begin_get(), rt_heap_end_get());
#endif
}

#ifdef RT_USING_CONSOLE
// ???UART
static int uart_init(void)
{
	
}
INIT_BOARD_EXPORT(uart_init);

// ?????????
void rt_hw_console_output(const char *str)
{
    size_t i = 0, size = 0;
    char a = '\r';
    // ??USART?????
    //LL_USART_EnableIT_TXE(USART1); // ??????????,????????????
    size = rt_strlen(str);
    /*******************
     * ????
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
     * ????
     * ????????????
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
 * SHELL????
***********************/ 
char rt_hw_console_getchar(void)
{
    char ch = 0;
    /* ? ringbuffer ????? */
    while (rt_ringbuffer_getchar(&uart_rxcb, (rt_uint8_t *)&ch) != 1)
    {
        rt_sem_take(&shell_rx_sem, RT_WAITING_FOREVER);
    } 
    return ch;  
}

#endif
/***********************
 * ??????
***********************/
void USART1_IRQHandler(void)                	//??1??????
{ 
    int ch = -1;
    /* enter interrupt */
    rt_interrupt_enter();          //?????????????,????
    if (LL_USART_IsActiveFlag_RXNE(USART1) && LL_USART_IsEnabledIT_RXNE(USART1))
    {
        while (LL_USART_IsActiveFlag_RXNE(USART1))
        {
            ch = LL_USART_ReceiveData8(USART1) & 0xFF; // Read received data
						//rt_kprintf("receiive %s",ch);
            if (ch == -1)
            { 
                break;
            }              /* ?????,????? ringbuffer */
            rt_ringbuffer_putchar(&uart_rxcb, ch);
        }        
        rt_sem_release(&shell_rx_sem);
    }
    /* leave interrupt */
//    rt_interrupt_leave();    //?????????????,????
//		rt_interrupt_enter(); 
		if (LL_USART_IsEnabledIT_IDLE(USART1) && LL_USART_IsActiveFlag_IDLE(USART1)) 
    {
        uart_dmarx_idle_isr(DEV_UART1);
        LL_USART_ClearFlag_IDLE(USART1);   
    }
		rt_interrupt_leave(); 
}

