/****************************************************************************************
* FILE         : drv_eth_mac.c
* Description  : STM ETH MAC ��������
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

//ETH MAC�ص������ṹ��
typedef struct
{
    void (*EthRxOverCallBack)(void);
}EthMac_CallBack_Typedef;
static EthMac_CallBack_Typedef EthMacCallBack = {NULL};
//DMA����������׷��ָ��
extern ETH_DMADESCTypeDef  *DMATxDescToSet;			
//DMA����������׷��ָ��
extern ETH_DMADESCTypeDef  *DMARxDescToGet;
//��̫��DMA�������������ݽṹ��ָ��
__align(4) ETH_DMADESCTypeDef *DMARxDscrTab;
//��̫��DMA�������������ݽṹ��ָ��
__align(4) ETH_DMADESCTypeDef *DMATxDscrTab;
//��̫���ײ���������buffersָ�� 
__align(4) uint8_t *Rx_Buff; 	
//��̫���ײ���������buffersָ��
__align(4) uint8_t *Tx_Buff; 					


/*****************************���²�����������/�ӿں���***********************/
/*==================================================================
* Function      : DrvEthMacDMacConfig
* Description   : ��ʼ��ETH MAC�㼰DMA����
* Input Para    : phyaddr : PHY��ַ
* Output Para   : none
* Return Value  : ��� 0,ʧ��;1,�ɹ�
==================================================================*/
u8 DrvEthMacDMacConfig(uint16_t phyaddr,ENUM_INTERFACE_TYPE type)
{
    u8 rval;
    ETH_InitTypeDef ETH_InitStructure; 
    
    //ʹ����̫��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_ETH_MAC | RCC_AHB1Periph_ETH_MAC_Tx |RCC_AHB1Periph_ETH_MAC_Rx, ENABLE);
    //MAC��PHY֮��ʹ�ýӿ�
    if(type == INTERFACE_MII)
    {
        SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_MII);
    }
    else
    {
        SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_RMII);
    }
    //AHB����������̫��
    ETH_DeInit(); 
    //�����������
    ETH_SoftwareReset();
    //�ȴ���������������
    while (ETH_GetSoftwareResetStatus() == SET);  
    //��ʼ������ΪĬ��ֵ
    ETH_StructInit(&ETH_InitStructure); 

	//����MAC�������� 
    //������������Ӧ����    
	ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;
    //�رշ���
    ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;	
    //�ر��ش�����
    ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable;  
    //�ر��Զ�ȥ��PDA/CRC���� 
    ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;
    //�رս������е�֡
    ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Disable;
    //����������й㲥֡
    ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;
    //�رջ��ģʽ�ĵ�ַ����
    ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;
    //�����鲥��ַʹ��������ַ����
    ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;
    //�Ե�����ַʹ��������ַ����
    ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;
#ifdef CHECKSUM_BY_HARDWARE
    //����ipv4��TCP/UDP/ICMP��֡У���ж��
	ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable; 			   
#endif 
    //������ʹ��֡У���ж�ع��ܵ�ʱ��һ��Ҫʹ�ܴ洢ת��ģʽ,�洢ת��ģʽ��Ҫ��֤����֡�洢��FIFO��,
    //����MAC�ܲ���/ʶ���֡У��ֵ,����У����ȷ��ʱ��DMA�Ϳ��Դ���֡,����Ͷ�������֡
    //��������TCP/IP����֡
    ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Enable;
    //�����������ݵĴ洢ת��ģʽ 
    ETH_InitStructure.ETH_ReceiveStoreForward = ETH_ReceiveStoreForward_Enable;
    //�����������ݵĴ洢ת��ģʽ
    ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;
    //��ֹת������֡ 
    ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Disable;
    //��ת����С�ĺ�֡ 
    ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Disable;
    //�򿪴���ڶ�֡����
    ETH_InitStructure.ETH_SecondFrameOperate = ETH_SecondFrameOperate_Enable;
    //����DMA����ĵ�ַ���빦��
    ETH_InitStructure.ETH_AddressAlignedBeats = ETH_AddressAlignedBeats_Enable; 
    //�����̶�ͻ������
    ETH_InitStructure.ETH_FixedBurst = ETH_FixedBurst_Enable;
    //DMA���͵����ͻ������Ϊ32������
    ETH_InitStructure.ETH_RxDMABurstLength = ETH_RxDMABurstLength_32Beat;
    //DMA���յ����ͻ������Ϊ32������
    ETH_InitStructure.ETH_TxDMABurstLength = ETH_TxDMABurstLength_32Beat;
    //ѡ��DMA����/�����ٲ�
    ETH_InitStructure.ETH_DMAArbitration = ETH_DMAArbitration_RoundRobin_RxTx_2_1;
    //����ETH
    rval=ETH_Init(&ETH_InitStructure,phyaddr);
    //���óɹ�
    if(rval==ETH_SUCCESS)
	{
        //ʹ����̫�������ж�
		ETH_DMAITConfig(ETH_DMA_IT_NIS|ETH_DMA_IT_R,ENABLE);  		
	}
	return rval;
}

