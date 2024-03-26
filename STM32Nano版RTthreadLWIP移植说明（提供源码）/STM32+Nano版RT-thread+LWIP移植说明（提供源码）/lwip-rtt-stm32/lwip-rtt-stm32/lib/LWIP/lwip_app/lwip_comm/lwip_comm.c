/****************************************************************************************
* FILE         : lwip_comm.c
* Description  :  将LWIP源码和以太网驱动(DP83848.c和ethernetif.c)结合起来的桥梁
*
* Copyright (c) 2022 by Tpson. All Rights Reserved.
*
* History:
* Version      Name            Date            Description
   0.1       zhongchao        2020/04/20      Initial Version

****************************************************************************************
****************************************************************************************/

/***************************************************************************************
方案：
MCU（STM32）<--->MAC（DMA MAC以太网控制模块）<--->PHY（DP83848）<--->RJ45

分层：
应用层：DNS（域名解析）、DHCP（动态主机配置协议）、SNMP（简单网络管理协议）、HTTP（超文本传输协议）、API、BSD Scocket(unix通用网络接口)
传输层：TCP、UDP
网络层：IPv4、IPv6、ICMP（网络调试、维护）、IGMP（网络管理，多播数据接收）、ARP（以太网地址解析协议）
网络接口层：PPP（点对点）、SLIP（串行线路）、以太网、回环接口

数据结构（大端模式）：
                                                                IP数据包 ------>IP层
                                                                  /\
                                                                  ||
                                                                  ||
                                                                  \/
            目的地址（6字节）+源地址（6字节）+以太网类型（2字节）+数据（46~1500字节）+FCS（4字节帧校验）------> MAC层
                                                    /\
                                                    ||
                                                    ||
                                                    \/
 （前同步码（7字节）+帧开始定界符（1字节）） + 以太网MAC帧 ------>物理层
               /\
               ||
               ||
               \/
          （物理层插入）
其中以太网类型：0x0806---ARP包 、0x0800---IP数据包 、0x8864---PPPoE数据包          
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
//使用操作系统
#if !NO_SYS
#include "lwip/tcpip.h"
#include "arch/sys_arch.h"
#endif

//在memp.c里面定义
extern u32 memp_get_memorysize(void);	
extern u8_t *memp_memory;				
extern u8_t *ram_heap;					
//lwip控制结构体 
Lwip_Dev_Typedef gLwipDev = {0};
//定义一个全局的网络接口
static struct netif gLwipNetif = {0};				

//lwip控制结构体配置接口 

/*==================================================================
* Function      : LwipCommMemFree
* Description   : lwip中mem和memp内存释放
* Input Para    : none
* Output Para   : none
* Return Value  : none
==================================================================*/
static void LwipCommMemFree(void)
{ 
    //释放内存
    if((memp_memory == NULL) || (ram_heap == NULL)) return;
	rt_free(memp_memory);
	rt_free(ram_heap); 
     #if !NO_SYS//使用操作系统
    //RTT如果用动态方式创建线程无需单独申请线程栈
    //释放内核任务申请堆栈 
	//释放dhcp任务堆栈申请内存空间
    #endif   
}

/*==================================================================
* Function      : LwipCommMemMalloc
* Description   : lwip中mem和memp的内存申请
* Input Para    : none
* Output Para   : none
* Return Value  : 0,成功;其他,失败
==================================================================*/
static u8 LwipCommMemMalloc(void)
{
	u32 mempsize;
	u32 ramheapsize; 
    //得到memp_memory数组大小
	mempsize = memp_get_memorysize();
    //为memp_memory申请内存
	memp_memory = rt_malloc(mempsize);
	//得到ram heap大小
	ramheapsize = LWIP_MEM_ALIGN_SIZE(MEM_SIZE)+2*LWIP_MEM_ALIGN_SIZE(4*3)+MEM_ALIGNMENT;
    //为ram_heap申请内存
	ram_heap = rt_malloc(ramheapsize);	
    #if !NO_SYS//使用操作系统
    //RTT如果用动态方式创建线程无需单独申请线程栈
    //给内核任务申请堆栈 
	//给dhcp任务堆栈申请内存空间
    #endif
    //有申请失败的
	if(!memp_memory||!ram_heap)
	{
		LwipCommMemFree();
		return 1;
	}    
    return 0;
}

