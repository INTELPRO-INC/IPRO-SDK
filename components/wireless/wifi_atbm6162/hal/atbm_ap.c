/**************************************************************************************************************
 * altobeam RTOS wifi hmac source code 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 *
 *  The source code contains proprietary information of AltoBeam, and shall not be distributed, 
 *  copied, reproduced, or disclosed in whole or in part without prior written permission of AltoBeam.
*****************************************************************************************************************/

#include "atbm_hal.h"
#if ATBM_SUPPORT_BRIDGE
#include "atbm_bridge.h"
#endif
#include "atbm_ratectrl.h"

atbm_void atbmwifi_mcast_timeout(atbm_void *data1,atbm_void *data2);
extern atbm_void atbmwifi_queued_timeout(atbm_void *data1,atbm_void *data2);

/**
 * @brief 解除 Link ID 在 LMAC 的映射
 *
 * @param priv 虚拟接口私有数据结构指针
 * @param link_id 要解除映射的 Link ID (1-8)
 * @return int 0:成功, 其他:失败错误码
 *
 * @note 该函数发送 WSM_MAP_LINK 命令到固件，解除 MAC 地址到 Link ID 的映射
 *       通常在 STA 断开连接或需要重新映射时调用
 *
 * @see atbmwifi_link_id_lmac() - 建立 Link ID 映射
 * @see atbmwifi_ap_deauth() - 调用该函数解除映射
 */
int _atbmwifi_unmap_link(struct atbmwifi_vif *priv, int link_id)
{
	struct atbmwifi_common *hw_priv = _atbmwifi_vifpriv_to_hwpriv(priv);
	struct wsm_map_link maplink;

	maplink.link_id = link_id;
	maplink.unmap = ATBM_TRUE;  /* 解除映射标志 */

	/* 复制 STA MAC 地址 */
	if (link_id)
		atbm_memcpy(&maplink.mac_addr[0],
			priv->link_id_db[link_id - 1].mac, ATBM_ETH_ALEN);

	/* 发送 WSM_MAP_LINK 命令到固件 */
	return wsm_map_link(hw_priv, &maplink, priv->if_id);
}

/**
 * @brief 将 Link ID 映射到 LMAC (底层 MAC)
 *
 * @param priv 虚拟接口私有数据结构指针
 * @param link_id Link ID (1-8)
 * @param mfp Management Frame Protection 使能标志
 *
 * @note 该函数在 STA 关联成功后被调用，主要完成以下工作：
 *       1. 更新 link_id_map 位图
 *       2. 清除 STA 的睡眠和 PS-Poll 状态
 *       3. 如果需要，先解除旧的映射
 *       4. 发送 WSM_MAP_LINK 命令到固件，建立 MAC 地址到 Link ID 的映射
 *
 * @see atbmwifi_sta_add() - 调用该函数映射 Link ID
 * @see _atbmwifi_unmap_link() - 解除 Link ID 映射
 */
atbm_void atbmwifi_link_id_lmac(struct atbmwifi_vif *priv,int link_id, ATBM_BOOL mfp)
{
	ATBM_BOOL need_reset;
	atbm_uint32 mask;
	struct atbmwifi_common *hw_priv = _atbmwifi_vifpriv_to_hwpriv(priv);
	struct wsm_map_link map_link;
	struct atbmwifi_sta_priv  *sta_priv;
	unsigned long flags;

	map_link.link_id = 0;
	map_link.unmap = 0;

	/* 仅在 AP 模式下执行 */
	if (priv->join_status != ATBMWIFI__JOIN_STATUS_AP)
		return;

	need_reset = ATBM_FALSE;
	mask = ATBM_BIT(link_id);

	/* 仅处理已硬连接的 STA */
	if(priv->link_id_db[link_id-1].status == ATBMWIFI__LINK_HARD) {
		atbm_spin_lock_irqsave(&priv->ps_state_lock, &flags);

		/* 如果 Link ID 已存在，需要重置 */
		if (priv->link_id_map & mask) {
			priv->sta_asleep_mask &= ~mask;
			priv->pspoll_mask &= ~mask;
			need_reset = ATBM_TRUE;
		}
		priv->link_id_map |= mask;

		/* 复制 STA MAC 地址 */
		atbm_memcpy(map_link.mac_addr, priv->link_id_db[link_id-1].mac,ATBM_ETH_ALEN);
		atbm_spin_unlock_irqrestore(&priv->ps_state_lock, flags);

		/* 如果需要，先解除旧的映射 */
		if (need_reset) {
			_atbmwifi_unmap_link(priv, link_id);
		}

		/* 配置映射参数 */
		map_link.link_id = link_id;
		map_link.mfp = mfp;

		/* 获取 STA 支持的速率 */
		sta_priv = &priv->link_id_db[link_id-1].sta_priv;
		atbm_sta_supp_rate(hw_priv->bands[sta_priv->band], sta_priv, &map_link.supp_rate);

		/* 发送 WSM_MAP_LINK 命令到固件 */
		wsm_map_link(hw_priv, &map_link, priv->if_id);
	}
}

/**
 * @brief 分配 Link ID 给新连接的 STA
 *
 * @param priv 虚拟接口私有数据结构指针
 * @param mac STA 的 MAC 地址
 * @return int 分配的 Link ID (1-8), 0:失败
 *
 * @note 该函数在 STA 认证阶段被调用，分配一个可用的 Link ID
 *       并初始化 Link ID 数据库条目。分配后状态为 ATBMWIFI__LINK_RESERVE
 *
 * @see atbmwifi_sta_add() - 在关联阶段调用，将状态改为 ATBMWIFI__LINK_HARD
 */
int atbmwifi_alloc_link_id(struct atbmwifi_vif *priv, const atbm_uint8 *mac)
{
	int i, ret = 0;

	/* 查找空闲的 Link ID */
	for (i = 0; i < priv->config.max_sta_count; ++i) {
		if (!priv->link_id_db[i].status) {
			ret = i + 1;
			break;
		}
	}
	ATBM_WARN_ON_FUNC(ret>ATBMWIFI__MAX_STA_IN_AP_MODE);
	ATBM_WARN_ON_FUNC(ret==0);

	/* 初始化 Link ID 数据库条目 */
	if (ret) {
		struct atbmwifi_link_entry *entry = &priv->link_id_db[ret - 1];
		wifi_printk(WIFI_ALWAYS,"[AP] STA added, link_id: %d\n",ret);
		entry->status = ATBMWIFI__LINK_RESERVE;
		atbm_memcpy(&entry->mac, mac, ATBM_ETH_ALEN);
		atbm_memset(&entry->buffered, 0, ATBMWIFI__MAX_TID);
		atbm_memcpy(entry->sta_priv.mac, mac, ATBM_ETH_ALEN);
		entry->sta_priv.priv =priv ;
		entry->sta_priv.link_id = 0 ;
		entry->sta_priv.flags = 0 ;
		entry->sta_priv.driver_buffered_tids =0 ;
	}

	return ret;
}

/*find hard connected link id ,get sta mac address, and copy together*/
int atbmwifi_get_hard_linked_macs(struct atbmwifi_vif *priv,  atbm_uint8 *mac, atbm_uint32 maccnt)
{
	int i;
	atbm_uint8 *tmp = mac;
	int mac_copyed_len = 0;
	unsigned long flags; 

	atbm_spin_lock_irqsave(&priv->ps_state_lock, &flags);

	for (i = 0; i <  priv->config.max_sta_count; ++i) {
		if ((priv->link_id_db[i].status==ATBMWIFI__LINK_HARD)) {

			atbm_memcpy(tmp, priv->link_id_db[i].mac,  ATBM_ETH_ALEN);
			mac_copyed_len++;
			tmp += ATBM_ETH_ALEN;	
			if(mac_copyed_len >= maccnt)
			{
				break;
			}
		}
	}
	atbm_spin_unlock_irqrestore(&priv->ps_state_lock, flags);
	return mac_copyed_len;
}
/*find link id ,have alloc link id*/
int atbmwifi_find_link_id(struct atbmwifi_vif *priv, const atbm_uint8 *mac)
{
	int i, ret = 0;
	unsigned long flags;

	atbm_spin_lock_irqsave(&priv->ps_state_lock, &flags);

	for (i = 0; i < priv->config.max_sta_count; ++i) {
		if (!atbm_memcmp(mac, priv->link_id_db[i].mac, ATBM_ETH_ALEN) &&
				priv->link_id_db[i].status) {
			ret = i + 1;
			break;
		}
	}
	atbm_spin_unlock_irqrestore(&priv->ps_state_lock, flags);
	return ret;
}
/*find link id ,have connect link id*/
int atbmwifi_find_hard_link_id(struct atbmwifi_vif *priv, const atbm_uint8 *mac)
{
	int i, ret = 0;
	unsigned long flags;

	atbm_spin_lock_irqsave(&priv->ps_state_lock, &flags);

	for (i = 0; i < priv->config.max_sta_count; ++i) {
		if (!atbm_memcmp(mac, priv->link_id_db[i].mac, ATBM_ETH_ALEN) &&
				(priv->link_id_db[i].status==ATBMWIFI__LINK_HARD)) {
			//priv->link_id_db[i].timestamp = atbm_GetOsTimeMs;
			ret = i + 1;
			break;
		}
	}
	atbm_spin_unlock_irqrestore(&priv->ps_state_lock, flags);
	return ret;
}


