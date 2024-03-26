#ifndef __DP83848_H__
#define __DP83848_H__
#include "public_define.h"

/*�������ź����� RMII�ӿ� 
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

#define DP83848_PHY_ADDRESS  	      0x01//DP83848 PHYоƬ��ַ.���ݺ��İ�Ӳ�����Ӿ���
//DP83848��ʼ��
u8 Dp83848Init(void);
//DP83848���ý��ջص�����
void Dp83848Banding(void (*Dp83848RxOverCallBack)(void));
//DP83848�����ڴ�����
u8 Dp83848MemMalloc(void);
//DP83848�����ڴ��ͷ�
void Dp83848MemFree(void);
//DP83848����
void Dp83848StartUp(uint8_t *mac);
//DP83848����ʼ��
void Dp83848DeInit(void);
#endif


