/****************************************************************************************
* FILE         : ethernetif.c
* Description  : 底层接口的驱动的模版，用户为自己的网络设备实现驱动时应参照此模块
*                netif是LwIP抽象出来的各网络接口，协议栈可以使用多个不同的接口，
*              而ethernetif则提供了netif访问硬件的各接口，每个不同的接口有不同的ethernetif。
* Copyright (c) 2022 by Tpson. All Rights Reserved.
*
* History:
* Version      Name            Date            Description
   0.1       zhongchao        2020/04/21      Initial Version

*****************************************************************************************
****************************************************************************************/
/****************************************************************************************
接收流程：
以太网中断--->EthernetifInput（上层）--->LowLevelInput（底层）---> ethernet_input（交给LWIP协议处理，IP数据或者ARP数据处理等）

发送流程：                         多播、广播
IP数据ip_output_if--->etharp_output（LWIP）--->LowLevelOutput（底层）
                                       ||
                                       ||
                                       \/     查找对应MAC
                                      单播--->etharp_query--->etharp_send_ip--->LowLevelOutput（底层）
                                                  ||
                                                  || ARP请求(数据包存放ARP表对应缓存中)        
                                                  \/
                                           etharp_request--->LowLevelOutput（底层）
                                                            
注意：如果带操作系统主要通过tcpip_thread（tcpip.c）线程来处理网络层及传输层协议，tcpip_thread循环调用sys_mbox_fetch从其他线程
读取数据(包括链路层及应用层)，lwip下层传递整个以太网帧给tcp/ip线程，处理ip报文前先要剥去以太网帧头部数据。
LWIP主线程，和其他线程间通过邮箱通讯。
*****************************************************************************************/
#include "netif/ethernetif.h" 
#include "lwip_comm.h" 
#include "netif/etharp.h"  
#include "string.h" 
#include "dp83848.h"
#include "drv_eth_mac.h"

//lwip控制结构体 在lwip_comm.c中
extern Lwip_Dev_Typedef gLwipDev;

/*==================================================================
* Function      : LowLevelInit
* Description   : 由ethernetif_init()调用用于初始化硬件
* Input Para    : netif:网卡结构体指针 
* Output Para   : none
* Return Value  : ERR_OK,发送正常 ERR_MEM,发送失败
==================================================================*/
//网卡复位以及参数初始化，实际网卡特性设置协议栈网络接口结构netif中网卡属性相关字段，
//如网卡mac地址长度等
static err_t LowLevelInit(struct netif *netif)
{
    //设置MAC地址长度,为6个字节
	netif->hwaddr_len = ETHARP_HWADDR_LEN; 
	//初始化MAC地址,设置什么地址由用户自己设置,但是不能与网络中其他设备MAC地址重复
	netif->hwaddr[0]=gLwipDev.Mac[0]; 
	netif->hwaddr[1]=gLwipDev.Mac[1]; 
	netif->hwaddr[2]=gLwipDev.Mac[2];
	netif->hwaddr[3]=gLwipDev.Mac[3];
	netif->hwaddr[4]=gLwipDev.Mac[4];
	netif->hwaddr[5]=gLwipDev.Mac[5];
    //最大允许传输单元,允许该网卡广播和ARP功能
	netif->mtu = 1500; 

	netif->flags = NETIF_FLAG_BROADCAST|NETIF_FLAG_ETHARP|NETIF_FLAG_LINK_UP;
    //启动网卡驱动
    Dp83848StartUp(netif->hwaddr);
	return ERR_OK;
} 

/*==================================================================
* Function      : LowLevelOutput
* Description   : 网卡数据包发送函数(底层，lwip通过netif->linkoutput指向该函数)
* Input Para    : netif:网卡结构体指针  p:pbuf数据结构体指针
* Output Para   : none
* Return Value  : ERR_OK,发送正常 ERR_MEM,发送失败
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
    //返回错误状态    
	if(res==ETH_ERROR)return ERR_MEM;
	return ERR_OK;
}  

/*==================================================================
* Function      : LowLevelInput
* Description   : 网卡数据包接收函数（底层），将收到的数据封装成pbuf形式
* Input Para    : netif:网卡结构体指针
* Output Para   : none
* Return Value  : pbuf数据结构体指针
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
    //得到包大小
	len=frame.length;
    //得到包数据地址 
	buffer=(u8 *)frame.buffer;
    //pbufs内存池分配pbuf
	p=pbuf_alloc(PBUF_RAW,len,PBUF_POOL);
	if(p!=NULL)
	{
		for(q=p;q!=NULL;q=q->next)
		{
			memcpy((u8_t*)q->payload,(u8_t*)&buffer[l], q->len);
			l=l+q->len;
		}    
	}
    //设置Rx描述符OWN位,buffer重归ETH DMA 
	frame.descriptor->Status=ETH_DMARxDesc_OWN;
    //当Rx Buffer不可用位(RBUS)被设置的时候,重置它.恢复传输
	if((ETH->DMASR&ETH_DMASR_RBUS)!=(u32)RESET)
	{ 
        //重置ETH DMA RBUS位
		ETH->DMASR=ETH_DMASR_RBUS; 
        //恢复DMA接收
		ETH->DMARPDR=0;
	}
	return p;
}

/*==================================================================
* Function      : EthernetifInput
* Description   : 网卡接收数据(lwip直接调用)
* Input Para    : netif:网卡结构体指针
* Output Para   : none
* Return Value  : ERR_OK,发送正常 ERR_MEM,发送失败
==================================================================*/
//调用底层网卡接收函数读取数据包，然后解析该数据包类型（1500，1518下：IP数据包
//1518上：ARP包），最好交给上层lwip协议处理
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
* Description   : 初始化网络
* Input Para    : netif:网卡结构体指针
* Output Para   : none
* Return Value  : ERR_OK,正常 其他,失败
==================================================================*/
//上层在管理网络接口接口netif调用，完成结构体中某些字段初始化，并最终调用LowLevelInit完成网卡初始化
err_t EthernetifInit(struct netif *netif)
{
	LWIP_ASSERT("netif!=NULL",(netif!=NULL));
    //LWIP_NETIF_HOSTNAME
#if LWIP_NETIF_HOSTNAME
    //初始化名称 
	netif->hostname = "lwip";  	
#endif 
    //初始化变量netif的name字段
	netif->name[0] = IFNAME0;
 	//在文件外定义这里不用关心具体值
	netif->name[1] = IFNAME1; 
    //IP层发送数据包函数	
	netif->output = etharp_output;
    //ARP模块发送数据包函数
	netif->linkoutput = LowLevelOutput;
    //底层硬件初始化函数
	LowLevelInit(netif); 		
	return ERR_OK;
}

