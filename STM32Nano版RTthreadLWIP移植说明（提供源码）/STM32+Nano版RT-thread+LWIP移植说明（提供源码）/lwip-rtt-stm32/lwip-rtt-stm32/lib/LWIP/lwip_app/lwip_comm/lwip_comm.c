/****************************************************************************************
* FILE         : lwip_comm.c
* Description  :  ��LWIPԴ�����̫������(DP83848.c��ethernetif.c)�������������
*
* Copyright (c) 2022 by Tpson. All Rights Reserved.
*
* History:
* Version      Name            Date            Description
   0.1       zhongchao        2020/04/20      Initial Version

****************************************************************************************
****************************************************************************************/

/***************************************************************************************
������
MCU��STM32��<--->MAC��DMA MAC��̫������ģ�飩<--->PHY��DP83848��<--->RJ45

�ֲ㣺
Ӧ�ò㣺DNS��������������DHCP����̬��������Э�飩��SNMP�����������Э�飩��HTTP�����ı�����Э�飩��API��BSD Scocket(unixͨ������ӿ�)
����㣺TCP��UDP
����㣺IPv4��IPv6��ICMP��������ԡ�ά������IGMP����������ಥ���ݽ��գ���ARP����̫����ַ����Э�飩
����ӿڲ㣺PPP����Ե㣩��SLIP��������·������̫�����ػ��ӿ�

���ݽṹ�����ģʽ����
                                                                IP���ݰ� ------>IP��
                                                                  /\
                                                                  ||
                                                                  ||
                                                                  \/
            Ŀ�ĵ�ַ��6�ֽڣ�+Դ��ַ��6�ֽڣ�+��̫�����ͣ�2�ֽڣ�+���ݣ�46~1500�ֽڣ�+FCS��4�ֽ�֡У�飩------> MAC��
                                                    /\
                                                    ||
                                                    ||
                                                    \/
 ��ǰͬ���루7�ֽڣ�+֡��ʼ�������1�ֽڣ��� + ��̫��MAC֡ ------>�����
               /\
               ||
               ||
               \/
          ���������룩
������̫�����ͣ�0x0806---ARP�� ��0x0800---IP���ݰ� ��0x8864---PPPoE���ݰ�          
*******************************************************************************************/
#include "lwip_comm.h" 
#include "lwip/dhcp.h"
#include "lwip/init.h"
#include "netif/etharp.h"
#include "ethernetif.h" 
#include "lwip/tcp_impl.h"
#include "dp83848.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include <rthw.h>
#include "lwip/timers.h"
//ʹ�ò���ϵͳ
#if !NO_SYS
#include "lwip/tcpip.h"
#include "arch/sys_arch.h"
#endif

//��memp.c���涨��
extern u32 memp_get_memorysize(void);	
extern u8_t *memp_memory;				
extern u8_t *ram_heap;					
//lwip���ƽṹ�� 
Lwip_Dev_Typedef gLwipDev = {0};
//����һ��ȫ�ֵ�����ӿ�
static struct netif gLwipNetif = {0};				

//lwip���ƽṹ�����ýӿ� 

/*==================================================================
* Function      : LwipCommMemFree
* Description   : lwip��mem��memp�ڴ��ͷ�
* Input Para    : none
* Output Para   : none
* Return Value  : none
==================================================================*/
static void LwipCommMemFree(void)
{ 
    //�ͷ��ڴ�
    if((memp_memory == NULL) || (ram_heap == NULL)) return;
	rt_free(memp_memory);
	rt_free(ram_heap); 
     #if !NO_SYS//ʹ�ò���ϵͳ
    //RTT����ö�̬��ʽ�����߳����赥�������߳�ջ
    //�ͷ��ں����������ջ 
	//�ͷ�dhcp�����ջ�����ڴ�ռ�
    #endif   
}

