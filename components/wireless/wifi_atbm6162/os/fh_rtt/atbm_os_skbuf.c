/*
 * Copyright (c) 2026 IntelPro Technology Co., Ltd.
 * All rights reserved.
 *
 * This software and associated documentation files (the "Software") are
 * proprietary and confidential to IntelPro Technology Co., Ltd..
 *
 * Access to and use of this Software is limited exclusively to authorized 
 * customers and partners of IntelPro Technology Co., Ltd. who have entered into a 
 * valid license or non-disclosure agreement.
 *
 * Unauthorized copying, modification, distribution, or reverse engineering 
 * of this Software, via any medium, is strictly prohibited without 
 * prior written permission from IntelPro Technology Co., Ltd..
 *
 * SPDX-License-Identifier: Proprietary
 */

#include "sal_ipaddr.h"

#include "atbm_hal.h"
#include "lwip/netif.h"
#include "netif/etharp.h"
#include "atbm_os_skbuf.h"
atbm_void atbm_skbbuffer_init()
{
}
struct atbm_net_device * atbm_alloc_netdev(atbm_int32 size)
{
	struct atbm_net_device *  netdev = atbm_kmalloc(size + sizeof(struct atbm_net_device),GFP_KERNEL);

	ATBM_ASSERT((netdev != ATBM_NULL));
	if(netdev)
		atbm_memset(netdev,0,(size + sizeof(struct atbm_net_device)));

	netdev->nif = atbm_kmalloc(sizeof(struct netif),GFP_KERNEL);
	ATBM_ASSERT(netdev->nif != ATBM_NULL);
	if(netdev->nif)
		atbm_memset(netdev->nif,0,sizeof(struct netif));
	atbm_os_init_waitevent(&netdev->tx_enable);
	atbm_os_init_waitevent(&netdev->rx_enable);
	wifi_printk(WIFI_ALWAYS,"atbm_alloc_netdev,netdev(%x),nif(%x),priv(%x)\n",netdev,netdev->nif,&netdev->drv_priv[0]);
	return  netdev;
}
atbm_void * netdev_drv_priv(struct atbm_net_device *ndev)
{
	return &ndev->drv_priv[0];

}
atbm_void atbm_free_netdev(struct atbm_net_device * netdev)
{
	if(netdev != ATBM_NULL)
		atbm_kfree(netdev);
}

atbm_void atbm_set_netif(struct netif *pNetIf)
{
}
atbm_void atbm_netdev_registed(struct netif *pNetIf)
{
	
}
atbm_void atbm_lwip_init(struct atbm_net_device *dev)
{
	//hal_create_mutex(&lwip_mutex);
}
extern int atbm_wifi_netif_init(struct atbm_net_device *dev);
extern void atbm_wifi_netif_deinit(struct atbm_net_device *dev);

atbm_void atbm_lwip_enable(struct atbm_net_device *dev)
{
	int queue_id;
	struct atbmwifi_vif *priv = netdev_drv_priv(dev);
	struct atbmwifi_common *hw_priv = _atbmwifi_vifpriv_to_hwpriv(priv);

	if(!dev->lwip_enable){
		dev->lwip_enable=1;
		dev->lwip_queue_enable=1;
		atbm_os_wakeup_event(&dev->tx_enable);
#if ATBM_SUPPORT_BRIDGE
		if(atbmwifi_is_sta_mode(priv->iftype))
#endif
		{
			atbm_wifi_netif_init(dev);
		}
		for(queue_id = ATBM_IEEE80211_AC_VO; queue_id <= ATBM_IEEE80211_AC_BK; queue_id++){
			if(hw_priv->tx_queue[queue_id].overfull == ATBM_TRUE){
				dev->lwip_queue_enable=0;
			}
		}
	}
}

atbm_void atbm_lwip_disable(struct atbm_net_device *dev)
{
	struct atbmwifi_vif *priv = netdev_drv_priv(dev);

	if(dev->lwip_enable){
		dev->lwip_enable=0;
		dev->lwip_queue_enable=0;
#if ATBM_SUPPORT_BRIDGE
		if(atbmwifi_is_sta_mode(priv->iftype))
#endif
		{
			atbm_wifi_netif_deinit(dev);
		}
	}
}

atbm_void atbm_lwip_txdone(struct atbm_net_device *dev)
{
}

atbm_void atbm_lwip_wake_queue(struct atbm_net_device *dev,int num)
{
	if(!dev->lwip_queue_enable && dev->lwip_enable){
		dev->lwip_queue_enable = 1;
		atbm_os_wakeup_event(&dev->tx_enable);
	}
}

atbm_void atbm_lwip_stop_queue(struct atbm_net_device *dev,int num)
{
	if(dev->lwip_queue_enable && dev->lwip_enable){
		//hal_wait_for_mutex(&lwip_mutex,2000);
		dev->lwip_queue_enable = 0;
	}
}

atbm_void atbm_lwip_task_event(struct atbm_net_device *dev)
{
}
/****************************************************************************
* Function:   	atbm_wifi_tx_pkt
*
* Purpose:   	This function is used to send packet to wifi driver
*
* Parameters: point to buffer of packet
*
* Returns:	None.
******************************************************************************/
#define STR2MAC		"%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC2STR(a)  a[0],a[1],a[2],a[3],a[4],a[5]

