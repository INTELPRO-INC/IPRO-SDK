/**************************************************************************************************************
 * altobeam RTOS API
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 *
 *  The source code contains proprietary information of AltoBeam, and shall not be distributed, 
 *  copied, reproduced, or disclosed in whole or in part without prior written permission of AltoBeam.
*****************************************************************************************************************/


#include "atbm_hal.h"
#include "atbm_wifi_driver_api.h"
#include "atbm_etf.h"
extern struct atbmwifi_common g_hw_prv;
/*ATBM WIFI READY*/

extern atbm_void* atbm_wifi_vif_get(int id);

extern int atbm_etf_set_powerTarget(int powerTar);
extern int atbm_etf_get_powerTarget(atbm_void);
extern atbm_int32 atbm_etf_start_tx_single_tone(int channel);
extern atbm_int32 atbm_etf_PT_Test_start(atbm_uint8 if_id,/*atbm_int32 targetFreq, atbm_int32 rssiFilter, atbm_int32 evmFilter, atbm_int32 cableLoss, */atbm_int32 isWriteEfuse);
extern atbm_int32 atbmwifi_get_rssi_avg(atbm_void);
extern atbm_int32 atbmwifi_set_tx_time(atbm_uint32 time_period, atbm_uint32 time_transmit);
extern atbm_int32 atbmwifi_set_retry(atbm_uint32 retry_num, atbm_uint32 retry_time_ms);
extern atbm_int32 atbmwifi_set_txpower(atbm_uint32 txpower_idx);
extern atbm_int32 atbmwifi_set_tx_rate(atbm_int32 rate);
extern atbm_int32 atbmwifi_set_sgi(atbm_uint32 sgi);
extern atbm_int32 atbmwifi_set_rate_txpower_mode(atbm_uint32 txpower_idx);
extern atbm_int32 atbmwifi_get_lmacLog_to_host(atbm_uint32 value);
extern atbm_int32 atbm_set_debug_to_host(atbm_uint32 value);
extern atbm_int32 atbmwifi_dev_set_adaptive(atbm_int32 val);
extern int atbmwifi_get_Tjroom(struct Tjroom_temperature_t *tjroom);

atbm_int32 atbm_wifi_get_current_mode_vif(atbm_uint8 if_id);   //0 : sta, 1: SW AP
int atbmwifi_scan(struct atbmwifi_vif *priv);


/****************************************************************************
* Function:   	atbm_wifi_hw_init
*
* Purpose:   	This function is used to initialize and start atbm wifi  hardware.
may be GPO, BUS PROBE, firmware init etc.
*
* Parameters: none
*
* Returns:	Returns 0 if succeed, otherwise a negative error code.
****************************************************************************/
atbm_int32  atbm_wifi_hw_init(atbm_void)
{
	//int ret;
	//int i;
	#if ATBM_USB_BUS
	atbm_usb_module_init();
	#else		
	atbm_sdio_module_init();
	#endif
	return 0;
}

/****************************************************************************
* Function:   	atbm_wifi_hw_deinit
*
* Purpose:   	This function is used to release and clean up the driver
*
* Parameters: none
*
* Returns:	Returns 0 if succeed, otherwise a negative error code.
****************************************************************************/
atbm_int32  atbm_wifi_hw_deinit(atbm_void)
{
	//int ret;
	//int i;
	#if ATBM_USB_BUS
	atbm_usb_module_exit();
	#else		
	atbm_sdio_module_exit();
	#endif
	return 0;
}

/****************************************************************************
* Function:   	atbm_wifi_vif_get
*
* Purpose:   	This function is used to get atbm wifi  interface id priv.                     
*
* Parameters:   0:Station InterfaceId, 1 Ap InterfaceId
*
* Returns:	void
****************************************************************************/

atbm_void* atbm_wifi_vif_get(int if_id)
{
    atbm_int32 waitloop = 100;
	/*Wait for atbmwifi fw & hmac done*/
	struct atbmwifi_vif *priv;
	while(1) {
		priv = _atbmwifi_hwpriv_to_vifpriv(&g_hw_prv,if_id);
		if(priv == ATBM_NULL){
			atbm_SleepMs(50);
		}else{
			break;
		}

		if(waitloop-- < 0){
			wifi_printk(WIFI_ALWAYS,"wait atbm_wifi_vif_get +timeout drop\n");
			break;
		}
	}

	return (atbm_void *)priv;
}
/****************************************************************************
* Function:   	atbm_wifi_on
*
* Purpose:   	This function is used to initialize and start atbm wifi  module as AP mode or STA mode.                     
*
* Parameters: AP_sta_mode     0: Ap Mode, 1 STA mode
*
* Returns:	Returns 0 if succeed, otherwise a negative error code.
****************************************************************************/
atbm_void* atbm_wifi_on_vif(ATBM_WIFI_MODE AP_sta_mode,atbm_uint8 if_id)
{
	atbm_int32 CurrentMode;
	struct atbmwifi_vif *priv;
	priv= _atbmwifi_hwpriv_to_vifpriv(&g_hw_prv,if_id);
	if(priv == ATBM_NULL){
		wifi_printk(WIFI_ALWAYS,"priv==NULL, if_id:%d\n", if_id);
		return ATBM_NULL;
	}
	/*Get Curret Mode,If Conf Mode is not equal CurrentMode,Do Modify to conf Mode */
	CurrentMode =  atbm_wifi_get_current_mode_vif(if_id); 
	if(atbmwifi_is_ap_mode(CurrentMode) &&(AP_sta_mode == ATBM_WIFI_AP_MODE)){
		wifi_printk(WIFI_ALWAYS,"<WARNING>atbm_wifi_on_vif+_mode %d+\n",CurrentMode);
		return (atbm_void* )priv->ndev;
	}
	if(atbmwifi_is_sta_mode(CurrentMode) &&(AP_sta_mode == ATBM_WIFI_STA_MODE)){
		wifi_printk(WIFI_ALWAYS,"<WARNING>atbm_wifi_on_vif+_mode %d+\n",CurrentMode);
		return (atbm_void* )priv->ndev;
	}

	if(AP_sta_mode == ATBM_WIFI_AP_MODE)
	{
		atbmwifi_start_wifimode(priv,ATBM_NL80211_IFTYPE_AP);
	} 
	if(AP_sta_mode == ATBM_WIFI_STA_MODE)
	{
		atbmwifi_start_wifimode(priv,ATBM_NL80211_IFTYPE_STATION);
	}

	return (atbm_void* )priv->ndev;

}
/****************************************************************************
* Function:   	atbm_wifi_on
*
* Purpose:   	This function is used to start atbm wifi  module.
*
* Returns:	Returns none.
*****************************************************************************/
atbm_void* atbm_wifi_on( ATBM_WIFI_MODE AP_sta_mode)
{
	int if_id=0;
	switch (AP_sta_mode){
			case ATBM_NL80211_IFTYPE_STATION:
			case ATBM_NL80211_IFTYPE_P2P_CLIENT:
				if_id=0;
				break;
			case ATBM_NL80211_IFTYPE_AP:
			case ATBM_NL80211_IFTYPE_P2P_GO:
				if_id=1;
				break;
			default:
				wifi_printk(WIFI_ALWAYS,"Mode cant support,Pls check it !!!\n");
				return ATBM_NULL;
	}	
	
	return atbm_wifi_on_vif(AP_sta_mode,if_id);
}

/****************************************************************************
* Function:   	atbm_wifi_off
*
* Purpose:   	This function is used to stop atbm wifi  module.
*
* Returns:	Returns none.
*****************************************************************************/
atbm_void  atbm_wifi_off_vif(atbm_uint8 if_id)
{
	atbm_int32 i32current_mode;
	struct atbmwifi_vif *priv=ATBM_NULL;
	priv= _atbmwifi_hwpriv_to_vifpriv(&g_hw_prv,if_id);

	if(priv == ATBM_NULL)
		return ;

	i32current_mode =	atbm_wifi_get_current_mode_vif(if_id);   // get current mode
	atbmwifi_stop_wifimode(priv,priv->iftype);
}

atbm_void  atbm_wifi_off(atbm_uint8 if_id)
{
	atbm_wifi_off_vif(if_id);
}

/****************************************************************************
* Function:   	atbm_wifi_scan_network
*
* Purpose:   	This function is used to ask driver to perform channel scan and return scan result.
*
* Parameters: scan_buf		Buffer to store the information of the found APs
*			buf_size		Size of the buffer
*
* Returns:	Returns 0 if succeed, 1 if some remained untaken, otherwise a negative error code.
******************************************************************************/
int	atbm_wifi_scan_network_vif(atbm_uint8 if_id,char* scan_buf, atbm_uint32 buf_size)
{
	WLAN_BSS_INFO *bss_info;
	WLAN_SCAN_RESULT *pScanResult;
	struct atbmwifi_scan_result_info *info;
    atbm_int32 waitloop = 10;
	atbm_int32 i=0;
	struct atbmwifi_vif *priv=ATBM_NULL;
	priv=_atbmwifi_hwpriv_to_vifpriv(&g_hw_prv,if_id);
	if(ATBM_NULL==priv){
		return -1;
	}
	wifi_printk(WIFI_ALWAYS,"atbm_wifi_scan_network_vif(%d) \n",priv->iftype);
	//wifi_printk(WIFI_ALWAYS,"taken(%d) len(%d)\n",priv->scan_ret.taken, priv->scan_ret.len);
	if(priv->scan_ret.taken < priv->scan_ret.len){
		goto get_result;
	}
	priv->scan_ret.taken = 0;
    priv->scan_no_connect_back = priv->scan_no_connect;
	priv->scan_expire = 2;

	if(atbmwifi_scan(priv)){
		return -2;
	}

	wifi_printk(WIFI_ALWAYS,"wait scan done++\n");
	//wait scan done,, wait scan complete
	while(1){
		atbm_mdelay(1000);
		if(priv->scan.in_progress==0)
			break;
		if(waitloop-- <=0){			
			wifi_printk(WIFI_ALWAYS,"wait scan done++timeout drop\n");
			return -2;
		}
	}
	wifi_printk(WIFI_ALWAYS,"wait scan done--,scan_ret.len(%d)\n",priv->scan_ret.len);
get_result:
	//atbm_wifi_get_scaned_list(scan_buf, buf_size);	
	pScanResult = (WLAN_SCAN_RESULT*)scan_buf;
	pScanResult->count = 0;
	bss_info =  (WLAN_BSS_INFO *)(&pScanResult->bss_info[0]);
	if(buf_size<sizeof(WLAN_SCAN_RESULT))
	{
		wifi_printk(WIFI_ERROR,"scan_buf very little x1\n");
		return -1;
	}

	//will copy to user API and delete AP list from driver's ap list. here porting for Mstar
	for(i=priv->scan_ret.taken;i<priv->scan_ret.len;i++){
		info = priv->scan_ret.info + i;
		if((char *)(bss_info + 1) > scan_buf+buf_size)
		{
			wifi_printk(WIFI_ERROR,"scan_buf very little x2\n");
			return 1;
		}
		//Copy ATBM scanned bss list  to platform dependent BSS list
		bss_info->SSID_len = info->ssidlen;
		if( bss_info->SSID_len > 32 )
		{
			 wifi_printk(WIFI_ALWAYS,"atbm_wifi_scan_network_vif SSID_len(%d)\n",bss_info->SSID_len);
			 bss_info->SSID_len = 32;
		}
		atbm_memcpy(bss_info->SSID, info->ssid,  bss_info->SSID_len);
		atbm_memcpy(bss_info->BSSID, info->BSSID, ATBM_ETH_ALEN);

		bss_info->beacon_period = info->beacon_interval;
		bss_info->capability    = info->capability;
		bss_info->chanspec      = info->channel;
		bss_info->RSSI = (atbm_uint16)info->rssi;
		bss_info->dtim_period   =  info->dtim_period;
		bss_info->security		= info->security;
		bss_info->length = sizeof(WLAN_BSS_INFO); //no ies
		bss_info++;
		pScanResult->count++;
		priv->scan_ret.taken++;
	}
	priv->scan_ret.len = 0;
	priv->scan_no_connect = priv->scan_no_connect_back;

	wifi_printk(WIFI_ALWAYS,"wait scan done,pScanResult->count(%d)\n",pScanResult->count);
	return 0;
}

int atbm_wifi_scan_network(char* scan_buf, atbm_uint32 buf_size)
{ 
	return atbm_wifi_scan_network_vif(0,scan_buf,buf_size);
}


static unsigned char atbm_wifi_scan_flag  = 0;
#if ATBM_SDIO_BUS
extern int atbm_wifi_get_init_flag(void);
#endif
int atbm_scan_get_flag_status(void)
{
	return atbm_wifi_scan_flag;
}
int atbm_scan_set_flag_status(int val)
{
	atbm_wifi_scan_flag = val;
}


int atbm_wifi_scan_open(void)
{
#if ATBM_SDIO_BUS
	if(atbm_wifi_get_init_flag() == 0){
		atbm_wifi_hw_init();
	}
#endif
	if(atbm_scan_get_flag_status() == 1){
		return 0;
	}
	atbm_SleepMs(100);
	atbm_wifi_on(0);
	atbm_scan_set_flag_status(1);
	return 0;
}

int atbm_wifi_scan_close(void)
{
	if(atbm_scan_get_flag_status() == 0){
		return 0;
	}
	atbm_wifi_off(0);
	atbm_scan_set_flag_status(0);
	atbm_SleepMs(100);
#if ATBM_SDIO_BUS
	if(atbm_wifi_get_init_flag() == 1){
		atbm_wifi_hw_deinit();
	}
#endif
}

int atbm_wifi_scan_start(char* scan_buf, atbm_uint32 buf_size)
{
	int ret = 0;
	if((atbm_scan_get_flag_status() == 0) 
#if ATBM_SDIO_BUS
	&& (atbm_wifi_get_init_flag() == 0)
#endif
	){
		return -1;
	}
	
	ret = atbm_wifi_scan_network(scan_buf,buf_size);

	return ret;
}




/****************************************************************************
* Function:   	atbm_wifi_get_mode
*
* Purpose:   	This function is used to get  wifi mode 
*
* Parameters: None
*
* Returns:	Returns 0 if in STA mode, 1 in SW AP mode.
******************************************************************************/
atbm_int32 atbm_wifi_get_current_mode_vif(atbm_uint8 if_id)   //0 : sta, 1: SW AP
{
	struct atbmwifi_vif *priv=ATBM_NULL;

	if(if_id>=2){
		return -1;
	}

	priv= _atbmwifi_hwpriv_to_vifpriv(&g_hw_prv,if_id);

	if(priv == ATBM_NULL)
		return -1;
	  
	return priv->iftype ;
}

atbm_int32 atbm_wifi_get_current_mode(atbm_void)   //0 : sta, 1: SW AP
{
	return atbm_wifi_get_current_mode_vif(0);  //default mode.
}


