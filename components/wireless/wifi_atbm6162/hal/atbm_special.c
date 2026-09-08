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

#include "atbm_special.h"
#include "atbm_hal.h"

#define WLAN_SPECIAL_IE 233



#define STR2MAC1		"%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC2STR1(a)  a[0],a[1],a[2],a[3],a[4],a[5]
unsigned char *atbm_ieee80211_find_ie(unsigned char eid, const unsigned char *ies, int len);

void ieee80211_special_filter_rx_package_handle(struct atbmwifi_vif *priv,struct atbm_buff *skb);
int atbm_set_frame_filter(struct atbmwifi_common *hw_priv,struct atbmwifi_vif *priv,unsigned int n_filters,
		  struct ieee80211_special_filter *filter_table,char enable);

int set_special_beacon(struct atbmwifi_vif *priv,char *special_ie,int special_ie_len);

int set_special_proberesp(struct atbmwifi_vif *priv,char *special_ie,int special_ie_len);
int set_special_scan(struct atbmwifi_vif *priv,char *special_ie,int special_ie_len,char chan);




unsigned char *atbm_ieee80211_find_ie(unsigned char eid, const unsigned char *ies, int len)
{
	while (len > 2 && ies[0] != eid) {
		len -= ies[1] + 2;
		ies += ies[1] + 2;
	}
	if (len < 2)
		return NULL;
	if (len < 2 + ies[1])
		return NULL;
	return ies;
}

/*
Handles administrative frames returned by private filters

contains : beacon / probe req / probe respones

*/
void ieee80211_special_filter_rx_package_handle(struct atbmwifi_vif *priv,struct atbm_buff *skb)
{
	struct atbmwifi_ieee80211_mgmt *mgmt = (struct atbmwifi_ieee80211_mgmt *)ATBM_OS_SKB_DATA(skb);
	unsigned char *elements;
	int baselen;
	struct atbmwifi_ieee802_11_elems elems;
	struct atbmwifi_ieee80211_rx_status *rx_status = ATBM_IEEE80211_SKB_RXCB(skb);
	int freq;
	char ssid[32]={0};
	const unsigned char *ie = NULL;

	int len = 0;
	atbm_uint8 * data = ATBM_NULL;


	if(atbmwifi_ieee80211_is_beacon(mgmt->frame_control)){

		baselen = offsetof(struct atbmwifi_ieee80211_mgmt, u.beacon.variable);
		if (baselen > ATBM_OS_SKB_LEN(skb)){
			wifi_printk(WIFI_DBG_ERROR,"[beacon] error ! \n");
		}
		elements = mgmt->u.beacon.variable;
		atbm_ieee802_11_parse_elems(elements, skb->dlen - baselen, &elems);
		if (elems.ds_params && elems.ds_params_len == 1)
			freq = atbmwifi_ieee80211_channel_to_frequency(elems.ds_params[0],
						      rx_status->band);
		else
			freq = rx_status->freq;

		memcpy(ssid,elems.ssid,elems.ssid_len);
		freq = (freq-2407)/5;

		ie = atbm_ieee80211_find_ie(WLAN_SPECIAL_IE,mgmt->u.beacon.variable,
				                   ATBM_OS_SKB_LEN(skb)-offsetof(struct atbmwifi_ieee80211_mgmt, u.beacon.variable));

		if(ie){
			char special_data[255]={0};
			memcpy(special_data,ie+2,ie[1]);
			wifi_printk(WIFI_DBG_ERROR,"[beacon] from ["STR2MAC1"] channel[%d] ssid[%s] ie[%d][%d][%s]\n",MAC2STR1(mgmt->bssid),freq,ssid,ie[0],ie[1],special_data);
		}else{
			wifi_printk(WIFI_DBG_ERROR,"[beacon] from ["STR2MAC1"] channel[%d] ssid[%s] \n",MAC2STR1(mgmt->bssid),freq,ssid);
		}

	}
	else if(atbmwifi_ieee80211_is_probe_req(mgmt->frame_control)){
		baselen = offsetof(struct atbmwifi_ieee80211_mgmt, u.probe_req.variable);
		if (baselen > ATBM_OS_SKB_LEN(skb)){
			wifi_printk(WIFI_DBG_ERROR,"[probereq] error ! \n");
		}
		elements = mgmt->u.probe_req.variable;
		atbm_ieee802_11_parse_elems((atbm_uint8 *)elements, len, &elems);
		if (elems.ds_params && elems.ds_params_len == 1)
			freq = atbmwifi_ieee80211_channel_to_frequency(elems.ds_params[0],
						      rx_status->band);
		else
			freq = rx_status->freq;

		freq = (freq-2407)/5;
		ie = atbm_ieee80211_find_ie(WLAN_SPECIAL_IE,mgmt->u.probe_req.variable,
				                   ATBM_OS_SKB_LEN(skb)-offsetof(struct atbmwifi_ieee80211_mgmt, u.probe_req.variable));
		if(ie){
			char special_data[255]={0};
			memcpy(special_data,ie+2,ie[1]);
			wifi_printk(WIFI_DBG_ERROR,"[probereq] from ["STR2MAC1"] channel[%d] special ie[%d][%d][%s]\n",MAC2STR1(mgmt->sa),freq,ie[0],ie[1],special_data);

		}else {
			wifi_printk(WIFI_DBG_ERROR,"[probereq] from ["STR2MAC1"] channel[%d] \n",MAC2STR1(mgmt->sa),freq);
		}
	}
	else if(atbmwifi_ieee80211_is_probe_resp(mgmt->frame_control)){

		baselen = offsetof(struct atbmwifi_ieee80211_mgmt, u.probe_resp.variable);
		if (baselen > ATBM_OS_SKB_LEN(skb)){
			wifi_printk(WIFI_DBG_ERROR,"[beacon] error ! \n");
		}
		elements = mgmt->u.probe_resp.variable;
		atbm_ieee802_11_parse_elems(elements, skb->dlen - baselen, &elems);
		if (elems.ds_params && elems.ds_params_len == 1)
			freq = atbmwifi_ieee80211_channel_to_frequency(elems.ds_params[0],rx_status->band);
		else
			freq = rx_status->freq;

		memcpy(ssid,elems.ssid,elems.ssid_len);
		freq = (freq-2407)/5;

		ie = atbm_ieee80211_find_ie(WLAN_SPECIAL_IE,mgmt->u.probe_resp.variable,
				                   ATBM_OS_SKB_LEN(skb)-offsetof(struct atbmwifi_ieee80211_mgmt, u.probe_resp.variable));
		if(ie){
			char special_data[255]={0};
			memcpy(special_data,ie+2,ie[1]);
			wifi_printk(WIFI_DBG_ERROR,"[probe resp] from ["STR2MAC1"] channel[%d] ssid[%s] ie[%d][%d][%s]\n",MAC2STR1(mgmt->bssid),freq,ssid,ie[0],ie[1],special_data);
		}else{
			wifi_printk(WIFI_DBG_ERROR,"[probe resp] from ["STR2MAC1"] channel[%d] ssid[%s] \n",MAC2STR1(mgmt->bssid),freq,ssid);
		}


	}
	else {
		wifi_printk(WIFI_DBG_ERROR,"[others][%x] from ["STR2MAC1"]\n",mgmt->frame_control,MAC2STR1(mgmt->sa));
	}
	//atbm_dev_kfree_skb(skb);
}


