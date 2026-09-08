/**************************************************************************************************************
 * altobeam RTOS
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 *
 *  The source code contains proprietary information of AltoBeam, and shall not be distributed, 
 *  copied, reproduced, or disclosed in whole or in part without prior written permission of AltoBeam.
*****************************************************************************************************************/

#include "atbm_hal.h"
#include "atbm_ratectrl.h"
extern int g_ProductTestGlobal;

extern struct atbmwifi_common g_hw_prv;
extern void etf_v2_scan_end(struct atbmwifi_common *hw_priv, struct atbmwifi_vif *vif );
extern int wpa_wsc_tx_process(atbm_void *ctx, int type, const atbm_uint8 *buf, atbm_size_t len);
atbm_void atbmwifi_wpa_event_process_scan_end(struct atbmwifi_common *hw_priv,
				atbm_uint32 scan_status,atbm_uint32 interfaceId);
atbm_void sta_scan_start_timer_func(atbm_void *data1,atbm_void *data2);
atbm_void atbmwifi_sta_join_timeout(atbm_void *data1,atbm_void *data2);
atbm_void smartconfig_start_timer_func(atbm_void *arg);
 int smartconfig_magic_scan_done(struct atbmwifi_vif *priv);
atbm_void atbmwifi_stop_smartconfig(struct atbmwifi_vif *priv );
extern atbm_void wpas_wps_timeout(void *eloop_ctx, atbm_void *timeout_ctx);
extern atbm_void ieee80211_ba_session_work(struct atbm_work_struct *work);

atbm_void atbmwifi_enable_sta_filter_retry(struct atbmwifi_vif *priv)
{
	priv->link_id_db[ATBMWIFI__MAX_STA_IN_AP_MODE].status = ATBMWIFI__LINK_HARD;
	atbm_memset(&priv->link_id_db[ATBMWIFI__MAX_STA_IN_AP_MODE].sta_retry,0xff,sizeof(priv->link_id_db[ATBMWIFI__MAX_STA_IN_AP_MODE].sta_retry.last_rx_seq));
	return;
}
atbm_void atbmwifi_disable_sta_filter_retry(struct atbmwifi_vif *priv)
{
	priv->link_id_db[ATBMWIFI__MAX_STA_IN_AP_MODE].status = ATBMWIFI__LINK_OFF;
	atbm_memset(&priv->link_id_db[ATBMWIFI__MAX_STA_IN_AP_MODE].sta_retry,0xff,sizeof(priv->link_id_db[ATBMWIFI__MAX_STA_IN_AP_MODE].sta_retry.last_rx_seq));
	return;
}

/**************************************************************************************************************
 * 函数名: atbm_join_work
 *
 * 功能描述:
 *   Join 工作队列回调函数。该函数在工作队列中执行，负责调用 atbmwifi_join_start() 
 *   发起 Join 请求。使用工作队列可以避免在中断上下文中执行耗时操作。
 *
 * 在连接流程中的位置:
 *   阶段 4: Join 阶段（工作队列入口）
 *   在扫描完成并找到目标 AP 后，通过 atbm_queue_work() 调度执行
 *
 * 参数说明:
 *   @work: 工作队列结构体指针，实际指向 atbmwifi_vif 结构体
 *
 * 返回值:
 *   无
 *
 * 处理流程:
 *   1. 从 work 指针获取 priv 结构体
 *   2. 调用 atbmwifi_join_start() 发起 Join
 *
 * 注意事项:
 *   - 在工作队列上下文中执行，可以睡眠
 *   - Join 完成后会触发认证流程
 *
 * 相关函数:
 *   - atbmwifi_join_start(): 实际的 Join 处理函数
 *   - atbm_queue_work(): 调度此工作
 *
 **************************************************************************************************************/
void atbm_join_work(struct atbm_work_struct *work)
{
	struct atbmwifi_vif *priv=(struct atbmwifi_vif *)work;
	/* 先发送 join，再发送 auth 认证请求 */
	atbmwifi_join_start(priv);
}
int atbm_event_handler(struct atbm_work_struct *work)
{
	return 0;

}
atbm_void atbm_sta_set_tim_work(struct atbm_work_struct *work)
{
	return;
}

/**************************************************************************************************************
 * 函数名: atbmwifi_init_scan_req
 *
 * 功能描述:
 *   初始化扫描请求参数。该函数根据扫描类型配置扫描参数，包括信道列表、扫描时间、
 *   探测请求数量等。支持 2.4G 和 5G 频段的主动/被动扫描。
 *
 * 在连接流程中的位置:
 *   阶段 3: 扫描准备
 *   在 atbm_scan_work() 中调用，准备扫描参数
 *
 * 参数说明:
 *   @priv: 虚拟接口指针，包含接口的配置和状态信息
 *   @type: 扫描类型，参见 ATBM_SCAN_TYPE 枚举:
 *          - ATBM_SCAN_24G_ACTIVE: 2.4G 主动扫描
 *          - ATBM_SCAN_24G_PASSAVE: 2.4G 被动扫描
 *          - ATBM_SCAN_24G_WPS: 2.4G WPS 扫描
 *          - ATBM_SCAN_5G_ACTIVE: 5G 主动扫描
 *          - ATBM_SCAN_5G_PASSAVE: 5G 被动扫描
 *          - ATBM_SCAN_5G_PASSAVE_DFS: 5G DFS 被动扫描
 *          - ATBM_SCAN_P2P: P2P 扫描
 *
 * 返回值:
 *   无
 *
 * 扫描参数配置:
 *   - channels: 扫描信道列表
 *   - n_channels: 信道数量
 *   - numOfProbeRequests: 每个信道发送的 Probe Request 数量
 *   - maxTransmitRate: 发送速率
 *   - minChannelTime: 最小信道停留时间
 *   - maxChannelTime: 最大信道停留时间
 *   - band: 频段（2.4G/5G）
 *   - numOfSSIDs: SSID 数量（主动扫描时为 1）
 *
 * 扫描类型说明:
 *   - 主动扫描: 发送 Probe Request，等待 Probe Response
 *   - 被动扫描: 只监听 Beacon 帧
 *   - WPS 扫描: 用于 WPS 配网
 *   - P2P 扫描: 在 1/6/11 信道扫描
 *
 * 注意事项:
 *   - 5G 扫描需要检查 DFS 信道
 *   - P2P 扫描只在社交信道（1/6/11）进行
 *   - 扫描参数会影响扫描速度和功耗
 *
 * 相关函数:
 *   - atbm_scan_work(): 调用此函数
 *   - wsm_scan(): 发送扫描命令
 *
 **************************************************************************************************************/
atbm_void atbmwifi_init_scan_req(struct atbmwifi_vif *priv, enum ATBM_SCAN_TYPE type){
	struct atbm_scan_req *req = ATBM_NULL;
	struct atbmwifi_common	*hw_priv;
	struct atbmwifi_ieee80211_channel *channels;
	int n_channels = 0, c_channels = 0, r_channels = 0;
	atbm_uint8 numOfProbeRequests, maxTransmitRate,  minChannelTime,  maxChannelTime;
	atbm_uint8 band;
	atbm_uint8 numOfSSIDs = 0;
#if CONFIG_P2P
	struct atbmwifi_ieee80211_channel p2p_scan_chans[3] = {
		{.hw_value = 1, .max_power = 30},
		{.hw_value = 6, .max_power = 30},
		{.hw_value = 11, .max_power = 30}
	};
#endif
#if CONFIG_5G_SUPPORT
	struct atbmwifi_ieee80211_channel avail_5g_channels[30];
#endif
	int i;

	hw_priv = priv->hw_priv;

	switch(type){
		case ATBM_SCAN_24G_ACTIVE:
			if(!priv->scan_no_connect){
				numOfSSIDs = 1;
			}
			atbm_fallthrough;
		case ATBM_SCAN_24G_WPS:
			channels = hw_priv->bands[ATBM_IEEE80211_BAND_2GHZ]->channels;
			n_channels = hw_priv->bands[ATBM_IEEE80211_BAND_2GHZ]->n_channels;
			maxTransmitRate = WSM_TRANSMIT_RATE_1;
			minChannelTime = 45;
			maxChannelTime = 75;
			band = WSM_PHY_BAND_2_4G;
			numOfProbeRequests = 2;
			break;
		case ATBM_SCAN_24G_PASSAVE:
			channels = hw_priv->bands[ATBM_IEEE80211_BAND_2GHZ]->channels;
			n_channels = hw_priv->bands[ATBM_IEEE80211_BAND_2GHZ]->n_channels;
			maxTransmitRate = WSM_TRANSMIT_RATE_1;
			minChannelTime = 120;
			maxChannelTime = 150;
			band = WSM_PHY_BAND_2_4G;
			numOfProbeRequests = 0;
			break;
#if CONFIG_5G_SUPPORT
		case ATBM_SCAN_5G_PASSAVE:
			atbm_fallthrough;
		case ATBM_SCAN_5G_PASSAVE_DFS:
			channels = hw_priv->bands[ATBM_NL80211_BAND_5GHZ]->channels;
			n_channels = hw_priv->bands[ATBM_NL80211_BAND_5GHZ]->n_channels;
			maxTransmitRate = WSM_TRANSMIT_RATE_6;
			minChannelTime = 120;
			maxChannelTime = 150;
			band = WSM_PHY_BAND_5G;
			numOfProbeRequests = 0;
			atbm_memset(priv->scan.dfs_map, 0, sizeof(priv->scan.dfs_map));
			break;
		case ATBM_SCAN_5G_ACTIVE:
			if(!priv->scan_no_connect){
				numOfSSIDs = 1;
			}
			atbm_fallthrough;
		case ATBM_SCAN_5G_WPS:
			channels = hw_priv->bands[ATBM_NL80211_BAND_5GHZ]->channels;
			n_channels = hw_priv->bands[ATBM_NL80211_BAND_5GHZ]->n_channels;
			maxTransmitRate = WSM_TRANSMIT_RATE_6;
			minChannelTime = 45;
			maxChannelTime = 75;
			band = WSM_PHY_BAND_5G;
			numOfProbeRequests = 2;
			break;
		case ATBM_SCAN_5G_ACTIVE_DFS:
			if(!priv->scan_no_connect){
				numOfSSIDs = 1;
			}
			n_channels = 0;
			for(i = 0; i < hw_priv->bands[ATBM_NL80211_BAND_5GHZ]->n_channels; i++){
				if(atbm_test_bit(hw_priv->bands[ATBM_NL80211_BAND_5GHZ]->channels[i].hw_value,
						priv->scan.dfs_map)){
					avail_5g_channels[n_channels].hw_value = hw_priv->bands[ATBM_NL80211_BAND_5GHZ]->channels[i].hw_value;
					avail_5g_channels[n_channels].max_power = hw_priv->bands[ATBM_NL80211_BAND_5GHZ]->channels[i].max_power;
					n_channels++;
				}
			}
			channels = &avail_5g_channels[0];
			maxTransmitRate = WSM_TRANSMIT_RATE_6;
			minChannelTime = 45;
			maxChannelTime = 75;
			band = WSM_PHY_BAND_5G;
			numOfProbeRequests = 2;
			break;
#endif
#if CONFIG_P2P
		case ATBM_SCAN_P2P:
			numOfSSIDs = 1;
			channels = p2p_scan_chans;
			n_channels = 3;
			maxTransmitRate = WSM_TRANSMIT_RATE_1;
			minChannelTime = 45;
			maxChannelTime = 75;
			band = WSM_PHY_BAND_2_4G;
			numOfProbeRequests = 2;
			break;
#endif
		default:
			wifi_printk(WIFI_ALWAYS, "Unknown Scan type %d\n", type);
			return;
	}


	do{
		if(n_channels == 0){
			break;
		}
		r_channels =  (n_channels - c_channels) > ATBM_MAX_SCAN_CHANNELS ? ATBM_MAX_SCAN_CHANNELS : (n_channels - c_channels);
		wifi_printk(WIFI_DBG_MSG, "ch %d %d %d\n", n_channels, c_channels, r_channels);
		req = (struct atbm_scan_req *)atbm_kzalloc(sizeof(struct atbm_scan_req) + numOfSSIDs*sizeof(struct wsm_ssid) + n_channels*sizeof(struct wsm_scan_ch), GFP_KERNEL);
		if(req == NULL){
			wifi_printk(WIFI_DBG_MSG, "%s:req is NULL\n",__func__);
			return;
		}
		memset(req,0,sizeof(struct atbm_scan_req) + numOfSSIDs*sizeof(struct wsm_ssid) + n_channels*sizeof(struct wsm_scan_ch));
		req->scan.ssids = (struct wsm_ssid *)(req + 1);
		req->scan.ch = (struct wsm_scan_ch *)(req->scan.ssids + numOfSSIDs);

		req->scan.scanType = WSM_SCAN_TYPE_FOREGROUND;
		req->scan.scanFlags =0;
		req->scan.numOfProbeRequests = numOfProbeRequests;
		req->scan.probeDelay = 10;
		req->scan.maxTransmitRate = maxTransmitRate;
		req->scan.band =  band;
		if (priv->join_status == ATBMWIFI__JOIN_STATUS_STA) {
			req->scan.scanType = WSM_SCAN_TYPE_BACKGROUND;
			req->scan.scanFlags = WSM_SCAN_FLAG_FORCE_BACKGROUND;
		}
		if(priv->iftype == ATBM_NL80211_IFTYPE_AP){
			req->scan.scanFlags |= WSM_FLAG_AP_BEST_CHANNEL;
		}
		if (priv->if_id){
			req->scan.scanFlags |= WSM_FLAG_MAC_INSTANCE_1;
		}else{
			req->scan.scanFlags &= ~WSM_FLAG_MAC_INSTANCE_1;
		}

		if(numOfSSIDs > 0){
			atbm_memcpy(req->scan.ssids->ssid ,&priv->ssid[0], priv->ssid_length);
			req->scan.ssids->length = priv->ssid_length;
		}
		req->scan.numOfSSIDs = numOfSSIDs;

		for(i = 0; i < r_channels; i++){
			req->scan.ch[i].number = channels[c_channels+i].hw_value;
			req->scan.ch[i].txPowerLevel = channels[c_channels+i].max_power;
			req->scan.ch[i].minChannelTime = minChannelTime;
			req->scan.ch[i].maxChannelTime= maxChannelTime;
		}
		req->scan.numOfChannels = r_channels;
		req->req_type = type;
		wifi_printk(WIFI_DBG_MSG, "Wifi add scan req %x type %d ch %d\n", req, req->req_type, req->scan.numOfChannels);
		atbm_list_add_tail(&req->list, &priv->scan.scan_req_list);
		c_channels += r_channels;
	}while(c_channels < n_channels);

	if(req){
		req->scan_end = ATBM_TRUE;
	}
}

