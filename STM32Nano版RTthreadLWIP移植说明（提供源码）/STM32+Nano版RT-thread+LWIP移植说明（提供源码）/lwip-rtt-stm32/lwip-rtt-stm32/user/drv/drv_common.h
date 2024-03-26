#ifndef DRV_COMMON_INC
#define DRV_COMMON_INC

//MCU��������
#define STM32F207_PIN_NUM_100                       100

#define STM32F207_PIN_NUM                           STM32F207_PIN_NUM_100

#define USE_RTT
#ifdef USE_RTT
#include <rtthread.h>
//�����ж�����
#define DrvInterruputEnter()                        rt_interrupt_enter()
#define DrvInterruputLeave()                        rt_interrupt_leave()
#else
#define DrvInterruputEnter()
#define DrvInterruputLeave()
#endif

#endif