int atbmwifi_sta_alloc(struct atbmwifi_vif *priv,
		  atbm_uint8 *sta_mac)
{
	int link_id = 0;

	if (!atbmwifi_is_ap_mode(priv->iftype))
		return 0;
	link_id = atbmwifi_find_link_id(priv, sta_mac);

	if((link_id > 0) && (priv->link_id_db[link_id-1].status > ATBMWIFI__LINK_RESERVE)){
		atbmwifi_sta_del(priv, sta_mac, ATBM_FALSE);
		link_id = 0;
	}

	if(link_id==0) {
		link_id = atbmwifi_alloc_link_id(priv, sta_mac);
		if(link_id ==0){
			wifi_printk(WIFI_CONNECT,"%s %d Err1\n",__FUNCTION__, __LINE__);
			return -1;
		}
	}
	else {
		//if(priv->link_id_db[link_id-1].status != ATBMWIFI__LINK_RESERVE)
		struct hostapd_data *hapd = (struct hostapd_data *)priv->appdata;
		struct hostapd_sta_info *sta = atbm_ap_get_sta(hapd, sta_mac);

		if(sta && (sta->aid != link_id
#if CONFIG_SAE
			|| sta->sae.state != SAE_COMMITTED
#endif
			))
		{
			ap_sta_del(hapd, sta);
		}
		atbm_memcpy(&priv->link_id_db[link_id-1].sta_priv.mac, sta_mac, ATBM_ETH_ALEN);
		priv->link_id_db[link_id-1].status = ATBMWIFI__LINK_RESERVE;
		priv->link_id_db[link_id-1].sta_priv.priv = priv;
		atbm_memset(priv->link_id_db[link_id-1].buffered, 0, ATBMWIFI__MAX_TID);
	}

	return link_id;
}
  
int atbmwifi_sta_add(struct atbmwifi_vif *priv,
		  atbm_uint8 *sta_mac)
{
	int link_id = 0;
	struct hostapd_sta_info *sta;
	struct atbmwifi_sta_priv *sta_priv = ATBM_NULL;
	//struct atbmwifi_cfg *config = atbmwifi_get_config(priv);
	
	if (!atbmwifi_is_ap_mode(priv->iftype))
		return 0;
	link_id = atbmwifi_find_hard_link_id(priv, sta_mac); 
	if(link_id !=0){
		wifi_printk(WIFI_CONNECT,"sta_add again just drop \n");
		return 0;
	}

	link_id = atbmwifi_find_link_id(priv, sta_mac);	
	if(link_id ==0){
		wifi_printk(WIFI_CONNECT,"sta_add Error \n");
		return -1;
	}

	sta_priv = &priv->link_id_db[link_id-1].sta_priv;
	sta_priv->link_id = link_id;
	atbm_memset(&priv->link_id_db[link_id-1].sta_retry,0xff,sizeof(struct atbmwifi_filter_retry));
	//priv->link_id_db[link_id-1].sta_priv.sta_rc_priv = mac80211_ratectrl->alloc_sta(&sta_priv->rate);
	//mac80211_ratectrl->sta_rate_init(&sta_priv->rate,sta_priv->sta_rc_priv);
	if(sta_priv->sta_rc_priv != ATBM_NULL){
		rate_control_free_sta(sta_priv);
	}
	sta_priv->rate.chip_category = priv->hw_priv->chip_priv->chip_cat;
	sta_priv->sta_rc_priv = rate_control_alloc_sta(sta_priv, GFP_KERNEL);
	rate_control_rate_init(sta_priv, priv->hw_priv->bands[priv->config.band], priv->bss.channel_num, sta_priv->rate.channel_type);

	priv->sta_asleep_mask &= ~ATBM_BIT(link_id);
	priv->buffered_set_mask &= ~ATBM_BIT(link_id);

	atbm_spin_lock_init(&sta_priv->lock);
	priv->link_id_db[link_id-1].status = ATBMWIFI__LINK_HARD;
	atbmwifi_link_id_lmac(priv,link_id,sta_priv->ieee_80211w);

	wifi_printk(WIFI_CONNECT,"[ap]:assoc OK %d\n",link_id);
#if ATBM_DRIVER_PROCESS_BA
	if(priv->hw_priv->driver_setup_ba){
		int i;

		if(!sta_priv->ampdu_mlme.init){
			atbm_os_mutexLockInit(&sta_priv->ampdu_mlme.mtx);
			sta_priv->ampdu_mlme.work = atbm_init_work(priv->hw_priv, 
							ieee80211_ba_session_work, sta_priv);
			sta_priv->ampdu_mlme.init = 1;
		}
		sta_priv->priv = priv;
		atbm_memcpy(sta_priv->mac, sta_mac, ATBM_ETH_ALEN);
		sta_priv->link_id = link_id;
		for (i = 0; i < STA_TID_NUM; i++) {
			/*
			 * timer_to_tid must be initialized with identity mapping
			 * to enable session_timer's data differentiation. See
			 * sta_rx_agg_session_timer_expired for usage.
			 */
			sta_priv->timer_to_tid[i] = i;
			sta_priv->rx_token[i] = -1;
			sta_priv->ampdu_mlme.trylimit[i] = 0;
			sta_priv->ampdu_mlme.timestamp[i] = atbm_GetOsTimeMs();
		}

	}
#endif
	return 0;
}
int atbmwifi_sta_del(struct atbmwifi_vif *priv,
		 atbm_uint8 * staMacAddr, ATBM_BOOL tx)
{
	int link_id =0;

	wifi_printk(WIFI_CONNECT,"[ap]:atbmwifi_sta_del \n");

	if (!atbmwifi_is_ap_mode(priv->iftype))
		return 0;

	link_id = atbmwifi_find_link_id(priv, staMacAddr);
	if((link_id > ATBMWIFI__MAX_STA_IN_AP_MODE) || ( link_id<=0)){
		wifi_printk(WIFI_DBG_MSG,"[ap]:sta_del link_id 0 drop\n");
		return -1;
	}
	//del hostapd sta priv
	priv->link_id_db[link_id-1].sta_priv.reserved = ATBM_NULL;
	atbmwifi_event_uplayer(priv,ATBM_WIFI_DEAUTH_EVENT,staMacAddr);

	if((link_id <= ATBMWIFI__MAX_STA_IN_AP_MODE) && link_id>0){
#if ATBM_SUPPORT_BRIDGE
		remove_item_from_brpool(link_id);
#endif
		atbmwifi_lock_queues(priv->hw_priv, priv, ATBM_LOCK_REASON_TX_FLUSH);
		atbmwifi_clear_queues_map(priv->hw_priv, priv->if_id, ATBM_BIT(link_id));
#if ATBM_DRIVER_PROCESS_BA
		if(priv->hw_priv->driver_setup_ba){
			if(priv->link_id_db[link_id-1].sta_priv.ampdu_mlme.init){
				priv->link_id_db[link_id-1].sta_priv.ampdu_mlme.init = 0;
				atbmwifi_tx_aggr_queue_purge(&priv->link_id_db[link_id-1].sta_priv);
				ieee80211_sta_tear_down_BA_sessions(&priv->link_id_db[link_id-1].sta_priv, tx);
				atbm_os_DeleteMutex(&priv->link_id_db[link_id-1].sta_priv.ampdu_mlme.mtx);
			}
		}
#endif

		if(tx){
			atbmwifi_ieee80211_send_deauth_disassoc(priv, staMacAddr,priv->bssid,
				   ATBM_IEEE80211_STYPE_DEAUTH,
				   ATBM_WLAN_REASON_DISASSOC_DUE_TO_INACTIVITY,
				   ATBM_NULL, ATBM_TRUE);
		}

		__atbm_flush(priv->hw_priv, ATBM_FALSE, priv->if_id, ATBM_BIT(link_id));
		priv->link_id_db[link_id-1].status = ATBMWIFI__LINK_OFF;

		if(priv->link_id_db[link_id-1].sta_priv.sta_rc_priv){
			rate_control_free_sta(&priv->link_id_db[link_id-1].sta_priv);
			priv->link_id_db[link_id-1].sta_priv.sta_rc_priv = ATBM_NULL;
		}
		atbmwifi_del_key(priv,0, link_id);
#if ATBM_PKG_REORDER
		atbm_reorder_func_reset(priv,link_id - 1);
#endif	//ATBM_PKG_REORDER	

		_atbmwifi_unmap_link(priv, link_id);

		atbmwifi_unlock_queues(priv->hw_priv, priv, ATBM_LOCK_REASON_TX_FLUSH);

		priv->pspoll_mask &= ~ATBM_BIT(link_id);
		priv->sta_asleep_mask &= ~ATBM_BIT(link_id);
		priv->buffered_set_mask &= ~ATBM_BIT(link_id);
		atbm_memset(&priv->link_id_db[link_id-1].sta_retry,0xff,sizeof(struct atbmwifi_filter_retry));
		wifi_printk(WIFI_DBG_MSG,"[ap]:sta_del link_id %d\n",link_id);
		atbm_memset(&priv->link_id_db[link_id-1].sta_priv, 0, sizeof(struct atbmwifi_sta_priv));
	}

	return 0;
}

