#include <rthw.h>
#include "lwip_comm.h" 
//#include "udp_demo.h" 
//#include "tcp_client_demo.h" 

/*==================================================================
* Function      :CycleTaskThreadEntry
* Description   :周期任务线程
* Input Para    :RT_NULL
* Output Para   :None
* Return Value  :None
==================================================================*/
static void CycleTaskThreadEntry(void* parameter)
{
    LwipCommInit();
//    udp_demo_test();
//    tcp_client_test();
    while (1)
    {     
//        CycSend();
//        TcpCycSend();
//        LwipPeriodicHandle();
        rt_thread_delay(10);
    }
}

/*==================================================================
* Function      :CycleTaskInit
* Description   :周期任务 app初始化
* Input Para    :None
* Output Para   :None
* Return Value  :RT_EOK
==================================================================*/
int CycleTaskInit(void)
{
    //线程控制块指针
    rt_thread_t CycleTaskThread = RT_NULL;
    //使用动态内存，返回控制块指针
    CycleTaskThread = rt_thread_create(
                 "cycle",               /* 线程名字 */
                 CycleTaskThreadEntry,  /* 线程入口函数 */
                 RT_NULL,               /* 线程入口函数参数 */
                 4096,                  /* 线程栈大小 */
                 6,                     /* 线程的优先级 */
                 20);                   /* 线程时间片 */
    if(CycleTaskThread != RT_NULL)
    {
        rt_thread_startup(CycleTaskThread); /* 启动线程，开启调度 */
        return RT_EOK;
    }
    else
    {
        return -1;
    }
}
INIT_APP_EXPORT(CycleTaskInit);