/*==================================================================
* Function      : ETH_IRQHandler
* Description   : ��̫���жϷ�����
* Input Para    : none
* Output Para   : none
* Return Value  : none
==================================================================*/
void ETH_IRQHandler(void)
{
    DrvInterruputEnter();
    //����Ƿ��յ����ݰ�
	while(ETH_GetRxPktSize(DMARxDescToGet)!=0) 	
	{ 
        //���ջص�����
        if(NULL != EthMacCallBack.EthRxOverCallBack)
        {
            EthMacCallBack.EthRxOverCallBack();
        }            
	}
    //����жϱ�־
	ETH_DMAClearITPendingBit(ETH_DMA_IT_R);
	ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS);  
    DrvInterruputLeave();
}

/*==================================================================
* Function      : DrvEthRxPacket
* Description   : ����һ���������ݰ�
* Input Para    : none
* Output Para   : none
* Return Value  : �������ݰ�֡�ṹ��
==================================================================*/
FrameTypeDef DrvEthRxPacket(void)
{ 
    u32 framelength=0;
    FrameTypeDef frame={0,0}; 
	//��鵱ǰ������,�Ƿ�����ETHERNET DMA(���õ�ʱ��)/CPU(��λ��ʱ��)
	if((DMARxDescToGet->Status&ETH_DMARxDesc_OWN)!=(u32)RESET)
	{	
        frame.length=ETH_ERROR; 
        if ((ETH->DMASR&ETH_DMASR_RBUS)!=(u32)RESET)  
        {
            //���ETH DMA��RBUSλ
            ETH->DMASR = ETH_DMASR_RBUS;
            //�ָ�DMA����
            ETH->DMARPDR=0;
        }
        //����,OWNλ��������
        return frame;
    }
    //������ȷ
    if(((DMARxDescToGet->Status&ETH_DMARxDesc_ES)==(u32)RESET)&& 
	((DMARxDescToGet->Status & ETH_DMARxDesc_LS)!=(u32)RESET)&&  
	((DMARxDescToGet->Status & ETH_DMARxDesc_FS)!=(u32)RESET))  
	{ 
        //�õ����հ�֡����(������4�ֽ�CRC)
        framelength=((DMARxDescToGet->Status&ETH_DMARxDesc_FL)>>ETH_DMARxDesc_FrameLengthShift)-4;
        //�õ����������ڵ�λ��
        frame.buffer = DMARxDescToGet->Buffer1Addr;
    }
    else
    {
        //����
        framelength=ETH_ERROR;
    }
    frame.length=framelength; 
	frame.descriptor=DMARxDescToGet; 
	//����ETH DMAȫ��Rx������Ϊ��һ��Rx������
	//Ϊ��һ��buffer��ȡ������һ��DMA Rx������
	DMARxDescToGet=(ETH_DMADESCTypeDef*)(DMARxDescToGet->Buffer2NextDescAddr);   
	return frame;      
}

