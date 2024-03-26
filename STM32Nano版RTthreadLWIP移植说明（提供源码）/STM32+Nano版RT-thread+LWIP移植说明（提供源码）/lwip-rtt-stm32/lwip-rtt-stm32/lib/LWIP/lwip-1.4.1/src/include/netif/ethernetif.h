#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__
#include "lwip/err.h"
#include "lwip/netif.h"

//网卡的名字
#define IFNAME0 'e'
#define IFNAME1 'n'
//初始化网络
err_t EthernetifInit(struct netif *netif);
//网卡接收数据(lwip直接调用)
err_t EthernetifInput(struct netif *netif);
#endif