/****************************************************************************
* Function:   	atbm_wifi_get_mac_address
*
* Purpose:   	This function is used to get wifi MAC address
*
* Parameters: point to buffer of MAC address
*
* Returns:	None.
******************************************************************************/
atbm_void atbm_wifi_get_mac_address_vif(atbm_uint8 if_id,unsigned char *addr)
{
	struct atbmwifi_vif *priv=ATBM_NULL;

	if(if_id>=2){
		return;
	}
	
	priv= _atbmwifi_hwpriv_to_vifpriv(&g_hw_prv,if_id);

	if(priv == ATBM_NULL)
		return;
	if(addr == ATBM_NULL)
	{		   
		return;
	}

	atbm_memcpy(addr, priv->mac_addr, 6);  //AP or STA 
}

atbm_void atbm_wifi_get_mac_address(unsigned char *addr)
{
	atbm_wifi_get_mac_address_vif(0,addr);
}

/****************************************************************************
* Function:   	atbm_temp_modify_mac
*
* Purpose:   	This function is temp change mac addr
*
* Parameters: point to buffer of MAC address
*
* Returns:	0 is success,other is fail.
******************************************************************************/


/**
 * @brief 检测 6字节 MAC 地址是否合法（通用工业标准）
 * @param mac 6字节uint8_t数组，存储二进制MAC
 * @return 合法返回 true，非法返回 false
 */
int atbm_is_mac_valid( atbm_uint8 *mac)
{
    // 1. 检查是否为 全0 MAC (非法)
    if (mac[0] == 0 && mac[1] == 0 && mac[2] == 0 &&
        mac[3] == 0 && mac[4] == 0 && mac[5] == 0) {
        return -1;
    }

    // 2. 检查是否为 广播MAC (全FF，非法)
    if (mac[0] == 0xFF && mac[1] == 0xFF && mac[2] == 0xFF &&
        mac[3] == 0xFF && mac[4] == 0xFF && mac[5] == 0xFF) {
        return -2;
    }

    // 3. 检查是否为 组播MAC (第一个字节最低位为1，非法)
    // 组播MAC规则：第1字节 bit0 = 1
    if ((mac[0] & 0x01) != 0) {
        return -3;
    }

    // 4. 合法单播MAC
    return 0;
}

char * atbm_wifi_mode_to_str(enum atbm_nl80211_iftype 	iftype)
{
	switch(iftype){
		case ATBM_NL80211_IFTYPE_STATION:{
			return "sta";
		}
		case ATBM_NL80211_IFTYPE_AP:{ 
			return "ap";
		}
		case ATBM_NL80211_IFTYPE_ADHOC:{
			return "adhoc";
		}
		case ATBM_NL80211_IFTYPE_MONITOR:{
			return "monitor";
		}
		case ATBM_NL80211_IFTYPE_P2P_CLIENT:{
			return "p2p_gc";
		}
		case ATBM_NL80211_IFTYPE_P2P_GO:{
			return "p2p_go";
		}
		case ATBM_NUM_NL80211_IFTYPES:{
			return "not work";
		}



	}
	return "not work";
}

