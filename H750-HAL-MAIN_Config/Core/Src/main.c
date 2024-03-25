/** 
  ******************************************************************************
  * @file    main.c 
  * @author  Hsiang Hsu
  * @version V0.0.1
  * @date    2022年12月25日(V0.0.1)：初始版本
  *          2023年11月27日(V0.0.2)：修改文件类型，设计LL库程序
  * @brief   主函数
  ******************************************************************************
**/ 
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* 1.Include -------------------------------------------------------------------*/
#include "rtthread.h"
#include "BSP_Init.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
SUASRT usart2 = {0};                                                      // 串口2数据
SUASRT uart4 = {0};                                                      // 串口4数据
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;
extern UART_HandleTypeDef huart1;

uint8_t rx_len=0;  
uint8_t recv_flag_usart2=0;
uint8_t recv_flag_uart4=0;
uint16_t PwmNum_Channel1 = 8000;
uint16_t PwmNum_Channel2 = 8000;

unsigned char Rx_Buf[USART_BUF_LEN] = {0};
unsigned char Rx_Buf_test[10] = {65,66,67,68,69,70,71,10};
unsigned char flag = 0;

static rt_thread_t LED_Test = RT_NULL;
static rt_thread_t DMA_Test = RT_NULL;
static rt_sem_t DMA_Sem = RT_NULL;
static rt_sem_t DMA_Sem_Stop = RT_NULL;

void SystemClock_Config(void);
static void MPU_Config(void);
static void Start_Count_PWM(uint16_t PwmNum_Channel, uint32_t Channel);
static void Rt_LED_Init(void);
static void LED_Test_entry(void *parameter);
static void Rt_Sem_Init(void);
static void Rt_DMA_Init(void);
static void DMA_Test_entry(void *parameter);
static void DMA_Start(void);
static void DMA_Stop(void);
static void AppUART_Init(void);
static void AppUART_Task(void);


/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
** 名    称 : main
** 功    能 : 主函数
** 入口参数 : 无
** 出口参数 : int
*******************************************************************************/
int main(void)
{
  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();
	SCB_EnableICache();

	SCB_EnableDCache();
	BSP_Init();
	//BSP_Init();

	recv_flag_uart4  =0;
  recv_flag_usart2 =0;
	
	Rt_LED_Init();
  Rt_Sem_Init();
	Rt_DMA_Init();
	
  static char tempbuf[128];

  

}

/* ==============   BOARD SPECIFIC CONFIGURATION CODE BEGIN    ============== */
/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL1 (HSE BYPASS)
  *            SYSCLK(Hz)                     = 400000000 (CPU Clock)
  *            HCLK(Hz)                       = 200000000 (AXI and AHBs Clock)
  *            AHB Prescaler                  = 2
  *            D1 APB3 Prescaler              = 2 (APB3 Clock  100MHz)
  *            D2 APB1 Prescaler              = 2 (APB1 Clock  100MHz)
  *            D2 APB2 Prescaler              = 2 (APB2 Clock  100MHz)
  *            D3 APB4 Prescaler              = 2 (APB4 Clock  100MHz)
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 5
  *            PLL_N                          = 160
  *            PLL_P                          = 2
  *            PLL_Q                          = 2
  *            PLL_R                          = 2
  *            VDD(V)                         = 3.3
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 160;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}


/* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}
/*******************************************************************************
** 名    称 : Start_Count_PWM
** 功    能 : 开启中断PWM计数，只能设置TIM1 Channel1 Channel2
** 入口参数 : uint16_t PwmNum_Channel PWM数量 
             uint32_t Channel        PWM 通道
** 出口参数 :  无
*******************************************************************************/
void Start_Count_PWM(uint16_t PwmNum_Channel, uint32_t Channel)
{
    if (Channel == TIM_CHANNEL_1)
    {
      PwmNum_Channel1 = PwmNum_Channel;
      HAL_TIM_PWM_Start_IT(&htim1,Channel);

    }
    if (Channel == TIM_CHANNEL_2)
    {
      PwmNum_Channel2 = PwmNum_Channel;
      HAL_TIM_PWM_Start_IT(&htim1,Channel);
    } 
}
/*******************************************************************************
** 名    称 : Start_Count_PWM_Gate
** 功    能 : 开启门控中断PWM计数，只能设置TIM1 Channel3
** 入口参数 : 无
** 出口参数 :  uint16_t PwmNum_Channel PWM数量  少于256
              uint32_t Channel         PWM 通道
*******************************************************************************/
void Start_Count_PWM_Gate(uint32_t PwmNum_Channel)
{  
  __HAL_TIM_SET_AUTORELOAD(&htim3,PwmNum_Channel-1); //设置要输出的PWM脉冲数 5个
  HAL_TIM_Base_Start_IT(&htim3);             //启动从定时器
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);  //启动主定时器PWM输出
}
/*******************************************************************************
** 名    称 : LED_Init
** 功    能 : LED线程初始化
** 入口参数 : 无
** 出口参数 :  无
*******************************************************************************/
void Rt_LED_Init(void)
{
    LED_Test = rt_thread_create("LED Test",
                                LED_Test_entry,
                                RT_NULL,
                                1024,
                                18,
                                10);
    if (LED_Test != RT_NULL) 
    {
      rt_thread_startup(LED_Test);
    }
}
/*******************************************************************************
** 名    称 : LED_Test_entry
** 功    能 : LED线程
** 入口参数 : 无
** 出口参数 :  无
*******************************************************************************/
void LED_Test_entry(void *parameter)
{
  while (1)
  {
    LED1_Toggle;
    rt_thread_mdelay(1000);
    LED2_Toggle;
    rt_thread_mdelay(1000);
  }
}
/*******************************************************************************
** 名    称 : Rt_Sem_Init
** 功    能 : 初始化信号量
** 入口参数 : 无
** 出口参数 :  无
*******************************************************************************/
void Rt_Sem_Init()
{
    DMA_Sem = rt_sem_create("DMA_Sem", 0, RT_IPC_FLAG_PRIO);
    if (DMA_Sem == RT_NULL)
    {
        rt_kprintf("create dynamic semaphore failed.\n");
    }
    DMA_Sem_Stop = rt_sem_create("DMA_Sem_Stop", 0, RT_IPC_FLAG_PRIO);
    if (DMA_Sem_Stop == RT_NULL)
    {
        rt_kprintf("create dynamic semaphore failed.\n");
    }
}
/*******************************************************************************
** 名    称 : Rt_DMA_Init
** 功    能 : 初始化DMA线程
** 入口参数 : 无
** 出口参数 :  无
*******************************************************************************/
void Rt_DMA_Init(void)
{
    DMA_Test = rt_thread_create("DMA Test",
                                DMA_Test_entry,
                                RT_NULL,
                                1024,
                                19,
                                10);
    if (DMA_Test != RT_NULL) 
    {
      rt_thread_startup(DMA_Test);
    }
}

void DMA_Test_entry(void *parameter)
{
  while (1)
  {
		if(recv_flag_uart4 ==1)			
		{	
			rt_kprintf("接收到的数据长度为%d    ",uart4.rxlen);
      recv_flag_uart4 = 0;
      /* 开启了cache 由于DMA更新了内存 cache不能同步，因此需要无效化从新加载 */
      SCB_InvalidateDCache_by_Addr((uint32_t *)uart4.rxbuf, USART_BUF_LEN);

      rt_memcpy(uart4.txbuf, uart4.rxbuf, USART_BUF_LEN);
      /* DMA发送时 cache的内容需要更新到SRAM中 */
      SCB_CleanDCache_by_Addr((uint32_t *)uart4.rxbuf, USART_BUF_LEN);
			HAL_UART_Transmit_DMA(&huart4,uart4.txbuf,USART_BUF_LEN);// DMA发送
      uart4.rxlen=0;
      HAL_UART_Receive_DMA(&huart4,uart4.rxbuf,USART_BUF_LEN);
		}
//      if(recv_flag_usart2 ==1)			
//		{	
//			rt_kprintf("usart2接收到的数据长度为%d    ",usart2.rxlen);
//      recv_flag_usart2 = 0;
//      /* 开启了cache 由于DMA更新了内存 cache不能同步，因此需要无效化从新加载 */
//      SCB_InvalidateDCache_by_Addr((uint32_t *)usart2.rxbuf, USART_BUF_LEN);

//      rt_memcpy(usart2.txbuf, usart2.rxbuf, USART_BUF_LEN);
//      /* DMA发送时 cache的内容需要更新到SRAM中 */
//      SCB_CleanDCache_by_Addr((uint32_t *)usart2.rxbuf, USART_BUF_LEN);
//			HAL_UART_Transmit_DMA(&huart2,usart2.txbuf,usart2.rxlen);// DMA发送
//      usart2.rxlen=0;  
//      HAL_UART_Receive_DMA(&huart2,usart2.rxbuf,USART_BUF_LEN);
//		}
		rt_thread_mdelay(1);
  }
}

