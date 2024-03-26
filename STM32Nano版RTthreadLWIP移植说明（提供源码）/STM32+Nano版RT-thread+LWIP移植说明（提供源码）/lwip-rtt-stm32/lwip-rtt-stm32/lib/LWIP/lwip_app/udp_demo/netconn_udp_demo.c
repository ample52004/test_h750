//netconn 相比socket更底层 ，省资源。socket通用且封装较多

#include "netconn_udp_demo.h" 
//#define NETCONN_UDP_CLIENT
#ifdef NETCONN_UDP_CLIENT


#include "lwip_comm.h"
#include "lwip/api.h"
#include <rthw.h>

static struct netconn *udp_netconn;
u8 udp_recvbuf[UDP_RX_BUFSIZE];			//UDP接收缓冲区
//待发送的数据
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
                    udp_netconn=netconn_new(NETCONN_UDP);  	//创建一个UDP链接
	                udp_netconn->recv_timeout=10;  			//接收超时函数
                    if(udp_netconn!=NULL)  //创建UDP连接成功
                    netconn_bind(udp_netconn,IP_ADDR_ANY,UDP_PORT);	//绑定UDP_PORT端口
		            IP4_ADDR(&udp_ipaddr,gLwipDev.RemoteIp[0],gLwipDev.RemoteIp[1],gLwipDev.RemoteIp[2], gLwipDev.RemoteIp[3]); //构造目的IP地址
		            err=netconn_connect(udp_netconn,&udp_ipaddr,UDP_PORT); 	//连接到远端主机端口
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
                    sendbuf->p->payload=(void*)udp_sendbuf;   		//指向udp_sentbuf数组
                    err = netconn_send(udp_netconn,sendbuf);//将netbuf中的数据发送出去
                    if(err!=ERR_OK)rt_kprintf("发送失败\r\n"); 
					netbuf_delete(sendbuf);  //删除buf				
				}  
		        err=netconn_recv(udp_netconn,&recvbuf);//查看是否接收到数据
				if(err==ERR_OK)
				{
					if(recvbuf->p->tot_len>=UDP_RX_BUFSIZE) //udp数据大于udp接收缓冲区
					{
						((char*)recvbuf->p->payload)[UDP_RX_BUFSIZE-1]=0;//末尾加入结束符
						memcpy(udp_recvbuf,recvbuf->p->payload,UDP_RX_BUFSIZE);//拷贝数据
					}else
					{
						memcpy(udp_recvbuf,recvbuf->p->payload,recvbuf->p->tot_len);
						udp_recvbuf[recvbuf->p->tot_len] = 0;
					}
					rt_kprintf("接收到发送端发送来数据:%s\r\n",udp_recvbuf);//打印接收到的数据
					netbuf_delete(recvbuf);      //删除buf
				} 
				rt_thread_delay(10);
			}
		}
	}
	if(udp_netconn)
	{
		netconn_disconnect(udp_netconn);//关闭UDP连接
		netconn_delete(udp_netconn);//删除udp连接	
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
                 netconn_udp_test,      /* 线程入口函数 */
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

