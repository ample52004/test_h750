
#include "tcp_client_demo.h"
#include "lwip_comm.h"
#include "lwip/api.h"
#include <rthw.h>
#include "lwip/dns.h"
#define NETCONN_TCP_CLIENT
#ifdef NETCONN_TCP_CLIENT
static struct netconn *tcp_clientconn;			//TCP CLIENT�������ӽṹ��
u8 tcp_client_recvbuf[TCP_CLIENT_RX_BUFSIZE];	//TCP�ͻ��˽��ջ�����
//�����͵�����
const u8 tcp_client_sendbuf[]="Explorer STM32F207 TCP Client send data\r\n";
//#define DNS_TEST
#ifdef DNS_TEST
static u8 gTestDoaminName[64] = "www.baidu.com";
#endif

extern Lwip_Dev_Typedef gLwipDev;

void tcp_client_test(void* parameter)
{
	err_t err; 
	ip_addr_t server_ipaddr,loca_ipaddr;
	u16 client_port,loca_port;
	struct netbuf *recvbuf;
	u8 t=0;
	static u8 gDhcpFlag = 0;
    static u16 i = 0;
    #ifdef DNS_TEST
    static u16 j = 0;
    #endif
	if(1)
	{
		if(1)
		{ 
			while(1)
			{
                i++;
                if((gLwipDev.DhcpStatus ==2) && (gDhcpFlag == 0))
                {	
                    client_port=TCP_CLIENT_PORT;
                    //����Զ��IP��ַ
                    IP4_ADDR(&server_ipaddr,gLwipDev.RemoteIp[0],gLwipDev.RemoteIp[1],gLwipDev.RemoteIp[2],gLwipDev.RemoteIp[3]);                  
                    tcp_clientconn=netconn_new(NETCONN_TCP); //����һ��TCP����
                    if(tcp_clientconn)
                    {
                        #ifdef DNS_TEST
                        //�������ӣ���ȡIP //DNS test
                        while (j < 5)
                        {
                            j++;
                            if (dns_gethostbyname((const char*)gTestDoaminName,&server_ipaddr, NULL, NULL) == 0)
                            {
                                //test
                                gLwipDev.RemoteIp[0] = ip4_addr1(&server_ipaddr);
                                gLwipDev.RemoteIp[1] = ip4_addr2(&server_ipaddr);
                                gLwipDev.RemoteIp[2] = ip4_addr3(&server_ipaddr);
                                gLwipDev.RemoteIp[3] = ip4_addr4(&server_ipaddr);
                                break;
                            }
                            rt_thread_delay(100);
                        }
                        #endif
                        err=netconn_connect(tcp_clientconn,&server_ipaddr,client_port);	//���ӷ����� 	
                        if(err==ERR_OK)//�����ɹ� 
                        {
                            tcp_clientconn->recv_timeout=10;
                            netconn_getaddr(tcp_clientconn,&loca_ipaddr,&loca_port,1); 		//��ȡ����IP����IP��ַ�Ͷ˿ں�
                            rt_kprintf("�����Ϸ�����%d.%d.%d.%d,�����˿ں�Ϊ:%d\r\n",gLwipDev.RemoteIp[0],gLwipDev.RemoteIp[1],gLwipDev.RemoteIp[2],gLwipDev.RemoteIp[3],loca_port);
                            gDhcpFlag = 1;
                        }
                        else
                        {
                             rt_kprintf("���ӷ�����ʧ�ܣ�\r\n"); 
                        }
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
					err=netconn_write(tcp_clientconn ,tcp_client_sendbuf,strlen((char*)tcp_client_sendbuf),NETCONN_COPY); //����tcp_server_sentbuf�е�����
					if(err!=ERR_OK)rt_kprintf("����ʧ��\r\n");		                    
                }
				err=netconn_recv(tcp_clientconn,&recvbuf);//�鿴�Ƿ���յ�����
				if(err==ERR_OK)  //���յ�����
				{		 
					if((recvbuf->p->tot_len)>=TCP_CLIENT_RX_BUFSIZE) //udp���ݴ���tcp���������ջ�����
					{
						((char*)recvbuf->p->payload)[TCP_CLIENT_RX_BUFSIZE-1]=0;//ĩβ���������
						memcpy(tcp_client_recvbuf,recvbuf->p->payload,TCP_CLIENT_RX_BUFSIZE);//��������
					}else
					{
						memcpy(tcp_client_recvbuf,recvbuf->p->payload,recvbuf->p->tot_len);
						tcp_client_recvbuf[recvbuf->p->tot_len]=0;//ĩβ��ӽ�����
					}
					rt_kprintf("������%d.%d.%d.%d��������:\r\n%s\r\n",gLwipDev.RemoteIp[0],gLwipDev.RemoteIp[1],gLwipDev.RemoteIp[2],gLwipDev.RemoteIp[3],tcp_client_recvbuf);
					netbuf_delete(recvbuf);
				}else if(err==ERR_CLSD)
				{
					netconn_close(tcp_clientconn);//�ر�tcp_clientconn����
					rt_kprintf("������%d.%d.%d.%d�Ͽ�����\r\n",gLwipDev.RemoteIp[0],gLwipDev.RemoteIp[1],gLwipDev.RemoteIp[2],gLwipDev.RemoteIp[3]);
					break;
				}
				rt_thread_delay(10);
			}
		}else//����ʧ��.
		{
			netconn_close(tcp_clientconn);			//�ر�tcp_clientconn����
			netconn_delete(tcp_clientconn); 		//ɾ��tcp_clientconn����
			tcp_clientconn=netconn_new(NETCONN_TCP);//����һ����TCP����
		}
		rt_thread_delay(10);
		t++;
		if(t==100)
		{
			t=0;
			rt_kprintf("TCP Client �������ӷ�����...\r\n");
		}		
	}
	if(tcp_clientconn!=NULL)//��tcp_clientconn���Ӵ��ڵ������ɾ������
	{
 		netconn_close(tcp_clientconn);  	//�ر�tcp_clientconn����
		netconn_delete(tcp_clientconn); 	//ɾ��tcp_clientconn����
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
                 tcp_client_test,      /* �߳���ں��� */
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
