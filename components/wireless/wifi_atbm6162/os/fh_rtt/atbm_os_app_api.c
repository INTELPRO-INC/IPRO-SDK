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

#include <stdio.h>
#include <stdlib.h>
#include "rtdef.h"
#include "atbm_os_app_api.h"
#include "atbm_hal.h"

#include "generalWifi.h"

static int wifi_inited = 0;
extern bool init_wififlag;
extern struct atbmwifi_common g_hw_prv;


extern atbm_void sta_scan_start_timer_func(atbm_void *data1,atbm_void *data2);



#define STR2MAC		"%02x:%02x:%02x:%02x:%02x:%02x"
//#define MAC2STR(a)  a[0],a[1],a[2],a[3],a[4],a[5]

//#ifdef RT_USING_FINSH
typedef struct{
	char id;
	unsigned char ssid[33];
	unsigned char ssid_len;
	unsigned char psk[33];
	unsigned char psk_len;
	unsigned char key_mgmt;
	unsigned char used_flag;
	unsigned char active_stat;
}wpa_cfg_t;

typedef struct{
	FH_CHAR *ssid;
	FH_UINT32 ssid_len;
	FH_UINT32 start_chan;
	FH_UINT32 end_chan;
	FH_UINT32 max_ap_number;
}scan_param_t;

typedef enum{
	STA_STATUS_CONNECTED = 0,
	STA_STATUS_AP_NOT_FOUND = -4,
	STA_STATUS_INVALID_KEY = -3,
	STA_STATUS_AUTH_FAILURE = -5,
	STA_STATUS_EAPOL_KEY_FAILURE = -6,
	STA_STATUS_CON_LOST = -7,
	STA_STATUS_STATUS_UNKNOWN = -11

}STA_CON_STATUS_E; 
	


const int sta_conn_status(int stat)
{
	switch(stat){
		case STA_STATUS_CONNECTED://0
			wifi_printk(WIFI_DBG_ERROR,"sta_con_status	= STA_STATUS_CONNECTED\n");	
			return STA_STATUS_CONNECTED;
		case STA_STATUS_AP_NOT_FOUND://-4
			wifi_printk(WIFI_DBG_ERROR,"sta_con_status	= STA_STATUS_AP_NOT_FOUND\n");	
			return STA_STATUS_AP_NOT_FOUND;
		case STA_STATUS_INVALID_KEY://-3
			wifi_printk(WIFI_DBG_ERROR,"sta_con_status	= STA_STATUS_INVALID_KEY\n");	
			return STA_STATUS_INVALID_KEY;
		case STA_STATUS_AUTH_FAILURE://-5
			wifi_printk(WIFI_DBG_ERROR,"sta_con_status	= STA_STATUS_AUTH_FAILURE\n");
			return STA_STATUS_AUTH_FAILURE;
		case STA_STATUS_EAPOL_KEY_FAILURE://-6
			wifi_printk(WIFI_DBG_ERROR,"sta_con_status	= STA_STATUS_EAPOL_KEY_FAILURE\n");
			return STA_STATUS_EAPOL_KEY_FAILURE;
		case STA_STATUS_CON_LOST://-7
			wifi_printk(WIFI_DBG_ERROR,"sta_con_status	= STA_STATUS_CON_LOST\n");
			return STA_STATUS_CON_LOST;
		case STA_STATUS_STATUS_UNKNOWN://-11
			wifi_printk(WIFI_DBG_ERROR,"sta_con_status	= STA_STATUS_STATUS_UNKNOWN\n");
			return STA_STATUS_STATUS_UNKNOWN;
		default:
			wifi_printk(WIFI_DBG_ERROR,"sta_con_status	= STA_STATUS_STATUS_UNKNOWN\n");
			return STA_STATUS_STATUS_UNKNOWN;
	}

}



const char * wpa_supplicant_state_txt(enum atbm_wpa_states state)
{
	switch (state) {
	case ATBM_WPA_DISCONNECTED:
		return "DISCONNECTED";
	case ATBM_WPA_INACTIVE:
		return "INACTIVE";
	case ATBM_WPA_INTERFACE_DISABLED:
		return "INTERFACE_DISABLED";
	case ATBM_WPA_SCANNING:
		return "SCANNING";
	case ATBM_WPA_AUTHENTICATING:
		return "AUTHENTICATING";
	case ATBM_WPA_ASSOCIATING:
		return "ASSOCIATING";
	case ATBM_WPA_ASSOCIATED:
		return "ASSOCIATED";
	case ATBM_WPA_4WAY_HANDSHAKE:
		return "4WAY_HANDSHAKE";
	case ATBM_WPA_GROUP_HANDSHAKE:
		return "GROUP_HANDSHAKE";
	case ATBM_WPA_COMPLETED:
		return "COMPLETED";
	default:
		return "UNKNOWN";
	}
}

const char * wpa_key_mgmt_txt(int key_mgmt, int proto)
{
	switch (key_mgmt) {
	case ATBM_WPA_KEY_MGMT_IEEE8021X:
		if (proto == (ATBM_WPA_PROTO_RSN | ATBM_WPA_PROTO_WPA))
			return "WPA2+WPA/IEEE 802.1X/EAP";
		return proto == ATBM_WPA_PROTO_RSN ?
			"WPA2/IEEE 802.1X/EAP" : "WPA/IEEE 802.1X/EAP";
	case ATBM_WPA_KEY_MGMT_PSK:
		if (proto == (ATBM_WPA_PROTO_RSN | ATBM_WPA_PROTO_WPA))
			return "WPA2-PSK+WPA-PSK";
		return proto == ATBM_WPA_PROTO_RSN ?
			"WPA2-PSK" : "WPA-PSK";
	case ATBM_WPA_KEY_MGMT_NONE:
		return "NONE";
	case ATBM_WPA_KEY_MGMT_WPA_NONE:
		return "WPA-NONE";
	case ATBM_WPA_KEY_MGMT_IEEE8021X_NO_WPA:
		return "IEEE 802.1X (no WPA)";
#ifdef CONFIG_IEEE80211R
	case ATBM_WPA_KEY_MGMT_FT_IEEE8021X:
		return "FT-EAP";
	case ATBM_WPA_KEY_MGMT_FT_PSK:
		return "FT-PSK";
#endif /* CONFIG_IEEE80211R */
#ifdef CONFIG_IEEE80211W
	case ATBM_WPA_KEY_MGMT_IEEE8021X_SHA256:
		return "WPA2-EAP-SHA256";
	case ATBM_WPA_KEY_MGMT_PSK_SHA256:
		return "WPA2-PSK-SHA256";
#endif /* CONFIG_IEEE80211W */
	case ATBM_WPA_KEY_MGMT_WPS:
		return "WPS";
	case ATBM_WPA_KEY_MGMT_WEP:
		return "WEP";
	default:
		return "UNKNOWN";
	}
}
const char * wpa_cipher_txt(int cipher)
{
	switch (cipher) {
	case ATBM_WPA_CIPHER_NONE:
		return "NONE";
	case ATBM_WPA_CIPHER_WEP40:
		return "WEP-40";
	case ATBM_WPA_CIPHER_WEP104:
		return "WEP-104";
	case ATBM_WPA_CIPHER_TKIP://WPA
		return "TKIP";
	case ATBM_WPA_CIPHER_CCMP://RSN
		return "CCMP";
	case ATBM_WPA_CIPHER_CCMP | ATBM_WPA_CIPHER_TKIP:
		return "CCMP+TKIP";
	case ATBM_WPA_CIPHER_GCMP:
		return "GCMP";
	default:
		return "UNKNOWN";
	}
}


/*
 * func: power up and recognize wifi, then initialize firmware
 * para1: mode (0: staion, 1: AP)
 * para2: sleep_flag (0: for common state, 1: for low power state)
 * XXX: multi-thread context
 * NOTE: w_start()/w_stop() should be called in the same thread
 
 运行sta/ap
 mode:
	0 station
	1 ap
sleep_flag:
	不支持休眠，驱动强制设置为0
 */

FH_SINT32 atbm_start(FH_UINT32 mode, FH_UINT32 sleep_flag)
{

	if(!wifi_inited){
		atbm_wifi_hw_init();
		wifi_inited = 1;
#ifndef WIFI_SDIO
		rt_thread_delay(150);
#endif
	}
/*
	atbm_wifi_off(mode1);
	rt_thread_delay(150);
*/
	//if(atbm_wifi_get_current_mode_vif(mode) != mode)
	{
	
		
		atbm_os_mutexLock(&g_hw_prv.swich_mode_lock,RT_WAITING_FOREVER);
		atbm_wifi_on(mode);
		atbm_os_mutexUnLock(&g_hw_prv.swich_mode_lock);
	}
	return 0;
}
/*
 * func: stop wifi according to mode, including resource release of wifi and protocol stack
 * para1: mode (0: staion, 1: AP)
 * NOTE: w_start()/w_stop() should be called in the same thread
 
 wifi 进入idle状态
 
 */