/*==================================================================
* Function      : LwipCommMemMalloc
* Description   : lwip��mem��memp���ڴ�����
* Input Para    : none
* Output Para   : none
* Return Value  : 0,�ɹ�;����,ʧ��
==================================================================*/
static u8 LwipCommMemMalloc(void)
{
	u32 mempsize;
	u32 ramheapsize; 
    //�õ�memp_memory�����С
	mempsize = memp_get_memorysize();
    //Ϊmemp_memory�����ڴ�
	memp_memory = rt_malloc(mempsize);
	//�õ�ram heap��С
	ramheapsize = LWIP_MEM_ALIGN_SIZE(MEM_SIZE)+2*LWIP_MEM_ALIGN_SIZE(4*3)+MEM_ALIGNMENT;
    //Ϊram_heap�����ڴ�
	ram_heap = rt_malloc(ramheapsize);	
    #if !NO_SYS//ʹ�ò���ϵͳ
    //RTT����ö�̬��ʽ�����߳����赥�������߳�ջ
    //���ں����������ջ 
	//��dhcp�����ջ�����ڴ�ռ�
    #endif
    //������ʧ�ܵ�
	if(!memp_memory||!ram_heap)
	{
		LwipCommMemFree();
		return 1;
	}    
    return 0;
}

/*==================================================================
* Function      : LwipCommDefaultIpSet
* Description   : lwip Ĭ��IP����
* Input Para    : lwipx:lwip���ƽṹ��ָ��
* Output Para   : none
* Return Value  : none
==================================================================*/
static void LwipCommDefaultIpSet(Lwip_Dev_Typedef *lwipx)
{
    u32 Sn0;
    u8 Mac[6] = {0};
    //�ж��Ƿ���Ҫʹ��lwipĬ�ϲ���
    if(memcmp(lwipx->Mac,Mac,6) != 0) return;
    //��ȡSTM32��ΨһID��ǰ24λ��ΪMAC��ַ�����ֽ�
    Sn0=*(vu32*)(0x1FFF7A10);
	//Ĭ��Զ��IPΪ:192.168.1.100
	lwipx->RemoteIp[0]=124;	
	lwipx->RemoteIp[1]=71;
	lwipx->RemoteIp[2]=168;
	lwipx->RemoteIp[3]=170;
	//MAC��ַ����(�����ֽڹ̶�Ϊ:2.0.0,�����ֽ���STM32ΨһID)
    //�����ֽ�(IEEE��֮Ϊ��֯ΨһID,OUI)��ַ�̶�Ϊ:2.0.0
	lwipx->Mac[0]=0x74;
	lwipx->Mac[1]=0xE3;
	lwipx->Mac[2]=0x85;
    //�����ֽ���STM32��ΨһID
	lwipx->Mac[3]=(Sn0>>16)&0XFF;
	lwipx->Mac[4]=(Sn0>>8)&0XFFF;;
	lwipx->Mac[5]=Sn0&0XFF; 
	//Ĭ�ϱ���IPΪ:192.168.1.30
	lwipx->Ip[0]=10;	
	lwipx->Ip[1]=0;
	lwipx->Ip[2]=0;
	lwipx->Ip[3]=98; 
 	//Ĭ����������:255.255.255.0
	lwipx->NetMask[0]=255;	
	lwipx->NetMask[1]=255;
	lwipx->NetMask[2]=254;
	lwipx->NetMask[3]=0;
	//Ĭ������:192.168.1.1
	lwipx->GateWay[0]=10;	
	lwipx->GateWay[1]=0;
	lwipx->GateWay[2]=0;
	lwipx->GateWay[3]=1;
	//û��DHCP
	lwipx->DhcpStatus=0;	   
}

/*==================================================================
* Function      : LwipPktHandle
* Description   : �������ݻص�����
* Input Para    : none
* Output Para   : none
* Return Value  : none
==================================================================*/
static void LwipPktHandle(void)
{
    //��ָ������ӿ����绺�����ж�ȡ���յ������ݰ������䷢�͸�LWIP���� 
    //֪ͨOS����ǰ�Ѿ��������ж�״̬
    rt_interrupt_enter();
    EthernetifInput(&gLwipNetif);
    rt_interrupt_leave();
}

//���ʹ����DHCP
#if LWIP_DHCP

//ʹ�ò���ϵͳ
#if !NO_SYS
//lwip DHCP����
//�����������ȼ�
#define LWIP_DHCP_TASK_PRIO       		9
//�߳̿��ƿ�ָ��
rt_thread_t gLwipDhcpThread = RT_NULL;