int atbm_req_scan_start(struct atbmwifi_vif *priv){
	int ret = -1;
	struct atbm_scan_req *req;
	struct wsm_template_frame frame;

	if(!atbm_list_empty(&priv->scan.scan_req_list)){
		req = atbm_list_first_entry(&priv->scan.scan_req_list, struct atbm_scan_req, list);
		priv->scan.cur_scan_req = req;
		wifi_printk(WIFI_ALWAYS, "Wifi start scan req band %d type %d ch %d\n", req->scan.band, req->req_type, req->scan.numOfChannels);
		frame.frame_type = WSM_FRAME_TYPE_PROBE_REQUEST;	
		frame.disable =0;
		frame.rate=0;
		frame.skb = atbmwifi_ieee80211_send_probe_req(priv,ATBM_NULL,priv->extra_ie,priv->extra_ie_len,0);
		
		if (!frame.skb)
			return -ATBM_ENOMEM;

		ret = wsm_set_template_frame(priv->hw_priv, &frame,
				priv->if_id);

		atbm_dev_kfree_skb(frame.skb);

		if(ret){
			wifi_printk(WIFI_SCAN,"%s fail \n",__FUNCTION__);
			return ret;
		}

		ret = wsm_scan(priv->hw_priv, &req->scan, priv->if_id);

		if(ret){
			wifi_printk(WIFI_SCAN,"%s fail \n",__FUNCTION__);
			//add by wp ,scan fail
			priv->scan.in_progress = 0;
			priv->scan.ApScan_in_process = 0;
			//atbmwifi_wpa_event_queue((atbm_void*)hw_priv,(atbm_void*)1,(atbm_void*)priv->if_id,
			//					WPA_EVENT__SUPPLICANT_SCAN_END,ATBM_WPA_EVENT_NOACK);
		}
		
		return ret;
	}else{
		priv->scan.cur_scan_req = ATBM_NULL;
	}
	return ret;
}

void atbm_clear_scan_req(struct atbmwifi_vif *priv){
	struct atbm_scan_req *req, *tmp;

	atbm_list_for_each_entry_safe(req, tmp, &priv->scan.scan_req_list, list, struct atbm_scan_req){
		atbm_list_del(&req->list);
		atbm_kfree(req);
	}
	priv->scan.cur_scan_req = ATBM_NULL;
	atbm_memset(priv->scan.dfs_map, 0, sizeof(priv->scan.dfs_map));
}

int atbmwifi_scan_start(struct atbmwifi_vif *priv){
	int ret = -1;
#if CONFIG_WPS
	struct wpa_supplicant *wpa_s = (struct wpa_supplicant *)priv->appdata;
#endif

	if(priv->scan.in_progress)
		return -1;

	priv->scan.status = 0;
	priv->scan.if_id = priv->if_id;
	priv->scan.in_progress = 1;

#if CONFIG_P2P
	if(priv->p2p_scan){
		atbmwifi_init_scan_req(priv, ATBM_SCAN_P2P);
		goto scan_start;
	}
#endif

#if CONFIG_WPS
	if(wpa_s->wps_mode != WPS_MODE_UNKNOWN){
		atbmwifi_init_scan_req(priv, ATBM_SCAN_24G_WPS);
#if CONFIG_5G_SUPPORT
		if(atbmwifi_5g_supported(priv->hw_priv->chip_priv)){
			atbmwifi_init_scan_req(priv, ATBM_SCAN_5G_WPS);
		}
#endif
		goto scan_start;
	}
#endif
#if FAST_CONNECT_MODE
	if(priv->fast_connect && priv->fast_channel){
		atbmwifi_init_scan_req(priv, ATBM_SCAN_FAST_LINK);
		priv->fast_connect = 0;
		goto scan_start;
	}
#endif

	atbmwifi_init_scan_req(priv, ATBM_SCAN_24G_ACTIVE);
#if CONFIG_5G_SUPPORT
	if(atbmwifi_5g_supported(priv->hw_priv->chip_priv)){
		atbmwifi_init_scan_req(priv, ATBM_SCAN_5G_PASSAVE_DFS);
	}
#endif

scan_start:
	ret = atbm_req_scan_start(priv);
	if(ret){
		return ret;
	}
	atbmwifi_event_uplayer(priv,ATBM_WIFI_SCANSTART_EVENT,0);
	return ret;

}

 int atbmwifi_scan_start_etf(struct atbmwifi_vif *priv)
{
	int ret;
	struct atbmwifi_common *hw_priv = _atbmwifi_vifpriv_to_hwpriv(priv);
	struct wsm_ssid ssids; 
	struct wsm_scan_ch	ch[2];	
	struct wsm_scan scan = {
		.scanType = WSM_SCAN_TYPE_FOREGROUND,
		.scanFlags =0,
		.numOfProbeRequests = 1,
		.probeDelay = 0,
		.numOfChannels = atbmwifi_band_2ghz.n_channels, 	
	};
	
	priv->scan.status = 0;
	priv->scan.if_id = priv->if_id;
	priv->scan.in_progress = 1;
	
	scan.maxTransmitRate = hw_priv->etf_rate;
	scan.band =  WSM_PHY_BAND_2_4G;
	if (priv->join_status == ATBMWIFI__JOIN_STATUS_STA) {
		scan.scanType = WSM_SCAN_TYPE_BACKGROUND;
		scan.scanFlags = WSM_SCAN_FLAG_FORCE_BACKGROUND;
		if (priv->if_id)
			scan.scanFlags |= WSM_FLAG_MAC_INSTANCE_1;
		else
			scan.scanFlags &= ~WSM_FLAG_MAC_INSTANCE_1;
	}
	scan.autoScanInterval = (0xba << 24)|(120 * 1024); /* 30 seconds, -70 rssi */
	scan.numOfProbeRequests = 50;
	scan.numOfChannels =1;
	scan.numOfSSIDs = 1;
	scan.probeDelay = 5;
	scan.scanFlags = 0; /* bit 0 set => forced background scan */
	scan.scanType =WSM_SCAN_TYPE_FOREGROUND;


	scan.ssids = &ssids;
	scan.ssids->length = 0;
	atbm_memcpy(ssids.ssid,"tttttttt",8);
	scan.ssids = &ssids;
	if(priv->scan_no_connect){
		scan.ssids->length = 0;
	}

	scan.ch = &ch[0];
	scan.ch[0].number = hw_priv->etf_channel;
	scan.ch[0].minChannelTime= 5;
	scan.ch[0].maxChannelTime= 100;
	scan.ch[0].txPowerLevel= 3;

	ret = wsm_scan(hw_priv, &scan, priv->if_id);
	if(ret){
		wifi_printk(WIFI_SCAN,"%s fail \n",__func__);
	}

	//atbmwifi_event_uplayer(priv,ATBM_WIFI_SCANSTART_EVENT,0);
	return ret;
}

/**************************************************************************************************************
 * 函数名: atbm_scan_work
 *
 * 功能描述:
 *   扫描工作队列回调函数。该函数在工作队列中执行，负责启动实际的扫描操作。
 *   使用工作队列可以避免在中断上下文中执行耗时的扫描操作。
 *
 * 在连接流程中的位置:
 *   阶段 3: 扫描执行
 *   在 atbmwifi_sta_scan() 中通过 atbm_queue_work() 调度执行
 *
 * 参数说明:
 *   @work: 工作队列结构体指针，实际指向 atbmwifi_vif 结构体
 *
 * 返回值:
 *   无
 *
 * 处理流程:
 *   1. 从 work 指针获取 priv 结构体
 *   2. 检查扫描是否已在进行中
 *   3. 根据模式启动扫描：
 *      - 产测模式: 调用 atbmwifi_scan_start_etf()
 *      - 正常模式: 调用 atbmwifi_scan_start()
 *
 * 扫描完成后:
 *   扫描完成后会触发 WPA_EVENT__SUPPLICANT_SCAN_END 事件
 *   如果找到目标 AP，会触发 Join 流程
 *
 * 注意事项:
 *   - 在工作队列上下文中执行，可以睡眠
 *   - 扫描是异步的，结果通过事件通知
 *   - 产测模式下使用特殊的扫描函数
 *
 * 相关函数:
 *   - atbmwifi_sta_scan(): 调度此工作
 *   - atbmwifi_scan_start(): 正常扫描
 *   - atbmwifi_scan_start_etf(): 产测扫描
 *
 **************************************************************************************************************/
 void atbm_scan_work(struct atbm_work_struct *work)
{	
	struct atbmwifi_vif *priv=(struct atbmwifi_vif *)work;
	if(!priv->scan.in_progress)
	{
		if(g_ProductTestGlobal == 1)
		{
			wifi_printk(WIFI_DBG_ERROR,"[%s]:atbmwifi_scan_start_etf \n",__FUNCTION__);
			atbmwifi_scan_start_etf(priv);
		}
		else
		{
			wifi_printk(WIFI_DBG_ERROR,"[%s] atbmwifi_scan_start \n",__FUNCTION__);
			atbmwifi_scan_start(priv);
		}
	}
}

atbm_void	atbmwifi_wpa_event_scan_expire(struct atbmwifi_vif *priv)
{
	//struct atbmwifi_common	*hw_priv=_atbmwifi_vifpriv_to_hwpriv(priv);
	wifi_printk(WIFI_DBG_ERROR,"%s %d\n",__func__,__LINE__);

    //peterjiang@20200518, if station is off, can't do station scan
    if(priv->iftype == ATBM_NUM_NL80211_IFTYPES){
        wifi_printk(WIFI_DBG_ERROR,"%s station is off, can not do scan, cancel scan timer\n",__func__);
        atbmwifi_eloop_cancel_timeout(sta_scan_start_timer_func, (atbm_void *)priv, ATBM_NULL);
        return;
    }
	//ATBM_WARN_ON(wsm_stop_scan(hw_priv,priv->scan.if_id ? 1 : 0));
	if(priv->scan.in_progress){
		priv->scan.status = -ATBM_ETIMEDOUT;
		priv->scan.in_progress = 0;
		wifi_printk(WIFI_DBG_MSG,"atbm: sta_scan_start_timer_func(), timeout\n");
	}
	if(!priv->scan.in_progress){
		atbmwifi_sta_scan(priv);
	}
}