/*==================================================================
* Function      : LwipCommDefaultIpSet
* Description   : lwip 默认IP设置
* Input Para    : lwipx:lwip控制结构体指针
* Output Para   : none
* Return Value  : none
==================================================================*/
static void LwipCommDefaultIpSet(Lwip_Dev_Typedef *lwipx)
{
    u32 Sn0;
    u8 Mac[6] = {0};
    //判断是否需要使用lwip默认参数
    if(memcmp(lwipx->Mac,Mac,6) != 0) return;
    //获取STM32的唯一ID的前24位作为MAC地址后三字节
    Sn0=*(vu32*)(0x1FFF7A10);
	//默认远端IP为:192.168.1.100
	lwipx->RemoteIp[0]=124;	
	lwipx->RemoteIp[1]=71;
	lwipx->RemoteIp[2]=168;
	lwipx->RemoteIp[3]=170;
	//MAC地址设置(高三字节固定为:2.0.0,低三字节用STM32唯一ID)
    //高三字节(IEEE称之为组织唯一ID,OUI)地址固定为:2.0.0
	lwipx->Mac[0]=0x74;
	lwipx->Mac[1]=0xE3;
	lwipx->Mac[2]=0x85;
    //低三字节用STM32的唯一ID
	lwipx->Mac[3]=(Sn0>>16)&0XFF;
	lwipx->Mac[4]=(Sn0>>8)&0XFFF;;
	lwipx->Mac[5]=Sn0&0XFF; 
	//默认本地IP为:192.168.1.30
	lwipx->Ip[0]=10;	
	lwipx->Ip[1]=0;
	lwipx->Ip[2]=0;
	lwipx->Ip[3]=98; 
 	//默认子网掩码:255.255.255.0
	lwipx->NetMask[0]=255;	
	lwipx->NetMask[1]=255;
	lwipx->NetMask[2]=254;
	lwipx->NetMask[3]=0;
	//默认网关:192.168.1.1
	lwipx->GateWay[0]=10;	
	lwipx->GateWay[1]=0;
	lwipx->GateWay[2]=0;
	lwipx->GateWay[3]=1;
	//没有DHCP
	lwipx->DhcpStatus=0;	   
}

/*==================================================================
* Function      : LwipPktHandle
* Description   : 接收数据回调函数
* Input Para    : none
* Output Para   : none
* Return Value  : none
==================================================================*/
static void LwipPktHandle(void)
{
    //从指定网络接口网络缓冲区中读取接收到的数据包并将其发送给LWIP处理 
    //通知OS，当前已经进入了中断状态
    rt_interrupt_enter();
    EthernetifInput(&gLwipNetif);
    rt_interrupt_leave();
}

//如果使能了DHCP
#if LWIP_DHCP

//使用操作系统
#if !NO_SYS
//lwip DHCP任务
//设置任务优先级
#define LWIP_DHCP_TASK_PRIO       		9
//线程控制块指针
rt_thread_t gLwipDhcpThread = RT_NULL;

//删除DHCP任务
static void LwipCommDhcpDelete(u8 owner)
{
    //关闭DHCP
	dhcp_stop(&gLwipNetif);
    //删除DHCP任务
    //RTT直接return就行，空闲线程资源释放（注意和freeRtos等差异）
    if(owner == 0)
    {
        rt_thread_delete(gLwipDhcpThread);
    }
    rt_kprintf("DHCP任务结束，删除DHCP TASK线程！\r\n");   
}

