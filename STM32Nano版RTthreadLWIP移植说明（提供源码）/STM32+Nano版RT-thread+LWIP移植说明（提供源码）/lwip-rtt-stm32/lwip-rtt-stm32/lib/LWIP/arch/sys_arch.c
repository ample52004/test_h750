/****************************************************************************************
* FILE         : sys_arch.c
* Description  : 操作系统抽象层：主要为协议栈提供邮箱、信号量等机制（此文件为协议栈的一部分）
*                内核线程创建、内核保护
* Copyright (c) 2022 by Tpson. All Rights Reserved.
*
* History:
* Version      Name            Date            Description
   0.1       zhongchao        2020/04/25      Initial Version

****************************************************************************************
****************************************************************************************/
/* lwIP includes. */
#include "lwip/debug.h"
#include "lwip/def.h"
#include "lwip/sys.h"
#include "lwip/mem.h"
#include "public_define.h"


/*-----------------------------LWIP时间计数-------------------------------------*/
#if NO_SYS
//为LWIP提供计时
/*extern */uint32_t lwip_localtime;//lwip本地时间计数器,单位:ms
u32_t sys_now(void){
	return lwip_localtime;
}
#else
//线程控制块指针
rt_thread_t TcpIpThread = RT_NULL;

//用于对象容器名称计数
static uint32_t gMboxCount=0;		

/*------------------------IPC邮箱实现-------------------------------------------*/
     
/*==================================================================
* Function      : sys_mbox_new
* Description   : 创建一个消息邮箱
* Input Para    : *mbox:消息邮箱 size:邮箱大小
* Output Para   : none
* Return Value  : ERR_OK,创建成功 其他,创建失败
==================================================================*/
err_t sys_mbox_new( sys_mbox_t *mbox, int size)
{
    #if USE_RTOS_TYPE == RTOS_RT_THREAD
	char s1[]="RT_MBX";
	char s2=gMboxCount+'0';
	char *s3=s1+s2;
	 gMboxCount++;	
	*mbox = rt_mb_create(s3,(rt_ubase_t)size,RT_IPC_FLAG_PRIO);
	if(*mbox == NULL) {
	  return ERR_MEM;
	}
    #endif
	return ERR_OK;
}

/*==================================================================
* Function      : sys_mbox_free
* Description   : 释放并删除一个消息邮箱
* Input Para    : *mbox:要删除的消息邮箱
* Output Para   : none
* Return Value  : none
==================================================================*/
void sys_mbox_free(sys_mbox_t * mbox)
{
    #if USE_RTOS_TYPE == RTOS_RT_THREAD
    rt_mb_delete(*mbox);
    #endif
}

/*==================================================================
* Function      : sys_mbox_post
* Description   : 向消息邮箱中发送一条消息(必须发送成功)
* Input Para    : *mbox:消息邮箱 *msg:要发送的消息
* Output Para   : none
* Return Value  : none
==================================================================*/
void sys_mbox_post(sys_mbox_t *mbox,void *msg)
{  
    #if USE_RTOS_TYPE == RTOS_RT_THREAD  
    //由于RTThread邮箱大小为4个字节，所以这里传递的是指针指向的地址
    while(ERR_OK != rt_mb_send_wait(*mbox, (rt_uint32_t)msg, RT_WAITING_FOREVER));  
    #endif
}

/*==================================================================
* Function      : sys_mbox_trypost
* Description   : 尝试向一个消息邮箱发送消息
* 此函数相对于sys_mbox_post函数只发送一次消息，发送失败后不会尝试第二次发送
* Input Para    : *mbox:消息邮箱 *msg:要发送的消息
* Output Para   : none
* Return Value  : ERR_OK,发送OK ERR_MEM,发送失败
==================================================================*/
err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{ 
    #if USE_RTOS_TYPE == RTOS_RT_THREAD  
    rt_base_t ret;
    //由于RTThread邮箱大小为4个字节，所以这里传递的是指针指向的地址
    ret = rt_mb_send(*mbox, (rt_uint32_t)msg);
    if (ret == RT_EOK) {
        return ERR_OK;
    } else {
    return ERR_MEM;
    }
    #endif
}

