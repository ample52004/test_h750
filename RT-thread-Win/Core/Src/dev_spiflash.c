#include "dev_spiflash.h"

/**
  * @brief spi4 读一个字节
  * @param None
  * @retval None
  */
#define cs_low      LL_GPIO_SetOutputPin(GPIOG, LL_GPIO_PIN_6)
#define cs_high     LL_GPIO_ResetOutputPin (GPIOG, LL_GPIO_PIN_6)
static uint8_t spi4_flash_read_byte(void)
{
    uint8_t *r_data;

		*r_data = LL_SPI_ReceiveData16(SPI4);
    return *r_data	;
}
/**
  * @brief spi4 写一个字节
  * @param byte 写入的字节
  * @retval 写状态 0成功 1失败
  */
static uint8_t spi4_flash_send_byte(uint8_t byte)
{
    LL_SPI_TransmitData16(SPI4, byte);
		return 0;
}
/**
  * @brief FLASH 写使能
  * @param None
  * @retval None
  */
static void spi4_flash_write_enable(void)
{
    cs_low;
    spi4_flash_send_byte(0x06);
    cs_high;
}
/**
  * @brief FLASH 等待写结束
  * @param None
  * @retval None
  */
static void spi4_flash_wait_for_write_end(void)
{
  uint8_t state = 0;

  cs_low;

  spi4_flash_send_byte(0x05);

  do
  {
      state = spi4_flash_read_byte();
  }
  while((state & 0x01) == SET);

  cs_high;
}
/**
  * @brief FLASH 读ID
  * @param None
  * @retval None
  */
uint32_t spi_flash_read_ID(void)
{
  uint32_t temp, temp0, temp1, temp2;

  cs_low;

  spi4_flash_send_byte(0x9F);

  temp0 = spi4_flash_read_byte();
  temp1 = spi4_flash_read_byte();
  temp2 = spi4_flash_read_byte();

  cs_high;

  temp = (temp0 << 16) | (temp1 << 8) | temp2;

  return temp;
}
/**
  * @brief 读FLASH
  * @param addr 读flash的起始地址
  * @param pdata 读到的数据存放起始地址
  * pdata size 读数据大小
  * @retval None
  */
void spi4_flash_read(uint32_t addr,uint8_t *pdata, uint16_t size)
{
  cs_low;

  spi4_flash_send_byte(0x03);

  spi4_flash_send_byte((addr & 0xFF0000) >> 16);
  spi4_flash_send_byte((addr & 0xFF00) >> 8);
  spi4_flash_send_byte(addr  & 0xFF);

  while (size--)
  {
    *pdata = spi4_flash_read_byte();
    pdata++;
  }
  cs_high;
}
/**
  * @brief 按页写FLASH
  * @param addr 写入flash的起始地址
  * @param pdata 写入数据的起始地址
  * pdata size 写数据大小
  * @retval None
  */
void spi4_flash_page_write(uint32_t addr, uint8_t *pdata, uint16_t size)
{
    uint16_t i;

    spi4_flash_write_enable();

    cs_low;

    spi4_flash_send_byte(0x02);
    spi4_flash_send_byte((uint8_t)((addr)>>16));
    spi4_flash_send_byte((uint8_t)((addr)>>8));
    spi4_flash_send_byte((uint8_t)addr);

    for(i = 0; i < size; i++)
    {
        spi4_flash_send_byte(pdata[i]);
    }

    cs_high;
    spi4_flash_wait_for_write_end();
}
/**
  * @brief 写FLASH
  * @param addr 写入flash的起始地址
  * @param pdata 写入数据的起始地址
  * pdata size 写数据大小
  * @retval None
  */
void spi4_flash_write(uint32_t addr, uint8_t *pdata, uint32_t size)
{
    uint32_t page_remain;

    page_remain = 256 - addr%256;

    if(size <= page_remain)
    {
        page_remain = size;
    }
    while(1)
    {
        spi4_flash_page_write(addr, pdata, page_remain);

        if(size == page_remain)
            break;
         else
        {
             pdata += page_remain;
             addr += page_remain;

             size -= page_remain;
            if(size > 256)
                page_remain = 256;
            else
                page_remain = size;
        }
    }
}
/**
  * @brief 擦除FLASH扇区
  * @param sector_addr 扇区的起始地址
  * @retval None
  */
void spi4_flash_sector_erase(uint32_t sector_addr)
{
    spi4_flash_write_enable();
    spi4_flash_wait_for_write_end();

    cs_low;
    spi4_flash_send_byte(0x20);
    spi4_flash_send_byte((sector_addr & 0xFF0000) >> 16);
    spi4_flash_send_byte((sector_addr & 0xFF00) >> 8);
    spi4_flash_send_byte(sector_addr & 0xFF);

    cs_high;

    spi4_flash_wait_for_write_end();
}
/**
  * @brief 擦除FLASH块
  * @param None
  * @retval None
  */
void spi4_flash_block_erase(void)
{
    spi4_flash_write_enable();

    cs_low;
    spi4_flash_send_byte(0xC7);
    cs_high;

    spi4_flash_wait_for_write_end();
	
}