FH_SINT32 atbm_stop(FH_UINT32 mode)
{
	
	if(!wifi_inited){
		atbm_wifi_hw_init();
		wifi_inited = 1;
#ifndef WIFI_SDIO
		rt_thread_delay(150);
#endif
	}

	atbm_os_mutexLock(&g_hw_prv.scan_lock,RT_WAITING_FOREVER);
	atbm_os_mutexLock(&g_hw_prv.dis_ap_lock,RT_WAITING_FOREVER);
	atbm_os_mutexLock(&g_hw_prv.conn_ap_lock,RT_WAITING_FOREVER);
	atbm_os_mutexLock(&g_hw_prv.ap_create_lock,RT_WAITING_FOREVER);
	atbm_os_mutexLock(&g_hw_prv.swich_mode_lock,RT_WAITING_FOREVER);
//	if(atbm_wifi_get_current_mode() == mode)
	{
		atbm_wifi_off(mode);
		atbm_SleepMs(300);
	}

	atbm_os_mutexUnLock(&g_hw_prv.scan_lock);
	atbm_os_mutexUnLock(&g_hw_prv.dis_ap_lock);
	atbm_os_mutexUnLock(&g_hw_prv.conn_ap_lock);
	atbm_os_mutexUnLock(&g_hw_prv.ap_create_lock);
	atbm_os_mutexUnLock(&g_hw_prv.swich_mode_lock);
	
	return 0;
}
FH_SINT32 atbm_get_conn_ssid(char *buff)
{
	int ret = 0;
	struct atbmwifi_vif *priv;
	struct wpa_supplicant *g_wpa_s;
	priv=_atbmwifi_hwpriv_to_vifpriv(&g_hw_prv,0);
	
	g_wpa_s = priv->appdata;
	if(!g_wpa_s){
		wifi_printk(WIFI_DBG_ERROR,"sta application(wpa_supplicant) not running \n");
	
		ret = -1;
		goto error;
	}
	//atbm_wifi_get_connected_info
	if (!atbmwifi_is_sta_mode(priv->iftype)){
		wifi_printk(WIFI_DBG_ERROR,"not sta mode \n");
		
		ret = -1;
		goto error;
	}
	if(buff){
		memcpy(buff,priv->config.ssid,priv->config.ssid_len);
		ret = 0;
		goto error;
	}else{
		wifi_printk(WIFI_DBG_ERROR,"atbm_sta_status:buff is NULL \n");
		ret = -1;
		goto error;
	}
error:
	atbm_os_mutexUnLock(&g_hw_prv.appdata_lock);
	return ret;
}


	
FH_SINT32 atbm_sta_status(FH_VOID)
{
	int ret = 0;
	struct atbmwifi_vif *priv ;
	struct wpa_supplicant *g_wpa_s;
	priv=_atbmwifi_hwpriv_to_vifpriv(&g_hw_prv,0);
	atbm_os_mutexLock(&g_hw_prv.appdata_lock,RT_WAITING_FOREVER);
	g_wpa_s = priv->appdata;
//	wifi_printk(WIFI_DBG_ERROR,"sta_con_status	= %d\n",sta_conn_status(priv->sta_con_status));	
	if(!g_wpa_s){
		wifi_printk(WIFI_DBG_ERROR,"sta application(wpa_supplicant) not running \n");
		ret = -1;
		goto error;
	}
	//atbm_wifi_get_connected_info
//	wifi_printk(WIFI_DBG_ERROR,"g_wpa_s = %x\n",g_wpa_s);
	priv = g_wpa_s->priv;
	if (!priv && !atbmwifi_is_sta_mode(priv->iftype)){
		wifi_printk(WIFI_DBG_ERROR,"not sta mode \n");
		ret = -1;
		goto error;
	}
	if(g_wpa_s->wpa_state == ATBM_WPA_COMPLETED){
		wifi_printk(WIFI_DBG_ERROR,"IFNAME	= %s\n",priv->if_name);
		wifi_printk(WIFI_DBG_ERROR,"MacAddr	= "STR2MAC"\n",MAC2STR(priv->mac_addr));
		wifi_printk(WIFI_DBG_ERROR,"AP ssid	= %s\n",priv->config.ssid);
		wifi_printk(WIFI_DBG_ERROR,"AP mac	= "STR2MAC"\n",MAC2STR(priv->bssid));
		wifi_printk(WIFI_DBG_ERROR,"Channel	= %d\n",priv->config.channel_index);
		wifi_printk(WIFI_DBG_ERROR,"Rssi	= %d\n",atbm_wifi_get_rssi_avg());
		wifi_printk(WIFI_DBG_ERROR,"pairwise_cipher	= %s\n",wpa_cipher_txt(priv->config.pairwise_cipher));
		wifi_printk(WIFI_DBG_ERROR,"group_cipher	= %s\n",wpa_cipher_txt(priv->config.group_cipher));
		wifi_printk(WIFI_DBG_ERROR,"KeyMgmt	= %s\n",wpa_key_mgmt_txt(priv->config.key_mgmt,priv->config.wpa));
	}
	wifi_printk(WIFI_DBG_ERROR,"Status	= %s\n",wpa_supplicant_state_txt(g_wpa_s->wpa_state));	
	ret = sta_conn_status(priv->sta_con_status);
error:
	atbm_os_mutexUnLock(&g_hw_prv.appdata_lock);
	return ret;
}


FH_SINT32 atbm_rssi_get(FH_SINT16 *rssi)
{
	
	int rssi_val = 0,ret = 0;
	struct atbmwifi_vif *priv ;
	struct wpa_supplicant *g_wpa_s;
	priv=_atbmwifi_hwpriv_to_vifpriv(&g_hw_prv,0);
	atbm_os_mutexLock(&g_hw_prv.appdata_lock,RT_WAITING_FOREVER);
	g_wpa_s = priv->appdata;
	if(!rssi){
		wifi_printk(WIFI_DBG_ERROR,"sta application(wpa_supplicant) not running or parameters is NULL \n");
		ret = -1;
		goto error;
	}
	if(g_wpa_s && g_wpa_s->wpa_state == ATBM_WPA_COMPLETED){
		rssi_val = atbm_wifi_get_rssi_avg();		
	}else{
		wifi_printk(WIFI_DBG_ERROR,"current status is [DISCONNECT] \n");
		rssi_val=  0;
	}
	wifi_printk(WIFI_DBG_ERROR,"current rssi is [%d] \n",rssi_val);
	*rssi = rssi_val;
	
error:
	atbm_os_mutexUnLock(&g_hw_prv.appdata_lock);
	return ret;
}








/*
	print current save BSS cfg
	return max id
*/
/*
	(1) 调用格式：fread(buf,sizeof(buf),1,fp);
读取成功时：当读取的数据量正好是sizeof(buf)个Byte时，返回值为1(即count)
                       否则返回值为0(读取数据量小于sizeof(buf))
(2)调用格式：fread(buf,1,sizeof(buf),fp);
读取成功返回值为实际读回的数据个数(单位为Byte)
*/
#define ATBM_CFG_FILE "atbm_cfg.txt"
#define SAVE_MAX_COUNT 5
wpa_cfg_t p_wpa_cfg[SAVE_MAX_COUNT];
/*

rw_flag:
	true : read file
	false: write file

*/
FH_SINT32 atbm_read_write_file(const char *file_name,char r_flag)
{
	FILE *file_fd;
	int read_num = -1,write_num = -1;
//	wifi_printk(WIFI_DBG_ERROR,"0----- \n");
	if(!file_name){
		wifi_printk(WIFI_DBG_ERROR,"file_name is NULL  \n");
		return -1;
	}
	if(r_flag == 1){
		file_fd = fopen(file_name,"rb+");
	}else{
		file_fd = fopen(file_name,"wb+");
	}
	if(!file_fd){
		wifi_printk(WIFI_DBG_ERROR,"open %s error  \n",file_name);
		return -1;
	}
	if(r_flag == 1){
		read_num = fread(p_wpa_cfg,1,sizeof(wpa_cfg_t) * SAVE_MAX_COUNT,file_fd);
		if(read_num <= 0){
			wifi_printk(WIFI_DBG_ERROR,"read %s error  \n",ATBM_CFG_FILE);
			fclose(file_fd);
			return -1;
		}

	}else{
		write_num = fwrite(p_wpa_cfg,1,sizeof(wpa_cfg_t) * SAVE_MAX_COUNT,file_fd);
		if(write_num <= 0){
			wifi_printk(WIFI_DBG_ERROR,"write %s error  \n",ATBM_CFG_FILE);
			fclose(file_fd);
			return -1;
		}

	}
	fclose(file_fd);
	return 0;
}



FH_SINT32 atbmListCfg(FH_VOID)
{
	struct atbmwifi_vif *priv = NULL;
	int i = 0,ret = -1,sta_status = 0;
	struct wpa_supplicant *g_wpa_s;
	priv=_atbmwifi_hwpriv_to_vifpriv(&g_hw_prv,0);
	atbm_os_mutexLock(&g_hw_prv.appdata_lock,RT_WAITING_FOREVER);
	g_wpa_s = priv->appdata;
	if(g_wpa_s){
		wifi_printk(WIFI_DBG_ERROR,"Status	= %s\n",wpa_supplicant_state_txt(g_wpa_s->wpa_state));
		sta_status = g_wpa_s->wpa_state;
		if(g_wpa_s->wpa_state == ATBM_WPA_COMPLETED){
			priv = g_wpa_s->priv;
		}
	}
	atbm_os_mutexUnLock(&g_hw_prv.appdata_lock);
	
	if((ret = atbm_read_write_file(ATBM_CFG_FILE,1)) == 0){
	
		wifi_printk(WIFI_DBG_ERROR,"id		ssid		key	flags\n");
		for(i=0;i<SAVE_MAX_COUNT;i++){
			if(p_wpa_cfg[i].used_flag){
				if(priv){//connect success
					if(memcmp(p_wpa_cfg[i].ssid,priv->config.ssid,priv->config.ssid_len) == 0)
						p_wpa_cfg[i].active_stat = 1;
					else
						p_wpa_cfg[i].active_stat = 0;
				}else{
					p_wpa_cfg[i].active_stat = 0;
				}
				wifi_printk(WIFI_DBG_ERROR,"%d		%s		%d	%s\n",
				p_wpa_cfg[i].id - 1,p_wpa_cfg[i].ssid,p_wpa_cfg[i].key_mgmt,p_wpa_cfg[i].active_stat?"[CURRENT]":"");		
			}else{
				p_wpa_cfg[i].id = -1;
			}
		}	
	}

	return ret;
}
/*
 * func: add or delete a NULL profile of AP with specific id (Please refer to wpa_cli in linux)
 * para1: id (id for different APs)
 * para2: add (0: delete, 1: add)
 sta 模式下，需要多个ap信息，添加新的 ap 信息，或者删除旧的ap信息，类似 
 wpa_cli add_network id
 wpa_cli remove_network id
 */

