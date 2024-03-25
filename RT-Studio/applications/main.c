/*
 * Copyright (c) 2006-2024, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-03-09     RT-Thread    first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include "drv_common.h"
#include "uart_config.h"
#include "stm32h7xx_hal.h"

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>
static char buf[] /*__attribute__((section(".ARM.__at_0x24000000")))*/ = "liujian test!\r\n";
uint8_t rs485_en;
static rt_err_t rs485_tx_done(rt_device_t dev, void *buffer)
{
    rt_pin_write(rs485_en, PIN_LOW);
}

int main(void)
{

    int count = 1;
    rt_err_t ret = RT_EOK;
    rt_size_t len = 0;
    rt_device_t uart1;
    uart1 = rt_device_find("uart1");
    if (uart1 == RT_NULL)
        return 0;

    ret = rt_device_open(uart1,RT_DEVICE_FLAG_RDWR| RT_DEVICE_FLAG_DMA_TX|RT_DEVICE_FLAG_DMA_RX);
    if (ret != RT_EOK)
        return 0;

    rt_device_set_tx_complete(uart1, rs485_tx_done);
/*
    conf.baud_rate = 115200;
    conf.data_bits = 8;
    conf.bufsz = 64;
    ret = rt_device_control(uart6, RT_DEVICE_CTRL_CONFIG, &conf);
    if (ret != RT_EOK)
        return 0;
*/
    while (count++)
    {

        len = rt_device_write(uart1, 0, buf, 15);
       // rt_thread_mdelay(2);

       // LOG_D("Hello RT-Thread!");
        rt_thread_mdelay(5);
    }

    return RT_EOK;
}

