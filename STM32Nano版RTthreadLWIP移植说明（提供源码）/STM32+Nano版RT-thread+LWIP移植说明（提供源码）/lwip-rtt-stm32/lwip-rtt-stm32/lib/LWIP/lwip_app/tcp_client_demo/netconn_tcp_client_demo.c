
#include "tcp_client_demo.h"
#include "lwip_comm.h"
#include "lwip/api.h"
#include <rthw.h>
#include "lwip/dns.h"
#define NETCONN_TCP_CLIENT
#ifdef NETCONN_TCP_CLIENT
static struct netconn *tcp_clientconn;			//TCP CLIENT网络连接结构体
u8 tcp_client_recvbuf[TCP_CLIENT_RX_BUFSIZE];	//TCP客户端接收缓冲区
//待发送的数据
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
                    //设置远端IP地址
                    IP4_ADDR(&server_ipaddr,gLwipDev.RemoteIp[0],gLwipDev.RemoteIp[1],gLwipDev.RemoteIp[2],gLwipDev.RemoteIp[3]);                  
                    tcp_clientconn=netconn_new(NETCONN_TCP); //创建一个TCP链接
                    if(tcp_clientconn)
                    {
                        #ifdef DNS_TEST
                        //域名连接，获取IP //DNS test
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
                        err=netconn_connect(tcp_clientconn,&server_ipaddr,client_port);	//连接服务器 	
                        if(err==ERR_OK)//创建成功 
                        {
                            tcp_clientconn->recv_timeout=10;
                            netconn_getaddr(tcp_clientconn,&loca_ipaddr,&loca_port,1); 		//获取本地IP主机IP地址和端口号
                            rt_kprintf("连接上服务器%d.%d.%d.%d,主机端口号为:%d\r\n",gLwipDev.RemoteIp[0],gLwipDev.RemoteIp[1],gLwipDev.RemoteIp[2],gLwipDev.RemoteIp[3],loca_port);
                            gDhcpFlag = 1;
                        }
                        else
                        {
                             rt_kprintf("连接服务器失败！\r\n"); 
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
					err=netconn_write(tcp_clientconn ,tcp_client_sendbuf,strlen((char*)tcp_client_sendbuf),NETCONN_COPY); //发送tcp_server_sentbuf中的数据
					if(err!=ERR_OK)rt_kprintf("发送失败\r\n");		                    
                }
				err=netconn_recv(tcp_clientconn,&recvbuf);//查看是否接收到数据
				if(err==ERR_OK)  //接收到数据
				{		 
					if((recvbuf->p->tot_len)>=TCP_CLIENT_RX_BUFSIZE) //udp数据大于tcp服务器接收缓冲区
					{
						((char*)recvbuf->p->payload)[TCP_CLIENT_RX_BUFSIZE-1]=0;//末尾加入结束符
						memcpy(tcp_client_recvbuf,recvbuf->p->payload,TCP_CLIENT_RX_BUFSIZE);//拷贝数据
					}else
					{
						memcpy(tcp_client_recvbuf,recvbuf->p->payload,recvbuf->p->tot_len);
						tcp_client_recvbuf[recvbuf->p->tot_len]=0;//末尾添加结束符
					}
					rt_kprintf("服务器%d.%d.%d.%d发来数据:\r\n%s\r\n",gLwipDev.RemoteIp[0],gLwipDev.RemoteIp[1],gLwipDev.RemoteIp[2],gLwipDev.RemoteIp[3],tcp_client_recvbuf);
					netbuf_delete(recvbuf);
				}else if(err==ERR_CLSD)
				{
					netconn_close(tcp_clientconn);//关闭tcp_clientconn连接
					rt_kprintf("服务器%d.%d.%d.%d断开连接\r\n",gLwipDev.RemoteIp[0],gLwipDev.RemoteIp[1],gLwipDev.RemoteIp[2],gLwipDev.RemoteIp[3]);
					break;
				}
				rt_thread_delay(10);
			}
		}else//连接失败.
		{
			netconn_close(tcp_clientconn);			//关闭tcp_clientconn连接
			netconn_delete(tcp_clientconn); 		//删除tcp_clientconn连接
			tcp_clientconn=netconn_new(NETCONN_TCP);//创建一个新TCP链接
		}
		rt_thread_delay(10);
		t++;
		if(t==100)
		{
			t=0;
			rt_kprintf("TCP Client 尝试连接服务器...\r\n");
		}		
	}
	if(tcp_clientconn!=NULL)//当tcp_clientconn连接存在的情况下删除连接
	{
 		netconn_close(tcp_clientconn);  	//关闭tcp_clientconn连接
		netconn_delete(tcp_clientconn); 	//删除tcp_clientconn连接
	} 
}  

/*==================================================================
* Function      :UdpTaskInit
* Description   :周期任务 
* Input Para    :None
* Output Para   :None
* Return Value  :RT_EOK
==================================================================*/
int UdpTaskInit(void)
{
    //线程控制块指针
    rt_thread_t UdpTaskThread = RT_NULL;
    //使用动态内存，返回控制块指针
    UdpTaskThread = rt_thread_create(
                 "netudp",              /* 线程名字 */
                 tcp_client_test,      /* 线程入口函数 */
                 RT_NULL,               /* 线程入口函数参数 */
                 1600,                  /* 线程栈大小 */
                 6,                     /* 线程的优先级 */
                 20);                   /* 线程时间片 */
    if(UdpTaskThread != RT_NULL)
    {
        rt_thread_startup(UdpTaskThread); /* 启动线程，开启调度 */
        return RT_EOK;
    }
    else
    {
        return -1;
    }
}
INIT_APP_EXPORT(UdpTaskInit);

#endif