FH_SINT32 atbmStaCfg_ap(FH_UINT32 id, FH_UINT32 add)
{
	int ret;
	if(id >= SAVE_MAX_COUNT){
		wifi_printk(WIFI_DBG_ERROR,"id[%d] not allow,max id %d \n",id,SAVE_MAX_COUNT-1);
		return -1;
	}
	memset(&p_wpa_cfg[id],0,sizeof(wpa_cfg_t));
	if(add){
		p_wpa_cfg[id].id = id+1;
		p_wpa_cfg[id].used_flag = 0;
	}else{
		if((ret = atbm_read_write_file(ATBM_CFG_FILE,0)) < 0){
			wifi_printk(WIFI_DBG_ERROR,"save file fail! delete %s fail! \n",p_wpa_cfg[id].ssid);
			return -1;
		}
	}
	
	wifi_printk(WIFI_DBG_ERROR,"atbmStaCfg_ap : id[%s] [%d] \n",add?"add":"delete",p_wpa_cfg[id].id);
	return 0;
}
/*
 * func: configure the ssid for a profile of AP with specific id (Please refer to wpa_cli in linux)
 * para1: id (id for different APs)
 * para2: ssid
 接着上一个函数的
 wpa_cli set_network id ssid xxx
 */

FH_SINT32 atbmStaCfg_ssid(FH_UINT32 id, FH_CHAR *ssid)
{
	int ssid_len;
	if(!ssid){
		wifi_printk(WIFI_DBG_ERROR,"id[%d] ssid is NULL!! \n",id);
		return -1;
	}
	if(id >= SAVE_MAX_COUNT){
		wifi_printk(WIFI_DBG_ERROR,"id[%d] not allow,max id %d \n",id,SAVE_MAX_COUNT-1);
		return -1;
	}
	ssid_len = strlen(ssid);
	if(p_wpa_cfg[id].id == (id+1)){
		if(ssid_len < 33){
			memcpy(p_wpa_cfg[id].ssid,ssid,ssid_len);
			p_wpa_cfg[id].ssid_len = ssid_len;
			if(p_wpa_cfg[id].ssid_len != 0){
				p_wpa_cfg[id].used_flag = 1;
			}
		}else{
			wifi_printk(WIFI_DBG_ERROR,"ssid[%s],ssid len[%d] > 32\n",ssid,p_wpa_cfg[id].ssid_len);
			return -1;
		}
	}else{
		wifi_printk(WIFI_DBG_ERROR,"please need add id[%d]\n",id);
		return -1;
	}
	return 0;
}
/*
 * func: configure the passwd for a profile of AP with specific id (Please refer to wpa_cli in linux)
 * para1: id (id for different APs)
 * para2: passwd(NULL: open, WEP?)
 接着上一个函数的
 wpa_cli set_network id psk xxxxx

 key_mgmt = 0 no encrypto
 key_mgmt = 1 wep
 key_mgmt = 2 other
 */

FH_SINT32 atbmStaCfg_psk(FH_UINT32 id, FH_CHAR *passwd)
{
	
	int psk_len;
	
	if(!passwd){
		wifi_printk(WIFI_DBG_ERROR,"id[%d] passwd is NULL!! \n",id);
		return -1;
	}
	
	if(id >= SAVE_MAX_COUNT){
		wifi_printk(WIFI_DBG_ERROR,"id[%d] not allow,max id %d \n",id,SAVE_MAX_COUNT-1);
		return -1;
	}
	psk_len = strlen(passwd);
	if(p_wpa_cfg[id].id == (id+1)){
		if(psk_len == 0){
			//memcpy(p_wpa_cfg[id].psk,passwd,psk_len);
			p_wpa_cfg[id].psk_len = 0;
			p_wpa_cfg[id].key_mgmt = 0;
			if((p_wpa_cfg[id].ssid_len!=0) && (p_wpa_cfg[id].psk!=0)){
				p_wpa_cfg[id].used_flag = 1;
			}
		}
		else if(psk_len < 33 && psk_len >0 ){
			memcpy(p_wpa_cfg[id].psk,passwd,psk_len);
			p_wpa_cfg[id].psk_len = psk_len;
			if(psk_len <= 5)
				p_wpa_cfg[id].key_mgmt = 1;
			else
				p_wpa_cfg[id].key_mgmt = 2;
			
			if((p_wpa_cfg[id].ssid_len!=0) && (p_wpa_cfg[id].psk!=0)){
				p_wpa_cfg[id].used_flag = 1;
			}
		}else{
			wifi_printk(WIFI_DBG_ERROR,"passwd[%s],passwd len[%d] > 32\n",passwd,p_wpa_cfg[id].psk_len);
			return -1;
		}
	}else{
		wifi_printk(WIFI_DBG_ERROR,"please need add id[%d]\n",id);
		return -1;
	}
	return 0;
}
/*
 * func: connect AP with a profile id of AP(Please refer to wpa_cli in linux)
 * para1: id (configured id for different APs)
 选择这个ap 进行连接
 wpa_cli enable_network id
 */


FH_SINT32 atbmStaConn_ap(FH_UINT32 id)
{
	int ret;
	int retry = 100;

	int i = 0,mode;
	struct atbmwifi_vif *priv;
	priv=_atbmwifi_hwpriv_to_vifpriv(&g_hw_prv,0);
	
	if(id >= SAVE_MAX_COUNT){
		wifi_printk(WIFI_DBG_ERROR,"id[%d] not allow,max id %d \n",id,SAVE_MAX_COUNT-1);
		return -1;
	}
	
	if(p_wpa_cfg[id].used_flag == 0){
		wifi_printk(WIFI_DBG_ERROR,"id[%d]  is NULL \n",id);
		return -1;
	}
	
	
	if(!wifi_inited){
		atbm_wifi_hw_init();
		wifi_inited = 1;
#ifndef WIFI_SDIO
		rt_thread_delay(150);
#endif
	}
	for(i = 0;i < 2;i++){
		if((mode = atbm_wifi_get_current_mode_vif(i)) >= 0){
			atbm_wifi_off(mode);
			rt_thread_delay(150);
			break;
		}
	}
	
	atbm_wifi_on(0);
	wifi_printk(WIFI_DBG_ERROR,"join--ap:\n");
	atbm_os_mutexLock(&g_hw_prv.conn_ap_lock,RT_WAITING_FOREVER);
	
	if(p_wpa_cfg[id].key_mgmt == 1)//wep
    	ret = atbm_wifi_sta_join_ap(p_wpa_cfg[id].ssid, RT_NULL, 1, 1, p_wpa_cfg[id].psk);
	else if(p_wpa_cfg[id].key_mgmt == 2)//other
    	ret = atbm_wifi_sta_join_ap(p_wpa_cfg[id].ssid, RT_NULL, 4, 0, p_wpa_cfg[id].psk);
	else
		ret = atbm_wifi_sta_join_ap(p_wpa_cfg[id].ssid, RT_NULL, 0, 0, RT_NULL);
	
	atbm_os_mutexUnLock(&g_hw_prv.conn_ap_lock);	
    if (ret)
    {
        wifi_printk(WIFI_DBG_ERROR,"%s-%d fail, ret %d\n", __func__, __LINE__, ret);
		return -1;
    }
//	wifi_printk(WIFI_DBG_ERROR,"netif name : %s\n",priv->ndev->nif->name);
	//dhcpc_start(priv->ndev->nif->name);
	atbm_read_write_file(ATBM_CFG_FILE,0);
	while(retry--){
		if(atbm_wifi_isconnected(0)){
			for(i = 0;i<SAVE_MAX_COUNT;i++){
				p_wpa_cfg[i].active_stat = 0;
			}
			wifi_printk(WIFI_DBG_ERROR,"connect %s success  \n",p_wpa_cfg[id].ssid);
			p_wpa_cfg[id].active_stat = 1;	
			atbm_read_write_file(ATBM_CFG_FILE,0);
			break;
		}
		rt_thread_delay(10);
	}
	
	return 0;
	
}
/* disconnect ap and delete cfg ap in station mode
	sta模式下断开和ap的连接并且删除配置
 */

FH_SINT32 atbmStaDel_ap(FH_UINT32 id)
{
	int ret = 0;
	char ssid[33]={0};
	
	atbm_get_conn_ssid(ssid); 
	
	if(memcmp(p_wpa_cfg[id].ssid,ssid,p_wpa_cfg[id].ssid_len) == 0){
		atbm_os_mutexLock(&g_hw_prv.dis_ap_lock,RT_WAITING_FOREVER);
		ret = atbm_disconnect_ap();
		atbm_os_mutexUnLock(&g_hw_prv.dis_ap_lock);
		if(ret  < 0){
			wifi_printk(WIFI_DBG_ERROR,"connect %s sunncess  \n",p_wpa_cfg[id].ssid);
			return ret;
		}
		
	}
	memset(&p_wpa_cfg[id],0,sizeof(wpa_cfg_t));
	if((ret = atbm_read_write_file(ATBM_CFG_FILE,0)) < 0){
		wifi_printk(WIFI_DBG_ERROR,"save file fail! delete %s fail! \n",p_wpa_cfg[id].ssid);
		return -1;
	}

	wifi_printk(WIFI_DBG_ERROR,"connect ap[%s],del ssid[%s] \n",ssid,p_wpa_cfg[id].ssid);
	
	return 0;
}