err_t atbm_wifi_tx_pkt_netvif(struct netif *netif, struct pbuf *p)
{
	struct pbuf *q = p;
	struct pbuf *temp_pbuf=ATBM_NULL;
	struct atbm_buff *AtbmBuf = ATBM_NULL;
	struct atbmwifi_vif *priv = (struct atbmwifi_vif *)(netif->state);
	struct atbmwifi_common * hw_priv = priv->hw_priv;
	
	if(priv == ATBM_NULL)
	{
		return -1;
	}

	//If the atbmQueue is full,pls drop??? 
	while(!priv->ndev->lwip_queue_enable){
		atbm_os_wait_event_timeout(&priv->ndev->tx_enable, HZ);
	}

	AtbmBuf = atbm_dev_alloc_skb(p->tot_len);
	if (!AtbmBuf)
	{
		ATBM_BUG_ON(1);
		wifi_printk(WIFI_TX,"<ERROR> tx_pkt alloc skb \n");
		return;
	}
	/*Here should copy pubf chain packet to atbmBuf*/
    for (temp_pbuf = p; temp_pbuf != NULL; temp_pbuf = temp_pbuf->next){
		atbm_memcpy(atbm_skb_put(AtbmBuf,temp_pbuf->len), temp_pbuf->payload, temp_pbuf->len);
		/*if q->len==q->tot_len,it means that it is the last packet*/
		if ( temp_pbuf->len == temp_pbuf->tot_len) {
			break;
		}
	}

	struct atbmwifi_ieee8023_hdr *ethdr = (struct atbmwifi_ieee8023_hdr *)ATBM_OS_SKB_DATA(AtbmBuf);
 
	if(memcmp(ethdr->h_source,priv->mac_addr,6)){
		wifi_printk(WIFI_DBG_ERROR,"atbm_wifi_tx_pkt_netvif"STR2MAC","STR2MAC"\n",
			MAC2STR(ethdr->h_source),MAC2STR(ethdr->h_dest)); 
		atbm_dev_kfree_skb(AtbmBuf);
		return 0;
	}

	if(priv->ndev && priv->ndev->netdev_ops){
		priv->ndev->netdev_ops->ndo_start_xmit(priv, AtbmBuf);
	}else{
		ATBM_BUG_ON(1);
		atbm_dev_kfree_skb(AtbmBuf);
	}

	return 0;
}
//void  ethernetif_input(struct netif *netif, void *p_buf,int size);
static atbm_void __atbm_wifi_rx_pkt(struct atbm_net_device *dev, struct atbm_buff *atbm_skb) 
{
	struct pbuf *p=ATBM_NULL;
	atbm_uint16 len = 0;
	atbm_uint8 *data=ATBM_NULL;
	ATBM_NETIF *netif = dev->nif;
	data = ATBM_OS_SKB_DATA(atbm_skb);
	/* Obtain the size of the packet and put it into the "len" variable. */
	len = ATBM_OS_SKB_LEN(atbm_skb);

	if(netif==NULL){
		goto RcvErr;
	}
	if (0 == len) {
		goto RcvErr;
	}
	//p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
	p = pbuf_alloc(PBUF_RAW, len, PBUF_RAM);
	if (p == ATBM_NULL){
		ATBM_BUG_ON(1);
		goto AllocErr;
	}	
	pbuf_take(p,data,len);
	/* full packet send to tcpip_thread to process */
	if(netif->input){
		if(netif->input(p,netif)!=ERR_OK)
			goto AllocErr;
	}else{
		goto AllocErr;
	}
	atbm_dev_kfree_skb(atbm_skb);
	return;
AllocErr:
	if(p){
//		wifi_printk(WIFI_ALWAYS,"Rx Why Here !!!\n");
		pbuf_free(p);
	}
RcvErr:
	atbm_dev_kfree_skb(atbm_skb);
	atbm_skb=ATBM_NULL;
	p=ATBM_NULL;
	return;
}

atbm_uint32 atbm_wifi_rx_pkt(struct atbm_net_device *dev, struct atbm_buff *at_skb)
{

	struct eth_hdr *ethhdr;

	ethhdr = (struct eth_hdr *)ATBM_OS_SKB_DATA(at_skb);

	switch (htons(ethhdr->type)) {
	  /* IP or ARP packet? */
	  case ETHTYPE_IP:
	  case ETHTYPE_ARP:
	  case 0x888E:
#if PPPOE_SUPPORT
	  /* PPPoE packet? */
	  case ETHTYPE_PPPOEDISC:
	  case ETHTYPE_PPPOE:
#endif /* PPPOE_SUPPORT */
	    /* full packet send to tcpip_thread to process */
	   
	    __atbm_wifi_rx_pkt(dev,at_skb);
	    break;

	  default:
	    atbm_dev_kfree_skb(at_skb);
	    break;
	}
	/* Receive the complete packet */
	return 0;
}
struct tcpip_opt lwip_tcp_opt ={
	.net_init = atbm_lwip_init,
	.net_enable = atbm_lwip_enable,//
#if (PLATFORM == FH_EVIZE_RTT)
	.net_disable = NULL,//
#else
	.net_disable = atbm_lwip_disable,
#endif
	.net_rx = atbm_wifi_rx_pkt,
	.net_tx_done =	atbm_lwip_txdone,
	.net_start_queue =	atbm_lwip_wake_queue,
	.net_stop_queue =	atbm_lwip_stop_queue,
	.net_task_event =	atbm_lwip_task_event,//
};