atbm_void sta_scan_start_timer_func(atbm_void *data1,atbm_void *data2)
{
	atbmwifi_wpa_event_queue((atbm_wpa_event_data)data1,(atbm_wpa_event_data)data2,(atbm_wpa_event_data)ATBM_NULL,
				WPA_EVENT__SUPPLICANT_SCAN_EXPIRE,ATBM_WPA_EVENT_NOACK);
}

atbm_void atbmwifi_scan_comlete(struct atbmwifi_vif *priv)
{
	struct atbmwifi_common *hw_priv=_atbmwifi_vifpriv_to_hwpriv(priv);
#if CONFIG_WPS
	struct wpa_supplicant *wpa_s = (struct wpa_supplicant *)priv->appdata;
#endif
	if(priv == ATBM_NULL){
		wifi_printk(WIFI_DBG_ERROR,"atbm: atbmwifi_scan_comlete(), priv is ATBM_NULL.\n");
		atbm_SleepMs(100);
		return;
	}

#if CONFIG_WPS
	if(priv->pin)
		wpa_s->scan_runs++;
#endif
	
	priv->scan.in_progress = 0;
	priv->scan.ApScan_in_process = 0;
	if(!priv->enabled){
		wifi_printk(WIFI_DBG_MSG,"atbm: atbmwifi_scan_comlete(), priv disabled.\n");
		return ;
	}

	if(hw_priv->bStartTx)
	{
		if(hw_priv->bStartTxWantCancel == 0)
		{
			wifi_printk(WIFI_SCAN,"## wsm_start_scan_etf ##\n");
			//wsm_start_scan_etf(hw_priv, priv);
		}
		else
		{
			hw_priv->bStartTx = 0;
			hw_priv->bStartTxWantCancel = 0;
			if(hw_priv->etf_test_v2){
				hw_priv->etf_test_v2 = 0;
				wifi_printk(WIFI_SCAN,"## start timer etf_tx_end_work ##\n");
				etf_v2_scan_end(hw_priv,priv);
			}
		}
		return;
	}

#if CONFIG_P2P
	if(priv->p2p_scan)
		p2p_scan_result();
#endif
   // atbm_CancelTimer(&priv->scan_expire_timer);
   atbmwifi_eloop_cancel_timeout(sta_scan_start_timer_func, (atbm_void *)priv, ATBM_NULL);
	if(priv->scan.status != ATBMWIFI_SCAN_CONNECT_AP_SUCCESS){
		wifi_printk(WIFI_SCAN,"scan fail again [%d]s, scan_no_connect=%d\n",priv->scan_expire, priv->scan_no_connect);
		if(priv->scan_no_connect){
			goto __end;
		}	
#if CONFIG_P2P
		if(priv->p2p_scan || priv->p2p_join){
			atbmwifi_eloop_register_timeout(0, 200, sta_scan_start_timer_func,(atbm_void *)priv,ATBM_NULL);
		}else
#endif
		{
	        //peterjiang@20200518, increase scan expire
			priv->scan_expire+=2;
			if(priv->scan_expire > 60){
				priv->scan_expire = 60;
			}
			atbmwifi_eloop_register_timeout(0,priv->scan_expire*1000,sta_scan_start_timer_func,(atbm_void *)priv,ATBM_NULL);
		}
	}
	else {
		if(priv->scan_no_connect == 0){
			//if the first scan success,
			if(priv->connect_expire==0){
				priv->scan_expire = 5;
				priv->connect_expire = 1;
			}
			else {
				if(priv->scan_expire < 60){
					priv->scan_expire += 5;
				}
				else {
					priv->scan_expire = INITIAL_SCAN_EXPIRE;
				}
			}
			wifi_printk(WIFI_SCAN|WIFI_CONNECT,"sta_join_work  \n");
#if CONFIG_WPS
			if(wpa_s->wps_mode != WPS_MODE_UNKNOWN){
				wifi_printk(WIFI_WPS|WIFI_CONNECT, "WPS: scan end\n");
				if(wpa_s->wps_ap_cnt == 0){
					wifi_printk(WIFI_WPS, "WPS: not found AP.scan again\n");
                    //peterjiang@20200424, wps ap is not found, we need to do scan again.
                    atbmwifi_eloop_register_timeout(0,priv->scan_expire*1000,sta_scan_start_timer_func,(atbm_void *)priv,ATBM_NULL);
					goto __end;
				}

				if(wpa_s->wps_ap_cnt > 1){
					wifi_printk(WIFI_WPS, "WPS: overlap(%d)\n", wpa_s->wps_ap_cnt);
					atbmwps_cancel(priv);
					goto __end;
				}
			}
#endif
		//	atbmwifi_event_uplayer(priv,ATBM_WIFI_JOIN_EVENT,0);
			atbm_queue_work(hw_priv,priv->join_work);	
		}
	}
__end:
	atbmwifi_event_uplayer(priv,ATBM_WIFI_SCANDONE_EVENT,0);
	priv->scan_no_connect=priv->scan_no_connect_back;
#if ATBM_SUPPORT_SMARTCONFIG
	if(priv->scan.scan_smartconfig){
		wifi_printk(WIFI_DBG_MSG,"atbm: atbmwifi_scan_comlete() ssc.\n");
		smartconfig_magic_scan_done(priv);
	}
#endif
	wifi_printk(WIFI_DBG_MSG,"atbm: atbmwifi_scan_comlete() <===\n");

	return;
}

static atbm_void __atbmwifi_autoconnect(atbm_void *data1,atbm_void *data2)
{
	struct atbmwifi_vif *priv = (struct atbmwifi_vif *)data1;
	if(priv->scan.scan_smartconfig){
		return;
	}

	priv->auto_connect_when_lost = 0;
	wifi_printk(WIFI_ALWAYS,"autoconnect\n");

	priv->scan_no_connect = 0;

	if(priv->scan_expire < 60){
		priv->scan_expire += 5;
	}
	else {
		priv->scan_expire = INITIAL_SCAN_EXPIRE;
	}

	priv->assoc_ok = 0; 
	wpa_connect_ap(priv);
}

int atbmwifi_autoconnect(struct atbmwifi_vif *priv, int time)
{
   // atbmwifi_event_uplayer(priv,ATBM_WIFI_PRE_ASSOC_EVENT,0);
	atbmwifi_wpa_event_queue((atbm_wpa_event_data)priv,(atbm_wpa_event_data)time,(atbm_wpa_event_data)ATBM_NULL,WPA_EVENT__SUPPLICANT_CONNECT_FAIL,ATBM_WPA_EVENT_NOACK);
	return 0;
}

 void atbmwifi_wpa_event_connect_fail(struct atbmwifi_vif *priv, int time)
{
	atbmwifi_eloop_cancel_timeout(__atbmwifi_autoconnect, priv, ATBM_NULL);
	atbmwifi_eloop_register_timeout(time, 0,__atbmwifi_autoconnect, priv, ATBM_NULL);
}

 atbm_void atbmwifi_wpa_event_process_scan_end(struct atbmwifi_common *hw_priv,
				atbm_uint32 scan_status,atbm_uint32 interfaceId)
{
	struct atbm_scan_req * req;
	struct atbmwifi_vif *priv = ATBM_NULL;
	priv = _atbmwifi_hwpriv_to_vifpriv(hw_priv,interfaceId);
	if(priv == ATBM_NULL){
		wifi_printk(WIFI_WPA,"scan_end priv err\n");
		return;
	}
	if(scan_status == 0){
		wifi_printk(WIFI_WPA,"scan timeout\n");
		ATBM_WARN_ON(wsm_stop_scan(hw_priv,priv->scan.if_id ? 1 : 0));
		if(priv->scan.in_progress){
			priv->scan.status = -ATBM_ETIMEDOUT;
			wifi_printk(WIFI_DBG_MSG,"atbm: sta_scan_start_timer_func(), timeout\n");
		}
	}

	if(scan_status == 1){
		if(!priv->scan_no_connect && priv->scan.status == ATBMWIFI_SCAN_CONNECT_AP_SUCCESS){
			atbm_clear_scan_req(priv);
			atbmwifi_scan_comlete(priv);
		}else if(priv->scan.cur_scan_req){
			atbm_list_del(&priv->scan.cur_scan_req->list);
			if(priv->scan.cur_scan_req->req_type == ATBM_SCAN_5G_PASSAVE_DFS
				&& priv->scan.cur_scan_req->scan_end){
				atbmwifi_init_scan_req(priv, ATBM_SCAN_5G_ACTIVE_DFS);
			}
			atbm_kfree(priv->scan.cur_scan_req);
			if(atbm_req_scan_start(priv)){
				atbmwifi_scan_comlete(priv);
			}
		}else{
			atbmwifi_scan_comlete(priv);
		}
	}

	if(scan_status == 0){
		priv->scan_expire+=3;
		if(priv->scan_expire > 30){
			priv->scan_expire = 10;
		}
		if(!priv->scan.in_progress)
			atbmwifi_sta_scan(priv);
	}

	return;
}

 atbm_void atbmwifi_scan_complete_cb(struct atbmwifi_common *hw_priv,
				int interface_link_id,struct wsm_scan_complete *arg)
{
	atbmwifi_wpa_event_queue((atbm_wpa_event_data)hw_priv,(atbm_wpa_event_data)1,(atbm_wpa_event_data)interface_link_id,
							WPA_EVENT__SUPPLICANT_SCAN_END,ATBM_WPA_EVENT_NOACK);
}

 int atbmwifi_sta_scan(struct atbmwifi_vif *priv)
{	
	int ret = 0;
	struct atbmwifi_common *hw_priv = _atbmwifi_vifpriv_to_hwpriv(priv);
	
	if (priv->join_status == ATBMWIFI__JOIN_STATUS_AP)
		return -ATBM_EOPNOTSUPP;

	priv->scan.if_id = priv->if_id;
	
	atbm_queue_work(hw_priv,priv->scan.scan_work);

	return ret;
}
 atbm_void atbmwifi_join_complete(struct atbmwifi_vif *priv)
{
#if CONFIG_SAE
	struct wpa_supplicant *wpa_s = (struct wpa_supplicant *)priv->appdata;
#endif

	if(!priv->enabled){
		return ;
	}
#if CONFIG_SAE
	wpa_s->sae_start = 1;
#endif
	wifi_printk(WIFI_ALWAYS,"sta_join_ ATBM_IEEE80211_STYPE_AUTH\n");

	atbmwifi_enable_sta_filter_retry(priv);
	wpa_prepare_auth(priv);

	return;
}

 atbm_uint32 atbmwifi_rate_mask_to_wsm(struct atbmwifi_common *hw_priv,
 				enum atbmwifi_ieee80211_band band, atbm_uint32 rates)
{
	atbm_uint32 ret = 0;
	int i;
	struct atbmwifi_ieee80211_rate * bitrates =
		hw_priv->bands[band]->bitrates;
	for (i = 0; i < 32; ++i) {
		if (rates & ATBM_BIT(i))
			ret |= ATBM_BIT(bitrates[i].hw_value);
	}
	return ret;
}