struct atbmwifi_sta_priv *atbmwifi_sta_find(struct atbmwifi_vif *priv,const atbm_uint8 *mac)
{	
	int i =0;	
	
	if (atbmwifi_is_sta_mode(priv->iftype) && priv->assoc_ok)
		return &priv->bss.sta_priv;
	
	for (i = 0; i < ATBMWIFI__MAX_STA_IN_AP_MODE; ++i) {
		if ((priv->link_id_db[i].status == ATBMWIFI__LINK_HARD) &&
			!atbm_memcmp(mac, priv->link_id_db[i].mac, ATBM_ETH_ALEN)){
				return &priv->link_id_db[i].sta_priv;
		}
	}
	return ATBM_NULL;
}

struct atbmwifi_sta_priv *atbmwifi_sta_find_form_hard_linkid(struct atbmwifi_vif *priv,const atbm_uint8 linkid)
{	

	if (!atbmwifi_is_ap_mode(priv->iftype))
		return ATBM_NULL;
	
	if(linkid > ATBMWIFI__MAX_STA_IN_AP_MODE)
		return ATBM_NULL;
	if(linkid == 0)
		return ATBM_NULL;
	
	if(priv->link_id_db[linkid-1].status == ATBMWIFI__LINK_HARD)
		return &priv->link_id_db[linkid-1].sta_priv;
	else 
		return ATBM_NULL;
}

struct atbmwifi_sta_priv *atbmwifi_sta_find_form_linkid(struct atbmwifi_vif *priv,const atbm_uint8 linkid)
{	

	if (!atbmwifi_is_ap_mode(priv->iftype))
		return ATBM_NULL;
	
	if(linkid > ATBMWIFI__MAX_STA_IN_AP_MODE)
		return ATBM_NULL;
	if(linkid == 0)
		return ATBM_NULL;
	
	if(priv->link_id_db[linkid-1].status != ATBMWIFI__LINK_OFF)
		return &priv->link_id_db[linkid-1].sta_priv;
	else 
		return ATBM_NULL;
}

static atbm_void __atbm_sta_notify(struct atbmwifi_vif *priv,
				enum sta_notify_cmd notify_cmd,
				int link_id)
{
	struct atbmwifi_common *hw_priv = _atbmwifi_vifpriv_to_hwpriv(priv);
	atbm_uint32 bit, prev;

	/* Zero link id means "for all link IDs" */
	if (link_id){
		bit = ATBM_BIT(link_id);
	}
	else if (ATBM_WARN_ON(notify_cmd != STA_NOTIFY_AWAKE)){
		bit = 0;
	}
	else{
		bit = priv->link_id_map;
	}
	prev = priv->sta_asleep_mask & bit;
	switch (notify_cmd) {
	case STA_NOTIFY_SLEEP:
		if (!prev) {
			if (priv->buffered_multicasts &&
					!priv->sta_asleep_mask)
				atbm_queue_work(priv->hw_priv, priv->set_tim_work);
			priv->sta_asleep_mask |= bit;
			wifi_printk(WIFI_PS,"STA_NOTIFY_SLEEP--->sta_asleep_mask %x\n",priv->sta_asleep_mask);
		}
		break;
	case STA_NOTIFY_AWAKE:
		if (prev) {
			priv->sta_asleep_mask &= ~bit;
			priv->pspoll_mask &= ~bit;
			priv->link_id_uapsd_mask &= ~bit;
			if (priv->tx_multicast && link_id &&
					!priv->sta_asleep_mask)
				atbm_queue_work(priv->hw_priv, priv->set_tim_work);
			wifi_printk(WIFI_PS,"STA_NOTIFY_AWAKE--->sta_asleep_mask %x\n",priv->sta_asleep_mask);
		}
		break;
	}
}


atbm_void atbm_ps_notify(struct atbmwifi_vif *priv,
		      int link_id, ATBM_BOOL ps)
{
	if (link_id > ATBMWIFI__MAX_STA_IN_AP_MODE)
		return;

	wifi_printk(WIFI_PS,"%s for LinkId: %d. STAs asleep: %.8X\n",
			ps ? "Stop" : "Start",
			link_id, priv->sta_asleep_mask);

	/* TODO:COMBO: __atbm_sta_notify changed. */
	__atbm_sta_notify(priv,
		ps ? STA_NOTIFY_SLEEP : STA_NOTIFY_AWAKE, link_id);
}

/**
 * @brief 实现 TIM (Traffic Indication Map) 更新
 *
 * @param priv 虚拟接口私有数据结构指针
 * @return int 0:成功
 *
 * @note 该函数更新 Beacon 帧中的 TIM IE，通知省电模式的 STA 是否有缓冲数据：
 *       1. 分配 TIM IE 缓冲区
 *       2. 加锁读取 buffered_set_mask 和 buffered_multicasts
 *       3. 调用 atbmwifi_add_tim() 构建 TIM IE
 *       4. 如果组播状态变化，注册 mcast_timeout
 *       5. 调用 wsm_update_ie() 更新 Beacon 帧
 *
 * @see atbm_set_tim() - 设置单个 STA 的 TIM 位
 * @see atbm_ap_set_tim_work() - 工作队列调用该函数
 * @see atbmwifi_mcast_timeout() - 组播数据发送超时处理
 */
