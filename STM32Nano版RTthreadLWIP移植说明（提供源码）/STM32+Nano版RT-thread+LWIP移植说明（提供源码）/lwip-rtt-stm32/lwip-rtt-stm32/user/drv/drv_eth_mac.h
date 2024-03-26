#ifndef __DRV_ETH_MAC_H__
#define __DRV_ETH_MAC_H__
#include "public_define.h"
//ͨѶ�ӿ�����
typedef enum
{
    //RMII
    INTERFACE_RMII =0,
    //MII
    INTERFACE_MII,   
}ENUM_INTERFACE_TYPE;

//��ʼ��ETH MAC�㼰DMA����
u8 DrvEthMacDMacConfig(uint16_t phyaddr,ENUM_INTERFACE_TYPE type);
//����һ���������ݰ�
FrameTypeDef DrvEthRxPacket(void);
//����һ���������ݰ�
u8 DrvEthTxPacket(u16 FrameLength);
//�õ���ǰ��������Tx buffer��ַ
u32 DrvEthGetCurrentTxBuffer(void);
//��̫���жϷ�������
void DrvEthNvicConfig(void);
//����ETH MAC��ص�����
void DrvEthMacBanding(void (*EthRxOverCallBack)(void));
//��ȡ�����ײ��ڴ���Ҫ���볤��
void DrvEthGetMemLen(uint16_t *DMARxLen,uint16_t *DMATxLen,uint16_t *RxLen,uint16_t *TxLen);
//���õײ����������ڴ��ַ
void DrvEthSetMemAddr(ETH_DMADESCTypeDef *DMARx,ETH_DMADESCTypeDef *DMATx,uint8_t *Rx,uint8_t *Tx);
//����MAC��DMA
void DrvEthMacStartUp(uint8_t * mac,bool hardcheckenable);
#endif