/**************************************************************************************************************
 * 函数名: atbmwifi_join_start
 *
 * 功能描述:
 *   发起 Join 请求，加入目标 BSS。该函数是连接流程中的关键步骤，在扫描找到目标 AP 后调用，
 *   向硬件发送 Join 命令，开始与 AP 的同步过程。
 *
 * 在连接流程中的位置:
 *   阶段 4: Join 阶段
 *   在扫描完成并找到目标 AP 后调用（通过 atbm_join_work 工作队列）
 *
 * 参数说明:
 *   @priv: 虚拟接口指针，包含接口的配置和状态信息
 *
 * 返回值:
 *   无
 *
 * 处理流程:
 *   1. 组合检查：检查 STA 和 AP 是否在同一信道（双接口模式）
 *   2. 设置操作模式：配置电源模式
 *   3. 初始化 Join 参数：
 *      - mode: BSS 模式
 *      - preambleType: 短前导码
 *      - probeForJoin: Join 前发送 Probe
 *      - dtimPeriod: DTIM 周期
 *      - beaconInterval: Beacon 间隔
 *      - channelNumber: 信道号
 *      - channel_type: 信道类型（20/40MHz）
 *      - band: 频段（2.4G/5G）
 *      - basicRateSet: 基本速率集
 *      - bssid: 目标 AP 的 BSSID
 *      - ssid: 目标 AP 的 SSID
 *   4. 设置 Block ACK 策略
 *   5. 发送 WSM Join 命令到硬件
 *   6. 更新 Join 状态为 ATBMWIFI__JOIN_STATUS_STA
 *   7. 发送认证帧（Auth）
 *
 * Join 成功后:
 *   Join 成功后，会发送 Auth 帧开始认证流程
 *   认证成功后，会发送 Assoc 帧进行关联
 *
 * 注意事项:
 *   - Join 是同步操作，会阻塞直到完成
 *   - Join 失败会触发重试或连接失败事件
 *   - 支持 WiFi6 (HE) 和多 BSSID 特性
 *
 * 相关函数:
 *   - atbm_join_work(): 工作队列回调，调用此函数
 *   - wsm_join(): WSM Join 命令
 *   - atbmwifi_ieee80211_tx_mgmt_auth(): 发送 Auth 帧
 *   - atbmwifi_join_complete(): Join 完成回调
 *
 **************************************************************************************************************/
 atbm_void atbmwifi_join_start(struct atbmwifi_vif *priv)
{
	int ret;
	struct atbmwifi_common *hw_priv = _atbmwifi_vifpriv_to_hwpriv(priv);
	struct wsm_operational_mode mode ;
	struct wsm_join join ;
	/*Station & Ap Must be stay the same channle num,otherwise it's be combination fail*/
	if(atbmwifi_iee80211_check_combination(priv,(atbm_uint8)priv->bss.channel_num) == ATBM_FALSE){
		return;
	}
	mode.power_mode = wsm_power_mode_quiescent;
	mode.disableMoreFlagUsage = ATBM_TRUE;

	atbm_memset(&join, 0, sizeof(join));
	
	join.mode = WSM_JOIN_MODE_BSS;
	join.preambleType = WSM_JOIN_PREAMBLE_SHORT;
	/*Do Scan one time Before join Bss*/
	join.probeForJoin = 1;
	/* dtimPeriod will be updated after association */
	join.dtimPeriod = 1;
	join.beaconInterval = priv->bss.sta_priv.beacon_interval;
	join.channelNumber = priv->bss.channel_num;
	join.channel_type= priv->bss.channel_type;
	priv->config.channel_index = priv->bss.channel_num;
	if (priv->if_id)
		join.flags |= WSM_FLAG_MAC_INSTANCE_1;
	else
		join.flags &= ~WSM_FLAG_MAC_INSTANCE_1;

	join.flags |= WSM_FLAG_JOIN_F_FORCE_JOIN;
	join.band = priv->bss.sta_priv.band == ATBM_IEEE80211_BAND_5GHZ ? WSM_PHY_BAND_5G : WSM_PHY_BAND_2_4G;
	join.basicRateSet = atbm_cpu_to_le32(atbmwifi_rate_mask_to_wsm(hw_priv, priv->bss.sta_priv.band, 
		priv->bss.sta_priv.rate.basic_rates));

#if CONFIG_HE
	if(priv->hw_priv->chip_version == OCEANUS_NO_WIFI6){
		atbm_memcpy(&join.transbssid[0], priv->bss.bssid, sizeof(join.transbssid));
		join.bssid_index = 0;
		join.max_bssid_index = priv->bss.max_bssid_indicator;
		join.not_support_multi_bssid = 1;
		join.is_trans_bssid = 1;
		join.wifi4_option = (priv->bss.sta_priv.he_used == ATBM_TRUE);
	}else
		if(priv->bss.nontransmit){
			atbm_memcpy(join.transbssid,priv->bss.transbssid, sizeof(join.transbssid));
			join.bssid_index = priv->bss.bssid_index;
			join.max_bssid_index = priv->bss.max_bssid_indicator;
			join.wifi4_option = 0;
			join.not_support_multi_bssid = 0;
			join.is_trans_bssid = 0;
		}else
#endif
		{
			atbm_memcpy(&join.transbssid[0], priv->bss.bssid, sizeof(join.transbssid));
			join.bssid_index = 0;
			join.max_bssid_index = priv->bss.max_bssid_indicator;
			join.not_support_multi_bssid = 1;
			join.is_trans_bssid = 1;
			join.wifi4_option = (priv->bss.sta_priv.he_used == ATBM_TRUE);
		}

	atbm_memcpy(&join.bssid[0], priv->bssid, sizeof(priv->bssid));
	//wifi_printk(WIFI_ALWAYS, "nontrans:"MACSTR" trans:"MACSTR"\n", MAC2STR(join.bssid), MAC2STR(join.transbssid));
	join.ssidLength = priv->ssid_length;
	atbm_memcpy(&join.ssid[0], priv->ssid, join.ssidLength);
	wsm_set_operational_mode(hw_priv, &mode, priv->if_id);
#if ATBM_DRIVER_PROCESS_BA
	if(!hw_priv->driver_setup_ba)
#endif
	{
		wsm_set_block_ack_policy(hw_priv,
				ATBMWIFI__TX_BLOCK_ACK_ENABLED_FOR_ALL_TID,
				ATBMWIFI__RX_BLOCK_ACK_ENABLED_FOR_ALL_TID,
				priv->if_id);
	}
#if FAST_CONNECT_NO_SCAN
	if(priv->fast_conn_noscan){
		join.probeForJoin = 0;
		priv->fast_conn_noscan = 0;
	}
#endif
	wifi_printk(WIFI_CONNECT,"wsm_join\n");
	ret = wsm_join(hw_priv, &join, priv->if_id);

	if(ret == 0){
#if 0 //FAST_CONNECT_NO_SCAN
/*Some AP need disconn before auth while some may cause other problems*/
		if(priv->fast_conn_noscan){
//			atbmwifi_ieee80211_tx_mgmt_deauth(priv,priv->bss.bssid,priv->bss.bssid,ATBM_WLAN_REASON_DEAUTH_LEAVING);
			priv->fast_conn_noscan = 0;
		}
#endif
		priv->join_status = ATBMWIFI__JOIN_STATUS_STA;
		priv->disable_beacon_filter = ATBM_TRUE;
		atbmwifi_join_complete(priv);
		//start connect timeout
		atbmwifi_eloop_register_timeout(0,ATBM_WIFI_AUTH_TIMEOUT,atbmwifi_sta_join_timeout,(atbm_void *)priv,ATBM_NULL);

	}else{
        wifi_printk(WIFI_ALWAYS,"wsm_join cmd send fail %d\n", ret);
	}
	//atbmwifi_update_filtering(priv);
	return;
}

 atbm_void atbmwifi_sta_join_timeout(atbm_void *data1,atbm_void *data2)
{	
	struct atbmwifi_vif *priv=(struct atbmwifi_vif *)data1;
	wifi_printk(WIFI_DBG_ERROR,"atbmwifi_sta_join_timeout() \n");
	atbmwifi_wpa_event_queue((atbm_wpa_event_data)data1,(atbm_wpa_event_data)data2,(atbm_wpa_event_data)ATBM_NULL,
				WPA_EVENT__CONNECT_TIMEOUT,ATBM_WPA_EVENT_NOACK);
}

 int atbmwifi_rx_assoc_rsp(struct atbmwifi_vif *priv,struct atbm_buff *skb)
{
	struct atbmwifi_ieee80211_mgmt *mgmt;
	atbm_uint16 capab_info, status_code, aid;
	struct atbmwifi_ieee802_11_elems elems;
#if (CONFIG_WPS || CONFIG_SAE) 
	struct wpa_supplicant *wpa_s = (struct wpa_supplicant *)priv->appdata;
#endif
	atbm_uint8 *pos;
	mgmt = (struct atbmwifi_ieee80211_mgmt *) ATBM_OS_SKB_DATA(skb);
	wifi_printk(WIFI_CONNECT,"atbmwifi_rx_assoc_rsp\n");

	capab_info = atbm_le16_to_cpu(mgmt->u.assoc_resp.capab_info);
	status_code = atbm_le16_to_cpu(mgmt->u.assoc_resp.status_code);
	aid = atbm_le16_to_cpu(mgmt->u.assoc_resp.aid);
	pos = mgmt->u.assoc_resp.variable;
	atbm_ieee802_11_parse_elems(pos, skb->dlen - (pos - (atbm_uint8 *) mgmt), &elems);
	if (status_code == ATBM_WLAN_STATUS_ASSOC_REJECTED_TEMPORARILY &&
			    elems.timeout_int && elems.timeout_int_len == 5 &&
			    elems.timeout_int[0] == ATBM_WLAN_TIMEOUT_ASSOC_COMEBACK) {
		atbm_uint32 tu, ms;
		tu = get_unaligned_le32(elems.timeout_int + 1);
		ms = tu * 1024 / 1000;
		wifi_printk(WIFI_CONNECT,"%pM rejected association temporarily; "
		       "comeback duration %u TU (%u ms)\n", mgmt->sa, tu, ms);
		if (ms > 5)
			//
		return 0;
	}
	if (status_code != ATBM_WLAN_STATUS_SUCCESS){
#if CONFIG_SAE
		if (wpa_s->sae_pmksa_caching && atbmwifi_wpa_key_mgmt_sae(wpa_s->key_mgmt)){
			wpa_printf(MSG_DEBUG,
			"PMKSA caching attempt rejected - drop PMKSA cache entry and fall back to SAE authentication");
			if (wpa_s->wpa && wpa_s->wpa->cur_pmksa) {
				wpa_printf(MSG_DEBUG,
					"RSN: Cancelling PMKSA caching attempt");
				wpa_s->wpa->cur_pmksa = ATBM_NULL;
				atbm_pmksa_cache_flush(wpa_s->wpa->pmksa, &priv->config, ATBM_NULL, 0);
			}
		}
#endif
		wifi_printk(WIFI_CONNECT,"%pM denied association (code=%d)\n", mgmt->sa, status_code);
		return -1;
	}
#if CONFIG_WPS
	else{
		atbm_memcpy(wpa_s->bssid, mgmt->bssid, ATBM_ETH_ALEN);
	}
#endif
	return 0;
}

#if CONFIG_HE
static atbm_uint8 atbm_he_get_ppe_val(atbm_uint8 *ppe, atbm_uint8 ppe_pos_bit)
{
	atbm_uint8 byte_num = ppe_pos_bit / 8;
	atbm_uint8 bit_num = ppe_pos_bit % 8;
	atbm_uint8 residue_bits;
	atbm_uint8 res;

	if (bit_num <= 5)
		return (ppe[byte_num] >> bit_num) &(ATBM_BIT(ATBM_IEEE80211_PPE_THRES_INFO_PPET_SIZE) - 1);

	/*
	 * If bit_num > 5, we have to combine bits with next byte.
	 * Calculate how many bits we need to take from current byte (called
	 * here "residue_bits"), and add them to bits from next byte.
	 */

	residue_bits = 8 - bit_num;

	res = (ppe[byte_num + 1] &(ATBM_BIT(ATBM_IEEE80211_PPE_THRES_INFO_PPET_SIZE - residue_bits) - 1)) << residue_bits;
	res += (ppe[byte_num] >> bit_num) & (ATBM_BIT(residue_bits) - 1);

	return res;
}
#endif

