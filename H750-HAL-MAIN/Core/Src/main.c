/** 
  ******************************************************************************
  * @file    main.c 
  * @author  Hsiang Hsu
  * @version V0.0.1
  * @date    2022��12��25��(V0.0.1)����ʼ�汾
  *          2023��11��27��(V0.0.2)���޸��ļ����ͣ����LL�����
  * @brief   ������
  ******************************************************************************
**/ 
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "BSP_Init.h"
#include "rtthread.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
SUASRT usart2 = {0};                                                      // ����2����
SUASRT uart4 = {0};                                                      // ����4����
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;
extern UART_HandleTypeDef huart1;

uint8_t rx_len=0;  
uint8_t recv_flag_usart2=0;
uint8_t recv_flag_uart4=0;
uint16_t PwmNum_Channel1 = 8000;
uint16_t PwmNum_Channel2 = 8000;
/* TIM2CH1_CAPTURE_STA ������λ˵��
** bit7   ������ɱ�־
** bit6	  ���񵽸ߵ�ƽ��־
** bit5~0 ����ߵ�ƽ��ʱ������Ĵ���*/
uint8_t TIM2CH1_CAPTURE_STA=0; // ���벶��״̬
uint16_t TIM2CH1_CAPTURE_VAL;  // ���벶��ֵ
/* TIM2CH2_CAPTURE_STA ������λ˵��
** bit7   ������ɱ�־
** bit6	  ���񵽸ߵ�ƽ��־
** bit5~0 ����ߵ�ƽ��ʱ������Ĵ���*/
uint8_t TIM2CH2_CAPTURE_STA=0;
uint16_t TIM2CH2_CAPTURE_VAL;
uint32_t temp_tim;
uint32_t temp_tim2;

unsigned char Rx_Buf[USART_BUF_LEN] = {0};
unsigned char Rx_Buf_test[10] = {65,66,67,68,69,70,71,10};
unsigned char flag = 0;

static rt_thread_t LED_Test = RT_NULL;
static rt_thread_t DMA_Test = RT_NULL;
static rt_thread_t TIM_Cap = RT_NULL;
static rt_sem_t DMA_Sem = RT_NULL;
static rt_sem_t DMA_Sem_Stop = RT_NULL;

