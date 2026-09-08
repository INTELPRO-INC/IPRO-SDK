/**************************************************************************************************************
 * altobeam RTOS wifi hmac source code 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 *
 *  The source code contains proprietary information of AltoBeam, and shall not be distributed, 
 *  copied, reproduced, or disclosed in whole or in part without prior written permission of AltoBeam.
 *
 * 文件说明:
 *   本文件包含 WiFi 连接流程的核心函数，包括 STA 模式下连接 AP、AP 模式启动、
 *   扫描处理、事件处理、断开连接等功能。
 *
 * 主要函数:
 *   - wifi_ConnectAP_vif(): STA 连接 AP 的入口函数
 *   - wpa_connect_ap(): 触发 WPA 连接事件
 *   - atbmwifi_wpa_event_associte_ap(): WPA 事件处理，启动扫描和关联
 *   - atbmwifi_scan_process(): 扫描处理函数
 *   - atbmwifi_event_handler(): WSM 事件处理函数
 *   - atbm_sta_deauth(): 断开连接函数
 *
*****************************************************************************************************************/
#include "atbm_hal.h"
extern atbm_void sta_scan_start_timer_func(atbm_void *data1,atbm_void *data2);
extern struct atbmwifi_common g_hw_prv;
#define MAX_SSID_LEN 32
#define MAX_PASSWORD_LEN 64
#define MAX_INTF_ID 2
#define ATBM_CHECK_AGRV_AVAILABLE(PARAM_1,PARAM_2) do{ \
		atbm_uint8 ret;\
		if(PARAM_1>MAX_SSID_LEN){ \
			ret=-1; \
		} \
		if(PARAM_2>MAX_PASSWORD_LEN){\
			ret= -2; \
		} \
	}while(0)
	
#define ATBM_CHECK_AGRV_AVAILABLE_POINTER(PARAM_1,PARAM_2) do{ \
				atbm_uint8 ret;\
				if(PARAM_1==ATBM_NULL){ \
					ret=-1; \
					goto EXIT;\
				} \
				if(PARAM_2==ATBM_NULL){\
					ret= -2; \
					goto EXIT;\
				} \
			}while(0)

/**************************************************************************************************************
 * 函数名: wifi_ConnectAP_vif
 *
 * 功能描述:
 *   STA 模式下连接指定 AP 的入口函数。该函数是应用层调用的连接 API，负责保存连接配置
 *   （SSID、密码、安全类型等），然后触发 WPA 连接流程。
 *
 * 在连接流程中的位置:
 *   阶段 1: 应用层入口
 *   这是连接流程的起点，由应用层调用，触发整个连接过程
 *
 * 参数说明:
 *   @if_id:    接口 ID（0: 主接口, 1: 次接口）
 *   @ssid:     目标 AP 的 SSID（网络名称）
 *   @ssidlen:  SSID 长度（最大 32 字节）
 *   @password: 连接密码（对于开放网络可为 NULL）
 *   @passwdlen: 密码长度（开放网络为 0）
 *   @key_mgmt: 安全类型，参见 ATBM_SECURITY_TYPE 枚举:
 *              - ATBM_KEY_NONE: 开放网络
 *              - ATBM_KEY_WEP: WEP 加密
 *              - ATBM_KEY_WEP_SHARE: WEP 共享密钥
 *              - ATBM_KEY_WPA: WPA-PSK
 *              - ATBM_KEY_WPA2: WPA2-PSK
 *              - ATBM_KEY_MIX: WPA/WPA2 混合模式
 *              - ATBM_KEY_SAE: WPA3-SAE
 *
 * 返回值:
 *   0:  成功
 *   -1: 接口无效
 *
 * 处理流程:
 *   1. 参数验证：检查 SSID 和密码长度
 *   2. 获取虚拟接口指针
 *   3. 保存连接配置到 priv->config 结构体
 *   4. 设置认证算法（Open/Shared Key）
 *   5. 设置 802.11w 管理帧保护（如果是 SAE）
 *   6. 调用 wpa_connect_ap() 触发 WPA 连接事件
 *
 * 连接流程概述:
 *   wifi_ConnectAP_vif() → wpa_connect_ap() → WPA 事件队列 →
 *   atbmwifi_wpa_event_associte_ap() → 扫描 → 找到 AP → Join →
 *   认证 → 关联 → 4 次握手 → 连接成功
 *
 * 注意事项:
 *   - 调用此函数前需确保 WiFi 已初始化并处于 STA 模式
 *   - 连接过程是异步的，需要通过事件回调获取连接结果
 *   - 如果已连接其他 AP，会先断开再连接新 AP
 *
 * 相关函数:
 *   - wpa_connect_ap(): 触发 WPA 连接事件
 *   - atbmwifi_wpa_event_associte_ap(): WPA 事件处理
 *   - atbmwifi_sta_scan(): 启动扫描
 *   - atbmwifi_join_start(): 发起 Join 请求
 *
 **************************************************************************************************************/