/*
强制断开连接

*/
FH_SINT32 atbmStaDis_ap(FH_UINT32 id)
{
	int i,ret;

//	atbm_wifi_sta_disjoin_ap();
	atbm_os_mutexLock(&g_hw_prv.dis_ap_lock,RT_WAITING_FOREVER);
	atbm_disconnect_ap();
	atbm_SleepMs(300);
	atbm_os_mutexUnLock(&g_hw_prv.dis_ap_lock);
	for(i=0;i<SAVE_MAX_COUNT;i++){
		if(p_wpa_cfg[i].used_flag)
			p_wpa_cfg[i].active_stat = 0;		
	}	
	if((ret = atbm_read_write_file(ATBM_CFG_FILE,0)) < 0){
		wifi_printk(WIFI_DBG_ERROR,"save file fail! delete %s fail! \n",p_wpa_cfg[id].ssid);
		return -1;
	}
	return 0;
}



int atbmwifi_scan_triger(struct atbmwifi_vif *priv,scan_param_t * scan_param,atbm_uint8 *chan_list,atbm_uint8 chan_num)
{
	int ret;
	int i;
	struct atbmwifi_common *hw_priv = _atbmwifi_vifpriv_to_hwpriv(priv);
	struct wsm_ssid ssids;
    struct wsm_scan scan;

	atbm_memset(&ssids, 0, sizeof(struct wsm_ssid));
	atbm_memset(&scan, 0, sizeof(struct wsm_scan));

	atbm_uint8 all_channel[14] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14};


	if (!priv)
	{
		wifi_printk(WIFI_SCAN,"atbm_scan_work");
	}
	scan.scanType = WSM_SCAN_TYPE_FOREGROUND;
	scan.scanFlags =0;
	scan.numOfProbeRequests = 2;
	scan.probeDelay = 100;
	if(scan_param)
		scan.numOfChannels = scan_param->end_chan - scan_param->start_chan + 1/*atbmwifi_band_2ghz.n_channels*/;
	else if(chan_list)
		scan.numOfChannels = chan_num;
	else
		scan.numOfChannels = 14;
	
	priv->scan.status = 0;
	priv->scan.if_id = priv->if_id;
	priv->scan.in_progress = 1;
	scan.maxTransmitRate = WSM_TRANSMIT_RATE_1;

	
	if (priv->scan.direct_probe){
		scan.maxTransmitRate = WSM_TRANSMIT_RATE_6;
	}



	scan.band =  WSM_PHY_BAND_2_4G;
	if (priv->join_status == ATBMWIFI__JOIN_STATUS_STA) {
		scan.scanType = WSM_SCAN_TYPE_BACKGROUND;
		scan.scanFlags = WSM_SCAN_FLAG_FORCE_BACKGROUND;
		if (priv->if_id)
			scan.scanFlags |= WSM_FLAG_MAC_INSTANCE_1;
		else
			scan.scanFlags &= ~WSM_FLAG_MAC_INSTANCE_1;
	}
	/*It's no need set ScanThrohold*/
	scan.autoScanInterval = (0<< 24)|(120 * 1024); /* 30 seconds, -70 rssi */
	scan.numOfSSIDs = 1;
	scan.ssids = &ssids;

#if (CONFIG_P2P == 0)
	if(priv->scan_no_connect){
		scan.ssids->length = 0;
		scan.numOfSSIDs = 0;
	}
	else 
#endif
	{
		if(priv->special_scan == 2){
			scan.numOfSSIDs = 0;
		}
		if(scan_param && scan_param->ssid_len != 0){
			atbm_memset(&priv->ssid[0],0,ATBM_IEEE80211_MAX_SSID_LEN);
			atbm_memcpy(&priv->ssid[0],scan_param->ssid,scan_param->ssid_len);
			priv->ssid_length = scan_param->ssid_len;	
		}

		if(priv->ssid_length > 0){
			atbm_memset(scan.ssids->ssid,0,32);
			atbm_memcpy(scan.ssids->ssid ,&priv->ssid[0], priv->ssid_length);
			scan.ssids->length = priv->ssid_length;
			wifi_printk(WIFI_ALWAYS,"%s :scan.ssids.ssid[%s]\n",__FUNCTION__,scan.ssids->ssid);
		}
	}
	scan.ch = (struct wsm_scan_ch *)atbm_kmalloc(sizeof(struct wsm_scan_ch)*scan.numOfChannels,GFP_KERNEL);
	if (!scan.ch) {
		priv->scan.status = -ATBM_ENOMEM;
		wifi_printk(WIFI_ALWAYS,"%s zalloc fail %d\n",__FUNCTION__,sizeof(struct wsm_scan_ch)*scan.numOfChannels);
		return 0;
	}

	/*
		set scan channel 
	*/
	for (i = 0; i < scan.numOfChannels; i++) {		
			scan.ch[i].minChannelTime = 55;//45;
			scan.ch[i].maxChannelTime = 105;//75;
			/*
				记录要扫描的信道号
			*/
			if(scan_param){
				scan.ch[i].number = atbmwifi_band_2ghz.channels[all_channel[i + scan_param->start_chan - 1] - 1].hw_value;
				scan.ch[i].txPowerLevel = atbmwifi_band_2ghz.channels[all_channel[i + scan_param->start_chan - 1] - 1].max_power;
			}else if(chan_list){
				scan.ch[i].number = atbmwifi_band_2ghz.channels[chan_list[i - 1] - 1].hw_value;
				scan.ch[i].txPowerLevel = atbmwifi_band_2ghz.channels[chan_list[i - 1] - 1].max_power;
			}else{
				scan.ch[i].number = atbmwifi_band_2ghz.channels[all_channel[i] - 1].hw_value;
				scan.ch[i].txPowerLevel = atbmwifi_band_2ghz.channels[all_channel[i] - 1].max_power;
			}
			wifi_printk(WIFI_ALWAYS,"scan channel [%d] \n",scan.ch[i].number);
	}
	wifi_printk(WIFI_ALWAYS,"atbm_scan_work if_id(%d),numOfChannels(%d),numOfSSIDs(%d)\n",priv->if_id,
		scan.numOfChannels,scan.numOfSSIDs);
	ret = wsm_scan(hw_priv, &scan, priv->if_id);
	atbm_kfree(scan.ch);

	if(ret){
		wifi_printk(WIFI_ALWAYS,"%s fail \n",__FUNCTION__);
		//add by wp ,scan fail
		priv->scan.in_progress = 0;
		priv->scan.ApScan_in_process = 0;

	}
	atbmwifi_event_uplayer(priv,ATBM_WIFI_SCANSTART_EVENT,0);
	return ret;

}




int ieee80211_internal_scan_triger(struct atbmwifi_vif *priv,scan_param_t * scan_param,atbm_uint8 *chan_list,atbm_uint8 chan_num)
{	
	int ret;
	struct atbmwifi_common *hw_priv = _atbmwifi_vifpriv_to_hwpriv(priv);	
	struct wsm_template_frame frame;
	frame.frame_type = WSM_FRAME_TYPE_PROBE_REQUEST;	
	frame.disable =0; 
	frame.rate=0;
	
	if (priv->join_status == ATBMWIFI__JOIN_STATUS_AP)
		return -ATBM_EOPNOTSUPP;
	
	frame.skb = atbmwifi_ieee80211_send_probe_req(priv,ATBM_NULL,priv->extra_ie,priv->extra_ie_len,0);

	if (!frame.skb)
		return -ATBM_ENOMEM;

	ret = wsm_set_template_frame(hw_priv, &frame,
			priv->if_id);
	priv->scan.if_id = priv->if_id;
	

	atbmwifi_scan_triger(priv,scan_param,chan_list,chan_num);
	
	atbm_dev_kfree_skb(frame.skb);
	
	return ret;
}