void SystemClock_Config(void);
void MPU_Config(void);
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
static void Rt_TIM_Cap_Init(void);
static void TIM_Cap_entry(void *parameter);
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
** ��    �� : main
** ��    �� : ������
** ��ڲ��� : ��
** ���ڲ��� : int
*******************************************************************************/
int main(void)
{

	BSP_Init();
  HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);

	//AppUART_Init();
	recv_flag_uart4=0;
  recv_flag_usart2 =0;

	Rt_LED_Init();
  Rt_Sem_Init();
	Rt_DMA_Init();	
	Rt_TIM_Cap_Init();
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
  __HAL_RCC_D2SRAM1_CLK_ENABLE();
  __HAL_RCC_D2SRAM2_CLK_ENABLE();
  __HAL_RCC_D2SRAM3_CLK_ENABLE();
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

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER1;
  MPU_InitStruct.BaseAddress = 0x30020000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_128KB;
  MPU_InitStruct.SubRegionDisable = 0x0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER2;
  MPU_InitStruct.BaseAddress = 0x30040000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_512B;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);


}
/*******************************************************************************
** ��    �� : Start_Count_PWM
** ��    �� : �����ж�PWM������ֻ������TIM1 Channel1 Channel2
** ��ڲ��� : uint16_t PwmNum_Channel PWM���� 
             uint32_t Channel        PWM ͨ��
** ���ڲ��� :  ��
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
** ��    �� : Start_Count_PWM_Gate
** ��    �� : �����ſ��ж�PWM������ֻ������TIM1 Channel3
** ��ڲ��� : ��
** ���ڲ��� :  uint16_t PwmNum_Channel PWM����  ����256
              uint32_t Channel         PWM ͨ��
*******************************************************************************/
void Start_Count_PWM_Gate(uint32_t PwmNum_Channel)
{  
  __HAL_TIM_SET_AUTORELOAD(&htim3,PwmNum_Channel-1); //����Ҫ�����PWM������ 5��
  HAL_TIM_Base_Start_IT(&htim3);             //�����Ӷ�ʱ��
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);  //��������ʱ��PWM���
}
/*******************************************************************************
** ��    �� : LED_Init
** ��    �� : LED�̳߳�ʼ��
** ��ڲ��� : ��
** ���ڲ��� :  ��
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
** ��    �� : LED_Test_entry
** ��    �� : LED�߳�
** ��ڲ��� : ��
** ���ڲ��� :  ��
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
** ��    �� : Rt_Sem_Init
** ��    �� : ��ʼ���ź���
** ��ڲ��� : ��
** ���ڲ��� :  ��
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
void Rt_TIM_Cap_Init(void)
{
  rt_kprintf("This is TIM_CAP test...\n");
  TIM_Cap = rt_thread_create("TIM_Cap",
                              TIM_Cap_entry,
                              RT_NULL,
                              1024,
                              16,
                              10);
  if (TIM_Cap != RT_NULL) 
  {
    rt_thread_startup(TIM_Cap);
  }
}
void TIM_Cap_entry(void *parameter)
{
  while (1)
  {
    rt_thread_mdelay(1);
    if(TIM2CH1_CAPTURE_STA &0X80)//�ɹ�����һ��������
		{
      temp_tim=TIM2CH1_CAPTURE_STA&0X3F;
      temp_tim*=65536;//���ʱ���ܺ�          //��Ӧ��TIM_Periodһ�� TIM��16λ�� ���Ϊ65536
      temp_tim+=TIM2CH1_CAPTURE_VAL;//�ܼ����ܺ�
		  if(temp_tim>1000)             //ʡȥһЩ�����ź�
      {
        rt_kprintf("Channel1��ƽ����ʱ��:%d us     \n",temp_tim);//us
      }
      TIM2CH1_CAPTURE_STA=0;//������һ�β���
		}
    if(TIM2CH2_CAPTURE_STA &0X80)//�ɹ�����һ��������
		{
      temp_tim2=TIM2CH2_CAPTURE_STA&0X3F;
      temp_tim2*=65536;//���ʱ���ܺ�          //��Ӧ��TIM_Periodһ�� TIM��16λ�� ���Ϊ65536
      temp_tim2+=TIM2CH2_CAPTURE_VAL;//�ܼ����ܺ�
		  if(temp_tim2>1000)             //ʡȥһЩ�����ź�
      {
        rt_kprintf("Channel2��ƽ����ʱ��:%d us     ",temp_tim2);//us
        rt_kprintf("\n");//����
      }
			TIM2CH2_CAPTURE_STA=0;//������һ�β���
		}
  }
}
/*******************************************************************************
** ��    �� : Rt_DMA_Init
** ��    �� : ��ʼ��DMA�߳�
** ��ڲ��� : ��
** ���ڲ��� :  ��
*******************************************************************************/
void Rt_DMA_Init(void)
{
    DMA_Test = rt_thread_create("DMA Test",
                                DMA_Test_entry,
                                RT_NULL,
                                1024,
                                15,
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
        rt_kprintf("���յ������ݳ���Ϊ%d    ",uart4.rxlen);
        recv_flag_uart4 = 0;
        /* ������cache ����DMA�������ڴ� cache����ͬ���������Ҫ��Ч�����¼��� */
        SCB_InvalidateDCache_by_Addr((uint32_t *)uart4.rxbuf, USART_BUF_LEN);

        rt_memcpy(uart4.txbuf, uart4.rxbuf, USART_BUF_LEN);
        /* DMA����ʱ cache��������Ҫ���µ�SRAM�� */
        SCB_CleanDCache_by_Addr((uint32_t *)uart4.rxbuf, USART_BUF_LEN);
        HAL_UART_Transmit_DMA(&huart4,uart4.txbuf,USART_BUF_LEN);// DMA����
        uart4.rxlen=0;  
        HAL_UART_Receive_DMA(&huart4,uart4.rxbuf,USART_BUF_LEN);
      }
  //      if(recv_flag_usart2 ==1)			
  //		{	
  //			rt_kprintf("usart2���յ������ݳ���Ϊ%d    ",usart2.rxlen);
  //      recv_flag_usart2 = 0;
  //      /* ������cache ����DMA�������ڴ� cache����ͬ���������Ҫ��Ч�����¼��� */
  //      SCB_InvalidateDCache_by_Addr((uint32_t *)usart2.rxbuf, USART_BUF_LEN);

  //      rt_memcpy(usart2.txbuf, usart2.rxbuf, USART_BUF_LEN);
  //      /* DMA����ʱ cache��������Ҫ���µ�SRAM�� */
  //      SCB_CleanDCache_by_Addr((uint32_t *)usart2.rxbuf, USART_BUF_LEN);
  //			HAL_UART_Transmit_DMA(&huart2,usart2.txbuf,usart2.rxlen);// DMA����
  //      usart2.rxlen=0;  
  //      HAL_UART_Receive_DMA(&huart2,usart2.rxbuf,USART_BUF_LEN);
  //		}
    rt_thread_mdelay(1);
  }
  

}

