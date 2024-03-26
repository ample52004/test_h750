#include "drv_usart.h"

//���ڲ������б�
const static uint32_t gBaudList[] = {1200,2400,4800,7200,9600,14400,19200,38400,57600,115200};

//����6
//����6���
static Usart_Handle_Typedef Usart6_Handle = {0};
//����6������ɻص�
//static UsartTxFinishCallBack pUsart6TxHandler = NULL;
//����6������ɻص�
static UsartRxFinishCallBack pUsart6RxHandler = NULL;
//����6�շ�������
//static uint8_t* pUsart6TxBuff;
//static uint8_t* pUsart6RxBuff;
//static uint16_t Usart6TxLen;
//static uint16_t Usart6RxLen;

/*==================================================================
* Function      :Usart6SendBuff
* Description   :����6��������
* Input Para    :TxBuff ����  TxSize ����
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
* Description   :����6���͵��ֽ�
* Input Para    :c ���ֽ�����
* Output Para   :None
* Return Value  :None
==================================================================*/
static void Usart6SendData(uint8_t ch)
{
    /* дһ���ֽڵ�USART6 */
    USART_SendData(USART6, (uint8_t) ch);

    /* �ȴ����ͽ��� */
    while (USART_GetFlagStatus(USART6, USART_FLAG_TC) == RESET)
    {}    
}

/*-------------------------------------------------*/
/*�������������жϺ���                            */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void USART6_IRQHandler(void) 
{
    u8   Data;
    u16  DataLen;

    if(USART_GetITStatus(USART6_UART, USART_IT_RXNE) != RESET)//�����ж�
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
* Description   :����6���ȼ���ʼ��
* Input Para    :None
* Output Para   :None
* Return Value  :None
==================================================================*/
static void Usart6NvicInit(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    u8 IRQChannel = USART6_IRQn;
    
    NVIC_InitStructure.NVIC_IRQChannel = IRQChannel;        //����6�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0; //��ռ���ȼ�6
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;       //�����ȼ�6
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //�ж�ͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);                         //������������
    USART_ITConfig(USART6_UART, USART_IT_RXNE, ENABLE);   
}

/*==================================================================
* Function      :Usart6ErrIrqCallback
* Description   :����6�����жϻص�������
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
* Description   :����6DMA��ʼ��
* Input Para    :TxBuff:DMA����buff TxSize:���ʹ�С RxBuff:����buff RxSize:���մ�С
* Output Para   :None
* Return Value  :None
==================================================================*/
static void Usart6DmaInit(uint8_t *TxBuff,uint16_t TxSize,uint8_t *RxBuff,uint16_t RxSize)
{
    
    
}

/*==================================================================
* Function       :Usart6TxDmaBtcIrqCallback
* Description    :DMA������ɻص�
* Input Para     :��
* Output Para    :��
* Return Value   :��
==================================================================*/
//static void Usart6TxDmaBtcIrqCallback(void)
//{
//    
//}

/*==================================================================
* Function      :Usart6ConfigInit
* Description   :����6���ó�ʼ��
* Input Para    :Rate:������ Len:����λ Stop:ֹͣλ Parity:У��λ
* Output Para   :None
* Return Value  :None
==================================================================*/
static void Usart6ConfigInit(ENUM_USART_BAUDRATE Rate,ENUM_USART_LEN Len,ENUM_USART_STOP Stop,ENUM_USART_PARITY Parity)
{
    USART_InitTypeDef USART_InitStructure;

    uint16_t TempWordLengthValue = 0,TempStopBitsValue = 0,TempParityValue = 0;

    //����λ����
    if(Len == LEN8BIT)
    {
       TempWordLengthValue = USART_WordLength_8b;
    }
    else if(Len == LEN9BIT)
    {
       TempWordLengthValue = USART_WordLength_9b;
    }
    //ֹͣλ
    if(Stop == STOPBIT1)
    {
       TempStopBitsValue = USART_StopBits_1;
    }
    else if(Stop == STOPBIT2)
    {
       TempStopBitsValue = USART_StopBits_2;
    }
    //��żУ��
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
    
    USART_InitStructure.USART_BaudRate = gBaudList[Rate];                          //����������
    USART_InitStructure.USART_WordLength = TempWordLengthValue;                    //�ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = TempStopBitsValue;                        //һ��ֹͣλ
    USART_InitStructure.USART_Parity = TempParityValue;                            //����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                //�շ�ģʽ
    USART_Init(USART6, &USART_InitStructure);                                      //���������ô���6
    
    USART_Cmd(USART6, ENABLE);                                                     //ʹ�ܴ��� 6
    USART_ClearFlag(USART6, USART_FLAG_TC);                                        //�����־λ    
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
* Description    :DMA������ɻص�
* Input Para     :��
* Output Para    :��
* Return Value   :��
==================================================================*/
//static void Usart6RxDmatcIrqCallback(void)
//{
//    
//}

/*==================================================================
* Function      :Usart6DmaStartRx
* Description   :����6DMA���¿�������
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
* Description   :����6�ܽų�ʼ��
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
* Description   :���ô���6�ص�����
* Input Para    :tx:������ɻص����� rx:������ɻص�����
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
* Description   :��������ע��
* Input Para    :UsartType ��������
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
* Description   :���ھ��
* Input Para    :UsartType ��������
* Output Para   :None
* Return Value  :���ھ��
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
        return NULL;//�������δע�᷵��NULL����
    }
}

