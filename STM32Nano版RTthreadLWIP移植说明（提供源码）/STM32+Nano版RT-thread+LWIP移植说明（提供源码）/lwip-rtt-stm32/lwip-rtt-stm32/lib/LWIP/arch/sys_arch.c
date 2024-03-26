/****************************************************************************************
* FILE         : sys_arch.c
* Description  : ����ϵͳ����㣺��ҪΪЭ��ջ�ṩ���䡢�ź����Ȼ��ƣ����ļ�ΪЭ��ջ��һ���֣�
*                �ں��̴߳������ں˱���
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


/*-----------------------------LWIPʱ�����-------------------------------------*/
#if NO_SYS
//ΪLWIP�ṩ��ʱ
/*extern */uint32_t lwip_localtime;//lwip����ʱ�������,��λ:ms
u32_t sys_now(void){
	return lwip_localtime;
}
#else
//�߳̿��ƿ�ָ��
rt_thread_t TcpIpThread = RT_NULL;

//���ڶ����������Ƽ���
static uint32_t gMboxCount=0;		

/*------------------------IPC����ʵ��-------------------------------------------*/
     
/*==================================================================
* Function      : sys_mbox_new
* Description   : ����һ����Ϣ����
* Input Para    : *mbox:��Ϣ���� size:�����С
* Output Para   : none
* Return Value  : ERR_OK,�����ɹ� ����,����ʧ��
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
* Description   : �ͷŲ�ɾ��һ����Ϣ����
* Input Para    : *mbox:Ҫɾ������Ϣ����
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
* Description   : ����Ϣ�����з���һ����Ϣ(���뷢�ͳɹ�)
* Input Para    : *mbox:��Ϣ���� *msg:Ҫ���͵���Ϣ
* Output Para   : none
* Return Value  : none
==================================================================*/
void sys_mbox_post(sys_mbox_t *mbox,void *msg)
{  
    #if USE_RTOS_TYPE == RTOS_RT_THREAD  
    //����RTThread�����СΪ4���ֽڣ��������ﴫ�ݵ���ָ��ָ��ĵ�ַ
    while(ERR_OK != rt_mb_send_wait(*mbox, (rt_uint32_t)msg, RT_WAITING_FOREVER));  
    #endif
}

