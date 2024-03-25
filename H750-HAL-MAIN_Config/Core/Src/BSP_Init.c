/** 
  ******************************************************************************
  * @file    BSP_Init.c 
  * @author  ample
  * @version V0.0.1
  * @date    2024��3��15��(V0.0.1)����ʼ�汾   
  * @brief   �弶֧�ְ���������
  ******************************************************************************
**/ 
/* 1.Include -------------------------------------------------------------------*/
#include "BSP_Init.h"
#include "rtthread.h"
#include "main.h"

/* 2.Private typedef -----------------------------------------------------------*/
/* 3.Private define ------------------------------------------------------------*/
/* 4.Private macro -------------------------------------------------------------*/
/* 5.Global variables ---------------------------------------------------------*/
extern uint16_t  PwmNum_Channel1;
extern uint16_t  PwmNum_Channel2;
extern SUASRT uart4;
extern SUASRT usart2;
/* 6.Private variables ---------------------------------------------------------*/
uint32_t tim2_ic1_start_cnt_value;							// TIM2ͨ��1����ʼʱ��������ֵ
uint32_t tim2_ic1_stop_cnt_value;								// TIM2ͨ��1�������ʱ��������ֵ
double tim2_c1_frequency;	  											// TIM2ͨ��1�������õ���Ƶ��
uint8_t tim2_ic1_complete_flag = 0;	  					// TIM2ͨ��1����״̬��1Ϊ������ɣ�0Ϊ����δ���
uint16_t tim2_ic1_count = 0;										// TIM2ͨ��1�������

uint32_t tim2_ic2_start_cnt_value;							// TIM2ͨ��2����ʼʱ��������ֵ
uint32_t tim2_ic2_stop_cnt_value;								// TIM2ͨ��2�������ʱ��������ֵ
double tim2_c2_frequency;	  											// TIM2ͨ��2�������õ���Ƶ��
uint8_t tim2_ic2_complete_flag = 0;	  					// TIM2ͨ��2����״̬��1Ϊ������ɣ�0Ϊ����δ���
uint16_t tim2_ic2_count = 0;										// TIM2ͨ��2�������

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim8;

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_uart4_rx;
DMA_HandleTypeDef hdma_uart4_tx;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;

#if defined ( __ICCARM__ ) /*!< IAR Compiler */

#pragma location=0x30000000
ETH_DMADescTypeDef  DMARxDscrTab[ETH_RX_DESC_CNT]; /* Ethernet Rx DMA Descriptors */
#pragma location=0x30000200
ETH_DMADescTypeDef  DMATxDscrTab[ETH_TX_DESC_CNT]; /* Ethernet Tx DMA Descriptors */

#elif defined ( __CC_ARM )  /* MDK ARM Compiler */

__attribute__((at(0x30000000))) ETH_DMADescTypeDef  DMARxDscrTab[ETH_RX_DESC_CNT]; /* Ethernet Rx DMA Descriptors */
__attribute__((at(0x30000200))) ETH_DMADescTypeDef  DMATxDscrTab[ETH_TX_DESC_CNT]; /* Ethernet Tx DMA Descriptors */

#elif defined ( __GNUC__ ) /* GNU Compiler */

ETH_DMADescTypeDef DMARxDscrTab[ETH_RX_DESC_CNT] __attribute__((section(".RxDecripSection"))); /* Ethernet Rx DMA Descriptors */
ETH_DMADescTypeDef DMATxDscrTab[ETH_TX_DESC_CNT] __attribute__((section(".TxDecripSection")));   /* Ethernet Tx DMA Descriptors */

#endif
ETH_BufferTypeDef Txbuffer[ETH_TX_DESC_CNT * 2U];
ETH_TxPacketConfig TxConfig;

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

ETH_HandleTypeDef heth;
/* 7.Global function prototypes -----------------------------------------------*/
/* 8.Private function prototypes -----------------------------------------------*/
static void GPIO_Configuration(void);
static void UART_Configuration(void);
static void TIM_Configuration(void);
static void EXTI_Configuration(void);
/* 9.functions -----------------------------------------------------------------*/
/*******************************************************************************
** ��    �� �� GPIO_Configuration
** ��    �� �� IO�����ú���
** ��ڲ��� �� ��
** ���ڲ��� �� ��
*******************************************************************************/

