#include <stdint.h>
/* RT-Thread 相关头文件 */
#include <rthw.h>
#include <rtthread.h>

#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
#define RTTHREAD_BUF_LEN          (1024*64)
static uint8_t RtthreadBuf[RTTHREAD_BUF_LEN]; //__attribute__ ((section("HEAP_RTOS")));
#define HEAP_BEGIN                (void*)(&RtthreadBuf)
#define HEAP_END                  (void*)(RtthreadBuf + RTTHREAD_BUF_LEN)
#endif

/*==================================================================
* Function	    :ClockInit
* Description	:开启各总线时钟
* Input Para	:None
* Output Para	:None
* Return Value  :None
==================================================================*/
static void ClockInit(void)
{
    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA , ENABLE);
    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB , ENABLE);
    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC , ENABLE);
    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOD , ENABLE);
    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOE , ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);
}


/*==================================================================
* Function      :rt_hw_board_init
* Description   :板级初始化
* Input Para    :None
* Output Para   :None
* Return Value  :None
==================================================================*/
void rt_hw_board_init()
{

    /*中断向量表*/
    /*开启时钟*/
    ClockInit();
    /*SysTick设置*/
    SysTick_Config( SystemCoreClock / RT_TICK_PER_SECOND );
    /* 调用组件初始化函数 */
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif

#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
    rt_system_heap_init((void*)HEAP_BEGIN, (void*)HEAP_END);
#endif

}


/*==================================================================
* Function      :SysTick_Handler
* Description   :SysTick中断
* Input Para    :None
* Output Para   :None
* Return Value  :None
==================================================================*/
void SysTick_Handler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();
    /* 更新时基 */
    rt_tick_increase();
    /* leave interrupt */
    rt_interrupt_leave();
}