atbm_int32 wifi_ConnectAP_vif(atbm_uint8 if_id,atbm_uint8 * ssid,int ssidlen,atbm_uint8 * password,int passwdlen,ATBM_SECURITY_TYPE key_mgmt)
{
	atbm_int32 ret = 0;
	struct atbmwifi_vif *priv;
	ATBM_CHECK_AGRV_AVAILABLE(ssidlen,passwdlen);

	priv= _atbmwifi_hwpriv_to_vifpriv(&g_hw_prv,if_id);
	
	if(priv == ATBM_NULL)
		return -1;
	
	atbm_memset(priv->config.ssid, 0, sizeof(priv->config.ssid));
	atbm_memset(priv->config.password, 0, sizeof(priv->config.password));

	atbm_memcpy(priv->config.ssid,ssid,ssidlen);
	priv->config.ssid_len = priv->ssid_length= ssidlen;
	atbm_memcpy(priv->ssid,priv->config.ssid,ssidlen);
	if(passwdlen){
		atbm_memcpy(priv->config.password,password,passwdlen);
	}
	if(ATBM_KEY_WEP_SHARE != key_mgmt){
		priv->config.auth_alg = ATBM_WLAN_AUTH_OPEN;
	}
	else {
		priv->config.auth_alg = ATBM_WLAN_AUTH_SHARED_KEY;
	}
	priv->config.password_len = passwdlen;
	priv->config.privacy = passwdlen?1:0;
	priv->config.key_mgmt = key_mgmt;
	priv->config.key_id = 0;
	priv->scan_expire = INITIAL_SCAN_EXPIRE;
	priv->connect_expire=0;
	//atbmwifi_autoconnect(priv);
#if CONFIG_IEEE80211W
	if(priv->config.key_mgmt == ATBM_KEY_SAE)
		priv->config.ieee80211w = ATBM_MGMT_FRAME_PROTECTION_REQUIRED;
	else
		priv->config.ieee80211w = ATBM_MGMT_FRAME_PROTECTION_OPTIONAL;
#endif
	wpa_connect_ap(priv);
	return ret;
}