int atbm_set_tim_impl(struct atbmwifi_vif *priv)
{
	struct atbmwifi_common *hw_priv = _atbmwifi_vifpriv_to_hwpriv(priv);
	struct atbmwifi_cfg *config=atbmwifi_get_config(priv);
	atbm_uint8 * tim_ie =ATBM_NULL;
	atbm_uint8 * tim_ie_end=ATBM_NULL;
	atbm_uint8 aid0_bit_set;
	struct wsm_update_ie update_ie={0};
	unsigned long flags;

	/* 设置更新 IE 类型为 Beacon */
	update_ie.what = WSM_UPDATE_IE_BEACON;
	update_ie.count = 1;
	update_ie.length = 0;

	/* 分配 TIM IE 缓冲区 */
	tim_ie = (atbm_uint8 *)atbm_kmalloc(sizeof(struct atbmwifi_ieee80211_tim_ie)+ ATBMWIFI__MAX_STA_IN_AP_MODE/8 + 4,GFP_KERNEL);
	if(atbm_unlikely(!tim_ie)){
		return -1;
	}

	/* 加锁读取省电状态 */
	atbm_spin_lock_irqsave(&priv->ps_state_lock, &flags);
	aid0_bit_set = priv->buffered_multicasts;
	tim_ie_end = atbmwifi_add_tim(tim_ie,priv,priv->buffered_multicasts);
	atbm_spin_unlock_irqrestore(&priv->ps_state_lock, flags);

	/* 如果组播状态变化，注册超时处理 */
	if(aid0_bit_set != priv->aid0_bit_set){
		long tmo=0;
		tmo=config->DTIMPeriod*(config->beaconInterval + 20);
		atbmwifi_eloop_register_timeout(0,tmo,atbmwifi_mcast_timeout,(atbm_void *)priv,ATBM_NULL);
	}

	/* 更新 Beacon IE */
	update_ie.ies = tim_ie;
	update_ie.length = (atbm_uint32)tim_ie_end-(atbm_uint32)tim_ie;
	ATBM_WARN_ON_FUNC(wsm_update_ie(hw_priv, &update_ie, priv->if_id));
	priv->aid0_bit_set = aid0_bit_set;

	atbm_kfree(tim_ie);

	return 0;
}

atbm_void atbm_ap_set_tim_work(struct atbm_work_struct *work)
{
	struct atbmwifi_vif *priv=(struct atbmwifi_vif *)work;
	if(atbm_bh_is_term(priv->hw_priv)){
		return;
	}
	atbm_set_tim_impl(priv);
}

int atbm_set_tim(struct atbmwifi_vif *priv, struct atbmwifi_sta_priv  *sta_priv,ATBM_BOOL set)
{
	if(atbm_bh_is_term(priv->hw_priv)){
		return 0;
	}
#ifdef P2P_MULTIVIF
	ATBM_WARN_ON_FUNC(priv->if_id == ATBM_WIFI_GENERIC_IF_ID);
#endif
	ATBM_WARN_ON_FUNC(!atbmwifi_is_ap_mode(priv->iftype));
	if(atbmwifi_set_tim(priv,sta_priv->link_id,set)){
		if(sta_priv->priv->sta_asleep_mask & ATBM_BIT(sta_priv->link_id)){
			atbm_queue_work(priv->hw_priv, priv->set_tim_work);
		}
	}
	return 0;
}
/**
 * @brief 组播数据发送超时处理函数
 *
 * @param data1 虚拟接口私有数据结构指针
 * @param data2 未使用
 *
 * @note 该函数在 DTIM 周期超时后被调用，检查是否有组播数据需要发送：
 *       1. 检查 aid0_bit_set 和 buffered_multicasts 标志
 *       2. 如果有组播数据，触发发送线程
 *
 *       这个函数确保在 DTIM Beacon 发送后，STA 唤醒时能及时收到组播数据
 *
 * @see atbm_set_tim_impl() - 注册该超时函数
 * @see atbm_suspend_resume() - WSM 回调处理组播发送
 */
atbm_void atbmwifi_mcast_timeout(atbm_void *data1,atbm_void *data2)
{
	struct atbmwifi_vif *priv =	(struct atbmwifi_vif *)data1;
	unsigned long flags;

	atbm_spin_lock_irqsave(&priv->ps_state_lock, &flags);
	/* 检查是否有组播数据需要发送 */
	priv->tx_multicast = priv->aid0_bit_set && priv->buffered_multicasts;
	if (priv->tx_multicast){
		/* 触发发送线程 */
		atbm_bh_schedule_tx(_atbmwifi_vifpriv_to_hwpriv(priv));
	}
	atbm_spin_unlock_irqrestore(&priv->ps_state_lock, flags);
}

/* ******************************************************************** */
/* WSM callback		 LMACtoUMAC_SuspendResumeTxInd when tx DTIM	*/
atbm_void atbm_suspend_resume(struct atbmwifi_vif *priv,
			   struct wsm_suspend_resume *arg)
{
	struct atbmwifi_common *hw_priv =
		_atbmwifi_vifpriv_to_hwpriv(priv);
	unsigned long flags;

	wifi_printk(WIFI_PS, "[AP] %s: %s\n",
			arg->stop ? "stop" : "start",
			arg->multicast ? "broadcast" : "unicast");

	if (arg->multicast) {
		ATBM_BOOL cancel_tmo = ATBM_FALSE;
		atbm_spin_lock_irqsave(&priv->ps_state_lock, &flags);
		if (arg->stop) {
			priv->tx_multicast = ATBM_FALSE;
		} else {
#if NEW_SUPPORT_PS
			//atbm_uint32 ac,n_frames;
			/* Firmware sends this indication every DTIM if there
			 * is a STA in powersave connected. There is no reason
			 * to suspend, following wakeup will consume much more
			 * power than it could be saved. */	
			 
			 priv->tx_multicast = (priv->aid0_bit_set &&priv->buffered_multicasts);
			 if(priv->tx_multicast)
			 {
			 	cancel_tmo= ATBM_TRUE;
//			 	wifi_printk(WIFI_PS,"--->muticast num n_frames =%d\n",n_frames); 
				atbm_bh_schedule_tx(priv->hw_priv);
			 }

			 
#endif
		}
		atbm_spin_unlock_irqrestore(&priv->ps_state_lock, flags);
		if (cancel_tmo)
		atbmwifi_eloop_cancel_timeout(atbmwifi_mcast_timeout, (atbm_void *)priv, ATBM_NULL);
	} else {
		/*lmac call here when p2p ps mode*/
		atbm_spin_lock_irqsave(&priv->ps_state_lock, &flags);
		atbm_ps_notify(priv, arg->link_id, arg->stop);
		atbm_spin_unlock_irqrestore(&priv->ps_state_lock, flags);
		if (!arg->stop)
			atbm_bh_wakeup(hw_priv);
	}
	return;
}

/**
 * @brief 解除 STA 的认证并删除 STA
 *
 * @param priv 虚拟接口私有数据结构指针
 * @param staMacAddr STA 的 MAC 地址
 * @param tx 是否发送 Deauth 帧
 * @return int 1:成功, -1:失败
 *
 * @note 该函数在 STA 断开连接或 AP 关闭时被调用，主要完成以下工作：
 *       1. 查找 STA 的 Link ID
 *       2. 调用 atbmwifi_sta_del() 删除 STA 并解除 Link ID 映射
 *
 * @see atbmwifi_stop_ap() - 调用该函数解除所有 STA 的认证
 * @see atbmwifi_sta_del() - 实际删除 STA 的函数
 */
int atbmwifi_ap_deauth(struct atbmwifi_vif *priv,atbm_uint8 *staMacAddr, ATBM_BOOL tx)
{
	int link_id = atbmwifi_find_link_id(priv, staMacAddr);

	/* 检查 Link ID 是否有效 */
	if((link_id > ATBMWIFI__MAX_STA_IN_AP_MODE) || ( link_id<0)){
		return -1;
	}

	/* 删除 STA 并解除 Link ID 映射 */
	atbmwifi_sta_del(priv,staMacAddr, tx);
	return 1;
}

int atbmwifi_ap_start_proberesp(struct atbmwifi_vif *priv)
{	
	int ret;
	struct atbmwifi_common *hw_priv = _atbmwifi_vifpriv_to_hwpriv(priv);
	struct wsm_template_frame frame={0};
	frame.frame_type = WSM_FRAME_TYPE_PROBE_RESPONSE;
	frame.disable =0; 
	//frame.rate = test_config_txrx.Rate;
#if CONFIG_P2P
	if(priv->p2p_ap)
		frame.rate = RATE_INDEX_A_6M;
	else
#endif
#if CONFIG_5G_SUPPORT
	if(priv->bss.sta_priv.band == ATBM_IEEE80211_BAND_5GHZ){
		ATBM_BUG_ON(!atbmwifi_5g_supported(hw_priv->chip_priv));
		frame.rate = RATE_INDEX_A_6M;
	}else
#endif
	frame.rate = RATE_INDEX_B_1M;
	frame.skb = atbmwifi_ieee80211_send_proberesp(priv,priv->extra_ie,priv->extra_ie_len);
	if (ATBM_WARN_ON(!frame.skb)){
		return 0;
	}
	ret = wsm_set_template_frame(hw_priv, &frame, priv->if_id);
	atbm_dev_kfree_skb(frame.skb);

	return ret;
}
/**
 * @brief 启动 AP Beacon 帧发送
 *
 * @param priv 虚拟接口私有数据结构指针
 * @return int 0:成功, 其他:失败错误码
 *
 * @note 该函数构建 Beacon 帧模板并发送到固件，主要完成以下工作：
 *       1. 配置 Beacon 帧发送速率 (根据频段和模式选择)
 *       2. 构建 Beacon 帧 (包含所有必要的 IE)
 *       3. 发送模板帧到固件
 *       4. 启动 Probe Response 发送
 *
 * @see atbm_start_ap() - 调用该函数启动 Beacon
 */
