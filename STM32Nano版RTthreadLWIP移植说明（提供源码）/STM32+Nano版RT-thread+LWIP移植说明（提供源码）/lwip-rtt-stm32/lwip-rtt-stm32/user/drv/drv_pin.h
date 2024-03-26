#ifndef __DRV_PIN_H__
#define __DRV_PIN_H__
#include "public_define.h"
typedef enum
{
    Pin_Out,
    Pin_In,
    Pin_Af,
}PinMode;

typedef enum
{
    Pin_Set,
    Pin_Reset,
    Pin_Toggle,
}PinStatus;

typedef enum
{
   AF_ETH = 0x0B,   
}ENUM_AF;

//初始化管脚
void DrvPinInit(uint8_t PinNum,PinMode Mode);
//设置管脚状态
void DrvPinSetStatus(uint8_t PinNum,PinStatus Status);
//获取管脚状态
PinStatus DrvPinGetStatus(uint8_t PinNum);
//设置第二功能
void DrvPinSetFun(uint8_t PinNum ,uint8_t Gpio_Af);

#endif