/*==================================================================
* Function      : LwipDhcpTask
* Description   : DHCP处理任务
* Input Para    : none
* Output Para   : none
* Return Value  : none
==================================================================*/
static void LwipDhcpTask(void* parameter)
{
	u32 Ip=0,NetMask=0,Gw=0;
    //开启DHCP
    dhcp_start(&gLwipNetif); 
    //正在DHCP
	gLwipDev.DhcpStatus=0;	
    rt_kprintf("正在查找DHCP服务器,请稍等...........\r\n");   
	while(1)
	{
        rt_kprintf("正在获取地址...\r\n");
        //读取新IP地址
		Ip=gLwipNetif.ip_addr.addr;	
        //读取子网掩码            
	    NetMask=gLwipNetif.netmask.addr;
        //读取默认网关 
	    Gw=gLwipNetif.gw.addr;			
	    //正确获取到IP地址的时候
		if(Ip!=0)			
		{
            //DHCP成功
	        gLwipDev.DhcpStatus=2;	
            rt_kprintf("网卡en的MAC地址为:................%d.%d.%d.%d.%d.%d\r\n",gLwipDev.Mac[0],gLwipDev.Mac[1],gLwipDev.Mac[2],gLwipDev.Mac[3],gLwipDev.Mac[4],gLwipDev.Mac[5]);
            //解析出通过DHCP获取到的IP地址
            gLwipDev.Ip[3]=(uint8_t)(Ip>>24); 
            gLwipDev.Ip[2]=(uint8_t)(Ip>>16);
            gLwipDev.Ip[1]=(uint8_t)(Ip>>8);
            gLwipDev.Ip[0]=(uint8_t)(Ip);
            rt_kprintf("通过DHCP获取到IP地址..............%d.%d.%d.%d\r\n",gLwipDev.Ip[0],gLwipDev.Ip[1],gLwipDev.Ip[2],gLwipDev.Ip[3]);
            //解析通过DHCP获取到的子网掩码地址
            gLwipDev.NetMask[3]=(uint8_t)(NetMask>>24);
            gLwipDev.NetMask[2]=(uint8_t)(NetMask>>16);
            gLwipDev.NetMask[1]=(uint8_t)(NetMask>>8);
            gLwipDev.NetMask[0]=(uint8_t)(NetMask);
            rt_kprintf("通过DHCP获取到子网掩码............%d.%d.%d.%d\r\n",gLwipDev.NetMask[0],gLwipDev.NetMask[1],gLwipDev.NetMask[2],gLwipDev.NetMask[3]);
            //解析出通过DHCP获取到的默认网关
            gLwipDev.GateWay[3]=(uint8_t)(Gw>>24);
            gLwipDev.GateWay[2]=(uint8_t)(Gw>>16);
            gLwipDev.GateWay[1]=(uint8_t)(Gw>>8);
            gLwipDev.GateWay[0]=(uint8_t)(Gw);
            rt_kprintf("通过DHCP获取到的默认网关..........%d.%d.%d.%d\r\n",gLwipDev.GateWay[0],gLwipDev.GateWay[1],gLwipDev.GateWay[2],gLwipDev.GateWay[3]);
            break;
        }
        //通过DHCP服务获取IP地址失败,且超过最大尝试次数
        else if(gLwipNetif.dhcp->tries > LWIP_MAX_DHCP_TRIES) 
        {
            //DHCP超时失败.
            gLwipDev.DhcpStatus=0XFF;
            //使用静态IP地址
            IP4_ADDR(&(gLwipNetif.ip_addr),gLwipDev.Ip[0],gLwipDev.Ip[1],gLwipDev.Ip[2],gLwipDev.Ip[3]);
            IP4_ADDR(&(gLwipNetif.netmask),gLwipDev.NetMask[0],gLwipDev.NetMask[1],gLwipDev.NetMask[2],gLwipDev.NetMask[3]);
            IP4_ADDR(&(gLwipNetif.gw),gLwipDev.GateWay[0],gLwipDev.GateWay[1],gLwipDev.GateWay[2],gLwipDev.GateWay[3]);
            rt_kprintf("DHCP服务超时,使用静态IP地址!\r\n");
            rt_kprintf("网卡en的MAC地址为:................%d.%d.%d.%d.%d.%d\r\n",gLwipDev.Mac[0],gLwipDev.Mac[1],gLwipDev.Mac[2],gLwipDev.Mac[3],gLwipDev.Mac[4],gLwipDev.Mac[5]);
            rt_kprintf("静态IP地址........................%d.%d.%d.%d\r\n",gLwipDev.Ip[0],gLwipDev.Ip[1],gLwipDev.Ip[2],gLwipDev.Ip[3]);
            rt_kprintf("子网掩码..........................%d.%d.%d.%d\r\n",gLwipDev.NetMask[0],gLwipDev.NetMask[1],gLwipDev.NetMask[2],gLwipDev.NetMask[3]);
            rt_kprintf("默认网关..........................%d.%d.%d.%d\r\n",gLwipDev.GateWay[0],gLwipDev.GateWay[1],gLwipDev.GateWay[2],gLwipDev.GateWay[3]);
            break;
        }
        rt_thread_delay(500);
    }
    //删除DHCP任务  
    LwipCommDhcpDelete(1);            
}