/**************************************************************************************************************
 * 函数名: atbmwifi_assoc_success
 *
 * 功能描述:
 *   处理关联成功响应（Association Response）。该函数在收到 AP 的 AssocRsp 帧后调用，
 *   解析响应内容，配置硬件参数，完成关联过程。
 *
 * 在连接流程中的位置:
 *   阶段 6: 关联完成
 *   在收到 AssocRsp 帧后调用，完成关联配置
 *
 * 参数说明:
 *   @priv: 虚拟接口指针，包含接口的配置和状态信息
 *   @skb:  包含 AssocRsp 帧的 Socket Buffer
 *
 * 返回值:
 *   0:  成功
 *   -1: 失败
 *
 * 处理流程:
 *   1. 解析 AssocRsp 帧：
 *      - capab_info: 能力信息
 *      - status_code: 状态码（0 表示成功）
 *      - aid: 关联 ID
 *   2. 解析 IE（Information Elements）：
 *      - 支持的速率
 *      - 扩展支持的速率
 *      - HT 能力
 *      - HT 操作信息
 *      - VHT 能力（如果支持）
 *      - HE 能力（WiFi6）
 *   3. 配置速率信息：
 *      - 基本速率
 *      - 支持速率
 *      - HT/VHT/HE 速率
 *   4. 配置 BSS 参数：
 *      - Beacon 间隔
 *      - DTIM 周期
 *      - 短前导码
 *      - 短时隙
 *   5. 配置关联模式：
 *      - 前导码类型
 *      - 基本速率集
 *      - 短时隙
 *   6. 初始化速率控制
 *   7. 设置 assoc_ok 标志
 *   8. 触发 4 次握手（如果需要加密）
 *
 * 关联成功后:
 *   - 如果是加密网络，开始 4 次握手
 *   - 如果是开放网络，直接通知连接成功
 *
 * 注意事项:
 *   - 函数会修改 priv->bss 中的配置
 *   - 速率控制需要在关联成功后初始化
 *   - HT/VHT/HE 能力需要根据 AP 的能力进行协商
 *
 * 相关函数:
 *   - atbmwifi_rx_assoc_rsp(): 调用此函数
 *   - wpa_sm_rx_eapol(): 4 次握手处理
 *   - rate_control_alloc_sta(): 速率控制初始化
 *
 **************************************************************************************************************/
 int atbmwifi_assoc_success(struct atbmwifi_vif *priv,struct atbm_buff *skb)
{	
	struct atbmwifi_ieee80211_mgmt *mgmt;
	struct atbmwifi_common *hw_priv = _atbmwifi_vifpriv_to_hwpriv(priv);	
	struct atbmwifi_cfg *config = atbmwifi_get_config(priv);
	struct wsm_association_mode	association_mode;
	struct wsm_set_bss_params	bss_params;

#if CONFIG_WPS
	struct wpa_supplicant *wpa_s = (struct wpa_supplicant *)priv->appdata;
#endif
	struct atbmwifi_ieee802_11_elems elems;
	struct atbmwifi_ieee80211_supported_band *sband;
	atbm_uint16 capab_info, status_code, aid;
	int ret = 0;
	atbm_uint8 *pos;
	atbm_uint32 rates, basic_rates;
	int i,j;
	ATBM_BOOL have_higher_than_11mbit = ATBM_FALSE;
	int min_rate = 30, min_rate_index = -1;
	atbm_uint16 ap_ht_cap_flags;

	mgmt =(struct atbmwifi_ieee80211_mgmt *)ATBM_OS_SKB_DATA(skb);
	capab_info = atbm_le16_to_cpu(mgmt->u.assoc_resp.capab_info);
	status_code = atbm_le16_to_cpu(mgmt->u.assoc_resp.status_code);
	aid = atbm_le16_to_cpu(mgmt->u.assoc_resp.aid);
	priv->bss.aid = aid;
	wifi_printk(WIFI_CONNECT, "RX AssocRsp (capab=0x%x "
	     "status=%d aid=%d)\n", 
	       capab_info, status_code, (atbm_uint16)(aid & ~(ATBM_BIT(15) | ATBM_BIT(14))));
	

	pos = mgmt->u.assoc_resp.variable;
	atbm_ieee802_11_parse_elems(pos, skb->dlen - (pos - (atbm_uint8 *) mgmt), &elems);
	sband = hw_priv->bands[priv->bss.sta_priv.band];
	priv->bss.sta_priv.priv = priv;
	rates = 0;
	basic_rates = 0;
	atbm_memset(&priv->bss.sta_priv.rate, 0, sizeof(struct atbmwifi_cfg80211_rate));
	atbm_memset(&bss_params, 0, sizeof(bss_params));

	atbmwifi_ieee80211_get_sta_rateinfo(&priv->bss.sta_priv.rate,
			sband, elems.supp_rates, elems.supp_rates_len);
	atbmwifi_ieee80211_get_sta_rateinfo(&priv->bss.sta_priv.rate,
			sband, elems.ext_supp_rates, elems.ext_supp_rates_len);

	if(priv->bss.sta_priv.rate.support_rates & (~0xf))
		have_higher_than_11mbit = ATBM_TRUE;

	/* cf. IEEE 802.11 9.2.12 */
	if (sband->band == ATBM_IEEE80211_BAND_2GHZ &&
	    have_higher_than_11mbit)
		priv->bss.sta_priv.rate_11g=ATBM_TRUE;

	if (elems.ht_cap_elem && !(config->flags & ATBM_IEEE80211_STA_DISABLE_11N)){
		atbmwifi_ieee80211_ht_cap_ie_to_sta_ht_cap(sband,
				elems.ht_cap_elem, &priv->bss.sta_priv.rate.ht_cap);
	}
	
	ap_ht_cap_flags = priv->bss.sta_priv.rate.ht_cap.cap;
	if (elems.ht_info_elem &&
	    !(config->flags  & ATBM_IEEE80211_STA_DISABLE_11N)){
		atbmwifi_ieee80211_enable_ht(sband,
					       elems.ht_info_elem,
					       priv, ap_ht_cap_flags,
					       ATBM_FALSE);
#if BW_40M_SUPPORT
		atbmwifi_iee80211_unify_channel_type(priv, priv->bss.channel_type);
#endif
	}

	priv->bss.sta_priv.rate.ht = atbm_is_ht(priv->bss.channel_type);
	priv->bss.sta_priv.rate.channel_type = priv->bss.channel_type;

	association_mode.greenfieldMode = MODE_11N_MIXED;
	association_mode.flags =
		WSM_ASSOCIATION_MODE_SNOOP_ASSOC_FRAMES |
		WSM_ASSOCIATION_MODE_USE_PREAMBLE_TYPE |
		WSM_ASSOCIATION_MODE_USE_HT_MODE |
		WSM_ASSOCIATION_MODE_USE_BASIC_RATE_SET |
		WSM_ASSOCIATION_MODE_USE_MPDU_START_SPACING;
	association_mode.preambleType =
		priv->bss.sta_priv.short_preamble ?
		WSM_JOIN_PREAMBLE_SHORT :
		WSM_JOIN_PREAMBLE_LONG;
		association_mode.basicRateSet = atbm_cpu_to_le32(
			atbmwifi_rate_mask_to_wsm(hw_priv, priv->bss.sta_priv.band,
			priv->bss.sta_priv.rate.basic_rates));
	association_mode.mpduStartSpacing = 0;

#if CONFIG_HE
	if(priv->hw_priv->chip_version != OCEANUS_NO_WIFI6){
		if(priv->bss.is_wifi6_ap){
			priv->association_mode.flags |= WSM_ASSOCIATION_MODE_USE_SHORT_BA_WINDOW;
		}
	}
#endif
#if CONFIG_VHT
	if(atbmwifi_vht_supported(hw_priv->chip_priv)){
		if (elems.vht_cap_elem){
			atbmwifi_ieee80211_vht_cap_ie_to_sta_vht_cap(sband, elems.vht_cap_elem, &priv->bss.sta_priv.rate.vht_cap);
			priv->bss.sta_priv.vht = 1;
		}
	}
#endif
#if CONFIG_HE
	if(priv->hw_priv->chip_version != OCEANUS_NO_WIFI6){
		if (elems.he_operation && elems.he_cap) {
			priv->bss.is_wifi6_ap = ATBM_TRUE;
			atbmwifi_ieee80211_he_cap_ie_to_sta_he_cap(hw_priv->bands[priv->bss.sta_priv.band],
				elems.he_cap,
				elems.he_cap_len,
				&priv->bss.sta_priv.rate.he_cap);
			priv->bss.sta_priv.he_used = priv->bss.sta_priv.rate.he_cap.has_he;
			priv->bss.sta_priv.rate.he_cap.he_operation.he_oper_params = elems.he_operation->he_oper_params;
		}else{
			priv->bss.sta_priv.he_used = 0;
		}

		if(priv->bss.sta_priv.he_used){
			priv->association_mode.flags |= WSM_ASSOCIATION_MODE_USE_HE_MODE;
			bss_params.he_support_flag = F_HE_SUPPORT_ENABLE;
			bss_params.color= priv->bss.he_bss_color.color;
			bss_params.he_support_flag |= priv->bss.he_bss_color.enabled?F_HE_COLOR_ENABLE:0;			
			
			if (priv->bss.uora_exists) {
				bss_params.he_support_flag |= F_HE_UORA_ENABLE;
				bss_params.uora_ocw_range = priv->bss.uora_ocw_range;
			}
			
			if (priv->bss.nontransmit) {
				bss_params.he_support_flag |= F_HE_MUL_BSSID_ENABLE;
				atbm_memcpy(bss_params.transmitter_bssid,priv->bss.transbssid,6);
				bss_params.max_bssid_indicator = priv->bss.max_bssid_indicator;
				bss_params.bssid_index		 = priv->bss.bssid_index;
				bss_params.ema_ap			 = priv->bss.ema_ap;
				bss_params.profile_periodicity = priv->bss.profile_periodicity;
			}
			
			bss_params.frame_time_rts_th = atbm_cpu_to_le16(priv->bss.frame_time_rts_th);

			if (priv->bss.sta_priv.rate.he_cap.he_cap_elem.mac_cap_info[0] & ATBM_IEEE80211_HE_MAC_CAP0_HTC_HE)
				bss_params.htc_flags |= atbm_cpu_to_le32(F_HTC_SUPPORT_HE_ENABLE);			
			if (priv->bss.sta_priv.rate.he_cap.he_cap_elem.mac_cap_info[2] & ATBM_IEEE80211_HE_MAC_CAP2_BSR)
				bss_params.htc_flags |= atbm_cpu_to_le32(F_HTC_SUPPORT_BSR_ENABLE);
			if (priv->bss.sta_priv.rate.he_cap.he_cap_elem.mac_cap_info[3] & ATBM_IEEE80211_HE_MAC_CAP3_OMI_CONTROL)
				bss_params.htc_flags |= atbm_cpu_to_le32(F_HTC_SUPPORT_OMI_ENABLE);
			if (priv->bss.sta_priv.rate.he_cap.he_cap_elem.mac_cap_info[4] & ATBM_IEEE80211_HE_MAC_CAP4_BQR)
				bss_params.htc_flags |= atbm_cpu_to_le32(F_HTC_SUPPORT_BQR_ENABLE);


			/*
			 * Initialize the PPE thresholds to "None" (7), as described in Table
			 * 9-262ac of 80211.ax/D3.0.
			 */
			atbm_memset(&bss_params.pkt_ext, 7, sizeof(bss_params.pkt_ext));

			/* If PPE Thresholds exist, parse them	*/
			if (priv->bss.sta_priv.rate.he_cap.he_cap_elem.phy_cap_info[6] &
				ATBM_IEEE80211_HE_PHY_CAP6_PPE_THRESHOLD_PRESENT) {
				atbm_uint8 nss = (priv->bss.sta_priv.rate.he_cap.ppe_thres[0] &  ATBM_IEEE80211_PPE_THRES_NSS_MASK) + 1;
				atbm_uint8 ru_index_bitmap = (priv->bss.sta_priv.rate.he_cap.ppe_thres[0] &
					 ATBM_IEEE80211_PPE_THRES_RU_INDEX_BITMASK_MASK) >>ATBM_IEEE80211_PPE_THRES_RU_INDEX_BITMASK_POS;
				atbm_uint8 *ppe = &priv->bss.sta_priv.rate.he_cap.ppe_thres[0];
				atbm_uint8 ppe_pos_bit = 7; /* Starting after PPE header */
			
				/*
				 * FW currently supports only nss == MAX_HE_SUPP_NSS
				 *
				 * If nss > MAX: we can ignore values we don't support
				 * If nss < MAX: we can set zeros in other streams
				 */
				if (nss > MAX_HE_SUPP_NSS) {
					wifi_printk(WIFI_DBG_MSG, "Got NSS = %d - trimming to %d\n", nss,MAX_HE_SUPP_NSS);
					nss = MAX_HE_SUPP_NSS;
				}
			
				for (i = 0; i < nss; i++) {
					atbm_uint8 ru_index_tmp = ru_index_bitmap << 1;
					atbm_uint8 bw;
			
					for(bw = 0; bw < MAX_HE_CHANNEL_BW_INDX; bw++) {
						ru_index_tmp >>= 1;
						if (!(ru_index_tmp & 1))
							continue;
			
						bss_params.pkt_ext.pkt_ext_qam_th[i][bw][1] = atbm_he_get_ppe_val(ppe, ppe_pos_bit);
						ppe_pos_bit += ATBM_IEEE80211_PPE_THRES_INFO_PPET_SIZE;
						bss_params.pkt_ext.pkt_ext_qam_th[i][bw][0] = atbm_he_get_ppe_val(ppe, ppe_pos_bit);
						ppe_pos_bit += ATBM_IEEE80211_PPE_THRES_INFO_PPET_SIZE;
					}
				}
			
				bss_params.he_support_flag	|= F_HE_MUL_PACKET_EXT;
			} else if ((priv->bss.sta_priv.rate.he_cap.he_cap_elem.phy_cap_info[9] &
					ATBM_IEEE80211_HE_PHY_CAP9_NOMIMAL_PKT_PADDING_MASK) !=
				  ATBM_IEEE80211_HE_PHY_CAP9_NOMIMAL_PKT_PADDING_RESERVED) {
				int low_th = -1;
				int high_th = -1;
			
				/* Take the PPE thresholds from the nominal padding info */
				switch (priv->bss.sta_priv.rate.he_cap.he_cap_elem.phy_cap_info[9] &
					ATBM_IEEE80211_HE_PHY_CAP9_NOMIMAL_PKT_PADDING_MASK) {
				case ATBM_IEEE80211_HE_PHY_CAP9_NOMIMAL_PKT_PADDING_0US:
					low_th = ATBM_HE_PKT_EXT_NONE;
					high_th = ATBM_HE_PKT_EXT_NONE;
					break;
				case ATBM_IEEE80211_HE_PHY_CAP9_NOMIMAL_PKT_PADDING_8US:
					low_th = ATBM_HE_PKT_EXT_BPSK;
					high_th = ATBM_HE_PKT_EXT_NONE;
					break;
				case ATBM_IEEE80211_HE_PHY_CAP9_NOMIMAL_PKT_PADDING_16US:
					low_th = ATBM_HE_PKT_EXT_NONE;
					high_th = ATBM_HE_PKT_EXT_BPSK;
					break;
				}
			
				/* Set the PPE thresholds accordingly */
				if (low_th >= 0 && high_th >= 0) {
					struct atbm_he_pkt_ext *pkt_ext = &bss_params.pkt_ext;
			
					for (i = 0; i < MAX_HE_SUPP_NSS; i++) {
						atbm_uint8 bw;
			
						for (bw = 0; bw < MAX_HE_CHANNEL_BW_INDX; bw++) {
							pkt_ext->pkt_ext_qam_th[i][bw][0] = low_th;
							pkt_ext->pkt_ext_qam_th[i][bw][1] = high_th;
						}
					}
			
					bss_params.he_support_flag |= F_HE_MUL_PACKET_EXT;
				}
			}

			if ((priv->bss.sta_priv.rate.he_cap.he_cap_elem.phy_cap_info[3] & ATBM_IEEE80211_HE_PHY_CAP3_DCM_MAX_CONST_TX_MASK)) {
				//priv->bss_params.he_support_flag |= F_HE_SU_4XLTF_08US;		
				if ((priv->bss.sta_priv.rate.he_cap.he_cap_elem.phy_cap_info[3] & ATBM_IEEE80211_HE_PHY_CAP3_DCM_MAX_CONST_TX_MASK) == ATBM_IEEE80211_HE_PHY_CAP3_DCM_MAX_CONST_TX_BPSK) {
					wifi_printk(WIFI_DBG_ERROR,"[STA] DCM_support MCS0\n");
				}
				else if ((priv->bss.sta_priv.rate.he_cap.he_cap_elem.phy_cap_info[3] & ATBM_IEEE80211_HE_PHY_CAP3_DCM_MAX_CONST_TX_MASK) == ATBM_IEEE80211_HE_PHY_CAP3_DCM_MAX_CONST_TX_QPSK) {
					wifi_printk(WIFI_DBG_ERROR,"[STA] DCM_support MCS0/1\n");
				}
				else {
					wifi_printk(WIFI_DBG_ERROR,"[STA] DCM_support MCS0/1/3/4\n");
				}
			}
			else {
				wifi_printk(WIFI_DBG_ERROR,"[STA] dcm not support\n");
			}
			if ((priv->bss.sta_priv.rate.he_cap.he_cap_elem.phy_cap_info[7] & ATBM_IEEE80211_HE_PHY_CAP7_HE_SU_MU_PPDU_4XLTF_AND_08_US_GI)) {
				bss_params.he_support_flag |= F_HE_SU_4XLTF_08US;
			}
			if ((priv->bss.sta_priv.rate.he_cap.he_cap_elem.phy_cap_info[8] & ATBM_IEEE80211_HE_PHY_CAP8_HE_ER_SU_PPDU_4XLTF_AND_08_US_GI)) {
				bss_params.he_support_flag |= F_HE_ER_4XLTF_08US;
			}
		}
		wifi_printk(WIFI_CONNECT, "[STA] color %d, he_support_flag: %x\n", bss_params.color, bss_params.he_support_flag);
	}
#endif

	if (elems.wmm_param){
		priv->bss.sta_priv.wmm_used = 1;
		if(priv->bss.sta_priv.he_used == 0){
			elems.mu_edca_param_set = ATBM_NULL;
		}
		if (!atbmwifi_ieee80211_sta_wmm_params(priv, elems.wmm_param,
			elems.wmm_param_len,
			elems.mu_edca_param_set)){
			/* still enable QoS since we might have HT/VHT */
			//atbmwifi_ieee80211_set_wmm_default(priv);
			priv->bss.sta_priv.wmm_used = 0;
		}
	}else{
		priv->bss.sta_priv.wmm_used = 0;
	}

	atbm_sta_supp_rate(sband, &priv->bss.sta_priv, &association_mode.supp_rate);
	if(wsm_set_association_mode(hw_priv,
			&association_mode, priv->if_id)){
		ret = -1;
		goto __error;
	}
	if(wsm_keep_alive_period(hw_priv,
			KEEP_ALIVE_PERIOD /* sec */,
			priv->if_id)){
		ret = -2;
		goto __error;
	}

	bss_params.operationalRateSet =
				atbm_cpu_to_le32(
				atbmwifi_rate_mask_to_wsm(hw_priv, priv->bss.sta_priv.band,
				priv->bss.sta_priv.rate.support_rates));
	
	bss_params.beaconLostCount = DEFAULT_BEACON_LOSS_CNT;
	
	bss_params.aid = priv->bss.aid;

	if(wsm_set_bss_params(hw_priv, &bss_params,
			priv->if_id)){
		ret = -3;
		goto __error;
	}
	if(wsm_set_beacon_wakeup_period(hw_priv,
				 priv->bss.sta_priv.beacon_interval * priv->bss.dtim_period >
				MAX_BEACON_SKIP_TIME_MS ? 1 :
				priv->bss.dtim_period, 0, priv->if_id)){
		ret = -4;
		goto __error;
	}
	if(priv->bss.sta_priv.sta_rc_priv != ATBM_NULL){
		rate_control_free_sta(&priv->bss.sta_priv);
	}

	priv->bss.sta_priv.rate.chip_category = priv->hw_priv->chip_priv->chip_cat;
	priv->bss.sta_priv.sta_rc_priv = rate_control_alloc_sta(&priv->bss.sta_priv, GFP_KERNEL);
	rate_control_rate_init(&priv->bss.sta_priv, sband, priv->bss.channel_num, priv->bss.channel_type);

	if (priv->bss.sta_priv.rate.ht) {
		/* Statically enabling block ack for TX/RX */
#if ATBM_DRIVER_PROCESS_BA
		if(!hw_priv->driver_setup_ba)
#endif
		{
			wsm_set_block_ack_policy(hw_priv,
				hw_priv->ba_tid_tx_mask, hw_priv->ba_tid_rx_mask,priv->if_id);
		}
	}

#if ATBM_DRIVER_PROCESS_BA
	if(hw_priv->driver_setup_ba){
		if(!priv->bss.sta_priv.ampdu_mlme.init){
			atbm_os_mutexLockInit(&priv->bss.sta_priv.ampdu_mlme.mtx);
			priv->bss.sta_priv.ampdu_mlme.work = atbm_init_work(hw_priv, 
							ieee80211_ba_session_work, &priv->bss.sta_priv);
			priv->bss.sta_priv.ampdu_mlme.init = 1;
		}
		priv->bss.sta_priv.priv = priv;
		atbm_memcpy(priv->bss.sta_priv.mac, priv->bss.bssid, ATBM_ETH_ALEN);
		priv->bss.sta_priv.link_id = 0;
		for (i = 0; i < STA_TID_NUM; i++) {
			/*
			 * timer_to_tid must be initialized with identity mapping
			 * to enable session_timer's data differentiation. See
			 * sta_rx_agg_session_timer_expired for usage.
			 */
			priv->bss.sta_priv.timer_to_tid[i] = i;
			priv->bss.sta_priv.rx_token[i] = -1;
			priv->bss.sta_priv.ampdu_mlme.trylimit[i] = 0;
			priv->bss.sta_priv.ampdu_mlme.timestamp[i] = atbm_GetOsTimeMs();
		}
	}
#endif
	priv->assoc_ok = 1;

	/*cancel connect timeout*/
	atbmwifi_eloop_cancel_timeout(atbmwifi_sta_join_timeout, (atbm_void *)priv, ATBM_NULL);
	atbmwifi_event_uplayer(priv,ATBM_WIFI_ASSOC_EVENT,(atbm_uint8*)skb);
#if CONFIG_WPS
	if(wpa_s->wps_mode != WPS_MODE_UNKNOWN){
#if CONFIG_P2P
		if(priv->p2p_join)
			priv->auto_connect_when_lost = 0;
#endif
		if(wpa_wsc_tx_process(wpa_s, ATBM_IEEE802_1X_TYPE_EAPOL_START, ATBM_NULL, 0) < 0)
			wifi_printk(WIFI_WPS, "WPS: eapol start failed.\n");
	}
#endif
__error:
	wifi_printk(WIFI_CONNECT|WIFI_DBG_ERROR,"connectap %s ret=%d \n",ret?"error":"success",ret);
	return status_code;
}