int atbm_temp_modify_mac(atbm_uint8 *mac)
{
	struct atbmwifi_common *hw_priv = &g_hw_prv;
	struct atbmwifi_vif		*priv;
	int ret = 0;
	int i = 0;
	for(i = 0;i < ATBM_WIFI_MAX_VIFS;i++){
		priv = hw_priv->vif_list[i];
		if(priv->iftype != ATBM_NUM_NL80211_IFTYPES){
			wifi_printk(WIFI_ALWAYS,"%s: wifi is work,work mode[%s],not change mac \n",__func__,atbm_wifi_mode_to_str(priv->iftype));
			return -1;
		}
	}

	if(mac == NULL){
		wifi_printk(WIFI_ALWAYS,"%s: mac is NULL \n",__func__);
		return -1;
	}

	
	ret = atbm_is_mac_valid(mac);

	if( ret != 0){
		switch(ret){
			case -1:{
				wifi_printk(WIFI_ALWAYS,"%s: mac is all 0 \n",__func__);
				}break;
			case -2:{
				wifi_printk(WIFI_ALWAYS,"%s: mac is all ff \n",__func__);
				}break;
			case -3:{
				wifi_printk(WIFI_ALWAYS,"%s: mac is braodcast addr,fist mac[%x] \n",__func__,mac[0]);
				}break;
		}
		
		return -1;
	}
	wifi_printk(WIFI_ALWAYS,"MAC Addr[0]:[%02x:%02x:%02x:%02x:%02x:%02x] , new mac[%02x:%02x:%02x:%02x:%02x:%02x]\n",hw_priv->addresses[0].addr[0],
								hw_priv->addresses[0].addr[1],
								hw_priv->addresses[0].addr[2],
								hw_priv->addresses[0].addr[3],
								hw_priv->addresses[0].addr[4],
								hw_priv->addresses[0].addr[5],
								mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
	atbm_memcpy(hw_priv->addresses[0].addr,mac,ATBM_ETH_ALEN);
	atbm_memcpy(hw_priv->addresses[1].addr,mac,ATBM_ETH_ALEN);
	hw_priv->addresses[1].addr[0] = hw_priv->addresses[0].addr[0] ^ 2;
	priv = hw_priv->vif_list[0];
	atbm_memcpy(priv->mac_addr,hw_priv->addresses[0].addr,ATBM_ETH_ALEN);
	priv = hw_priv->vif_list[1];
	atbm_memcpy(priv->mac_addr,hw_priv->addresses[1].addr,ATBM_ETH_ALEN);
	atbmwifi_setup_mac(hw_priv);
	return 0;

}




/****************************************************************************
* Function:     atbm_wifi_sta_join_ap
*
* Purpose:      This function is used to ask driver to join a network.
*
* Parameters: ssid          SSID of the AP used to join a network
*            authMode   authentication mode used to join a network
*            encryption encryption mode used to join a network
*            key            passphrase used to join a network
*
* Returns:  Returns 0 if succeed, otherwise a negative error code.
******************************************************************************/

atbm_int32 atbm_wifi_sta_join_ap_vif(atbm_uint8 if_id,char *ssid, char *bssid, WLAN_AUTH_MODE authMode, WLAN_ENCRYPTION encryption,  char *key)
{
	ATBM_SECURITY_TYPE key_mgmt;
	atbm_int32 ret;
	switch(authMode){
		case WLAN_WPA_AUTH_DISABLED:
		case WLAN_WPA_AUTH_NONE:
			if(encryption == WLAN_ENCRYPT_WEP){
				key_mgmt = ATBM_KEY_WEP;
			}else if(encryption == WLAN_ENCRYPT_WEP_SHARED){
				key_mgmt = ATBM_KEY_WEP_SHARE;
			}else{
				key_mgmt = ATBM_KEY_NONE;
			}
			break;
		case WLAN_WPA_AUTH_PSK:
			key_mgmt = ATBM_KEY_WPA;
			break;
		case WLAN_WPA2_AUTH_PSK:
			key_mgmt = ATBM_KEY_WPA2;
			break;
		case WLAN_MIX_AUTH_PSK:
			key_mgmt = ATBM_KEY_MIX;
			break;
#if CONFIG_SAE
		case WLAN_WPA_AUTH_SAE:
			key_mgmt = ATBM_KEY_SAE;
			break;
#endif
		default:
			key_mgmt = ATBM_KEY_NONE;
	} 
	ret=wifi_ConnectAP_vif(if_id,(unsigned char *)ssid,strlen(ssid),(unsigned char *)key, key ? strlen(key) : 0, key_mgmt);
	return ret;
}

atbm_int32 atbm_wifi_sta_join_ap(char *ssid, char *bssid, WLAN_AUTH_MODE authMode, WLAN_ENCRYPTION encryption, char *key)
{
	atbm_int32 ret=0;
	/* Station/ P2p-Client Will use if_id 0*/
	ret=atbm_wifi_sta_join_ap_vif(0,/*if_id*/ssid, bssid, authMode, encryption,key);
	return ret;
}

#if FAST_CONNECT_MODE
atbm_int32 atbm_wifi_set_fast_connect_mode(atbm_uint8 enable, atbm_uint8 channel, atbm_uint8 *pmk)
{
	struct atbmwifi_vif *priv=ATBM_NULL;
	priv= _atbmwifi_hwpriv_to_vifpriv(&g_hw_prv,0);

	struct atbmwifi_cfg *config = &priv->config;
	if(enable){
		priv->fast_connect = 1;
		priv->fast_channel = channel;
		if(pmk && pmk[0] != '\0'){
			config->psk_set = 1;
			atbm_memcpy(config->psk, pmk, 32);
		}else{
			config->psk_set = 0;
		}
	}else{
		priv->fast_connect = 0;
		config->psk_set = 0;
	}
	return 0;
}

atbm_int32 atbm_wifi_get_fast_connect_info(atbm_uint8 *channel, atbm_uint8 *pmk)
{
	struct atbmwifi_vif *priv=ATBM_NULL;
	priv= _atbmwifi_hwpriv_to_vifpriv(&g_hw_prv,0);
	struct atbmwifi_cfg *config = &priv->config;
	*channel = config->channel_index;
	if(config->psk_set){
		atbm_memcpy(pmk, config->psk, 32);
	}
	return 0;
}
#endif

#if FAST_CONNECT_NO_SCAN
static	struct atbmwifi_cfg hmac_cfg;
//First connect:config serve as ssid while bss as password
atbm_int32 atbm_wifi_fast_link_noscan(FAST_LINK_INFO * finfo)
{
	struct atbmwifi_vif *priv	= _atbmwifi_hwpriv_to_vifpriv(&g_hw_prv, 0);
	struct atbmwifi_cfg80211_bss *bss;
	struct atbmwifi_cfg *config, *save_config;

	if(priv==ATBM_NULL){
		return 0;
	}
	bss = &priv->bss;
	config = atbmwifi_get_config(priv);
	save_config = (struct atbmwifi_cfg *)finfo->config;

	if(finfo->enable){
		priv->fast_conn_noscan = 1;
		config->ssid_len = save_config->ssid_len;
		config->password_len= save_config->password_len;
		atbm_memcpy(config->ssid, save_config->ssid, save_config->ssid_len);
		atbm_memcpy(priv->ssid, save_config->ssid, save_config->ssid_len);
		atbm_memcpy(config->password, save_config->password, save_config->password_len);
		config->auth_alg = save_config->auth_alg;
		config->privacy = save_config->privacy;
		config->key_mgmt = save_config->key_mgmt;
		config->key_id = save_config->key_id;
		config->psk_set = save_config->psk_set;
		config->channel_index = save_config->channel_index;
		priv->ssid_length = save_config->ssid_len;
#if CONFIG_IEEE80211W
		config->ieee80211w=save_config->ieee80211w;
#endif
		atbm_memcpy(config->psk, save_config->psk, sizeof(config->psk));
		if(bss->information_elements){
			atbm_kfree(bss->information_elements);
		}
		atbm_memcpy(bss, finfo->bss, sizeof(struct atbmwifi_cfg80211_bss));
		if(bss->len_information_elements){
			bss->information_elements = (atbm_uint8 *)atbm_kmalloc(bss->len_information_elements, GFP_KERNEL);
			if(bss->information_elements){
				atbm_memcpy(bss->information_elements, finfo->ie, bss->len_information_elements);
			}
		}
		bss->channel_type = 0;
		bss->rc_priv = ATBM_NULL;
		atbm_memcpy(priv->daddr,bss->bssid,6);
		atbm_memcpy(priv->bssid,bss->bssid,6);
		priv->auth_retry = 1;
		return atbmwifi_wpa_event_queue((atbm_wpa_event_data)priv, (atbm_wpa_event_data)ATBM_NULL,
			 (atbm_wpa_event_data)ATBM_NULL,WPA_EVENT__SUPPLICANT_START_CONNECT,ATBM_WPA_EVENT_NOACK);
	}else{
		priv->fast_conn_noscan = 0;
		atbm_memset(config, 0, sizeof(struct atbmwifi_cfg));
		atbm_memset(bss, 0, sizeof(struct atbmwifi_cfg80211_bss));
		config->psk_set = 0;
		priv->auth_retry = 0;
		return atbm_wifi_sta_join_ap((char *)finfo->config,NULL,0,0,(char *)finfo->bss);
	}
}

atbm_void atbm_wifi_get_linkinfo_noscan(FAST_LINK_INFO * finfo)
{
	struct atbmwifi_vif *priv=ATBM_NULL;
	priv= _atbmwifi_hwpriv_to_vifpriv(&g_hw_prv,0);
	if(priv==ATBM_NULL){
		return;
	}
	struct atbmwifi_cfg80211_bss *bss = &priv->bss;
	struct atbmwifi_cfg *config = atbmwifi_get_config(priv);

	finfo->enable = 1;
	atbm_memcpy(finfo->config, config, sizeof(struct atbmwifi_cfg));
	finfo->ssid_offset = offsetof(struct atbmwifi_cfg, ssid);
	finfo->psk_offset = offsetof(struct atbmwifi_cfg, password);
	if(bss->len_information_elements){
		if(bss->len_information_elements <= sizeof(finfo->ie)){
			atbm_memcpy(finfo->ie, bss->information_elements, bss->len_information_elements);
		}else{
			if((bss->len_information_elements = atbm_wifi_reserve_key_ie(finfo->ie, sizeof(finfo->ie), bss->information_elements, bss->len_information_elements)) <= 0){
				wifi_printk(WIFI_ALWAYS, "error:cannot save ies for fastlink!!\n");				
			}
		}
	}
	atbm_memcpy(finfo->bss, bss, sizeof(struct atbmwifi_cfg80211_bss));
}
#endif

/**
 * @brief 断开与 AP 的连接（用户 API）
 * 
 * 该函数是提供给上层应用使用的 API，用于主动断开与当前 AP 的连接。
 * 函数通过调用 atbm_wifi_off() 来关闭 WiFi 接口，从而触发完整的断网流程。
 * 
 * @return 成功返回 0，失败返回 -1
 * 
 * @note 该函数会获取接口 0 的私有数据结构
 * @note 如果接口未初始化，函数会返回错误
 * @note 调用该函数后，WiFi 接口会被关闭，需要重新初始化才能使用
 * 
 * @see atbm_wifi_off()
 * @see atbmwifi_stop_sta()
 */
atbm_int32 atbm_wifi_sta_disjoin_ap()
{
	struct atbmwifi_vif *priv=ATBM_NULL;
	priv= _atbmwifi_hwpriv_to_vifpriv(&g_hw_prv,0);
	if(priv==ATBM_NULL){
		return 0;
	}
	if(!atbm_wifi_initialed(priv->if_id)){
		wifi_printk(WIFI_DBG_ERROR,"atbm_wifi_initialed err\n");	
		return -1;
	}
	atbm_wifi_off(0);
	return 0;
}

int atbm_wifi_isconnected(atbm_uint8 if_id)
{
	struct atbmwifi_vif *priv=ATBM_NULL;
	priv= _atbmwifi_hwpriv_to_vifpriv(&g_hw_prv,if_id);
	if(priv == ATBM_NULL){
		wifi_printk(WIFI_DBG_ERROR,"not init\n");
		return 0;
	}
	return priv->connect_ok;
}

/****************************************************************************
* Function:   	atbm_wifi_get_driver_version
*
* Purpose:   	This function is used to return the driver's release version
*
* Parameters: None
*
* Returns:	Returns the driver release version
*****************************************************************************
*/
signed char* atbm_wifi_get_driver_version(atbm_void)
{
	struct atbmwifi_vif *priv=ATBM_NULL;
	priv= _atbmwifi_hwpriv_to_vifpriv(&g_hw_prv,0);
	if(priv==ATBM_NULL){
		return 0;
	}
	return (signed char*)0;

}
/****************************************************************************
* Function:     wlan_get_connection_info
*
* Purpose:      This function is used to get the current connection information at STA mode
*
* Parameters: wlan connection information
*
* Returns:  Returns 0 if succeed, otherwise a negative error code.

******************************************************************************/
int atbm_wifi_get_connected_info_vif(atbm_uint8 if_id,ATBM_WLAN_CONNECTION_INFO *wlan_connection_info )
{
	struct atbmwifi_vif *priv=ATBM_NULL;

	if(if_id>=2){
		return -1;
	}

	priv= _atbmwifi_hwpriv_to_vifpriv(&g_hw_prv,if_id);

	if(priv == ATBM_NULL)
		return -1;

	if(wlan_connection_info->Ssid != ATBM_NULL)
	{

		wlan_connection_info->Ssid_len = priv->ssid_length;
		atbm_memcpy(wlan_connection_info->Ssid, priv->ssid, wlan_connection_info->Ssid_len);
		atbm_memcpy(wlan_connection_info->bssid, priv->bssid, 6);
	}
	wlan_connection_info->channel = priv->bss.channel_num;	  
	//RSSI, PHY rate?? mapping?  or current used  rx/tx rate?//from rx_status
	wlan_connection_info->Rssi = priv->bss.rssi;
			
	return 0;
}

int atbm_wifi_get_connected_info(ATBM_WLAN_CONNECTION_INFO *wlan_connection_info )
{
	return atbm_wifi_get_connected_info_vif(0,wlan_connection_info);
}

int atbm_wifi_get_work_channel(void)
{
	
	struct atbmwifi_vif *priv=ATBM_NULL;

	priv= _atbmwifi_hwpriv_to_vifpriv(&g_hw_prv,0);
	if(priv == ATBM_NULL)
		return -1;

	return priv->bss.channel_num;

}
int atbm_wifi_set_ap_whitelist(atbm_uint8 if_id, atbm_uint8 *sta_addr)
{
	struct atbmwifi_vif *priv = ATBM_NULL;
	int i;

	if (if_id >= 2)
	{
		return -1;
	}
	priv = _atbmwifi_hwpriv_to_vifpriv(&g_hw_prv, if_id);

	if (priv == ATBM_NULL)
		return -1;

	for (i = 0; i < ATBMWIFI__MAX_STA_IN_AP_MODE; i++){
		if((priv->whitelist_enable & (1<<i)) && atbm_memcmp(&priv->whitelist[i], sta_addr, 6) == 0){
			return 0;
		}
	}
	for (i = 0; i < ATBMWIFI__MAX_STA_IN_AP_MODE; i++){
		if(!(priv->whitelist_enable & (1<<i))) {
			atbm_memcpy(&priv->whitelist[i], sta_addr, 6);
			wifi_printk(WIFI_DBG_ERROR,"add whitelist %02x%02x%02x%02x%02x%02x\n",sta_addr[0],sta_addr[1],
				sta_addr[2],sta_addr[3],sta_addr[4],sta_addr[5]);
			priv->whitelist_enable |= (1<<i);
			break;
		}
	}

	return 0;
}

int atbm_wifi_clear_ap_whitelist(atbm_uint8 if_id, atbm_uint8 *sta_addr)
{
	struct atbmwifi_vif *priv = ATBM_NULL;
	int i, j;

	if (if_id >= 2)
	{
		return -1;
	}
	priv = _atbmwifi_hwpriv_to_vifpriv(&g_hw_prv, if_id);
	if (priv == ATBM_NULL)
		return -1;

	for (i = 0; i < ATBMWIFI__MAX_STA_IN_AP_MODE; i++){
		if((priv->whitelist_enable & (1<<i)) && atbm_memcmp(&priv->whitelist[i], sta_addr, 6) == 0){
			priv->whitelist_enable &= ~(1<<i);
			atbm_memset(priv->whitelist, 0, 6);
			wifi_printk(WIFI_DBG_ERROR,"delete whitelist %02x%02x%02x%02x%02x%02x\n",sta_addr[0],sta_addr[1],
				sta_addr[2],sta_addr[3],sta_addr[4],sta_addr[5]);
			break;
		}
	}

	return 0;
}

int atbm_wifi_ap_get_sta_addr(atbm_uint8 if_id, atbm_uint8 *mac)
{
	struct atbmwifi_vif *priv=ATBM_NULL;
	int i;

	if(if_id>=2){
		return -1;
	}
	priv= _atbmwifi_hwpriv_to_vifpriv(&g_hw_prv,if_id);
	

	if(priv == ATBM_NULL) {
		return -1;
	}

	if (!atbmwifi_is_ap_mode(priv->iftype)) {
		return -1;
	}

	struct hostapd_data *hapd	 = (struct hostapd_data *)priv->appdata;
	if(!hapd || !hapd->num_sta) {
		return 0;
	}
	for(i = 0; i < hapd->num_sta; i++)
	memcpy(mac + i * 6, hapd->sta_list[i]->addr, 6);
	return hapd->num_sta;
}
int atbm_ap_mode_get_sta_rssi(unsigned char *mac)
{
	int i = 0,ret = 100;
	struct atbmwifi_vif	*priv;
	priv = _atbmwifi_hwpriv_to_vifpriv(&g_hw_prv,1);

	for(i = 0; i < ATBMWIFI__MAX_STA_IN_AP_MODE;i++){
		if(memcmp(priv->link_id_db[i].mac,mac,6) == 0){
			ret = priv->link_id_db[i].sta_priv.rssi;
			break;
		}
	}
	
	return ret;
}


/*************************************************************************************

**************                SW AP functions                                                                        *********

**************************************************************************************/

/****************************************************************************
* Function:     atbm_wifi_ap_create
*
* Purpose:      This function is used to create a SW AP network
*
* Parameters: ssid          SSID of the SW AP to be created
*            authMode   Authentication mode used for the SW AP
*            encryption Encryption mode used for the SW AP
*            key            Passphrase used for the SW AP
*            channel        Channel used for the SW AP
*            ssidBcst       0: to broadcast SSID, 1: to hide SSID
*
* Returns:  Returns 0 if succeed, otherwise a negative error code.
******************************************************************************/
atbm_int32 atbm_wifi_ap_create_vif(atbm_uint8 if_id,char* ssid, int authMode, int encryption, 
	char *key, int channel, ATBM_BOOL ssidBcst )

{
	ATBM_SECURITY_TYPE key_mgmt;
	switch(authMode){
		case WLAN_WPA_AUTH_DISABLED:
		case WLAN_WPA_AUTH_NONE:
			if(encryption == WLAN_ENCRYPT_WEP){
				key_mgmt= ATBM_KEY_WEP; 
			}else if(encryption == WLAN_ENCRYPT_WEP_SHARED){
				key_mgmt= ATBM_KEY_WEP_SHARE; 
			}else{
				key_mgmt = ATBM_KEY_NONE;
			}
			break;
		case WLAN_WPA_AUTH_PSK:
            //peterjiang@20200519, wpa need to be configed MIX, otherwis, some wireless card can not support tkip; 
			//key_mgmt = ATBM_KEY_WPA;
            key_mgmt = ATBM_KEY_MIX;
			break;
		case WLAN_WPA2_AUTH_PSK:
			key_mgmt = ATBM_KEY_WPA2;
			break;
		case WLAN_MIX_AUTH_PSK:
			key_mgmt=ATBM_KEY_MIX;
			break;
		case WLAN_WPA_AUTH_SAE:
			key_mgmt=ATBM_KEY_SAE;
			break;
		case WLAN_WPA_AUTH_PSK_SAE:
			key_mgmt=ATBM_KEY_SAE_COMPIT;
			break;
		default:
			key_mgmt = ATBM_KEY_NONE;
	} 	
	//add Auth mode and channel 
	wifi_StartAP_vif(if_id,(unsigned char *)ssid,strlen(ssid), (unsigned char *)key, strlen(key),channel,key_mgmt,ssidBcst);
	return 0;

}
atbm_int32 atbm_wifi_ap_create(char* ssid, int authMode, int encryption, 
	char *key, int channel, ATBM_BOOL ssidBcst )

{
	return atbm_wifi_ap_create_vif(1,ssid,authMode,encryption,key,channel,ssidBcst);
}


/****************************************************************************
* Function:     wlan_get_assoc_list
*
* Purpose:      This function is used to the associated client list in SW AP mode
*
* Parameters: buf           The buffer to store the associated client list
*                    uiBufSize       size of the buffer
*
* Returns:  Returns 0 if succeed, otherwise a negative error code.

* For ioctls that take a list of MAC addresses *

******************************************************************************/

atbm_int32 atbm_wifi_get_associated_client_list_vif(atbm_uint8 if_id,atbm_void *pchBuf, unsigned int uiBufSize)
{
	WLAN_MACLIST *maclist = (WLAN_MACLIST*)pchBuf;
	struct atbmwifi_vif *priv=ATBM_NULL;

	if(if_id>=2){
		return -1;
	}

	priv= _atbmwifi_hwpriv_to_vifpriv(&g_hw_prv,if_id);

	if(priv == ATBM_NULL)
		return -1;

	atbm_memset(pchBuf, 0, uiBufSize);

	if(maclist->count >  ATBMWIFI__MAX_STA_IN_AP_MODE)
	{
		maclist->count = ATBMWIFI__MAX_STA_IN_AP_MODE;
	}	

	maclist->count = atbmwifi_get_hard_linked_macs(priv,  maclist->ea[0].mac, maclist->count);

	return 0;
}


atbm_int32 atbm_wifi_get_associated_client_list(atbm_uint8 *pchBuf, atbm_uint32 uiBufSize)
{
	return atbm_wifi_get_associated_client_list_vif(1,pchBuf, uiBufSize);
}


atbm_int32 atbmwifi_enable_lmaclog(atbm_uint32 value ){
	
	return atbmwifi_get_lmacLog_to_host(value);
}

atbm_int32 atbm_wifi_get_rssi_avg(atbm_void)
{
	return atbmwifi_get_rssi_avg();
}
atbm_int32 atbm_wifi_set_tx_time(atbm_uint32 time_period, atbm_uint32 time_transmit)
{
	return atbmwifi_set_tx_time(time_period, time_transmit);
}
atbm_int32 atbm_wifi_set_retry(atbm_uint32 retry_num, atbm_uint32 retry_time_ms)
{
	return atbmwifi_set_retry(retry_num, retry_time_ms);
}
atbm_uint32 atbm_hw_priv_chip_id(void)
{
	struct atbmwifi_common *hw_priv = &g_hw_prv;

	return hw_priv->chip_id;
}

atbm_int32 atbm_wifi_set_txpower(atbm_uint32 txpower_idx)
{
	return atbmwifi_set_txpower(txpower_idx);
}
atbm_int32 atbm_wifi_set_txpower_mode(atbm_uint32 txpower_idx)
{
	return atbmwifi_set_rate_txpower_mode(txpower_idx);
}
atbm_int32 atbm_wifi_set_tx_rate(atbm_int32 rate)
{
	return atbmwifi_set_tx_rate(rate);
}
atbm_int32 atbm_wifi_set_sgi(atbm_uint32 sgi)
{
	return atbmwifi_set_sgi(sgi);
}
atbm_int32 atbm_wifi_set_adptive(atbm_uint32 value)
{
	return atbmwifi_dev_set_adaptive(value);
}


int atbm_wifi_get_256BITSEFUSE(atbm_uint8 *data, atbm_uint32 length)
{
	int ret = 0;
	int i;
	struct atbmwifi_common *hw_priv = &g_hw_prv;;

	if((data == NULL) || (length != 32)){
		wifi_printk(WIFI_ERROR, "invalid parameter,please try again!\n");
		return -1;
	}
	
	if ((ret = wsm_get_SIGMSTAR_256BITSEFUSE(hw_priv, data, length)) == 0){
		
		wifi_printk(WIFI_ERROR, "Get efuse data:\n");
		for(i = 0; i < length; i++)
		{
			wifi_printk(WIFI_ALWAYS, "%x ", data[i]);
		}
		wifi_printk(WIFI_ALWAYS, "\n");
		return ret;
	}
	else{
		wifi_printk(WIFI_ERROR, "read efuse failed\n");
		return -1;
	}
}
int atbm_wifi_set_256BITSEFUSE(atbm_uint8 *data, atbm_uint32 length)
{
	atbm_int32 i;
	atbm_int32 ret = 0;
	struct atbmwifi_common *hw_priv = &g_hw_prv;;
	
	if((data == NULL) || (length != 32)){
		wifi_printk(WIFI_ERROR, "invalid parameter,please try again!\n");
		return -1;
	}

	if ((ret = wsm_set_SIGMSTAR_256BITSEFUSE(hw_priv, data, length)) == 0){
		
		wifi_printk(WIFI_ALWAYS, "Set efuse data:\n");
		for(i = 0; i < length; i++)
		{
			wifi_printk(WIFI_ALWAYS, "%x ", data[i]);
		}
		wifi_printk(WIFI_ALWAYS, "\n");
		return ret;
	}
	else{
		wifi_printk(WIFI_ERROR, "write efuse failed\n");
		return -1;
	}

}

extern int wsm_efuse_change_data_cmd(struct atbmwifi_common *hw_priv, const struct efuse_headr *arg, int if_id);
int atbm_save_efuse(struct atbmwifi_common *hw_priv, struct efuse_headr *efuse_save)
{
	int ret = 0;
	int iResult=0;
	//struct atbm_vif *vif;
	struct efuse_headr efuse_bak;

	ret = wsm_efuse_change_data_cmd(hw_priv, efuse_save,0);
	if (ret == LMC_STATUS_CODE__EFUSE_FIRST_WRITE)
	{
		iResult = -3;
	}else if (ret == LMC_STATUS_CODE__EFUSE_PARSE_FAILED)
	{
		iResult = -4;
	}else if (ret == LMC_STATUS_CODE__EFUSE_FULL)
	{
		iResult = -5;
	}else if (ret == LMC_STATUS_CODE__EFUSE_VERSION_CHANGE)
	{
		iResult = -6;
	}else
	{
		iResult = 0;
	}
	
	wsm_get_efuse_data(hw_priv,(void *)&efuse_bak, sizeof(struct efuse_headr));
	
	if(atbm_memcmp((void *)&efuse_bak,(void *)efuse_save, sizeof(struct efuse_headr)) !=0)
	{
		iResult = -2;
	}else
	{
		iResult = 0;
	}

	return iResult;
}

/*
txpower_idx: [-16:16] -> [-8:0.5:8]dB -> [-80:5:80]/10 dB
*/
atbm_int32 atbm_wifi_set_rate_txpower_mode(atbm_int32 txpower_idx)
{
	return atbmwifi_set_rate_txpower_mode(txpower_idx);
}

int atbm_wifi_get_chip_temperature(int *temperature){
	struct Tjroom_temperature_t tjroom;

	if(atbmwifi_get_Tjroom(&tjroom))
		return -1;

	*temperature = (int)tjroom.tempC;

	return 0;
}
int atbmwifi_get_cca_threshold(int *cca)
{
	struct atbmwifi_common *hw_priv = &g_hw_prv;
	int ret = 0;
	ret = wsm_get_cca_threshold(hw_priv,cca,sizeof(int));
	return ret;
}





/*************************************************************************************
fwcmd cmd

**************************************************************************************/

atbm_int32 atbm_wifi_ioctl_fwcmd(int cmd_len,char *cmd)
{
	int ret = 0;
	struct atbmwifi_common *hw_priv = &g_hw_prv;

	ret = wsm_write_mib(hw_priv, WSM_MIB_ID_FW_CMD,
				cmd, cmd_len, 0);

	return ret;

}



/*************************************************************************************

**************                  Manufacturing test functions                                                     *********

**************************************************************************************/

atbm_void  atbm_wifi_mfg_start(atbm_void)
{
	wifi_printk(WIFI_ALWAYS, "atbm_wifi_mfg_start()\n");
	return;
}


/**************************************************************************************
* Function:     atbm_wifi_mfg_set_pktTxBG
*
* Purpose:      This function is used to perform manufacturing 11b/g continuous TX test
*
* Parameters:   channel       Channel used for TX
*               	  rate          11b/g rate used for TX
*               	  powerValue    Output power index, -1 means default power
*
* Returns:      Returns 0 if succeed, otherwise a negative error code.
****************************************************************************************/
atbm_int32 atbm_wifi_mfg_set_pktTxBG(WLAN_CHANNEL channel, WLAN_RATE rate, atbm_int32 powerValue)
{
	atbm_int32 rate_val = 0;

	wifi_printk(WIFI_ALWAYS, "atbm_wifi_mfg_set_pktTxBG()\n");
	
	if(channel < 1 || channel > 14){
		wifi_printk(WIFI_DBG_ERROR, "invalid channel! %d\n", channel);
		return -1;
	}

	switch(rate){
		//11b
		case WLAN_RATE_1M: rate_val = 10;
		break;
		case WLAN_RATE_2M: rate_val = 20;
		break;
		case WLAN_RATE_5M5: rate_val = 55;
		break;
		case WLAN_RATE_11M: rate_val = 110;
		break;
		//11g
		case WLAN_RATE_6M: rate_val = 60;
		break;
		case WLAN_RATE_9M: rate_val = 90;
		break;
		case WLAN_RATE_12M: rate_val = 120;
		break;
		case WLAN_RATE_18M: rate_val = 180;
		break;
		case WLAN_RATE_24M: rate_val = 240;
		break;
		case WLAN_RATE_36M: rate_val = 360;
		break;
		case WLAN_RATE_48M: rate_val = 480;
		break;
		case WLAN_RATE_54M: rate_val = 540;
		break;

		default:
			wifi_printk(WIFI_DBG_ERROR, "invalid rate! %d\n", rate);
			return -1;			
	}
	
	//txpower_idx: [-16:16] -> [-8:0.5:8]dB -> [-80:5:80]/10 dB
	if(powerValue > 16 || powerValue < -16){
		wifi_printk(WIFI_DBG_ERROR, "invalid txpower index! %d\n", powerValue);
		return -1;
	}
	
	if(atbm_wifi_set_rate_txpower_mode(powerValue) < 0){
		wifi_printk(WIFI_DBG_ERROR, "set rate txpower mode failed! %d\n", powerValue);
		return -1;
	}
/*
	if(atbm_etf_start_tx(channel, rate_val, 0, 0) < 0){
		wifi_printk(WIFI_DBG_ERROR, "start tx bg failed!\n");
		return -1;
	}
*/
	return 0;
}


/***********************************************************************************
* Function:     atbm_wifi_mfg_set_PktTxN
*
* Purpose:      This function is used to perform manufacturing 11n continuous TX test
*
* Parameters:   channel       Channel used for TX
*              	   rate          11n rate used for TX
*               	  powerValue    Output power index, -1 means default power
*
* Returns:      Returns 0 if succeed, otherwise a negative error code.
************************************************************************************/
atbm_int32 atbm_wifi_mfg_set_PktTxN(WLAN_CHANNEL channel, WLAN_RATE rate, atbm_int32 powerValue)
{
	atbm_int32 rate_val = 0;

	wifi_printk(WIFI_ALWAYS, "atbm_wifi_mfg_set_PktTxN()\n");
	
	if(channel < 1 || channel > 14){
		wifi_printk(WIFI_DBG_ERROR, "invalid channel! %d\n", channel);
		return -1;
	}
	
	switch(rate){
		case WLAN_MCS_RATE_0: rate_val = 65;
		break;
		case WLAN_MCS_RATE_1: rate_val = 130;
		break;
		case WLAN_MCS_RATE_2: rate_val = 195;
		break;
		case WLAN_MCS_RATE_3: rate_val = 260;
		break;
		case WLAN_MCS_RATE_4: rate_val = 390;
		break;
		case WLAN_MCS_RATE_5: rate_val = 520;
		break;
		case WLAN_MCS_RATE_6: rate_val = 585;
		break;
		case WLAN_MCS_RATE_7: rate_val = 650;
		break;
		
		default:
			wifi_printk(WIFI_DBG_ERROR, "invalid rate! %d\n", rate);
			return -1;			
	}

	//txpower_idx: [-16:16] -> [-8:0.5:8]dB -> [-80:5:80]/10 dB
	if(powerValue > 16 || powerValue < -16){
		wifi_printk(WIFI_DBG_ERROR, "invalid txpower index! %d\n", powerValue);
		return -1;
	}

	if(atbm_wifi_set_rate_txpower_mode(powerValue) < 0){
		wifi_printk(WIFI_DBG_ERROR, "set rate txpower mode failed! %d\n", powerValue);
		return -1;
	}
/*
	if(atbm_etf_start_tx(channel, rate_val, 1, 0) < 0){
		wifi_printk(WIFI_DBG_ERROR, "start tx bg failed!\n");
		return -1;
	}
*/
	return 0;
}

atbm_int32 atbm_wifi_mfg_PT_Test(atbm_uint8 if_id,/*atbm_int32 targetFreq, atbm_int32 rssiFilter, atbm_int32 evmFilter, atbm_int32 cableLoss, */atbm_int32 isWriteEfuse)
{

	atbm_etf_PT_Test_start(if_id,/*targetFreq, rssiFilter, evmFilter, cableLoss, */isWriteEfuse);
	
	
	return 0;
}


/****************************************************************************
* Function:     atbm_wifi_mfg_CarrierTone
*
* Purpose:      This function is used to perform manufacturing non-modulation TX test
*
* Parameters:   channel       Channel used for test
*
* Returns:      Returns 0 if succeed, otherwise a negative error code.
******************************************************************************/
atbm_int32 atbm_wifi_mfg_CarrierTone(WLAN_CHANNEL channel)
{
	atbm_int32 rate_val = 65;

	wifi_printk(WIFI_ALWAYS, "atbm_wifi_mfg_CarrierTone()\n");
	
	if(channel < 1 || channel > 14){
		wifi_printk(WIFI_DBG_ERROR, "invalid channel! %d\n", channel);
		return -1;
	}

	if(atbm_etf_start_tx_single_tone(channel) < 0){
		wifi_printk(WIFI_DBG_ERROR, "start tx CarrierTone failed!\n");
		return -1;
	}

	return 0;
}

int atbm_wifi_etf_start_tx(int channel,int mode, int rateIdx,int  bw, int chOff, int ldpc, int packetLen,int precom)
{
	return atbm_etf_start_tx(channel,mode,  rateIdx,  bw,  chOff,  ldpc,  packetLen, precom);
}
int atbm_wifi_etf_stop_tx()
{
	return atbm_etf_stop_tx();
}
int atbm_wifi_etf_start_rx(int channel ,int bw, int chOff, int mode)
{
	return atbm_etf_start_rx(channel , bw,  chOff, mode);
}
int atbm_wifi_etf_stop_rx(get_result_rx_data *rx_data)
{
	return atbm_etf_stop_rx(rx_data);
}




/****************************************************************************
* Function:     atbm_wifi_mfg_set_PktRxMode
*
* Purpose:      This function  is used to perform manufacturing RX test
*
* Parameters:   channel     Channel used for RX
*
* Returns:      Returns 0 if succeed, otherwise a negative error code.
******************************************************************************/
atbm_int32 atbm_wifi_mfg_set_PktRxMode(WLAN_CHANNEL channel)
{
	wifi_printk(WIFI_ALWAYS, "atbm_wifi_mfg_set_PktRxMode()\n");

	if(channel < 1 || channel > 14){
		wifi_printk(WIFI_DBG_ERROR, "invalid channel! %d\n", channel);
		return -1;
	}

	if(atbm_etf_start_rx(channel, 0, 0, 0) < 0){
		wifi_printk(WIFI_DBG_ERROR, "start etf rx failed!\n");
		return -1;
	}
	
	return 0;
}


/****************************************************************************
* Function:     atbm_wifi_mfg_get_RxPkt
*
* Purpose:      This function is used to get received packet count
*
* Parameters:   uiCount     Received packet count
*
* Returns:      Returns 0 if succeed, otherwise a negative error code.
******************************************************************************/
atbm_int32 atbm_wifi_mfg_get_RxPkt(atbm_uint32* uiCount)
{
	wifi_printk(WIFI_ALWAYS, "atbm_wifi_mfg_get_RxPkt()\n");
	return 0;
}



/****************************************************************************
* Function:     atbm_wifi_mfg_stop
*
* Purpose:      This function is used to stop manufacturing test
*
* Parameters:   None
*
* Returns:      Returns 0 if succeed, otherwise a negative error code.
******************************************************************************/
atbm_int32 atbm_wifi_mfg_stop(atbm_void)
{
	wifi_printk(WIFI_ALWAYS, "atbm_wifi_mfg_stop()\n");

	atbm_etf_stop_rx(NULL);
	atbm_etf_stop_tx();
	return 0;
}
#if ATBM_SUPPORT_SMARTCONFIG
extern atbm_void atbm_ht_smt_setting(atbm_void);
atbm_uint8 atbm_smartconfig_start(void)
{
	/*Station if_id ==0*/
	int if_id=0;
	struct smartconfig_config st_cfg = 
	{
		.type = CONFIG_TP_ATBM_SMART,
		.magic_cnt = 1,
		.magic_time = 70,
		.payload_time = 12000,
	};

	atbm_ht_smt_setting();
    smartconfig_start(&st_cfg,if_id);

	return 0;
}

atbm_uint8 atbm_smartconfig_stop(void)
{
	/*Station if_id ==0*/
	int if_id=0;

    smartconfig_stop(if_id);

	return 0;
}
#endif
#if CONFIG_WPS
/****************************************************************************
* Function:     atbmwps_start_pbc
*
* Purpose:      This function is used to start wps->pbc mode
*
* Parameters:   None
*
* Returns:      Returns 0 if succeed, otherwise a negative error code.
******************************************************************************/
atbm_uint8 atbm_wpspbc_start(ATBM_WIFI_MODE AP_sta_mode)
{
	int ret;
	int if_id;
	struct atbmwifi_vif *priv=ATBM_NULL;
	switch (AP_sta_mode){
			case ATBM_NL80211_IFTYPE_STATION:
			case ATBM_NL80211_IFTYPE_P2P_CLIENT:
				if_id=0;
				break;
			case ATBM_NL80211_IFTYPE_AP:
			case ATBM_NL80211_IFTYPE_P2P_GO:
				if_id=1;
				break;
			default:
				wifi_printk(WIFI_ALWAYS,"Mode cant support,Pls check it !!!\n");
				return -1;
	}	
	atbm_wifi_on(AP_sta_mode);
	priv= _atbmwifi_hwpriv_to_vifpriv(&g_hw_prv,if_id);
	ret=atbmwps_start_pbc(priv,ATBM_NULL);
	return ret;
}

/****************************************************************************
* Function: 	atbmwps_start_pin
*
* Purpose:		 This function is used to start wps->pin mode
*
* Parameters:	None
*
* Returns:		Returns 0 if succeed, otherwise a negative error code.
******************************************************************************/
atbm_uint8 atbm_wpspin_start(ATBM_WIFI_MODE AP_sta_mode,const char *pin)
{
	int ret;
	int if_id;
	struct atbmwifi_vif *priv=ATBM_NULL;
	switch (AP_sta_mode){
			case ATBM_NL80211_IFTYPE_STATION:
			case ATBM_NL80211_IFTYPE_P2P_CLIENT:
				if_id=0;
				break;
			case ATBM_NL80211_IFTYPE_AP:
			case ATBM_NL80211_IFTYPE_P2P_GO:
				if_id=1;
				break;
			default:
				wifi_printk(WIFI_ALWAYS,"Mode cant support,Pls check it !!!\n");
				return -1;
	}	
	atbm_wifi_on(AP_sta_mode);
	priv = _atbmwifi_hwpriv_to_vifpriv(&g_hw_prv,if_id);
	ret = atbmwps_start_pin(priv, pin, NULL, 0);
	return ret;
}


/****************************************************************************
* Function: 	atbmwps_cancel
*
* Purpose:		This function is used to cancel wps func
*
* Parameters:	None
*
* Returns:		Returns 0 if succeed, otherwise a negative error code.
******************************************************************************/
atbm_uint8 atbm_wpsmode_cancel(ATBM_WIFI_MODE AP_sta_mode)
{
	int ret;
	int if_id;
	struct atbmwifi_vif *priv=ATBM_NULL;
	switch (AP_sta_mode){
			case ATBM_NL80211_IFTYPE_STATION:
			case ATBM_NL80211_IFTYPE_P2P_CLIENT:
				if_id=0;
				break;
			case ATBM_NL80211_IFTYPE_AP:
			case ATBM_NL80211_IFTYPE_P2P_GO:
				if_id=1;
				break;
			default:
				wifi_printk(WIFI_ALWAYS,"Mode cant support,Pls check it !!!\n");
				return -1;
	}	
	priv = _atbmwifi_hwpriv_to_vifpriv(&g_hw_prv,if_id);
	ret = atbmwps_cancel(priv);
	atbm_wifi_off(AP_sta_mode);
	return ret;
}
#endif

#if BENTU_OS
/****************************************************************************
* Function:   	atbm_wifi_get_if_status
*
* Purpose:   	This function is used to get interface setup status                   
*
* Parameters: if_id     0: Sta interface, 1 AP interface
*
* Returns:  1:interface on, 0:interface off.
****************************************************************************/
ATBM_WIFI_IFACE_STATE atbm_wifi_get_if_status(ATBM_WIFI_MODE mode)
{
    int if_id = -1;
    struct atbmwifi_vif *priv;
    
    if((mode == ATBM_WIFI_STA_MODE) || (mode == ATBM_WIFI_P2P_CLIENT)){
        if_id = 0;
    }else if((mode == ATBM_WIFI_AP_MODE) || (mode == ATBM_WIFI_P2P_GO)){
        if_id = 1;
    }else{
        wifi_printk(WIFI_ALWAYS,"%s not support mode %d\n", __func__, mode);
        return -1;//ATBM_WIFI_IFACE_ERR;
    }
    
    priv = atbm_wifi_vif_get(if_id);
    if(priv == ATBM_NULL){
        wifi_printk(WIFI_ALWAYS,"%s iface(%d) is not init\n", __func__, if_id);
        return -1;//ATBM_WIFI_IFACE_ERR;
    }

    if(priv->iftype == ATBM_NUM_NL80211_IFTYPES){
        return 0;//ATBM_WIFI_IFACE_OFF;
    }else{
        return 1;//ATBM_WIFI_IFACE_ON;
    }
}
#endif

#if CONFIG_P2P
/*  if all interface is occupied, then stop AP mode first */
struct atbmwifi_vif * p2p_get_available_ifpriv(){
	return _atbmwifi_hwpriv_to_vifpriv(&g_hw_prv,0);
}

/****************************************************************************
* Function: 	atbm_wifi_p2p_start
*
* Purpose:		This function is used to create p2p main task
*
* Parameters:	None
*
* Returns: None
******************************************************************************/
int atbm_wifi_p2p_start()
{
	int ret = 0;
	struct atbmwifi_vif *priv = p2p_get_available_ifpriv();
	g_hw_prv.p2p_if_id = priv->if_id;

	if(priv == ATBM_NULL){
		wifi_printk(WIFI_ALWAYS, "p2p:error, priv is NULL\n");
		ret = -1;
		goto _error;
	}
	atbm_p2p_start(priv);
_error:
	return ret;
}

/****************************************************************************
* Function: 	atbm_wifi_p2p_find
*
* Purpose:		This function is used to create p2p main task
*
* Parameters:	None
*
* Returns: None
******************************************************************************/
int atbm_wifi_p2p_find(int timeout)
{
	int ret = 0;
	struct atbmwifi_vif *priv = _atbmwifi_hwpriv_to_vifpriv(&g_hw_prv, g_hw_prv.p2p_if_id);

	if(priv == ATBM_NULL){
		wifi_printk(WIFI_ALWAYS, "p2p:error, priv is NULL\n");
		ret = -1;
		goto _error;
	}

	ret = atbm_p2p_find_only(priv, timeout);
_error:
	return ret;
}

/****************************************************************************
* Function: 	atbm_wifi_p2p_find
*
* Purpose:		This function is used to create p2p main task
*
* Parameters:	None
*
* Returns: None
******************************************************************************/
int atbm_wifi_p2p_find_accept(int go_intent)
{
	int ret = 0;
	struct atbmwifi_vif *priv = _atbmwifi_hwpriv_to_vifpriv(&g_hw_prv, g_hw_prv.p2p_if_id);

	if(priv == ATBM_NULL){
		wifi_printk(WIFI_ALWAYS, "p2p:error, priv is NULL\n");
		ret = -1;
		goto _error;
	}

	ret = atbm_p2p_find_wait_connect(priv, go_intent);
_error:
	return ret;
}

/****************************************************************************
* Function: 	atbm_wifi_p2p_find_stop
*
* Purpose:		This function is used to create p2p main task
*
* Parameters:	None
*
* Returns: None
******************************************************************************/
int atbm_wifi_p2p_find_stop(int timeout)
{
	int ret = 0;
	struct atbmwifi_vif *priv = _atbmwifi_hwpriv_to_vifpriv(&g_hw_prv, g_hw_prv.p2p_if_id);

	if(priv == ATBM_NULL){
		wifi_printk(WIFI_ALWAYS, "p2p:error, priv is NULL\n");
		ret = -1;
		goto _error;
	}

	atbm_p2p_stop_find(priv);
_error:
	return ret;
}

/****************************************************************************
* Function: 	atbm_wifi_p2p_find_stop
*
* Purpose:		This function is used to create p2p main task
*
* Parameters:	None
*
* Returns: None
******************************************************************************/
int atbm_wifi_p2p_listen_accept(int go_intent, int channel)
{
	int ret = 0;
	struct atbmwifi_vif *priv = _atbmwifi_hwpriv_to_vifpriv(&g_hw_prv, g_hw_prv.p2p_if_id);

	if(priv == ATBM_NULL){
		wifi_printk(WIFI_ALWAYS, "p2p:error, priv is NULL\n");
		ret = -1;
		goto _error;
	}

	atbm_p2p_listen_wait_connect(priv, go_intent, channel);
_error:
	return ret;
}


/****************************************************************************
* Function: 	atbm_wifi_p2p_show_peers
*
* Purpose:		This function is used to create p2p main task
*
* Parameters:	None
*
* Returns: None
******************************************************************************/
int atbm_wifi_p2p_show_peers()
{
	int ret = 0;
	struct atbmwifi_vif *priv = _atbmwifi_hwpriv_to_vifpriv(&g_hw_prv, g_hw_prv.p2p_if_id);

	if(priv == ATBM_NULL){
		wifi_printk(WIFI_ALWAYS, "p2p:error, priv is NULL\n");
		ret = -1;
		goto _error;
	}

	atbm_p2p_get_peers(priv);
_error:
	return ret;
}

/****************************************************************************
* Function: 	atbm_wifi_p2p_go_start
*
* Purpose:		This function is used to create p2p main task
*
* Parameters:	None
*
* Returns: None
******************************************************************************/
int atbm_wifi_p2p_go_start()
{
	int ret = 0;
	struct atbmwifi_vif *priv = _atbmwifi_hwpriv_to_vifpriv(&g_hw_prv, g_hw_prv.p2p_if_id);

	if(priv == ATBM_NULL){
		wifi_printk(WIFI_ALWAYS, "p2p:error, priv is NULL\n");
		ret = -1;
		goto _error;
	}

	atbm_p2p_go_start(priv);
_error:
	return ret;
}

/****************************************************************************
* Function: 	atbm_wifi_p2p_go_start
*
* Purpose:		This function is used to create p2p main task
*
* Parameters:	None
*
* Returns: None
******************************************************************************/
int atbm_wifi_p2p_go_stop()
{
	int ret = 0;
	struct atbmwifi_vif *priv = _atbmwifi_hwpriv_to_vifpriv(&g_hw_prv, g_hw_prv.p2p_if_id);

	if(priv == ATBM_NULL){
		wifi_printk(WIFI_ALWAYS, "p2p:error, priv is NULL\n");
		ret = -1;
		goto _error;
	}

	atbm_p2p_go_stop(priv);
_error:
	return ret;
}

/****************************************************************************
* Function: 	atbm_wifi_p2p_connect
*
* Purpose:		This function is used to create p2p main task
*
* Parameters:	None
*
* Returns: None
******************************************************************************/
int atbm_wifi_p2p_connect(atbm_uint8 *mac, int go_intent)
{
	int ret = 0;
	struct atbmwifi_vif *priv = _atbmwifi_hwpriv_to_vifpriv(&g_hw_prv, g_hw_prv.p2p_if_id);

	if(priv == ATBM_NULL){
		wifi_printk(WIFI_ALWAYS, "p2p:error, priv is NULL\n");
		ret = -1;
		goto _error;
	}

	atbm_p2p_stop_find(priv);
	atbm_SleepMs(200);
	ret = atbm_p2p_auto_connect(priv, mac, go_intent);
_error:
	return ret;
}

/****************************************************************************
* Function: 	atbm_wifi_p2p_stop
*
* Purpose:		This function is used to create p2p main task
*
* Parameters:	None
*
* Returns: None
******************************************************************************/
int atbm_wifi_p2p_stop()
{
	int ret = 0;
	struct atbmwifi_vif *priv = _atbmwifi_hwpriv_to_vifpriv(&g_hw_prv, g_hw_prv.p2p_if_id);

	if(priv == ATBM_NULL){
		wifi_printk(WIFI_ALWAYS, "p2p:error, priv is NULL\n");
		ret = -1;
		goto _error;
	}

	ret = atbm_p2p_deinit(priv);
_error:
	return ret;
}
#endif

atbm_int8 atbm_wifi_get_bssid(atbm_uint8 *bssid, atbm_int32 if_id)
{
    struct atbmwifi_vif *priv;

    priv = _atbmwifi_hwpriv_to_vifpriv(&g_hw_prv,if_id);
    if(priv == ATBM_NULL){
        wifi_printk(WIFI_ALWAYS,"%s priv is null\n", __func__);
        return -1;
    }

    //get station connection status
    if((bssid==ATBM_NULL) || (atbm_wifi_isconnected(if_id) == 0)){
        return -1;
    }

    atbm_memcpy(bssid, priv->bssid, ATBM_ETH_ALEN);
    
    return 0;    
}


char MCS_LUT_20UL_OFDM_5G[OFDM_40M_INDEX_MAX] = {	
0x50, //6M/MCS0 /MCS32
0x4C, //9M/
0x4C, //12M/MCS1/
0x48, //18M/MCS2/
0x48, //24M/MCS3/
0x44, //36M/MCS4/
0x44, //48M/MCS5/
0x42, //54M/MCS6/
0x40,  //MCS7/
0x50,//VHT MCS0
0x4C,//VHT MCS1
0x48,//VHT MCS2
0x48,//VHT MCS3
0x44,//VHT MCS4
0x44,//VHT MCS5
0x42,//VHT MCS6
0x40,//VHT MCS7
0x40,//VHT MCS8
0x3C,//VHT MCS9
0x50,//SU/ER_SU242 MCS0
0x4C,//SU/ER_SU242 MCS1
0x48,//SU/ER_SU242 MCS2
0x48,//SU MCS3
0x44,//SU MCS4
0x44,//SU MCS5
0x42,//SU MCS6
0x40,//SU MCS7
0x40,//SU MCS8
0x3C,//SU MCS9
0x38,//SU MCS10
0x34,//SU MCS11
};

char MCS_LUT_40M_OFDM_5G[OFDM_40M_INDEX_MAX] = {	
0x4C, //6M/MCS0 /MCS32
0x44, //9M/
0x44, //12M/MCS1/
0x40, //18M/MCS2/
0x40, //24M/MCS3/
0x3C, //36M/MCS4/
0x3C, //48M/MCS5/
0x38, //54M/MCS6/
0x36,  //MCS7/
0x4C,//VHT MCS0
0x44,//VHT MCS1
0x40,//VHT MCS2
0x40,//VHT MCS3
0x3C,//VHT MCS4
0x3C,//VHT MCS5
0x38,//VHT MCS6
0x36,//VHT MCS7
0x2C,//VHT MCS8
0x2C,//VHT MCS9
0x4C,//SU/ER_SU242 MCS0
0x44,//SU/ER_SU242 MCS1
0x40,//SU/ER_SU242 MCS2
0x40,//SU MCS3
0x3C,//SU MCS4
0x3C,//SU MCS5
0x38,//SU MCS6
0x36,//SU MCS7
0x2C,//SU MCS8
0x2C,//SU MCS9
0x20,//SU MCS10
0x20,//SU MCS11
};

char MCS_LUT_20M_Modify[OFDM_20M_INDEX_MAX] = {	
0x54,//1M/2M/
0x54,//5.5M/11M/
0x54, //6M/MCS0 /MCS32
0x50, //9M/
0x4C, //12M/MCS1/
0x4C, //18M/MCS2/
0x4C, //24M/MCS3/
0x4C, //36M/MCS4/
0x4C, //48M/MCS5/
0x48, //54M/MCS6/
0x48,  //MCS7/
0x54,//VHT MCS0
0x4C,//VHT MCS1
0x4C,//VHT MCS2
0x4C,//VHT MCS3
0x4C,//VHT MCS4
0x4C,//VHT MCS5
0x48,//VHT MCS6
0x48,//VHT MCS7
0x44,//VHT MCS8
0x44,//VHT MCS9
0x54,//SU/ER_SU242 MCS0
0x4C,//SU/ER_SU242 MCS1
0x4C,//SU/ER_SU242 MCS2
0x4C,//SU MCS3
0x4C,//SU MCS4
0x4C,//SU MCS5
0x48,//SU MCS6
0x48,//SU MCS7
0x44,//SU MCS8
0x44,//SU MCS9
0x40,//SU MCS10
0x40,//SU MCS11

						};

char MCS_LUT_20M_delta_mode[OFDM_20M_INDEX_MAX] = {
0x0,//1M/2M			
0x0,//5.5M/11M	
0x0,//6M	MCS0/MCS32	
0x0,//9M	?a?a			
0x0,//12M	MCS1		
0x0,//18M	MCS2		
0x0,//24M	MCS3		
0x0,//36M	MCS4		
0x0,//48M	MCS5		
0x0,//54M	MCS6		
0x0,//MCS7			
0x0,//VHT_MCS0		
0x0,//VHT_MCS1		
0x0,//VHT_MCS2		
0x0,//VHT_MCS3		
0x0,//VHT_MCS4		
0x0,//VHT_MCS5		
0x0,//VHT_MCS6		
0x0,//VHT_MCS7		
0x0,//VHT_MCS8		
0x0,//VHT_MCS9		
0x0,//HE-SU_MCS0	MCS0
0x0,//HE-SU_MCS1	MCS1
0x0,//HE-SU_MCS2	MCS2
0x0,//HE-SU_MCS3	
0x0,//HE-SU_MCS4	
0x0,//HE-SU_MCS5	
0x0,//HE-SU_MCS6	
0x0,//HE-SU_MCS7	
0x0,//HE-SU_MCS8	
0x0,//HE-SU_MCS9	
0x0,//HE-SU_MCS10	
0x0,//HE-SU_MCS11
};
						
char MCS_LUT_20M_delta_bw[OFDM_20M_INDEX_MAX] = {	
0x0,//1M/2M			
0x0,//5.5M/11M	
0x0,//6M	MCS0/MCS32	
0x0,//9M	?a?a			
0x0,//12M	MCS1		
0x0,//18M	MCS2		
0x0,//24M	MCS3		
0x0,//36M	MCS4		
0x0,//48M	MCS5		
0x0,//54M	MCS6		
0x0,//MCS7			
0x0,//VHT_MCS0		
0x0,//VHT_MCS1		
0x0,//VHT_MCS2		
0x0,//VHT_MCS3		
0x0,//VHT_MCS4		
0x0,//VHT_MCS5		
0x0,//VHT_MCS6		
0x0,//VHT_MCS7		
0x0,//VHT_MCS8		
0x0,//VHT_MCS9		
0x0,//HE-SU_MCS0	MCS0
0x0,//HE-SU_MCS1	MCS1
0x0,//HE-SU_MCS2	MCS2
0x0,//HE-SU_MCS3	
0x0,//HE-SU_MCS4	
0x0,//HE-SU_MCS5	
0x0,//HE-SU_MCS6	
0x0,//HE-SU_MCS7	
0x0,//HE-SU_MCS8	
0x0,//HE-SU_MCS9	
0x0,//HE-SU_MCS10	
0x0,//HE-SU_MCS11
};

char MCS_LUT_40M_Modify[OFDM_40M_INDEX_MAX] = {	

0x54, //6M/MCS0 /MCS32
0x50, //9M/
0x4C, //12M/MCS1/
0x4C, //18M/MCS2/
0x4C, //24M/MCS3/
0x4C, //36M/MCS4/
0x4C, //48M/MCS5/
0x48, //54M/MCS6/
0x48,  //MCS7/
0x54,//VHT MCS0
0x4C,//VHT MCS1
0x4C,//VHT MCS2
0x4C,//VHT MCS3
0x4C,//VHT MCS4
0x4C,//VHT MCS5
0x48,//VHT MCS6
0x48,//VHT MCS7
0x44,//VHT MCS8
0x44,//VHT MCS9
0x54,//SU/ER_SU242 MCS0
0x4C,//SU/ER_SU242 MCS1
0x4C,//SU/ER_SU242 MCS2
0x4C,//SU MCS3
0x4C,//SU MCS4
0x4C,//SU MCS5
0x48,//SU MCS6
0x48,//SU MCS7
0x44,//SU MCS8
0x44,//SU MCS9
0x40,//SU MCS10
0x40,//SU MCS11

						};

char MCS_LUT_40M_delta_mode[OFDM_40M_INDEX_MAX] = {	
0x0,//6M	MCS0/MCS32	
0x0,//9M	?a?a			
0x0,//12M	MCS1		
0x0,//18M	MCS2		
0x0,//24M	MCS3		
0x0,//36M	MCS4		
0x0,//48M	MCS5		
0x0,//54M	MCS6		
0x0,//MCS7			
0x0,//VHT_MCS0		
0x0,//VHT_MCS1		
0x0,//VHT_MCS2		
0x0,//VHT_MCS3		
0x0,//VHT_MCS4		
0x0,//VHT_MCS5		
0x0,//VHT_MCS6		
0x0,//VHT_MCS7		
0x0,//VHT_MCS8		
0x0,//VHT_MCS9		
0x0,//HE-SU_MCS0	MCS0
0x0,//HE-SU_MCS1	MCS1
0x0,//HE-SU_MCS2	MCS2
0x0,//HE-SU_MCS3	
0x0,//HE-SU_MCS4	
0x0,//HE-SU_MCS5	
0x0,//HE-SU_MCS6	
0x0,//HE-SU_MCS7	
0x0,//HE-SU_MCS8	
0x0,//HE-SU_MCS9	
0x0,//HE-SU_MCS10	
0x0,//HE-SU_MCS11
};
						
char MCS_LUT_40M_delta_bw[OFDM_40M_INDEX_MAX] = {	
0x0,//6M	MCS0/MCS32	
0x0,//9M	?a?a			
0x0,//12M	MCS1		
0x0,//18M	MCS2		
0x0,//24M	MCS3		
0x0,//36M	MCS4		
0x0,//48M	MCS5		
0x0,//54M	MCS6		
0x0,//MCS7			
0x0,//VHT_MCS0		
0x0,//VHT_MCS1		
0x0,//VHT_MCS2		
0x0,//VHT_MCS3		
0x0,//VHT_MCS4		
0x0,//VHT_MCS5		
0x0,//VHT_MCS6		
0x0,//VHT_MCS7		
0x0,//VHT_MCS8		
0x0,//VHT_MCS9		
0x0,//HE-SU_MCS0	MCS0
0x0,//HE-SU_MCS1	MCS1
0x0,//HE-SU_MCS2	MCS2
0x0,//HE-SU_MCS3	
0x0,//HE-SU_MCS4	
0x0,//HE-SU_MCS5	
0x0,//HE-SU_MCS6	
0x0,//HE-SU_MCS7	
0x0,//HE-SU_MCS8	
0x0,//HE-SU_MCS9	
0x0,//HE-SU_MCS10	
0x0,//HE-SU_MCS11
};
 char MCS_LUT_20M_Modify_Cronus[CRONUS_OFDM_INDEX_MAX] = {	
 
							0x48,//1M/2M/
							0x48,//5.5M/11M/
							0x44, //6M/MCS0 /
							0x44, //9M/
							0x40, //12M/MCS1/
							0x40, //18M/MCS2/
							0x3C, //24M/MCS3/
							0x3C, //36M/MCS4/
							0x3C, //48M/MCS5/
							0x3C, //54M/MCS6/
							0x38,  //MCS7/
							0x44,//SU/ER_SU242 MCS0
							0x44,//SU/ER_SU242 MCS1
							0x40,//SU/ER_SU242 MCS2
							0x40,//SU MCS3
							0x3C,//SU MCS4
							0x3C,//SU MCS5
							0x3C,//SU MCS6
							0x38,//SU MCS7
							0x38,//SU MCS8
							0x38,//SU MCS9
							0x34,//SU MCS10
							0x34,//SU MCS11
						};

char MCS_LUT_20M_delta_mode_Cronus[CRONUS_OFDM_INDEX_MAX] = {	
							0x0,//1M/2M/
							0x0,//5.5M/11M/
							0x0, //6M/MCS0 /
							0x0, //9M/
							0x0, //12M/MCS1/
							0x0, //18M/MCS2/
							0x0, //24M/MCS3/
							0x0, //36M/MCS4/
							0x0, //48M/MCS5/
							0x0, //54M/MCS6/
							0x0,  //MCS7/
							0x0,//SU/ER_SU242 MCS0
							0x0,//SU/ER_SU242 MCS1
							0x0,//SU/ER_SU242 MCS2
							0x0,//SU MCS3
							0x0,//SU MCS4
							0x0,//SU MCS5
							0x0,//SU MCS6
							0x0,//SU MCS7
							0x0,//SU MCS8
							0x0,//SU MCS9
							0x0,//SU MCS10
							0x0,//SU MCS11
						};
						
char MCS_LUT_20M_delta_bw_Cronus[CRONUS_OFDM_INDEX_MAX] = {	
							0x0,//1M/2M/
							0x0,//5.5M/11M/
							0x0, //6M/MCS0 /
							0x0, //9M/
							0x0, //12M/MCS1/
							0x0, //18M/MCS2/
							0x0, //24M/MCS3/
							0x0, //36M/MCS4/
							0x0, //48M/MCS5/
							0x0, //54M/MCS6/
							0x0,  //MCS7/
							0x0,//SU/ER_SU242 MCS0
							0x0,//SU/ER_SU242 MCS1
							0x0,//SU/ER_SU242 MCS2
							0x0,//SU MCS3
							0x0,//SU MCS4
							0x0,//SU MCS5
							0x0,//SU MCS6
							0x0,//SU MCS7
							0x0,//SU MCS8
							0x0,//SU MCS9
							0x0,//SU MCS10
							0x0,//SU MCS11
						};


const char MCS_LUT_40M_Cronus[CRONUS_OFDM_INDEX_MAX] = {	

							0x48, //1M/2M/
							0x48, //5.5M/11M/
							0x44, //6M/MCS0 /
							0x44, //9M/
							0x40, //12M/MCS1/
							0x40, //18M/MCS2/
							0x3C, //24M/MCS3/
							0x3C, //36M/MCS4/
							0x3C, //48M/MCS5/
							0x3C, //54M/MCS6/
							0x38,  //MCS7/
							0x44,//SU MCS0
							0x44,//SU MCS1
							0x40,//SU MCS2
							0x40,//SU MCS3
							0x3C,//SU MCS4
							0x3C,//SU MCS5
							0x3C,//SU MCS6
							0x38,//SU MCS7
							0x38,//SU MCS8
							0x38,//SU MCS9
							0x34,//SU MCS10
							0x34,//SU MCS11

						};	
char MCS_LUT_40M_Modify_Cronus[CRONUS_OFDM_INDEX_MAX] = {	

							0x48,//1M/2M/
							0x48,//5.5M/11M/
							0x44, //6M/MCS0 /
							0x44, //9M/
							0x40, //12M/MCS1/
							0x40, //18M/MCS2/
							0x3C, //24M/MCS3/
							0x3C, //36M/MCS4/
							0x3C, //48M/MCS5/
							0x3C, //54M/MCS6/
							0x38,  //MCS7/
							0x44,//SU/ER_SU242 MCS0
							0x44,//SU/ER_SU242 MCS1
							0x40,//SU/ER_SU242 MCS2
							0x40,//SU MCS3
							0x3C,//SU MCS4
							0x3C,//SU MCS5
							0x3C,//SU MCS6
							0x38,//SU MCS7
							0x38,//SU MCS8
							0x38,//SU MCS9
							0x34,//SU MCS10
							0x34,//SU MCS11

						};

char MCS_LUT_40M_delta_mode_Cronus[CRONUS_OFDM_INDEX_MAX] = {	
							0x0,//1M/2M/
							0x0,//5.5M/11M/
							0x0, //6M/MCS0 /
							0x0, //9M/
							0x0, //12M/MCS1/
							0x0, //18M/MCS2/
							0x0, //24M/MCS3/
							0x0, //36M/MCS4/
							0x0, //48M/MCS5/
							0x0, //54M/MCS6/
							0x0,  //MCS7/
							0x0,//SU/ER_SU242 MCS0
							0x0,//SU/ER_SU242 MCS1
							0x0,//SU/ER_SU242 MCS2
							0x0,//SU MCS3
							0x0,//SU MCS4
							0x0,//SU MCS5
							0x0,//SU MCS6
							0x0,//SU MCS7
							0x0,//SU MCS8
							0x0,//SU MCS9
							0x0,//SU MCS10
							0x0,//SU MCS11
						};
						
char MCS_LUT_40M_delta_bw_Cronus[CRONUS_OFDM_INDEX_MAX] = {	
							0x0,//1M/2M/
							0x0,//5.5M/11M/
							0x0, //6M/MCS0 /
							0x0, //9M/
							0x0, //12M/MCS1/
							0x0, //18M/MCS2/
							0x0, //24M/MCS3/
							0x0, //36M/MCS4/
							0x0, //48M/MCS5/
							0x0, //54M/MCS6/
							0x0,  //MCS7/
							0x0,//SU/ER_SU242 MCS0
							0x0,//SU/ER_SU242 MCS1
							0x0,//SU/ER_SU242 MCS2
							0x0,//SU MCS3
							0x0,//SU MCS4
							0x0,//SU MCS5
							0x0,//SU MCS6
							0x0,//SU MCS7
							0x0,//SU MCS8
							0x0,//SU MCS9
							0x0,//SU MCS10
							0x0,//SU MCS11
						};
							
					


int atbm_cronus_Get_MCS_LUT_Offset_Index(int WiFiMode,int OFDMMode,int ChBW,int RateIndex)
{
	int offsetIndex = 0;
	atbm_uint8 DSSS_Index[] = {0, 1};
	
	atbm_uint8 OFDM_LM_MM_Index[] = {2, 3, 4, 5, 6, 7, 8, 9, 10};
	
	atbm_uint8 OFDM_SU_ER_SU_Index[] = {11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22};
	
	atbm_uint8 OFDM_TB_Index[] = {24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35};

	if(WiFiMode == ATBM_WIFI_MODE_DSSS)
	{
		if(RateIndex < 2)
			offsetIndex = DSSS_Index[0];//1M/2M
		else
			offsetIndex = DSSS_Index[1];//5.5M/11M
	}
	else
	{
		if(OFDMMode == ATBM_WIFI_OFDM_MD_LM)
			offsetIndex = OFDM_LM_MM_Index[RateIndex];//6M - 54M
		else if(OFDMMode == ATBM_WIFI_OFDM_MD_MM)
		{
			if((RateIndex == 0) || (RateIndex == 8))
				offsetIndex = OFDM_LM_MM_Index[RateIndex];//mcs0/mcs32
			else
				offsetIndex = OFDM_LM_MM_Index[RateIndex+1];//mcs1~mcs7
		}
		else if(OFDMMode == ATBM_WIFI_OFDM_MD_HE_SU)
			offsetIndex = OFDM_SU_ER_SU_Index[RateIndex];//mcs0 - mcs11
		else if(OFDMMode == ATBM_WIFI_OFDM_MD_HE_ER_SU)
		{
			if(RateIndex < 3)
				offsetIndex = OFDM_SU_ER_SU_Index[RateIndex];//mcs0/mcs1/mcs2
			else
				wifi_printk(WIFI_ALWAYS,"HE_ER_SU only support MCS0/MCS1/MCS2\n");
		}
		else if(OFDMMode == ATBM_WIFI_OFDM_MD_HE_TB)
			offsetIndex = OFDM_TB_Index[RateIndex];//mcs0 - mcs11
	}

	if(ChBW == ATBM_WIFI_BW_40M)
		offsetIndex += CRONUS_OFDM_40M_OFFSET;

	return offsetIndex;
}


int atbm_oceanus_Get_MCS_LUT_Offset_Index(int WiFiMode,int OFDMMode,int ChBW,int RateIndex)
{
    atbm_uint8 BLE_Index[] = {0, 1, 2, 3};
    atbm_uint8 DSSS_Index[] = {4, 5};
    atbm_uint8 OFDM_LM_MM_Index[] = {6, 7, 8, 9, 10, 11, 12, 13, 14};    
    atbm_uint8 OFDM_VHT_Index[] = {15, 16, 17, 18, 19, 20,21, 22, 23, 24};    
    atbm_uint8 OFDM_SU_ER_SU_Index[] = { 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36};    
    atbm_uint8 OFDM_TB_Index[] = {38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49};
    
	int offsetIndex = 0;

    if((WiFiMode != ATBM_WIFI_MODE_DSSS)&&(WiFiMode != ATBM_WIFI_MODE_OFDM))
    {
        offsetIndex = BLE_Index[RateIndex];
    }
	else if(WiFiMode == ATBM_WIFI_MODE_DSSS)
	{
		if(RateIndex < 2)
			offsetIndex = DSSS_Index[0];//1M/2M
		else
			offsetIndex = DSSS_Index[1];//5.5M/11M
	}
	else
	{
		if(OFDMMode == ATBM_WIFI_OFDM_MD_LM)
			offsetIndex = OFDM_LM_MM_Index[RateIndex];//6M - 54M
		else if(OFDMMode == ATBM_WIFI_OFDM_MD_MM)
		{
			if((RateIndex == 0) || (RateIndex == 8) || (RateIndex == 9))
            {
                offsetIndex = OFDM_LM_MM_Index[RateIndex];//mcs0/mcs32/MCS8
            }         
			else
            {
                if(RateIndex > 9)
                {//mcs9~mcs15
                    RateIndex -= 9;
                }
                else
                {//mcs1~mcs7
                    
                }
                offsetIndex = OFDM_LM_MM_Index[RateIndex+1];//mcs9~mcs15
            }         
                }
                else if((OFDMMode == ATBM_WIFI_OFDM_MD_VHT)||(OFDMMode == ATBM_WIFI_OFDM_MD_VHT_SU)||(OFDMMode == ATBM_WIFI_OFDM_MD_VHT_MU))
        {
            offsetIndex = OFDM_VHT_Index[RateIndex];//mcs0 - mcs11
        }   
                else if(OFDMMode == ATBM_WIFI_OFDM_MD_HE_SU)
        {
            offsetIndex = OFDM_SU_ER_SU_Index[RateIndex];//mcs0 - mcs11
        }      
		else if(OFDMMode == ATBM_WIFI_OFDM_MD_HE_ER_SU)
		{
			if(RateIndex < 3)
				offsetIndex = OFDM_SU_ER_SU_Index[RateIndex];//mcs0/mcs1/mcs2
			else
				wifi_printk(WIFI_ALWAYS,"HE_ER_SU only support MCS0/MCS1/MCS2");
		}
		else if(OFDMMode == ATBM_WIFI_OFDM_MD_HE_TB)
			offsetIndex = OFDM_TB_Index[RateIndex];//mcs0 - mcs11
	}

	if(ChBW == ATBM_WIFI_BW_40M)
    {
        offsetIndex += OFDM_40M_MCS_LUT_OFFSET;
    }

	return offsetIndex;
    
}

int atbm_Get_MCS_LUT_Offset_Index(int WiFiMode,int OFDMMode,int ChBW,int RateIndex)
{
	int offsetIndex = 0;

	if(atbm_hw_priv_chip_id() == HW_CHIP_VERSION_Cronus)
		offsetIndex = atbm_cronus_Get_MCS_LUT_Offset_Index(WiFiMode, OFDMMode, ChBW, RateIndex);
	else
		offsetIndex = atbm_oceanus_Get_MCS_LUT_Offset_Index(WiFiMode, OFDMMode, ChBW, RateIndex);

	return offsetIndex;
}

unsigned int atbm_Get_MCS_LUT_Addr_ByTxUICtrlIndex(int WiFiModeCtrlIndex,int OfdmModeCtrlIndex,int ChBwCtrlIndex,int RateCtrlIndex)
{
	int offsetIndex = 0;
	atbm_uint32 mcsLUTAddr = 0x0ACBD510;//0xACBE010;//mcs LUT base addr

	if(atbm_hw_priv_chip_id() == HW_CHIP_VERSION_Cronus)
		mcsLUTAddr = 0xACBE010;//cronus mcs LUT base addr

	offsetIndex = atbm_Get_MCS_LUT_Offset_Index(WiFiModeCtrlIndex,OfdmModeCtrlIndex,ChBwCtrlIndex,RateCtrlIndex);
	mcsLUTAddr += (offsetIndex << 2);
	return mcsLUTAddr;
}

unsigned int atbm_Get_MCS_LUT_Addr(int wifi_mode,int ofdm_mode, int ch_bw, int rate_index)
{
    return atbm_Get_MCS_LUT_Addr_ByTxUICtrlIndex(wifi_mode,ofdm_mode,ch_bw,rate_index);
}
/*
* McsOffset: the return value of 'atbm_Get_MCS_LUT_Offset_Index'
*/
static char atbm_chip_work_on_5g(void)
{
	if(HW_CHIP_VERSION_Oceanus_FM == atbm_hw_priv_chip_id())
	{
		atbm_uint32 txrf_rf_bandsel = 0;
		txrf_rf_bandsel = HW_READ_REG_BIT(0x0ACC0028, 22, 22); //txrf_rf_bandsel
		return ((1 == txrf_rf_bandsel)?(1):(0));
	}
	else
	{
		return 0;
	}
}

static void atbm_oceanus_one_mcs_target_save(int bw,int McsOffset,int power_target)
{
    if(atbm_chip_work_on_5g())
    {
        if(bw == 0)
            MCS_LUT_20UL_OFDM_5G[McsOffset-BLE_MCS_LUT_NUM] = (char)power_target;
        else
            MCS_LUT_40M_OFDM_5G[McsOffset-OFDM_40M_OFFSET] = (char)power_target;
    }
    else
    {
        if(bw == 0)
            MCS_LUT_20M_Modify[McsOffset-BLE_MCS_LUT_NUM] = (char)power_target;
        else
           MCS_LUT_40M_Modify[McsOffset-OFDM_40M_OFFSET] = (char)power_target;
    }
}

int atbm_set_target_power(int mode,int rateIndex,int bw,int power)
{
	int power_target = 0;
	atbm_uint32 mcsLUTAddr = 0;
	int index = 0;
	int wifi_mode = 0;
	int ofdm_mode = 0;
	int rateMin = 0;
	int rateMax = 0;
	int neg = 0;
	
	power = (power * 4);
	
	if(neg)
		power = 0 - power;

	switch(mode){
		case 0://DSSS
			wifi_mode = ATBM_WIFI_MODE_DSSS;
			rateMin = 0;
			rateMax = 3;
			break;
		case 1://LM
			wifi_mode  = ATBM_WIFI_MODE_OFDM;
			ofdm_mode = ATBM_WIFI_OFDM_MD_LM;
			rateMin = 0;
			rateMax = 7;
			break;
		case 2://MM
			wifi_mode  = ATBM_WIFI_MODE_OFDM;
			ofdm_mode = ATBM_WIFI_OFDM_MD_MM;
			rateMin = 0;
			rateMax = 7;
			break;
		case 3://HE-SU
			wifi_mode  = ATBM_WIFI_MODE_OFDM;
			ofdm_mode = ATBM_WIFI_OFDM_MD_HE_SU;
			rateMin = 0;
			rateMax = 11;
			break;
#if CONFIG_VHT			
		case 4://VHT
			wifi_mode  = ATBM_WIFI_MODE_OFDM;
			ofdm_mode = ATBM_WIFI_OFDM_MD_VHT;
			rateMin = 0;
			rateMax = 9;//power��?��??-20M/40M��????��mcs9????��??-???��
			break;
#endif			
		default:
			wifi_mode  = ATBM_WIFI_MODE_OFDM;
			ofdm_mode = ATBM_WIFI_OFDM_MD_MM;
			rateMin = 0;
			rateMax = 7;
			wifi_printk(WIFI_ALWAYS,"Invalid mode ,%d\n",mode);
			return -1;
	}
	
	if((rateIndex < rateMin) || (rateIndex > rateMax)){
		wifi_printk(WIFI_ALWAYS,"Invalid rate index:rateIndex[%d],range[%d,%d]\n",rateIndex,rateMin,rateMax);
		return -1;
	}

	bw = bw?1:0;
	mcsLUTAddr = atbm_Get_MCS_LUT_Addr(wifi_mode, ofdm_mode, bw, rateIndex);
	index = atbm_Get_MCS_LUT_Offset_Index(wifi_mode, ofdm_mode, bw, rateIndex);

	if(atbm_hw_priv_chip_id() == HW_CHIP_VERSION_Cronus)
	{
		if(bw == 0)
			power_target = MCS_LUT_20M_delta_mode_Cronus[index] + MCS_LUT_20M_delta_bw_Cronus[index] + power;
		else
			power_target = MCS_LUT_40M_delta_mode_Cronus[index-CRONUS_OFDM_40M_OFFSET] + MCS_LUT_40M_delta_bw_Cronus[index-CRONUS_OFDM_40M_OFFSET] + power;

		if(power_target > WIFI_POWER_TARGET_MAX*4)
			power_target = WIFI_POWER_TARGET_MAX*4;
		
		HW_WRITE_REG_BIT(mcsLUTAddr, 7, 0, (atbm_uint32)power_target);//function register
		if(bw == 0)
			MCS_LUT_20M_Modify_Cronus[index] = power;
		else
			MCS_LUT_40M_Modify_Cronus[index-CRONUS_OFDM_40M_OFFSET] = power;
	}
	else
	{
		if(bw == 0)
			power_target = MCS_LUT_20M_delta_mode[index-BLE_MCS_LUT_NUM] + MCS_LUT_20M_delta_bw[index-BLE_MCS_LUT_NUM] + power;
		else
			power_target = MCS_LUT_40M_delta_mode[index-OFDM_40M_OFFSET] + MCS_LUT_40M_delta_bw[index-OFDM_40M_OFFSET] + power;

		if(power_target > WIFI_POWER_TARGET_MAX*4)
			power_target = WIFI_POWER_TARGET_MAX*4;
		
		HW_WRITE_REG_BIT(mcsLUTAddr, 15, 8, (atbm_uint32)power_target);//function register
		//atbm_printk_always("mcsLUTAddr=0x%x\n", mcsLUTAddr);
		atbm_oceanus_one_mcs_target_save(bw,index,power);
	}
	
	//HW_WRITE_REG_BIT(0x0AC80CF8, 15, 8, (atbm_uint32)power_target);//force register

	return 0;
}



/*
	rate_txpower value = 10*dB
	power index offset:
	[4,5]:dsss
	[6,14]:gn
	[15,24]:VHT MCS0-MCS9
	[25,36]:HE-SU MCS0-MCS11
	[50,58]:gn 40M
	[59,68]:VHT 40M MCS0-MCS9
	[69,80]:HE-SU 40M MCS0-MCS11
*/
#define RATE_CFG_TXPOWER_ARRY_MAX 	(100)

int rate_txpower_cfg[RATE_CFG_TXPOWER_ARRY_MAX] = {0};
int rate_txpower_cfg_5g[RATE_CFG_TXPOWER_ARRY_MAX] = {0};
/*

target_or_delta=1
b_1M_2M=210
b_5_5M_11M=210
g_6M_n_6_5M=210
g_9M=200
g_12M_n_13M=190
g_18M_n_19_5M=190
g_24M_n_26M=190
g_36M_n_39M=190
g_48M_n_52M=190
g_54M_n_58_5M=180
n_65M=180
HE-SU_MCS0=210
HE-SU_MCS1=190
HE-SU_MCS2=190
HE-SU_MCS3=190
HE-SU_MCS4=190
HE-SU_MCS5=190
HE-SU_MCS6=180
HE-SU_MCS7=180
HE-SU_MCS8=170
HE-SU_MCS9=170
HE-SU_MCS10=160
HE-SU_MCS11=160
g_6M_n_6_5M_40M=200
g_9M_40M=190
g_12M_n_13M_40M=190
g_18M_n_19_5M_40M=190
g_24M_n_26M_40M=190
g_36M_n_39M_40M=190
g_48M_n_52M_40M=190
g_54M_n_58_5M_40M=180
n_65M_40M=170
HE-SU_MCS0_40M=200
HE-SU_MCS1_40M=190
HE-SU_MCS2_40M=190
HE-SU_MCS3_40M=190
HE-SU_MCS4_40M=190
HE-SU_MCS5_40M=190
HE-SU_MCS6_40M=180
HE-SU_MCS7_40M=170
HE-SU_MCS8_40M=170
HE-SU_MCS9_40M=170
HE-SU_MCS10_40M=160
HE-SU_MCS11_40M=160


*/
int wsm_set_rate_power(struct atbmwifi_common *hw_priv,int *rate_txpower)
{
	//int rate_txpower[100] = {0};//validfalg,data
	unsigned char u8_rate_txpower[100] = {0};
	//target_power[0]:0:2.4G;1:5G;
	//target_power[1]:0:20M;1:40M;
	//target_power[2]:power target
	//2.4G:
	//20M:11b 1M~11ax mcs11  summary 33 
	//40M:11g 6M~11ax mcs11  summary 31
	//5G:
	//20M:11g 6M~11ax mcs11  summary 31
	//40M:11g 6M~11ax mcs11  summary 31
	unsigned char target_power[36] = {0};
	int i = 0;
	int ret = -1;
	int power = 0;
	int powerTarFlag = 0;

	int cpbytes = 0;
	wifi_printk(WIFI_ALWAYS,"CHIP ID = %x",hw_priv->chip_id);
//	if(hw_priv->chip_id == HW_CHIP_VERSION_Cronus_Lite_FM)
	{
		//cronus lite not support 11ac
		//20M:rate_txpower[1]~rate_txpower[23]
		//40M:rate_txpower[24]~rate_txpower[44]
		for(i=1;i<=64;i++)
			u8_rate_txpower[i] = (rate_txpower[i] * 4);
#if CONFIG_VHT	
		target_power[1] = 0;
		memcpy(&target_power[2], &u8_rate_txpower[1], 33);
		ret = wsm_write_mib(hw_priv, WSM_MIB_ID_TX_TARGET_POWER_CONFIG, target_power, 33+2, 0);
		if(ret != 0){
			wifi_printk(WIFI_ALWAYS,"write mib failed(%d)\n", ret);
		}

		//40M
		target_power[1] = 1;
		memcpy(&target_power[2], &u8_rate_txpower[34], 31);
		ret = wsm_write_mib(hw_priv, WSM_MIB_ID_TX_TARGET_POWER_CONFIG, target_power, 31+2, 0);
		if(ret != 0){
			wifi_printk(WIFI_ALWAYS,"write mib failed(%d)\n", ret);
		}
#else

		//20M
		target_power[1] = 0;
		memcpy(&target_power[2], &u8_rate_txpower[1], 23);
		ret = wsm_write_mib(hw_priv, WSM_MIB_ID_TX_TARGET_POWER_CONFIG, target_power, 23+2, 0);
		if(ret != 0){
			wifi_printk(WIFI_ALWAYS,"write mib failed(%d)\n", ret);
		}

		//40M
		target_power[1] = 1;
		memcpy(&target_power[2], &u8_rate_txpower[24], 21);
		ret = wsm_write_mib(hw_priv, WSM_MIB_ID_TX_TARGET_POWER_CONFIG, target_power, 21+2, 0);
		if(ret != 0){
			wifi_printk(WIFI_ALWAYS,"write mib failed(%d)\n", ret);
		}
#endif		
		memcpy(rate_txpower_cfg, rate_txpower, sizeof(rate_txpower));
	}
#if 0	
	else
	{
		//---------------------------------------------------Oceanus 2.4G
		//20M:rate_txpower[1]~rate_txpower[33]
		//40M:rate_txpower[34]~rate_txpower[64]
		{
			for(i=1;i<=64;i++)
			{
				u8_rate_txpower[i] = (rate_txpower[i] * 4)/10;
			}

			//20M
			target_power[1] = 0;
			memcpy(&target_power[2], &u8_rate_txpower[1], 33);
			ret = wsm_write_mib(hw_priv, WSM_MIB_ID_TX_TARGET_POWER_CONFIG, target_power, 33+2, 0);
			if(ret != 0){
				atbm_printk_err("write mib failed(%d)\n", ret);
			}

			//40M
			target_power[1] = 1;
			memcpy(&target_power[2], &u8_rate_txpower[34], 31);
			ret = wsm_write_mib(hw_priv, WSM_MIB_ID_TX_TARGET_POWER_CONFIG, target_power, 31+2, 0);
			if(ret != 0){
				atbm_printk_err("write mib failed(%d)\n", ret);
			}
			memcpy(rate_txpower_cfg, rate_txpower, sizeof(rate_txpower));
		}

		memset(rate_txpower, 0, sizeof(rate_txpower));
		//---------------------------------------------------Oceanus 5G
		//20M:rate_txpower[1]~rate_txpower[31]
		//40M:rate_txpower[32]~rate_txpower[62]
		{
			for(i=1;i<=64;i++)
			u8_rate_txpower[i] = (rate_txpower[i] * 4)/10;
		
			target_power[0] = 1;
			//20M
			target_power[1] = 0;
			memcpy(&target_power[2], &u8_rate_txpower[1], 31);
			ret = wsm_write_mib(hw_priv, WSM_MIB_ID_TX_TARGET_POWER_CONFIG, target_power, 31+2, 0);
			if(ret != 0){
				atbm_printk_err("write mib failed(%d)\n", ret);
			}

			//40M
			target_power[1] = 1;
			memcpy(&target_power[2], &u8_rate_txpower[32], 31);
			ret = wsm_write_mib(hw_priv, WSM_MIB_ID_TX_TARGET_POWER_CONFIG, target_power, 31+2, 0);
			if(ret != 0){
				atbm_printk_err("write mib failed(%d)\n", ret);
			}
			memcpy(rate_txpower_cfg_5g, rate_txpower, sizeof(rate_txpower));
		}
	}
#endif	
	return ret;
}
int atbm_hweight32(atbm_uint32 data_mask)
{
	int bit = 0;
	int agg_cnt = 0;
	for(bit = 0;bit < 32;bit++){
		if(data_mask & ATBM_BIT(bit)){
			agg_cnt++;
		}
	}
	
	return agg_cnt;
}

int atbm_set_ap_channel(int if_id, int channel, int chantype)
{
	struct wsm_set_chantype wsm_chantype = {0};
	struct atbmwifi_vif *priv = NULL;
	struct atbmwifi_common *hw_priv = NULL;
#if CONFIG_ATBM_SUPPORT_CSA
	struct atbm_ieee80211_csa_request csa = {0};
#endif

	if (if_id >= 2 || if_id < 0) {
		wifi_printk(WIFI_ERROR, "%s: Invalid interface id %d\n", __func__, if_id);
		return -EINVAL;
	}

	priv = atbm_wifi_vif_get(if_id);
	hw_priv = _atbmwifi_vifpriv_to_hwpriv(priv);
	if (!priv || !hw_priv) {
		return -EINVAL;
	}

	if (!atbmwifi_is_ap_mode(priv->iftype)) {
		wifi_printk(WIFI_ERROR,"Only support in ap mode\n");
		return -EINVAL;
	}

	wifi_printk(WIFI_ALWAYS,"band(%d) set_channel: channel %d, chantype %d\n", priv->config.band, channel, chantype);
	if ((priv->config.band == ATBM_IEEE80211_BAND_2GHZ && (channel > 14 || channel < 1))
		|| (priv->config.band == ATBM_IEEE80211_BAND_5GHZ && (channel > 165 || channel < 36))) {
		wifi_printk(WIFI_ERROR,"set_channel: Invalid channel %d\n", channel);
		return -EINVAL;
	}

#if CONFIG_ATBM_SUPPORT_CSA
	csa.ie.new_ch_num = channel;
	csa.ie.mode = 1;
	csa.ie.count = 10;
	csa.chan = channel;
	csa.chantype = chantype;
	csa.mactime = atbm_GetOsTimeMs();
	wifi_printk(WIFI_CSA,"band %d start csa work\n", priv->config.band);
	atbm_ieee80211_start_csa_work(priv, &csa);
#else
	wsm_chantype.band = priv->config.band;
	wsm_chantype.channelNumber = channel;
	wsm_chantype.channelType = chantype;
	wsm_chantype.flag = 0;
	priv->config.channel_index = channel;
	priv->bss.channel_type = chantype;
	wpa_comm_init_extra_ie(priv);
	wsm_set_chantype_func(hw_priv, &wsm_chantype, priv->if_id);
	atbmwifi_ap_start_beacon(priv);
	if (priv->extra_ie) {
		atbm_kfree(priv->extra_ie);
		priv->extra_ie = ATBM_NULL;
		priv->extra_ie_len = 0;
	}
#endif
	return 0;
}


#if ATBM_STA_WOL
int atbm_set_sta_wow(atbm_uint8 enable)
{
	struct atbmwifi_vif *priv = NULL;
	struct atbmwifi_common *hw_priv = NULL;

	priv = atbm_wifi_vif_get(0);
	hw_priv = _atbmwifi_vifpriv_to_hwpriv(priv);
	if (!priv || !hw_priv) {
		return -EINVAL;
	}
	
	if(wsm_set_wol_enable(hw_priv,enable,0) < 0){
		wifi_printk(WIFI_ALWAYS,"%s:%s wow fail \n",__func__,enable == 0?"stop":"start");
		return -1;
	}
	return 0;
}



#endif


#if CONFIG_ATBM_SUPPORT_REKEY
int atbm_set_rekey_data(struct atbmwifi_vif *priv,struct atbmwifi_wpa_ptk *ptk,  int enable)
{
	struct atbmwifi_common *hw_priv = priv->hw_priv;
	struct wsm_rekey_data data;
	

	memset(&data,0,sizeof(struct wsm_rekey_data));

	if(enable){
		data.flags = 1;
		memcpy(data.kck,ptk->kck,16);
		memcpy(data.kek,ptk->kek,16);
	}
	
	if(wsm_set_rekey_data(hw_priv,&data,priv->if_id) < 0){
		wifi_printk(WIFI_ALWAYS,"%s: set rekey fail\n",__func__);
		return -1;
	}
	return 0;
}
#endif
#if CONFIG_VHT	
#define MODE_MAX 5
int atbm_ladder_txpower_63(void)
{
	struct atbmwifi_common *hw_priv = &g_hw_prv;
	int txrate_power[100] = {0};
	int txrate_power_temp[200] = {0},pos = 0;
	int i = 0,j = 0,ret = 0,ladder = 0,m = 1,n = 34;
	int mode[MODE_MAX] = {0,1,2,3,4};
	int RateMaxId[MODE_MAX]={2,8,1,10,12};

	 int power_target_hight[MODE_MAX][12]={
			 {22,22},//11b 1M/2M  5.5M/11M
			 {22,21,21,20,19,19,18,18},//11g/n
			 {18},//11n mcs7
			 {22,21,20,19,19,19,18,18,17,17},//11ac
			 {22,21,20,19,19,19,18,18,17,17,16,16},//11ax
		 };

	int (*power_target)[12] = NULL;


	power_target = power_target_hight;
	
	txrate_power[0] = 1;
	 for(i = 0;i < MODE_MAX;i++){
		 for(j = 0;j < RateMaxId[i];j++){

			txrate_power[m] = power_target[i][j];
			m++;
#if BW_40M_SUPPORT
			if( i > 0){
				txrate_power[n] = power_target[i][j];
				n++;
			}
#endif		 
		}
	 }
	 
	 for(i = 0;i < 100;i++){
	 	wifi_printk(WIFI_ALWAYS,"[%d] %d \n",i,txrate_power[i]);	
	 }
	
	ret = wsm_set_rate_power(hw_priv,txrate_power);

	 return ret;
}
#else
int atbm_ladder_txpower_63(void)
{
	struct atbmwifi_common *hw_priv = &g_hw_prv;
	int txrate_power[100] = {0};
	int txrate_power_temp[200] = {0},pos = 0;
	int i = 0,j = 0,ret = 0,ladder = 0,m = 1,n = 24;
	int mode[MODE_MAX] = {0,1,2,3,0};
	int RateMaxId[MODE_MAX]={2,8,1,12,0};

	 int power_target_hight[5][12]={
			 {22,22},//11b 1M/2M  5.5M/11M
			 {22,21,21,20,19,19,18,18},//11g/n
			 {18},//11n mcs7
			 {22,21,20,19,19,19,18,18,17,17,16,16},//11ax
		 };

	int (*power_target)[12] = NULL;


	power_target = power_target_hight;
	
	txrate_power[0] = 1;
	 for(i = 0;i < MODE_MAX;i++){
		 for(j = 0;j < RateMaxId[i];j++){

			txrate_power[m] = power_target[i][j];
			m++;
#if BW_40M_SUPPORT
			if( i > 0){
				txrate_power[n] = power_target[i][j];
				n++;
			}
#endif		 
		}
	 }
	 

	
	ret = wsm_set_rate_power(hw_priv,txrate_power);
	 return ret;
}

#endif

extern int atbm_set_frame_filter(struct atbmwifi_common *hw_priv,struct atbmwifi_vif *priv,unsigned int n_filters,
		  struct ieee80211_special_filter *filter_table,char enable);
extern int set_special_beacon(struct atbmwifi_vif *priv,char *special_ie,int special_ie_len);

extern int set_special_proberesp(struct atbmwifi_vif *priv,char *special_ie,int special_ie_len);
extern int send_special_proberesp(struct atbmwifi_vif *priv,char *special_ie,int special_ie_len,char *dst_mac);

extern int set_special_scan(struct atbmwifi_vif *priv,char *special_ie,int special_ie_len,char chan);


/*
	insert private data on beacon frame
	default :
		IE ID : 233
		IE LEN : max 255
		IE DATA : max  255 byte

paramters :
	ie : IE DATA

*/
int atbm_set_beacon(char *ie)
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

int atbm_set_prbrsp(char *ie)
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
                wifi_printk(WIFI_DBG_ERROR,"atbm_set_prbrsp : ap mode not running \n");
                return -1;
        }
        if(ie){
                ie_len = strlen(ie);
                buff[0] = 233;
                buff[1] = ie_len;
                memcpy(buff+2,ie,buff[1]);
        }else{
                wifi_printk(WIFI_DBG_ERROR,"atbm_set_prbrsp : ie is NULL\n");
                return -1;
        }

        return set_special_proberesp(priv,buff,ie_len + 2);
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
struct atbm_vendor_cfg_ie{
	char ie_id;
	char ie_len;
	char OUI[4];
	char ssid_len;
	char password_len;
	char ssid[32];
	char password[64];
};	
#define ATBM_6441_PRIVATE_OUI 0x4154424d