int atbm_set_frame_filter(struct atbmwifi_common *hw_priv,struct atbmwifi_vif *priv,unsigned int n_filters,
		  struct ieee80211_special_filter *filter_table,char enable)
{
	int ret;
	struct wsm_beacon_filter_table table;
	int index = 0;
	struct wsm_beacon_filter_control enabled = {
		.enabled = 0,
		.bcn_count = 1,
	};
	if(n_filters && (enable == 1)){
		memset(&table,0,sizeof(struct wsm_beacon_filter_table));

		for(index = 0;index < n_filters;index ++){
			table.entry[index].ieId = filter_table[index].filter_action;
			memcpy(table.entry[index].oui,filter_table[index].oui,3);

			if(filter_table[index].flags & SPECIAL_F_FLAGS_FRAME_TYPE){
				table.entry[index].actionFlags = WSM_BEACON_FILTER_FRAME_TYPE;
				wifi_printk(WIFI_DBG_ERROR,"%s:frame [%d]\n",__func__,table.entry[index].ieId);
			}
			else if(filter_table[index].flags & SPECIAL_F_FLAGS_FRAME_OUI){
				table.entry[index].actionFlags = WSM_BEACON_FILTER_ACTION_ENABLE | WSM_BEACON_FILTER_OUI;
				wifi_printk(WIFI_DBG_ERROR,"%s:ie_oui ie[%d],oui[%d:%d:%d]\n",__func__,
				table.entry[index].ieId,table.entry[index].oui[0],table.entry[index].oui[1],table.entry[index].oui[2]);
			}
			else if(filter_table[index].flags & SPECIAL_F_FLAGS_FRAME_IE){
				table.entry[index].actionFlags = WSM_BEACON_FILTER_ACTION_ENABLE;
				wifi_printk(WIFI_DBG_ERROR,"%s:ie [%d]\n",__func__,table.entry[index].ieId);
			}
			else
				wifi_printk(WIFI_DBG_ERROR,"%s:set error\n",__func__);
		}

		table.numOfIEs = atbm_cpu_to_le32(n_filters);

		ret = wsm_set_beacon_filter_table(hw_priv,&table,priv->if_id);

		if(ret != 0)
			return ret;
	}
	enabled.enabled = enable == 1;
	ret = wsm_beacon_filter_control(hw_priv,
					&enabled, priv->if_id);

