/****************************************************************************************
* FILE         : dp83848.c
* Description  : DP83848 网卡驱动程序
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

//网卡驱动buf
static uint8_t * gDMARxBuf = NULL;
static uint8_t * gDMATxBuf = NULL;
static uint8_t * gRxBuf = NULL; 
static uint8_t * gTxBuf = NULL;

/*==================================================================
* Function      : Dp83848Init
* Description   : DP83848初始化
* Input Para    : none
* Output Para   : none
* Return Value  : 结果 0,失败;1,成功
==================================================================*/
u8 Dp83848Init(void)
{
    u8 Rval=0;
    u8 i = 0;
    u8 EthPin[] = {ETH_MDIO,ETH_MDC,ETH_RMII_REF_CLK,ETH_RMII_CRS_DV,ETH_RMII_RXD0,ETH_RMII_RXD1,ETH_RMII_TX_EN,
                    ETH_RMII_TXD0,ETH_RMII_TXD1};
    //初始化通讯接口引脚
    for(i=0;i<(sizeof(EthPin));i++)
    {
        DrvPinInit(EthPin[i],Pin_Af);
        DrvPinSetFun(EthPin[i],AF_ETH);
    }
                    
    DrvPinInit(ETH_RESET,Pin_Out); 
    //硬件复位dp83848
    DrvPinSetStatus(ETH_RESET,Pin_Reset);
    //delay 50ms
    rt_thread_delay(10);
    DrvPinSetStatus(ETH_RESET,Pin_Set);
    rt_thread_delay(100);
    //设置ETH MAC中断
    DrvEthNvicConfig();
    //设置ETH MAC
    Rval = DrvEthMacDMacConfig(DP83848_PHY_ADDRESS,INTERFACE_RMII);
    //ETH的规则为:0,失败;1,成功;所以要取反一下 
    return !Rval;					
}

/*==================================================================
* Function      : Dp83848DeInit
* Description   : DP83848反初始化
* Input Para    : none
* Output Para   : none
* Return Value  : none
==================================================================*/
void Dp83848DeInit(void)
{
    //复位以太网
    ETH_DeInit();  					
}


/*==================================================================
* Function      : Dp83848StartUp
* Description   : DP83848网卡启动
* Input Para    : mac 地址
* Output Para   : none
* Return Value  : none
==================================================================*/
void Dp83848StartUp(uint8_t *mac)
{
    bool HardCheckEnable = false;
       //使用硬件帧校验
#ifdef CHECKSUM_BY_HARDWARE 
    HardCheckEnable = true;   
#endif
    DrvEthMacStartUp(mac,HardCheckEnable);
    
}

/*==================================================================
* Function      : Dp83848Banding
* Description   : 设置DP83848接收回调函数
* Input Para    : 83848接收回调函数
* Output Para   : none
* Return Value  : none
==================================================================*/
void Dp83848Banding(void (*Dp83848RxOverCallBack)(void))
{
    DrvEthMacBanding(Dp83848RxOverCallBack);
}


/*==================================================================
* Function      : Dp83848MemFree
* Description   : DP83848驱动内存释放
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
    //释放内存
	rt_free(gDMARxBuf);
	rt_free(gDMATxBuf);
	rt_free(gRxBuf);		
	rt_free(gTxBuf);		
    DrvEthSetMemAddr((ETH_DMADESCTypeDef *)DMARx,(ETH_DMADESCTypeDef *)DMATx,Rx,Tx);
}
   
/*==================================================================
* Function      : Dp83848MemMalloc
* Description   : DP83848驱动内存申请
* Input Para    : none
* Output Para   : none
* Return Value  : 0,正常  其他,失败
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
