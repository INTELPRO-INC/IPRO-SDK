/**************************************************************************************************************
 * altobeam RTOS wifi hmac source code 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 *
 *  The source code contains proprietary information of AltoBeam, and shall not be distributed, 
 *  copied, reproduced, or disclosed in whole or in part without prior written permission of AltoBeam.
*****************************************************************************************************************/
#include "atbm_hal.h"
/******** Functions below is Wlan API **********/
#include "atbm_sysops.h"
#include <stdint.h>
#include "netif/etharp.h"
#include "lwip/tcpip.h"
#include "lwip/netif.h"
#include "lwip/sockets.h"
#include "lwip/dhcp.h"
#include "lwip/netifapi.h"   /* IPRO7 port: thread-safe netif/dhcp bring-up */

extern struct atbmwifi_common g_hw_prv;
extern err_t atbm_wifi_tx_pkt_netvif(struct netif *netif, struct pbuf *p);
extern err_t etharp_output(struct netif *netif, struct pbuf *q, const ip4_addr_t *ipaddr);
int atbm_wifi_netif_init(struct atbm_net_device *dev);


int atbmwifi_event_OsCallback(atbm_void *prv,int eventid,atbm_void *param)
{
	struct atbmwifi_vif *priv = prv;

	if(atbmwifi_is_ap_mode(priv->iftype)){
		atbm_uint8 * staMacAddr ;
		switch(eventid){

			case ATBM_WIFI_DEAUTH_EVENT:
				wifi_printk(WIFI_ALWAYS,"event_OsCallback DEAUTH\n");
				
				break;
			case ATBM_WIFI_AUTH_EVENT:
				break;
			case ATBM_WIFI_ASSOC_EVENT:
				break;
			case ATBM_WIFI_ASSOCRSP_TXOK_EVENT: 
				staMacAddr =(atbm_uint8 *) param;

				break;
			case ATBM_WIFI_DEASSOC_EVENT:
				break;
			case ATBM_WIFI_JOIN_EVENT:			
				staMacAddr =(atbm_uint8 *) param;
				wifi_printk(WIFI_ALWAYS, "ATBM_WIFI_JOIN_EVENT\n");
				break;
			default:
				break;
		}
	}
	else {	
		switch(eventid){
			case ATBM_WIFI_SCANSTART_EVENT:
				break;
			case ATBM_WIFI_SCANDONE_EVENT:				
//				event.event_type = WLAN_E_SCAN_COMPLETE;
//				WLAN_SYS_StatusCallback(&event);	
				break;
			case ATBM_WIFI_DEAUTH_EVENT:				
				
				break;
			case ATBM_WIFI_AUTH_EVENT:
				break;
			case ATBM_WIFI_ASSOC_EVENT:				
				
				break;
			case ATBM_WIFI_DEASSOC_EVENT:
				break;
			case ATBM_WIFI_JOIN_EVENT:
				wifi_printk(WIFI_ALWAYS, "~~~~ATBM_WIFI_JOIN_EVENT\n");
				break;
			case ATBM_WIFI_SMARTCONFIG_SUCCESS:
				break;
#if CONFIG_WPS
			case ATBM_WIFI_WPS_SUCCESS:
				{
					struct atbm_wpa_ssid *ssid = (struct atbm_wpa_ssid *)param;
					atbm_wifi_sta_join_ap(ssid->ssid, NULL, 0, 0, ssid->passphrase);
				}
				break;
#endif
			default:
				break;
		}
	}
}

struct netif *atbm_priv_get_netif(struct atbm_net_device *dev)
{
	return dev->nif;
}

atbm_uint8 atbm_get_wifimode(struct atbmwifi_vif * priv)
{
	return priv->iftype;
}

atbm_uint32 atbm_os_random()
{
	atbm_uint32 data = atbm_random()/3;
	return (data>>1);
}

static void atbm_wifi_set_netaddr(struct netif *netif){
	struct atbmwifi_vif *priv = (struct atbmwifi_vif *)(netif->state);
	struct atbmwifi_common	*hw_priv = ATBM_NULL;
	hw_priv = priv->hw_priv;
	netif->hwaddr_len = 6;
	atbm_memcpy(netif->hwaddr,priv->mac_addr,netif->hwaddr_len);
		
	wifi_printk(WIFI_ALWAYS,"atbm_wifi_set_netaddr:"MACSTR"\n",MAC2STR(netif->hwaddr));
}


