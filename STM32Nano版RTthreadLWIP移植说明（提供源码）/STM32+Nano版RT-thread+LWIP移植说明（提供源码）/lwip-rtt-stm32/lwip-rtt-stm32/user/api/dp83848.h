#ifndef __DP83848_H__
#define __DP83848_H__
#include "public_define.h"

/*网络引脚号设置 RMII接口 
  ETH_MDIO -------------------------> 25
  ETH_MDC --------------------------> 16
  ETH_RMII_REF_CLK------------------> 24
  ETH_RMII_CRS_DV ------------------> 32
  ETH_RMII_RXD0 --------------------> 33
  ETH_RMII_RXD1 --------------------> 34
  ETH_RMII_TX_EN -------------------> 48
  ETH_RMII_TXD0 --------------------> 51
  ETH_RMII_TXD1 --------------------> 52
  ETH_RESET-------------------------> 54*/

#define ETH_MDIO                      25
#define ETH_MDC                       16
#define ETH_RMII_REF_CLK              24
#define ETH_RMII_CRS_DV               32
#define ETH_RMII_RXD0                 33
#define ETH_RMII_RXD1                 34
#define ETH_RMII_TX_EN                48
#define ETH_RMII_TXD0                 51
#define ETH_RMII_TXD1                 52
#define ETH_RESET                     54

#define DP83848_PHY_ADDRESS  	      0x01//DP83848 PHY芯片地址.根据核心板硬件连接决定
//DP83848初始化
u8 Dp83848Init(void);
//DP83848设置接收回调函数
void Dp83848Banding(void (*Dp83848RxOverCallBack)(void));
//DP83848驱动内存申请
u8 Dp83848MemMalloc(void);
//DP83848驱动内存释放
void Dp83848MemFree(void);
//DP83848启动
void Dp83848StartUp(uint8_t *mac);
//DP83848反初始化
void Dp83848DeInit(void);
#endif