/*SUCCESS return linkid;fail return 0*/
 int sta_add_linkid(struct atbmwifi_vif *priv,
		  atbm_uint8 *sta_mac)
{
	int link_id = 0;
/*

	link_id = sta_alloc_linkid(priv, sta_mac);	
	if(link_id ==0){
		wifi_printk(WIFI_CONNECT,"%s Error \n",__FUNCTION__);
		return 0;
	}
	
	priv->link_id_db[link_id-1].status = ATBMWIFI__LINK_HARD;

	atbmwifi_link_id_lmac(priv,link_id,0);
*/
	/*STA mode not need to add linkid, in lmac LMC_ActivateInterface TxLinkEnabled = 0x8001;*/
	return link_id;
}

 int sta_del_linkid(struct atbmwifi_vif *priv,
		 atbm_uint8 * staMacAddr)
{

//	int link_id =0;
/*
	wifi_printk(WIFI_DBG_MSG,"[sta]:%s++\n",__FUNCTION__);

	link_id = atbmwifi_find_link_id(priv, staMacAddr);

	if((link_id <= ATBMWIFI__MAX_STA_IN_AP_MODE) && link_id>0){
		_atbmwifi_unmap_link(priv, link_id);
		priv->link_id_db[link_id-1].status = ATBMWIFI__LINK_OFF;
		wifi_printk(WIFI_DBG_MSG,"[sta]:%d link_id %d\n",__FUNCTION__,link_id);
	}
	*/
	return 0;
}

atbm_void sta_connect_complete(struct atbmwifi_vif *priv)
{
	return;
}

