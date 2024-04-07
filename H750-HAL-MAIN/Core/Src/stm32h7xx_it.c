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
#include "rtthread.h"
#include "ringbuffer.h"
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

extern uint8_t TIM2CH1_CAPTURE_STA;
extern uint16_t TIM2CH1_CAPTURE_VAL;
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
  /* USER CODE BEGIN DMA1_Stream1_IRQn 0 */
  rt_interrupt_enter();
  /* USER CODE END DMA1_Stream1_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart2_tx);
  /* USER CODE BEGIN DMA1_Stream1_IRQn 1 */
  rt_interrupt_leave();
  /* USER CODE END DMA1_Stream1_IRQn 1 */
}

/**
  * @brief This function handles TIM1 capture compare interrupt.
  */
void TIM1_CC_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_CC_IRQn 0 */
  rt_interrupt_enter();
  /* USER CODE END TIM1_CC_IRQn 0 */
  HAL_TIM_IRQHandler(&htim1);
  /* USER CODE BEGIN TIM1_CC_IRQn 1 */
  rt_interrupt_leave();
  /* USER CODE END TIM1_CC_IRQn 1 */
}

/**
  * @brief This function handles TIM2 global interrupt.
  */
void TIM2_IRQHandler(void)   
{
	
  // if((TIM2CH1_CAPTURE_STA&0X80)==0)//未捕获                   1000 0000 
  // {
  //     if(TIM_GetITStatus(TIM2,TIM_IT_Update) != RESET) 
  //     {
  //         if(TIM2CH1_CAPTURE_STA&0X40)//已经捕获到高电平            0100 0000
  //         {
  //             if((TIM2CH1_CAPTURE_STA&0X3F)==0X3F)//溢出处理       0011 1111
  //             { 
  //                 TIM2CH1_CAPTURE_STA|=0X80;//标记成功捕获一次      1000 0000 
  //                 TIM2CH1_CAPTURE_VAL=0XFFFF;
  //             }else TIM2CH1_CAPTURE_STA++;
  //         }
  //     }
  //     if(TIM_GetITStatus(TIM2,TIM_IT_CC1) !=RESET)          //捕获到上升沿中断
  //     {
  //         if(TIM2CH1_CAPTURE_STA & 0x40)  //捕获到一个下降沿             0100 0000
  //         {
  //             TIM2CH1_CAPTURE_STA|=0X80;  //标记成功捕获到一次上升沿     1000 0000 
  //             TIM2CH1_CAPTURE_VAL = TIM_GetCounter(TIM2);
  //             TIM_OC1PolarityConfig(TIM2,TIM_ICPolarity_Rising); //CC1P=0 设置上升沿捕获
  //         }
  //         else
  //         {                                     //还没有捕获到新的上升沿
  //             TIM2CH1_CAPTURE_STA=0;            //清空
  //             TIM2CH1_CAPTURE_VAL=0;
                
  //             TIM_SetCounter(TIM2,0);
  //             TIM2CH1_CAPTURE_STA|=0X40;        //标记捕获到了上升沿     0100 0000
  //             TIM_OC1PolarityConfig(TIM2,TIM_ICPolarity_Falling);        //CC1P=1 设置为下降沿捕获
  //         }
  //     }
        
  // }
  // TIM_ClearITPendingBit(TIM2,TIM_IT_CC1|TIM_IT_Update); /*清除中断标志位*/
  HAL_TIM_IRQHandler(&htim2);

}

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */
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
  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */
  rt_interrupt_leave();
  /* USER CODE END USART1_IRQn 1 */
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

/* USER CODE BEGIN 1 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &huart1)
    {
        uint8_t ch = huart1.Instance->RDR & 0xFF; // Read received data
        //rt_kprintf("receive %s", ch); // 如果需要打印接收到的数据，请取消注释这一行

        if (ch != 0xFF) // 这里的判断条件需要根据具体情况修改
        {
            rt_ringbuffer_putchar(&uart_rxcb, ch); // 将接收到的数据放入 ringbuffer
        }

        rt_sem_release(&shell_rx_sem); // 释放信号量
    }
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
 
/* USER CODE END 1 */
