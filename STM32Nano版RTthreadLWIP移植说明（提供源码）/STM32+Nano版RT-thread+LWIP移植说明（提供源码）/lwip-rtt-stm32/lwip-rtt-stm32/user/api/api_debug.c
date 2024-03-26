#include "api_debug.h"
#include "drv_usart.h"
#include <rthw.h>

//DEBUG串口句柄
static pUsart_Handle_Typedef pDebugUsart_Handle = NULL;


#if defined(RT_USING_CONSOLE)

/*==================================================================
* Function      :rt_hw_console_output
* Description   :控制台输出
* Input Para    :str:输出字符串
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

//控制台输入消息队列
static rt_mq_t gConsoleMq = RT_NULL;

/*==================================================================
* Function      :DebugLowTxCallback
* Description   :调试口底层发送回调函数
* Input Para    :None
* Output Para   :None
* Return Value  :None
==================================================================*/
static void DebugLowTxCallback(void)
{

}

/*==================================================================
* Function      :DebugLowRxCallback
* Description   :调试口底层接收回调
* Input Para    :rxaddress :DMA接收偏移后的地址
* Output Para   :None
* Return Value  :None
==================================================================*/
static void DebugLowRxCallback(uint8_t* data,uint16_t len)
{
    if((gConsoleMq == NULL) || (len == 0))
    {
        return;
    }
    rt_mq_send(gConsoleMq,       /* 写入（接收）队列的ID(句柄) */
               data,             /* 写入（接收）的数据 */
               1);   
}

/*==================================================================
* Function      :rt_hw_console_getchar
* Description   :控制台输入
* Input Para    :None
* Output Para   :None
* Return Value  :-1:接收失败 其他:接收到的字符
==================================================================*/
int rt_hw_console_getchar(void)
{
    uint8_t data = 0;
    rt_err_t uwRet = RT_EOK;
    //用于接收消息队列数据
    /* 队列读取（接收），等待时间为一直等待 */
    uwRet = rt_mq_recv(gConsoleMq,            /* 读取（接收）队列的ID(句柄) */
                        &data,                /* 读取（接收）的数据保存位置 */
                        1,                    /* 读取（接收）的数据的长度 */
                        RT_WAITING_FOREVER);  /* 等待时间：一直等 */
    if (RT_EOK == uwRet)
    {
        return data;
    }
    return -1;
}

int ConsoleEnvInit(void)
{
    gConsoleMq = rt_mq_create("debugmq",            /* 消息队列名字 */
                            1,                      /* 消息的最大长度 */
                            256,                    /* 消息队列的最大容量 */
                            RT_IPC_FLAG_FIFO);      /* 队列模式 FIFO(0x00)*/
    if (gConsoleMq != RT_NULL)
    {
        rt_kprintf("控制台消息队列创建成功！\r\n");
    }
    else
    {      
        return -1;
    }
    //设置串口底层收发回调函数
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
* Description   :调试口初始化
* Input Para    :None
* Output Para   :None
* Return Value  :结果
==================================================================*/
static int DebugInit(void)
{
    //串口初始化
    UartDriverRegister(TYPE_USART6);
    pDebugUsart_Handle = UartGetHanle(TYPE_USART6);
    if(NULL != pDebugUsart_Handle)
    {
        pDebugUsart_Handle->GpioInit();
        pDebugUsart_Handle->ConfigInit(BAUDRATE115200,LEN8BIT,STOPBIT1,PARITY_NONE);//包含uart_deinit
        pDebugUsart_Handle->NvicInit();
    }
    return RT_EOK;
}
INIT_BOARD_EXPORT(DebugInit);
