/****************************************************************************************
* FILE         : ethernetif.c
* Description  : �ײ�ӿڵ�������ģ�棬�û�Ϊ�Լ��������豸ʵ������ʱӦ���մ�ģ��
*                netif��LwIP��������ĸ�����ӿڣ�Э��ջ����ʹ�ö����ͬ�Ľӿڣ�
*              ��ethernetif���ṩ��netif����Ӳ���ĸ��ӿڣ�ÿ����ͬ�Ľӿ��в�ͬ��ethernetif��
* Copyright (c) 2022 by Tpson. All Rights Reserved.
*
* History:
* Version      Name            Date            Description
   0.1       zhongchao        2020/04/21      Initial Version

*****************************************************************************************
****************************************************************************************/
/****************************************************************************************
�������̣�
��̫���ж�--->EthernetifInput���ϲ㣩--->LowLevelInput���ײ㣩---> ethernet_input������LWIPЭ�鴦��IP���ݻ���ARP���ݴ���ȣ�

�������̣�                         �ಥ���㲥
IP����ip_output_if--->etharp_output��LWIP��--->LowLevelOutput���ײ㣩
                                       ||
                                       ||
                                       \/     ���Ҷ�ӦMAC
                                      ����--->etharp_query--->etharp_send_ip--->LowLevelOutput���ײ㣩
                                                  ||
                                                  || ARP����(���ݰ����ARP���Ӧ������)        
                                                  \/
                                           etharp_request--->LowLevelOutput���ײ㣩
                                                            
ע�⣺���������ϵͳ��Ҫͨ��tcpip_thread��tcpip.c���߳�����������㼰�����Э�飬tcpip_threadѭ������sys_mbox_fetch�������߳�
��ȡ����(������·�㼰Ӧ�ò�)��lwip�²㴫��������̫��֡��tcp/ip�̣߳�����ip����ǰ��Ҫ��ȥ��̫��֡ͷ�����ݡ�
LWIP���̣߳��������̼߳�ͨ������ͨѶ��
*****************************************************************************************/
#include "netif/ethernetif.h" 
#include "lwip_comm.h" 
#include "netif/etharp.h"  
#include "string.h" 
#include "dp83848.h"
#include "drv_eth_mac.h"

//lwip���ƽṹ�� ��lwip_comm.c��
extern Lwip_Dev_Typedef gLwipDev;

/*==================================================================
* Function      : LowLevelInit
* Description   : ��ethernetif_init()�������ڳ�ʼ��Ӳ��
* Input Para    : netif:�����ṹ��ָ�� 
* Output Para   : none
* Return Value  : ERR_OK,�������� ERR_MEM,����ʧ��
==================================================================*/
//������λ�Լ�������ʼ����ʵ��������������Э��ջ����ӿڽṹnetif��������������ֶΣ�
//������mac��ַ���ȵ�
static err_t LowLevelInit(struct netif *netif)
{
    //����MAC��ַ����,Ϊ6���ֽ�
	netif->hwaddr_len = ETHARP_HWADDR_LEN; 
	//��ʼ��MAC��ַ,����ʲô��ַ���û��Լ�����,���ǲ����������������豸MAC��ַ�ظ�
	netif->hwaddr[0]=gLwipDev.Mac[0]; 
	netif->hwaddr[1]=gLwipDev.Mac[1]; 
	netif->hwaddr[2]=gLwipDev.Mac[2];
	netif->hwaddr[3]=gLwipDev.Mac[3];
	netif->hwaddr[4]=gLwipDev.Mac[4];
	netif->hwaddr[5]=gLwipDev.Mac[5];
    //��������䵥Ԫ,����������㲥��ARP����
	netif->mtu = 1500; 

	netif->flags = NETIF_FLAG_BROADCAST|NETIF_FLAG_ETHARP|NETIF_FLAG_LINK_UP;
    //������������
    Dp83848StartUp(netif->hwaddr);
	return ERR_OK;
} 

