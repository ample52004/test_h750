#include "drv_spi.h"
#include "drv_config.h"
#include <string.h>
#include "board.h"
#include<rtthread.h>
#include<rtdevice.h>
/*定义一个SPI设备对象*/
struct rt_spi_device * spi_dev_w25q;
int rt_hw_w25Q_Init(void)
{
    rt_uint8_t dummy = 0xFF;

    rt_hw_spi_device_attach("spi4", "spi4", GPIOB, GPIO_PIN_12);

     spi_dev_w25q = (struct rt_spi_device *)rt_device_find("spi4");
     if (!spi_dev_w25q)
     {
         rt_kprintf("Can't find the spi4 device!\r\n" );
     }
    else
     {    /* configure SPI*/
         struct rt_spi_configuration cfg;
         cfg.data_width = 8;
         cfg.mode = RT_SPI_MASTER | RT_SPI_MODE_3 | RT_SPI_MSB;
         cfg.max_hz = 20 * 1000 *1000;       //设置工作大频率20MHz
         rt_spi_configure(spi_dev_w25q, &cfg);
         rt_spi_send(spi_dev_w25q,&dummy,1);//开启传输
     }
     return 0;
}
INIT_DEVICE_EXPORT(rt_hw_w25Q_Init);

