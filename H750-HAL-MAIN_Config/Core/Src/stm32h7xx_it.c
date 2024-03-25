/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32h7xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32h7xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "rtthread.h"
#include "ringbuffer.h"
/* USER CODE END Includes */


/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern DMA_HandleTypeDef hdma_uart4_rx;
extern DMA_HandleTypeDef hdma_uart4_tx;
extern DMA_HandleTypeDef hdma_usart2_rx;
extern DMA_HandleTypeDef hdma_usart2_tx;
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
/* USER CODE BEGIN EV */
extern SUASRT uart4;
extern SUASRT usart2;
extern struct rt_ringbuffer  uart_rxcb; 
extern struct rt_semaphore shell_rx_sem;

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
   while (1)
  {
  }

}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{

  while (1)
  {

  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{

  while (1)
  {

  }
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{

}

/******************************************************************************/
/* STM32H7xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32h7xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles DMA1 stream0 global interrupt.
  */
void DMA1_Stream0_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_usart2_rx);
}

/**
  * @brief This function handles DMA1 stream1 global interrupt.
  */
void DMA1_Stream1_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_usart2_tx);
}

/**
  * @brief This function handles TIM1 capture compare interrupt.
  */
void TIM1_CC_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim1);
}

/**
  * @brief This function handles TIM2 global interrupt.
  */
void TIM2_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim2);
}

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
  rt_interrupt_enter();
	//rt_kprintf("MSH ReC");
//	uint8_t  ch = -1;
//	if (HAL_UART_Receive_IT(&huart1, &ch, 1) == HAL_OK)
//	{
//        uint8_t ch = huart1.Instance->RDR & 0xFF; // Read received data
//        // rt_kprintf("receiive %s", ch); // 如果需要打印接收到的数据，请取消注释这一行

//        if (ch != 0xFF) // 这里的判断条件需要根据具体情况修改
//        {
//            rt_ringbuffer_putchar(&uart_rxcb, ch); // 将接收到的数据放入 ringbuffer
//						
//        }

//        rt_sem_release(&shell_rx_sem); // 释放信号量
//	}
  HAL_UART_IRQHandler(&huart1);
  rt_interrupt_leave();
}

/**
  * @brief This function handles USART2 global interrupt.
  */
void USART2_IRQHandler(void)
{
  static uint16_t i = 0;
	if((__HAL_UART_GET_FLAG(&huart2,UART_FLAG_IDLE) != RESET))
	{ 
    HAL_UART_DMAStop(&huart2); 
		__HAL_UART_CLEAR_IDLEFLAG(&huart2);	     
		usart2.rxlen =  USART_BUF_LEN - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
		recv_flag_usart2 = 1;
	}
  HAL_UART_IRQHandler(&huart2);

}

/**
  * @brief This function handles UART4 global interrupt.
  */
void UART4_IRQHandler(void)
{
	static uint16_t i = 0;
	if((__HAL_UART_GET_FLAG(&huart4,UART_FLAG_IDLE) != RESET))
	{ 
  HAL_UART_DMAStop(&huart4); 
	__HAL_UART_CLEAR_IDLEFLAG(&huart4);	     
	uart4.rxlen =  USART_BUF_LEN - __HAL_DMA_GET_COUNTER(&hdma_uart4_rx);
	recv_flag_uart4 = 1;  // 如果接收到数据，设置完成标志 
	}
  HAL_UART_IRQHandler(&huart4);
}

/**
  * @brief This function handles DMA2 stream0 global interrupt.
  */
void DMA2_Stream0_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_uart4_rx);
}

/**
  * @brief This function handles DMA2 stream1 global interrupt.
  */
void DMA2_Stream1_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_uart4_tx);
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
		// rt_interrupt_enter();
    // /* HAL_UART_RECEPTION_TOIDLE 说明是DMA接收完成，或者半传输完成 */
    // if (huart->ReceptionType == HAL_UART_RECEPTION_TOIDLE)
    // {
    //     HAL_UARTEx_ReceiveToIdle_DMA(huart, uart4.rxbuf, USART_BUF_LEN);
    // }
    // /* HAL_UART_RECEPTION_STANDARD 缓冲区未满空闲中断回调函数处理 */
    // if (huart->ReceptionType == HAL_UART_RECEPTION_STANDARD)
    // {
    //     recv_flag_uart4 = 1;
    //     uart4.rxlen = Size;
    //     HAL_UARTEx_ReceiveToIdle_DMA(huart, uart4.rxbuf, USART_BUF_LEN);
    // }
		// rt_interrupt_leave();
}
 
