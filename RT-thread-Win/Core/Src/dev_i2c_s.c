#include "dev_i2c_s.h"
#include "main.h"
#include "i2c.h"
#define i2c_sda_high() 			LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_9);
#define i2c_sda_low()  			LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_9);
#define i2c_scl_high() 			LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_8);
#define i2c_scl_low(); 			LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_8);
#define i2c_read_sda() 			LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_9)
/**
 * @brief 模拟I2C延时
 * @retval none
 */
static void analog_i2c_delay(void)
{
    volatile uint8_t i;

    for (i = 0; i < 10; i++);
}
/**
 * @brief 软件模拟I2C初始化
 * @retval none
 */
void bsp_analog_i2c_init(void)
{
	
    LL_GPIO_InitTypeDef gpio_init_struct;

    LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOB);

    /* SCL引脚配置 */
    gpio_init_struct.Pin = LL_GPIO_PIN_8;
    gpio_init_struct.Mode = LL_GPIO_MODE_OUTPUT;
    gpio_init_struct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    gpio_init_struct.Pull = LL_GPIO_PULL_UP;
    gpio_init_struct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    LL_GPIO_Init(GPIOB, &gpio_init_struct);

    /* SDA引脚配置 */
    gpio_init_struct.Pin = LL_GPIO_PIN_9;
    gpio_init_struct.Mode = LL_GPIO_MODE_OUTPUT;
    gpio_init_struct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    gpio_init_struct.Pull = LL_GPIO_PULL_UP;
    gpio_init_struct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    LL_GPIO_Init(GPIOB, &gpio_init_struct);
	
  bsp_analog_i2c_stop();
}

/**
 * @brief I2C 开始,SCL为高电平的时候SDA产生一个下降沿信号
 * @retval none
 */
void bsp_analog_i2c_start(void)
{
    /*    _____
     *SDA      \_____________
     *    __________
     *SCL           \________
     */
    i2c_sda_high();
    i2c_scl_high();
    analog_i2c_delay();
    i2c_sda_low();
    analog_i2c_delay();
    i2c_scl_low();
    analog_i2c_delay();
}

/**
 * @brief I2C 停止,SCL为高电平的时候SDA产生一个上升沿信号
 * @retval none
 */
void bsp_analog_i2c_stop(void)
{
    /*               _______
     *SDA __________/
     *          ____________
     *SCL _____/
     */
    i2c_sda_low();
		analog_i2c_delay();
    i2c_scl_high();
    analog_i2c_delay();
    i2c_sda_high();
    analog_i2c_delay();
}

/**
 * @brief I2C 等待响应
 * @retval none
 */
uint8_t bsp_analog_i2c_wait_ack(void)
{
    uint32_t timeout = 0;

    i2c_sda_high();
    analog_i2c_delay();
    i2c_scl_high();
    analog_i2c_delay();
    while(i2c_read_sda())
    {
        timeout++;
        if(timeout > 2000)
        {
            return 0;
        }
    }
    i2c_scl_low();
    analog_i2c_delay();
    return 1;
}
/**
 * @brief I2C 响应
 * @retval none
 */
void bsp_analog_i2c_ack(void)
{
    /*           ____
     *SCL ______/    \______
     *    ____         _____
     *SDA     \_______/
     */
    i2c_sda_low();
    analog_i2c_delay();
    i2c_scl_high();
    analog_i2c_delay();
    i2c_scl_low();
    analog_i2c_delay();
    i2c_sda_high();
		analog_i2c_delay();
}
/**
 * @brief I2C 不响应
 * @retval none
 */
void bsp_analog_i2c_nack(void)
{
    /*           ____
     *SCL ______/    \______
     *    __________________
     *SDA
     */
    i2c_sda_high();
    analog_i2c_delay();
    i2c_scl_high();
    analog_i2c_delay();
    i2c_scl_low();
    analog_i2c_delay();
}
/**
 * @brief I2C 发送一个字节数据
 * @retval none
 */
void bsp_analog_i2c_send_byte(uint8_t data)
{
    uint8_t i;

    for(i = 0; i < 8; i++)
    {
        if(data & 0x80)
        {
           i2c_sda_high();
        }
        else
        {
            i2c_sda_low();
        }

        analog_i2c_delay();
        i2c_scl_high();
        analog_i2c_delay();
        i2c_scl_low();
        if(i == 7)
        {
            i2c_sda_high();
        }
        data <<= 1;
        analog_i2c_delay();
    }
}
/**
 * @brief I2C 读一个字节数据
 * @retval none
 */
uint8_t bsp_analog_i2c_read_byte(void)
{
    uint8_t i, data = 0;

    for(i = 0; i < 8; i++ )
    {
        data <<= 1;
        i2c_scl_high();
        analog_i2c_delay();
        if(i2c_read_sda())
        {
            data++;
        }
        i2c_scl_low();
        analog_i2c_delay();
    }

    return data;
}
void at24cxx_delay(uint32_t t)
{
    uint32_t i, j;
    for(i = 0; i < t; i++)
    {
        for(j = 100; j > 0; j--);
    }
}

void at24cxx_init(void) 
{
    bsp_analog_i2c_init();
}


