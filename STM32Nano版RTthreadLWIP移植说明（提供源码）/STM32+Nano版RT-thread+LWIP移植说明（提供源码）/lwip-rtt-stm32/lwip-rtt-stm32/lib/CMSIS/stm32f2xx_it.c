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
//        __set_FAULTMASK(1);      // �ر������ж�
//        NVIC_SystemReset();      // ��λ
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
/*�������������жϺ���                            */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
bool Usart1RevFlag = false; //��������ڵı�־
void USART1_IRQHandler(void)   //�����жϷ������-----2Gģ��ͨ��
{
//    if(USART_GetITStatus(COM_DB9_UART, USART_IT_RXNE) != RESET)  //�����ж�
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
/*����������ʱ��3�жϷ�����                      */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void TIM3_IRQHandler(void)
{
//    if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //����ж�
//    {
//        LWipTime+=10;
//        TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //����жϱ�־λ
//    }
}

/*-------------------------------------------------*/
/*����������ʱ��4�жϷ�����                      */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
extern __IO uint8_t NB_Timer_Start;
extern uint16_t NB_WaitTime;
extern uint8_t NB_WaitTime_Reach;
void TIM4_IRQHandler(void)  //20msһ��
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

//    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)  //���TIM4�����жϷ������
//    {
//        GIO_callBack();
//        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);     //���TIMx�����жϱ�־

//        BJZJ_Original_RcCntTime();
//        #if ((COMMUNICATION_Function_Select == NBIOT_ONENET) || (COMMUNICATION_Function_Select == NBIOT_CHINATELECOM) || (COMMUNICATION_Function_Select == NBIOT_UDP))
//        /*  ����ATָ��ķ��ؼ�ʱ  */
//        //��ʼ����
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
//        //��ʼ����
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
//        //GPRS���������-200mS
//        if(++T_mS >= 10)
//        {
//            T_mS = 0;
//            GPRS_Module.RunInterval = 1;
//        }
//        if(++T_GPRS_S >= 3000) //60s  һ����
//        {
//            T_GPRS_S = 0;
//            GPRS_Module.CheckInterval = 1;
//        }
//        //�绰���г�ʱ��ʱ
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
/*����������̫���жϴ�����                       */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
 #if ((COMMUNICATION_Function_Select == ETHERNET_TCP) || (COMMUNICATION_Function_Select == GPRS_TCP))
extern void LwIP_Pkt_Handle(void);

//void ETH_IRQHandler(void)
//{
//    if(ETH_RevFlg == true)
//    {
//        return;
//    }
//  //����Ƿ��յ����ݰ�
//  while(ETH_CheckFrameReceived())
//  {
//    LwIP_Pkt_Handle();//�������ݺ���
//  }

//  ETH_DMAClearITPendingBit(ETH_DMA_IT_R);     //���DMA�жϱ�־λ
//  ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS);   //���DMA�����жϱ�־λ
//}
#endif
///*-------------------------------------------------*/
///*�������������жϺ���                            */
///*��  ������                                       */
///*����ֵ����                                       */
///*-------------------------------------------------*/
//void USART6_IRQHandler(void)   //�����жϷ������----������ͨ��
//{
////    u8   res;
////    u16  dataLen;

////    if(USART_GetITStatus(BLUE_UART, USART_IT_RXNE) != RESET)  //�����ж�
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
/*�����������ڽ��մ�����                        */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void USART2_IRQHandler(void)          //�����жϷ������-----��������ͨ��
{
//    u8 res = 0;
//    if(USART_GetITStatus(COM_UART, USART_IT_RXNE) != RESET)  //�����ж�
//    {
//        res = USART_ReceiveData(COM_UART);
//        USART_ClearFlag(COM_UART,USART_FLAG_RXNE);
//        
//        if(Queue_IsFull() == false)
//        {
//            COM_RX_BUf.Data[COM_RX_BUf.Head++] = res;
//            COM_RX_BUf.Head = COM_RX_BUf.Head % COM_RXBUFF_SIZE;
//        }
//        g_COMRecvFlag = 1;//�����ݽ���ʱ��λ1,3Sû�н��յ�������λ0��Ϊ0ʱ����������ݽ�������
//        g_COMRecvState = COM_STATE_RRAMERECV;
//        #if (Printer_Serial_Type != PRINTER_Serial_GST5000_485)
//        g_3sTime = LWipTime;
//        #endif
//        //��������ԭʼ����
//        BJZJ_Original_RevData(res);
//    }
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