/*==================================================================
* Function      : sys_arch_mbox_fetch
* Description   : 等待邮箱中的消息
* Input Para    : *mbox:消息邮箱 *timeout:超时时间
* Output Para   : *msg:消息
* Return Value  : 等待消息所用的时间/SYS_ARCH_TIMEOUT
==================================================================*/
u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{ 
    #if USE_RTOS_TYPE == RTOS_RT_THREAD  
    rt_base_t ret;
    void *msg_dummy;

    if (!msg) 
    {
        msg = &msg_dummy;
    }

    if (!timeout) 
    {
        /* wait infinite */
        ret = rt_mb_recv(*mbox, (rt_uint32_t*)msg, RT_WAITING_FOREVER);
    }
    else 
    {
        rt_tick_t timeout_ticks = timeout;
        ret = rt_mb_recv(*mbox, (rt_uint32_t*)msg, timeout_ticks);
        if (ret != RT_EOK) 
        {
            /* timed out */
            *msg = NULL;
            return SYS_ARCH_TIMEOUT;
        }
    }
    #endif
    /* Old versions of lwIP required us to return the time waited.
     This is not the case any more. Just returning != SYS_ARCH_TIMEOUT
     here is enough. */
    return 1;   
}

/*==================================================================
* Function      : sys_arch_mbox_tryfetch
* Description   : 尝试获取消息
* Input Para    : *mbox:消息邮箱 
* Output Para   : *msg:消息
* Return Value  : 等待消息所用的时间/SYS_ARCH_TIMEOUT
==================================================================*/
u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
    #if USE_RTOS_TYPE == RTOS_RT_THREAD  
    rt_base_t ret;
    void *msg_dummy;

    if (!msg) 
    {
        msg = &msg_dummy;
    }

    ret = rt_mb_recv(*mbox, (rt_uint32_t*)&(*msg), 0);
    if (ret != RT_EOK) 
    {
        *msg = NULL;
        return SYS_MBOX_EMPTY;
    }

    /* Old versions of lwIP required us to return the time waited.
     This is not the case any more. Just returning != SYS_ARCH_TIMEOUT
     here is enough. */
    #endif
    return 1;  
}

/*==================================================================
* Function      : sys_mbox_valid
* Description   : 检查一个消息邮箱是否有效
* Input Para    : *mbox:消息邮箱 
* Output Para   : none
* Return Value  : 1,有效.0,无效
==================================================================*/
int sys_mbox_valid(sys_mbox_t *mbox)
{ 
    #if USE_RTOS_TYPE == RTOS_RT_THREAD      
    if (*mbox == NULL)
        return 0;
    else
        return 1; 
    #endif
}

/*==================================================================
* Function      : sys_mbox_set_invalid
* Description   : 设置一个消息邮箱为无效
* Input Para    : *mbox:消息邮箱 
* Output Para   : none
* Return Value  : none
==================================================================*/
void sys_mbox_set_invalid(sys_mbox_t *mbox)
{
    #if USE_RTOS_TYPE == RTOS_RT_THREAD 
    *mbox = NULL;
    #endif
}

/*----------------------------IPC信号量实现---------------------------------*/
//用于对象容器名称计数
static uint32_t gSemCount=0;		
     
/*==================================================================
* Function      : sys_sem_new
* Description   : 创建一个信号量
* Input Para    : *sem:创建的信号量 count:信号量值
* Output Para   : none
* Return Value  : ERR_OK,创建OK ERR_MEM,创建失败
==================================================================*/
err_t sys_sem_new(sys_sem_t * sem, u8_t count)
{ 
    #if USE_RTOS_TYPE == RTOS_RT_THREAD          
	char s1[]="RT_SEM";
	char s2=gSemCount+'0';
	char *s3=s1+s2;
	gSemCount++;
	
    *sem = rt_sem_create(s3,count,RT_IPC_FLAG_PRIO);
    if(*sem == NULL) 
    {
        return ERR_MEM;
    }
    #endif
    return ERR_OK;   
}

