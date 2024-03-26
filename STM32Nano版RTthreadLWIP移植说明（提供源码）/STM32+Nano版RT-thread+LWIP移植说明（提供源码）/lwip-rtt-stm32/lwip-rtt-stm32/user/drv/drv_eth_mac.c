/****************************************************************************************
* FILE         : drv_eth_mac.c
* Description  : STM ETH MAC 驱动程序
*
* Copyright (c) 2022 by Tpson. All Rights Reserved.
*
* History:
* Version      Name            Date            Description
   0.1       zhongchao        2020/04/19      Initial Version

****************************************************************************************
****************************************************************************************/

#include "drv_eth_mac.h"
#include "drv_common.h"
#include "lwipopts.h"

//ETH MAC回调函数结构体
typedef struct
{
    void (*EthRxOverCallBack)(void);
}EthMac_CallBack_Typedef;
static EthMac_CallBack_Typedef EthMacCallBack = {NULL};
//DMA发送描述符追踪指针
extern ETH_DMADESCTypeDef  *DMATxDescToSet;			
//DMA接收描述符追踪指针
extern ETH_DMADESCTypeDef  *DMARxDescToGet;
//以太网DMA接收描述符数据结构体指针
__align(4) ETH_DMADESCTypeDef *DMARxDscrTab;
//以太网DMA发送描述符数据结构体指针
__align(4) ETH_DMADESCTypeDef *DMATxDscrTab;
//以太网底层驱动接收buffers指针 
__align(4) uint8_t *Rx_Buff; 	
//以太网底层驱动发送buffers指针
__align(4) uint8_t *Tx_Buff; 					


/*****************************以下部分网卡配置/接口函数***********************/
/*==================================================================
* Function      : DrvEthMacDMacConfig
* Description   : 初始化ETH MAC层及DMA配置
* Input Para    : phyaddr : PHY地址
* Output Para   : none
* Return Value  : 结果 0,失败;1,成功
==================================================================*/
u8 DrvEthMacDMacConfig(uint16_t phyaddr,ENUM_INTERFACE_TYPE type)
{
    u8 rval;
    ETH_InitTypeDef ETH_InitStructure; 
    
    //使能以太网时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_ETH_MAC | RCC_AHB1Periph_ETH_MAC_Tx |RCC_AHB1Periph_ETH_MAC_Rx, ENABLE);
    //MAC和PHY之间使用接口
    if(type == INTERFACE_MII)
    {
        SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_MII);
    }
    else
    {
        SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_RMII);
    }
    //AHB总线重启以太网
    ETH_DeInit(); 
    //软件重启网络
    ETH_SoftwareReset();
    //等待软件重启网络完成
    while (ETH_GetSoftwareResetStatus() == SET);  
    //初始化网络为默认值
    ETH_StructInit(&ETH_InitStructure); 

	//网络MAC参数设置 
    //开启网络自适应功能    
	ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;
    //关闭反馈
    ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;	
    //关闭重传功能
    ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable;  
    //关闭自动去除PDA/CRC功能 
    ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;
    //关闭接收所有的帧
    ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Disable;
    //允许接收所有广播帧
    ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;
    //关闭混合模式的地址过滤
    ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;
    //对于组播地址使用完美地址过滤
    ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;
    //对单播地址使用完美地址过滤
    ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;
#ifdef CHECKSUM_BY_HARDWARE
    //开启ipv4和TCP/UDP/ICMP的帧校验和卸载
	ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable; 			   
#endif 
    //当我们使用帧校验和卸载功能的时候，一定要使能存储转发模式,存储转发模式中要保证整个帧存储在FIFO中,
    //这样MAC能插入/识别出帧校验值,当真校验正确的时候DMA就可以处理帧,否则就丢弃掉该帧
    //开启丢弃TCP/IP错误帧
    ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Enable;
    //开启接收数据的存储转发模式 
    ETH_InitStructure.ETH_ReceiveStoreForward = ETH_ReceiveStoreForward_Enable;
    //开启发送数据的存储转发模式
    ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;
    //禁止转发错误帧 
    ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Disable;
    //不转发过小的好帧 
    ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Disable;
    //打开处理第二帧功能
    ETH_InitStructure.ETH_SecondFrameOperate = ETH_SecondFrameOperate_Enable;
    //开启DMA传输的地址对齐功能
    ETH_InitStructure.ETH_AddressAlignedBeats = ETH_AddressAlignedBeats_Enable; 
    //开启固定突发功能
    ETH_InitStructure.ETH_FixedBurst = ETH_FixedBurst_Enable;
    //DMA发送的最大突发长度为32个节拍
    ETH_InitStructure.ETH_RxDMABurstLength = ETH_RxDMABurstLength_32Beat;
    //DMA接收的最大突发长度为32个节拍
    ETH_InitStructure.ETH_TxDMABurstLength = ETH_TxDMABurstLength_32Beat;
    //选择DMA发送/接收仲裁
    ETH_InitStructure.ETH_DMAArbitration = ETH_DMAArbitration_RoundRobin_RxTx_2_1;
    //配置ETH
    rval=ETH_Init(&ETH_InitStructure,phyaddr);
    //配置成功
    if(rval==ETH_SUCCESS)
	{
        //使能以太网接收中断
		ETH_DMAITConfig(ETH_DMA_IT_NIS|ETH_DMA_IT_R,ENABLE);  		
	}
	return rval;
}