static void GPIO_Configuration(void)
{  
	GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  // ����IOʱ��
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOJ_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
  // LED1 ��ʼ��
  /*Configure GPIO pin : PA3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  // LED2 ��ʼ��
  /*Configure GPIO pin : PB0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}
static void UART_Configuration(void)
{
  //�ȳ�ʼ��DMA�������û������
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();
  __HAL_RCC_DMA2_CLK_ENABLE();
  /* DMA interrupt init */
  /* DMA1_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
  /* DMA1_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
  /* DMA2_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
	__HAL_UART_ENABLE_IT(&huart1,UART_IT_RXNE); 

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
	__HAL_UART_CLEAR_IT(&huart2, UART_CLEAR_IDLEF);
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);//����IDLE�ж�
  HAL_UART_Receive_DMA(&huart2,usart2.rxbuf,USART_BUF_LEN);//����DMA����
  
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 9600;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart4.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart4, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart4, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart4) != HAL_OK)
  {
    Error_Handler();
  }

	__HAL_UART_CLEAR_IT(&huart4, UART_CLEAR_IDLEF);
	__HAL_UART_ENABLE_IT(&huart4, UART_IT_IDLE);//����IDLE�ж�
	
  HAL_UART_Receive_DMA(&huart4,uart4.rxbuf,USART_BUF_LEN);//����DMA����
}
/*******************************************************************************
** ��    �� �� TIM_Configuration
** ��    �� �� ʱ��Դ����
** ��ڲ��� �� ��
** ���ڲ��� �� ��
*******************************************************************************/
static void TIM_Configuration(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 199;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 1000-1;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 500;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_TIM_MspPostInit(&htim1);

  TIM_ClockConfigTypeDef sClockSourceConfig2 = {0};
  TIM_MasterConfigTypeDef sMasterConfig2 = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};
  //Tim2��ʼ��
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 200;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 200-1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig2.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig2.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig2.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig2) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICFilter = 8;
  if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  TIM_ClockConfigTypeDef sClockSourceConfig3 = {0};
  TIM_MasterConfigTypeDef sMasterConfig3 = {0};
  TIM_OC_InitTypeDef sConfigOC3 = {0};
  //Tim3��ʼ��
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 400-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 1000-1;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig3.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig3.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig3.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig3) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC3.OCMode = TIM_OCMODE_PWM1;
  sConfigOC3.Pulse = 500;
  sConfigOC3.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC3.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC3, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC3, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_TIM_MspPostInit(&htim3);


  //TIM4��ʼ��
  TIM_SlaveConfigTypeDef sSlaveConfig4 = {0};
  TIM_MasterConfigTypeDef sMasterConfig4 = {0};
  
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 0;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 65535;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sSlaveConfig4.SlaveMode = TIM_SLAVEMODE_DISABLE;
  sSlaveConfig4.InputTrigger = TIM_TS_ITR2;
  if (HAL_TIM_SlaveConfigSynchro(&htim4, &sSlaveConfig4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig4.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig4.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig4) != HAL_OK)
  {
    Error_Handler();
  }
  //TIM5��ʼ��
  TIM_ClockConfigTypeDef sClockSourceConfig5 = {0};
  TIM_MasterConfigTypeDef sMasterConfig5 = {0};

  
  __HAL_RCC_TIM5_CLK_ENABLE();
  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 0;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 4294967295;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim5) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig5.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim5, &sClockSourceConfig5) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig5.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig5.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig5) != HAL_OK)
  {
    Error_Handler();
  }

  TIM_ClockConfigTypeDef sClockSourceConfig8 = {0};
  TIM_MasterConfigTypeDef sMasterConfig8 = {0};

  /* USER CODE BEGIN TIM8_Init 1 */

  /* USER CODE END TIM8_Init 1 */
  htim8.Instance = TIM8;
  htim8.Init.Prescaler = 0;
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim8.Init.Period = 65535;
  htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim8.Init.RepetitionCounter = 0;
  htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim8) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig8.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim8, &sClockSourceConfig8) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig8.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig8.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig8.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig8) != HAL_OK)
  {
    Error_Handler();
  }
}
/*******************************************************************************
** ��    �� �� BSP_Init
** ��    �� �� �弶֧�ְ���ʼ��
** ��ڲ��� �� ��
** ���ڲ��� �� ��
*******************************************************************************/
void BSP_Init(void)
{
  // IO��������
  GPIO_Configuration();
	LED1_Toggle;
  LED2_Toggle;
  // ��������
	UART_Configuration();                                                 
  HAL_UART_Transmit(&huart1,"create u1\r\n",11,10);
  HAL_UART_Transmit(&huart2,"create u2\r\n",11,10);
  HAL_UART_Transmit(&huart4,"create u4\r\n",11,10);
	// ��ʱ������
	TIM_Configuration();
  //MX_ETH_Init();
}
/*******************************************************************************
** ��    �� �� HAL_UART_MspInit
** ��    �� �� UART MSP��ʼ��
** ��ڲ��� �� uartHandle uartHandle�ṹ�� 
** ���ڲ��� �� ��
*******************************************************************************/
void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(uartHandle->Instance==UART4)
  {
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_UART4;
    PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* UART4 clock enable */
    __HAL_RCC_UART4_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**UART4 GPIO Configuration
    PA12     ------> UART4_TX
    PA11     ------> UART4_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF6_UART4;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* UART4 DMA Init */
    /* UART4_RX Init */
    hdma_uart4_rx.Instance = DMA2_Stream0;
    hdma_uart4_rx.Init.Request = DMA_REQUEST_UART4_RX;
    hdma_uart4_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_uart4_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_uart4_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_uart4_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_uart4_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_uart4_rx.Init.Mode = DMA_NORMAL;
    hdma_uart4_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_uart4_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_uart4_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_uart4_rx);

    /* UART4_TX Init */
    hdma_uart4_tx.Instance = DMA2_Stream1;
    hdma_uart4_tx.Init.Request = DMA_REQUEST_UART4_TX;
    hdma_uart4_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_uart4_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_uart4_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_uart4_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_uart4_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_uart4_tx.Init.Mode = DMA_NORMAL;
    hdma_uart4_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_uart4_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_uart4_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmatx,hdma_uart4_tx);

    /* UART4 interrupt Init */
    HAL_NVIC_SetPriority(UART4_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(UART4_IRQn);
  /* USER CODE BEGIN UART4_MspInit 1 */

  /* USER CODE END UART4_MspInit 1 */
  }
  else if(uartHandle->Instance==USART1)
  {
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1;
    PeriphClkInitStruct.Usart16ClockSelection = RCC_USART16CLKSOURCE_D2PCLK2;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA10     ------> USART1_RX
    PA9     ------> USART1_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  }
  else if(uartHandle->Instance==USART2)
  {

    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART2;
    PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* USART2 clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();

    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**USART2 GPIO Configuration
    PD5     ------> USART2_TX
    PD6     ------> USART2_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* USART2 DMA Init */
    /* USART2_RX Init */
    hdma_usart2_rx.Instance = DMA1_Stream0;
    hdma_usart2_rx.Init.Request = DMA_REQUEST_USART2_RX;
    hdma_usart2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart2_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart2_rx.Init.Mode = DMA_NORMAL;
    hdma_usart2_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart2_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart2_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart2_rx);

    /* USART2_TX Init */
    hdma_usart2_tx.Instance = DMA1_Stream1;
    hdma_usart2_tx.Init.Request = DMA_REQUEST_USART2_TX;
    hdma_usart2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart2_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart2_tx.Init.Mode = DMA_NORMAL;
    hdma_usart2_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart2_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart2_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart2_tx);

    /* USART2 interrupt Init */
    HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  }
}
/*******************************************************************************
** ��    �� �� HAL_TIM_Base_MspInit
** ��    �� �� TIMMSP��ʼ��
** ��ڲ��� �� htim��TIM�ṹ��
** ���ڲ��� �� ��
*******************************************************************************/
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(tim_baseHandle->Instance==TIM1)
  {
  /* USER CODE BEGIN TIM1_MspInit 0 */

  /* USER CODE END TIM1_MspInit 0 */
    /* TIM1 clock enable */
    __HAL_RCC_TIM1_CLK_ENABLE();

    /* TIM1 interrupt Init */
    HAL_NVIC_SetPriority(TIM1_CC_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);
  /* USER CODE BEGIN TIM1_MspInit 1 */

  /* USER CODE END TIM1_MspInit 1 */
  }
  else if(tim_baseHandle->Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspInit 0 */

  /* USER CODE END TIM2_MspInit 0 */
    /* TIM2 clock enable */
    __HAL_RCC_TIM2_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**TIM2 GPIO Configuration
    PA15 (JTDI)     ------> TIM2_CH1
    PB3 (JTDO/TRACESWO)     ------> TIM2_CH2
    */
    GPIO_InitStruct.Pin = GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* TIM2 interrupt Init */
    HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
  /* USER CODE BEGIN TIM2_MspInit 1 */

  /* USER CODE END TIM2_MspInit 1 */
  }
  else if(tim_baseHandle->Instance==TIM3)
  {
  /* USER CODE BEGIN TIM3_MspInit 0 */

  /* USER CODE END TIM3_MspInit 0 */
    /* TIM3 clock enable */
    __HAL_RCC_TIM3_CLK_ENABLE();
  /* USER CODE BEGIN TIM3_MspInit 1 */

  /* USER CODE END TIM3_MspInit 1 */
  }
  else if(tim_baseHandle->Instance==TIM4)
  {
  /* USER CODE BEGIN TIM4_MspInit 0 */

  /* USER CODE END TIM4_MspInit 0 */
    /* TIM4 clock enable */
    __HAL_RCC_TIM4_CLK_ENABLE();
  /* USER CODE BEGIN TIM4_MspInit 1 */

  /* USER CODE END TIM4_MspInit 1 */
  }
  else if(tim_baseHandle->Instance==TIM5)
  {
  /* USER CODE BEGIN TIM5_MspInit 0 */

  /* USER CODE END TIM5_MspInit 0 */
    /* TIM5 clock enable */
    __HAL_RCC_TIM5_CLK_ENABLE();
  /* USER CODE BEGIN TIM5_MspInit 1 */

  /* USER CODE END TIM5_MspInit 1 */
  }
  else if(tim_baseHandle->Instance==TIM8)
  {
  /* USER CODE BEGIN TIM8_MspInit 0 */

  /* USER CODE END TIM8_MspInit 0 */
    /* TIM8 clock enable */
    __HAL_RCC_TIM8_CLK_ENABLE();
  /* USER CODE BEGIN TIM8_MspInit 1 */

  /* USER CODE END TIM8_MspInit 1 */
  }
}
/*******************************************************************************
** ��    �� �� HAL_TIM_MspPostInit
** ��    �� �� PWM����ʼ��
** ��ڲ��� �� htim��TIM�ṹ��
** ���ڲ��� �� ��
*******************************************************************************/
void HAL_TIM_MspPostInit(TIM_HandleTypeDef* timHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(timHandle->Instance==TIM1)
  {
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOJ_CLK_ENABLE();
    /**TIM1 GPIO Configuration
    PA8     ------> TIM1_CH1
    PJ11     ------> TIM1_CH2
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
    HAL_GPIO_Init(GPIOJ, &GPIO_InitStruct);
  }
  else if(timHandle->Instance==TIM3)
  {

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**TIM3 GPIO Configuration
    PB5     ------> TIM3_CH2
    PB4 (NJTRST)     ------> TIM3_CH1
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  }
}

/*******************************************************************************
** ��    �� �� HAL_TIM_PWM_PulseFinishedCallback
** ��    �� �� PWM����жϻص�����
** ��ڲ��� �� htim��TIM�ṹ��
** ���ڲ��� �� ��
*******************************************************************************/
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
  static uint16_t cnt_channle1 = 0;    
  static uint16_t cnt_channle2 = 0;    
  if (htim->Instance == TIM1)
  {
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
    {
      cnt_channle1 ++;
      if(cnt_channle1 == PwmNum_Channel1)
			{
        rt_kprintf("PwmNum_Channel1 = %d\n",PwmNum_Channel1);
        HAL_TIM_PWM_Stop_IT(&htim1,TIM_CHANNEL_1);
      }                    
    }
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
    {
      cnt_channle2 ++;
      if(cnt_channle2 == PwmNum_Channel2)
			{
        rt_kprintf("PwmNum_Channel2 = %d\n",PwmNum_Channel2);
        HAL_TIM_PWM_Stop_IT(&htim1,TIM_CHANNEL_2);
      }                    
    }
  }
}
/*******************************************************************************
** ��    �� �� HAL_TIM_PeriodElapsedCallback
** ��    �� �� PWM�ſ��жϻص�����
** ��ڲ��� �� htim��TIM�ṹ��
** ���ڲ��� �� ��
*******************************************************************************/
void  HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim == &htim4)
  {
      if(__HAL_TIM_GET_FLAG(&htim4, TIM_FLAG_CC1) != RESET)  //�ж��Ƿ񴥷��ж�   
      {
          __HAL_TIM_CLEAR_FLAG(&htim4, TIM_FLAG_CC1);      //����жϱ�־
          HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);   //�ر�����ʱ��
          HAL_TIM_Base_Stop_IT(&htim4);         //�رմӶ�ʱ��
      }
  }
}
/*******************************************************************************
** ��    �� �� HAL_TIM_IC_CaptureCallback
** ��    �� �� TIM���벶���жϻص�����
** ��ڲ��� �� htim��TIM�ṹ��
** ���ڲ��� �� ��
*******************************************************************************/
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM2)
  {
		/* ������� */
		tim2_ic1_count++;
		
		/* ����ʼ */
		if (tim2_ic1_count == TIM2_ic1_START_CNT)
		{
			/* ȡ����ʼʱ��������ֵ */
			tim2_ic1_start_cnt_value = TIM2->CNT;
		}
		
		/* ������� */
		if (tim2_ic1_count == TIM2_ic1_STOP_CNT)
		{	
			/* ȡ�������ʱ��������ֵ */
			tim2_ic1_stop_cnt_value = TIM2->CNT;
			
			/* ������ɱ�־��λ */
			tim2_ic1_complete_flag = 1;
		}

    		/* ������� */
		tim2_ic2_count++;
		
		/* ����ʼ */
		if (tim2_ic2_count == TIM2_ic2_START_CNT)
		{
			/* ȡ����ʼʱ��������ֵ */
			tim2_ic2_start_cnt_value = TIM2->CNT;
		}
    
		/* ������� */
		if (tim2_ic2_count == TIM2_ic2_STOP_CNT)
		{	
			/* ȡ�������ʱ��������ֵ */
			tim2_ic2_stop_cnt_value = TIM2->CNT;
			
			/* ������ɱ�־��λ */
			tim2_ic2_complete_flag = 1;
		}
  }
}

