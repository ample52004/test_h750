#ifndef __DRV_ETH_MAC_H__
#define __DRV_ETH_MAC_H__
#include "public_define.h"
//通讯接口类型
typedef enum
{
    //RMII
    INTERFACE_RMII =0,
    //MII
    INTERFACE_MII,   
}ENUM_INTERFACE_TYPE;

//初始化ETH MAC层及DMA配置
u8 DrvEthMacDMacConfig(uint16_t phyaddr,ENUM_INTERFACE_TYPE type);
//接收一个网卡数据包
FrameTypeDef DrvEthRxPacket(void);
//发送一个网卡数据包
u8 DrvEthTxPacket(u16 FrameLength);
//得到当前描述符的Tx buffer地址
u32 DrvEthGetCurrentTxBuffer(void);
//以太网中断分组配置
void DrvEthNvicConfig(void);
//设置ETH MAC层回调函数
void DrvEthMacBanding(void (*EthRxOverCallBack)(void));
//获取驱动底层内存需要申请长度
void DrvEthGetMemLen(uint16_t *DMARxLen,uint16_t *DMATxLen,uint16_t *RxLen,uint16_t *TxLen);
//设置底层驱动申请内存地址
void DrvEthSetMemAddr(ETH_DMADESCTypeDef *DMARx,ETH_DMADESCTypeDef *DMATx,uint8_t *Rx,uint8_t *Tx);
//开启MAC和DMA
void DrvEthMacStartUp(uint8_t * mac,bool hardcheckenable);
#endif




