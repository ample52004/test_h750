/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"
#include "fmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <rtthread.h>
#include "stm32h750xx.h"
#include "dev_uart.h"
#include "bsp_uart.h"
#include "nandflash.h"
#include "dev_i2c_s.h"
#include "dev_sdRam.h"
#include "dev_spiflash.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/*SDRAM大小，64M字节*/
#define W9825G6KH_SIZE 0x4000000
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static rt_thread_t PA9_thread_on = RT_NULL;
static rt_thread_t thread_test_dma = RT_NULL;
static void PA9_thread_on_entry(void *parameter);
static void Rtthread_test_dma(void *parameter);
static rt_uint16_t count=0;
unsigned char Rx_Buf[USART_BUF_LEN] = {0};
SUASRT usart4 = {0};  
unsigned char flag = 0;
//static uint32_t s_count = 0;
static	uint16_t size = 4;
static	uint8_t buf[4]={65,66,67,68};
static	uint8_t buf_read[4]={0};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
	//NAND_check();
//	rt_kprintf("TEST DMA \n");
//	SendData(buf,4);
	at24cxx_write_byte(0,buf,size);
	at24cxx_read_byte(0,buf_read,size);
	rt_kprintf("eeprom test = %s\n",buf_read);
	rt_kprintf("SDRam_Test\n");

	
	uint32_t temp;

	/*向SDRAM写入8位数据*/
	*( uint8_t*) (SDRAM_BANK_ADDR ) = (uint8_t)0xAA;
	/*从SDRAM读取数据*/
	temp =  *( uint8_t*) (SDRAM_BANK_ADDR );
	rt_kprintf("temp = %d\n",temp);
	/*写/读 16位数据*/
	*( uint16_t*) (SDRAM_BANK_ADDR+10 ) = (uint16_t)0xBBBB;
	temp =  *( uint16_t*) (SDRAM_BANK_ADDR+10 );
	rt_kprintf("temp = %d\n",temp);
	/*写/读 32位数据*/
	*( uint32_t*) (SDRAM_BANK_ADDR+20 ) = (uint32_t)0xCCCCCCCC;
	temp =  *( uint32_t*) (SDRAM_BANK_ADDR+20 );
	rt_kprintf("temp = %d\n",temp);
	
  rt_kprintf("Start\n");
  PA9_thread_on =
  rt_thread_create("PA9_thread_on",              /* 线程名字 */
                    PA9_thread_on_entry,   /* 线程入口函数 */
                    RT_NULL,             /* 线程入口函数参数 */
                    1024,                 /* 线程栈大?? */
                    6,                   /* 线程的优先级 */
                    10);
  if ( PA9_thread_on != RT_NULL){
      //rt_thread_startup(PA9_thread_on);
    }
    else{
    LL_USART_TransmitData8(USART1, 'e');
    return -1;
  }
  thread_test_dma =
  rt_thread_create("thread_test_dma",              /* 线程名字 */
                   Rtthread_test_dma,   /* 线程入口函数 */
                    RT_NULL,             /* 线程入口函数参数 */
                    1024,                 /* 线程栈大?? */
                    7,                   /* 线程的优先级 */
                    10);
  if ( thread_test_dma != RT_NULL){
      rt_thread_startup(thread_test_dma);
    }
    else{
    LL_USART_TransmitData8(USART1, 'e');
    return -1;
}
		
}
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_2)
  {
  }
  LL_PWR_ConfigSupply(LL_PWR_LDO_SUPPLY);
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  while (LL_PWR_IsActiveFlag_VOS() == 0)
  {
  }
  LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1)
  {

  }
  LL_RCC_PLL_SetSource(LL_RCC_PLLSOURCE_HSE);
  LL_RCC_PLL1P_Enable();
  LL_RCC_PLL1R_Enable();
  LL_RCC_PLL1_SetVCOInputRange(LL_RCC_PLLINPUTRANGE_4_8);
  LL_RCC_PLL1_SetVCOOutputRange(LL_RCC_PLLVCORANGE_WIDE);
  LL_RCC_PLL1_SetM(5);
  LL_RCC_PLL1_SetN(160);
  LL_RCC_PLL1_SetP(2);
  LL_RCC_PLL1_SetQ(2);
  LL_RCC_PLL1_SetR(2);
  LL_RCC_PLL1_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL1_IsReady() != 1)
  {
  }

   /* Intermediate AHB prescaler 2 when target frequency clock is higher than 80 MHz */
   LL_RCC_SetAHBPrescaler(LL_RCC_AHB_DIV_2);

  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL1);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL1)
  {

  }
  LL_RCC_SetSysPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAHBPrescaler(LL_RCC_AHB_DIV_2);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_2);
  LL_RCC_SetAPB3Prescaler(LL_RCC_APB3_DIV_2);
  LL_RCC_SetAPB4Prescaler(LL_RCC_APB4_DIV_2);
  LL_SetSystemCoreClock(400000000);

   /* Update the time base */
  if (HAL_InitTick (TICK_INT_PRIORITY) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
static void SendData(unsigned char *buf, unsigned short len)
{
    // DMA 发送初始化，DMA1通道1
    LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_3);
	LL_DMA_SetPeriphRequest(DMA1, LL_DMA_STREAM_3, LL_DMAMUX1_REQ_USART3_TX); // 串口3
	LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_STREAM_3, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
	LL_DMA_SetStreamPriorityLevel(DMA1, LL_DMA_STREAM_3, LL_DMA_PRIORITY_LOW);
	LL_DMA_SetMode(DMA1, LL_DMA_STREAM_3, LL_DMA_MODE_NORMAL);
	LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_STREAM_3, LL_DMA_PERIPH_NOINCREMENT);
	LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_STREAM_3, LL_DMA_MEMORY_INCREMENT);
	LL_DMA_SetPeriphSize(DMA1, LL_DMA_STREAM_3, LL_DMA_PDATAALIGN_BYTE);
	LL_DMA_SetMemorySize(DMA1, LL_DMA_STREAM_3, LL_DMA_MDATAALIGN_BYTE);
	LL_DMA_DisableFifoMode(DMA1, LL_DMA_STREAM_3);
	LL_DMA_SetPeriphAddress(DMA1, LL_DMA_STREAM_3, LL_USART_DMA_GetRegAddr(UART4, LL_USART_DMA_REG_DATA_TRANSMIT));

	//配置内存地址
	usart4.txlen = len;
	rt_memcpy(usart4.txbuf, buf, usart4.txlen);
	LL_DMA_SetMemoryAddress(DMA1, LL_DMA_STREAM_1,(uint32_t)usart4.txbuf);
	LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_1, usart4.txlen);

    // 清除中断标志位
	LL_DMA_ClearFlag_TC1(DMA1);
	LL_DMA_ClearFlag_HT1(DMA1);
	LL_DMA_ClearFlag_TE1(DMA1);
	LL_DMA_ClearFlag_DME1(DMA1);
	LL_DMA_ClearFlag_FE1(DMA1);

	// 使能传输完成中断
	LL_DMA_EnableIT_TC(DMA1, LL_DMA_STREAM_1);
	LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_1);

}
static void PA9_thread_on_entry(void* parameter)
{	
    while (1)
    {

      count =count+1;
      rt_kprintf("LED ON\n");
      //rt_thread_mdelay(30);
      rt_kprintf("count = %d\n",count);
      if (count == 20)
      {
        count =0;
        break;
      }
}
}
static void Rtthread_test_dma(void *param)
{
; 
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
