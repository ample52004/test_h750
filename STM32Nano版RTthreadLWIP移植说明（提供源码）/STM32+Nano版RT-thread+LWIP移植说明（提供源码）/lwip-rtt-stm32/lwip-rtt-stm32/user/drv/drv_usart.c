#include "drv_usart.h"

//串口波特率列表
const static uint32_t gBaudList[] = {1200,2400,4800,7200,9600,14400,19200,38400,57600,115200};

//串口6
//串口6句柄
static Usart_Handle_Typedef Usart6_Handle = {0};
//串口6发送完成回调
//static UsartTxFinishCallBack pUsart6TxHandler = NULL;
//串口6接收完成回调
static UsartRxFinishCallBack pUsart6RxHandler = NULL;
//串口6收发缓存区
//static uint8_t* pUsart6TxBuff;
//static uint8_t* pUsart6RxBuff;
//static uint16_t Usart6TxLen;
//static uint16_t Usart6RxLen;

/*==================================================================
* Function      :Usart6SendBuff
* Description   :串口6发送数据
* Input Para    :TxBuff 数据  TxSize 长度
* Output Para   :None
* Return Value  :None
==================================================================*/
static void Usart6SendBuff(uint8_t *TxBuff,uint16_t TxSize)
{
    u16 i;

    while((USART6->SR&0X40)==0);
    for(i = 0; i < TxSize; i++)
    {
        USART6->DR = TxBuff[i];
        while((USART6->SR&0X40)==0);
    }    
}

/*==================================================================
* Function      :Usart6SendData
* Description   :串口6发送单字节
* Input Para    :c 单字节数据
* Output Para   :None
* Return Value  :None
==================================================================*/
static void Usart6SendData(uint8_t ch)
{
    /* 写一个字节到USART6 */
    USART_SendData(USART6, (uint8_t) ch);

    /* 等待发送结束 */
    while (USART_GetFlagStatus(USART6, USART_FLAG_TC) == RESET)
    {}    
}

/*-------------------------------------------------*/
/*函数名：串口中断函数                            */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void USART6_IRQHandler(void) 
{
    u8   Data;
    u16  DataLen;

    if(USART_GetITStatus(USART6_UART, USART_IT_RXNE) != RESET)//接收中断
    {
        USART_ClearFlag(USART6_UART,USART_FLAG_RXNE);
        Data = USART_ReceiveData(USART6_UART);
        DataLen = 1;
        if(NULL != pUsart6RxHandler)
        {
            pUsart6RxHandler(&Data,DataLen);
        }
    }
}

/*==================================================================
* Function      :Usart6NvicInit
* Description   :串口6优先级初始化
* Input Para    :None
* Output Para   :None
* Return Value  :None
==================================================================*/
static void Usart6NvicInit(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    u8 IRQChannel = USART6_IRQn;
    
    NVIC_InitStructure.NVIC_IRQChannel = IRQChannel;        //串口6中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0; //抢占优先级6
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;       //子优先级6
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //中断通道使能
    NVIC_Init(&NVIC_InitStructure);                         //根据配置设置
    USART_ITConfig(USART6_UART, USART_IT_RXNE, ENABLE);   
}

/*==================================================================
* Function      :Usart6ErrIrqCallback
* Description   :串口6错误中断回调处理函数
* Input Para    :None
* Output Para   :None
* Return Value  :None
==================================================================*/
//static void Usart6ErrIrqCallback(void)
//{
//    
//}

/*==================================================================
* Function      :Usart6DmaInit
* Description   :串口6DMA初始化
* Input Para    :TxBuff:DMA发送buff TxSize:发送大小 RxBuff:接收buff RxSize:接收大小
* Output Para   :None
* Return Value  :None
==================================================================*/
static void Usart6DmaInit(uint8_t *TxBuff,uint16_t TxSize,uint8_t *RxBuff,uint16_t RxSize)
{
    
    
}

/*==================================================================
* Function       :Usart6TxDmaBtcIrqCallback
* Description    :DMA发送完成回调
* Input Para     :无
* Output Para    :无
* Return Value   :无
==================================================================*/
//static void Usart6TxDmaBtcIrqCallback(void)
//{
//    
//}

