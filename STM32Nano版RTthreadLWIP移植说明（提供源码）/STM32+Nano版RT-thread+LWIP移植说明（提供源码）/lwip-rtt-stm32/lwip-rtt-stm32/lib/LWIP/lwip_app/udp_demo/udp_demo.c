#include "udp_demo.h" 
#include "stdio.h"
#include "string.h" 
#include "public_define.h"  
//#define UDP_CLIENT
#ifdef UDP_CLIENT
 
//UDP�������ݻ�����
u8 udp_demo_recvbuf[UDP_DEMO_RX_BUFSIZE];
//UDP������������
const u8 tcp_demo_sendbuf[]="Explorer STM32F207 UDP demo send data\r\n";
extern Lwip_Dev_Typedef gLwipDev;
//UDP ����ȫ��״̬��Ǳ���
//bit7:û���õ�
//bit6:0,û���յ�����;1,�յ�������.
//bit5:0,û��������;1,��������.
//bit4~0:����
u8 udp_demo_flag;
struct udp_pcb *udppcb;//����һ��TCP���������ƿ�

//����Զ��IP��ַ
void udp_demo_set_remoteip(void)
{
    //��
} 

//UDP����
void udp_demo_test(void)
{
 	err_t err;
	struct ip_addr rmtipaddr;//Զ��ip��ַ 		
	udp_demo_set_remoteip();//��ѡ��IP
	udppcb=udp_new();
	if(udppcb)//�����ɹ�
	{ 
		IP4_ADDR(&rmtipaddr,gLwipDev.RemoteIp[0],gLwipDev.RemoteIp[1],gLwipDev.RemoteIp[2],gLwipDev.RemoteIp[3]);
		err=udp_connect(udppcb,&rmtipaddr,UDP_DEMO_PORT);//UDP�ͻ������ӵ�ָ��IP��ַ�Ͷ˿ںŵķ�����
		if(err==ERR_OK)
		{
			err=udp_bind(udppcb,IP_ADDR_ANY,UDP_DEMO_PORT);//�󶨱���IP��ַ��˿ں�
			if(err==ERR_OK)	//�����
			{
				udp_recv(udppcb,udp_demo_recv,NULL);//ע����ջص����� 
			}
		}		
	}
} 

//UDP��ʱ��������
void CycSend(void)
{
    static u32 tick = 0;
    tick++;
    if(tick >= 500)
    {
        udp_demo_senddata(udppcb); 
        tick = 0;
    }
}

//UDP�������ص�����
void udp_demo_recv(void *arg,struct udp_pcb *upcb,struct pbuf *p,struct ip_addr *addr,u16_t port)
{
	u32 data_len = 0;
	struct pbuf *q;
	if(p!=NULL)	//���յ���Ϊ�յ�����ʱ
	{
		memset(udp_demo_recvbuf,0,UDP_DEMO_RX_BUFSIZE);  //���ݽ��ջ���������
		for(q=p;q!=NULL;q=q->next)  //����������pbuf����
		{
			//�ж�Ҫ������UDP_DEMO_RX_BUFSIZE�е������Ƿ����UDP_DEMO_RX_BUFSIZE��ʣ��ռ䣬�������
			//�Ļ���ֻ����UDP_DEMO_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
			if(q->len > (UDP_DEMO_RX_BUFSIZE-data_len)) memcpy(udp_demo_recvbuf+data_len,q->payload,(UDP_DEMO_RX_BUFSIZE-data_len));//��������
			else memcpy(udp_demo_recvbuf+data_len,q->payload,q->len);
			data_len += q->len;  	
			if(data_len > UDP_DEMO_RX_BUFSIZE) break; //����TCP�ͻ��˽�������,����	
		}
		upcb->remote_ip=*addr;//��¼Զ��������IP��ַ
		upcb->remote_port=port;//��¼Զ�������Ķ˿ں�
		udp_demo_flag|=1<<6;//��ǽ��յ�������
		pbuf_free(p);//�ͷ��ڴ�
	}else
	{
		udp_disconnect(upcb); 
	} 
} 

//UDP��������������
void udp_demo_senddata(struct udp_pcb *upcb)
{
	struct pbuf *ptr;
	ptr=pbuf_alloc(PBUF_TRANSPORT,strlen((char*)tcp_demo_sendbuf),PBUF_POOL); //�����ڴ�
	if(ptr)
	{
		ptr->payload=(void*)tcp_demo_sendbuf; 
		udp_send(upcb,ptr);	//udp�������� 
		pbuf_free(ptr);//�ͷ��ڴ�
	} 
} 

//�ر�tcp����
void udp_demo_connection_close(struct udp_pcb *upcb)
{
	udp_disconnect(upcb); 
	udp_remove(upcb);//�Ͽ�UDP���� 
}
#endif

