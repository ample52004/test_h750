
/***************************************************************************************
****************************************************************************************
* FILE        : drv_pin.c
* Description :
*
* Copyright (c)      2022 by Tpson.        All Rights Reserved.
*
* History:
* Version         Name            Date              Description
* 0.1            zhongchao       2022/04/20        initial Version
*
****************************************************************************************
****************************************************************************************/

#include "drv_pin.h"
#include "drv_common.h"
#define PIN_MAX_INDEX   16

struct PinConfig
{
    GPIO_TypeDef* Port;
    uint32_t  Pin;
};
//GPIO_Pin_sources
static const uint8_t gGpioPinSource[] = {GPIO_PinSource0, GPIO_PinSource1 , GPIO_PinSource2 , GPIO_PinSource3 , GPIO_PinSource4,
                                        GPIO_PinSource5, GPIO_PinSource6 , GPIO_PinSource7 , GPIO_PinSource8, GPIO_PinSource9,
                                        GPIO_PinSource10, GPIO_PinSource11 , GPIO_PinSource12 , GPIO_PinSource13 , GPIO_PinSource14,
                                        GPIO_PinSource15
};
#if (STM32F207_PIN_NUM == STM32F207_PIN_NUM_100)

/*
    TCK_SWCLK   =   1 << 0,             ///< TCK or SWCLK
    TMS_SWDIO   =   1 << 1,             ///< TMS or SWDIO
    TDO_SWO     =   1 << 2,             ///< TOD or SWD
    TDI         =   1 << 3,             ///< TDI
    TRST        =   1 << 4,             ///< TRest
*/
static const uint8_t gDebugPortTable[5] = {76, 72 , 89 , 77 , 90};//分别对应

