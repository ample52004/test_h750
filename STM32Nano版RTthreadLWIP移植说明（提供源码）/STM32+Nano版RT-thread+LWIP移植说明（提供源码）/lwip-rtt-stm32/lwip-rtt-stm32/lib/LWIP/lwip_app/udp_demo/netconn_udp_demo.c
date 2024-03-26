//netconn ���socket���ײ� ��ʡ��Դ��socketͨ���ҷ�װ�϶�

#include "netconn_udp_demo.h" 
//#define NETCONN_UDP_CLIENT
#ifdef NETCONN_UDP_CLIENT


#include "lwip_comm.h"
#include "lwip/api.h"
#include <rthw.h>

static struct netconn *udp_netconn;
u8 udp_recvbuf[UDP_RX_BUFSIZE];			//UDP���ջ�����
//�����͵�����
const u8 udp_sendbuf[] ="Explorer STM32F207 UDP send data\r\n";
extern Lwip_Dev_Typedef gLwipDev;
 
void netconn_udp_test(void* parameter)
{
	err_t err;
    static u16 i = 0;
	struct ip_addr udp_ipaddr;
	struct netbuf  *recvbuf;
	struct netbuf  *sendbuf;	
    static u8 gDhcpFlag = 0;
	if(1)
	{
		if(1)
		{
			while(1)
			{
                i++;
                if((gLwipDev.DhcpStatus ==2) && (gDhcpFlag == 0))
                {
                    udp_netconn=netconn_new(NETCONN_UDP);  	//����һ��UDP����
	                udp_netconn->recv_timeout=10;  			//���ճ�ʱ����
                    if(udp_netconn!=NULL)  //����UDP���ӳɹ�
                    netconn_bind(udp_netconn,IP_ADDR_ANY,UDP_PORT);	//��UDP_PORT�˿�
		            IP4_ADDR(&udp_ipaddr,gLwipDev.RemoteIp[0],gLwipDev.RemoteIp[1],gLwipDev.RemoteIp[2], gLwipDev.RemoteIp[3]); //����Ŀ��IP��ַ
		            err=netconn_connect(udp_netconn,&udp_ipaddr,UDP_PORT); 	//���ӵ�Զ�������˿�
                    if(err==ERR_OK)
                    {
                        gDhcpFlag = 1;
                    }
                }
                else
                {
                    if(gDhcpFlag == 0)
                    {
                        rt_thread_delay(10);
                        continue;
                    }
                }
                if((i>= 500) && (gDhcpFlag == 1))
                {
                    i = 0;
                    sendbuf=netbuf_new();
                    netbuf_alloc(sendbuf,strlen((char *)udp_sendbuf));
                    sendbuf->p->payload=(void*)udp_sendbuf;   		//ָ��udp_sentbuf����
                    err = netconn_send(udp_netconn,sendbuf);//��netbuf�е����ݷ��ͳ�ȥ
                    if(err!=ERR_OK)rt_kprintf("����ʧ��\r\n"); 
					netbuf_delete(sendbuf);  //ɾ��buf				
				}  
		        err=netconn_recv(udp_netconn,&recvbuf);//�鿴�Ƿ���յ�����
				if(err==ERR_OK)
				{
					if(recvbuf->p->tot_len>=UDP_RX_BUFSIZE) //udp���ݴ���udp���ջ�����
					{
						((char*)recvbuf->p->payload)[UDP_RX_BUFSIZE-1]=0;//ĩβ���������
						memcpy(udp_recvbuf,recvbuf->p->payload,UDP_RX_BUFSIZE);//��������
					}else
					{
						memcpy(udp_recvbuf,recvbuf->p->payload,recvbuf->p->tot_len);
						udp_recvbuf[recvbuf->p->tot_len] = 0;
					}
					rt_kprintf("���յ����Ͷ˷���������:%s\r\n",udp_recvbuf);//��ӡ���յ�������
					netbuf_delete(recvbuf);      //ɾ��buf
				} 
				rt_thread_delay(10);
			}
		}
	}
	if(udp_netconn)
	{
		netconn_disconnect(udp_netconn);//�ر�UDP����
		netconn_delete(udp_netconn);//ɾ��udp����	
	}
}

/*==================================================================
* Function      :UdpTaskInit
* Description   :�������� 
* Input Para    :None
* Output Para   :None
* Return Value  :RT_EOK
==================================================================*/
int UdpTaskInit(void)
{
    //�߳̿��ƿ�ָ��
    rt_thread_t UdpTaskThread = RT_NULL;
    //ʹ�ö�̬�ڴ棬���ؿ��ƿ�ָ��
    UdpTaskThread = rt_thread_create(
                 "netudp",              /* �߳����� */
                 netconn_udp_test,      /* �߳���ں��� */
                 RT_NULL,               /* �߳���ں������� */
                 1600,                  /* �߳�ջ��С */
                 6,                     /* �̵߳����ȼ� */
                 20);                   /* �߳�ʱ��Ƭ */
    if(UdpTaskThread != RT_NULL)
    {
        rt_thread_startup(UdpTaskThread); /* �����̣߳��������� */
        return RT_EOK;
    }
    else
    {
        return -1;
    }
}
INIT_APP_EXPORT(UdpTaskInit);
#endif