/*==================================================================
* Function      : ETH_IRQHandler
* Description   : 以太网中断服务函数
* Input Para    : none
* Output Para   : none
* Return Value  : none
==================================================================*/
void ETH_IRQHandler(void)
{
    DrvInterruputEnter();
    //检测是否收到数据包
	while(ETH_GetRxPktSize(DMARxDescToGet)!=0) 	
	{ 
        //接收回调函数
        if(NULL != EthMacCallBack.EthRxOverCallBack)
        {
            EthMacCallBack.EthRxOverCallBack();
        }            
	}
    //清除中断标志
	ETH_DMAClearITPendingBit(ETH_DMA_IT_R);
	ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS);  
    DrvInterruputLeave();
}

/*==================================================================
* Function      : DrvEthRxPacket
* Description   : 接收一个网卡数据包
* Input Para    : none
* Output Para   : none
* Return Value  : 网络数据包帧结构体
==================================================================*/
FrameTypeDef DrvEthRxPacket(void)
{ 
    u32 framelength=0;
    FrameTypeDef frame={0,0}; 
	//检查当前描述符,是否属于ETHERNET DMA(设置的时候)/CPU(复位的时候)
	if((DMARxDescToGet->Status&ETH_DMARxDesc_OWN)!=(u32)RESET)
	{	
        frame.length=ETH_ERROR; 
        if ((ETH->DMASR&ETH_DMASR_RBUS)!=(u32)RESET)  
        {
            //清除ETH DMA的RBUS位
            ETH->DMASR = ETH_DMASR_RBUS;
            //恢复DMA接收
            ETH->DMARPDR=0;
        }
        //错误,OWN位被设置了
        return frame;
    }
    //接收正确
    if(((DMARxDescToGet->Status&ETH_DMARxDesc_ES)==(u32)RESET)&& 
	((DMARxDescToGet->Status & ETH_DMARxDesc_LS)!=(u32)RESET)&&  
	((DMARxDescToGet->Status & ETH_DMARxDesc_FS)!=(u32)RESET))  
	{ 
        //得到接收包帧长度(不包含4字节CRC)
        framelength=((DMARxDescToGet->Status&ETH_DMARxDesc_FL)>>ETH_DMARxDesc_FrameLengthShift)-4;
        //得到包数据所在的位置
        frame.buffer = DMARxDescToGet->Buffer1Addr;
    }
    else
    {
        //错误
        framelength=ETH_ERROR;
    }
    frame.length=framelength; 
	frame.descriptor=DMARxDescToGet; 
	//更新ETH DMA全局Rx描述符为下一个Rx描述符
	//为下一次buffer读取设置下一个DMA Rx描述符
	DMARxDescToGet=(ETH_DMADESCTypeDef*)(DMARxDescToGet->Buffer2NextDescAddr);   
	return frame;      
}

/*==================================================================
* Function      : DrvEthTxPacket
* Description   : 发送一个网卡数据包
* Input Para    : FrameLength:数据包长度
* Output Para   : none
* Return Value  : ETH_ERROR,发送失败(0)，ETH_SUCCESS,发送成功(1)
==================================================================*/
u8 DrvEthTxPacket(u16 FrameLength)
{   
	//检查当前描述符,是否属于ETHERNET DMA(设置的时候)/CPU(复位的时候)
    //错误,OWN位被设置了
	if((DMATxDescToSet->Status&ETH_DMATxDesc_OWN)!=(u32)RESET)return ETH_ERROR;     
    //设置帧长度,bits[12:0]
    DMATxDescToSet->ControlBufferSize=(FrameLength&ETH_DMATxDesc_TBS1);
    //设置最后一个和第一个位段置位(1个描述符传输一帧)
    DMATxDescToSet->Status|=ETH_DMATxDesc_LS|ETH_DMATxDesc_FS;
    //设置Tx描述符的OWN位,buffer重归ETH DMA
    DMATxDescToSet->Status|=ETH_DMATxDesc_OWN;
    //当Tx Buffer不可用位(TBUS)被设置的时候,重置它.恢复传输
    if((ETH->DMASR&ETH_DMASR_TBUS)!=(u32)RESET)
	{ 
        //重置ETH DMA TBUS位
		ETH->DMASR=ETH_DMASR_TBUS; 
        //恢复DMA发送
		ETH->DMATPDR=0;
	} 
	//更新ETH DMA全局Tx描述符为下一个Tx描述符
	//为下一次buffer发送设置下一个DMA Tx描述符 
	DMATxDescToSet=(ETH_DMADESCTypeDef*)(DMATxDescToSet->Buffer2NextDescAddr);    
	return ETH_SUCCESS;     
}