const static struct PinConfig Stm32PinConfig[STM32F207_PIN_NUM] =
{
    {GPIOE,GPIO_Pin_2},
    {GPIOE,GPIO_Pin_3},
    {GPIOE,GPIO_Pin_4},
    {GPIOE,GPIO_Pin_5},
    {GPIOE,GPIO_Pin_6},
    {GPIOH,GPIO_Pin_2},
    {GPIOC,GPIO_Pin_13},
    {GPIOC,GPIO_Pin_14},
    {GPIOC,GPIO_Pin_15},
    {(GPIO_TypeDef*)0,(uint32_t)0},//VSS
    {(GPIO_TypeDef*)0,(uint32_t)0},//VCC
    {GPIOH,GPIO_Pin_0},
    {GPIOH,GPIO_Pin_1},
    {(GPIO_TypeDef*)0,(uint32_t)0},//NRST
    {GPIOC,GPIO_Pin_0},
    {GPIOC,GPIO_Pin_1},
    {GPIOC,GPIO_Pin_2},
    {GPIOC,GPIO_Pin_3},
    {(GPIO_TypeDef*)0,(uint32_t)0},//VCC
    {(GPIO_TypeDef*)0,(uint32_t)0},//AVSS
    {(GPIO_TypeDef*)0,(uint32_t)0},//VREFH
    {(GPIO_TypeDef*)0,(uint32_t)0},//AVCC
    {GPIOA,GPIO_Pin_0},
    {GPIOA,GPIO_Pin_1},
    {GPIOA,GPIO_Pin_2},

    {GPIOA,GPIO_Pin_3},
    {(GPIO_TypeDef*)0,(uint32_t)0},//VSS
    {(GPIO_TypeDef*)0,(uint32_t)0},//VCC
    {GPIOA,GPIO_Pin_4},
    {GPIOA,GPIO_Pin_5},
    {GPIOA,GPIO_Pin_6},
    {GPIOA,GPIO_Pin_7},
    {GPIOC,GPIO_Pin_4},
    {GPIOC,GPIO_Pin_5},
    {GPIOB,GPIO_Pin_0},
    {GPIOB,GPIO_Pin_1},
    {GPIOB,GPIO_Pin_2},
    {GPIOE,GPIO_Pin_7},
    {GPIOE,GPIO_Pin_8},
    {GPIOE,GPIO_Pin_9},
    {GPIOE,GPIO_Pin_10},
    {GPIOE,GPIO_Pin_11},
    {GPIOE,GPIO_Pin_12},
    {GPIOE,GPIO_Pin_13},
    {GPIOE,GPIO_Pin_14},
    {GPIOE,GPIO_Pin_15},
    {GPIOB,GPIO_Pin_10},
    {GPIOB,GPIO_Pin_11},
    {(GPIO_TypeDef*)0,(uint32_t)0},//VSS
    {(GPIO_TypeDef*)0,(uint32_t)0},//VCC

    {GPIOB,GPIO_Pin_12},
    {GPIOB,GPIO_Pin_13},
    {GPIOB,GPIO_Pin_14},
    {GPIOB,GPIO_Pin_15},
    {GPIOD,GPIO_Pin_8},
    {GPIOD,GPIO_Pin_9},
    {GPIOD,GPIO_Pin_10},
    {GPIOD,GPIO_Pin_11},
    {GPIOD,GPIO_Pin_12},
    {GPIOD,GPIO_Pin_13},
    {GPIOD,GPIO_Pin_14},
    {GPIOD,GPIO_Pin_15},
    {GPIOC,GPIO_Pin_6},
    {GPIOC,GPIO_Pin_7},
    {GPIOC,GPIO_Pin_8},
    {GPIOC,GPIO_Pin_9},
    {GPIOA,GPIO_Pin_8},
    {GPIOA,GPIO_Pin_9},
    {GPIOA,GPIO_Pin_10},
    {GPIOA,GPIO_Pin_11},
    {GPIOA,GPIO_Pin_12},
    {GPIOA,GPIO_Pin_13},
    {(GPIO_TypeDef*)0,(uint32_t)0},//VCAP_2
    {(GPIO_TypeDef*)0,(uint32_t)0},//VSS
    {(GPIO_TypeDef*)0,(uint32_t)0},//VCC

    {GPIOA,GPIO_Pin_14},
    {GPIOA,GPIO_Pin_15},
    {GPIOC,GPIO_Pin_10},
    {GPIOC,GPIO_Pin_11},
    {GPIOC,GPIO_Pin_12},
    {GPIOD,GPIO_Pin_0},
    {GPIOD,GPIO_Pin_1},
    {GPIOD,GPIO_Pin_2},
    {GPIOD,GPIO_Pin_3},
    {GPIOD,GPIO_Pin_4},
    {GPIOD,GPIO_Pin_5},
    {GPIOD,GPIO_Pin_6},
    {GPIOD,GPIO_Pin_7},
    {GPIOB,GPIO_Pin_3},
    {GPIOB,GPIO_Pin_4},
    {GPIOB,GPIO_Pin_5},
    {GPIOB,GPIO_Pin_6},
    {GPIOB,GPIO_Pin_7},
    {GPIOB,GPIO_Pin_11},
    {GPIOB,GPIO_Pin_8},
    {GPIOB,GPIO_Pin_9},
    {GPIOE,GPIO_Pin_0},
    {GPIOE,GPIO_Pin_1},
    {(GPIO_TypeDef*)0,(uint32_t)0},//VSS
    {(GPIO_TypeDef*)0,(uint32_t)0},//VCC
};
#endif

/*==================================================================
* Function      :GetPinIndex
* Description   :获取pin序列号
* Input Para    :None
* Output Para   :None
* Return Value  :pin 序列号
==================================================================*/
static uint8_t GetPinIndex(uint16_t pin)
{
    uint16_t TempPin = pin;
    uint8_t i = 0;
    for(i=0;i<PIN_MAX_INDEX;i++)
    {
        if((TempPin &0x0001) != 0)
        {
            return i;
        }
        else
        {
           TempPin =  TempPin >> 1;
        }
    }
    return PIN_MAX_INDEX;
}