/*******************************************************************************
** 名    称 : DMA_Start
** 功    能 : 开启DMA接收
** 入口参数 : 无
** 出口参数 :  无
*******************************************************************************/
void DMA_Start()
{
  rt_sem_release(DMA_Sem);
}
/*******************************************************************************
** 名    称 : DMA_Start
** 功    能 : 开启DMA接收
** 入口参数 : 无
** 出口参数 :  无
*******************************************************************************/
void DMA_Stop()
{
  rt_sem_release(DMA_Sem_Stop);
	
}
/*******************************************************************************
** 名    称 : AppUART_Init
** 功    能 : 测试DMA接收
** 入口参数 : 无
** 出口参数 :  无
*******************************************************************************/
void AppUART_Init(void)
{
    HAL_UARTEx_ReceiveToIdle_DMA(&huart4, uart4.rxbuf, USART_BUF_LEN);
}
/*******************************************************************************
** 名    称 : AppUART_Task
** 功    能 : 测试DMA接收
** 入口参数 : 无
** 出口参数 :  无
*******************************************************************************/
void AppUART_Task(void)
{
    if (recv_flag_uart4 == 1)
    {
        recv_flag_uart4 = 0;
        /* 开启了cache 由于DMA更新了内存 cache不能同步，因此需要无效化从新加载 */
        SCB_InvalidateDCache_by_Addr((uint32_t *)uart4.rxbuf, USART_BUF_LEN);
        //printf("%.*s\r\n", g_U1RxSize, g_U1RxBuffer);
         
        memcpy(uart4.txbuf, uart4.rxbuf, USART_BUF_LEN);
        /* DMA发送时 cache的内容需要更新到SRAM中 */
        SCB_CleanDCache_by_Addr((uint32_t *)uart4.rxbuf, USART_BUF_LEN);
        HAL_UART_Transmit_DMA(&huart4, uart4.txbuf, USART_BUF_LEN);
    }
    rt_thread_mdelay(10);
}
/*******************************************************************************
** 名    称 : delay_us
** 功    能 : 微秒延时
** 入口参数 : 无
** 出口参数 :  无
*******************************************************************************/
void delay_us(uint16_t nus)
{
	__HAL_TIM_SET_COUNTER(TIM_HANDLE, 0); //把计数器的值设置为0
	__HAL_TIM_ENABLE(TIM_HANDLE); //开启计数
	while (__HAL_TIM_GET_COUNTER(TIM_HANDLE) < nus); //每计数一次，就是1us，直到计数器值等于我们需要的时间
	__HAL_TIM_DISABLE(TIM_HANDLE); //关闭计数
}
/*******************************************************************************
** 名    称 : delay_s
** 功    能 : 秒延时
** 入口参数 : 无
** 出口参数 :  无
*******************************************************************************/
void delay_s(uint16_t s)
{
	__HAL_TIM_SET_COUNTER(TIM_HANDLE_S, 0); //把计数器的值设置为0
	__HAL_TIM_ENABLE(TIM_HANDLE_S); //开启计数
	while (__HAL_TIM_GET_COUNTER(TIM_HANDLE_S) < s); //每计数一次，就是1s，直到计数器值等于我们需要的时间
	__HAL_TIM_DISABLE(TIM_HANDLE_S); //关闭计数
}
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