int wifi_SetAPConfig(struct atbmwifi_vif *priv,atbm_uint8 * ssid,int ssidlen,atbm_uint8 * password,int passwdlen,int channel,ATBM_SECURITY_TYPE key_mgmt,ATBM_BOOL ssidBcst){
	if(ssidlen == 0 || ssidlen >32){
		wifi_printk(WIFI_DBG_ERROR,"wifi_StartAP_vif ssid len is zero err\n");
		return -1;
	}

	if(!atbmwifi_5g_supported(priv->hw_priv->chip_priv) && (channel > 14)){
		wifi_printk(WIFI_DBG_ERROR,"wifi_StartAP_vif channel is zero err\n");
		return -1;
	}

	if(passwdlen>64){
		return -1;
	}

	atbm_memcpy(priv->config.ssid,ssid,ssidlen);
	priv->config.ssid_len = priv->ssid_length= ssidlen;
	atbm_memcpy(priv->ssid,priv->config.ssid,ssidlen);
	if(passwdlen){
		atbm_memcpy(priv->config.password,password,passwdlen);
		priv->config.password_len = passwdlen;
		priv->config.privacy = 1;
	}else{
		priv->config.privacy = 0;
	}
	priv->config.key_mgmt = key_mgmt;
	priv->config.hide_ssid = ssidBcst;
	/*Other Ap initial*/
	priv->config.beaconInterval = TEST_BEACON_INTV; 
	priv->config.DTIMPeriod = TEST_DTIM_INTV;
	priv->config.preambleType = TEST_SHORT_PREAMBLE;
	///////////////////////////////////////////////////
	/*If use the ap auto channel function,user need set channle 0*/
	//channel=0;
	channel = channel ? channel : atbmwifi_iee80211_peerif_channel(priv);
	if(channel == 0){
		atbm_autoChann_Select(priv,&priv->config.channel_index);
	}else{
		priv->config.channel_index = channel;
#if BW_40M_SUPPORT
#if CONFIG_5G_SUPPORT
		if(channel >= 36){
			switch(channel){
				case 36:
				case 44:
				case 52:
				case 60:
				case 100:
				case 108:
				case 116:
				case 124:
				case 132:
				case 149:
				case 157:
					priv->hw_priv->channel_type = ATBM_NL80211_CHAN_HT40PLUS;
					priv->bss.channel_type = ATBM_NL80211_CHAN_HT40PLUS;
					break;
				case 40:
				case 48:
				case 56:
				case 64:
				case 104:
				case 112:
				case 120:
				case 128:
				case 136:
				case 153:
				case 161:
					priv->hw_priv->channel_type = ATBM_NL80211_CHAN_HT40MINUS;
					priv->bss.channel_type = ATBM_NL80211_CHAN_HT40MINUS;
					break;
				case 140:
				case 165:
					priv->hw_priv->channel_type = ATBM_NL80211_CHAN_HT20;
					priv->bss.channel_type = ATBM_NL80211_CHAN_HT20;	
					break;
				default:
					wifi_printk(WIFI_DBG_ERROR, "Unsupported chan %d\n", channel);
					return -1;
			}
		}else
#endif
		if(channel >= 10){
			priv->hw_priv->channel_type = ATBM_NL80211_CHAN_HT40MINUS;
			priv->bss.channel_type = ATBM_NL80211_CHAN_HT40MINUS;
		}else if(channel <= 2){
			priv->hw_priv->channel_type = ATBM_NL80211_CHAN_HT40PLUS;
			priv->bss.channel_type = ATBM_NL80211_CHAN_HT40PLUS;
		}else{
			priv->bss.channel_type = atbmwifi_iee80211_peerif_channel_type(priv);
		}
#else
		priv->bss.channel_type = g_hw_prv.channel_type;
#endif
	}
	if(priv->config.channel_index >= 36){
		priv->config.basicRateSet = ATBM_BIT(6)|ATBM_BIT(8)|ATBM_BIT(10);
	}else{
		priv->config.basicRateSet = ATBM_BIT(0)|ATBM_BIT(1)|ATBM_BIT(2)|ATBM_BIT(3);
	}
#if CONFIG_ATBM_SUPPORT_CSA
	priv->config.cs_count = 5;
	priv->config.block_tx = 1;
#endif
	return 0;
}