/*==================================================================
* Function      : sys_arch_sem_wait
* Description   : 等待一个信号量
* Input Para    : *sem:要等待的信号量 timeout:超时时间
* Output Para   : none
* Return Value  : 等待信号量所用的时间/SYS_ARCH_TIMEOUT
==================================================================*/
u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{ 
    #if USE_RTOS_TYPE == RTOS_RT_THREAD  
    rt_base_t ret;

    if(!timeout) 
    {
        /* wait infinite */
        ret = rt_sem_take(*sem, RT_WAITING_FOREVER);
    } 
    else 
    {
        rt_tick_t timeout_ticks = timeout;
        ret = rt_sem_take(*sem, timeout_ticks);
        if (ret != RT_EOK) 
        {
            /* timed out */
            return SYS_ARCH_TIMEOUT;
        }
    }
    /* Old versions of lwIP required us to return the time waited.
     This is not the case any more. Just returning != SYS_ARCH_TIMEOUT
     here is enough. */
    #endif
    return 1;
    
}

/*==================================================================
* Function      : sys_sem_signal
* Description   : 发送一个信号量
* Input Para    : sem:信号量指针
* Output Para   : none
* Return Value  : none
==================================================================*/
void sys_sem_signal(sys_sem_t *sem)
{
    #if USE_RTOS_TYPE == RTOS_RT_THREAD  
    rt_sem_release(*sem); 
    #endif    
}

/*==================================================================
* Function      : sys_sem_free
* Description   : 释放并删除一个信号量
* Input Para    : sem:信号量指针
* Output Para   : none
* Return Value  : none
==================================================================*/
void sys_sem_free(sys_sem_t *sem)
{
    #if USE_RTOS_TYPE == RTOS_RT_THREAD    
    rt_sem_delete(*sem);
    #endif   
}

/*==================================================================
* Function      : sys_sem_valid
* Description   : 查询一个信号量的状态,无效或有效
* Input Para    : sem:信号量指针
* Output Para   : none
* Return Value  : 1,有效.0,无效
==================================================================*/
int sys_sem_valid(sys_sem_t *sem)
{
    #if USE_RTOS_TYPE == RTOS_RT_THREAD    
    if (*sem == NULL)
        return 0;
    else
        return 1;
    #endif    
}

/*==================================================================
* Function      : sys_sem_set_invalid
* Description   : 设置一个信号量无效
* Input Para    : sem:信号量指针
* Output Para   : none
* Return Value  : none
==================================================================*/
void sys_sem_set_invalid(sys_sem_t *sem)
{
    #if USE_RTOS_TYPE == RTOS_RT_THREAD
    *sem = NULL;
    #endif
}

/*----------------------------IPC互斥量---------------------------------*/
#if LWIP_COMPAT_MUTEX == 0  //不起用互斥量时会使用信号量
uint32_t gMutexCount=0;		//用于对象容器名称计数

/*==================================================================
* Function      : sys_mutex_new
* Description   : 创建一个新的互斥锁
* Input Para    : mutex 指向要创建的互斥锁
* Output Para   : none
* Return Value  : 如果成功则为ERR_OK，否则为另一个错误
==================================================================*/
err_t sys_mutex_new(sys_mutex_t *mutex)
{
    #if USE_RTOS_TYPE == RTOS_RT_THREAD    
	char s1[]="RT_MUTEX";
	char s2=gMutexCount+'0';
	char *s3=s1+s2;
	gMutexCount++;

	*mutex = rt_mutex_create(s3,RT_IPC_FLAG_PRIO);
	if(*mutex == NULL) {
		return ERR_MEM;
	}
    #endif    
	return ERR_OK;
}

/*==================================================================
* Function      : sys_mutex_free
* Description   : 取消分配互斥锁
* Input Para    : mutex 要删除的互斥锁
* Output Para   : none
* Return Value  : none
==================================================================*/
void sys_mutex_free(sys_mutex_t *mutex)
{
    #if USE_RTOS_TYPE == RTOS_RT_THREAD    
	rt_mutex_delete(*mutex);
    #endif
}

/*==================================================================
* Function      : sys_mutex_lock
* Description   : 阻止线程，直到可以抓住互斥锁
* Input Para    : mutex 锁定的互斥锁
* Output Para   : none
* Return Value  : none
==================================================================*/
void sys_mutex_lock(sys_mutex_t *mutex)
{
    #if USE_RTOS_TYPE == RTOS_RT_THREAD    
    rt_mutex_take(*mutex, RT_WAITING_FOREVER);
    #endif
}