//ɾ��DHCP����
static void LwipCommDhcpDelete(u8 owner)
{
    //�ر�DHCP
	dhcp_stop(&gLwipNetif);
    //ɾ��DHCP����
    //RTTֱ��return���У������߳���Դ�ͷţ�ע���freeRtos�Ȳ��죩
    if(owner == 0)
    {
        rt_thread_delete(gLwipDhcpThread);
    }
    rt_kprintf("DHCP���������ɾ��DHCP TASK�̣߳�\r\n");   
}

/*==================================================================
* Function      : LwipDhcpTask
* Description   : DHCP��������
* Input Para    : none
* Output Para   : none
* Return Value  : none
==================================================================*/
static void LwipDhcpTask(void* parameter)
{
	u32 Ip=0,NetMask=0,Gw=0;
    //����DHCP
    dhcp_start(&gLwipNetif); 
    //����DHCP
	gLwipDev.DhcpStatus=0;	
    rt_kprintf("���ڲ���DHCP������,���Ե�...........\r\n");   
	while(1)
	{
        rt_kprintf("���ڻ�ȡ��ַ...\r\n");
        //��ȡ��IP��ַ
		Ip=gLwipNetif.ip_addr.addr;	
        //��ȡ��������            
	    NetMask=gLwipNetif.netmask.addr;
        //��ȡĬ������ 
	    Gw=gLwipNetif.gw.addr;			
	    //��ȷ��ȡ��IP��ַ��ʱ��
		if(Ip!=0)			
		{
            //DHCP�ɹ�
	        gLwipDev.DhcpStatus=2;	
            rt_kprintf("����en��MAC��ַΪ:................%d.%d.%d.%d.%d.%d\r\n",gLwipDev.Mac[0],gLwipDev.Mac[1],gLwipDev.Mac[2],gLwipDev.Mac[3],gLwipDev.Mac[4],gLwipDev.Mac[5]);
            //������ͨ��DHCP��ȡ����IP��ַ
            gLwipDev.Ip[3]=(uint8_t)(Ip>>24); 
            gLwipDev.Ip[2]=(uint8_t)(Ip>>16);
            gLwipDev.Ip[1]=(uint8_t)(Ip>>8);
            gLwipDev.Ip[0]=(uint8_t)(Ip);
            rt_kprintf("ͨ��DHCP��ȡ��IP��ַ..............%d.%d.%d.%d\r\n",gLwipDev.Ip[0],gLwipDev.Ip[1],gLwipDev.Ip[2],gLwipDev.Ip[3]);
            //����ͨ��DHCP��ȡ�������������ַ
            gLwipDev.NetMask[3]=(uint8_t)(NetMask>>24);
            gLwipDev.NetMask[2]=(uint8_t)(NetMask>>16);
            gLwipDev.NetMask[1]=(uint8_t)(NetMask>>8);
            gLwipDev.NetMask[0]=(uint8_t)(NetMask);
            rt_kprintf("ͨ��DHCP��ȡ����������............%d.%d.%d.%d\r\n",gLwipDev.NetMask[0],gLwipDev.NetMask[1],gLwipDev.NetMask[2],gLwipDev.NetMask[3]);
            //������ͨ��DHCP��ȡ����Ĭ������
            gLwipDev.GateWay[3]=(uint8_t)(Gw>>24);
            gLwipDev.GateWay[2]=(uint8_t)(Gw>>16);
            gLwipDev.GateWay[1]=(uint8_t)(Gw>>8);
            gLwipDev.GateWay[0]=(uint8_t)(Gw);
            rt_kprintf("ͨ��DHCP��ȡ����Ĭ������..........%d.%d.%d.%d\r\n",gLwipDev.GateWay[0],gLwipDev.GateWay[1],gLwipDev.GateWay[2],gLwipDev.GateWay[3]);
            break;
        }
        //ͨ��DHCP�����ȡIP��ַʧ��,�ҳ�������Դ���
        else if(gLwipNetif.dhcp->tries > LWIP_MAX_DHCP_TRIES) 
        {
            //DHCP��ʱʧ��.
            gLwipDev.DhcpStatus=0XFF;
            //ʹ�þ�̬IP��ַ
            IP4_ADDR(&(gLwipNetif.ip_addr),gLwipDev.Ip[0],gLwipDev.Ip[1],gLwipDev.Ip[2],gLwipDev.Ip[3]);
            IP4_ADDR(&(gLwipNetif.netmask),gLwipDev.NetMask[0],gLwipDev.NetMask[1],gLwipDev.NetMask[2],gLwipDev.NetMask[3]);
            IP4_ADDR(&(gLwipNetif.gw),gLwipDev.GateWay[0],gLwipDev.GateWay[1],gLwipDev.GateWay[2],gLwipDev.GateWay[3]);
            rt_kprintf("DHCP����ʱ,ʹ�þ�̬IP��ַ!\r\n");
            rt_kprintf("����en��MAC��ַΪ:................%d.%d.%d.%d.%d.%d\r\n",gLwipDev.Mac[0],gLwipDev.Mac[1],gLwipDev.Mac[2],gLwipDev.Mac[3],gLwipDev.Mac[4],gLwipDev.Mac[5]);
            rt_kprintf("��̬IP��ַ........................%d.%d.%d.%d\r\n",gLwipDev.Ip[0],gLwipDev.Ip[1],gLwipDev.Ip[2],gLwipDev.Ip[3]);
            rt_kprintf("��������..........................%d.%d.%d.%d\r\n",gLwipDev.NetMask[0],gLwipDev.NetMask[1],gLwipDev.NetMask[2],gLwipDev.NetMask[3]);
            rt_kprintf("Ĭ������..........................%d.%d.%d.%d\r\n",gLwipDev.GateWay[0],gLwipDev.GateWay[1],gLwipDev.GateWay[2],gLwipDev.GateWay[3]);
            break;
        }
        rt_thread_delay(500);
    }
    //ɾ��DHCP����  
    LwipCommDhcpDelete(1);            
}