atbm_void wifi_StartAP_vif(atbm_uint8 if_id,atbm_uint8 * ssid,int ssidlen,atbm_uint8 * password,int passwdlen,int channel,ATBM_SECURITY_TYPE key_mgmt,ATBM_BOOL ssidBcst)
{
	struct atbmwifi_vif *priv;
	priv= _atbmwifi_hwpriv_to_vifpriv(&g_hw_prv,if_id);

	wifi_printk(WIFI_ALWAYS,"%s @@@@ if_id=%d, scan.if_id=%d\n", __func__, priv->if_id, priv->scan.if_id);

	if(wifi_SetAPConfig(priv, ssid, ssidlen, password, passwdlen, channel, key_mgmt, ssidBcst)){
		return;
	}
	atbmwifi_wpa_event_queue((atbm_wpa_event_data)priv,(atbm_wpa_event_data)ATBM_NULL,(atbm_wpa_event_data)ATBM_NULL,WPA_EVENT__HOSTAPD_START,ATBM_WPA_EVENT_ACK);
}

 atbm_void atbmwifi_wpa_event_start_ap(struct atbmwifi_vif *priv)
{
	struct atbmwifi_cfg *config = NULL;

	if(priv == ATBM_NULL){		
		wifi_printk(WIFI_ALWAYS, "%s priv is null\n", __func__);
		return;
	}

	config = atbmwifi_get_config(priv);
	if(atbmwifi_iee80211_check_combination(priv,config->channel_index) == ATBM_FALSE){
        wifi_printk(WIFI_ALWAYS, "%s AP(ch:%d) and STA is not work at same channel\n", __func__, config->channel_index);
		return;
	}
	hostapd_start(priv);
}
/*
change wifi powersave mode
*/
 atbm_void wifi_ChangePsMode(struct atbmwifi_vif *priv,atbm_uint8 enable,atbm_uint8 ds_timeout)
{
	atbmwifi_set_pm(priv,enable,ds_timeout);
}

 atbm_void AT_WDisConnect_vif(struct atbmwifi_vif *priv,char *pLine)
{
	if(!priv->enabled){
		wifi_printk(WIFI_ALWAYS,"not support not enabled!\n");
		return;
	}
	if(!atbmwifi_is_sta_mode(priv->iftype)) {
		return;
	}
	priv->auto_connect_when_lost = 0;
	atbm_sta_deauth(priv, ATBM_TRUE);
	atbm_mdelay(100);
	wpa_disconnect(priv);
	atbm_mdelay(200);
}

 atbm_void AT_WDisConnect(char *pLine)
{		
	struct atbmwifi_common *hw_priv;
	struct atbmwifi_vif *priv;
	hw_priv=&g_hw_prv;
	priv= _atbmwifi_hwpriv_to_vifpriv(&g_hw_prv,0);
	if(priv!=ATBM_NULL){
		AT_WDisConnect_vif(priv,pLine);
	}
}
/*
**************************************************************
*@breif		���ӵ�ָ����ap
*@params	netdev:�����豸
*			essid:��������
*			key_mgmt:��Կ���ܷ�ʽ
*@retval		none
**************************************************************
*/
extern int atbmwifi_sta_scan(struct atbmwifi_vif *priv);

/**************************************************************************************************************
 * 函数名: wpa_connect_ap
 *
 * 功能描述:
 *   触发 WPA 连接事件，将连接请求加入 WPA 事件队列。该函数是连接流程中的关键桥梁，
 *   将同步的 API 调用转换为异步的事件处理。
 *
 * 在连接流程中的位置:
 *   阶段 2: 事件触发
 *   在 wifi_ConnectAP_vif() 中调用，将连接请求加入事件队列
 *
 * 参数说明:
 *   @priv: 虚拟接口指针，包含接口的配置和状态信息
 *
 * 返回值:
 *   0: 成功（事件已加入队列）
 *
 * 处理流程:
 *   1. 调用 atbmwifi_wpa_event_queue() 将连接事件加入队列
 *   2. 事件类型: WPA_EVENT__SUPPLICANT_START_CONNECT
 *   3. 事件模式: ATBM_WPA_EVENT_NOACK（不需要确认）
 *
 * 事件处理:
 *   WPA 事件线程会从队列中取出事件，调用 atbmwifi_wpa_event_associte_ap() 处理
 *
 * 注意事项:
 *   - 函数立即返回，实际连接在事件线程中异步进行
 *   - 连接结果通过事件回调通知应用层
 *
 * 相关函数:
 *   - wifi_ConnectAP_vif(): 调用此函数
 *   - atbmwifi_wpa_event_queue(): 事件队列函数
 *   - atbmwifi_wpa_event_associte_ap(): 事件处理函数
 *
 **************************************************************************************************************/
int wpa_connect_ap(struct atbmwifi_vif *priv)
{
	atbmwifi_wpa_event_queue((atbm_wpa_event_data)priv,(atbm_wpa_event_data)ATBM_NULL,
		(atbm_wpa_event_data)ATBM_NULL,WPA_EVENT__SUPPLICANT_START_CONNECT,ATBM_WPA_EVENT_NOACK);
	return 0;
}