/*==================================================================
* Function      : sys_mutex_unlock
* Description   : 释放先前通过'sys_mutex_lock（）'锁定的互斥锁
* Input Para    : mutex 解锁互斥锁
* Output Para   : none
* Return Value  : none
==================================================================*/
void sys_mutex_unlock(sys_mutex_t *mutex)
{
    #if USE_RTOS_TYPE == RTOS_RT_THREAD    
    rt_mutex_release(*mutex);
    #endif
}
#endif

/*----------------------------保护内存申请等---------------------------------*/
#if SYS_LIGHTWEIGHT_PROT == 1
//强烈建议在lwipopts.h将该宏打开，
//SYS_LIGHTWEIGHT_PROT宏实现可以用于保护内核申请内存时通过互斥量保护，
//可以有效避免由于内存申请出现的错误。
#if USE_RTOS_TYPE == RTOS_RT_THREAD    
rt_mutex_t gLwipSysMutex = RT_NULL;			//内存保护
#endif

/*==================================================================
* Function      : sys_init
* Description   : arch初始化
* Input Para    : none
* Output Para   : none
* Return Value  : none
==================================================================*/
void sys_init(void)
{ 
    #if USE_RTOS_TYPE == RTOS_RT_THREAD        
    gLwipSysMutex = rt_mutex_create("RT_MUTEX_PRO",RT_IPC_FLAG_PRIO);
    #endif
} 

/*==================================================================
* Function      : sys_arch_protect
* Description   : 临界区执行“快速”保护
* Input Para    : none
* Output Para   : none
* Return Value  : 旧保护级别在变量“lev”中返回
==================================================================*/
sys_prot_t sys_arch_protect(void)
{
    #if USE_RTOS_TYPE == RTOS_RT_THREAD    
	rt_mutex_take(gLwipSysMutex, RT_WAITING_FOREVER);
    #endif
	return 1;
}

/*==================================================================
* Function      : sys_arch_unprotect
* Description   : 退出临界区保护
* Input Para    : 保护级别设置为“lev”
* Output Para   : none
* Return Value  : none
==================================================================*/
void sys_arch_unprotect(sys_prot_t pval)
{
	LWIP_UNUSED_ARG(pval);
    #if USE_RTOS_TYPE == RTOS_RT_THREAD  	
	rt_mutex_release(gLwipSysMutex);
    #endif
}

/*==================================================================
* Function      : sys_thread_new
* Description   : 创建一个新进程
* Input Para    : *name:进程名称 thred:进程任务函数 *arg:进程任务函数的参数 stacksize:进程任务的堆栈大小 prio:进程任务的优先级
* Output Para   : none
* Return Value  : 线程ID,也就是任务优先级
==================================================================*/
sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio)
{
    //创建TCP IP内核任务，其他任务必须通过操作系统本身接口创建
    if(strcmp(name,TCPIP_THREAD_NAME)==0)
	{ 
        #if USE_RTOS_TYPE == RTOS_RT_THREAD  	
        //线程控制块指针
        rt_thread_t TcpIpThread = RT_NULL;
        //使用动态内存，返回控制块指针
        TcpIpThread = rt_thread_create(
                     name,                  /* 线程名字 */
                     thread,                /* 线程入口函数 */
                     arg,                   /* 线程入口函数参数 */
                     stacksize,             /* 线程栈大小 */
                     prio,                  /* 线程的优先级 */
                     20);                   /* 线程时间片 */
        if(TcpIpThread != RT_NULL)
        {     
            rt_thread_startup(TcpIpThread); /* 启动线程，开启调度 */             
            rt_kprintf("TCPIP线程创建成功！\r\n");
        }
        #endif
    }
    return prio;    
}
#endif

/*==================================================================
* Function      : sys_now
* Description   : 获取系统时间,LWIP1.4.1增加的函数
* Input Para    : none
* Output Para   : none
* Return Value  : 当前系统时间(单位:毫秒)
==================================================================*/
u32_t sys_now(void)
{
    #if USE_RTOS_TYPE == RTOS_RT_THREAD
    return rt_tick_get(); 
    #endif    
}
#endif