/*==================================================================
* Function      : LwipCommDhcpCreat
* Description   : ����DHCP����
* Input Para    : none
* Output Para   : none
* Return Value  : none
==================================================================*/
static void LwipCommDhcpCreat(void)
{
    //ʹ�ö�̬�ڴ棬���ؿ��ƿ�ָ��
    gLwipDhcpThread = rt_thread_create(
                 "dhcptask",            /* �߳����� */
                 LwipDhcpTask,          /* �߳���ں��� */
                 RT_NULL,               /* �߳���ں������� */
                 4096,                  /* �߳�ջ��С */
                 LWIP_DHCP_TASK_PRIO,   /* �̵߳����ȼ� */
                 20);                   /* �߳�ʱ��Ƭ */
    if(gLwipDhcpThread != RT_NULL)
    {
        rt_thread_startup(gLwipDhcpThread); /* �����̣߳��������� */
        rt_kprintf("DHCP TASK�̴߳����ɹ���\r\n");
    }        
}

//ɾ��next_timeout()���ݽṹ(������times.c�ļ�)
//timers.c�ļ�
extern void ip_reass_timer(void *arg);
extern void arp_timer(void *arg);		
extern void dhcp_timer_coarse(void *arg);
extern void dhcp_timer_fine(void *arg); 
extern struct sys_timeo *next_timeout; 
extern int tcpip_tcp_timer_active;  
extern void dns_timer(void *arg);
static void lwip_comm_delete_next_timeout(void)
{
#if IP_REASSEMBLY   //IP_PREASSEMBLY = 1
	sys_untimeout(ip_reass_timer,NULL); 
#endif 
#if LWIP_ARP		//LWIP_ARP = 1
	sys_untimeout(arp_timer,NULL); 
#endif  
#if LWIP_DHCP      	//LWIP_DHCP = 1
	sys_untimeout(dhcp_timer_coarse,NULL); 
	sys_untimeout(dhcp_timer_fine,NULL); 
#endif   
#if LWIP_IGMP      	//LWIP_IGMP = 1
	sys_untimeout(igmp_timer,NULL);
#endif  
#if LWIP_DNS       	//LWIP_DNS = 1
	sys_untimeout(dns_timer,NULL);
#endif  
	if(next_timeout!=NULL)next_timeout=NULL;
	tcpip_tcp_timer_active=0;
}

