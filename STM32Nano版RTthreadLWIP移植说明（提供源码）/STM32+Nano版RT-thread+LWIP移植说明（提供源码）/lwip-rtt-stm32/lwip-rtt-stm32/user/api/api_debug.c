#include "api_debug.h"
#include "drv_usart.h"
#include <rthw.h>

//DEBUG���ھ��
static pUsart_Handle_Typedef pDebugUsart_Handle = NULL;


#if defined(RT_USING_CONSOLE)

/*==================================================================
* Function      :rt_hw_console_output
* Description   :����̨���
* Input Para    :str:����ַ���
* Output Para   :None
* Return Value  :None
==================================================================*/
void rt_hw_console_output(const char *str)
{
    rt_enter_critical();
    pUsart_Handle_Typedef DebugUsart =  UartGetHanle(TYPE_USART6);
    if(NULL == DebugUsart)
    {
        rt_exit_critical();
        return;
    }
    uint8_t data = 0;
    while (*str!='\0')
    {
        if (*str=='\n')
        {
           data = 0x0D;
           DebugUsart->SendData(data);
        }
        DebugUsart->SendData(*str++);
    }
    rt_exit_critical();
}

//����̨������Ϣ����
static rt_mq_t gConsoleMq = RT_NULL;

/*==================================================================
* Function      :DebugLowTxCallback
* Description   :���Կڵײ㷢�ͻص�����
* Input Para    :None
* Output Para   :None
* Return Value  :None
==================================================================*/
static void DebugLowTxCallback(void)
{

}

/*==================================================================
* Function      :DebugLowRxCallback
* Description   :���Կڵײ���ջص�
* Input Para    :rxaddress :DMA����ƫ�ƺ�ĵ�ַ
* Output Para   :None
* Return Value  :None
==================================================================*/
static void DebugLowRxCallback(uint8_t* data,uint16_t len)
{
    if((gConsoleMq == NULL) || (len == 0))
    {
        return;
    }
    rt_mq_send(gConsoleMq,       /* д�루���գ����е�ID(���) */
               data,             /* д�루���գ������� */
               1);   
}

/*==================================================================
* Function      :rt_hw_console_getchar
* Description   :����̨����
* Input Para    :None
* Output Para   :None
* Return Value  :-1:����ʧ�� ����:���յ����ַ�
==================================================================*/
int rt_hw_console_getchar(void)
{
    uint8_t data = 0;
    rt_err_t uwRet = RT_EOK;
    //���ڽ�����Ϣ��������
    /* ���ж�ȡ�����գ����ȴ�ʱ��Ϊһֱ�ȴ� */
    uwRet = rt_mq_recv(gConsoleMq,            /* ��ȡ�����գ����е�ID(���) */
                        &data,                /* ��ȡ�����գ������ݱ���λ�� */
                        1,                    /* ��ȡ�����գ������ݵĳ��� */
                        RT_WAITING_FOREVER);  /* �ȴ�ʱ�䣺һֱ�� */
    if (RT_EOK == uwRet)
    {
        return data;
    }
    return -1;
}

int ConsoleEnvInit(void)
{
    gConsoleMq = rt_mq_create("debugmq",            /* ��Ϣ�������� */
                            1,                      /* ��Ϣ����󳤶� */
                            256,                    /* ��Ϣ���е�������� */
                            RT_IPC_FLAG_FIFO);      /* ����ģʽ FIFO(0x00)*/
    if (gConsoleMq != RT_NULL)
    {
        rt_kprintf("����̨��Ϣ���д����ɹ���\r\n");
    }
    else
    {      
        return -1;
    }
    //���ô��ڵײ��շ��ص�����
    if(NULL != pDebugUsart_Handle)
    {
        pDebugUsart_Handle->SetCallBack(DebugLowTxCallback,DebugLowRxCallback);
        return RT_EOK;
    }
    return -1;
}
INIT_COMPONENT_EXPORT(ConsoleEnvInit);
#endif

/*==================================================================
* Function      :DebugInit
* Description   :���Կڳ�ʼ��
* Input Para    :None
* Output Para   :None
* Return Value  :���
==================================================================*/
static int DebugInit(void)
{
    //���ڳ�ʼ��
    UartDriverRegister(TYPE_USART6);
    pDebugUsart_Handle = UartGetHanle(TYPE_USART6);
    if(NULL != pDebugUsart_Handle)
    {
        pDebugUsart_Handle->GpioInit();
        pDebugUsart_Handle->ConfigInit(BAUDRATE115200,LEN8BIT,STOPBIT1,PARITY_NONE);//����uart_deinit
        pDebugUsart_Handle->NvicInit();
    }
    return RT_EOK;
}
INIT_BOARD_EXPORT(DebugInit);