/****************************************************
Function Name: atbmwifi_scan_process
Return: scan status,0 success,other fail
******************************************************/
 int atbm_internal_cmd_scan_triger(struct atbmwifi_vif *priv,scan_param_t * scan_param,atbm_uint8 *chan_list,atbm_uint8 chan_num)
{

	struct atbmwifi_common *hw_priv = NULL;
	int i = 0,j = 0;
	atbm_uint8  buff[14] = {0},buff_len;
	if(!atbmwifi_is_sta_mode(priv->iftype)) {	
		wifi_printk(WIFI_ALWAYS,"not support scan in AP mode!\n");
		return -1;		
	}
	if(!priv->enabled){
		wifi_printk(WIFI_ALWAYS,"not support not enabled!\n");
		return -2;
	}

	if(priv->scan.scan_smartconfig){
		wifi_printk(WIFI_ALWAYS,"scan_smartconfig now!please try later!\n");
		return -3;
   }
	
	if((priv->assoc_ok==0) && ( priv->join_status == ATBMWIFI__JOIN_STATUS_STA)){
		 wifi_printk(WIFI_ALWAYS,"join now!please try later!\n");
		 return -6;
	}

	if(!priv->scan.in_progress){
		if(priv->scan_ret.info == ATBM_NULL){
			priv->scan_ret.info = (struct atbmwifi_scan_result_info *)atbm_kmalloc(sizeof(struct atbmwifi_scan_result_info) * MAX_SCAN_INFO_NUM,GFP_KERNEL);
			if(priv->scan_ret.info == ATBM_NULL){
				wifi_printk(WIFI_ALWAYS,"scan malloc fail!");
				return -4;
			}
		}
		priv->scan_ret.len = 0;
		priv->scan.if_id = priv->if_id;
		priv->scan_expire = 2;
		priv->scan_no_connect_back = priv->scan_no_connect;
		
		if(priv->special_scan == 1)
			priv->scan_no_connect = 1;
		else
			priv->scan_no_connect = 0;

		/*
			channel limit with country 
		*/
		hw_priv = priv->hw_priv;
		if(scan_param){
			if(scan_param->start_chan > hw_priv->country_channel){
				wifi_printk(WIFI_ALWAYS,"ERROR!!! country [%s] scan chan not allow !!!  scan start chan %d\n",hw_priv->country_code,scan_param->start_chan);
				return -1;
			}
			if(scan_param->end_chan > hw_priv->country_channel){
				wifi_printk(WIFI_ALWAYS,"WARING!! country [%s]  scan channel limit %d , original channel %d\n",hw_priv->country_code,hw_priv->country_channel,scan_param->end_chan);
				scan_param->end_chan = hw_priv->country_channel;
			}
		}else if(chan_list && (chan_num != 0)){
			buff_len = chan_num;
			for(i=0;i<chan_num;i++){
				if(chan_list[i] <= hw_priv->country_channel){
					buff[j++] = chan_list[i];
				}else{
					buff_len--;	
				}
			}
			if(buff_len == 0){
				wifi_printk(WIFI_ALWAYS,"ERROR!!! country [%s] ,scan chan not allow !!! \n",hw_priv->country_code);
				return -1;
			}
			if(buff_len != chan_num){
				memcpy(chan_list,buff,buff_len);
				chan_num = buff_len;
			}
		}
		
		return ieee80211_internal_scan_triger(priv,scan_param,chan_list,chan_num);
	}
	else {
		wifi_printk(WIFI_ALWAYS,"scan busy!please try later!");
		return -5;
	}
}


/****************************************************************************
* Function:   	atbm_wifi_scan_network
*
* Purpose:   	This function is used to ask driver to perform channel scan and return scan result.
*
* Parameters: scan_buf		Buffer to store the information of the found APs
*			buf_size		Size of the buffer
*
* Returns:	Returns 0 if succeed, otherwise a negative error code.
******************************************************************************/
int	atbm_wifi_scan_bss(atbm_uint8 if_id,FH_WiFi_AccessPoint_List* scan_buf, scan_param_t * scan_param)
{
	FH_WiFi_AccessPoint *bss_info;
	FH_WiFi_AccessPoint_List *pScanResult;
	struct atbmwifi_scan_result_info *info;
    atbm_int32 waitloop = 10;
	atbm_int32 i=0;
	struct atbmwifi_vif *priv=ATBM_NULL;
	priv=_atbmwifi_hwpriv_to_vifpriv(&g_hw_prv,if_id);
	if(ATBM_NULL==priv){
		return -1;
	}
	wifi_printk(WIFI_ALWAYS,"atbm_wifi_scan_network_vif(%d) \n",priv->iftype);
	
    priv->scan_no_connect_back = priv->scan_no_connect;
	priv->scan_expire = 2;
	priv->special_scan = 2;
	if(atbm_internal_cmd_scan_triger(priv,scan_param,NULL,0)){
		return -2;
	}

	wifi_printk(WIFI_ALWAYS,"wait scan done++\n");
	//wait scan done,, wait scan complete
	while(1){
		atbm_mdelay(1000);
		if(priv->scan.in_progress == 0)
			break;
		if(waitloop-- <=0){	
			wifi_printk(WIFI_ALWAYS,"wait scan done++timeout drop\n");
			return -2;
		}
	}
	wifi_printk(WIFI_ALWAYS,"wait scan done--,scan_ret.len(%d)\n",priv->scan_ret.len);
		
	pScanResult = scan_buf;
	if(scan_param->max_ap_number == 0){
		pScanResult->ap = (FH_WiFi_AccessPoint *)atbm_kmalloc(sizeof(FH_WiFi_AccessPoint) * priv->scan_ret.len,GFP_KERNEL);
		pScanResult->count = priv->scan_ret.len;
	}else{
		if(scan_param->max_ap_number > priv->scan_ret.len)
			pScanResult->count = priv->scan_ret.len;
		else
			pScanResult->count = scan_param->max_ap_number;
		pScanResult->ap = (FH_WiFi_AccessPoint *)atbm_kmalloc(sizeof(FH_WiFi_AccessPoint) * pScanResult->count,GFP_KERNEL);
	//	pScanResult->count = scan_param->max_ap_number;
	}
	memset(pScanResult->ap,0,sizeof(FH_WiFi_AccessPoint) * pScanResult->count);
	bss_info = (FH_WiFi_AccessPoint *)pScanResult->ap;
	//will copy to user API and delete AP list from driver's ap list. here porting for Mstar
	//for(i=0;i< priv->scan_ret.len ;i++){
	for(i=0;i< pScanResult->count ;i++){
		info = priv->scan_ret.info + i;	
		//Copy ATBM scanned bss list  to platform dependent BSS list
		atbm_memcpy(bss_info->ssid, info->ssid,  info->ssidlen);
		atbm_memcpy(bss_info->bssid, info->BSSID, ATBM_ETH_ALEN);
		bss_info->channel	    = info->channel;
		bss_info->rssi 			= info->rssi;

		wifi_printk(WIFI_ALWAYS,"security = %d , %x \n",info->security,info->capability);
		bss_info->security = 0;
		if(info->encrypt){
			if(info->wpa)
				bss_info->security		|= ATBM_WPA_CIPHER_TKIP;
			if(info->rsn)
				bss_info->security		|= ATBM_WPA_CIPHER_CCMP;
			
			if(info->ht == 0 && info->security == 1)
				bss_info->security = ATBM_WPA_CIPHER_WEP40;
			
			if(bss_info->security == 0 && info->ht)
				bss_info->security = ATBM_WPA_CIPHER_GCMP;
					
			
		}else{
			bss_info->security = ATBM_WPA_CIPHER_NONE;
		}
		bss_info++;
	}
	
	priv->scan_no_connect = priv->scan_no_connect_back;
	atbmwifi_eloop_cancel_timeout(sta_scan_start_timer_func, (atbm_void *)priv, ATBM_NULL);
	wifi_printk(WIFI_ALWAYS,"wait scan done,pScanResult->count(%d)\n",pScanResult->count);
	return 0;
}

/*
 * 功能: 扫描周边热点,并且返回扫描到的AP列表.
 *       
 *
 * 注意: 在调用函数atbm_scan_ext之后,你必须调用函数atbm_free_scan
 *       释放扫描过程中动态分配的内存.
 *
 * 参数描述:
 * max_ap_num[IN]:        期望返回的扫描到的AP的最大数盿 0表示没有限制.
 * ssid[IN]:              带ssid的扫揿一般情况下填NULL即可.
 * scan_channel_down[IN]: 指定扫描信道的起始信避
 * scan_channel_up[IN]:   指定扫描信道的结束信避
 *      比如: scan_channel_down=4, scan_channel_up=6, 表示在[4,5,6]三个信道扫描
 *      比如: scan_channel_down=0, scan_channel_up=0, 表示全信道扫揿 *
 * 返回倿 NULL表示扫描失败, 否则表示扫描到的AP列表.
 */


FH_WiFi_AccessPoint_List* atbm_scan_ext(FH_UINT32 max_ap_num, FH_CHAR *ssid,
							FH_UINT32 scan_channel_down, FH_UINT32 scan_channel_up)
{
	FH_UINT32 i = 0;
	FH_WiFi_AccessPoint_List *AP_List=NULL;
	scan_param_t scan_param;
	FH_WiFi_AccessPoint *bss_info;

		if((scan_channel_up < scan_channel_down) ||
			(scan_channel_up < 0) || 
			(scan_channel_down<0) ||
			(max_ap_num < 0)
		){
			wifi_printk(WIFI_ALWAYS,"ERROR : start chan[%d],end chan[%d] ,max_ap_num[%d]\n",
										scan_channel_down,scan_channel_up,max_ap_num);
			return NULL;
		}

		atbm_os_mutexLock(&g_hw_prv.scan_lock,RT_WAITING_FOREVER);
		if((scan_channel_down == 0) && (scan_channel_up == 0)){		
			scan_param.start_chan = 1;
			scan_param.end_chan = 14;
		}else{
			scan_param.start_chan = scan_channel_down;
			scan_param.end_chan = scan_channel_up;
		}
		
		scan_param.ssid = ssid;
		if(ssid)
			scan_param.ssid_len = strlen(ssid);
		else
			scan_param.ssid_len = 0;
		scan_param.max_ap_number = max_ap_num;
		AP_List = (FH_WiFi_AccessPoint_List *)atbm_kmalloc(sizeof(FH_WiFi_AccessPoint_List),GFP_KERNEL);
		AP_List->ap = NULL;
		AP_List->count = 0;
		
		if(atbm_wifi_scan_bss(0,AP_List,&scan_param) < 0){
			atbm_os_mutexUnLock(&g_hw_prv.scan_lock);
			return NULL;
		}
		
		for(i = 0; i < AP_List->count;i++){
			bss_info = AP_List->ap + i;
			wifi_printk(WIFI_ALWAYS,"ssid		:%s\n",bss_info->ssid);
			wifi_printk(WIFI_ALWAYS,"bssid		:"STR2MAC"\n",MAC2STR(bss_info->bssid));
			wifi_printk(WIFI_ALWAYS,"RSSI		:%d\n",bss_info->rssi);
			wifi_printk(WIFI_ALWAYS,"channel		:%d\n",bss_info->channel);
			wifi_printk(WIFI_ALWAYS,"security	:%s\n",wpa_cipher_txt(bss_info->security));
			wifi_printk(WIFI_ALWAYS,"\n");

			switch (bss_info->security) {
			case ATBM_WPA_CIPHER_NONE:
				bss_info->security = 1;
				break;
			case ATBM_WPA_CIPHER_WEP40:
			case ATBM_WPA_CIPHER_WEP104:
				bss_info->security = 2;
				break;
			case ATBM_WPA_CIPHER_TKIP://WPA
				bss_info->security = 4;
				break;
			case ATBM_WPA_CIPHER_CCMP://RSN		
			case ATBM_WPA_CIPHER_CCMP | ATBM_WPA_CIPHER_TKIP:
				bss_info->security = 8;
				break;
			case ATBM_WPA_CIPHER_GCMP://wpa3
				bss_info->security = 16;
				break;
			default:
				bss_info->security = 0;
				break;
			}
		}
		
		wifi_printk(WIFI_ALWAYS,"AP_List addr : 0x%x,0x%x \n",AP_List,AP_List->ap);
		atbm_os_mutexUnLock(&g_hw_prv.scan_lock);

		return AP_List;
	
	
}

