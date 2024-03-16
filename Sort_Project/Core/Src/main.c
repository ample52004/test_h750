/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "stdlib.h"
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
typedef struct Step_Config
{
  //uint16_t Shippingspace;//仓位
  //uint16_t Shippingspace;//哪一个步进电机
  uint32_t X_PWM_Count;
  uint32_t Y_PWM_Count;
  uint32_t Now_PositionX;
  uint32_t Now_PositionY;
  uint32_t Last_PositionX;
  uint32_t Last_PositionY;
  int deltaPositionx;
  int deltaPositiony;
  int PWM_X_DirFlag;//电机正反
  int PWM_Y_DirFlag;
}Step_Config1;
Step_Config1 space1;
Step_Config1 space2;
Step_Config1 space3;
Step_Config1 space4;
/*Recive 前三个字符是机器序列号小仓位,后三位是X坐标然后是Y坐标,最后一个字符验证接受结束*/
uint8_t RxBuffer[3];

int Shippingspace;
int FineFraction = 32; 
//细分数 步进电机驱动设置
/*PWM_Start_flag[0]每一个大仓位中的四个仓位的哪一个*/
/*PWM_Start_flag[1]抓药启动标志*/
uint8_t PWM_Start_flag[2];
int CircleCount1X_Count = 0;//步进电机转的圈数
int CircleCount1Y_Count = 0;
int CircleCount2X_Count = 0;//步进电机转的圈数
int CircleCount2Y_Count = 0;
int CircleCount3X_Count = 0;//步进电机转的圈数
int CircleCount3Y_Count = 0;
int CircleCount4X_Count = 0;//步进电机转的圈数
int CircleCount4Y_Count = 0;
int X1_Flag = 0;
int Y1_Flag = 0;
int X2_Flag = 0;
int Y2_Flag = 0;
int X3_Flag = 0;
int Y3_Flag = 0;
int X4_Flag = 0;
int Y4_Flag = 0;
char Error[] ="Messageerror"; 
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
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM5_Init();
  MX_USART3_UART_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  MX_UART4_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  space1.PWM_X_DirFlag = 0;
  space1.PWM_Y_DirFlag = 0;
  space1.Last_PositionX=0;
  space1.Last_PositionY=0;
  space1.Now_PositionX = 0;
  space1.Now_PositionY = 0;
  space1.deltaPositionx = 0;
  space1.deltaPositiony = 0;
  space1.X_PWM_Count = 0;
  space1.Y_PWM_Count = 0;

  space2.Last_PositionX=0;
  space2.Last_PositionY=0;
  space2.PWM_X_DirFlag = 0;
  space2.PWM_Y_DirFlag = 0;
  space2.deltaPositionx = 0;
  space2.deltaPositiony = 0;
  space2.X_PWM_Count = 0;
  space2.Y_PWM_Count = 0;
  
  space3.Last_PositionX=0;
  space3.Last_PositionY=0;
  space3.PWM_X_DirFlag = 0;
  space3.PWM_Y_DirFlag = 0;
  space3.deltaPositionx = 0;
  space3.deltaPositiony = 0;
  space3.X_PWM_Count = 0;
  space3.Y_PWM_Count = 0;
  
  space4.Last_PositionX=0;
  space4.Last_PositionY=0;
  space4.PWM_X_DirFlag = 0;
  space4.PWM_Y_DirFlag = 0;
  space4.deltaPositionx = 0;
  space4.deltaPositiony = 0;
  space4.X_PWM_Count = 0;
  space4.Y_PWM_Count = 0;  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    HAL_UART_Receive_IT(&huart3,RxBuffer,3);
    HAL_UART_Receive_IT(&huart1,PWM_Start_flag,2);
  }
  return 0; 
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
  uint16_t Now_PositonX=0;
  uint16_t Now_PositonY=0;
  /*接受香橙派开发板的位置信息
  * Shippingspace 四个仓位的那一个
  */
  if (huart==&huart3)
  {
    Shippingspace = RxBuffer[0];
    Now_PositonX = RxBuffer[1];
    Now_PositonY = RxBuffer[2];
    if (Now_PositonX>500)
    {
      Now_PositonX =500;
    }
    if (Now_PositonX<0)
    {
      Now_PositonX = 0;
    }
    if (Now_PositonY>500)
    {
      Now_PositonY =500;
    }
    if (Now_PositonY<0)
    {
      Now_PositonY = 0;
    }
    switch (Shippingspace)
    {
    case 1:
      space1.Now_PositionX = Now_PositonX;
      space1.Now_PositionY = Now_PositonY;
      space1.deltaPositionx = space1.Now_PositionX - space1.Last_PositionX;
      space1.deltaPositiony = space1.Now_PositionY - space1.Last_PositionY;
      if (space1.deltaPositionx > 0 )
      {
        space1.PWM_X_DirFlag = 1;
        HAL_GPIO_WritePin(Mechine1_Dir_X_GPIO_Port,Mechine1_Dir_X_Pin,GPIO_PIN_SET);
      }
      else
      { 
        space1.PWM_X_DirFlag = 0;
        HAL_GPIO_WritePin(Mechine1_Dir_X_GPIO_Port,Mechine1_Dir_X_Pin,GPIO_PIN_RESET);
      }
      if (space1.deltaPositiony > 0 )
      {
        space1.PWM_Y_DirFlag = 1;
        HAL_GPIO_WritePin(Mechine1_Dir_Y_GPIO_Port,Mechine1_Dir_Y_Pin,GPIO_PIN_SET);
      }
      else
      {
        space1.PWM_Y_DirFlag = 0;
        HAL_GPIO_WritePin(Mechine1_Dir_Y_GPIO_Port,Mechine1_Dir_Y_Pin,GPIO_PIN_RESET);
      }
      break;
    case 2:
      space2.Now_PositionX = Now_PositonX;
      space2.Now_PositionY = Now_PositonY;
      space2.deltaPositionx = space2.Now_PositionX - space2.Last_PositionX;
      space2.deltaPositiony = space2.Now_PositionY - space2.Last_PositionY;
      if (space2.deltaPositionx > 0 )
      {
        space2.PWM_X_DirFlag = 1;
        HAL_GPIO_WritePin(Mechine2_Dir_X_GPIO_Port,Mechine2_Dir_X_Pin,GPIO_PIN_SET);
      }
      else
      {
        space2.PWM_X_DirFlag = 0;
        HAL_GPIO_WritePin(Mechine2_Dir_X_GPIO_Port,Mechine2_Dir_X_Pin,GPIO_PIN_RESET);
      }
      if (space2.deltaPositiony > 0 )
      {
        space2.PWM_Y_DirFlag = 1;
        HAL_GPIO_WritePin(Mechine2_Dir_Y_GPIO_Port,Mechine2_Dir_Y_Pin,GPIO_PIN_SET);
      }
      else
      {        
        space2.PWM_Y_DirFlag = 0;
        HAL_GPIO_WritePin(Mechine2_Dir_Y_GPIO_Port,Mechine2_Dir_Y_Pin,GPIO_PIN_RESET);
      }
      break;
    case 3:
      space3.Now_PositionX = Now_PositonX;
      space3.Now_PositionY = Now_PositonY;
      space3.deltaPositionx = space3.Now_PositionX - space3.Last_PositionX;
      space3.deltaPositiony = space3.Now_PositionY - space3.Last_PositionY;
      if (space3.deltaPositionx > 0 )
      {
        space3.PWM_X_DirFlag = 1;
        HAL_GPIO_WritePin(Mechine3_Dir_X_GPIO_Port,Mechine3_Dir_X_Pin,GPIO_PIN_SET);
      }
      else
      {
        space3.PWM_X_DirFlag = 0;
        HAL_GPIO_WritePin(Mechine3_Dir_X_GPIO_Port,Mechine3_Dir_X_Pin,GPIO_PIN_RESET);
      }
      if (space3.deltaPositiony > 0 )
      {
        space3.PWM_Y_DirFlag = 1;
        HAL_GPIO_WritePin(Mechine3_Dir_Y_GPIO_Port,Mechine3_Dir_Y_Pin,GPIO_PIN_SET);
      }
      else
      {
        space3.PWM_Y_DirFlag = 0;
        HAL_GPIO_WritePin(Mechine3_Dir_Y_GPIO_Port,Mechine3_Dir_Y_Pin,GPIO_PIN_RESET);
      }
      break;
    case 4:
      space4.Now_PositionX = Now_PositonX;
      space4.Now_PositionY = Now_PositonY;
      space4.deltaPositionx = space4.Now_PositionX - space4.Last_PositionX;
      space4.deltaPositiony = space4.Now_PositionY - space4.Last_PositionY;
      if (space4.deltaPositionx > 0 )
      {
        space4.PWM_X_DirFlag = 1;
        HAL_GPIO_WritePin(Mechine4_Dir_X_GPIO_Port,Mechine4_Dir_X_Pin,GPIO_PIN_SET);
      }
      else
      {        
        space4.PWM_X_DirFlag = 0;
        HAL_GPIO_WritePin(Mechine4_Dir_X_GPIO_Port,Mechine4_Dir_X_Pin,GPIO_PIN_RESET);
      }

      if (space4.deltaPositiony > 0 )
      {
        space4.PWM_Y_DirFlag = 1;
        HAL_GPIO_WritePin(Mechine4_Dir_Y_GPIO_Port,Mechine4_Dir_Y_Pin,GPIO_PIN_SET);
      }
      else
      {
        space4.PWM_Y_DirFlag = 0;
        HAL_GPIO_WritePin(Mechine3_Dir_Y_GPIO_Port,Mechine3_Dir_Y_Pin,GPIO_PIN_RESET);
      }
      break;
    default:
      break;
    }
     
  }
  /*接受主机传送启动命令
  * PWM_Start_flag[0] 哪一台主机;
  * PWM_Start_flag[1]运动标志位
  */
  if (huart==&huart1)
  {
    int Whichdevice;
    int Startflag = 0;
    Whichdevice = PWM_Start_flag[0];
    Startflag = PWM_Start_flag[1];
    if (Whichdevice == 0X31)
    {
     if (Startflag == 0X31)
     {
      space1.X_PWM_Count = (int)(space1.deltaPositionx/50 * FineFraction);
      space1.Y_PWM_Count = (int)(space1.deltaPositiony/50 * FineFraction);
      space1.X_PWM_Count =6400;
      HAL_GPIO_WritePin(Mechine1_Dir_X_GPIO_Port,Mechine1_Dir_X_Pin,GPIO_PIN_SET);
      X1_Flag =1;
      Y1_Flag =1;
      HAL_TIM_PWM_Start_IT(&htim2,TIM_CHANNEL_3);
      HAL_TIM_PWM_Start_IT(&htim2,TIM_CHANNEL_4);
     }
    }
    else if (Whichdevice == 0X32)
    {
      space2.X_PWM_Count = (int)(space2.deltaPositionx/50 * FineFraction);
      space2.Y_PWM_Count = (int)(space2.deltaPositiony/50 * FineFraction); 
      X2_Flag =1;
      Y2_Flag =1;
      HAL_TIM_PWM_Start_IT(&htim3,TIM_CHANNEL_1);
      HAL_TIM_PWM_Start_IT(&htim3,TIM_CHANNEL_2);
    }
    else if (Whichdevice == 0X33)
    {
      space3.X_PWM_Count = (int)(space3.deltaPositionx/50 * FineFraction);
      space3.Y_PWM_Count = (int)(space3.deltaPositiony/50 * FineFraction); 
      X3_Flag =1;
      Y3_Flag =1;
      HAL_TIM_PWM_Start_IT(&htim4,TIM_CHANNEL_1);
      HAL_TIM_PWM_Start_IT(&htim4,TIM_CHANNEL_2);
    }
    else if (Whichdevice == 0X34)
    {
      space4.X_PWM_Count = (int)(space4.deltaPositionx/50 * FineFraction);
      space4.Y_PWM_Count = (int)(space4.deltaPositiony/50 * FineFraction); 
      X4_Flag = 1;
      Y4_Flag =1;
      HAL_TIM_PWM_Start_IT(&htim5,TIM_CHANNEL_1);
      HAL_TIM_PWM_Start_IT(&htim5,TIM_CHANNEL_2);
    }
    else
    HAL_UART_Transmit(&huart1,(uint8_t*)Error,sizeof(Error),100);
  }
  HAL_UART_Receive_IT(&huart3,RxBuffer,3);
  HAL_UART_Receive_IT(&huart1,PWM_Start_flag,2);
}