/*******************************************************************************
** ��    �� �� TIM_IC1_Frequency
** ��    �� �� TIM2 ͨ��1���벶��
** ��ڲ��� �� ��
** ���ڲ��� �� Ƶ��
*******************************************************************************/
double TIM_IC1_Frequency(void)
{
	uint16_t count = 0;
	
	/* ������ɱ�־��λ */
	tim2_ic1_complete_flag = 0;
	
	/* �����벶�� */
	HAL_TIM_IC_Start(&htim2, TIM_CHANNEL_1);
	/* �����벶���ж� */
	__HAL_TIM_ENABLE_IT(&htim2, TIM_IT_CC1);

	/* �ȴ�������ɱ�־��λ */
	while (tim2_ic1_complete_flag != 1)
	{
		count++;
		/* ��ʱ */
		if (count > 2000)
		{
			printf("No frequency!\r\n");
			break;
		}
		HAL_Delay(1);
	}
	
	/* �ر����벶�� */
	HAL_TIM_IC_Stop(&htim2, TIM_CHANNEL_1);
	/* �ر����벶���ж� */
	__HAL_TIM_DISABLE_IT(&htim2, TIM_IT_CC1);
	
	/* ����Ƶ�� */
	tim2_c1_frequency = 200000000.0
										/
			((double)(tim2_ic1_stop_cnt_value - tim2_ic1_start_cnt_value) / (TIM2_ic1_STOP_CNT - TIM2_ic1_START_CNT));

	/* ������ɱ�־��λ */
	tim2_ic1_complete_flag = 0;

	/* TIM2���������0���ȴ��´βɼ� */
	tim2_ic1_count = 0;

	return tim2_c1_frequency;
}
/*******************************************************************************
** ��    �� �� TIM_IC2_Frequency
** ��    �� �� TIM2 ͨ��2���벶��
** ��ڲ��� �� ��
** ���ڲ��� �� Ƶ��
*******************************************************************************/
double TIM_IC2_Frequency(void)
{
	uint16_t count = 0;
	
	/* ������ɱ�־��λ */
	tim2_ic2_complete_flag = 0;
	
	/* �����벶�� */
	HAL_TIM_IC_Start(&htim2, TIM_CHANNEL_2);
	/* �����벶���ж� */
	__HAL_TIM_ENABLE_IT(&htim2, TIM_IT_CC2);
	
	/* �ȴ�������ɱ�־��λ */
	while (tim2_ic2_complete_flag != 1)
	{
		count++;
		/* ��ʱ */
		if (count > 2000)
		{
			printf("No frequency!\r\n");
			break;
		}
		rt_thread_mdelay(1);
	}
	
	/* �ر����벶�� */
	HAL_TIM_IC_Stop(&htim2, TIM_CHANNEL_2);
	/* �ر����벶���ж� */
	__HAL_TIM_DISABLE_IT(&htim2, TIM_IT_CC2);
	
	/* ����Ƶ�� */
	tim2_c2_frequency = 200000000.0
										/
			((double)(tim2_ic2_stop_cnt_value - tim2_ic2_start_cnt_value) / (TIM2_ic2_STOP_CNT - TIM2_ic2_START_CNT));

	/* ������ɱ�־��λ */
	tim2_ic2_complete_flag = 0;

	/* TIM2���������0���ȴ��´βɼ� */
	tim2_ic2_count = 0;

	return tim2_c2_frequency;
}
/*******************************************************************************
** ��    �� �� MX_ETH_Init
** ��    �� �� ETH��ʼ��
** ��ڲ��� �� ��
** ���ڲ��� �� Ƶ��
*******************************************************************************/
void MX_ETH_Init(void)
{
  static uint8_t MACAddr[6];
  heth.Instance = ETH;
  MACAddr[0] = 0x00;
  MACAddr[1] = 0x80;
  MACAddr[2] = 0xE1;
  MACAddr[3] = 0x00;
  MACAddr[4] = 0x00;
  MACAddr[5] = 0x00;
  heth.Init.MACAddr = &MACAddr[0];
  heth.Init.MediaInterface = HAL_ETH_MII_MODE;
  heth.Init.TxDesc = DMATxDscrTab;
  heth.Init.RxDesc = DMARxDscrTab;
  heth.Init.RxBuffLen = 1524;

  if (HAL_ETH_Init(&heth) != HAL_OK)
  {
    Error_Handler();
  }

  memset(&TxConfig, 0 , sizeof(ETH_TxPacketConfig));
  TxConfig.Attributes = ETH_TX_PACKETS_FEATURES_CSUM | ETH_TX_PACKETS_FEATURES_CRCPAD;
  TxConfig.ChecksumCtrl = ETH_CHECKSUM_IPHDR_PAYLOAD_INSERT_PHDR_CALC;
  TxConfig.CRCPadCtrl = ETH_CRC_PAD_INSERT;

}
/*******************************************************************************
** ��    �� �� HAL_ETH_MspInit
** ��    �� �� ETHMSP��ʼ��
** ��ڲ��� �� ��
** ���ڲ��� �� Ƶ��
*******************************************************************************/
void HAL_ETH_MspInit(ETH_HandleTypeDef* ethHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(ethHandle->Instance==ETH)
  {
    /* ETH clock enable */
    __HAL_RCC_ETH1MAC_CLK_ENABLE();
    __HAL_RCC_ETH1TX_CLK_ENABLE();
    __HAL_RCC_ETH1RX_CLK_ENABLE();

    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**ETH GPIO Configuration
    PG11     ------> ETH_TX_EN
    PE2     ------> ETH_TXD3
    PG12     ------> ETH_TXD1
    PG13     ------> ETH_TXD0
    PC1     ------> ETH_MDC
    PC2     ------> ETH_TXD2
    PC3     ------> ETH_TX_CLK
    PH2     ------> ETH_CRS
    PA2     ------> ETH_MDIO
    PA1     ------> ETH_RX_CLK
    PH3     ------> ETH_COL
    PA7     ------> ETH_RX_DV
    PC4     ------> ETH_RXD0
    PB1     ------> ETH_RXD3
    PH6     ------> ETH_RXD2
    PC5     ------> ETH_RXD1
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4
                          |GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_1|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  }
}