FH_VOID atbm_free_scan(FH_WiFi_AccessPoint_List *aplist)
{
	wifi_printk(WIFI_ALWAYS,"AP_List addr : 0x%x \n",aplist);
	if(aplist){
		if(aplist->ap){
			wifi_printk(WIFI_ALWAYS,"AP_List->ap addr : 0x%x \n",aplist->ap);
			atbm_kfree(aplist->ap);
			aplist->ap = NULL;
		}
		atbm_kfree(aplist);
		aplist = NULL;
	}
}
/*

All-channel scan for debugging purposes

*/

FH_SINT32 atbm_sta_scan(FH_VOID)
{
	FH_WiFi_AccessPoint_List *aplist = NULL;

	atbm_os_mutexLock(&g_hw_prv.scan_lock,RT_WAITING_FOREVER);
	aplist = atbm_scan_ext(0,NULL,0,0);
	if(aplist)
		atbm_free_scan(aplist);
	atbm_os_mutexUnLock(&g_hw_prv.scan_lock);
	
	return 0;
}

unsigned char atbm_getVifCurr(FH_VOID)
{
	return g_hw_prv.vif_current;
}

char atbm_change_hw_addr(struct atbmwifi_common *hw_priv,char *new_mac)
{
	atbm_uint8 macAddr[6] = {0};
	if (!new_mac && atbm_memcmp(&new_mac[0], &macAddr[0],ATBM_ETH_ALEN) == 0)
	{
		wifi_printk(WIFI_ALWAYS,"atbm_change_hw_addr fail , new_mac = %d\n",new_mac);
		return -1;
	}
	
	atbm_memcpy(macAddr,new_mac,ATBM_ETH_ALEN);
	if (macAddr[0]| macAddr[1]|macAddr[2]|macAddr[3]|macAddr[4]|macAddr[5])
	{
		atbm_memcpy(hw_priv->addresses[0].addr,macAddr,ATBM_ETH_ALEN);
	}
	

	if (hw_priv->addresses[0].addr[1] == 0 &&
		hw_priv->addresses[0].addr[2] == 0 &&
		hw_priv->addresses[0].addr[3] == 0 &&
		hw_priv->addresses[0].addr[4] == 0 &&
		hw_priv->addresses[0].addr[5] == 0)
	{
		
		hw_priv->addresses[0].addr[0] = (atbm_uint8)0;
		hw_priv->addresses[0].addr[3] = (atbm_uint8)atbm_os_random();
		hw_priv->addresses[0].addr[4] = (atbm_uint8)atbm_os_random();
		hw_priv->addresses[0].addr[5] = (atbm_uint8)atbm_os_random();

	}
	
	hw_priv->addresses[1].addr[0] = hw_priv->addresses[0].addr[0] ^ 2;
	hw_priv->addresses[1].addr[1] = hw_priv->addresses[0].addr[1];
	hw_priv->addresses[1].addr[2] = hw_priv->addresses[0].addr[2];
	hw_priv->addresses[1].addr[3] = hw_priv->addresses[0].addr[3];
	hw_priv->addresses[1].addr[4] = hw_priv->addresses[0].addr[4];
	hw_priv->addresses[1].addr[5] = hw_priv->addresses[0].addr[5];

	
	wifi_printk(WIFI_ALWAYS,"MAC Addr[0]:[%02x:%02x:%02x:%02x:%02x:%02x]\n",hw_priv->addresses[0].addr[0],
								hw_priv->addresses[0].addr[1],
								hw_priv->addresses[0].addr[2],
								hw_priv->addresses[0].addr[3],
								hw_priv->addresses[0].addr[4],
								hw_priv->addresses[0].addr[5]);
	return 0;
}


unsigned char change_interface_mac(struct atbmwifi_common *hw_priv)
{
	struct atbmwifi_vif *priv = ATBM_NULL;
	int if_id = 0;	
	for(if_id = 0;if_id < 2;if_id++){
		priv=_atbmwifi_hwpriv_to_vifpriv(hw_priv,if_id);
		if(priv){
			atbm_memcpy(priv->mac_addr,hw_priv->addresses[priv->if_id].addr,6);
		}else{
			wifi_printk(WIFI_ALWAYS,"priv is NULL,change_interface_mac fail! \n");
			return -1;
		}
	}
	return 0;
}



unsigned char atbm_changeMac(FH_CHAR *mac)
{
	struct atbmwifi_common *hw_priv;
	unsigned char mac_buf[6] = {0};
	
	if(!mac){
		wifi_printk(WIFI_ALWAYS,"input MAC is NULL \n");	
		return -1;
	}
	hw_priv = &g_hw_prv;
	wifi_printk(WIFI_ALWAYS,"input mac : %s \n",mac);
	
	sscanf(mac,"%02x:%02x:%02x:%02x:%02x:%02x",
				&mac_buf[0],&mac_buf[1],&mac_buf[2],&mac_buf[3],&mac_buf[4],&mac_buf[5]);

				
	wifi_printk(WIFI_ALWAYS,"new MAC Addr[0]:[%02x:%02x:%02x:%02x:%02x:%02x]\n",
								mac_buf[0],mac_buf[1],mac_buf[2],mac_buf[3],mac_buf[4],mac_buf[5]);

	
	if(atbm_change_hw_addr(hw_priv,mac_buf) < 0){
		wifi_printk(WIFI_ALWAYS,"atbm_change_hw_addr err \n");	
		return -1;
	}	
	
	if(change_interface_mac(hw_priv) < 0){
		wifi_printk(WIFI_ALWAYS,"change_interface_mac err \n");	
		return -1;
	}

	atbmwifi_setup_mac(hw_priv);
	
	wifi_printk(WIFI_ALWAYS,"atbm_changeMac success \n");
	return 0;
}
/*
 * func: atbm_ratePower
 * country: 
 		default : 0
 * used_flag: 
 		0 : clear config
 		1 : used config
 
 进入 ap 模式
 */
enum country_power{
	CN = 0,
	US = 1,
	EU = 2,
	JP = 3,
};


/*
	
*/	
#define SET_RATE_POWER(rate_buff,b_1M_2M, b_5_5M_11M, g_6M_n_6_5M, g_9M, \
						g_12M_n_13M, g_18M_n_19_5M,g_24M_n_26M, g_36M_n_39M, \
						g_48M_n_52M, g_54M_n_58_5M,n_65M, \
						b_1M_2M_40M, b_5_5M_11M_40M, g_6M_n_6_5M_40M, \
						g_9M_40M, g_12M_n_13M_40M, g_18M_n_19_5M_40M, \
						g_24M_n_26M_40M,g_36M_n_39M_40M, g_48M_n_52M_40M, \
						g_54M_n_58_5M_40M, n_65M_40M) \
						{ \
						rate_buff[0] = b_1M_2M;\
						rate_buff[1] = b_5_5M_11M;\
						rate_buff[2] = g_6M_n_6_5M;\
						rate_buff[3] = g_9M;\
						rate_buff[4] = g_12M_n_13M;\
						rate_buff[5] = g_18M_n_19_5M;\
						rate_buff[6] = g_24M_n_26M;\
						rate_buff[7] = g_36M_n_39M;\
						rate_buff[8] = g_48M_n_52M;\
						rate_buff[9] = g_54M_n_58_5M;\
						rate_buff[10] = n_65M; \
						rate_buff[12] = b_1M_2M_40M;\
						rate_buff[12] = b_5_5M_11M_40M;\
						rate_buff[14] = g_6M_n_6_5M_40M;\
						rate_buff[15] = g_9M_40M;\
						rate_buff[16] = g_12M_n_13M_40M;\
						rate_buff[17] = g_18M_n_19_5M_40M;\
						rate_buff[18] = g_24M_n_26M_40M;\
						rate_buff[19] = g_36M_n_39M_40M;\
						rate_buff[20] = g_48M_n_52M_40M;\
						rate_buff[21] = g_54M_n_58_5M_40M;\
						rate_buff[22] = n_65M_40M;  }