/*==================================================================
* Function      :Usart6ConfigInit
* Description   :串口6配置初始化
* Input Para    :Rate:波特率 Len:数据位 Stop:停止位 Parity:校验位
* Output Para   :None
* Return Value  :None
==================================================================*/
static void Usart6ConfigInit(ENUM_USART_BAUDRATE Rate,ENUM_USART_LEN Len,ENUM_USART_STOP Stop,ENUM_USART_PARITY Parity)
{
    USART_InitTypeDef USART_InitStructure;

    uint16_t TempWordLengthValue = 0,TempStopBitsValue = 0,TempParityValue = 0;

    //数据位长度
    if(Len == LEN8BIT)
    {
       TempWordLengthValue = USART_WordLength_8b;
    }
    else if(Len == LEN9BIT)
    {
       TempWordLengthValue = USART_WordLength_9b;
    }
    //停止位
    if(Stop == STOPBIT1)
    {
       TempStopBitsValue = USART_StopBits_1;
    }
    else if(Stop == STOPBIT2)
    {
       TempStopBitsValue = USART_StopBits_2;
    }
    //奇偶校验
    if(Parity == PARITY_NONE)
    {
       TempParityValue =  USART_Parity_No;
    }
    else if(Parity == PARITY_EVEN)
    {
       TempParityValue =  USART_Parity_Even;
    }
    else if(Parity == PARITY_ODD)
    {
       TempParityValue =  USART_Parity_Odd;
    }
    
    USART_DeInit(USART6); 
    
    USART_InitStructure.USART_BaudRate = gBaudList[Rate];                          //波特率设置
    USART_InitStructure.USART_WordLength = TempWordLengthValue;                    //字长为8位数据格式
    USART_InitStructure.USART_StopBits = TempStopBitsValue;                        //一个停止位
    USART_InitStructure.USART_Parity = TempParityValue;                            //无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                //收发模式
    USART_Init(USART6, &USART_InitStructure);                                      //按配置设置串口6
    
    USART_Cmd(USART6, ENABLE);                                                     //使能串口 6
    USART_ClearFlag(USART6, USART_FLAG_TC);                                        //清除标志位    
}

/*==================================================================
* Function      :Usart6TimeoutIrqCallback
==================================================================*/
//static void Usart6TimeoutIrqCallback(void)
//{
//    
//}

/*==================================================================
* Function       :Usart6RxDmaBtcIrqCallback
* Description    :DMA发送完成回调
* Input Para     :无
* Output Para    :无
* Return Value   :无
==================================================================*/
//static void Usart6RxDmatcIrqCallback(void)
//{
//    
//}

/*==================================================================
* Function      :Usart6DmaStartRx
* Description   :串口6DMA重新开启接收
* Input Para    :None
* Output Para   :None
* Return Value  :None
==================================================================*/
//static void Usart6DmaStartRx(void)
//{
//    
//    
//}

/*==================================================================
* Function      :Usart6GpioInit
* Description   :串口6管脚初始化
* Input Para    :None
* Output Para   :None
* Return Value  :None
==================================================================*/
static void Usart6GpioInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = USART6_TX_PIN | USART6_RX_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(USART6_PORT, &GPIO_InitStructure);

    GPIO_PinAFConfig(USART6_PORT, USART6_AF_TX_PIN, USART6_AF_TYPE);
    GPIO_PinAFConfig(USART6_PORT, USART6_AF_RX_PIN, USART6_AF_TYPE);   
}

/*==================================================================
* Function      :Usart6SetCallBack
* Description   :设置串口6回调函数
* Input Para    :tx:发送完成回调函数 rx:接收完成回调函数
* Output Para   :None
* Return Value  :None
==================================================================*/
static void Usart6SetCallBack(UsartTxFinishCallBack tx,UsartRxFinishCallBack rx)
{
//    pUsart6TxHandler = tx;
    pUsart6RxHandler = rx;
}

/*==================================================================
* Function      :UartDriverRegister
* Description   :串口驱动注册
* Input Para    :UsartType 串口类型
* Output Para   :None
* Return Value  :None
==================================================================*/
void UartDriverRegister(ENUM_USART_TYPE UsartType)
{
    if(TYPE_USART6 == UsartType)
    {
        Usart6_Handle.GpioInit = Usart6GpioInit;
        Usart6_Handle.ConfigInit = Usart6ConfigInit;
        Usart6_Handle.DmaInit = Usart6DmaInit;
        Usart6_Handle.NvicInit = Usart6NvicInit;
        Usart6_Handle.SetCallBack = Usart6SetCallBack;
        Usart6_Handle.SendData = Usart6SendData;
        Usart6_Handle.SendBuff = Usart6SendBuff;
    }
}

/*==================================================================
* Function      :UartGetHanle
* Description   :串口句柄
* Input Para    :UsartType 串口类型
* Output Para   :None
* Return Value  :串口句柄
==================================================================*/
pUsart_Handle_Typedef UartGetHanle(ENUM_USART_TYPE UsartType)
{
    if(TYPE_USART6 == UsartType)
    {
        if(Usart6_Handle.GpioInit == NULL)
        {
            return NULL;
        }
        else
        {
            return &Usart6_Handle;
        }        
    }
    else
    {
        return NULL;//如果驱动未注册返回NULL错误
    }
}