/*==================================================================
* Function      : LwipCommDhcpCreat
* Description   : 创建DHCP任务
* Input Para    : none
* Output Para   : none
* Return Value  : none
==================================================================*/
static void LwipCommDhcpCreat(void)
{
    //使用动态内存，返回控制块指针
    gLwipDhcpThread = rt_thread_create(
                 "dhcptask",            /* 线程名字 */
                 LwipDhcpTask,          /* 线程入口函数 */
                 RT_NULL,               /* 线程入口函数参数 */
                 4096,                  /* 线程栈大小 */
                 LWIP_DHCP_TASK_PRIO,   /* 线程的优先级 */
                 20);                   /* 线程时间片 */
    if(gLwipDhcpThread != RT_NULL)
    {
        rt_thread_startup(gLwipDhcpThread); /* 启动线程，开启调度 */
        rt_kprintf("DHCP TASK线程创建成功！\r\n");
    }        
}

//删除next_timeout()数据结构(变量在times.c文件)
//timers.c文件
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

//内核线程控制块指针(sys_arch.c)
extern rt_thread_t TcpIpThread;
extern rt_mutex_t gLwipSysMutex;
//消息邮箱 全局变量(在tcpip.c里面定义)
extern sys_mbox_t mbox;  				
//LWIP资源回收函数，退出LWIP时使用（目前暂时不用，网络一直使用）
void LwipCommDestroy(void)
{
    //1.硬件资源释放
    //复位以太网
    Dp83848DeInit();
    //删除lwip_netif网卡
    netif_remove(&gLwipNetif); 
    //2.IPC资源释放
    //删除mbox消息邮箱
    sys_mbox_free(&mbox);
    //删除互斥信号量
    rt_mutex_delete(gLwipSysMutex);
    //删除定时事件链表第一个事件
    lwip_comm_delete_next_timeout(); 
    //3.全局变量释放
    //......
    //4.线程资源释放（RTT注意不能自己线程删除，只改变线程状态，并在空闲线程释放资源）
    #if LWIP_DHCP
    //dhcp任务删除 
	LwipCommDhcpDelete(0);		
    #endif
    //LWIP内核线程删除
    rt_thread_delete(gLwipDhcpThread);    
    //5.动态内存资源释放
    //释放LWIP申请的内存.
	LwipCommMemFree();
    //释放网卡驱动申请的内存
	Dp83848MemFree();
}

