/****************************************************************************************
* FILE         : dp83848.c
* Description  : DP83848 ������������
*
* Copyright (c) 2022 by Tpson. All Rights Reserved.
*
* History:
* Version      Name            Date            Description
   0.1       zhongchao        2020/04/18      Initial Version

****************************************************************************************
****************************************************************************************/

#include "dp83848.h"
#include "drv_eth_mac.h"
#include "drv_pin.h"
#include <rthw.h>
#include "lwipopts.h"

//��������buf
static uint8_t * gDMARxBuf = NULL;
static uint8_t * gDMATxBuf = NULL;
static uint8_t * gRxBuf = NULL; 
static uint8_t * gTxBuf = NULL;

/*==================================================================
* Function      : Dp83848Init
* Description   : DP83848��ʼ��
* Input Para    : none
* Output Para   : none
* Return Value  : ��� 0,ʧ��;1,�ɹ�
==================================================================*/
u8 Dp83848Init(void)
{
    u8 Rval=0;
    u8 i = 0;
    u8 EthPin[] = {ETH_MDIO,ETH_MDC,ETH_RMII_REF_CLK,ETH_RMII_CRS_DV,ETH_RMII_RXD0,ETH_RMII_RXD1,ETH_RMII_TX_EN,
                    ETH_RMII_TXD0,ETH_RMII_TXD1};
    //��ʼ��ͨѶ�ӿ�����
    for(i=0;i<(sizeof(EthPin));i++)
    {
        DrvPinInit(EthPin[i],Pin_Af);
        DrvPinSetFun(EthPin[i],AF_ETH);
    }
                    
    DrvPinInit(ETH_RESET,Pin_Out); 
    //Ӳ����λdp83848
    DrvPinSetStatus(ETH_RESET,Pin_Reset);
    //delay 50ms
    rt_thread_delay(10);
    DrvPinSetStatus(ETH_RESET,Pin_Set);
    rt_thread_delay(100);
    //����ETH MAC�ж�
    DrvEthNvicConfig();
    //����ETH MAC
    Rval = DrvEthMacDMacConfig(DP83848_PHY_ADDRESS,INTERFACE_RMII);
    //ETH�Ĺ���Ϊ:0,ʧ��;1,�ɹ�;����Ҫȡ��һ�� 
    return !Rval;					
}

/*==================================================================
* Function      : Dp83848DeInit
* Description   : DP83848����ʼ��
* Input Para    : none
* Output Para   : none
* Return Value  : none
==================================================================*/
void Dp83848DeInit(void)
{
    //��λ��̫��
    ETH_DeInit();  					
}


/*==================================================================
* Function      : Dp83848StartUp
* Description   : DP83848��������
* Input Para    : mac ��ַ
* Output Para   : none
* Return Value  : none
==================================================================*/
void Dp83848StartUp(uint8_t *mac)
{
    bool HardCheckEnable = false;
       //ʹ��Ӳ��֡У��
#ifdef CHECKSUM_BY_HARDWARE 
    HardCheckEnable = true;   
#endif
    DrvEthMacStartUp(mac,HardCheckEnable);
    
}

/*==================================================================
* Function      : Dp83848Banding
* Description   : ����DP83848���ջص�����
* Input Para    : 83848���ջص�����
* Output Para   : none
* Return Value  : none
==================================================================*/
void Dp83848Banding(void (*Dp83848RxOverCallBack)(void))
{
    DrvEthMacBanding(Dp83848RxOverCallBack);
}


/*==================================================================
* Function      : Dp83848MemFree
* Description   : DP83848�����ڴ��ͷ�
* Input Para    : none
* Output Para   : none
* Return Value  : none
==================================================================*/
void Dp83848MemFree(void)
{  
    if(gDMARxBuf == NULL || gDMATxBuf == NULL
        || gRxBuf == NULL || gTxBuf) return;
    uint8_t * DMARx = NULL;
    uint8_t * DMATx = NULL;
    uint8_t * Rx = NULL; 
    uint8_t * Tx = NULL;
    //�ͷ��ڴ�
	rt_free(gDMARxBuf);
	rt_free(gDMATxBuf);
	rt_free(gRxBuf);		
	rt_free(gTxBuf);		
    DrvEthSetMemAddr((ETH_DMADESCTypeDef *)DMARx,(ETH_DMADESCTypeDef *)DMATx,Rx,Tx);
}
   
/*==================================================================
* Function      : Dp83848MemMalloc
* Description   : DP83848�����ڴ�����
* Input Para    : none
* Output Para   : none
* Return Value  : 0,����  ����,ʧ��
==================================================================*/
u8 Dp83848MemMalloc(void)
{ 
    uint16_t DMARxLen = 0, DMATxLen = 0, RxLen = 0, TxLen = 0;
    DrvEthGetMemLen(&DMARxLen,&DMATxLen,&RxLen,&TxLen);
	gDMARxBuf = rt_malloc(DMARxLen);
	gDMATxBuf = rt_malloc(DMATxLen); 
	gRxBuf = rt_malloc(RxLen);	
	gTxBuf = rt_malloc(TxLen);	
	if(!gDMARxBuf||!gDMATxBuf||!gRxBuf||!gTxBuf)
	{
		Dp83848MemFree();
		return 1;	
	}	
    DrvEthSetMemAddr((ETH_DMADESCTypeDef *)gDMARxBuf,(ETH_DMADESCTypeDef *)gDMATxBuf,gRxBuf,gTxBuf);
	return 0;		
}