int atbmwifi_ap_start_beacon(struct atbmwifi_vif *priv)
{
	int ret = 0;
	struct atbmwifi_common *hw_priv =_atbmwifi_vifpriv_to_hwpriv(priv);
	struct wsm_template_frame frame={0};

	/* 配置模板帧参数 */
	frame.frame_type = WSM_FRAME_TYPE_BEACON;
	frame.disable =0;

	/* 根据频段和模式选择 Beacon 发送速率 */
#if CONFIG_P2P
	if(priv->p2p_ap)
		frame.rate = RATE_INDEX_A_6M;
	else
#endif
#if CONFIG_5G_SUPPORT
	if(priv->bss.sta_priv.band == ATBM_IEEE80211_BAND_5GHZ)
		frame.rate = RATE_INDEX_A_6M;
	else
#endif
	frame.rate = RATE_INDEX_B_1M;

	/* 构建 Beacon 帧 */
	frame.skb = atbmwifi_ieee80211_send_beacon(priv,priv->extra_ie,priv->extra_ie_len);
	if (ATBM_WARN_ON(!frame.skb)){
		return 0;
	}

	/* 发送模板帧到固件 */
	ret = wsm_set_template_frame(hw_priv, &frame, priv->if_id);
	if (!ret)
	{
		/* 启动 Probe Response 发送 */
		atbmwifi_ap_start_proberesp(priv);
	}

	atbm_dev_kfree_skb(frame.skb);

	return ret;
}

/**
 * @brief 实际的 AP 启动函数 - 配置并启动 AP 硬件
 *
 * @param priv 虚拟接口私有数据结构指针
 * @return int 0:成功, 其他:失败错误码
 *
 * @note 该函数是 AP 模式的核心启动函数，主要完成以下工作：
 *       1. 配置 WSM 启动参数 (信道、Beacon 间隔、DTIM 周期等)
 *       2. 启动 Beacon 发送
 *       3. 初始化省电管理相关变量
 *       4. 发送 WSM_START 命令到固件
 *       5. 设置不活动超时
 *       6. 配置块确认策略
 *       7. 设置操作模式
 *
 * @see atbmwifi_start_ap() - AP 模式入口函数
 * @see atbmwifi_ap_start_beacon() - 启动 Beacon 发送
 */
int atbm_start_ap(struct atbmwifi_vif *priv)
{
	int ret;
	struct atbmwifi_cfg *config = atbmwifi_get_config(priv);
	struct atbmwifi_common *hw_priv = _atbmwifi_vifpriv_to_hwpriv(priv);
	struct wsm_start start={0};
	struct wsm_inactivity inactivity;
	struct wsm_operational_mode mode;

	/* 初始化 WSM 命令结构体 */
	atbm_memset(&inactivity, 0, sizeof(inactivity));
	atbm_memset(&mode, 0, sizeof(mode));

	/* 配置 WSM 启动参数 */
	start.mode =  priv->config.mode;
	start.band =  priv->config.band;
	start.channelNumber =  config->channel_index;
	start.beaconInterval = config->beaconInterval;
	start.DTIMPeriod 	= config->DTIMPeriod;
	start.preambleType 	= config->preambleType;
	start.probeDelay 	= 100;
	start.basicRateSet 	= config->basicRateSet;
	start.channel_type = priv->bss.channel_type;

	/* 配置不活动超时 (用于检测 STA 是否断开) */
	inactivity.min_inactivity = 39;
	inactivity.max_inactivity = 1;

	/* 配置操作模式 (电源管理) */
#if SUPPORT_LIGHT_SLEEP
	mode.power_mode = wsm_power_mode_quiescent;
#else
	mode.power_mode = wsm_power_mode_active;
#endif
	mode.disableMoreFlagUsage = ATBM_TRUE;

	/* 配置 MAC 实例标志 */
	if (priv->if_id)
		start.mode |= WSM_FLAG_MAC_INSTANCE_1;
	else
		start.mode &= ~WSM_FLAG_MAC_INSTANCE_1;

	/* 配置次信道 (40MHz 模式) */
	if(priv->hw_priv->channel_type	== ATBM_NL80211_CHAN_HT40PLUS){
		config->secondary_channel=1;//above bandwidth
	}else if(priv->hw_priv->channel_type	== ATBM_NL80211_CHAN_HT40MINUS){
		config->secondary_channel=-1;//below bandwidth
	}

	/* 步骤 1: 启动 Beacon 发送 */
	priv->special_beacon_set = 0;
	atbmwifi_ap_start_beacon(priv);

	/* 初始化省电管理相关变量 */
	priv->tx_multicast = ATBM_FALSE;
	priv->aid0_bit_set = ATBM_FALSE;
	priv->buffered_multicasts = ATBM_FALSE;
	priv->pspoll_mask = 0;
	priv->link_id_uapsd_mask=0;

	/* 设置 SSID */
	start.ssidLength = priv->config.ssid_len;
	atbm_memcpy(&start.ssid[0], priv->config.ssid, start.ssidLength);
	atbm_memset(&priv->link_id_db[0], 0, sizeof(priv->link_id_db));

	wifi_printk(WIFI_CONNECT, "[AP] ch: %d(%d), bcn: %d(%d), "
		"brt: 0x%x, ssid: %s %d\n",
		start.channelNumber, start.band,
		start.beaconInterval, start.DTIMPeriod,
		start.basicRateSet,
		start.ssid,priv->if_id);

	/* 步骤 2: 发送 WSM_START 命令到固件 */
	ret = wsm_start(hw_priv, &start, priv->if_id);
	wsm_set_inactivity(hw_priv, &inactivity, priv->if_id);
	if (!ret) {
		priv->join_status = ATBMWIFI__JOIN_STATUS_AP;
		atbmwifi_event_uplayer(priv,ATBM_WIFI_ENABLE_NET_EVENT,ATBM_NULL);
	}

	/* 步骤 3: 设置块确认策略 */
#if ATBM_DRIVER_PROCESS_BA
	if(!hw_priv->driver_setup_ba)
#endif
	{
		wsm_set_block_ack_policy(hw_priv,
			hw_priv->ba_tid_tx_mask,
			hw_priv->ba_tid_rx_mask,
			priv->if_id);
	}

	/* 步骤 4: 设置操作模式 */
	wsm_set_operational_mode(hw_priv, &mode, priv->if_id);
	return ret;
}

atbm_void atbmwifi_ap_deauth_sta(struct atbmwifi_vif *priv,atbm_uint8 link_id,int reason_code)
{

	struct atbmwifi_sta_priv * sta_priv = atbmwifi_sta_find_form_hard_linkid(priv,priv->connect_timer_linkid);
	if(sta_priv){		
		wifi_printk(WIFI_ALWAYS,"atbmwifi_ap_deauth_sta\n");
		atbmwifi_ieee80211_tx_mgmt_deauth(priv,sta_priv->mac,priv->bssid,reason_code);
		if(atbmwifi_ap_deauth(priv,sta_priv->mac, ATBM_TRUE) < 0){	
			atbmwifi_event_uplayer(priv,ATBM_WIFI_DEAUTH_EVENT,sta_priv->mac);
		}
	}
}
atbm_void atbmwifi_ap_join_timeout(atbm_void *data1,atbm_void *data2)
{	
	//atbmwifi_autoconnect(priv);
	struct atbmwifi_vif *priv=(struct atbmwifi_vif *)data1;
	wifi_printk(WIFI_WPA,"atbm: atbmwifi_ap_join_timeout(), ms=%d\n", atbm_GetOsTimeMs());
	atbmwifi_ap_deauth_sta(priv,priv->connect_timer_linkid,ATBM_WLAN_REASON_DISASSOC_STA_HAS_LEFT);
	priv->connect_timer_linkid = 0;
}
/**
 * @brief 解除所有 STA 的认证并清理 AP 资源
 *
 * @param priv 虚拟接口私有数据结构指针
 *
 * @note 该函数在关闭 AP 时被调用，主要完成以下工作：
 *       1. 删除组密钥
 *       2. 释放 BSS 信息元素
 *       3. 重置重排序功能
 *
 * @see atbmwifi_stop_ap() - 调用该函数清理资源
 */