void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
  if (htim == &htim2)
  {
    if (X1_Flag == 1)
    {
      CircleCount1X_Count++;
      HAL_UART_Transmit(&huart1,(uint8_t*)&X1_Flag,1,100);
    }
    if (Y1_Flag == 1 )
    {
      CircleCount1Y_Count++;
    }
    if (space1.X_PWM_Count == CircleCount1X_Count )
    {
      HAL_TIM_PWM_Stop(&htim2,TIM_CHANNEL_4);
      CircleCount1X_Count = 0;
      X1_Flag = 0;
    }
    if (space1.Y_PWM_Count == CircleCount1Y_Count)
    {
      HAL_TIM_PWM_Stop(&htim2,TIM_CHANNEL_3);
      CircleCount1Y_Count = 0;
      Y1_Flag = 0;
    }  
  }
  if (htim == &htim3)
  {
    if (X2_Flag == 1)
    {
      CircleCount2X_Count++;

    }
    if (Y2_Flag == 1 )
    {
      CircleCount2Y_Count++;
    }
    if (space2.X_PWM_Count == CircleCount2X_Count )
    {
      HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_1);
      CircleCount2X_Count = 0;
      X2_Flag = 0;
    }
    if (space2.Y_PWM_Count == CircleCount2Y_Count)
    {
      HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_2);
      CircleCount2Y_Count = 0;
      Y2_Flag = 0;
    }  
  if (htim == &htim4)
  {
    if (X3_Flag == 1)
    {
      CircleCount3X_Count++;
    }
    if (Y3_Flag == 1 )
    {
      CircleCount3Y_Count++;
    }
    if (space3.X_PWM_Count == CircleCount3X_Count )
    {
      HAL_TIM_PWM_Stop(&htim4,TIM_CHANNEL_1);
      CircleCount3X_Count = 0;
      X3_Flag = 0;
    }
    if (space3.Y_PWM_Count == CircleCount3Y_Count)
    {
      HAL_TIM_PWM_Stop(&htim4,TIM_CHANNEL_2);
      CircleCount3Y_Count = 0;
      Y3_Flag = 0;
    }

  }
  if (htim == &htim5)
  {
    if (X4_Flag == 1)
    {
      CircleCount4X_Count++;
    }
    if (Y4_Flag == 1 )
    {
      CircleCount4Y_Count++;
    }
    if (space4.X_PWM_Count == CircleCount4X_Count )
    {
      HAL_TIM_PWM_Stop(&htim5,TIM_CHANNEL_2);
      X4_Flag = 0;
      CircleCount4X_Count = 0;
    }
    if (space4.Y_PWM_Count == CircleCount3Y_Count)
    {
      HAL_TIM_PWM_Stop(&htim5,TIM_CHANNEL_3);
      Y4_Flag = 0;
      CircleCount4Y_Count = 0;
    }

  }
}
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
