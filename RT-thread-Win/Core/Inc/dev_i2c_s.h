#include "main.h"
#include "gpio.h"

static void analog_i2c_delay(void);
void bsp_analog_i2c_init(void);
void bsp_analog_i2c_start(void);
void bsp_analog_i2c_stop(void);
uint8_t bsp_analog_i2c_wait_ack(void);
void bsp_analog_i2c_ack(void);
void bsp_analog_i2c_nack(void);
void bsp_analog_i2c_send_byte(uint8_t data);
uint8_t bsp_analog_i2c_read_byte(void);
void at24cxx_delay(uint32_t t);
int8_t at24cxx_write_one_byte(uint8_t reg, uint8_t data);
int8_t at24cxx_read_byte(uint8_t reg, uint8_t *pdata, uint16_t size);
int8_t at24cxx_write_byte(uint8_t reg, uint8_t *pdata, uint16_t size);
void EEPROM_ClearCR(void);
uint8_t EEPROM_Write_Byte(uint8_t DeviceAddress, uint8_t MemAddress, uint8_t *pData, uint8_t TxLen,uint16_t Timeout);
uint8_t I2C_ReadData(uint8_t devAddr, uint8_t regAddr);