/**************************************************************************************************************
 * 函数名: atbm_sta_deauth
 *
 * 功能描述:
 *   STA 模式下断开连接（Deauthentication）。该函数负责清理连接状态，发送 Deauth 帧，
 *   重置硬件配置，释放相关资源。
 *
 * 在连接流程中的位置:
 *   断开连接阶段
 *   可以在以下情况调用：
 *   - 用户主动断开
 *   - 收到 AP 的 Deauth/Disassoc 帧
 *   - BSS Lost 事件
 *   - 连接超时
 *
 * 参数说明:
 *   @priv: 虚拟接口指针，包含接口的配置和状态信息
 *   @tx:   是否发送 Deauth 帧
 *          - ATBM_TRUE: 发送 Deauth 帧通知 AP
 *          - ATBM_FALSE: 不发送（被动断开）
 *
 * 返回值:
 *   无
 *
 * 处理流程:
 *   1. 通知上层网络不可用：ATBM_WIFI_DISENABLE_NET_EVENT
 *   2. 锁定发送队列：防止新数据包进入
 *   3. 清理队列映射
 *   4. 清理 AMPDU 会话（如果启用 BA）
 *   5. 发送 Deauth 帧（如果 tx == ATBM_TRUE）
 *   6. 刷新发送队列
 *   7. 解锁发送队列
 *   8. 更新 Join 状态为 PASSIVE
 *   9. 释放 BSS 信息内存
 *   10. 释放速率控制资源
 *   11. 删除密钥
 *   12. 重置 WSM
 *   13. 设置操作模式
 *   14. 停止 Keep Alive
 *
 * 资源清理:
 *   - information_elements: BSS IE 信息
 *   - sta_rc_priv: 速率控制私有数据
 *   - GTK/PTK: 加密密钥
 *
 * 注意事项:
 *   - 函数会清理所有连接相关的状态和资源
 *   - 如果是被动断开（收到 Deauth），不需要发送 Deauth
 *   - 断开后需要重新连接才能通信
 *
 * 相关函数:
 *   - atbmwifi_ieee80211_send_deauth_disassoc(): 发送 Deauth 帧
 *   - atbmwifi_del_key(): 删除密钥
 *   - wsm_reset(): 重置 WSM
 *   - atbmwifi_event_handler(): BSS Lost 事件处理
 *
 **************************************************************************************************************/
atbm_void atbm_sta_deauth(struct atbmwifi_vif *priv, ATBM_BOOL tx)
{	
	struct atbmwifi_common	*hw_priv = priv->hw_priv;
	//struct atbmwifi_cfg *config = atbmwifi_get_config(priv);
	struct wsm_reset reset = {
		.reset_statistics = ATBM_TRUE,
		 .link_id = 0,
	};
	
	struct wsm_operational_mode mode = {
		.power_mode = wsm_power_mode_quiescent,
		.disableMoreFlagUsage = ATBM_TRUE,
	};

	/*Disable net interface*/
	atbmwifi_event_uplayer(priv,ATBM_WIFI_DISENABLE_NET_EVENT,ATBM_NULL);

	atbmwifi_lock_queues(hw_priv, priv, ATBM_LOCK_REASON_TX_FLUSH);
	atbmwifi_clear_queues_map(hw_priv, priv->if_id, ATBM_BIT(0));

#if ATBM_DRIVER_PROCESS_BA
	if(hw_priv->driver_setup_ba){
		if(priv->bss.sta_priv.ampdu_mlme.init){
			priv->bss.sta_priv.ampdu_mlme.init = 0;
			atbmwifi_tx_aggr_queue_purge(&priv->bss.sta_priv);
			ieee80211_sta_tear_down_BA_sessions(&priv->bss.sta_priv, tx);
			atbm_os_DeleteMutex(&priv->bss.sta_priv.ampdu_mlme.mtx);
		}
	}
#endif
	if(tx){
		atbmwifi_ieee80211_send_deauth_disassoc(priv, priv->daddr,priv->bssid,
			       ATBM_IEEE80211_STYPE_DEAUTH,
			       ATBM_WLAN_REASON_DEAUTH_LEAVING,
			       ATBM_NULL, ATBM_TRUE);
	}

	__atbm_flush(hw_priv, ATBM_FALSE, priv->if_id, ATBM_BIT(0));
	atbmwifi_unlock_queues(hw_priv, priv, ATBM_LOCK_REASON_TX_FLUSH);

	priv->join_status = ATBMWIFI__JOIN_STATUS_PASSIVE;
	if(priv->bss.information_elements){
		atbm_kfree(priv->bss.information_elements);
		priv->bss.information_elements = ATBM_NULL;
		priv->bss.len_information_elements = 0;
	}
	if(priv->bss.sta_priv.sta_rc_priv){
		rate_control_free_sta(&priv->bss.sta_priv);
		priv->bss.sta_priv.sta_rc_priv = ATBM_NULL;
	}
	atbmwifi_del_key(priv,1,0);	
	atbmwifi_del_key(priv,0,0);
#if CONFIG_IEEE80211W
	atbmwifi_del_key(priv,2,0);
#endif
    atbm_memset(priv->connect.gtk, 0, sizeof(priv->connect.gtk));
    atbm_memset(priv->connect.ptk, 0, sizeof(priv->connect.ptk));
	wsm_reset(hw_priv, &reset, priv->if_id);	
	wsm_set_operational_mode(hw_priv, &mode, priv->if_id);
	wsm_keep_alive_period(hw_priv, 0, priv->if_id);
	//wsm_set_output_power(priv->hw_priv,priv->hw_priv->output_power * 10, priv->if_id);
#if ATBM_DRIVER_PROCESS_BA
	if(!hw_priv->driver_setup_ba)
#endif
	{
		wsm_set_block_ack_policy(hw_priv,0, 0, priv->if_id);
	}
	atbmwifi_set_pm(priv,ATBM_FALSE,0xFF);
	//atbmwifi_disable_sta_filter_retry(priv);

    //peterjiang@20200428, notify apply layer, and do disconnect;
	//wpa_supplicant_event_disassoc(priv);
	atbmwifi_event_uplayer(priv,ATBM_WIFI_DEASSOC_EVENT,0);
#if ATBM_PKG_REORDER
	wifi_printk(WIFI_CONNECT,"atbm_reorder_func_reset.\n");
	atbm_reorder_func_reset(priv,0xff);
#endif
	atbmwifi_eloop_cancel_timeout(atbmwifi_sta_join_timeout, (atbm_void *)priv, ATBM_NULL);

	priv->assoc_ok = 0;
	priv->connect_ok = 0;
	priv->connect.encrype = 0;

	atbm_memset(&priv->bss,0,sizeof(struct atbmwifi_cfg80211_bss));

	if(priv->auto_connect_when_lost){
		atbmwifi_autoconnect(priv, priv->scan_expire);
	}

	return;
}
/*
ps_enabled[0~1]  power save mode ,1 power save ,0 ACTIVE
dynamic_ps_timeout [0~127 ms] fast_ps timeout;
*/
atbm_void atbmwifi_set_pm(struct atbmwifi_vif *priv,ATBM_BOOL ps_enabled,atbm_uint8 dynamic_ps_timeout)
{	
	if (ps_enabled == ATBM_FALSE)
		priv->powersave_mode.pmMode = WSM_PSM_ACTIVE;
	else if (dynamic_ps_timeout <= 0)
		priv->powersave_mode.pmMode = WSM_PSM_PS;
	else
		priv->powersave_mode.pmMode = WSM_PSM_FAST_PS;

	wifi_printk(WIFI_PS, "[STA] Aid: %d, Joined: %s, Powersave: %s\n",
		priv->bss.aid,
		priv->join_status == ATBMWIFI__JOIN_STATUS_STA ? "yes" : "no",
		priv->powersave_mode.pmMode == WSM_PSM_ACTIVE ? "WSM_PSM_ACTIVE" :
		priv->powersave_mode.pmMode == WSM_PSM_PS ? "WSM_PSM_PS" :
		priv->powersave_mode.pmMode == WSM_PSM_FAST_PS ? "WSM_PSM_FAST_PS" :
		"UNKNOWN");

	/* Firmware requires that value for this 1-byte field must
	 * be specified in units of 500us. Values above the 128ms
	 * threshold are not supported. */
	if (dynamic_ps_timeout >= 0x80)
		priv->powersave_mode.fastPsmIdlePeriod = 0xFF;
	else
		priv->powersave_mode.fastPsmIdlePeriod =
				dynamic_ps_timeout << 1;

	if (priv->join_status == ATBMWIFI__JOIN_STATUS_STA 
		&&priv->bss.aid
		&&priv->assoc_ok)
		wsm_set_pm(priv->hw_priv, &priv->powersave_mode,
				priv->if_id);

	return;
}

int atbm_wifi_free_scaned_list(struct atbmwifi_vif *priv)
{

	return 0;
}
 /**
 * @brief 启动 STA 模式的 WPA Supplicant
 *
 * @param priv 虚拟接口指针
 *
 * @note 该函数初始化 WPA Supplicant 数据结构，用于 STA 模式的认证和连接
 *
 * @see atbmwifi_stop_sta() - 停止 STA 模式并释放 WPA Supplicant
 * @see init_wpa_supplicant() - 初始化 WPA Supplicant
 */
atbm_void atbmwifi_start_station(struct atbmwifi_vif *priv)
{
	priv->appdata = init_wpa_supplicant(priv);
}

/**
 * @brief 锁定发送队列
 *
 * @param hw_priv 硬件私有数据指针
 * @param priv 虚拟接口指针
 * @param reason 锁定原因
 *
 * @note 该函数锁定所有 4 个 WMM AC 发送队列，阻止新的数据包进入队列
 *       通常在刷新队列前调用，防止新的数据包干扰刷新过程
 *
 * 适用场景:
 * - 刷新队列前锁定
 * - 模式切换前锁定
 * - 接口禁用时锁定
 *
 * @see atbmwifi_unlock_queues() - 解锁发送队列
 * @see atbmwifi_queue_lock() - 单个队列锁定
 * @see __atbm_flush() - 刷新发送队列
 */
atbm_void atbmwifi_lock_queues(struct atbmwifi_common *hw_priv, struct atbmwifi_vif *priv,  enum atbm_lock_reason reason)
{
	int i;

	for(i = 0; i < 4; i++){
		atbmwifi_queue_lock(&hw_priv->tx_queue[i],priv, 0, reason);
	}
}

/**
 * @brief 解锁发送队列
 *
 * @param hw_priv 硬件私有数据指针
 * @param priv 虚拟接口指针
 * @param reason 锁定原因
 *
 * @note 该函数解锁所有 4 个 WMM AC 发送队列，允许数据包继续发送
 *
 * @see atbmwifi_lock_queues() - 锁定发送队列
 * @see atbmwifi_queue_unlock() - 单个队列解锁
 */
atbm_void atbmwifi_unlock_queues(struct atbmwifi_common *hw_priv, struct atbmwifi_vif *priv,  enum atbm_lock_reason reason)
{
   int i;

   for(i = 0; i < 4; i++){
	   atbmwifi_queue_unlock(&hw_priv->tx_queue[i],priv, 0, reason);
   }
}

/**
 * @brief 刷新发送队列
 *
 * @param hw_priv 硬件私有数据指针
 * @param drop ATBM_TRUE: 丢弃队列中的数据包, ATBM_FALSE: 等待发送完成
 * @param if_id 虚拟接口 ID
 * @param link_id_map Link ID 位图，指定要刷新的链路
 * @return int 0:成功
 *
 * @note 该函数用于刷新发送队列，确保所有数据包都被处理或丢弃：
 *       1. 如果 drop 为 ATBM_TRUE，直接清空所有发送队列
 *       2. 如果 drop 为 ATBM_FALSE，等待队列中的数据包发送完成
 *       3. 使用重试机制，最多等待 3 个 DTIM 周期
 *       4. 超时后强制丢弃数据包
 *
 * 适用场景:
 * - STA 断开连接时刷新队列
 * - AP 关闭时刷新队列
 * - 模式切换时刷新队列
 * - 接口禁用时刷新队列
 *
 * @see atbmwifi_stop_sta() - 调用该函数刷新 STA 队列
 * @see atbmwifi_stop_ap() - 调用该函数刷新 AP 队列
 * @see atbmwifi_queue_clear() - 清空发送队列
 * @see atbmwifi_queue_stats_is_empty() - 检查队列是否为空
 */
