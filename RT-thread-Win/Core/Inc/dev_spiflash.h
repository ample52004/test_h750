#include "spi.h"
#include "gpio.h"

static uint8_t spi4_flash_read_byte(void);
static uint8_t spi4_flash_send_byte(uint8_t byte);
static void spi4_flash_write_enable(void);
static void spi4_flash_wait_for_write_end(void);
uint32_t spi_flash_read_ID(void);

void spi4_flash_read(uint32_t addr,uint8_t *pdata, uint16_t size);
void spi4_flash_page_write(uint32_t addr, uint8_t *pdata, uint16_t size);
void spi4_flash_write(uint32_t addr, uint8_t *pdata, uint32_t size);
void spi4_flash_sector_erase(uint32_t sector_addr);
void spi4_flash_block_erase(void);