/*==================================================================
* Function      : DrvEthTxPacket
* Description   : ����һ���������ݰ�
* Input Para    : FrameLength:���ݰ�����
* Output Para   : none
* Return Value  : ETH_ERROR,����ʧ��(0)��ETH_SUCCESS,���ͳɹ�(1)
==================================================================*/
u8 DrvEthTxPacket(u16 FrameLength)
{   
	//��鵱ǰ������,�Ƿ�����ETHERNET DMA(���õ�ʱ��)/CPU(��λ��ʱ��)
    //����,OWNλ��������
	if((DMATxDescToSet->Status&ETH_DMATxDesc_OWN)!=(u32)RESET)return ETH_ERROR;     
    //����֡����,bits[12:0]
    DMATxDescToSet->ControlBufferSize=(FrameLength&ETH_DMATxDesc_TBS1);
    //�������һ���͵�һ��λ����λ(1������������һ֡)
    DMATxDescToSet->Status|=ETH_DMATxDesc_LS|ETH_DMATxDesc_FS;
    //����Tx��������OWNλ,buffer�ع�ETH DMA
    DMATxDescToSet->Status|=ETH_DMATxDesc_OWN;
    //��Tx Buffer������λ(TBUS)�����õ�ʱ��,������.�ָ�����
    if((ETH->DMASR&ETH_DMASR_TBUS)!=(u32)RESET)
	{ 
        //����ETH DMA TBUSλ
		ETH->DMASR=ETH_DMASR_TBUS; 
        //�ָ�DMA����
		ETH->DMATPDR=0;
	} 
	//����ETH DMAȫ��Tx������Ϊ��һ��Tx������
	//Ϊ��һ��buffer����������һ��DMA Tx������ 
	DMATxDescToSet=(ETH_DMADESCTypeDef*)(DMATxDescToSet->Buffer2NextDescAddr);    
	return ETH_SUCCESS;     
}

/*==================================================================
* Function      : DrvEthMacBanding
* Description   : ����ETH MAC��ص�����
* Input Para    : ETH ������ɻص�����
* Output Para   : none
* Return Value  : none
==================================================================*/
void DrvEthMacBanding(void (*EthRxOverCallBack)(void))
{
    EthMacCallBack.EthRxOverCallBack = EthRxOverCallBack;   
}

/*==================================================================
* Function      : DrvEthNvicConfig
* Description   : ��̫���жϷ�������
* Input Para    : none
* Output Para   : none
* Return Value  : none
==================================================================*/
void DrvEthNvicConfig(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
    /* 2 bit for pre-emption priority, 2 bits for subpriority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    
	//��̫���ж�
	NVIC_InitStructure.NVIC_IRQChannel = ETH_IRQn; 
    //�жϼĴ�����2������ȼ�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0X00; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0X00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/*==================================================================
* Function      : DrvEthGetCurrentTxBuffer
* Description   : �õ���ǰ��������Tx buffer��ַ
* Input Para    : none
* Output Para   : none
* Return Value  : Tx buffer��ַ
==================================================================*/
u32 DrvEthGetCurrentTxBuffer(void)
{ 
    //����Tx buffer��ַ 
    return DMATxDescToSet->Buffer1Addr;  
}

/*==================================================================
* Function      : DrvEthSetMemAddr
* Description   : ���õײ����������ڴ��ַ
* Input Para    : DMARx ���������� DMATx ����������  Rx ��������buffers Tx ��������buffers
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
* Description   : ��ȡ�����ײ��ڴ���Ҫ���볤��
* Input Para    : DMARxLen ���������� DMATxLen ����������  RxLen ��������buffers TxLen ��������buffers
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
* Description   : ����MAC��DMA
* Input Para    : mac ��ַ hardcheckenable Ӳ�����ʹ��
* Output Para   : none
* Return Value  : none
==================================================================*/
void DrvEthMacStartUp(uint8_t * mac,bool hardcheckenable)
{
	//MAC��ַ�Ĵ�����д��MAC��ַ
    uint8_t i = 0;
	ETH_MACAddressConfig(ETH_MAC_Address0, mac); 
	ETH_DMATxDescChainInit(DMATxDscrTab, Tx_Buff, ETH_TXBUFNB);
	ETH_DMARxDescChainInit(DMARxDscrTab, Rx_Buff, ETH_RXBUFNB);
	if(true == hardcheckenable)
    {
        for(i=0;i<ETH_TXBUFNB;i++)	
        {
            //ʹ��TCP,UDP��ICMP�ķ���֡У��,TCP,UDP��ICMP�Ľ���֡У����DMA��������
            ETH_DMATxDescChecksumInsertionConfig(&DMATxDscrTab[i], ETH_DMATxDesc_ChecksumTCPUDPICMPFull);
        }
    }
    //����MAC��DMA	
	ETH_Start(); 
}