	return ret;

}


/*
	insert special ie to beacon to lmac

special_ie:
	IE ID + IE_LEN + IE DATA
special_ie_len:
	IE_LEN + 2



*/
int set_special_beacon(struct atbmwifi_vif *priv,char *special_ie,int special_ie_len)
{
	char *ie_buf;
	int ie_len = 0;

	if(special_ie && (special_ie_len > 0)){

		//ie_len = strlen(ie);

		if(special_ie_len > 257){
			wifi_printk(WIFI_DBG_ERROR,"%s:ie_len lager 255 , ie_len = %d\n",__func__,special_ie_len);
			return -1;
		}

		ie_buf = (char *)atbm_kmalloc(special_ie_len,GFP_KERNEL);
		if(atbm_unlikely(!ie_buf)){
			return -1;
		}
		atbm_memset(ie_buf,0,special_ie_len);

		if(priv->special_extra_ie){
			atbm_kfree(priv->special_extra_ie);
		}
		priv->special_extra_ie = ie_buf;
	//	priv->special_extra_ie[0] = WLAN_SPECIAL_IE; // ie id
	//	priv->special_extra_ie[1] = ie_len;
		atbm_memcpy(priv->special_extra_ie,special_ie,special_ie_len);
		priv->special_extra_ie_len =  special_ie_len;
	}else{
		wifi_printk(WIFI_DBG_ERROR,"%s:ie is NULL,clear private IE \n",__func__);
		if(priv->special_extra_ie){
			atbm_kfree(priv->special_extra_ie);
		}
		priv->special_extra_ie = NULL;
		priv->special_extra_ie_len = 0;
		return -1;
	}
	hostapd_init_extra_ie(priv);
	priv->special_beacon_set = 1;
	atbmwifi_ap_start_beacon(priv);
	priv->special_beacon_set = 0;

	atbm_kfree(priv->extra_ie);
	priv->extra_ie = ATBM_NULL;
	priv->extra_ie_len = 0;
	return 0;
}