/*==================================================================
* Function      :DrvPinInit
* Description   :管脚初始化
* Input Para    :PinNum:管脚号 Mode:输入/输出
* Output Para   :None
* Return Value  :None
==================================================================*/
void DrvPinInit(uint8_t PinNum,PinMode Mode)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    if(PinNum == 0 || PinNum > STM32F207_PIN_NUM)
    {
        return;
    }
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    if(Mode == Pin_Out)
    {
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    }
    else if(Mode == Pin_Af)
    {
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    }
    else
    {
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    }
    GPIO_InitStructure.GPIO_Pin = Stm32PinConfig[PinNum-1].Pin;
    GPIO_Init(Stm32PinConfig[PinNum-1].Port, &GPIO_InitStructure);

    uint8_t i;
    for(i  = 0 ; i < 5; i++)
    {
        if(PinNum == gDebugPortTable[i])
        {
            GPIO_PinAFConfig(Stm32PinConfig[PinNum-1].Port, gGpioPinSource[GetPinIndex(Stm32PinConfig[PinNum-1].Pin)], GPIO_AF_SWJ);   
            /* Disable JLink, enable SW */
        }
    }
}

/*==================================================================
* Function	    :DrvPinSetFun
* Description   :复用功能设置
* Input Para    : PinNum :GPIO PIN Gpio_Af: 复用类型
* Output Para   : None
* Return Value  : None
==================================================================*/
void DrvPinSetFun(uint8_t PinNum ,uint8_t Gpio_Af)
{
    if(PinNum == 0 || PinNum > STM32F207_PIN_NUM)
    {
        return;
    }

    uint8_t i;
    for(i  = 0 ; i < 5; i++)
    {
        if(PinNum == gDebugPortTable[i])
        {
            GPIO_PinAFConfig(Stm32PinConfig[PinNum-1].Port, gGpioPinSource[GetPinIndex(Stm32PinConfig[PinNum-1].Pin)], GPIO_AF_SWJ);   
            /* Disable JLink, enable SW */
        }
    }
    if(PIN_MAX_INDEX == GetPinIndex(Stm32PinConfig[PinNum-1].Pin)) return;
    GPIO_PinAFConfig(Stm32PinConfig[PinNum-1].Port, gGpioPinSource[GetPinIndex(Stm32PinConfig[PinNum-1].Pin)], Gpio_Af);
}
/*==================================================================
* Function      :DrvPinSetStatus
* Description   :设置管脚状态
* Input Para    :PinNum:管脚号 Status:拉高/拉低/跳转
* Output Para   :None
* Return Value  :None
==================================================================*/
void DrvPinSetStatus(uint8_t PinNum,PinStatus Status)
{
    if(PinNum == 0 || PinNum > STM32F207_PIN_NUM)
    {
        return;
    }
    switch(Status)
    {
        default:
            break;
        case Pin_Set:
            GPIO_SetBits(Stm32PinConfig[PinNum-1].Port, Stm32PinConfig[PinNum-1].Pin);
            break;
        case Pin_Reset:
            GPIO_ResetBits(Stm32PinConfig[PinNum-1].Port, Stm32PinConfig[PinNum-1].Pin);
            break;
        case Pin_Toggle:
            GPIO_ToggleBits(Stm32PinConfig[PinNum-1].Port, Stm32PinConfig[PinNum-1].Pin);
            break;
    }
}

/*==================================================================
* Function      :DrvPinGetStatus
* Description   :获取管脚状态
* Input Para    :PinNum:管脚号
* Output Para   :None
* Return Value  :None
==================================================================*/
PinStatus DrvPinGetStatus(uint8_t PinNum)
{
    uint8_t status;

    if(PinNum == 0 || PinNum > STM32F207_PIN_NUM)
    {
        return Pin_Set;
    }

    status = GPIO_ReadOutputDataBit(Stm32PinConfig[PinNum-1].Port, Stm32PinConfig[PinNum-1].Pin);

    if(Bit_RESET == status)
    {
        return Pin_Reset;
    }

    return Pin_Set;
}