int atbm_send_prbrsp(char *ie_ssid,char *ie_psk,char *dst_addr)
{
	int i =0,ap = 0;
	struct atbmwifi_vif *priv;
	unsigned char dmac[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
	struct atbm_vendor_cfg_ie private_ie;
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
		//sscanf(dst_addr,"%02x:%02x:%02x:%02x:%02x:%02x",&dmac[0],&dmac[1],&dmac[2],&dmac[3],&dmac[4],&dmac[5]);
		//wifi_printk(WIFI_ALWAYS,"dest MAC Addr:[%02x:%02x:%02x:%02x:%02x:%02x]\n",
		//						dmac[0],dmac[1],dmac[2],dmac[3],dmac[4],dmac[5]);
	}else{
		wifi_printk(WIFI_ALWAYS,"atbm_send_prbrsp : dst_addr is NULL,send error \n");
		return -1;
	}
	if(ie_ssid){
		atbm_memset(&private_ie,0,sizeof(struct atbm_vendor_cfg_ie));
		private_ie.ie_id = 221;
		private_ie.OUI[0] = (ATBM_6441_PRIVATE_OUI >> 24) & 0xFF;
		private_ie.OUI[1] = (ATBM_6441_PRIVATE_OUI >> 16) & 0xFF;
		private_ie.OUI[2] = (ATBM_6441_PRIVATE_OUI >> 8) & 0xFF;
		private_ie.OUI[3] = ATBM_6441_PRIVATE_OUI & 0xFF;
		atbm_memcpy(private_ie.ssid,ie_ssid,strlen(ie_ssid));
		private_ie.ssid_len = strlen(ie_ssid);
		atbm_memcpy(private_ie.password,ie_psk,strlen(ie_psk));
		private_ie.password_len = strlen(ie_psk);
		private_ie.ie_len = sizeof(struct atbm_vendor_cfg_ie) - 2;
	}else{
		wifi_printk(WIFI_DBG_ERROR,"atbm_priv_scan : ie is NULL\n");
		return -1;
	}

	return send_special_proberesp(priv,&private_ie,sizeof(struct atbm_vendor_cfg_ie),dmac);
}



atbm_int32 atbmwifi_set_chan_to_freq(atbm_uint32 chan, atbm_uint32 freq)
{
	atbm_int32 ret = 0;
	char cmd[32];
	atbm_uint8 if_id;
	struct atbmwifi_vif *priv=ATBM_NULL;
	struct atbmwifi_common *hw_priv=ATBM_NULL;

	atbm_memset(cmd, 0, sizeof(cmd));
	atbm_for_each_vif(&g_hw_prv,priv,if_id){
		if(priv== NULL){
			continue;
		}
		hw_priv	= _atbmwifi_vifpriv_to_hwpriv(priv);
		sprintf(cmd,"set_freq %d %d ", chan,freq);
		ret = wsm_write_mib(hw_priv, WSM_MIB_ID_FW_CMD,cmd, strlen(cmd), priv->if_id);
	}
	return ret;
}

