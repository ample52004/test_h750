#ifndef _LWIP_COMM_H
#define _LWIP_COMM_H 
#include "public_define.h"
#include "lwipopts.h"
//DHCP������������Դ���  
#define LWIP_MAX_DHCP_TRIES		4   
//lwip���ƽṹ��
typedef struct  
{
	u8 Mac[6];      //MAC��ַ
	u8 RemoteIp[4];	//Զ������IP��ַ 
	u8 Ip[4];       //����IP��ַ
	u8 NetMask[4]; 	//��������
	u8 GateWay[4]; 	//Ĭ�����ص�IP��ַ
	//DNS����
    //DNS����
	vu8 DhcpStatus;	//dhcp״̬ 
					//0,δ��ȡDHCP��ַ;
					//1,����DHCP��ȡ״̬
					//2,�ɹ���ȡDHCP��ַ
					//0XFF,��ȡʧ��.
}Lwip_Dev_Typedef;	

//LWIP��ʼ��(LWIP������ʱ��ʹ��)
u8 LwipCommInit(void);
//��ʹ�ò���ϵͳ
#if NO_SYS
//LWIP��ѯ����10ms���ں������ڶ�ʱ����TCP��ARP��DHCP��
void LwipPeriodicHandle(void);
#endif
//LWIP��Դ���պ������˳�LWIPʱʹ��
void LwipCommDestroy(void);
//lwip���ƽṹ�����ýӿ� 

#endif

