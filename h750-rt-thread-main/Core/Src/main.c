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
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <rtthread.h>
#include "stm32h750xx.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static rt_thread_t PA9_thread_on = RT_NULL;
static rt_thread_t PA9_thread_off = RT_NULL;
static void PA9_thread_on_entry(void *parameter);
static void PA9_thread_off_entry(void *parameter);
static rt_uint16_t count=0;
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
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  LL_APB4_GRP1_EnableClock(LL_APB4_GRP1_PERIPH_SYSCFG);

  /* System interrupt init*/
  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);


  /* USER CODE BEGIN 2 */
  //usart ÊµãËØï
  rt_kprintf("Start\n");
  PA9_thread_on =
  rt_thread_create("PA9_thread_on",              /* Á∫øÁ®ãÂêçÂ≠ó */
                    PA9_thread_on_entry,   /* Á∫øÁ®ãÂÖ•Âè£ÂáΩÊï∞ */
                    RT_NULL,             /* Á∫øÁ®ãÂÖ•Âè£ÂáΩÊï∞ÂèÇÊï∞ */
                    1024,                 /* Á∫øÁ®ãÊ†àÂ§ßÂ∞? */
                    6,                   /* Á∫øÁ®ãÁöÑ‰ºòÂÖàÁ∫ß */
                    10);
  if ( PA9_thread_on != RT_NULL){
      rt_thread_startup(PA9_thread_on);
    }
    else{
    LL_USART_TransmitData8(USART1, 'e');
    return -1;
  }
  PA9_thread_off =
  rt_thread_create("PA9_thread_off",              /* Á∫øÁ®ãÂêçÂ≠ó */
                    PA9_thread_off_entry,   /* Á∫øÁ®ãÂÖ•Âè£ÂáΩÊï∞ */
                    RT_NULL,             /* Á∫øÁ®ãÂÖ•Âè£ÂáΩÊï∞ÂèÇÊï∞ */
                    1024,                 /* Á∫øÁ®ãÊ†àÂ§ßÂ∞? */
                    7,                   /* Á∫øÁ®ãÁöÑ‰ºòÂÖàÁ∫ß */
                    10);
  if ( PA9_thread_off != RT_NULL){
      rt_thread_startup(PA9_thread_off);
    }
    else{
    LL_USART_TransmitData8(USART1, 'e');
    return -1;
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_4)
  {
  }
  LL_PWR_ConfigSupply(LL_PWR_LDO_SUPPLY);
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE0);
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
  LL_RCC_PLL1_SetN(192);
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

  LL_Init1msTick(480000000);

  LL_SetSystemCoreClock(480000000);
}

/* USER CODE BEGIN 4 */
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
static void PA9_thread_off_entry(void *param)
{ 
  rt_uint32_t count = 0; /* Á∫? Á®? 2 Êã? Êú? Ëæ? È´? Áö? ‰º? ÂÖ? Á∫? Ôº? ‰ª? Êä? Âç? Á∫? Á®? 1 ËÄ? Ëé? Âæ? Êâ? Ë°? */ 

  
  for (count = 0; count < 10 ; count++) 
  { 
    /* Á∫? Á®? 2 Êâ? Âç? ËÆ? Êï? ÂÄ? */ 
    //LL_mDelay(50);
    rt_kprintf("PA9_thread_off_entry count: %d\n", count); 
  
  } 
    rt_kprintf("PA9_thread_off_entry exit\n"); /* Á∫? Á®? 2 Ëø? Ë°? Áª? Êù? Âê? ‰π? Â∞? Ëá? Âä? Ë¢? Á≥? Áª? ËÑ? Á¶? */ 
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
