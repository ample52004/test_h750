#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__
#include "lwip/err.h"
#include "lwip/netif.h"

//����������
#define IFNAME0 'e'
#define IFNAME1 'n'
//��ʼ������
err_t EthernetifInit(struct netif *netif);
//������������(lwipֱ�ӵ���)
err_t EthernetifInput(struct netif *netif);
#endif