unsigned char atbm_ratePower(FH_UINT32 country,FH_UINT32 used_flag)
{
	struct atbmwifi_common *hw_priv;
	char rate_txpower[23] = {0};
	int  ret = -1;
	//atbmwifi_enable_lmaclog(1);
	hw_priv = &g_hw_prv;
	switch(country){
		case CN:{
			SET_RATE_POWER(rate_txpower,3,3,6,6,6,6,6,6,5,1,1,0,0,0,0,0,0,0,0,0,0,0);
			hw_priv->country_channel = 13;
			memcpy(hw_priv->country_code,"CN",2);
			printf("max singal num: %d\n", hw_priv->country_channel);
			atbmwifi_ieee80211_channel_country(hw_priv,country_chinese);
			}break;
		case US:{
			SET_RATE_POWER(rate_txpower,3,3,6,6,6,6,6,6,5,1,1,0,0,0,0,0,0,0,0,0,0,0);
			hw_priv->country_channel = 11;
			memcpy(hw_priv->country_code,"US",2);
			printf("max singal num: %d\n", hw_priv->country_channel);
			atbmwifi_ieee80211_channel_country(hw_priv,country_usa);
			}break;
		case EU:{
			SET_RATE_POWER(rate_txpower,-6,-6,-2,-2,-2,-2,-2,-2,-2,-3,-1,0,0,0,0,0,0,0,0,0,0,0);
			hw_priv->country_channel = 13;
			memcpy(hw_priv->country_code,"EU",2);
			printf("max singal num: %d\n", hw_priv->country_channel);
			atbmwifi_ieee80211_channel_country(hw_priv,country_chinese);
			}break;
		case JP:{
			SET_RATE_POWER(rate_txpower,-6,-6,-2,-2,-2,-2,-2,-2,-2,-3,-1,0,0,0,0,0,0,0,0,0,0,0);
			hw_priv->country_channel = 14;
			memcpy(hw_priv->country_code,"JP",2);
			printf("max singal num: %d\n", hw_priv->country_channel);
			atbmwifi_ieee80211_channel_country(hw_priv,country_japan);
			}break;
		default:{
			SET_RATE_POWER(rate_txpower,3,3,6,6,6,6,6,6,5,1,1,0,0,0,0,0,0,0,0,0,0,0);
			hw_priv->country_channel = 13;
			memcpy(hw_priv->country_code,"CN",2);
			printf("max singal num: %d\n", hw_priv->country_channel);
			atbmwifi_ieee80211_channel_country(hw_priv,country_chinese);
			}break;
	}
	rate_txpower[11] = used_flag;
	ret = atbm_wifi_set_spec_rate_txpower_mode(rate_txpower,used_flag);

	return ret;
}

/*
channel:
	1~14
rate_value:
	11b : 10 , 20 , 55, 110
	11g : 60 , 90 , 120 , 180 , 240 , 360 , 480 , 540
	11n : 65 , 130 , 195 , 260 , 390 , 520 , 585 , 650
is_40M :
	0 : HT20 mode
	1 : HT40 mode
greedfiled:
	0 : leacy filed mode
	1 : greed filed mode
	usualy select 0

*/
FH_SINT32 atbm_start_tx(int channel,int mode, int rateIdx,int  bw, int chOff, int ldpc, int packetLen)
{
	return atbm_etf_start_tx(channel,mode, rateIdx, bw, chOff, ldpc, packetLen);
}

FH_SINT32 atbm_stop_tx(FH_VOID)
{
	return atbm_etf_stop_tx();
}
FH_SINT32 atbm_start_rx(int channel ,int bw, int chOff, int mode)
{
	return atbm_etf_start_rx(channel , bw,  chOff, mode);
}
FH_SINT32 atbm_stop_rx(FH_VOID)
{
	return atbm_etf_stop_rx();
}






/*
 * func: start and work as AP
 * para1: ssid
 * para2: passwd
 * para3: chan_id
 
 进入 ap 模式
 */

FH_SINT32 atbm_ap_on(FH_CHAR *ssid, FH_CHAR *passwd, FH_UINT32 chan_id)
{
	int ret,i,mode;
	
	struct atbmwifi_vif *priv=ATBM_NULL;
	
	
	if(!wifi_inited){
		atbm_wifi_hw_init();
		wifi_inited = 1;
#ifndef WIFI_SDIO
		rt_thread_delay(150);
#endif
	}
	
	priv= _atbmwifi_hwpriv_to_vifpriv(&g_hw_prv,0);
	wifi_printk(WIFI_ALWAYS,"priv->iftype = %d\n",priv->iftype);
	if(priv){
		atbm_lwip_disable(priv->ndev);
		if(priv->iftype == ATBM_NL80211_IFTYPE_STATION)
			atbm_wifi_off(0);			
	}	
	
	atbm_wifi_off(1);
	rt_thread_delay(150);
	atbm_wifi_on(1);
	//rt_thread_delay(150);
    atbm_os_mutexLock(&g_hw_prv.ap_create_lock,RT_WAITING_FOREVER);
    ret = atbm_wifi_ap_create(ssid, (passwd && passwd[0]) ? 0x4 : 0, 0, passwd, chan_id, 0);
    atbm_os_mutexUnLock(&g_hw_prv.ap_create_lock);
    if (ret)
    {
        wifi_printk(WIFI_DBG_ERROR,"%s-%d fail, ret %d\n", __func__, __LINE__, ret);
		//return ret;
    }

	return ret;
}


FH_SINT32 atbm_g_sta_num(FH_VOID)
{
	int i =0,ap = 0,sta_num = 0;	
	struct atbmwifi_vif *priv; 
	for(i = 0;i < ATBM_WIFI_MAX_VIFS;i++){
		priv = g_hw_prv.vif_list[i];
		if (atbmwifi_is_ap_mode(priv->iftype)){
			ap = 1;
			break;
		}
	}
	if (!ap){
		wifi_printk(WIFI_DBG_ERROR,"w_get_assoc_num : ap not running \n");
		return -1;
	}
	
	for (i = 0; i < ATBMWIFI__MAX_STA_IN_AP_MODE; ++i) {
		if (priv->link_id_db[i].status == ATBMWIFI__LINK_HARD){
				sta_num++;
		}
	}
	wifi_printk(WIFI_DBG_ERROR,"now connect sta number [%d]  \n",sta_num);
	return sta_num;
}

extern struct atbmwifi_common g_hw_prv;
/* 鐢ㄤ簬鍒ゆ柇WiFi椹卞姩閭ｄ釜鏄惁鍒濆鍖栧畬鎴 */
unsigned char atbm_get_g_hw_prv_vif_current(FH_VOID)
{
	return g_hw_prv.vif_current;
}


extern int atbm_set_frame_filter(struct atbmwifi_common *hw_priv,struct atbmwifi_vif *priv,unsigned int n_filters,
		  struct ieee80211_special_filter *filter_table,char enable);
extern int set_special_beacon(struct atbmwifi_vif *priv,char *special_ie,int special_ie_len);

extern int set_special_proberesp(struct atbmwifi_vif *priv,char *special_ie,int special_ie_len);
extern int set_special_scan(struct atbmwifi_vif *priv,char *special_ie,int special_ie_len,char chan);


/*



paramters : 

	ap_mode : 	current wifi mode 
				0 : STA MODE
				1 : AP MODE
				
	on 		:  	filter on & off
				0 : off , clear filter set
				1 : on , set filter
				
	action 	: 	filter frame type
				64 (0x40)：probe req
				80 (0x50): probe resp
				128(0x80): beacon

*/
#if 0
#define FILTER_BEACON 0x80
#define FILTER_PROBE_REQ 0x40
#define FILTER_PROBE_RESP 0x50
#endif