int8_t at24cxx_write_one_byte(uint8_t reg, uint8_t data)
{
    bsp_analog_i2c_start();
    /* 发送设备地址 */
    bsp_analog_i2c_send_byte(0xA0);
    if(bsp_analog_i2c_wait_ack() == 0)
    {
        bsp_analog_i2c_stop();
        return -1;
    }
    /* 发送寄存器地址 */
    bsp_analog_i2c_send_byte(reg);
    if(bsp_analog_i2c_wait_ack() == 0)
    {
        bsp_analog_i2c_stop();
        return -2;
    }
    /* 发送数据 */
    bsp_analog_i2c_send_byte(data);
    if(bsp_analog_i2c_wait_ack() == 0)
    {
        bsp_analog_i2c_stop();
        return -3;
    }
    bsp_analog_i2c_stop();

    at24cxx_delay(0xFFFF);

    return 0;
}

int8_t at24cxx_read_byte(uint8_t reg, uint8_t *pdata, uint16_t size)
{
    uint16_t i;

    bsp_analog_i2c_start();
    /* 发送设备地址 */
    bsp_analog_i2c_send_byte(0xA0);
    if(bsp_analog_i2c_wait_ack() == 0)
    {
        bsp_analog_i2c_stop();
        return -1;
    }
    /* 发送寄存器地址 */
    bsp_analog_i2c_send_byte(reg);
    if(bsp_analog_i2c_wait_ack() == 0)
    {
        bsp_analog_i2c_stop();
        return -1;
    }
    bsp_analog_i2c_start();
    /* 发送设备地址 */
    bsp_analog_i2c_send_byte(0XA1);
    if(bsp_analog_i2c_wait_ack() == 0)
    {
        bsp_analog_i2c_stop();
        return -1;
    }
    /* 接收数据 */
    for(i = 0; i < size; i++)
    {
        pdata[i] = bsp_analog_i2c_read_byte();
        if(i != size - 1 )
        {
            bsp_analog_i2c_ack();
        }
        else
        {
            bsp_analog_i2c_nack();
        }
    }

    bsp_analog_i2c_stop();

    return 0;
}

int8_t at24cxx_write_byte(uint8_t reg, uint8_t *pdata, uint16_t size)
{
    int8_t ret = 0 ;
    uint16_t i;

    for(i = 0; i < size; i++)
    {
        ret = at24cxx_write_one_byte(reg+i, pdata[i]);
        if(ret < 0)
            break;
    }

    return ret;
}
 
void I2C_WriteData(uint8_t devAddr, uint8_t regAddr, uint8_t data) {
    while (LL_I2C_IsActiveFlag_BUSY(I2C1))
        ;

    // 发送起始信号
    LL_I2C_GenerateStartCondition(I2C1);
    while (!LL_I2C_IsActiveFlag_STOP(I2C1))
        ;

    // 发送设备地址和写标志
    LL_I2C_TransmitData8(I2C1, devAddr << 1);
    while (!LL_I2C_IsActiveFlag_ADDR(I2C1))
        ;
    LL_I2C_ClearFlag_ADDR(I2C1);

    // 发送寄存器地址
    LL_I2C_TransmitData8(I2C1, regAddr);
    while (!LL_I2C_IsActiveFlag_TXE(I2C1))
        ;

    // 发送数据
    LL_I2C_TransmitData8(I2C1, data);
    while (!LL_I2C_IsActiveFlag_TXE(I2C1))
        ;

    // 发送停止信号
    LL_I2C_GenerateStopCondition(I2C1);
}

uint8_t I2C_ReadData(uint8_t devAddr, uint8_t regAddr) {
    uint8_t data;

    while (LL_I2C_IsActiveFlag_BUSY(I2C1))
        ;

    // 发送起始信号
    LL_I2C_GenerateStartCondition(I2C1);
    while (!LL_I2C_IsActiveFlag_STOP(I2C1))
        ;

    // 发送设备地址和写标志
    LL_I2C_TransmitData8(I2C1, devAddr << 1);
    while (!LL_I2C_IsActiveFlag_ADDR(I2C1))
        ;
    LL_I2C_ClearFlag_ADDR(I2C1);

    // 发送寄存器地址
    LL_I2C_TransmitData8(I2C1, regAddr);
    while (!LL_I2C_IsActiveFlag_TXE(I2C1))
        ;

    // 重新启动
    LL_I2C_GenerateStartCondition(I2C1);
    while (!LL_I2C_IsActiveFlag_STOP(I2C1))
        ;

    // 发送设备地址和读标志
    LL_I2C_TransmitData8(I2C1, (devAddr << 1) | 1);
    while (!LL_I2C_IsActiveFlag_ADDR(I2C1))
        ;
    LL_I2C_ClearFlag_ADDR(I2C1);

    // 开启接收模式
    LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_ACK);

    // 发送停止信号
    LL_I2C_GenerateStopCondition(I2C1);

    // 等待数据接收
    while (!LL_I2C_IsActiveFlag_RXNE(I2C1))
        ;
    data = LL_I2C_ReceiveData8(I2C1);

    return data;
}