/*==================================================================
* Function      : sys_mbox_trypost
* Description   : ������һ����Ϣ���䷢����Ϣ
* �˺��������sys_mbox_post����ֻ����һ����Ϣ������ʧ�ܺ󲻻᳢�Եڶ��η���
* Input Para    : *mbox:��Ϣ���� *msg:Ҫ���͵���Ϣ
* Output Para   : none
* Return Value  : ERR_OK,����OK ERR_MEM,����ʧ��
==================================================================*/
err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{ 
    #if USE_RTOS_TYPE == RTOS_RT_THREAD  
    rt_base_t ret;
    //����RTThread�����СΪ4���ֽڣ��������ﴫ�ݵ���ָ��ָ��ĵ�ַ
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
* Description   : �ȴ������е���Ϣ
* Input Para    : *mbox:��Ϣ���� *timeout:��ʱʱ��
* Output Para   : *msg:��Ϣ
* Return Value  : �ȴ���Ϣ���õ�ʱ��/SYS_ARCH_TIMEOUT
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
* Description   : ���Ի�ȡ��Ϣ
* Input Para    : *mbox:��Ϣ���� 
* Output Para   : *msg:��Ϣ
* Return Value  : �ȴ���Ϣ���õ�ʱ��/SYS_ARCH_TIMEOUT
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
* Description   : ���һ����Ϣ�����Ƿ���Ч
* Input Para    : *mbox:��Ϣ���� 
* Output Para   : none
* Return Value  : 1,��Ч.0,��Ч
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
* Description   : ����һ����Ϣ����Ϊ��Ч
* Input Para    : *mbox:��Ϣ���� 
* Output Para   : none
* Return Value  : none
==================================================================*/
void sys_mbox_set_invalid(sys_mbox_t *mbox)
{
    #if USE_RTOS_TYPE == RTOS_RT_THREAD 
    *mbox = NULL;
    #endif
}

/*----------------------------IPC�ź���ʵ��---------------------------------*/
//���ڶ����������Ƽ���
static uint32_t gSemCount=0;		
     
/*==================================================================
* Function      : sys_sem_new
* Description   : ����һ���ź���
* Input Para    : *sem:�������ź��� count:�ź���ֵ
* Output Para   : none
* Return Value  : ERR_OK,����OK ERR_MEM,����ʧ��
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
* Description   : �ȴ�һ���ź���
* Input Para    : *sem:Ҫ�ȴ����ź��� timeout:��ʱʱ��
* Output Para   : none
* Return Value  : �ȴ��ź������õ�ʱ��/SYS_ARCH_TIMEOUT
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
* Description   : ����һ���ź���
* Input Para    : sem:�ź���ָ��
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
* Description   : �ͷŲ�ɾ��һ���ź���
* Input Para    : sem:�ź���ָ��
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
* Description   : ��ѯһ���ź�����״̬,��Ч����Ч
* Input Para    : sem:�ź���ָ��
* Output Para   : none
* Return Value  : 1,��Ч.0,��Ч
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
* Description   : ����һ���ź�����Ч
* Input Para    : sem:�ź���ָ��
* Output Para   : none
* Return Value  : none
==================================================================*/
void sys_sem_set_invalid(sys_sem_t *sem)
{
    #if USE_RTOS_TYPE == RTOS_RT_THREAD
    *sem = NULL;
    #endif
}

/*----------------------------IPC������---------------------------------*/
#if LWIP_COMPAT_MUTEX == 0  //�����û�����ʱ��ʹ���ź���
uint32_t gMutexCount=0;		//���ڶ����������Ƽ���

/*==================================================================
* Function      : sys_mutex_new
* Description   : ����һ���µĻ�����
* Input Para    : mutex ָ��Ҫ�����Ļ�����
* Output Para   : none
* Return Value  : ����ɹ���ΪERR_OK������Ϊ��һ������
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
* Description   : ȡ�����以����
* Input Para    : mutex Ҫɾ���Ļ�����
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
* Description   : ��ֹ�̣߳�ֱ������ץס������
* Input Para    : mutex �����Ļ�����
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
* Description   : �ͷ���ǰͨ��'sys_mutex_lock����'�����Ļ�����
* Input Para    : mutex ����������
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

/*----------------------------�����ڴ������---------------------------------*/
#if SYS_LIGHTWEIGHT_PROT == 1
//ǿ�ҽ�����lwipopts.h���ú�򿪣�
//SYS_LIGHTWEIGHT_PROT��ʵ�ֿ������ڱ����ں������ڴ�ʱͨ��������������
//������Ч���������ڴ�������ֵĴ���
#if USE_RTOS_TYPE == RTOS_RT_THREAD    
rt_mutex_t gLwipSysMutex = RT_NULL;			//�ڴ汣��
#endif

/*==================================================================
* Function      : sys_init
* Description   : arch��ʼ��
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
* Description   : �ٽ���ִ�С����١�����
* Input Para    : none
* Output Para   : none
* Return Value  : �ɱ��������ڱ�����lev���з���
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
* Description   : �˳��ٽ�������
* Input Para    : ������������Ϊ��lev��
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
* Description   : ����һ���½���
* Input Para    : *name:�������� thred:���������� *arg:�����������Ĳ��� stacksize:��������Ķ�ջ��С prio:������������ȼ�
* Output Para   : none
* Return Value  : �߳�ID,Ҳ�����������ȼ�
==================================================================*/
sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio)
{
    //����TCP IP�ں����������������ͨ������ϵͳ����ӿڴ���
    if(strcmp(name,TCPIP_THREAD_NAME)==0)
	{ 
        #if USE_RTOS_TYPE == RTOS_RT_THREAD  	
        //�߳̿��ƿ�ָ��
        rt_thread_t TcpIpThread = RT_NULL;
        //ʹ�ö�̬�ڴ棬���ؿ��ƿ�ָ��
        TcpIpThread = rt_thread_create(
                     name,                  /* �߳����� */
                     thread,                /* �߳���ں��� */
                     arg,                   /* �߳���ں������� */
                     stacksize,             /* �߳�ջ��С */
                     prio,                  /* �̵߳����ȼ� */
                     20);                   /* �߳�ʱ��Ƭ */
        if(TcpIpThread != RT_NULL)
        {     
            rt_thread_startup(TcpIpThread); /* �����̣߳��������� */             
            rt_kprintf("TCPIP�̴߳����ɹ���\r\n");
        }
        #endif
    }
    return prio;    
}
#endif

/*==================================================================
* Function      : sys_now
* Description   : ��ȡϵͳʱ��,LWIP1.4.1���ӵĺ���
* Input Para    : none
* Output Para   : none
* Return Value  : ��ǰϵͳʱ��(��λ:����)
==================================================================*/
u32_t sys_now(void)
{
    #if USE_RTOS_TYPE == RTOS_RT_THREAD
    return rt_tick_get(); 
    #endif    
}
#endif
