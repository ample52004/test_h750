/* RT-Thread config file */

#ifndef __RTTHREAD_CFG_H__
#define __RTTHREAD_CFG_H__

#include "public_define.h"


#define RT_USING_FINSH

// <<< Use Configuration Wizard in Context Menu >>>
// <h>Basic Configuration
// <o>Maximal level of thread priority <8-256>
//  <i>Default: 32
#define RT_THREAD_PRIORITY_MAX  32//表示RT-Thread 支持多少个优先级
// <o>OS tick per second
//  <i>Default: 1000   (1ms)
#define RT_TICK_PER_SECOND  1000 //表示操作系统每秒钟有多少个tick
// <o>Alignment size for CPU architecture data access
//  <i>Default: 4
#define RT_ALIGN_SIZE   4 //表示CPU 处理的数据需要多少个字节对齐
// <o>the max length of object name<2-16>
//  <i>Default: 8
#define RT_NAME_MAX    20 //表示内核对象名字的最大长度
// <c1>Using RT-Thread components initialization
//  <i>Using RT-Thread components initialization
#define RT_USING_COMPONENTS_INIT //使用RT-Thread 组件初始化，默认使能
// </c>

#define RT_USING_USER_MAIN //使用用户main 函数，默认打开。

// <o>the stack size of main thread<1-4086>
//  <i>Default: 512
#define RT_MAIN_THREAD_STACK_SIZE     1600 //main 线程栈大小


// </h>

// <h>Debug Configuration //调试配置
// <c1>enable kernel debug configuration
//  <i>Default: enable kernel debug configuration
#define RT_DEBUG  //内核调试配置
// </c>
// <o>enable components initialization debug configuration<0-1>
//  <i>Default: 0
#define RT_DEBUG_INIT 0 //组件调试配置
// <c1>thread stack over flow detect
//  <i> Diable Thread stack over flow detect
#define RT_USING_OVERFLOW_CHECK  //线程栈溢出检测
// </c>
// </h>

// <h>Hook Configuration  //钩子函数配置
// <c1>using hook
//  <i>using hook
#define RT_USING_HOOK
// </c>
// <c1>using idle hook
//  <i>using idle hook
#define RT_USING_IDLE_HOOK
// </c>
// </h>

// <e>Software timers Configuration //软件定时器配置
// <i> Enables user timers
#define RT_USING_TIMER_SOFT         1
#if RT_USING_TIMER_SOFT == 0
    #undef RT_USING_TIMER_SOFT
#endif
// <o>The priority level of timer thread <0-31>
//  <i>Default: 4
#define RT_TIMER_THREAD_PRIO        4
// <o>The stack size of timer thread <0-8192>
//  <i>Default: 512
#define RT_TIMER_THREAD_STACK_SIZE  2048
// </e>

// <h>IPC(Inter-process communication) Configuration //内部通信配置
// <c1>Using Semaphore
//  <i>Using Semaphore
#define RT_USING_SEMAPHORE //信号量
// </c>
// <c1>Using Mutex
//  <i>Using Mutex
#define RT_USING_MUTEX //互斥量
// </c>
// <c1>Using Event
//  <i>Using Event
#define RT_USING_EVENT //事件
// </c>
// <c1>Using MailBox
//  <i>Using MailBox
#define RT_USING_MAILBOX //邮箱
// </c>
// <c1>Using Message Queue
//  <i>Using Message Queue
#define RT_USING_MESSAGEQUEUE //消息队列
// </c>
// </h>

// <h>Memory Management Configuration //内存管理
// <c1>Dynamic Heap Management
//  <i>Dynamic Heap Management
//#define RT_USING_MEMPOOL //表示是否使用内存池
#define RT_USING_HEAP //表示是否堆
// </c>
// <c1>using small memory
//  <i>using small memory
#define RT_USING_SMALL_MEM //表示是否使用小内存
// </c>
// <c1>using tiny size of memory
//  <i>using tiny size of memory
//#define RT_USING_TINY_SIZE //表示是否使用极小内存
// </c>
// </h>

// <h>Console Configuration  //控制台配置
// <c1>Using console
//  <i>Using console
#define RT_USING_CONSOLE
// </c>
// <o>the buffer size of console <1-1024>
//  <i>the buffer size of console
//  <i>Default: 128  (128Byte)
#define RT_CONSOLEBUF_SIZE          512

// <s>The device name for console
//  <i>The device name for console
//  <i>Default: uart2
#define RT_CONSOLE_DEVICE_NAME "uart2"
// </h>

//FINSH 配置。
#if defined(RT_USING_FINSH)
    #define FINSH_USING_MSH
    #define FINSH_USING_MSH_ONLY
    #define FINSH_USING_DESCRIPTION
    #define FINSH_USING_HISTORY
    // <h>Finsh Configuration
    // <o>the priority of finsh thread <1-7>
    //  <i>the priority of finsh thread
    //  <i>Default: 6
    #define __FINSH_THREAD_PRIORITY     5
    #define FINSH_THREAD_PRIORITY       (RT_THREAD_PRIORITY_MAX / 8 * __FINSH_THREAD_PRIORITY + 1)
    // <o>the stack of finsh thread <1-4096>
    //  <i>the stack of finsh thread
    //  <i>Default: 4096  (4096Byte)
    #define FINSH_THREAD_STACK_SIZE     1024
    // <o>the history lines of finsh thread <1-32>
    //  <i>the history lines of finsh thread
    //  <i>Default: 5
    #define FINSH_HISTORY_LINES         5

    #define FINSH_USING_SYMTAB
    // </h>
#endif

#if defined(RTE_USING_DEVICE) //设备配置
#define RT_USING_DEVICE
#define RT_USING_SERIAL
#define BSP_USING_UART2
#endif




#endif
// <<< end of configuration section >>>