int atbm_rx_filter(int ap_mode,char on,unsigned char action)
{
	int i =0,mode = -1;	
	struct atbmwifi_vif *priv; 

	for(i = 0;i < ATBM_WIFI_MAX_VIFS;i++){
		priv = g_hw_prv.vif_list[i];
		if (atbmwifi_is_ap_mode(priv->iftype) && (ap_mode == 1)){
			mode = 1;
			break;
		}
		if (atbmwifi_is_sta_mode(priv->iftype) && (ap_mode == 0)){
			mode = 0;
			break;
		}
	}
	
	if (mode != ap_mode){
		wifi_printk(WIFI_DBG_ERROR,"atbm_rx_filter :%s mode not running \n",ap_mode?"AP":"STA");
		return -1;
	}
	
	if(on == 1){
		wifi_printk(WIFI_DBG_ERROR,"atbm_rx_filter : action(%d) \n",action);
		switch(action){
			case FILTER_BEACON:
				wifi_printk(WIFI_DBG_ERROR,"atbm_rx_filter : FILTER_BEACON \n");
				break;
			case FILTER_PROBE_REQ:
				wifi_printk(WIFI_DBG_ERROR,"atbm_rx_filter : FILTER_PROBE_REQ \n");
				break;
			case FILTER_PROBE_RESP:
				wifi_printk(WIFI_DBG_ERROR,"atbm_rx_filter : FILTER_PROBE_RESP \n");
				break;
			default:
				wifi_printk(WIFI_DBG_ERROR,"atbm_rx_filter : FILTER_UNKNOW action(%x) \n",action);
				return -1;
		}	
	
		
		priv->filter_table.special_filter[priv->filter_table.filter_num].filter_action = action;
		priv->filter_table.special_filter[priv->filter_table.filter_num].flags = SPECIAL_F_FLAGS_FRAME_TYPE;
		priv->filter_table.filter_num++;
		atbm_set_frame_filter(&g_hw_prv,priv,priv->filter_table.filter_num,&priv->filter_table.special_filter,1);
	}else{
		
		atbm_set_frame_filter(&g_hw_prv,priv,0,NULL,0);
		memset(&priv->filter_table,0,sizeof(struct special_filter_table));
	}	
}
/*
	Gets the filter criteria set on all interfaces 
*/
FH_SINT32 atbm_get_ftrtbl(void)
{
	int i =0,ap = -1,j = 0;	
	struct atbmwifi_vif *priv; 
	for(i = 0;i < ATBM_WIFI_MAX_VIFS;i++){
		priv = g_hw_prv.vif_list[i];
		if (atbmwifi_is_ap_mode(priv->iftype)){
			wifi_printk(WIFI_DBG_ERROR,"current mode is AP , filter table is :  \n");
			if(priv->filter_table.filter_num == 0){
				wifi_printk(WIFI_DBG_ERROR,"no filter  \n");
				continue;
			}			
		}
		
		if (atbmwifi_is_sta_mode(priv->iftype)){
			wifi_printk(WIFI_DBG_ERROR,"current mode is STA , filter table is :  \n");
			if(priv->filter_table.filter_num == 0){
				wifi_printk(WIFI_DBG_ERROR,"no filter   \n");
				continue;
			}
		}
		
		for(j = 0;j < priv->filter_table.filter_num;j++){
			switch(priv->filter_table.special_filter[j].filter_action){
				case FILTER_BEACON:
					wifi_printk(WIFI_DBG_ERROR,"FILTER_BEACON \n");
					break;
				case FILTER_PROBE_REQ:
					wifi_printk(WIFI_DBG_ERROR,"FILTER_PROBE_REQ \n");
					break;
				case FILTER_PROBE_RESP:
					wifi_printk(WIFI_DBG_ERROR,"FILTER_PROBE_RESP \n");
					break;
				default:
					wifi_printk(WIFI_DBG_ERROR,"FILTER_UNKNOW action(%x) \n",priv->filter_table.special_filter[j].filter_action);
					break;
			}	
		}
	}

}
/*
	insert private data on beacon frame
	default : 
		IE ID : 233
		IE LEN : max 255 
		IE DATA : max  255 byte
		
paramters : 
	ie : IE DATA

*/
FH_SINT32 atbm_set_beacon(FH_CHAR *ie)
{
	int i =0,ap = 0,ie_len;	
	struct atbmwifi_vif *priv; 
	char buff[257] = {0};
	for(i = 0;i < ATBM_WIFI_MAX_VIFS;i++){
		priv = g_hw_prv.vif_list[i];
		if (atbmwifi_is_ap_mode(priv->iftype)){
			ap = 1;
			break;
		}
	}
	if (!ap){
		wifi_printk(WIFI_DBG_ERROR,"atbm_set_beacon : ap mode not running \n");
		return -1;
	}
	if(ie){
		ie_len = strlen(ie);
		buff[0] = 233;
		buff[1] = ie_len;
		memcpy(buff+2,ie,buff[1]);
	}else{
		wifi_printk(WIFI_DBG_ERROR,"atbm_priv_scan : ie is NULL\n");
		return -1;
	}

	
	return set_special_beacon(priv,buff,ie_len + 2);
}
/*
	insert private data on probe response frame and send
	default : 
		IE ID : 233
		IE LEN : max 255 
		IE DATA : max  255 byte

paramters : 

	ie 			: IE DATA
	dst_addr	：dest mac addr
					"dc:29:19:11:22:33"
*/
FH_SINT32 atbm_send_prbrsp(FH_CHAR *ie,FH_CHAR *dst_addr)
{
	int i =0,ap = 0,ie_len = 0;	
	struct atbmwifi_vif *priv; 
	unsigned char  buff[257] = {0};
	unsigned char dmac[6] = {0};
	for(i = 0;i < ATBM_WIFI_MAX_VIFS;i++){
		priv = g_hw_prv.vif_list[i];
		if (atbmwifi_is_ap_mode(priv->iftype)){
			ap = 1;
			break;
		}
	}
	if (!ap){
		wifi_printk(WIFI_DBG_ERROR,"atbm_set_prbresp : ap mode not running \n");
		return -1;
	}
	if(dst_addr){
		sscanf(dst_addr,"%02x:%02x:%02x:%02x:%02x:%02x",&dmac[0],&dmac[1],&dmac[2],&dmac[3],&dmac[4],&dmac[5]);
		wifi_printk(WIFI_ALWAYS,"dest MAC Addr:[%02x:%02x:%02x:%02x:%02x:%02x]\n",
								dmac[0],dmac[1],dmac[2],dmac[3],dmac[4],dmac[5]);
	}else{
		wifi_printk(WIFI_ALWAYS,"atbm_send_prbrsp : dst_addr is NULL,send error \n");
		return -1;
	}
	if(ie){
		ie_len = strlen(ie);
		buff[0] = 233;
		buff[1] = ie_len;
		memcpy(buff+2,ie,buff[1]);
	}else{
		wifi_printk(WIFI_DBG_ERROR,"atbm_priv_scan : ie is NULL\n");
		return -1;
	}
	
	return send_special_proberesp(priv,buff,ie_len+2,dmac);
}

/*
	insert private data on probe request frame
	default : 
		IE ID : 233
		IE LEN : max 255 
		IE DATA : max  255 byte

paramters : 

	ie : 
	
	chan: 
			scan channel , range : 1~14
			other val  is scan all channel

*/
FH_SINT32 atbm_priv_scan(FH_CHAR *ie,FH_CHAR chan)
{
	int i =0,sta = 0,ie_len = 0;	
	struct atbmwifi_vif *priv;
	unsigned char buff[257] = {0};
	/*
	
	*/
	for(i = 0;i < ATBM_WIFI_MAX_VIFS;i++){
		priv = g_hw_prv.vif_list[i];
		if (atbmwifi_is_sta_mode(priv->iftype)){
			sta = 1;
			break;
		}
	}
	if (!sta){
		wifi_printk(WIFI_DBG_ERROR,"atbm_priv_scan : sta mode not running\n");
		return -1;
	}
	if(ie){
		ie_len = strlen(ie);
		buff[0] = 233;
		buff[1] = 255;
		wifi_printk(WIFI_DBG_ERROR,"atbm_priv_scan : buff[1] = %d\n",buff[1]);
		for(i = 0;i<255;i++)
			buff[2+i] = 1;
		//memset(buff+2,1,buff[1]);
	}else{
		wifi_printk(WIFI_DBG_ERROR,"atbm_priv_scan : ie is NULL\n");
		return -1;
	}
	
	return set_special_scan(priv,buff,buff[1] + 2,chan);
}

/*
key_mgmt = 0 no encrypto
key_mgmt = 1 wep
key_mgmt = 2 other

return connect channel
*/


FH_SINT32 start_br_sta(FH_CHAR *conn_ssid,FH_CHAR *conn_pwd)
{
	int psk_len,key_mgmt,ret;
	struct atbmwifi_vif *priv ;
	struct wpa_supplicant *g_wpa_s;
	int sta_mode_stat,conn_wait = 5;
	if(!conn_pwd || !conn_ssid){
		wifi_printk(WIFI_DBG_ERROR,"start_br_sta: passwd or ssid is NULL!! \n");
		return -1;
	}
/*
	reset station mode
*/	
	atbm_stop(0);
	atbm_SleepMs(200);
	atbm_start(0,0);
	atbm_SleepMs(200);

	
	priv=_atbmwifi_hwpriv_to_vifpriv(&g_hw_prv,ATBM_NL80211_IFTYPE_STATION);
	g_wpa_s = priv->appdata;
	if(!g_wpa_s){
		wifi_printk(WIFI_DBG_ERROR,"sta application(wpa_supplicant) not running , sta mode start fail \n");
		return -1;
	}
	

	psk_len = strlen(conn_pwd);
		
	if(psk_len == 0){
		psk_len = 0;
		key_mgmt = 0;

	}
	else if(psk_len < 33 && psk_len >0 ){
		if(psk_len <= 5)
			key_mgmt = 1;
		else
			key_mgmt = 2;
	}else{
		wifi_printk(WIFI_DBG_ERROR,"passwd[%s],passwd len[%d] > 32\n",conn_pwd,psk_len);
	}

	atbm_os_mutexLock(&g_hw_prv.conn_ap_lock,RT_WAITING_FOREVER);
	if(key_mgmt == 1)//wep
		ret = atbm_wifi_sta_join_ap(conn_ssid, RT_NULL, 1, 1, conn_pwd);
	else if(key_mgmt == 0)//other
		ret = atbm_wifi_sta_join_ap(conn_ssid, RT_NULL, 0, 0, RT_NULL);	
	else
		ret = atbm_wifi_sta_join_ap(conn_ssid, RT_NULL, 4, 0, conn_pwd);
	
	atbm_os_mutexUnLock(&g_hw_prv.conn_ap_lock);	
	
	while(conn_wait--){
		if(g_wpa_s->wpa_state == ATBM_WPA_COMPLETED){
			wifi_printk(WIFI_DBG_ERROR,"connect success! channel[%d]\n",priv->config.channel_index);
			return priv->config.channel_index;
		}
		atbm_SleepMs(200);
	}
	ret = 0;
	return ret;
}





FH_SINT32 atbm_br_start(FH_CHAR *conn_ssid,FH_CHAR *conn_pwd,FH_CHAR *ap_ssid,FH_CHAR *ap_pwd)
{
	int ret = 0,channel;
	struct atbmwifi_vif *priv ;
	struct wpa_supplicant *g_wpa_s;
	
	channel = start_br_sta(conn_ssid,conn_pwd);
	if(channel < 0){
		wifi_printk(WIFI_DBG_ERROR,"start br_sta mode failed \n");
		return -1;
	}else if(channel == 0){
		wifi_printk(WIFI_DBG_ERROR,"start br_sta mode success , but not connect ap , ap channel default 6\n");
		channel = 6;
	}else if(channel > 14){
		wifi_printk(WIFI_DBG_ERROR,"start br_sta mode success , but channel[%d] not allow! \n",channel);
		return -1;
	}
	/*
		start dhcp get ipaddr
	*/
	atbm_ap_on(ap_ssid,ap_pwd,channel);
	
	
	return ret;
}




//#endif