/**************************************************************************************************************
 * 函数名: atbmwifi_wpa_event_associte_ap
 *
 * 功能描述:
 *   WPA 事件处理函数，处理连接 AP 的事件。该函数在 WPA 事件线程中被调用，负责启动扫描流程，
 *   查找目标 AP，并准备进行关联。
 *
 * 在连接流程中的位置:
 *   阶段 3: 扫描准备
 *   在 WPA 事件线程中调用，响应 WPA_EVENT__SUPPLICANT_START_CONNECT 事件
 *
 * 参数说明:
 *   @priv: 虚拟接口指针，包含接口的配置和状态信息
 *
 * 返回值:
 *   0:  成功
 *   -1: 扫描正在进行中
 *
 * 处理流程:
 *   1. 检查扫描状态：如果扫描正在进行，返回 -1
 *   2. 参数验证：检查 wpa_s 和 config 是否有效
 *   3. 断开现有连接：如果已关联，先发送 Deauth
 *   4. 设置自动重连标志
 *   5. 初始化 WPA 状态为 SCANNING
 *   6. 初始化信道类型和 Extra IE
 *   7. 启动扫描：
 *      - 快速连接模式：直接发起 Join
 *      - 普通模式：启动扫描，注册扫描超时定时器
 *   8. 释放 Extra IE 内存
 *
 * 扫描完成后:
 *   扫描完成后，如果找到目标 AP，会触发 Join 流程（atbmwifi_join_start）
 *
 * 注意事项:
 *   - 如果已连接其他 AP，会先断开
 *   - 扫描是异步的，扫描结果通过回调处理
 *   - 支持快速连接模式（跳过扫描）
 *
 * 相关函数:
 *   - wpa_connect_ap(): 触发此事件
 *   - atbmwifi_sta_scan(): 启动扫描
 *   - atbm_sta_deauth(): 断开现有连接
 *   - atbmwifi_join_start(): 扫描完成后发起 Join
 *
 **************************************************************************************************************/
int atbmwifi_wpa_event_associte_ap(struct atbmwifi_vif *priv)
{	
	struct wpa_supplicant *wpa_s = (struct wpa_supplicant *)priv->appdata;
	struct atbmwifi_cfg *config = atbmwifi_get_config(priv);

	if(priv->scan.in_progress){
		return -1;
	}

	ATBM_CHECK_AGRV_AVAILABLE_POINTER(wpa_s,config);
	priv->scan_no_connect_back = priv->scan_no_connect = 0;

	priv->auto_connect_when_lost = 0;
	if(wpa_s->wpa_state >= ATBM_WPA_ASSOCIATED)
		atbm_sta_deauth(priv, ATBM_TRUE);
#if CONFIG_P2P
	if(!priv->p2pdata)
#endif
		priv->auto_connect_when_lost = 1;
#if (FAST_CONNECT_MODE == 0) && (FAST_CONNECT_NO_SCAN == 0)
	config->psk_set=0;
#endif
	wpa_s->wpa_state = ATBM_WPA_SCANNING;
	priv->bss.channel_type = CH_OFF_20;
	atbm_spin_lock_init(&priv->bss.sta_priv.lock);
	wpa_comm_init_extra_ie(priv);
#if FAST_CONNECT_NO_SCAN
	if(!priv->fast_conn_noscan){
		if(!priv->scan.in_progress)
			atbmwifi_sta_scan(priv);
	}else{
		atbm_queue_work(_atbmwifi_vifpriv_to_hwpriv(priv),priv->join_work);
	}
#else
	if(!priv->scan.in_progress){		
		atbmwifi_eloop_register_timeout(0,priv->scan_expire*1000,sta_scan_start_timer_func,(atbm_void *)priv,ATBM_NULL);
	}
	atbmwifi_sta_scan(priv);
#endif
	atbm_kfree(priv->extra_ie);
	priv->extra_ie = ATBM_NULL;
	priv->extra_ie_len = 0;
EXIT:
	return 0;
}