//�ں��߳̿��ƿ�ָ��(sys_arch.c)
extern rt_thread_t TcpIpThread;
extern rt_mutex_t gLwipSysMutex;
//��Ϣ���� ȫ�ֱ���(��tcpip.c���涨��)
extern sys_mbox_t mbox;  				
//LWIP��Դ���պ������˳�LWIPʱʹ�ã�Ŀǰ��ʱ���ã�����һֱʹ�ã�
void LwipCommDestroy(void)
{
    //1.Ӳ����Դ�ͷ�
    //��λ��̫��
    Dp83848DeInit();
    //ɾ��lwip_netif����
    netif_remove(&gLwipNetif); 
    //2.IPC��Դ�ͷ�
    //ɾ��mbox��Ϣ����
    sys_mbox_free(&mbox);
    //ɾ�������ź���
    rt_mutex_delete(gLwipSysMutex);
    //ɾ����ʱ�¼������һ���¼�
    lwip_comm_delete_next_timeout(); 
    //3.ȫ�ֱ����ͷ�
    //......
    //4.�߳���Դ�ͷţ�RTTע�ⲻ���Լ��߳�ɾ����ֻ�ı��߳�״̬�����ڿ����߳��ͷ���Դ��
    #if LWIP_DHCP
    //dhcp����ɾ�� 
	LwipCommDhcpDelete(0);		
    #endif
    //LWIP�ں��߳�ɾ��
    rt_thread_delete(gLwipDhcpThread);    
    //5.��̬�ڴ���Դ�ͷ�
    //�ͷ�LWIP������ڴ�.
	LwipCommMemFree();
    //�ͷ���������������ڴ�
	Dp83848MemFree();
}

#else
/*==================================================================
* Function      : LwipDhcpProcessHandle
* Description   : DHCP��������
* Input Para    : none
* Output Para   : none
* Return Value  : none
==================================================================*/
static void LwipDhcpProcessHandle(void)
{
	u32 Ip=0,NetMask=0,Gw=0;
	switch(gLwipDev.DhcpStatus)
	{
        //����DHCP
		case 0: 	
			dhcp_start(&gLwipNetif);
            //�ȴ�ͨ��DHCP��ȡ���ĵ�ַ
			gLwipDev.DhcpStatus = 1;		
			rt_kprintf("���ڲ���DHCP������,���Ե�...........\r\n");  
			break;
        //�ȴ���ȡ��IP��ַ
		case 1:		
		{
            //��ȡ��IP��ַ
			Ip=gLwipNetif.ip_addr.addr;	
            //��ȡ��������            
			NetMask=gLwipNetif.netmask.addr;
            //��ȡĬ������ 
			Gw=gLwipNetif.gw.addr;			
			//��ȷ��ȡ��IP��ַ��ʱ��
			if(Ip!=0)			
			{
                //DHCP�ɹ�
				gLwipDev.DhcpStatus=2;	
				rt_kprintf("����en��MAC��ַΪ:................%d.%d.%d.%d.%d.%d\r\n",gLwipDev.Mac[0],gLwipDev.Mac[1],gLwipDev.Mac[2],gLwipDev.Mac[3],gLwipDev.Mac[4],gLwipDev.Mac[5]);
				//������ͨ��DHCP��ȡ����IP��ַ
				gLwipDev.Ip[3]=(uint8_t)(Ip>>24); 
				gLwipDev.Ip[2]=(uint8_t)(Ip>>16);
				gLwipDev.Ip[1]=(uint8_t)(Ip>>8);
				gLwipDev.Ip[0]=(uint8_t)(Ip);
				rt_kprintf("ͨ��DHCP��ȡ��IP��ַ..............%d.%d.%d.%d\r\n",gLwipDev.Ip[0],gLwipDev.Ip[1],gLwipDev.Ip[2],gLwipDev.Ip[3]);
				//����ͨ��DHCP��ȡ�������������ַ
				gLwipDev.NetMask[3]=(uint8_t)(NetMask>>24);
				gLwipDev.NetMask[2]=(uint8_t)(NetMask>>16);
				gLwipDev.NetMask[1]=(uint8_t)(NetMask>>8);
				gLwipDev.NetMask[0]=(uint8_t)(NetMask);
				rt_kprintf("ͨ��DHCP��ȡ����������............%d.%d.%d.%d\r\n",gLwipDev.NetMask[0],gLwipDev.NetMask[1],gLwipDev.NetMask[2],gLwipDev.NetMask[3]);
				//������ͨ��DHCP��ȡ����Ĭ������
				gLwipDev.GateWay[3]=(uint8_t)(Gw>>24);
				gLwipDev.GateWay[2]=(uint8_t)(Gw>>16);
				gLwipDev.GateWay[1]=(uint8_t)(Gw>>8);
				gLwipDev.GateWay[0]=(uint8_t)(Gw);
				rt_kprintf("ͨ��DHCP��ȡ����Ĭ������..........%d.%d.%d.%d\r\n",gLwipDev.GateWay[0],gLwipDev.GateWay[1],gLwipDev.GateWay[2],gLwipDev.GateWay[3]);
			}
            //ͨ��DHCP�����ȡIP��ַʧ��,�ҳ�������Դ���
            else if(gLwipNetif.dhcp->tries > LWIP_MAX_DHCP_TRIES) 
			{
				gLwipDev.DhcpStatus=0XFF;//DHCP��ʱʧ��.
				//ʹ�þ�̬IP��ַ
				IP4_ADDR(&(gLwipNetif.ip_addr),gLwipDev.Ip[0],gLwipDev.Ip[1],gLwipDev.Ip[2],gLwipDev.Ip[3]);
				IP4_ADDR(&(gLwipNetif.netmask),gLwipDev.NetMask[0],gLwipDev.NetMask[1],gLwipDev.NetMask[2],gLwipDev.NetMask[3]);
				IP4_ADDR(&(gLwipNetif.gw),gLwipDev.GateWay[0],gLwipDev.GateWay[1],gLwipDev.GateWay[2],gLwipDev.GateWay[3]);
				rt_kprintf("DHCP����ʱ,ʹ�þ�̬IP��ַ!\r\n");
				rt_kprintf("����en��MAC��ַΪ:................%d.%d.%d.%d.%d.%d\r\n",gLwipDev.Mac[0],gLwipDev.Mac[1],gLwipDev.Mac[2],gLwipDev.Mac[3],gLwipDev.Mac[4],gLwipDev.Mac[5]);
				rt_kprintf("��̬IP��ַ........................%d.%d.%d.%d\r\n",gLwipDev.Ip[0],gLwipDev.Ip[1],gLwipDev.Ip[2],gLwipDev.Ip[3]);
				rt_kprintf("��������..........................%d.%d.%d.%d\r\n",gLwipDev.NetMask[0],gLwipDev.NetMask[1],gLwipDev.NetMask[2],gLwipDev.NetMask[3]);
				rt_kprintf("Ĭ������..........................%d.%d.%d.%d\r\n",gLwipDev.GateWay[0],gLwipDev.GateWay[1],gLwipDev.GateWay[2],gLwipDev.GateWay[3]);
			}
		}
		break;
		default : break;
	}    
}