#else
/*==================================================================
* Function      : LwipDhcpProcessHandle
* Description   : DHCP处理任务
* Input Para    : none
* Output Para   : none
* Return Value  : none
==================================================================*/
static void LwipDhcpProcessHandle(void)
{
	u32 Ip=0,NetMask=0,Gw=0;
	switch(gLwipDev.DhcpStatus)
	{
        //开启DHCP
		case 0: 	
			dhcp_start(&gLwipNetif);
            //等待通过DHCP获取到的地址
			gLwipDev.DhcpStatus = 1;		
			rt_kprintf("正在查找DHCP服务器,请稍等...........\r\n");  
			break;
        //等待获取到IP地址
		case 1:		
		{
            //读取新IP地址
			Ip=gLwipNetif.ip_addr.addr;	
            //读取子网掩码            
			NetMask=gLwipNetif.netmask.addr;
            //读取默认网关 
			Gw=gLwipNetif.gw.addr;			
			//正确获取到IP地址的时候
			if(Ip!=0)			
			{
                //DHCP成功
				gLwipDev.DhcpStatus=2;	
				rt_kprintf("网卡en的MAC地址为:................%d.%d.%d.%d.%d.%d\r\n",gLwipDev.Mac[0],gLwipDev.Mac[1],gLwipDev.Mac[2],gLwipDev.Mac[3],gLwipDev.Mac[4],gLwipDev.Mac[5]);
				//解析出通过DHCP获取到的IP地址
				gLwipDev.Ip[3]=(uint8_t)(Ip>>24); 
				gLwipDev.Ip[2]=(uint8_t)(Ip>>16);
				gLwipDev.Ip[1]=(uint8_t)(Ip>>8);
				gLwipDev.Ip[0]=(uint8_t)(Ip);
				rt_kprintf("通过DHCP获取到IP地址..............%d.%d.%d.%d\r\n",gLwipDev.Ip[0],gLwipDev.Ip[1],gLwipDev.Ip[2],gLwipDev.Ip[3]);
				//解析通过DHCP获取到的子网掩码地址
				gLwipDev.NetMask[3]=(uint8_t)(NetMask>>24);
				gLwipDev.NetMask[2]=(uint8_t)(NetMask>>16);
				gLwipDev.NetMask[1]=(uint8_t)(NetMask>>8);
				gLwipDev.NetMask[0]=(uint8_t)(NetMask);
				rt_kprintf("通过DHCP获取到子网掩码............%d.%d.%d.%d\r\n",gLwipDev.NetMask[0],gLwipDev.NetMask[1],gLwipDev.NetMask[2],gLwipDev.NetMask[3]);
				//解析出通过DHCP获取到的默认网关
				gLwipDev.GateWay[3]=(uint8_t)(Gw>>24);
				gLwipDev.GateWay[2]=(uint8_t)(Gw>>16);
				gLwipDev.GateWay[1]=(uint8_t)(Gw>>8);
				gLwipDev.GateWay[0]=(uint8_t)(Gw);
				rt_kprintf("通过DHCP获取到的默认网关..........%d.%d.%d.%d\r\n",gLwipDev.GateWay[0],gLwipDev.GateWay[1],gLwipDev.GateWay[2],gLwipDev.GateWay[3]);
			}
            //通过DHCP服务获取IP地址失败,且超过最大尝试次数
            else if(gLwipNetif.dhcp->tries > LWIP_MAX_DHCP_TRIES) 
			{
				gLwipDev.DhcpStatus=0XFF;//DHCP超时失败.
				//使用静态IP地址
				IP4_ADDR(&(gLwipNetif.ip_addr),gLwipDev.Ip[0],gLwipDev.Ip[1],gLwipDev.Ip[2],gLwipDev.Ip[3]);
				IP4_ADDR(&(gLwipNetif.netmask),gLwipDev.NetMask[0],gLwipDev.NetMask[1],gLwipDev.NetMask[2],gLwipDev.NetMask[3]);
				IP4_ADDR(&(gLwipNetif.gw),gLwipDev.GateWay[0],gLwipDev.GateWay[1],gLwipDev.GateWay[2],gLwipDev.GateWay[3]);
				rt_kprintf("DHCP服务超时,使用静态IP地址!\r\n");
				rt_kprintf("网卡en的MAC地址为:................%d.%d.%d.%d.%d.%d\r\n",gLwipDev.Mac[0],gLwipDev.Mac[1],gLwipDev.Mac[2],gLwipDev.Mac[3],gLwipDev.Mac[4],gLwipDev.Mac[5]);
				rt_kprintf("静态IP地址........................%d.%d.%d.%d\r\n",gLwipDev.Ip[0],gLwipDev.Ip[1],gLwipDev.Ip[2],gLwipDev.Ip[3]);
				rt_kprintf("子网掩码..........................%d.%d.%d.%d\r\n",gLwipDev.NetMask[0],gLwipDev.NetMask[1],gLwipDev.NetMask[2],gLwipDev.NetMask[3]);
				rt_kprintf("默认网关..........................%d.%d.%d.%d\r\n",gLwipDev.GateWay[0],gLwipDev.GateWay[1],gLwipDev.GateWay[2],gLwipDev.GateWay[3]);
			}
		}
		break;
		default : break;
	}    
}

#endif
#endif

