#include <rthw.h>
#include "lwip_comm.h" 
//#include "udp_demo.h" 
//#include "tcp_client_demo.h" 

/*==================================================================
* Function      :CycleTaskThreadEntry
* Description   :���������߳�
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
* Description   :�������� app��ʼ��
* Input Para    :None
* Output Para   :None
* Return Value  :RT_EOK
==================================================================*/
int CycleTaskInit(void)
{
    //�߳̿��ƿ�ָ��
    rt_thread_t CycleTaskThread = RT_NULL;
    //ʹ�ö�̬�ڴ棬���ؿ��ƿ�ָ��
    CycleTaskThread = rt_thread_create(
                 "cycle",               /* �߳����� */
                 CycleTaskThreadEntry,  /* �߳���ں��� */
                 RT_NULL,               /* �߳���ں������� */
                 4096,                  /* �߳�ջ��С */
                 6,                     /* �̵߳����ȼ� */
                 20);                   /* �߳�ʱ��Ƭ */
    if(CycleTaskThread != RT_NULL)
    {
        rt_thread_startup(CycleTaskThread); /* �����̣߳��������� */
        return RT_EOK;
    }
    else
    {
        return -1;
    }
}
INIT_APP_EXPORT(CycleTaskInit);