/*******************************************************************************
** ��    �� : DMA_Start
** ��    �� : ����DMA����
** ��ڲ��� : ��
** ���ڲ��� :  ��
*******************************************************************************/
void DMA_Start()
{
  rt_sem_release(DMA_Sem);
}
/*******************************************************************************
** ��    �� : DMA_Start
** ��    �� : ����DMA����
** ��ڲ��� : ��
** ���ڲ��� :  ��
*******************************************************************************/
void DMA_Stop()
{
  rt_sem_release(DMA_Sem_Stop);
	
}
/*******************************************************************************
** ��    �� : AppUART_Init
** ��    �� : ����DMA����
** ��ڲ��� : ��
** ���ڲ��� :  ��
*******************************************************************************/
void AppUART_Init(void)
{
    HAL_UARTEx_ReceiveToIdle_DMA(&huart4, uart4.rxbuf, USART_BUF_LEN);
}
/*******************************************************************************
** ��    �� : AppUART_Task
** ��    �� : ����DMA����
** ��ڲ��� : ��
** ���ڲ��� :  ��
*******************************************************************************/
void AppUART_Task(void)
{
    if (recv_flag_uart4 == 1)
    {
        recv_flag_uart4 = 0;
        /* ������cache ����DMA�������ڴ� cache����ͬ���������Ҫ��Ч�����¼��� */
        SCB_InvalidateDCache_by_Addr((uint32_t *)uart4.rxbuf, USART_BUF_LEN);
        //printf("%.*s\r\n", g_U1RxSize, g_U1RxBuffer);
         
        memcpy(uart4.txbuf, uart4.rxbuf, USART_BUF_LEN);
        /* DMA����ʱ cache��������Ҫ���µ�SRAM�� */
        SCB_CleanDCache_by_Addr((uint32_t *)uart4.rxbuf, USART_BUF_LEN);
        HAL_UART_Transmit_DMA(&huart4, uart4.txbuf, USART_BUF_LEN);
    }
    rt_thread_mdelay(10);
}
/*******************************************************************************
** ��    �� : delay_us
** ��    �� : ΢����ʱ
** ��ڲ��� : ��
** ���ڲ��� :  ��
*******************************************************************************/
void delay_us(uint16_t nus)
{
	__HAL_TIM_SET_COUNTER(TIM_HANDLE, 0); //�Ѽ�������ֵ����Ϊ0
	__HAL_TIM_ENABLE(TIM_HANDLE); //��������
	while (__HAL_TIM_GET_COUNTER(TIM_HANDLE) < nus); //ÿ����һ�Σ�����1us��ֱ��������ֵ����������Ҫ��ʱ��
	__HAL_TIM_DISABLE(TIM_HANDLE); //�رռ���
}
/*******************************************************************************
** ��    �� : delay_s
** ��    �� : ����ʱ
** ��ڲ��� : ��
** ���ڲ��� :  ��
*******************************************************************************/
void delay_s(uint16_t s)
{
	__HAL_TIM_SET_COUNTER(TIM_HANDLE_S, 0); //�Ѽ�������ֵ����Ϊ0
	__HAL_TIM_ENABLE(TIM_HANDLE_S); //��������
	while (__HAL_TIM_GET_COUNTER(TIM_HANDLE_S) < s); //ÿ����һ�Σ�����1s��ֱ��������ֵ����������Ҫ��ʱ��
	__HAL_TIM_DISABLE(TIM_HANDLE_S); //�رռ���
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
