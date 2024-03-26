/**
  ******************************************************************************
  * @file    stm32f2xx_it.c
  * @author  MCD Application Team
  * @version V1.0.2
  * @date    06-June-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "public_define.h"

//#if (Printer_Parallel_Type == PRINTER_Parallel_FT8000)
//extern u16 Revlen;

//#endif
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
//void HardFault_Handler(void)
//{
//    uint32_t i=0,j=0;
//    /* Go to infinite loop when Hard Fault exception occurs */
//    while (1)
//    {
//        for(i=0;i<500;i++)
//            for(j=0;j<1000;j++);
//        __set_FAULTMASK(1);      // 关闭所有中端
//        NVIC_SystemReset();      // 复位
//    }
//}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
//      Bprintf("MemManage_Handler \r\n");
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
//      Bprintf("BusFault_Handler \r\n");
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
//      Bprintf("UsageFault_Handler \r\n");
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
//    Bprintf("DebugMon_Handler \r\n");
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
//void PendSV_Handler(void)
//{
//}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
//void SysTick_Handler(void)
//{
//    bool Usart1RevFlag = false;
//    Usart1RevFlag = 0;
//    
//}

/*-------------------------------------------------*/
/*函数名：串口中断函数                            */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
bool Usart1RevFlag = false; //进入过串口的标志
void USART1_IRQHandler(void)   //串口中断服务程序-----2G模组通信
{
//    if(USART_GetITStatus(COM_DB9_UART, USART_IT_RXNE) != RESET)  //接收中断
//    {
//        DB9_Data.Rev_Buff[DB9_Data.Rev_Buff_Len++]=USART_ReceiveData(COM_DB9_UART);
//        Usart1RevFlag = true;
//        if(DB9_Data.Rev_Buff_Len>=COM_DB9_RXBUFF_SIZE)
//        {
//            DB9_Data.Rev_Buff_Len=0;
//        }
//        USART_ClearFlag(COM_DB9_UART,USART_FLAG_RXNE);
//    }
}

/*-------------------------------------------------*/
/*函数名：定时器3中断服务函数                      */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void TIM3_IRQHandler(void)
{
//    if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断
//    {
//        LWipTime+=10;
//        TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清除中断标志位
//    }
}

/*-------------------------------------------------*/
/*函数名：定时器4中断服务函数                      */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
extern __IO uint8_t NB_Timer_Start;
extern uint16_t NB_WaitTime;
extern uint8_t NB_WaitTime_Reach;
void TIM4_IRQHandler(void)  //20ms一次
{
//#if ((COMMUNICATION_Function_Select == 1) || (COMMUNICATION_Function_Select == NBIOT_CHINATELECOM) || (COMMUNICATION_Function_Select == GPRS_TCP) || (COMMUNICATION_Function_Select == NBIOT_UDP))
//    static uint8_t Times=0;
//    static uint8_t T_nS=0;
//    static uint8_t T_mS=0;
//    static uint16_t T_GPRS_S=0;
//    static uint8_t T_GprsCall_S=0;
//    static uint8_t T_GprsCall_ms=0;
//#endif
//#if (PARAM_Function_Config_Enable == FUNCTION_ENABLE)
//    static uint8_t T_S=0;
//#endif

//    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)  //检查TIM4更新中断发生与否
//    {
//        GIO_callBack();
//        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);     //清除TIMx更新中断标志

//        BJZJ_Original_RcCntTime();
//        #if ((COMMUNICATION_Function_Select == NBIOT_ONENET) || (COMMUNICATION_Function_Select == NBIOT_CHINATELECOM) || (COMMUNICATION_Function_Select == NBIOT_UDP))
//        /*  用于AT指令的返回计时  */
//        //开始计数
//        if(NB_Timer_Start==1)
//        {
//            if(++Times>=50)//1s
//            {
//                Times=0;
//                if(++T_nS >= NB_WaitTime)
//                {
//                    T_nS=0;
//                    NB_WaitTime_Reach=1;
//                }
//            }
//        }
//        else
//        {
//            Times=0;
//            T_nS=0;
//        }
//        if(++T_mS >=5)
//        {
//            T_mS = 0;
//            NB_Module.RunInterval = 1;
//        }
//        #endif

//        #if (COMMUNICATION_Function_Select == GPRS_TCP)
//        //开始计数
//        if(NB_Timer_Start==1)
//        {
//            if(++Times>=50)//1s
//            {
//                Times=0;
//                if(++T_nS >= NB_WaitTime)
//                {
//                    T_nS=0;
//                    NB_WaitTime_Reach = 1;
//                }
//            }
//        }
//        else
//        {
//            Times=0;
//            T_nS=0;
//        }
//        //GPRS任务进入间隔-200mS
//        if(++T_mS >= 10)
//        {
//            T_mS = 0;
//            GPRS_Module.RunInterval = 1;
//        }
//        if(++T_GPRS_S >= 3000) //60s  一分钟
//        {
//            T_GPRS_S = 0;
//            GPRS_Module.CheckInterval = 1;
//        }
//        //电话呼叫超时计时
//        if(GSMCall_Timer_Start == 1)
//        {
//            if(++T_GprsCall_S >= 50)//1s
//            {
//                T_GprsCall_S = 0;
//                if(++T_GprsCall_ms >= GSMCall_WaitTime)
//                {
//                    T_GprsCall_ms=0;
//                    GSMCall_WaitTime_Reach=1;
//                }
//            }
//        }
//        else
//        {
//            T_GprsCall_S=0;
//            T_GprsCall_ms=0;
//        }
//        #endif


//        #if (PARAM_Function_Config_Enable == FUNCTION_ENABLE)
//        if(Config_TimeStart == true)
//        {
//            if(++T_S>=150)//3s
//            {
//                T_S=0;
//                Config_TimeStart = false;
//                Config_TimeOut = true;
//            }
//        }
//        else
//        {
//            T_S = 0;
//        }
//        #endif
//    }
}