atbm_void atbmwifi_ap_deauth_all(struct atbmwifi_vif *priv)
{
	priv->connect_timer_linkid = 0;

	/* 删除组密钥 */
	atbmwifi_del_key(priv,1,1);

	/* 释放 BSS 信息元素 */
	if(priv->bss.information_elements){
		atbm_kfree(priv->bss.information_elements);
		priv->bss.information_elements = ATBM_NULL;
		priv->bss.len_information_elements = 0;
	}

#if ATBM_PKG_REORDER
	/* 重置重排序功能 */
	atbm_reorder_func_reset(priv,0xff);
#endif
}

/**
 * @brief 停止 AP 模式
 *
 * @param priv 虚拟接口私有数据结构指针
 *
 * @note 这是 AP 模式关闭的核心函数，主要完成以下工作：
 *       1. 取消 AP 扫描工作
 *       2. 向所有连接的 STA 发送 Deauth/Disassoc 帧
 *       3. 刷新发送队列
 *       4. 解除所有 STA 的 Link ID 映射
 *       5. 清理 AP 资源
 *       6. 重置 Link ID 数据库
 *       7. 发送 WSM_RESET 命令到固件
 *       8. 释放 hostapd 资源
 *
 * @see atbmwifi_start_ap() - 启动 AP 模式
 */
atbm_void atbmwifi_stop_ap(struct atbmwifi_vif *priv)
{
	int i;
	atbm_uint8 cnt = 0;
	atbm_uint32 link_id_map = 0;

	struct atbmwifi_common *hw_priv = _atbmwifi_vifpriv_to_hwpriv(priv);
	struct atbmwifi_cfg *config = atbmwifi_get_config(priv);
	struct wsm_reset reset = {
		.reset_statistics = ATBM_TRUE,
		.link_id = 0,
	};
	struct wsm_operational_mode mode;

	priv->doing_stop_off = 1;
	atbm_memset(&mode, 0, sizeof(mode));

	/* 检查 AP 是否已启用 */
	if(!priv->enabled){
		wifi_printk(WIFI_ALWAYS,"atbmwifi_stop_ap drop\n");
		goto ap_off;
	}
	wifi_printk(WIFI_ALWAYS,"%s\n",__FUNCTION__);

	/* 检查当前模式是否为 AP 模式 */
	if(!atbmwifi_is_ap_mode(priv->iftype)){
		goto ap_off;
	}

	/* 步骤 1: 取消 AP 扫描工作 */
	atbm_cancel_work(hw_priv, priv->scan.ap_scan_work);

	/* 步骤 2: 向所有 STA 发送 Deauth/Disassoc 帧 */
	link_id_map = priv->link_id_map;
	wifi_printk(WIFI_ALWAYS,"%s link_id_map=0x%x\n",__FUNCTION__, link_id_map);
	atbmwifi_lock_queues(priv->hw_priv, priv, ATBM_LOCK_REASON_TX_FLUSH);
	atbmwifi_clear_queues_map(priv->hw_priv, priv->if_id, ATBM_BIT(0));

	for (i = 0; link_id_map; ++i) {
		if (link_id_map & ATBM_BIT(i)) {
			if(i > 0){
				/* 发送多次 Deauth/Disassoc 帧确保 STA 收到 */
				while(cnt++ < 3){
					atbmwifi_ieee80211_tx_mgmt_deauth(priv, priv->link_id_db[i-1].mac, priv->bssid, ATBM_WLAN_REASON_DISASSOC_AP_BUSY);
					atbmwifi_ieee80211_send_deauth_disassoc(priv, priv->link_id_db[i-1].mac,priv->bssid,
							   ATBM_IEEE80211_STYPE_DISASSOC,
							   ATBM_WLAN_REASON_DISASSOC_DUE_TO_INACTIVITY,
							   ATBM_NULL, ATBM_TRUE);
				}
				cnt = 0;
			}
			link_id_map &= ~ATBM_BIT(i);
		}
	}

	/* 步骤 3: 刷新发送队列 */
	__atbm_flush(hw_priv, ATBM_FALSE, priv->if_id, ATBM_BIT(0));
	atbmwifi_unlock_queues(priv->hw_priv, priv, ATBM_LOCK_REASON_TX_FLUSH);

	/* 配置操作模式 */
	mode.power_mode = wsm_power_mode_quiescent;
	mode.disableMoreFlagUsage = ATBM_TRUE;

	/* 步骤 4: 解除所有 STA 的 Link ID 映射 */
	for (i = 0; priv->link_id_map; ++i) {
		if (priv->link_id_map & ATBM_BIT(i)) {
			if(i > 0){
				atbmwifi_ap_deauth(priv, priv->link_id_db[i-1].mac, ATBM_TRUE);
			}
			priv->link_id_map &= ~ATBM_BIT(i);
		}
	}

	/* 步骤 5: 清理 AP 资源 */
	atbmwifi_ap_deauth_all(priv);
	priv->enabled = 0;
	priv->doing_stop_off = 0;

	/* 步骤 6: 重置 Link ID 数据库和状态 */
	atbm_memset(priv->link_id_db, 0,sizeof(priv->link_id_db));
	priv->sta_asleep_mask = 0;
	priv->buffered_set_mask = 0;
	priv->enable_beacon = ATBM_FALSE;
	priv->tx_multicast = ATBM_FALSE;
	priv->aid0_bit_set = ATBM_FALSE;
	priv->buffered_multicasts = ATBM_FALSE;
	priv->pspoll_mask = 0;

	/* 步骤 7: 发送 WSM_RESET 命令到固件 */
	reset.link_id = 0;
	wsm_reset(priv->hw_priv, &reset, priv->if_id);
	ATBM_WARN_ON_FUNC(wsm_set_operational_mode(priv->hw_priv, &mode, priv->if_id));

	/* 清理连接状态 */
	priv->connect.crypto_pairwise=0;
	priv->connect.crypto_group=0;
	priv->connect.encrype = 0;
	priv->assoc_ok = 0;
	priv->connect_ok = 0;
	priv->connect_timer_linkid = 0;
	atbmwifi_eloop_cancel_timeout(atbmwifi_mcast_timeout, (atbm_void *)priv, ATBM_NULL);
	atbmwifi_eloop_cancel_timeout(atbmwifi_ap_join_timeout, (atbm_void *)priv, ATBM_NULL);
	atbmwifi_eloop_cancel_timeout(atbmwifi_queued_timeout,(atbm_void *)hw_priv->tx_queue,ATBM_NULL);
	
	atbmwifi_event_uplayer(priv,ATBM_WIFI_DEAUTH_EVENT,atbm_broadcast_ether_addr);
	atbmwifi_event_uplayer(priv,ATBM_WIFI_DISENABLE_NET_EVENT,ATBM_NULL);
	priv->join_status = ATBMWIFI__JOIN_STATUS_PASSIVE;
	atbm_memset(config,0,sizeof(struct atbmwifi_cfg));
	atbm_clear_scan_req(priv);

	if(priv->extra_ie){
		atbm_kfree(priv->extra_ie);
		priv->extra_ie = ATBM_NULL;
	}
	free_hostapd(priv);
	priv->appdata = ATBM_NULL;
	atbm_mdelay(100);
ap_off:
	priv->iftype = ATBM_NUM_NL80211_IFTYPES;
}
/**
 * @brief 初始化 hostapd 组件
 *
 * @param priv 虚拟接口私有数据结构指针
 *
 * @note 该函数初始化 hostapd 数据结构，用于管理 AP 模式的认证和关联过程
 */
atbm_void atbmwifi_start_hostapd(struct atbmwifi_vif *priv)
{
	priv->appdata = init_hostapd(priv);
}