/*
	insert special ie to PROBE RESP to lmac

special_ie:
	IE ID + IE_LEN + IE DATA
special_ie_len:
	IE_LEN + 2



*/
int set_special_proberesp(struct atbmwifi_vif *priv,char *special_ie,int special_ie_len)
{
	char *ie_buf;
	int ie_len = 0;

	if(special_ie && (special_ie_len > 0)){

		//ie_len = strlen(ie);

		if(special_ie_len > 257){
			wifi_printk(WIFI_DBG_ERROR,"%s:ie_len lager 255 , ie_len = %d\n",__func__,special_ie_len);
			return -1;
		}

		ie_buf = (char *)atbm_kmalloc(special_ie_len,GFP_KERNEL);
		if(atbm_unlikely(!ie_buf)){
			return -1;
		}

		atbm_memset(ie_buf,0,special_ie_len);

		if(priv->special_extra_ie){
			atbm_kfree(priv->special_extra_ie);
		}
		priv->special_extra_ie = ie_buf;
	//	priv->special_extra_ie[0] = WLAN_SPECIAL_IE; // ie id
	//	priv->special_extra_ie[1] = ie_len;
		atbm_memcpy(priv->special_extra_ie,special_ie,special_ie_len);
		priv->special_extra_ie_len = special_ie_len;
	}else{
		wifi_printk(WIFI_DBG_ERROR,"%s:ie is NULL,clear private IE \n",__func__);
		if(priv->special_extra_ie){
			atbm_kfree(priv->special_extra_ie);
		}
		priv->special_extra_ie = NULL;
		priv->special_extra_ie_len = 0;
	}
	hostapd_init_extra_ie(priv);
	priv->special_beacon_set = 0;
	atbmwifi_ap_start_proberesp(priv);

	atbm_kfree(priv->extra_ie);
	priv->extra_ie = ATBM_NULL;
	priv->extra_ie_len = 0;
	return 0;
}
/*
special_ie:
	IE ID + IE_LEN + IE DATA
special_ie_len:
	IE_LEN + 2

*/
int send_special_proberesp(struct atbmwifi_vif *priv,char *special_ie,int special_ie_len,char *dst_mac)
{

	char *ie_buf;
	int ie_len = 0;
	int ret;
	struct atbmwifi_common *hw_priv = _atbmwifi_vifpriv_to_hwpriv(priv);
	struct wsm_template_frame frame={0};

	if(!special_ie){
		wifi_printk(WIFI_DBG_ERROR,"%s:ie is NULL \n",__func__);
		return -1;
	}

	//ie_len = strlen(ie);

	if(special_ie_len > 257){
		wifi_printk(WIFI_DBG_ERROR,"%s:ie_len lager 255 , ie_len = %d\n",__func__,special_ie_len);
		return -1;
	}

	if(!dst_mac){
		wifi_printk(WIFI_DBG_ERROR,"%s:dst_mac is NULL\n",__func__);
		return -1;
	}

	ie_buf = (char *)atbm_kmalloc(special_ie_len,GFP_KERNEL);
	if(atbm_unlikely(!ie_buf)){
		return -1;
	}

	atbm_memset(ie_buf,0,special_ie_len);

	if(priv->special_extra_ie){
		atbm_kfree(priv->special_extra_ie);
	}
	priv->special_extra_ie = ie_buf;
	//priv->extra_ie[0] = WLAN_SPECIAL_IE; // ie id
	//priv->extra_ie[1] = ie_len;
	atbm_memcpy(priv->special_extra_ie,special_ie,special_ie_len);
	priv->special_extra_ie_len =  special_ie_len;


	/*set dst mac addr*/
	atbm_memcpy(priv->daddr,dst_mac,6);

	hostapd_init_extra_ie(priv);
	frame.frame_type = WSM_FRAME_TYPE_PROBE_RESPONSE;
	frame.disable =0;
	//frame.rate = test_config_txrx.Rate;
	frame.rate = 0;//test_config_txrx.Rate;
	frame.skb = atbmwifi_ieee80211_send_proberesp(priv,priv->extra_ie,priv->extra_ie_len);
	if (ATBM_WARN_ON(!frame.skb))
		return 0;


	atbmwifi_tx(hw_priv,frame.skb,priv);

//	atbm_dev_kfree_skb(frame.skb);

}



/*
	insert special ie to PROBE REQ and send probe req

special_ie:
	IE ID + IE_LEN + IE DATA
special_ie_len:
	IE_LEN + 2


*/
#if 0
#define SCAN_CHANNEL_COUNT 5
int set_special_scan(struct atbmwifi_vif *priv,char *special_ie,int special_ie_len,char chan)
{

	char *ie_buf;
	int ie_len = 0,scan_all = -1,chan_Num = 0;
	char chan_list[14] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14};

	if(chan <= 0 || chan > 14){
		wifi_printk(WIFI_DBG_ERROR,"%s:chan(%d) not allow , SCAN all channel \n",__func__,chan);
		//scan_all = 1;
		chan_Num = 14;
	}else{
		memset(chan_list,chan,SCAN_CHANNEL_COUNT);
		chan_Num = SCAN_CHANNEL_COUNT;
	}
	if(special_ie && (special_ie_len > 0)){

		//ie_len = strlen(ie);

		if(special_ie_len > 257){
			wifi_printk(WIFI_DBG_ERROR,"%s:ie_len lager 255 , ie_len = %d\n",__func__,special_ie_len);
			return -1;
		}

		ie_buf = (char *)atbm_kmalloc(special_ie_len,GFP_KERNEL);
		atbm_memset(ie_buf,0,special_ie_len);

		if(priv->special_extra_ie){
			atbm_kfree(priv->special_extra_ie);
		}
		priv->special_extra_ie = ie_buf;
		//priv->special_extra_ie[0] = WLAN_SPECIAL_IE; // ie id
		//priv->special_extra_ie[1] = ie_len;
		atbm_memcpy(priv->special_extra_ie,special_ie,special_ie_len);
		priv->special_extra_ie_len =  special_ie_len;
	}else{
		wifi_printk(WIFI_DBG_ERROR,"%s:ie is NULL,clear private IE \n",__func__);
		if(priv->special_extra_ie){
			atbm_kfree(priv->special_extra_ie);
		}
		priv->special_extra_ie = NULL;
		priv->special_extra_ie_len = 0;
	}

	priv->special_scan = 1;

	return atbm_internal_cmd_scan_triger(priv,NULL,chan_list,chan_Num);

}
#endif

