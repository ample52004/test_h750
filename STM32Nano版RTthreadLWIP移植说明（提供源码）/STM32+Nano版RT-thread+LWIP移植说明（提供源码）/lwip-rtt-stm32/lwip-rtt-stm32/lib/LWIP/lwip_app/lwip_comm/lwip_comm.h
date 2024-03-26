#ifndef _LWIP_COMM_H
#define _LWIP_COMM_H 
#include "public_define.h"
#include "lwipopts.h"
//DHCP服务器最大重试次数  
#define LWIP_MAX_DHCP_TRIES		4   
//lwip控制结构体
typedef struct  
{
	u8 Mac[6];      //MAC地址
	u8 RemoteIp[4];	//远端主机IP地址 
	u8 Ip[4];       //本机IP地址
	u8 NetMask[4]; 	//子网掩码
	u8 GateWay[4]; 	//默认网关的IP地址
	//DNS主动
    //DNS备用
	vu8 DhcpStatus;	//dhcp状态 
					//0,未获取DHCP地址;
					//1,进入DHCP获取状态
					//2,成功获取DHCP地址
					//0XFF,获取失败.
}Lwip_Dev_Typedef;	

//LWIP初始化(LWIP启动的时候使用)
u8 LwipCommInit(void);
//不使用操作系统
#if NO_SYS
//LWIP轮询任务（10ms，内核中周期定时器，TCP、ARP、DHCP）
void LwipPeriodicHandle(void);
#endif
//LWIP资源回收函数，退出LWIP时使用
void LwipCommDestroy(void);
//lwip控制结构体配置接口 

#endif