/**
 * @brief 启动 AP 模式
 *
 * @param priv 虚拟接口私有数据结构指针
 *
 * @note 这是 AP 模式启动的入口函数，主要完成以下工作：
 *       1. 设置 AP 模式类型 (普通 AP 或 P2P GO)
 *       2. 配置 BSS 参数 (P2P 模式)
 *       3. 设置 BSSID
 *       4. 初始化 hostapd
 *       5. 创建 AP 扫描工作项
 *
 * @see atbm_start_ap() - 实际的 AP 启动函数
 */
atbm_void atbmwifi_start_ap(struct atbmwifi_vif *priv)
{
	struct atbmwifi_common * hw_priv = priv->hw_priv;
	wifi_printk(WIFI_ALWAYS,"atbmwifi_start_ap++\n");
	priv->config.mode = (priv->iftype == ATBM_NL80211_IFTYPE_P2P_GO) ? WSM_START_MODE_P2P_GO : WSM_START_MODE_AP;

#if CONFIG_P2P
	if(priv->p2p_ap){
		priv->bss.sta_priv.wmm_used = 1;
		priv->bss.sta_priv.uapsd_supported = 0;
		priv->bss.sta_priv.ht = 1;
		priv->bss.parameter_set_count=1;
		priv->bss.channel_type = ATBM_NL80211_CHAN_HT20;
	}
#endif

	atbm_memcpy(priv->bssid ,priv->mac_addr,ATBM_ETH_ALEN);
	atbm_memcpy(priv->config.bssid,priv->bssid,ATBM_ETH_ALEN);

	priv->enabled = 1;
	priv->connect_timer_linkid = 0;
	atbmwifi_start_hostapd(priv);
	//atbm_InitTimer(&priv->connect_expire_timer,atbmwifi_ap_join_timeout,(atbm_void*)priv);
	priv->scan.ap_scan_work = atbm_init_work(hw_priv, atbmwifi_ap_scan_start,priv);
}
atbm_void atbmwifi_ap_scan_start(struct atbm_work_struct *work)
{
	struct atbmwifi_vif *priv=(struct atbmwifi_vif *)work;

	if(priv->scan.ApScan_in_process){
		return;
	}

	priv->scan.ApScan_in_process = 1;

	atbmwifi_init_scan_req(priv, ATBM_SCAN_24G_ACTIVE);
#if CONFIG_5G_SUPPORT
	if(atbmwifi_5g_supported(priv->hw_priv->chip_priv)){
		atbmwifi_init_scan_req(priv, ATBM_SCAN_5G_PASSAVE);
	}
#endif
	if(atbm_req_scan_start(priv) != 0){
		return;
	}
}

int ap_scan(struct atbmwifi_vif *priv)
{	
	struct atbmwifi_common *hw_priv = _atbmwifi_vifpriv_to_hwpriv(priv);
	
	atbm_queue_work(hw_priv,priv->scan.ap_scan_work);
	return 0;
}

int atbmwifi_ap_auto_process(struct atbmwifi_vif *priv)
{
	wifi_printk(WIFI_ALWAYS,"%s ApScan_in_process=%d\n",__FUNCTION__,priv->scan.ApScan_in_process);
	if(!priv->scan.ApScan_in_process){
		if(priv->scan_ret.info==ATBM_NULL){
			priv->scan_ret.info = (struct atbmwifi_scan_result_info *)atbm_kmalloc(sizeof(struct atbmwifi_scan_result_info) * MAX_SCAN_INFO_NUM,GFP_KERNEL);
			if(priv->scan_ret.info ==ATBM_NULL){
				wifi_printk(WIFI_ALWAYS,"scan malloc fail!");
				return -1;
			}
		}
		priv->scan_ret.len = 0;
		priv->scan.if_id = priv->if_id;
		priv->scan_no_connect = 1;
		return ap_scan(priv);
	}
	else {
		wifi_printk(WIFI_ALWAYS,"scan busy!please try later!");
		return -2;
	}
}
static int WEIGHT(int x){
	if((x)<=-75){ 
		return 0; 
	}else if((x)<=-65){ 
		return 1; 
	}else if((x)<=-50){
		return 2;
	}else if((x)>-50){
		return 3;
	}
	return 3;
}

#define NUM_AP_CHNN 13

#define WAIT_CMP(x) \
	while (x) { \
		atbm_mdelay(3000); \
	} \

static atbm_uint8 ReturnMin_InSort_1(atbm_uint8 * aCHList,atbm_uint8 *ChanArray,atbm_uint8 index,atbm_uint8 offset){
	atbm_uint8 i,j;
	atbm_uint8 minIndex;
		minIndex=i=0;
		for(j=i+1;j<index;++j){
			if(aCHList[ChanArray[j]-offset]<aCHList[ChanArray[minIndex]-offset])
				minIndex=j;
		}
	//	if(minIndex!=i)	
		//	AtbmSwap(&aCHList[ChanArray[i]-offset], &aCHList[ChanArray[minIndex]-offset]);

		//wifi_printk(WIFI_ALWAYS,"Array[ChanArray[%d] %d\n",i,aCHList[ChanArray[i]-offset]);
	//}
	//wifi_printk(WIFI_ALWAYS,"ReturnMin_InSort_1 minIndex=%d: %d\n",minIndex,ChanArray[minIndex]);
	return ChanArray[minIndex];
}	
/**
 * @brief AP 自动信道选择函数
 *
 * @param priv 虚拟接口私有数据结构指针
 * @param SetChan 输出参数，返回选定的信道号
 * @return int 0:成功, -1:失败
 *
 * @note 该函数实现 AP 自动信道选择算法，主要完成以下工作：
 *       1. 触发 AP 扫描，收集周围 AP 信息
 *       2. 基于扫描结果计算各信道的干扰权重
 *       3. 考虑信道占用率 (busy_ratio) 进行筛选
 *       4. 选择干扰最小的信道
 *
 * 算法原理:
 * - 对每个检测到的 AP，根据其 RSSI 计算对相邻信道的干扰权重
 * - 相邻信道的干扰随距离衰减 (±1信道: -2dB, ±2信道: -4dB, ±3信道: -8dB)
 * - 优先选择占用率低于 110 的信道
 * - 在候选信道中，选择总干扰权重最小的信道
 *
 * @see atbmwifi_ap_auto_process() - 触发 AP 自动扫描
 * @see ReturnMin_InSort_1() - 选择最小干扰信道
 */
