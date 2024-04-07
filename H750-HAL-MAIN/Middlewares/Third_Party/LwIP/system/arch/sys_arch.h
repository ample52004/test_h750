#ifndef __SYS_ARCH_H__
#define __SYS_ARCH_H__
 
#include "lwip/opt.h"
 
#include "arch/cc.h"
#include <rtthread.h>
 
#if (NO_SYS != 0)
#error "NO_SYS need to be set to 0 to use threaded API"
#endif
 
#ifdef  __cplusplus
extern "C" {
#endif
 
#ifndef BYTE_ORDER
#define BYTE_ORDER LITTLE_ENDIAN
#endif
 
#define SYS_MBOX_NULL RT_NULL
#define SYS_SEM_NULL  RT_NULL
 
//typedef uint32_t sys_prot_t;
 
#define SYS_MBOX_SIZE 10
#define SYS_LWIP_TIMER_NAME "lwip_timer"
#define SYS_LWIP_MBOX_NAME "lwip_mbox"
#define SYS_LWIP_SEM_NAME "lwip_sem"
#define SYS_LWIP_MUTEX_NAME "lwip_mu"
 
typedef rt_sem_t sys_sem_t;
typedef rt_mutex_t sys_mutex_t;
typedef rt_mailbox_t  sys_mbox_t;
typedef rt_thread_t sys_thread_t;
 
#ifdef  __cplusplus
}
#endif
 
#endif /* __SYS_ARCH_H__ */
 