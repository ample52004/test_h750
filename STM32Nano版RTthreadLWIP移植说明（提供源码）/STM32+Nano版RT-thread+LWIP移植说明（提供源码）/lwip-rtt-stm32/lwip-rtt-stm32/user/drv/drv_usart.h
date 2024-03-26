#ifndef _DRV_USART_H_
#define _DRV_USART_H_
#include "public_define.h"

//串口类型枚举
typedef enum
{
   TYPE_USART6 = 0,
}ENUM_USART_TYPE;

//波特率枚举
typedef enum
{
    BAUDRATE1200 = 0,
    BAUDRATE2400,
    BAUDRATE4800,
    BAUDRATE7200,
    BAUDRATE9600,
    BAUDRATE14400,
    BAUDRATE19200,
    BAUDRATE38400,
    BAUDRATE57600,
    BAUDRATE115200,
}ENUM_USART_BAUDRATE;

//数据位枚举
typedef enum
{
    LEN8BIT = 0,
    LEN9BIT,
}ENUM_USART_LEN;

//停止位枚举
typedef enum
{
    STOPBIT0_5 ,
    STOPBIT1,
    STOPBIT1_5,
    STOPBIT2,
}ENUM_USART_STOP;

//校验位枚举
typedef enum
{
    PARITY_NONE,
    PARITY_ODD,
    PARITY_EVEN,
}ENUM_USART_PARITY;

#define USART6_UART             USART6
#define USART6_PORT             GPIOC
#define USART6_TX_PIN           GPIO_Pin_6
#define USART6_RX_PIN           GPIO_Pin_7

#define USART6_AF_TYPE          GPIO_AF_USART6
#define USART6_AF_TX_PIN        GPIO_PinSource6
#define USART6_AF_RX_PIN        GPIO_PinSource7

//USART 发送完成回调函数
typedef void (*UsartTxFinishCallBack)(void);
//USART 接收完成回调函数
typedef void (*UsartRxFinishCallBack)(uint8_t*,uint16_t);
//串口句柄结构体
typedef struct
{
    void (*GpioInit)(void);
    void (*ConfigInit)(ENUM_USART_BAUDRATE Rate,ENUM_USART_LEN Len,ENUM_USART_STOP Stop,ENUM_USART_PARITY Parity);
    void (*DmaInit)(uint8_t *TxBuff,uint16_t TxSize,uint8_t *RxBuff,uint16_t RxSize);
    void (*NvicInit)(void);
    void (*SetCallBack)(UsartTxFinishCallBack tx,UsartRxFinishCallBack rx);
    void (*SendData)(uint8_t c);
    void (*SendBuff)(uint8_t *TxBuff,uint16_t TxSize);
}Usart_Handle_Typedef,*pUsart_Handle_Typedef;

//USART 驱动注册
void UartDriverRegister(ENUM_USART_TYPE UsartType);
//USART 获取串口句柄
pUsart_Handle_Typedef UartGetHanle(ENUM_USART_TYPE UsartType);

#endif /*_DRV_USART_H_*/