#endif
#endif

//��ʹ�ò���ϵͳ
#if NO_SYS
/*==================================================================
* Function      : LwipPeriodicHandle
* Description   : LWIP��ѯ����10ms���ں������ڶ�ʱ����TCP��ARP��DHCP��
* Input Para    : none
* Output Para   : none
* Return Value  : none
==================================================================*/
void LwipPeriodicHandle(void)
{
    //TCP��ѯ��ʱ��
    static u32 gTCPTimer=0;
    //ARP��ѯ��ʱ��
    static u32 gARPTimer=0;
    //lwip����ʱ�������,��λ:ms
    static u32 gLwipLocalTime;
	gLwipLocalTime = rt_tick_get();
    #if LWIP_DHCP
    //DHCP��ϸ�����ʱ��
    static u32 gDHCPfineTimer=0;	
    //DHCP�ֲڴ����ʱ��
    static u32 gDHCPcoarseTimer=0;	
    #endif
#if LWIP_TCP
    //Э��ջҪ��ÿ250ms����һ��tcp_tmr()����
    if(gLwipLocalTime - gTCPTimer >= TCP_TMR_INTERVAL)
    {
        gTCPTimer =  gLwipLocalTime;
        tcp_tmr();
    }
#endif    
    //ARPÿ5s�����Ե���һ��
    if ((gLwipLocalTime - gARPTimer) >= ARP_TMR_INTERVAL)
    {
        gARPTimer =  gLwipLocalTime;
        etharp_tmr();
    }    
    //���ʹ��DHCP�Ļ�
#if LWIP_DHCP 
    //ÿ500ms����һ��dhcp_fine_tmr()
    if (gLwipLocalTime - gDHCPfineTimer >= DHCP_FINE_TIMER_MSECS)
    {
        gDHCPfineTimer =  gLwipLocalTime;
        dhcp_fine_tmr();
        if ((gLwipDev.DhcpStatus != 2)&&(gLwipDev.DhcpStatus != 0XFF))
        { 
            //DHCP����
            LwipDhcpProcessHandle();  
        }
    }
    //ÿ60sִ��һ��DHCP�ֲڴ���
    if (gLwipLocalTime - gDHCPcoarseTimer >= DHCP_COARSE_TIMER_MSECS)
    {
        gDHCPcoarseTimer =  gLwipLocalTime;
        dhcp_coarse_tmr();
    }  
#endif    
    
}
#endif