/*==================================================================
* Function      : LowLevelOutput
* Description   : �������ݰ����ͺ���(�ײ㣬lwipͨ��netif->linkoutputָ��ú���)
* Input Para    : netif:�����ṹ��ָ��  p:pbuf���ݽṹ��ָ��
* Output Para   : none
* Return Value  : ERR_OK,�������� ERR_MEM,����ʧ��
==================================================================*/
static err_t LowLevelOutput(struct netif *netif, struct pbuf *p)
{
	u8 res;
	struct pbuf *q;
	int l = 0;
	u8 *buffer=(u8 *)DrvEthGetCurrentTxBuffer(); 
	for(q=p;q!=NULL;q=q->next) 
	{
		memcpy((u8_t*)&buffer[l], q->payload, q->len);
		l=l+q->len;
	} 
	res = DrvEthTxPacket(l);
    //���ش���״̬    
	if(res==ETH_ERROR)return ERR_MEM;
	return ERR_OK;
}  

/*==================================================================
* Function      : LowLevelInput
* Description   : �������ݰ����պ������ײ㣩�����յ������ݷ�װ��pbuf��ʽ
* Input Para    : netif:�����ṹ��ָ��
* Output Para   : none
* Return Value  : pbuf���ݽṹ��ָ��
==================================================================*/
static struct pbuf * LowLevelInput(struct netif *netif)
{  
	struct pbuf *p, *q;
	u16_t len;
	int l =0;
	FrameTypeDef frame;
	u8 *buffer;
	p = NULL;
	frame=DrvEthRxPacket();
    //�õ�����С
	len=frame.length;
    //�õ������ݵ�ַ 
	buffer=(u8 *)frame.buffer;
    //pbufs�ڴ�ط���pbuf
	p=pbuf_alloc(PBUF_RAW,len,PBUF_POOL);
	if(p!=NULL)
	{
		for(q=p;q!=NULL;q=q->next)
		{
			memcpy((u8_t*)q->payload,(u8_t*)&buffer[l], q->len);
			l=l+q->len;
		}    
	}
    //����Rx������OWNλ,buffer�ع�ETH DMA 
	frame.descriptor->Status=ETH_DMARxDesc_OWN;
    //��Rx Buffer������λ(RBUS)�����õ�ʱ��,������.�ָ�����
	if((ETH->DMASR&ETH_DMASR_RBUS)!=(u32)RESET)
	{ 
        //����ETH DMA RBUSλ
		ETH->DMASR=ETH_DMASR_RBUS; 
        //�ָ�DMA����
		ETH->DMARPDR=0;
	}
	return p;
}

/*==================================================================
* Function      : EthernetifInput
* Description   : ������������(lwipֱ�ӵ���)
* Input Para    : netif:�����ṹ��ָ��
* Output Para   : none
* Return Value  : ERR_OK,�������� ERR_MEM,����ʧ��
==================================================================*/
//���õײ��������պ�����ȡ���ݰ���Ȼ����������ݰ����ͣ�1500��1518�£�IP���ݰ�
//1518�ϣ�ARP��������ý����ϲ�lwipЭ�鴦��
err_t EthernetifInput(struct netif *netif)
{
	err_t err;
	struct pbuf *p;
	p=LowLevelInput(netif);
	if(p==NULL) return ERR_MEM;
	err=netif->input(p, netif);
	if(err!=ERR_OK)
	{
		LWIP_DEBUGF(NETIF_DEBUG,("ethernetif_input: IP input error\n"));
		pbuf_free(p);
		p = NULL;
	} 
	return err;
} 

/*==================================================================
* Function      : EthernetifInit
* Description   : ��ʼ������
* Input Para    : netif:�����ṹ��ָ��
* Output Para   : none
* Return Value  : ERR_OK,���� ����,ʧ��
==================================================================*/
//�ϲ��ڹ�������ӿڽӿ�netif���ã���ɽṹ����ĳЩ�ֶγ�ʼ���������յ���LowLevelInit���������ʼ��
err_t EthernetifInit(struct netif *netif)
{
	LWIP_ASSERT("netif!=NULL",(netif!=NULL));
    //LWIP_NETIF_HOSTNAME
#if LWIP_NETIF_HOSTNAME
    //��ʼ������ 
	netif->hostname = "lwip";  	
#endif 
    //��ʼ������netif��name�ֶ�
	netif->name[0] = IFNAME0;
 	//���ļ��ⶨ�����ﲻ�ù��ľ���ֵ
	netif->name[1] = IFNAME1; 
    //IP�㷢�����ݰ�����	
	netif->output = etharp_output;
    //ARPģ�鷢�����ݰ�����
	netif->linkoutput = LowLevelOutput;
    //�ײ�Ӳ����ʼ������
	LowLevelInit(netif); 		
	return ERR_OK;
}