//不使用操作系统
#if NO_SYS
/*==================================================================
* Function      : LwipPeriodicHandle
* Description   : LWIP轮询任务（10ms，内核中周期定时器，TCP、ARP、DHCP）
* Input Para    : none
* Output Para   : none
* Return Value  : none
==================================================================*/
void LwipPeriodicHandle(void)
{
    //TCP查询计时器
    static u32 gTCPTimer=0;
    //ARP查询计时器
    static u32 gARPTimer=0;
    //lwip本地时间计数器,单位:ms
    static u32 gLwipLocalTime;
	gLwipLocalTime = rt_tick_get();
    #if LWIP_DHCP
    //DHCP精细处理计时器
    static u32 gDHCPfineTimer=0;	
    //DHCP粗糙处理计时器
    static u32 gDHCPcoarseTimer=0;	
    #endif
#if LWIP_TCP
    //协议栈要求每250ms调用一次tcp_tmr()函数
    if(gLwipLocalTime - gTCPTimer >= TCP_TMR_INTERVAL)
    {
        gTCPTimer =  gLwipLocalTime;
        tcp_tmr();
    }
#endif    
    //ARP每5s周期性调用一次
    if ((gLwipLocalTime - gARPTimer) >= ARP_TMR_INTERVAL)
    {
        gARPTimer =  gLwipLocalTime;
        etharp_tmr();
    }    
    //如果使用DHCP的话
#if LWIP_DHCP 
    //每500ms调用一次dhcp_fine_tmr()
    if (gLwipLocalTime - gDHCPfineTimer >= DHCP_FINE_TIMER_MSECS)
    {
        gDHCPfineTimer =  gLwipLocalTime;
        dhcp_fine_tmr();
        if ((gLwipDev.DhcpStatus != 2)&&(gLwipDev.DhcpStatus != 0XFF))
        { 
            //DHCP处理
            LwipDhcpProcessHandle();  
        }
    }
    //每60s执行一次DHCP粗糙处理
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
* Description   : LWIP初始化(LWIP启动的时候使用)
* Input Para    : none
* Output Para   : none
* Return Value  : 0,成功 1,内存错误 2,DB83848初始化失败 3,网卡添加失败
==================================================================*/
u8 LwipCommInit(void)
{
    //调用netif_add()函数时的返回值,用于判断网络初始化是否成功
    struct netif *NetifInitFlag;
    //ip地址
    struct ip_addr IpAddr; 
    //子网掩码
	struct ip_addr NetMask;
    //默认网关 
	struct ip_addr Gw;      			
    //1.网卡驱动内存申请
    if(Dp83848MemMalloc()) return 1;
    //2.lwip内存申请
    if(LwipCommMemMalloc())return 1;
    //3.初始化DP83848 
	if(Dp83848Init())return 2;    
    //4.初始化LWIP内核
    //使用操作系统
    #if !NO_SYS
    //初始化tcp ip内核,该函数里面会创建tcpip_thread内核任务
    tcpip_init(NULL,NULL);				
    #else
    lwip_init();
    #endif
    //5.设置默认IP等信息
    LwipCommDefaultIpSet(&gLwipDev);
    //6.DHCP
    //使用动态IP
#if LWIP_DHCP		
	IpAddr.addr = 0;
	NetMask.addr = 0;
	Gw.addr = 0;
    //使用静态IP 
#else				 
	IP4_ADDR(&IpAddr,gLwipDev.Ip[0],gLwipDev.Ip[1],gLwipDev.Ip[2],gLwipDev.Ip[3]);
	IP4_ADDR(&NetMask,gLwipDev.NetMask[0],gLwipDev.NetMask[1] ,gLwipDev.NetMask[2],gLwipDev.NetMask[3]);
	IP4_ADDR(&Gw,gLwipDev.GateWay[0],gLwipDev.GateWay[1],gLwipDev.GateWay[2],gLwipDev.GateWay[3]);
#endif 
    //7.设置网卡接收数据回调函数
    Dp83848Banding(LwipPktHandle);
    //8.向网卡列表中添加一个网口    
//不使用操作系统    
#if NO_SYS     
    NetifInitFlag=netif_add(&gLwipNetif,&IpAddr,&NetMask,&Gw,NULL,&EthernetifInit,&ethernet_input);
//如果使用DHCP的话
#if LWIP_DHCP
    //DHCP标记为0
	gLwipDev.DhcpStatus=0;
	//开启DHCP服务
	dhcp_start(&gLwipNetif);	
#endif 
#else
    NetifInitFlag=netif_add(&gLwipNetif,&IpAddr,&NetMask,&Gw,NULL,&EthernetifInit,&tcpip_input);
#endif
    //网卡添加失败 
    if(NetifInitFlag==NULL)return 3;
    //8.网口添加成功后,设置netif为默认值,并且打开netif网口
	else
	{
        //设置netif为默认网口
		netif_set_default(&gLwipNetif);
        //打开netif网口
		netif_set_up(&gLwipNetif);		
	}
    //使用操作系统
    #if !NO_SYS
    //创建DHCP任务
    LwipCommDhcpCreat();
    #endif
    //操作OK.
    return 0;
}