int	atbm_autoChann_Select(struct atbmwifi_vif *priv,atbm_uint8 *SetChan)
{
	atbm_uint8 chanNum,index=0,ssidNum=0;
	atbm_uint8 aCHList[NUM_AP_CHNN]={0};        /* 各信道的干扰权重累计值 */
	atbm_uint8 MinChanNum[14]={1,2,3,4,5,6,7,8,9,10,11,12,13,14}; /* 候选信道列表 */

	WLAN_SCAN_RESULT *AutoScanBuf=ATBM_NULL;
	WLAN_BSS_INFO *bss_info;
	struct atbmwifi_scan_result_info *info;
	struct atbmwifi_common *hw_priv =priv->hw_priv;

	/* 步骤 1: 设置 AP 扫描标志 */
	hw_priv->ApScan_process_flag=1;

	/* 步骤 2: 等待 AP 使能并触发自动扫描 */
	WAIT_CMP(!priv->enabled);
	if(atbmwifi_ap_auto_process(priv)){
		return -1;
	}

	/* 步骤 3: 等待扫描完成 */
	WAIT_CMP(priv->scan.ApScan_in_process);

	/* 步骤 4: 根据扫描结果选择信道 */
	/* 如果所有信道都没有 AP，默认选择信道 2 */
	if(priv->scan_ret.len==0){
		*SetChan=2;
	}else{
		/* 分配扫描结果缓冲区 */
		AutoScanBuf=(WLAN_SCAN_RESULT*)atbm_kmalloc(priv->scan_ret.len*sizeof(WLAN_SCAN_RESULT),GFP_KERNEL);
		if(atbm_unlikely(!AutoScanBuf)){
			return -1;
		}
		bss_info =  (WLAN_BSS_INFO *)(&AutoScanBuf->bss_info[0]);

		/* 步骤 4.1: 计算各信道的干扰权重 */
		for(ssidNum=0;ssidNum<priv->scan_ret.len;ssidNum++){
			info = priv->scan_ret.info + ssidNum;
			bss_info->chanspec      = info->channel;
			bss_info->RSSI = info->rssi;

			/* 根据信道号计算相邻信道的干扰权重 */
			switch(bss_info->chanspec){
				case 1:
					if(bss_info->chanspec+1 >= NUM_AP_CHNN) break;
					aCHList[bss_info->chanspec+1]+=WEIGHT(bss_info->RSSI-2);
					if(bss_info->chanspec+2 >= NUM_AP_CHNN) break;
					aCHList[bss_info->chanspec+2]+=WEIGHT(bss_info->RSSI-4);
					if(bss_info->chanspec+3 >= NUM_AP_CHNN) break;
					aCHList[bss_info->chanspec+3]+=WEIGHT(bss_info->RSSI-8);
					break;
				case 2:
					if(bss_info->chanspec-1 >= NUM_AP_CHNN) break;
					aCHList[bss_info->chanspec-1]+=WEIGHT(bss_info->RSSI-2);
					if(bss_info->chanspec+1 >= NUM_AP_CHNN) break;
					aCHList[bss_info->chanspec+1]+=WEIGHT(bss_info->RSSI-2);
					if(bss_info->chanspec+2 >= NUM_AP_CHNN) break;
					aCHList[bss_info->chanspec+2]+=WEIGHT(bss_info->RSSI-4);
					if(bss_info->chanspec+3 >= NUM_AP_CHNN) break;
					aCHList[bss_info->chanspec+3]+=WEIGHT(bss_info->RSSI-8);
					break;
				case 3:
					if(bss_info->chanspec-2 >= NUM_AP_CHNN) break;
					aCHList[bss_info->chanspec-2]+=WEIGHT(bss_info->RSSI-4);
					if(bss_info->chanspec-1 >= NUM_AP_CHNN) break;
					aCHList[bss_info->chanspec-1]+=WEIGHT(bss_info->RSSI-2);
					if(bss_info->chanspec+1 >= NUM_AP_CHNN) break;
					aCHList[bss_info->chanspec+1]+=WEIGHT(bss_info->RSSI-2);
					if(bss_info->chanspec+2 >= NUM_AP_CHNN) break;
					aCHList[bss_info->chanspec+2]+=WEIGHT(bss_info->RSSI-4);
					if(bss_info->chanspec+3 >= NUM_AP_CHNN) break;
					aCHList[bss_info->chanspec+3]+=WEIGHT(bss_info->RSSI-8);
					break;
				case 4:
				case 5:
				case 6:
				case 7:
				case 8:
				case 9:
				case 10:
					if(bss_info->chanspec-3 >= NUM_AP_CHNN) break;
					aCHList[bss_info->chanspec-3]+=WEIGHT(bss_info->RSSI-8);
					if(bss_info->chanspec-2 >= NUM_AP_CHNN) break;
					aCHList[bss_info->chanspec-2]+=WEIGHT(bss_info->RSSI-4);
					if(bss_info->chanspec-1 >= NUM_AP_CHNN) break;
					aCHList[bss_info->chanspec-1]+=WEIGHT(bss_info->RSSI-2);
					if(bss_info->chanspec+1 >= NUM_AP_CHNN) break;
					aCHList[bss_info->chanspec+1]+=WEIGHT(bss_info->RSSI-2);
					if(bss_info->chanspec+2 >= NUM_AP_CHNN) break;
					aCHList[bss_info->chanspec+2]+=WEIGHT(bss_info->RSSI-4);
					if(bss_info->chanspec+3 >= NUM_AP_CHNN) break;
					aCHList[bss_info->chanspec+3]+=WEIGHT(bss_info->RSSI-8);
					break;
				case 11:
					if(bss_info->chanspec-3 >= NUM_AP_CHNN) break;
					aCHList[bss_info->chanspec-3]+=WEIGHT(bss_info->RSSI-8);
					if(bss_info->chanspec-2 >= NUM_AP_CHNN) break;
					aCHList[bss_info->chanspec-2]+=WEIGHT(bss_info->RSSI-4);
					if(bss_info->chanspec-1 >= NUM_AP_CHNN) break;
					aCHList[bss_info->chanspec-1]+=WEIGHT(bss_info->RSSI-2);
					if(bss_info->chanspec+1 >= NUM_AP_CHNN) break;
					aCHList[bss_info->chanspec+1]+=WEIGHT(bss_info->RSSI-2);
					if(bss_info->chanspec+2 >= NUM_AP_CHNN) break;
					aCHList[bss_info->chanspec+2]+=WEIGHT(bss_info->RSSI-4);
					break;
				case 12:
					if(bss_info->chanspec-3 >= NUM_AP_CHNN) break;
					aCHList[bss_info->chanspec-3]+=WEIGHT(bss_info->RSSI-8);
					if(bss_info->chanspec-2 >= NUM_AP_CHNN) break;
					aCHList[bss_info->chanspec-2]+=WEIGHT(bss_info->RSSI-4);
					if(bss_info->chanspec-1 >= NUM_AP_CHNN) break;
					aCHList[bss_info->chanspec-1]+=WEIGHT(bss_info->RSSI-2);
					if(bss_info->chanspec+1 >= NUM_AP_CHNN) break;
					aCHList[bss_info->chanspec+1]+=WEIGHT(bss_info->RSSI-2);
					break;
				case 13:
					if(bss_info->chanspec-3 >= NUM_AP_CHNN) break;
					aCHList[bss_info->chanspec-3]+=WEIGHT(bss_info->RSSI-8);
					if(bss_info->chanspec-2 >= NUM_AP_CHNN) break;
					aCHList[bss_info->chanspec-2]+=WEIGHT(bss_info->RSSI-4);
					if(bss_info->chanspec+1 >= NUM_AP_CHNN) break;
					aCHList[bss_info->chanspec-1]+=WEIGHT(bss_info->RSSI-2);						
					break;
				default:
					wifi_printk(WIFI_ALWAYS,"Channel Num > 13\n");
					break;
				}
			/* 累加当前信道的干扰权重 */
			aCHList[bss_info->chanspec]+=WEIGHT(bss_info->RSSI);

			bss_info++;
		}

		/* 步骤 4.2: 根据信道占用率筛选候选信道 */
		for(chanNum=1;chanNum<NUM_AP_CHNN+1;chanNum++){
			/* 选择占用率低于 110 的信道作为候选 */
			if(hw_priv->busy_ratio[chanNum]<110){
				MinChanNum[index]=chanNum;
				index++;
			}
		}

		/* 步骤 4.3: 在候选信道中，选择干扰最小的信道 */
		if(index==0){
			/* 如果没有占用率低于 110 的信道，
			 * 从所有信道中选择占用率最小的
			 */
			*SetChan=ReturnMin_InSort_1(&hw_priv->busy_ratio[1],&MinChanNum[0],NUM_AP_CHNN,1);
		}else{
			/* 在候选信道中，选择总干扰权重最小的信道 */
			*SetChan=ReturnMin_InSort_1(&aCHList[MinChanNum[0]],&MinChanNum[0],index,MinChanNum[0]);
		}
	}

	/* 步骤 5: 释放扫描结果缓冲区 */
	atbm_kfree(AutoScanBuf);
	wifi_printk(WIFI_ALWAYS,"SetChan =%d\n",*SetChan);
	return 0;
}

#if CONFIG_ATBM_SUPPORT_CSA
int atbmwifi_do_csa(struct atbmwifi_vif *priv,struct atbm_ieee80211_csa_request *request)
{
	struct atbmwifi_common *hw_priv = _atbmwifi_vifpriv_to_hwpriv(priv);
	struct wsm_csa csa;

	if(!priv->enabled){
		wifi_printk(WIFI_DBG_MSG, "[%s] has been disabled\n",priv->if_name);
		return -1;
	}

	if(atbm_bh_is_term(hw_priv)){
		return -1;
	}

	csa.flags       = request->start == true ? WSM_CSA_FLAGS_START : 0;
	csa.csa_mode    = request->ie.mode;
	csa.csa_channel = request->ie.new_ch_num;
	csa.csa_count   = request->ie.count;
	
	return wsm_csa_req(hw_priv,&csa,priv->if_id);
}
#endif /* CONFIG_ATBM_SUPPORT_CSA */