/*==================================================================
* Function      : DrvEthMacBanding
* Description   : 设置ETH MAC层回调函数
* Input Para    : ETH 接收完成回调函数
* Output Para   : none
* Return Value  : none
==================================================================*/
void DrvEthMacBanding(void (*EthRxOverCallBack)(void))
{
    EthMacCallBack.EthRxOverCallBack = EthRxOverCallBack;   
}

/*==================================================================
* Function      : DrvEthNvicConfig
* Description   : 以太网中断分组配置
* Input Para    : none
* Output Para   : none
* Return Value  : none
==================================================================*/
void DrvEthNvicConfig(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
    /* 2 bit for pre-emption priority, 2 bits for subpriority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    
	//以太网中断
	NVIC_InitStructure.NVIC_IRQChannel = ETH_IRQn; 
    //中断寄存器组2最高优先级
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0X00; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0X00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/*==================================================================
* Function      : DrvEthGetCurrentTxBuffer
* Description   : 得到当前描述符的Tx buffer地址
* Input Para    : none
* Output Para   : none
* Return Value  : Tx buffer地址
==================================================================*/
u32 DrvEthGetCurrentTxBuffer(void)
{ 
    //返回Tx buffer地址 
    return DMATxDescToSet->Buffer1Addr;  
}

/*==================================================================
* Function      : DrvEthSetMemAddr
* Description   : 设置底层驱动申请内存地址
* Input Para    : DMARx 接收描述符 DMATx 发送描述符  Rx 驱动接收buffers Tx 驱动发送buffers
* Output Para   : none
* Return Value  : none
==================================================================*/
void DrvEthSetMemAddr(ETH_DMADESCTypeDef *DMARx,ETH_DMADESCTypeDef *DMATx,uint8_t *Rx,uint8_t *Tx)
{ 
	DMARxDscrTab = DMARx;
	DMATxDscrTab = DMATx;
	Rx_Buff = Rx;
	Tx_Buff = Tx;
}

/*==================================================================
* Function      : DrvEthGetMemLen
* Description   : 获取驱动底层内存需要申请长度
* Input Para    : DMARxLen 接收描述符 DMATxLen 发送描述符  RxLen 驱动接收buffers TxLen 驱动发送buffers
* Output Para   : none
* Return Value  : none
==================================================================*/
void DrvEthGetMemLen(uint16_t *DMARxLen,uint16_t *DMATxLen,uint16_t *RxLen,uint16_t *TxLen)
{ 
    *DMARxLen = ETH_RXBUFNB*sizeof(ETH_DMADESCTypeDef);
    *DMATxLen = ETH_TXBUFNB*sizeof(ETH_DMADESCTypeDef);
    *RxLen = ETH_RX_BUF_SIZE*ETH_RXBUFNB;
    *TxLen = ETH_TX_BUF_SIZE*ETH_TXBUFNB; 
}

/*==================================================================
* Function      : DrvEthMacStartUp
* Description   : 开启MAC和DMA
* Input Para    : mac 地址 hardcheckenable 硬件检测使能
* Output Para   : none
* Return Value  : none
==================================================================*/
void DrvEthMacStartUp(uint8_t * mac,bool hardcheckenable)
{
	//MAC地址寄存器中写入MAC地址
    uint8_t i = 0;
	ETH_MACAddressConfig(ETH_MAC_Address0, mac); 
	ETH_DMATxDescChainInit(DMATxDscrTab, Tx_Buff, ETH_TXBUFNB);
	ETH_DMARxDescChainInit(DMARxDscrTab, Rx_Buff, ETH_RXBUFNB);
	if(true == hardcheckenable)
    {
        for(i=0;i<ETH_TXBUFNB;i++)	
        {
            //使能TCP,UDP和ICMP的发送帧校验,TCP,UDP和ICMP的接收帧校验在DMA中配置了
            ETH_DMATxDescChecksumInsertionConfig(&DMATxDscrTab[i], ETH_DMATxDesc_ChecksumTCPUDPICMPFull);
        }
    }
    //开启MAC和DMA	
	ETH_Start(); 
}