static err_t atbm_wifi_if_init(struct netif *netif)
{
	netif->output = etharp_output;
	netif->linkoutput = atbm_wifi_tx_pkt_netvif;
	/* set MAC hardware address length */
	atbm_wifi_set_netaddr(netif);
	netif->hwaddr_len = 6;
	/* maximum transfer unit */
	netif->mtu = 1500;
	/* device capabilities */
	/* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
	netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;
	return 0;
}

void dhcp_restart(struct netif *p_netif)
{
	dhcp_stop(p_netif);
	atbm_mdelay(50);
	dhcp_start(p_netif);
	return;
}

/*
 * @brief usb_net_dhcp_stop, stop dhcp client
 * @param: netif
 * @param: timeout in second
 * @return  0 - stop dhcp successfully, -1 - errors happen
 */
int atbmusb_net_dhcp_stop (struct netif *netif)
{
	/* release and stop dhcp client */
	dhcp_release (netif);
	dhcp_stop (netif);

	return 0;
}

int atbm_wifi_netif_init(struct atbm_net_device *dev)
{
	struct atbmwifi_vif * priv = netdev_drv_priv(dev);
	struct ip4_addr ipaddr, netmask, gw;
	struct netif *p_netif = ATBM_NULL;

	char * IP_ADDR = 	"192.168.43.1";
	char * GW_ADDR = 	"192.168.43.1";
	char * MASK_ADDR = 	"255.255.255.0";

	if(!atbm_wifi_initialed(priv->if_id)){
		wifi_printk(WIFI_DBG_ERROR, "wifi_netif_init err\n");	
		return 0;
	}
	if(atbmwifi_is_ap_mode(priv->iftype)){
		gw.addr = inet_addr(GW_ADDR);
		ipaddr.addr = inet_addr(IP_ADDR);
		netmask.addr = inet_addr(MASK_ADDR);
	}else{
		gw.addr = 0;
		ipaddr.addr = 0;
		netmask.addr = 0;
	}

	p_netif = atbm_priv_get_netif(dev);

	if(p_netif == ATBM_NULL){
		wifi_printk(WIFI_DBG_ERROR,"wifi_netif_init p_netif == ATBM_NULL\n");	
		return 0;
	}
	wifi_printk(WIFI_ALWAYS,"wifi_netif_init p_netif (%x)\n",p_netif);	
	/* IPRO7 port: lwIP is built with LWIP_TCPIP_CORE_LOCKING=1 and this runs on the
	 * WPA event thread (not tcpip_thread), so use the thread-safe netifapi variants
	 * — the raw netif/dhcp calls would self-deadlock on lock_tcpip_core.
	 * netifapi_netif_add returns ERR_OK(0) on success (inverted vs raw netif_add). */
	if (netifapi_netif_add(p_netif, &ipaddr, &netmask, &gw, (void*)&dev->drv_priv[0], atbm_wifi_if_init, tcpip_input) != ERR_OK)
	{
		wifi_printk(WIFI_DBG_ERROR,"wifi_netif_init netif_add err\n");
		return  - 1;
	}
	netifapi_netif_set_default(p_netif);
	netifapi_netif_set_up(p_netif);
	if(atbmwifi_is_ap_mode(priv->iftype)){
		p_netif->name[0] = 'a'; p_netif->name[1] = 'p';   /* lwIP name is char[2] */
//		dhcps_start(ipaddr);
	}else{
		p_netif->name[0] = 'w'; p_netif->name[1] = '0';
		/* IPRO7 port: STA DHCP client — bring link up then start DHCP (thread-safe). */
		netifapi_netif_set_link_up(p_netif);
		netifapi_dhcp_start(p_netif);
	}
	return 0;
}

/**
 * @brief  wifi_netif_deinit, remove netif of wifi
 * @param  : 
 * @retval void
 */
void atbm_wifi_netif_deinit(struct atbm_net_device *dev)
{
	struct netif *p_netif = NULL;
	
	struct atbmwifi_vif * priv = netdev_drv_priv(dev);
	if(!atbm_wifi_initialed(priv->if_id)){
		wifi_printk(WIFI_DBG_ERROR,"wifi_netif_deinit err\n");
		return ;
	}
	p_netif = atbm_priv_get_netif(dev);
	if(p_netif == ATBM_NULL){
		wifi_printk(WIFI_DBG_ERROR,"wifi_netif_deinit p_netif == ATBM_NULL\n");	
		return;
	}

	if(atbm_get_wifimode(priv) == ATBM_NL80211_IFTYPE_STATION)
		dhcpc_stop(p_netif->name);
//	else if(atbm_get_wifimode(priv) == ATBM_NL80211_IFTYPE_AP)
//		dhcpd_stop();

	atbm_wifi_set_netaddr(p_netif);
	netif_remove(p_netif);
	netif_set_down(p_netif);

}