/**************************************************************************************************************
 * 函数名: atbmwifi_scan_process
 *
 * 功能描述:
 *   扫描处理函数，用于启动 WiFi 扫描。该函数是应用层调用的扫描 API，负责检查扫描条件，
 *   分配扫描结果缓冲区，并启动扫描流程。
 *
 * 在连接流程中的位置:
 *   阶段 3: 扫描阶段
 *   可以独立调用进行扫描，也可以在连接流程中自动调用
 *
 * 参数说明:
 *   @priv: 虚拟接口指针，包含接口的配置和状态信息
 *
 * 返回值:
 *   0:  成功启动扫描
 *   -1: 不支持在 AP 模式下扫描
 *   -2: 接口未使能
 *   -3: SmartConfig 正在进行
 *   -4: 内存分配失败
 *   -5: 扫描正在进行中
 *   -6: 正在进行 Join 操作
 *
 * 处理流程:
 *   1. 模式检查：只支持 STA 模式
 *   2. 状态检查：接口必须已使能
 *   3. SmartConfig 检查：SmartConfig 期间不能扫描
 *   4. Join 状态检查：Join 期间不能扫描
 *   5. 扫描状态检查：如果扫描正在进行，返回错误
 *   6. 分配扫描结果缓冲区
 *   7. 初始化扫描参数
 *   8. 调用 atbmwifi_sta_scan() 启动扫描
 *
 * 扫描结果:
 *   扫描结果保存在 priv->scan_ret.info 数组中
 *   最大扫描结果数量: MAX_SCAN_INFO_NUM
 *
 * 注意事项:
 *   - 扫描是异步的，扫描完成通过事件通知
 *   - 扫描期间不能进行其他操作
 *   - 扫描结果缓冲区需要在扫描完成后释放
 *
 * 相关函数:
 *   - atbmwifi_sta_scan(): 实际的扫描函数
 *   - atbmwifi_scan(): 扫描的另一个入口（通过事件队列）
 *
 **************************************************************************************************************/
 int atbmwifi_scan_process(struct atbmwifi_vif *priv)
{
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
		if(priv->scan_ret.info==ATBM_NULL){
			priv->scan_ret.info = (struct atbmwifi_scan_result_info *)atbm_kmalloc(sizeof(struct atbmwifi_scan_result_info) * MAX_SCAN_INFO_NUM,GFP_KERNEL);
			if(priv->scan_ret.info ==ATBM_NULL){
				wifi_printk(WIFI_ALWAYS,"scan malloc fail!");
				return -4;
			}
		}
		priv->scan_ret.len = 0;
		priv->scan.if_id = priv->if_id;
		priv->scan_expire = INITIAL_SCAN_EXPIRE;
		priv->scan_no_connect_back = priv->scan_no_connect;
		priv->scan_no_connect = 1;
		return atbmwifi_sta_scan(priv);
	}
	else {
		wifi_printk(WIFI_ALWAYS,"scan busy!please try later!");
		return -5;
	}
}
/*********************station interface**********************/
/****************************************************
Function Name: atbmwifi_scan
Return: NULL
******************************************************/

 int atbmwifi_scan(struct atbmwifi_vif *priv)
{
	 atbmwifi_wpa_event_queue((atbm_wpa_event_data)priv,(atbm_wpa_event_data)ATBM_NULL,
			 (atbm_wpa_event_data)ATBM_NULL,WPA_EVENT__SUPPLICANT_SCAN,ATBM_WPA_EVENT_ACK);

	return 0;
}
/**************************************************************************************************************
 * 函数名: atbmwifi_event_handler
 *
 * 功能描述:
 *   WSM 事件处理函数，处理来自硬件/固件的各种事件。该函数是事件驱动架构的核心，
 *   负责处理 BSS 丢失、BSS 恢复、不活动检测、电源管理错误、CSA 等事件。
 *
 * 在连接流程中的位置:
 *   事件处理层
 *   在整个 WiFi 运行期间持续处理各种事件
 *
 * 参数说明:
 *   @priv:      虚拟接口指针，包含接口的配置和状态信息
 *   @eventId:   事件 ID，标识事件类型
 *   @eventData: 事件数据，具体含义取决于事件类型
 *
 * 返回值:
 *   无
 *
 * 支持的事件类型:
 *   - WSM_EVENT_ERROR: 错误事件
 *   - WSM_EVENT_BSS_LOST: BSS 丢失（连接断开）
 *   - WSM_EVENT_BSS_REGAINED: BSS 恢复
 *   - WSM_EVENT_RADAR_DETECTED: 雷达检测（DFS）
 *   - WSM_EVENT_RCPI_RSSI: RSSI 变化
 *   - WSM_EVENT_BT_INACTIVE: 蓝牙不活动
 *   - WSM_EVENT_BT_ACTIVE: 蓝牙活动
 *   - WSM_EVENT_INACTIVITY: 站点不活动（AP 模式）
 *   - WSM_EVENT_PS_MODE_ERROR: 电源管理错误
 *   - WSM_EVENT_IND_BEACON_CSA: CSA 完成（信道切换）
 *
 * BSS_LOST 事件处理:
 *   1. 调用 atbmwifi_ieee80211_connection_loss() 通知连接丢失
 *   2. STA 模式下调用 atbm_sta_deauth() 断开连接
 *
 * INACTIVITY 事件处理（AP 模式）:
 *   1. 解除站点映射
 *   2. 构造 Deauth 帧
 *   3. 发送到 RX 处理流程
 *   4. 更新 TIM
 *
 * 注意事项:
 *   - BSS_LOST 是连接断开的主要触发事件
 *   - 事件处理可能触发状态机转换
 *   - 某些事件需要通知应用层
 *
 * 相关函数:
 *   - atbmwifi_ieee80211_connection_loss(): 连接丢失处理
 *   - atbm_sta_deauth(): 断开连接
 *   - _atbmwifi_unmap_link(): 解除链路映射
 *
 **************************************************************************************************************/
 atbm_void atbmwifi_event_handler(struct atbmwifi_vif *priv,atbm_uint32 eventId,atbm_uint32 eventData)
{
	switch (eventId) {
		case WSM_EVENT_ERROR:
			/* I even don't know what is it about.. */
			//STUB();
			break;
		case WSM_EVENT_BSS_LOST:
		{
			wifi_printk(WIFI_DBG_ERROR,"[CQM] BSS lost.\n");
			
			atbmwifi_ieee80211_connection_loss(priv);
			if(atbmwifi_is_sta_mode(priv->iftype))
			{
				wifi_printk(WIFI_ALWAYS,"atbmwifi_event_handler() ---deauth\n");
				atbm_sta_deauth(priv, ATBM_FALSE);
			}
			else
			{
				//atbmwifi_ap_deauth(priv,StaMac, ATBM_TRUE);
			}
			break;
		}
		case WSM_EVENT_BSS_REGAINED:
		{
			//sta_printk(KERN_DEBUG "[CQM] BSS regained.\n");
			//priv->delayed_link_loss = 0;
			//atbm_spin_lock(&priv->bss_loss_lock);
			//priv->bss_loss_status = ATBMWIFI__BSS_LOSS_NONE;		
			//atbm_spin_unlock(&priv->bss_loss_lock);
			//cancel_delayed_work_sync(&priv->bss_loss_work);
			//cancel_delayed_work_sync(&priv->connection_loss_work);
			break;
		}
		case WSM_EVENT_RADAR_DETECTED:
			//STUB();
			break;
		case WSM_EVENT_RCPI_RSSI:
		{
			break;
		}
		case WSM_EVENT_BT_INACTIVE:
			//STUB();
			break;
		case WSM_EVENT_BT_ACTIVE:
			//STUB();
			break;
		case WSM_EVENT_INACTIVITY://WSM_EVENT_IND_INACTIVITY
		{
			int link_id = atbm_ffs((atbm_uint32)eventData) - 1;
			struct atbm_buff *skb;
	        struct atbmwifi_ieee80211_mgmt *deauth;
	        struct atbmwifi_link_entry *entry = ATBM_NULL;

			wifi_printk(WIFI_DBG_ERROR, "Inactivity Event Rx "
					"link_id %d\n", link_id);
			_atbmwifi_unmap_link(priv, link_id);

			skb = atbm_dev_alloc_skb(sizeof(struct atbmwifi_ieee80211_mgmt));
			//atbm_skb_reserve(skb, 64);
			deauth = (struct atbmwifi_ieee80211_mgmt *)atbm_skb_put(skb, sizeof(struct atbmwifi_ieee80211_mgmt));
            ATBM_WARN_ON_FUNC(!deauth);
            entry = &priv->link_id_db[link_id - 1];
            deauth->duration = 0;	
			atbm_memcpy(deauth->da, priv->mac_addr, ATBM_ETH_ALEN);
            atbm_memcpy(deauth->sa, entry->mac/*priv->link_id_db[i].mac*/, ATBM_ETH_ALEN);
            atbm_memcpy(deauth->bssid,priv->mac_addr, ATBM_ETH_ALEN);
			deauth->frame_control = atbm_cpu_to_le16(ATBM_IEEE80211_FTYPE_MGMT |
	                                            ATBM_IEEE80211_STYPE_DEAUTH |
	                                            ATBM_IEEE80211_FCTL_TODS);
            deauth->u.deauth.reason_code = ATBM_WLAN_REASON_DEAUTH_LEAVING;
            deauth->seq_ctrl = 0;
            if(atbmwifi_ieee80211_rx_irqsafe(priv, skb) != 0){
				atbm_dev_kfree_skb(skb);
			}
			atbm_set_tim_impl(priv);
			break;
		}
		case WSM_EVENT_PS_MODE_ERROR:
		{
		}
#if CONFIG_ATBM_SUPPORT_CSA
		case WSM_EVENT_IND_BEACON_CSA:
		{ 
			int csa_status = (atbm_uint32)eventData;
			atbm_ieee80211_event_csa_done(priv, csa_status);
			break;
	}
#endif
}
}
int atbmwifi_disable_listening(struct atbmwifi_vif *priv)
{
	int ret;
	struct wsm_reset reset={0};	
	reset.reset_statistics = ATBM_TRUE;
	
#ifdef P2P_MULTIVIF
	if(priv->if_id != 2) {
		ATBM_WARN_ON_FUNC(priv->join_status > ATBMWIFI__JOIN_STATUS_MONITOR);
		return 0;
	}
#endif //change by wp
	priv->join_status = ATBMWIFI__JOIN_STATUS_PASSIVE;

	ATBM_WARN_ON_FUNC(priv->join_status > ATBMWIFI__JOIN_STATUS_MONITOR);

	ret = wsm_reset(priv->hw_priv, &reset, ATBM_WIFI_GENERIC_IF_ID);
	return ret;
}
atbm_void atbmwifi_stop(void)
{
		struct atbmwifi_common * hw_priv;
		int i = 0;	
		struct atbmwifi_vif *priv;	
		hw_priv = &g_hw_prv;
		atbm_for_each_vif(hw_priv,priv,i){
			switch (priv->join_status) {
				case ATBMWIFI__JOIN_STATUS_STA:
					atbmwifi_stop_sta(priv);
					break;
				case ATBMWIFI__JOIN_STATUS_AP:				
					atbmwifi_stop_ap(priv);				
					break;
				case ATBMWIFI__JOIN_STATUS_MONITOR:
					atbmwifi_disable_listening(priv);
					break;
				default:
					break;
				}	
			/* TODO:COMBO: May be reset of these variables "delayed_link_loss and
			 * join_status to default can be removed as dev_priv will be freed by
			 * mac80211 */
			//priv->delayed_link_loss = 0;
			priv->join_status = ATBMWIFI__JOIN_STATUS_PASSIVE;
			wsm_unlock_tx(hw_priv);
			priv->listening = ATBM_FALSE;
		}
		for (i = 0; i < 4; i++){
			atbmwifi_queue_clear(&hw_priv->tx_queue[i], -1);  //clear all queue
		}	
}