/*==================================================================
* Function      : LwipCommInit
* Description   : LWIP��ʼ��(LWIP������ʱ��ʹ��)
* Input Para    : none
* Output Para   : none
* Return Value  : 0,�ɹ� 1,�ڴ���� 2,DB83848��ʼ��ʧ�� 3,�������ʧ��
==================================================================*/
u8 LwipCommInit(void)
{
    //����netif_add()����ʱ�ķ���ֵ,�����ж������ʼ���Ƿ�ɹ�
    struct netif *NetifInitFlag;
    //ip��ַ
    struct ip_addr IpAddr; 
    //��������
	struct ip_addr NetMask;
    //Ĭ������ 
	struct ip_addr Gw;      			
    //1.���������ڴ�����
    if(Dp83848MemMalloc()) return 1;
    //2.lwip�ڴ�����
    if(LwipCommMemMalloc())return 1;
    //3.��ʼ��DP83848 
	if(Dp83848Init())return 2;    
    //4.��ʼ��LWIP�ں�
    //ʹ�ò���ϵͳ
    #if !NO_SYS
    //��ʼ��tcp ip�ں�,�ú�������ᴴ��tcpip_thread�ں�����
    tcpip_init(NULL,NULL);				
    #else
    lwip_init();
    #endif
    //5.����Ĭ��IP����Ϣ
    LwipCommDefaultIpSet(&gLwipDev);
    //6.DHCP
    //ʹ�ö�̬IP
#if LWIP_DHCP		
	IpAddr.addr = 0;
	NetMask.addr = 0;
	Gw.addr = 0;
    //ʹ�þ�̬IP 
#else				 
	IP4_ADDR(&IpAddr,gLwipDev.Ip[0],gLwipDev.Ip[1],gLwipDev.Ip[2],gLwipDev.Ip[3]);
	IP4_ADDR(&NetMask,gLwipDev.NetMask[0],gLwipDev.NetMask[1] ,gLwipDev.NetMask[2],gLwipDev.NetMask[3]);
	IP4_ADDR(&Gw,gLwipDev.GateWay[0],gLwipDev.GateWay[1],gLwipDev.GateWay[2],gLwipDev.GateWay[3]);
#endif 
    //7.���������������ݻص�����
    Dp83848Banding(LwipPktHandle);
    //8.�������б������һ������    
//��ʹ�ò���ϵͳ    
#if NO_SYS     
    NetifInitFlag=netif_add(&gLwipNetif,&IpAddr,&NetMask,&Gw,NULL,&EthernetifInit,&ethernet_input);
//���ʹ��DHCP�Ļ�
#if LWIP_DHCP
    //DHCP���Ϊ0
	gLwipDev.DhcpStatus=0;
	//����DHCP����
	dhcp_start(&gLwipNetif);	
#endif 
#else
    NetifInitFlag=netif_add(&gLwipNetif,&IpAddr,&NetMask,&Gw,NULL,&EthernetifInit,&tcpip_input);
#endif
    //�������ʧ�� 
    if(NetifInitFlag==NULL)return 3;
    //8.������ӳɹ���,����netifΪĬ��ֵ,���Ҵ�netif����
	else
	{
        //����netifΪĬ������
		netif_set_default(&gLwipNetif);
        //��netif����
		netif_set_up(&gLwipNetif);		
	}
    //ʹ�ò���ϵͳ
    #if !NO_SYS
    //����DHCP����
    LwipCommDhcpCreat();
    #endif
    //����OK.
    return 0;
}