int __atbm_flush(struct atbmwifi_common *hw_priv, ATBM_BOOL drop, int if_id, atbm_uint32 link_id_map)
{
	int i, ret;
	int retry = 0;
	struct atbmwifi_vif *priv =	_atbmwifi_hwpriv_to_vifpriv(hw_priv, if_id);

	/* 如果 BH 任务已终止，强制丢弃数据包 */
	if(atbm_bh_is_term(hw_priv)){
		drop = ATBM_TRUE;
	}

	for (;;) {
		/* 如果需要丢弃，清空所有发送队列 */
		if (drop) {
			for (i = 0; i < 4; ++i){
				atbmwifi_queue_clear(&hw_priv->tx_queue[i],if_id);
			}
		}

		ret = 0;

		/* 锁定 TX，检查队列状态 */
		wsm_vif_lock_tx(priv);
		if (atbm_unlikely(!atbmwifi_queue_stats_is_empty(
				&hw_priv->tx_queue_stats, link_id_map, if_id))) {
			/* 队列不为空，WSM 可能重新排队了帧 */
			wsm_unlock_tx(hw_priv);
			/* 检查是否超时 */
			if(retry >= 3 * TEST_DTIM_INTV){
				wifi_printk(WIFI_ALWAYS,"wait tx flush timeout\n");
				drop = ATBM_TRUE;  /* 超时后强制丢弃 */
			}
			wifi_printk(WIFI_ALWAYS,"wait tx flush\n");
			atbm_mdelay(TEST_BEACON_INTV);  /* 等待一个 Beacon 周期 */
			retry++;
			continue;
		}
		break;
	}

	return ret;
}

 atbm_void sta_work_task(struct atbmwifi_vif *priv)
{
	return;
}

void atbm_set_omi_work(struct atbm_work_struct *work){
	struct atbmwifi_vif *priv = (struct atbmwifi_vif *)work;
	struct atbmwifi_ieee80211_htc_om htc;

	memset(&htc, 0, sizeof(struct atbmwifi_ieee80211_htc_om));
	/*
	*set he A-Control
	*/
	htc.ht_control = 3;
	/*
	*om control
	*/
	htc.control_id = 1;
	/*
	*om fild
	*/
	htc.channel_width = !!atbmwifi_chtype_is_40M(priv->bss.channel_type);
	htc.ul_mu_disable = 1;
	htc.ul_mu_data_diable = 1;
	atbmwifi_ieee80211_send_htc_qosnullfunc(priv, *((atbm_uint32*)&htc));
	wifi_printk(WIFI_ALWAYS, "send htc qos null\n");
}

/**************************************************************************************************************
 * 函数名: atbmwifi_stop_sta
 *
 * 功能描述:
 *   停止 STA 模式。该函数负责完全关闭 STA 模式，包括取消所有定时器、工作队列，
 *   断开连接，释放资源，重置状态。
 *
 * 在连接流程中的位置:
 *   STA 模式关闭阶段
 *   在用户关闭 WiFi 或切换模式时调用
 *
 * 参数说明:
 *   @priv: 虚拟接口指针，包含接口的配置和状态信息
 *
 * 返回值:
 *   无
 *
 * 处理流程:
 *   1. 模式检查：确认是 STA 模式
 *   2. 使能检查：确认接口已使能
 *   3. 取消 WPS 定时器（如果启用）
 *   4. 取消 Join 超时定时器
 *   5. 取消扫描定时器
 *   6. 通知连接丢失
 *   7. 取消扫描和 Join 工作队列
 *   8. 刷新发送队列
 *   9. 释放扫描结果内存
 *   10. 释放 Extra IE 内存
 *   11. 重置状态标志
 *   12. 调用 atbm_sta_deauth() 断开连接
 *   13. 清除加密配置
 *   14. 释放扫描列表
 *   15. 释放 WPA Supplicant
 *   16. 重置接口类型
 *
 * 资源清理:
 *   - scan_ret.info: 扫描结果缓冲区
 *   - extra_ie: 额外 IE 缓冲区
 *   - config: 配置结构体
 *   - wpa_supplicant: WPA 状态机
 *
 * 注意事项:
 *   - 函数会清理所有 STA 相关的资源
 *   - 调用后需要重新调用 atbmwifi_start_sta() 才能使用 STA 模式
 *   - 会发送 Deauth 帧通知 AP
 *
 * 相关函数:
 *   - atbmwifi_start_sta(): 启动 STA 模式
 *   - atbm_sta_deauth(): 断开连接
 *   - free_wpa_supplicant(): 释放 WPA Supplicant
 *
 **************************************************************************************************************/
atbm_void atbmwifi_stop_sta(struct atbmwifi_vif *priv)
{
    struct wpa_supplicant *wpa_s = priv->appdata;
	struct atbmwifi_common *hw_priv = _atbmwifi_vifpriv_to_hwpriv(priv);
	if(!atbmwifi_is_sta_mode(priv->iftype))
		goto sta_off;
	if(!priv->enabled){
		wifi_printk(WIFI_ALWAYS,"atbmwifi_stop_sta drop\n");
		goto sta_off;
	}
	priv->doing_stop_off = 1;
#if CONFIG_WPS
    //peterjiang@20200531,fix bug #35766
    if((wpa_s != ATBM_NULL) && (wpa_s->wps_mode != WPS_MODE_UNKNOWN))
        atbmwifi_eloop_cancel_timeout(wpas_wps_timeout, wpa_s, NULL);
#endif
    //peterjiang@20200518, wifi hungup issue
    //if scan timer is working, then do station off from webpage, the next scan will happened null pointer
	atbmwifi_eloop_cancel_timeout(atbmwifi_sta_join_timeout, (atbm_void *)priv, ATBM_NULL);
    atbmwifi_eloop_cancel_timeout(sta_scan_start_timer_func, (atbm_void *)priv, ATBM_NULL);
    
	atbmwifi_ieee80211_connection_loss(priv);

	atbm_cancel_work(hw_priv, priv->scan.scan_work);
	atbm_cancel_work(hw_priv, priv->join_work);			
	
	__atbm_flush(hw_priv, ATBM_FALSE, priv->if_id, 0xff);
	
	if(priv->scan_ret.info ){
		atbm_kfree(priv->scan_ret.info);
		priv->scan_ret.info = ATBM_NULL;
		priv->scan_ret.len =0;
	}
	if(priv->extra_ie){
		atbm_kfree(priv->extra_ie);
		priv->extra_ie_len = 0;
		priv->extra_ie = ATBM_NULL;
	}

	priv->auto_connect_when_lost =0;
	priv->listening = ATBM_FALSE;
	
	priv->enabled = 0;
	/*Here need clear station states*/
	atbm_sta_deauth(priv, ATBM_TRUE);

	priv->connect.crypto_pairwise=0;
	priv->connect.crypto_group=0;
	priv->doing_stop_off = 0;

	atbm_memset(&priv->config,0,sizeof(struct atbmwifi_cfg));

	//atbmwifi_event_uplayer(priv,ATBM_WIFI_DEAUTH_EVENT,ATBM_NULL);
	/*Disable net interface*/
	//atbmwifi_event_uplayer(priv,ATBM_WIFI_DISENABLE_NET_EVENT,ATBM_NULL);
	atbm_wifi_free_scaned_list(priv);
	
	priv->auto_connect_when_lost = 0;
	priv->join_status = ATBMWIFI__JOIN_STATUS_PASSIVE;
	free_wpa_supplicant(priv);
	atbm_mdelay(100);
sta_off:
	priv->iftype = ATBM_NUM_NL80211_IFTYPES;
	return;
}

/**************************************************************************************************************
 * 函数名: atbmwifi_start_sta
 *
 * 功能描述:
 *   启动 STA 模式。该函数负责初始化 STA 模式所需的资源，包括使能接口、初始化工作队列、
 *   设置信道类型，并启动 WPA Supplicant。
 *
 * 在连接流程中的位置:
 *   STA 模式启动阶段
 *   在用户开启 WiFi STA 模式时调用
 *
 * 参数说明:
 *   @priv: 虚拟接口指针，包含接口的配置和状态信息
 *
 * 返回值:
 *   无
 *
 * 处理流程:
 *   1. 使能接口：设置 priv->enabled = 1
 *   2. 设置扫描超时：scan_expire = 2
 *   3. 初始化扫描工作队列：scan.scan_work
 *   4. 初始化 Join 工作队列：join_work
 *   5. 设置信道类型：CH_OFF_20（20MHz）
 *   6. 调用 atbmwifi_start_station() 初始化 WPA Supplicant
 *
 * 初始化的资源:
 *   - scan.scan_work: 扫描工作队列
 *   - join_work: Join 工作队列
 *   - appdata: WPA Supplicant 实例
 *
 * 注意事项:
 *   - 调用此函数后，STA 模式才能正常工作
 *   - 需要先调用此函数，再调用 wifi_ConnectAP_vif() 连接 AP
 *   - 关闭 STA 模式需要调用 atbmwifi_stop_sta()
 *
 * 相关函数:
 *   - atbmwifi_stop_sta(): 停止 STA 模式
 *   - atbmwifi_start_station(): 初始化 WPA Supplicant
 *   - wifi_ConnectAP_vif(): 连接 AP
 *
 **************************************************************************************************************/
 atbm_void atbmwifi_start_sta(struct atbmwifi_vif *priv)
{
	struct atbmwifi_common *hw_priv = _atbmwifi_vifpriv_to_hwpriv(priv);
	/* 使能当前接口，初始值为 0 */
	priv->enabled = 1;
	priv->scan_expire = 2;
	/* 初始化 Station 定时器（已注释） */
	//atbm_InitTimer(&priv->scan_expire_timer,sta_scan_start_timer_func,(atbm_void*)priv);
	//atbm_InitTimer(&priv->smartconfig_expire_timer,smartconfig_start_timer_func,(atbm_void*)priv);
	//atbm_InitTimer(&priv->connect_expire_timer,atbmwifi_sta_join_timeout,(atbm_void*)priv);
	//atbm_InitTimer(&priv->chswitch_timer,atbmwifi_sw_chntimeout,(atbm_void *)priv);
	/* 初始化扫描/Join 工作队列 */
	priv->scan.scan_work = atbm_init_work(hw_priv, atbm_scan_work,priv);
	priv->join_work = atbm_init_work(hw_priv, atbm_join_work,priv);
	/* 初始化信道类型为 20MHz */
	priv->bss.channel_type = CH_OFF_20;
	atbmwifi_start_station(priv);
	
	return;
}

#if CONFIG_P2P
int atbm_enable_listening(struct atbmwifi_vif *priv, atbm_uint16 chanNum){
	struct atbmwifi_common *hw_priv = _atbmwifi_vifpriv_to_hwpriv(priv);

	struct wsm_start start = {
#ifdef P2P_MULTIVIF
		.mode = WSM_START_MODE_P2P_DEV | (priv->if_id ? (1 << 4) : 0),
#else
		.mode = WSM_START_MODE_P2P_DEV | (priv->if_id << 4),
#endif
		.band = WSM_PHY_BAND_2_4G,			//0:2.4G,1:5G
		.channelNumber = chanNum,	// channel number
		.beaconInterval = 100,
		.DTIMPeriod = 1,
		.probeDelay = 0,
		.basicRateSet = 0x0F,
	};

	if(priv->if_id == 0)
		start.channel_type = (atbm_uint32)(hw_priv->channel_type);
	else
		start.channel_type = ATBM_NL80211_CHAN_HT20;

	if((priv->iftype == ATBM_NL80211_IFTYPE_P2P_CLIENT) || (priv->iftype == ATBM_NL80211_IFTYPE_P2P_GO)){
		start.mode |= ATBM_BIT(6);
		wifi_printk(WIFI_ALWAYS, "[P2P MODE] SET ATBM_BIT(6)");
	}

	return wsm_start(hw_priv,&start,0);
}

int atbm_disable_listening(struct atbmwifi_vif *priv){
	struct atbmwifi_common *hw_priv = _atbmwifi_vifpriv_to_hwpriv(priv);
	int ret;

	struct wsm_reset reset = {
		.reset_statistics = ATBM_TRUE,
	};
	priv->join_status = ATBMWIFI__JOIN_STATUS_PASSIVE;
	ret = wsm_reset(hw_priv, &reset, priv->if_id);
	if(priv->p2pdata){
	}
	return ret;
}
#endif