/*-------------------------------------------------*/
/*函数名：以太网中断处理函数                       */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
 #if ((COMMUNICATION_Function_Select == ETHERNET_TCP) || (COMMUNICATION_Function_Select == GPRS_TCP))
extern void LwIP_Pkt_Handle(void);

//void ETH_IRQHandler(void)
//{
//    if(ETH_RevFlg == true)
//    {
//        return;
//    }
//  //检测是否收到数据包
//  while(ETH_CheckFrameReceived())
//  {
//    LwIP_Pkt_Handle();//接收数据函数
//  }

//  ETH_DMAClearITPendingBit(ETH_DMA_IT_R);     //清除DMA中断标志位
//  ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS);   //清除DMA接收中断标志位
//}
#endif
///*-------------------------------------------------*/
///*函数名：串口中断函数                            */
///*参  数：无                                       */
///*返回值：无                                       */
///*-------------------------------------------------*/
//void USART6_IRQHandler(void)   //串口中断服务程序----蓝牙口通信
//{
////    u8   res;
////    u16  dataLen;

////    if(USART_GetITStatus(BLUE_UART, USART_IT_RXNE) != RESET)  //接收中断
////    {
////        res = USART_ReceiveData(BLUE_UART);
////        USART_ClearFlag(BLUE_UART,USART_FLAG_RXNE);

////        if(BLUE_RX_LEN>=BLUE_RXBUFF_SIZE)
////        {
////            BLUE_RX_LEN=0;
////        }
////        if (BLUE_RX_STATE == BLUE_STATE_NORECV)
////        {
////            if (res == BLUE_PRO_HEAD)
////            {
////                BLUE_RX_BUF[BLUE_RX_LEN++] = res;
////                if (BLUE_RX_LEN == 2)
////                {
////                    BLUE_RX_STATE = BLUE_STATE_HEADRECV;
////                    (void)StartTime10ms(COMM_TIM_BLUE, BLUE_DELAY_10MS, Blue_callback);
////                }
////            }
////            else
////            {
////                BLUE_RX_LEN = 0;
////            }
////        }
////        else if (BLUE_RX_STATE == BLUE_STATE_HEADRECV)
////        {
////            (void)StartTime10ms(COMM_TIM_BLUE, BLUE_DELAY_10MS, Blue_callback);
////            BLUE_RX_BUF[BLUE_RX_LEN++]=res;
////            dataLen = (BLUE_RX_BUF[BLUE_LEN_INDEX]<<8) + BLUE_RX_BUF[BLUE_LEN_INDEX+1];

////            if ( (BLUE_RX_LEN >= dataLen + 6) && (dataLen >= 3) )
////            {
////                if ( (BLUE_RX_BUF[BLUE_RX_LEN-2] == BLUE_PRO_END) &&
////                     (BLUE_RX_BUF[BLUE_RX_LEN-1] == BLUE_PRO_END) )
////                {
////                    BLUE_RX_STATE = BLUE_STATE_RRAMERECV;
////                    (void)CloseTime10ms(COMM_TIM_BLUE);
////                }
////            }
////        }
////    }
//}

/*-------------------------------------------------*/
/*函数名：串口接收处理函数                        */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void USART2_IRQHandler(void)          //串口中断服务程序-----报警主机通信
{
//    u8 res = 0;
//    if(USART_GetITStatus(COM_UART, USART_IT_RXNE) != RESET)  //接收中断
//    {
//        res = USART_ReceiveData(COM_UART);
//        USART_ClearFlag(COM_UART,USART_FLAG_RXNE);
//        
//        if(Queue_IsFull() == false)
//        {
//            COM_RX_BUf.Data[COM_RX_BUf.Head++] = res;
//            COM_RX_BUf.Head = COM_RX_BUf.Head % COM_RXBUFF_SIZE;
//        }
//        g_COMRecvFlag = 1;//有数据接收时置位1,3S没有接收到数据置位0，为0时不会进入数据解析函数
//        g_COMRecvState = COM_STATE_RRAMERECV;
//        #if (Printer_Serial_Type != PRINTER_Serial_GST5000_485)
//        g_3sTime = LWipTime;
//        #endif
//        //报警主机原始数据
//        BJZJ_Original_RevData(res);
//    }
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
