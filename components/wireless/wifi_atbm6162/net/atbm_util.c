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

/*
 * Wireless utility functions
 */
#include "atbm_hal.h"
#include "wpa_main.h"
#if CONFIG_P2P
#include "p2p_common.h"
#endif
#if ATBM_SUPPORT_BRIDGE
#include "atbm_bridge.h"
#endif

extern atbm_void wpas_wps_timeout(void *eloop_ctx, atbm_void *timeout_ctx);
extern atbm_void sta_scan_start_timer_func(atbm_void *data1,atbm_void *data2);
unsigned int atbm_cfg80211_classify8021d(struct atbm_buff *skb);
/*
static const  struct country_code_to_string country_strings[] = {
    {CTRY_DEBUG,	 	"DB" },
    {CTRY_DEFAULT,	 	"NA" },
    {CTRY_ALBANIA,		"AL" },
    {CTRY_ALGERIA,		"DZ" },
    {CTRY_ARGENTINA,		"AR" },
    {CTRY_ARMENIA,		"AM" },
    {CTRY_AUSTRALIA,		"AU" },
    {CTRY_AUSTRIA,		"AT" },
    {CTRY_AZERBAIJAN,		"AZ" },
    {CTRY_BAHRAIN,		"BH" },
    {CTRY_BELARUS,		"BY" },
    {CTRY_BELGIUM,		"BE" },
    {CTRY_BELIZE,		"BZ" },
    {CTRY_BOLIVIA,		"BO" },
    {CTRY_BRAZIL,		"BR" },
    {CTRY_BRUNEI_DARUSSALAM,	"BN" },
    {CTRY_BULGARIA,		"BG" },
    {CTRY_CANADA,		"CA" },
    {CTRY_CHILE,		"CL" },
    {CTRY_CHINA,		"CN" },
    {CTRY_COLOMBIA,		"CO" },
    {CTRY_COSTA_RICA,		"CR" },
    {CTRY_CROATIA,		"HR" },
    {CTRY_CYPRUS,		"CY" },
    {CTRY_CZECH,		"CZ" },
    {CTRY_DENMARK,		"DK" },
    {CTRY_DOMINICAN_REPUBLIC,	"DO" },
    {CTRY_ECUADOR,		"EC" },
    {CTRY_EGYPT,		"EG" },
    {CTRY_EL_SALVADOR,		"SV" },    
    {CTRY_ESTONIA,		"EE" },
    {CTRY_FINLAND,		"FI" },
    {CTRY_FRANCE,		"FR" },
    {CTRY_FRANCE2,		"F2" },
    {CTRY_GEORGIA,		"GE" },
    {CTRY_GERMANY,		"DE" },
    {CTRY_GREECE,		"GR" },
    {CTRY_GUATEMALA,		"GT" },
    {CTRY_HONDURAS,		"HN" },
    {CTRY_HONG_KONG,		"HK" },
    {CTRY_HUNGARY,		"HU" },
    {CTRY_ICELAND,		"IS" },
    {CTRY_INDIA,		"IN" },
    {CTRY_INDONESIA,		"ID" },
    {CTRY_IRAN,			"IR" },
    {CTRY_IRELAND,		"IE" },
    {CTRY_ISRAEL,		"IL" },
    {CTRY_ITALY,		"IT" },
    {CTRY_JAPAN,		"JP" },
    {CTRY_JAPAN1,		"J1" },
    {CTRY_JAPAN2,		"J2" },    
    {CTRY_JAPAN3,		"J3" },
    {CTRY_JAPAN4,		"J4" },
    {CTRY_JAPAN5,		"J5" },    
    {CTRY_JAPAN7,		"JP" },
    {CTRY_JAPAN6,		"JP" },
    {CTRY_JAPAN8,		"JP" },
    {CTRY_JAPAN9,	      	"JP" },
    {CTRY_JAPAN10,	      	"JP" }, 
    {CTRY_JAPAN11,	      	"JP" },
    {CTRY_JAPAN12,	      	"JP" },
    {CTRY_JAPAN13,	      	"JP" },
    {CTRY_JAPAN14,	      	"JP" },
    {CTRY_JAPAN15,	      	"JP" },
    {CTRY_JAPAN16,	      	"JP" }, 
    {CTRY_JAPAN17,	      	"JP" },
    {CTRY_JAPAN18,	      	"JP" },
    {CTRY_JAPAN19,	      	"JP" },
    {CTRY_JAPAN20,	      	"JP" },
    {CTRY_JAPAN21,	      	"JP" }, 
    {CTRY_JAPAN22,	      	"JP" },
    {CTRY_JAPAN23,	      	"JP" },
    {CTRY_JAPAN24,	      	"JP" },
    {CTRY_JAPAN25,	      	"JP" }, 
    {CTRY_JAPAN26,	      	"JP" },
    {CTRY_JAPAN27,	      	"JP" },
    {CTRY_JAPAN28,	      	"JP" },
    {CTRY_JAPAN29,	      	"JP" },
    {CTRY_JAPAN30,      	"JP" },
    {CTRY_JAPAN31,      	"JP" },
    {CTRY_JAPAN32,      	"JP" },
    {CTRY_JAPAN33,      	"JP" },
    {CTRY_JAPAN34,      	"JP" },
    {CTRY_JAPAN35,      	"JP" },
    {CTRY_JAPAN36,      	"JP" },
    {CTRY_JAPAN37,      	"JP" },
    {CTRY_JAPAN38,      	"JP" },
    {CTRY_JAPAN39,      	"JP" },
    {CTRY_JAPAN40,      	"JP" },
    {CTRY_JAPAN41,      	"JP" },
    {CTRY_JAPAN42,      	"JP" },
    {CTRY_JAPAN43,      	"JP" },
    {CTRY_JAPAN44,      	"JP" },
    {CTRY_JAPAN45,      	"JP" },
    {CTRY_JAPAN46,      	"JP" },
    {CTRY_JAPAN47,      	"JP" },
    {CTRY_JAPAN48,      	"JP" },
    {CTRY_JORDAN,		"JO" },
    {CTRY_KAZAKHSTAN,		"KZ" },
    {CTRY_KOREA_NORTH,		"KP" },
    {CTRY_KOREA_ROC,		"KR" },
    {CTRY_KOREA_ROC2,		"K2" },
    {CTRY_KUWAIT,		"KW" },
    {CTRY_LATVIA,		"LV" },
    {CTRY_LEBANON,		"LB" },
    {CTRY_LIECHTENSTEIN,	"LI" },
    {CTRY_LITHUANIA,		"LT" },
    {CTRY_LUXEMBOURG,		"LU" },
    {CTRY_MACAU,		"MO" },
    {CTRY_MACEDONIA,		"MK" },
    {CTRY_MALAYSIA,		"MY" },
    {CTRY_MEXICO,		"MX" },
    {CTRY_MONACO,		"MC" },
    {CTRY_MOROCCO,		"MA" },
    {CTRY_NETHERLANDS,		"NL" },
    {CTRY_NEW_ZEALAND,		"NZ" },
    {CTRY_NORWAY,		"NO" },
    {CTRY_OMAN,			"OM" },
    {CTRY_PAKISTAN,		"PK" },
    {CTRY_PANAMA,		"PA" },
    {CTRY_PERU,			"PE" },
    {CTRY_PHILIPPINES,		"PH" },
    {CTRY_POLAND,		"PL" },
    {CTRY_PORTUGAL,		"PT" },
    {CTRY_PUERTO_RICO,		"PR" },
    {CTRY_QATAR,		"QA" },
    {CTRY_ROMANIA,		"RO" },
    {CTRY_RUSSIA,		"RU" },
    {CTRY_SAUDI_ARABIA,		"SA" },
    {CTRY_SINGAPORE,		"SG" },
    {CTRY_SLOVAKIA,		"SK" },
    {CTRY_SLOVENIA,		"SI" },
    {CTRY_SOUTH_AFRICA,		"ZA" },
    {CTRY_SPAIN,		"ES" },
    {CTRY_SWEDEN,		"SE" },
    {CTRY_SWITZERLAND,		"CH" },
    {CTRY_SYRIA,		"SY" },
    {CTRY_TAIWAN,		"TW" },
    {CTRY_THAILAND,		"TH" },
    {CTRY_TRINIDAD_Y_TOBAGO,	"TT" },
    {CTRY_TUNISIA,		"TN" },
    {CTRY_TURKEY,		"TR" },
    {CTRY_UKRAINE,		"UA" },
    {CTRY_UAE,			"AE" },
    {CTRY_UNITED_KINGDOM,	"GB" },
    {CTRY_UNITED_STATES,	"US" },
    {CTRY_UNITED_STATES_FCC49,	"US" },
    {CTRY_URUGUAY,		"UY" },
    {CTRY_UZBEKISTAN,		"UZ" },    
    {CTRY_VENEZUELA,		"VE" },
    {CTRY_VIET_NAM,		"VN" },
    {CTRY_YEMEN,		"YE" },
    {CTRY_ZIMBABWE,		"ZW" }    
};
*/




 atbm_void atbmwifi_ieee80211_channel_country(struct atbmwifi_common *hw_priv,int region_id){
	
	struct atbmwifi_ieee80211_channel *chantable=atbmwifi_band_2ghz.channels;
	
	int nchan = 0;
	int chan_vl = 1;
	switch(region_id){
		case country_usa:
		case country_canada:/*1~11*/	
		case region_taiwan:/*1~11*/
		case country_SINGAPORE:/*1~11*/
			for(chan_vl=1;chan_vl<=11;chan_vl++){
				chantable[nchan].hw_value = chan_vl;
				nchan++;
			}
			break;
		case country_brazil:/*1~14*/
		case country_japan:/*1~14*/
			for(chan_vl=1;chan_vl<=14;chan_vl++){
				chantable[nchan].hw_value = chan_vl;
				nchan++;
			}
			break;
		case country_Israel:/*3~9*/
			for(chan_vl=3;chan_vl<=9;chan_vl++){
				chantable[nchan].hw_value = chan_vl;
				nchan++;
			}
			break;
		case country_chinese:
		case country_europe:
		case country_australia:/*1~13*/
		default:
			for(chan_vl=1;chan_vl<=13;chan_vl++){
				chantable[nchan].hw_value = chan_vl;
				nchan++;
			}
			break;

	}
	atbmwifi_band_2ghz.n_channels = nchan;
	hw_priv->region_id = region_id;
}

/* See IEEE 802.1H for LLC/SNAP encapsulation/decapsulation */
/* Ethernet-II snap header (RFC1042 for most EtherTypes) */
static const unsigned char atbm_rfc1042_header[]  =
	{ 0xaa, 0xaa, 0x03, 0x00, 0x00, 0x00 };

/* Bridge-Tunnel header (for EtherTypes ATBM_ETH_P_AARP and ATBM_ETH_P_IPX) */
static const unsigned char atbm_bridge_tunnel_header[]  =
	{ 0xaa, 0xaa, 0x03, 0x00, 0x00, 0xf8 };

unsigned int  atbmwifi_ieee80211_hdrlen(atbm_uint16 fc)
{
	unsigned int hdrlen = 24;

	if (atbmwifi_ieee80211_is_data(fc)) {
		if (atbmwifi_ieee80211_has_a4(fc))
			hdrlen = 30;
		if (atbmwifi_ieee80211_is_data_qos(fc)) {
			hdrlen += ATBM_IEEE80211_QOS_CTL_LEN;
			if (atbmwifi_ieee80211_has_order(fc))
				hdrlen += ATBM_IEEE80211_HT_CTL_LEN;
		}
		goto out;
	}

	if (atbmwifi_ieee80211_is_ctl(fc)) {
		/*
		 * ACK and CTS are 10 bytes, all others 16. To see how
		 * to get this condition consider
		 *   subtype mask:   0b0000000011110000 (0x00F0)
		 *   ACK subtype:    0b0000000011010000 (0x00D0)
		 *   CTS subtype:    0b0000000011000000 (0x00C0)
		 *   bits that matter:         ^^^      (0x00E0)
		 *   value of those: 0b0000000011000000 (0x00C0)
		 */
		if ((fc & atbm_cpu_to_le16(0x00E0)) == atbm_cpu_to_le16(0x00C0))
			hdrlen = 10;
		else
			hdrlen = 16;
	}
out:
	return hdrlen;
}

unsigned int atbmwifi_ieee80211_get_hdrlen_from_skb(const struct atbm_buff *skb)
{
	const struct atbmwifi_ieee80211_hdr *hdr =
			(const struct atbmwifi_ieee80211_hdr *)ATBM_OS_SKB_DATA(skb);
	unsigned int hdrlen;

	if (atbm_unlikely(ATBM_OS_SKB_LEN(skb) < 10))
		return 0;
	hdrlen = atbmwifi_ieee80211_hdrlen(hdr->frame_control);
	if (atbm_unlikely(hdrlen > ATBM_OS_SKB_LEN(skb)))
		return 0;
	return hdrlen;
}

/*
 * requires that rx->skb is a frame with ethernet header
 */
ATBM_BOOL atbmwifi_ieee80211_frame_allowed(struct atbm_buff *skb, atbm_uint16 fc)
{
//	static const atbm_uint8 pae_group_addr[ATBM_ETH_ALEN] __aligned(2)
//		= { 0x01, 0x80, 0xC2, 0x00, 0x00, 0x03 };


	return ATBM_TRUE;
}

/**************************************************************************************************************
 * 函数名: atbmwifi_ieee80211_amsdu_to_8023s
 *
 * 功能描述:
 *   将 AMSDU 聚合帧解聚合为多个 802.3 以太网帧。
 *   这是 AMSDU 解聚合的核心函数，负责从 AMSDU 中提取每个子帧并转换为 802.3 格式。
 *
 * 在 RX 流程中的位置:
 *   阶段 6: AMSDU 解聚合
 *   在 atbmwifi_ieee80211_rx_h_amsdu() 中调用
 *
 * 参数说明:
 *   @skb:             输入的 AMSDU 数据包
 *   @list:            输出的子帧队列（struct atbm_buff_head）
 *   @addr:            MAC 地址（用于 802.11 到 802.3 转换）
 *   @iftype:          接口类型（STA/AP）
 *   @has_80211_header: 是否包含 802.11 头部
 *                      - ATBM_TRUE: 需要先转换 802.11 头部
 *                      - ATBM_FALSE: 已经是 802.3 格式
 *
 * 返回值:
 *   无（子帧通过 list 参数返回）
 *
 * 处理流程:
 *   1. 802.11 到 802.3 转换：如果 has_80211_header 为 TRUE，先转换
 *   2. 跳过包装头部：跳过外层的以太网头部
 *   3. 循环提取子帧：
 *      a. 解析子帧头部：DA (6) + SA (6) + Length (2)
 *      b. 计算填充：4 字节对齐
 *      c. 安全检查：防止 A-MSDU 注入攻击
 *      d. 分配子帧缓冲区：
 *         - 最后一个子帧：重用原始 skb
 *         - 中间子帧：分配新的 skb 或重用（ATBM_RX_REUSE_PACKET）
 *      e. 构建 802.3 以太网头部：
 *         - RFC1042 封装：移除封装，使用 ethertype
 *         - 其他封装：使用长度字段
 *      f. 加入输出队列
 *   4. 移动到下一个子帧
 *
 * AMSDU 子帧格式:
 *   ┌──────────┬──────────┬──────────┬──────────┬──────────┐
 *   │ DA       │ SA       │ Length   │ MSDU     │ Padding  │
 *   │ (6 字节) │ (6 字节) │ (2 字节) │ (可变)   │ (0-3)    │
 *   └──────────┴──────────┴──────────┴──────────┴──────────┘
 *
 * 注意事项:
 *   - 每个子帧（除最后一个）需要 4 字节对齐
 *   - 子帧头部使用长度字段（而非类型字段）
 *   - RFC1042 封装需要移除
 *   - 安全检查防止注入攻击
 *   - 支持零拷贝模式（ATBM_RX_REUSE_PACKET）
 *
 * 相关函数:
 *   - atbmwifi_ieee80211_rx_h_amsdu(): 调用此函数
 *   - atbmwifi_ieee80211_data_to_8023(): 802.11 到 802.3 转换
 *
 **************************************************************************************************************/
atbm_void atbmwifi_ieee80211_amsdu_to_8023s(struct atbm_buff *skb, struct atbm_buff_head *list,
			      const atbm_uint8 *addr, enum atbm_nl80211_iftype iftype,
			      ATBM_BOOL has_80211_header)
{
	struct atbm_buff *frame = ATBM_NULL;
	atbm_uint16 ethertype;
	atbm_uint8 *payload;
	const struct atbmwifi_ieee8023_hdr *eth;
	int remaining, err;
	atbm_uint8 dst[ATBM_ETH_ALEN], src[ATBM_ETH_ALEN];

	if (has_80211_header) {
		err = atbmwifi_ieee80211_data_to_8023(skb, addr, iftype);
		if (err)
			goto out;

		/* skip the wrapping header */
		eth = (struct atbmwifi_ieee8023_hdr *) atbm_skb_pull(skb, sizeof(struct atbmwifi_ieee8023_hdr));
		if (!eth)
			goto out;
	} else {
		eth = (struct atbmwifi_ieee8023_hdr *) ATBM_OS_SKB_DATA(skb);
	}

	while (skb != frame) {
		atbm_uint8 padding;
		atbm_uint16 len = eth->h_proto;
		unsigned int subframe_len = sizeof(struct atbmwifi_ieee8023_hdr) + atbm_ntohs(len);

		remaining = ATBM_OS_SKB_LEN(skb);
		atbm_memcpy(dst, eth->h_dest, ATBM_ETH_ALEN);
		atbm_memcpy(src, eth->h_source, ATBM_ETH_ALEN);

		padding = (4 - subframe_len) & 0x3;
		/* the last MSDU has no padding */
		if (subframe_len > remaining)
			goto purge;

		/* mitigate A-MSDU aggregation injection attacks */
		if (!atbm_compare_ether_addr(eth->h_dest, atbm_rfc1042_header))
			goto purge;

		atbm_skb_pull(skb, sizeof(struct atbmwifi_ieee8023_hdr));
		/* reuse skb for the last subframe */
		if (remaining <= subframe_len + padding)
			frame = skb;
		else {
#if ATBM_RX_REUSE_PACKET
			frame = atbm_dev_reuse_skb(skb, ATBM_OS_SKB_DATA(skb)-sizeof(struct atbmwifi_ieee8023_hdr), atbm_ntohs(len) + sizeof(struct atbmwifi_ieee8023_hdr));
			atbm_skb_pull(frame, sizeof(struct atbmwifi_ieee8023_hdr));
#else
			unsigned int hlen = 32;
			/*
			 * Allocate and reserve two bytes more for payload
			 * alignment since sizeof(struct ethhdr) is 14.
			 */
			frame = atbm_dev_alloc_skb(hlen + subframe_len + 2);
			if (!frame)
				goto purge;

			atbm_skb_reserve(frame, hlen + sizeof(struct atbmwifi_ieee8023_hdr) + 2);
			atbm_memcpy(atbm_skb_put(frame, atbm_ntohs(len)), ATBM_OS_SKB_DATA(skb),atbm_ntohs(len));
#endif
			eth = (struct atbmwifi_ieee8023_hdr *)atbm_skb_pull(skb, atbm_ntohs(len) +
							padding);
			if (!eth) {
				atbm_dev_kfree_skb(frame);
				goto purge;
			}
		}

		//skb_reset_network_header(frame);
		//frame->dev = skb->dev;
		//frame->priority = skb->priority;

		payload = ATBM_OS_SKB_DATA(frame);
		ethertype = (payload[6] << 8) | payload[7];

		if (atbm_likely((atbm_compare_ether_addr(payload, atbm_rfc1042_header) == 0 &&
			    ethertype != ATBM_ETH_P_AARP && ethertype != ATBM_ETH_P_IPX) ||
			   atbm_compare_ether_addr(payload,atbm_bridge_tunnel_header) == 0)) {
			/* remove RFC1042 or Bridge-Tunnel
			 * encapsulation and replace EtherType */
			atbm_skb_pull(frame, 6);
			atbm_memcpy(atbm_skb_push(frame, ATBM_ETH_ALEN), src, ATBM_ETH_ALEN);
			atbm_memcpy(atbm_skb_push(frame, ATBM_ETH_ALEN), dst, ATBM_ETH_ALEN);
		} else {
			atbm_memcpy(atbm_skb_push(frame, sizeof(atbm_uint16)), &len,
				sizeof(atbm_uint16));
			atbm_memcpy(atbm_skb_push(frame, ATBM_ETH_ALEN), src, ATBM_ETH_ALEN);
			atbm_memcpy(atbm_skb_push(frame, ATBM_ETH_ALEN), dst, ATBM_ETH_ALEN);
		}
		atbm_skb_queue_tail(list, frame);
	}

	return;

 purge:
	atbm_skb_queue_purge(list);
 out:
	atbm_dev_kfree_skb(skb);
}
/*
if ap mode ,we need forward mul-data or unicast data for other sta  to the air
skb need upload to tcpip return 1, other return 0
*/
#define ATBM_MAX_FORWARD_NUM 160
struct atbmwifi_vif *atbmwifi_ieee80211_forward_skb(struct atbmwifi_vif *priv,struct atbm_buff *skb)
{
	struct atbmwifi_ieee8023_hdr *hdr = (struct atbmwifi_ieee8023_hdr *) ATBM_OS_SKB_DATA(skb);
	struct atbm_buff *txskb = ATBM_NULL;
	struct atbmwifi_sta_priv * sta_priv =ATBM_NULL;
	struct atbmwifi_vif *peer_priv;

	/*	wifi_printk(WIFI_ALWAYS, "rx iftype:%s proto:%s port:%d src:"MACSTR" dst:"MACSTR"\n",
			(priv->iftype == ATBM_NL80211_IFTYPE_AP) ? "ap":"sta",
			hdr->h_proto,
			MAC2STR(hdr->h_source), MAC2STR(hdr->h_dest));
	*/

	peer_priv = _atbmwifi_hwpriv_to_vifpriv(priv->hw_priv, !(priv->if_id));
	if((priv->iftype == ATBM_NL80211_IFTYPE_AP) || (priv->iftype == ATBM_NL80211_IFTYPE_P2P_GO)){
		if (atbm_is_multicast_ether_addr(hdr->h_dest)) {
			if(priv->config.disallow_broadcast == ATBM_FALSE){
				txskb = atbm_dev_alloc_skb(ATBM_OS_SKB_LEN(skb));
	            if (txskb) {
					  txskb->priority = 0;
	                  atbm_memcpy(ATBM_OS_SKB_DATA(txskb), ATBM_OS_SKB_DATA(skb), ATBM_OS_SKB_LEN(skb));
					  atbm_skb_put(txskb,ATBM_OS_SKB_LEN(skb));
#if (ATBM_RX_TASK_QUEUE == 2)
					  if(priv->rx_task_skb_list.qlen < ATBM_MAX_FORWARD_NUM){
						  atbm_skb_queue_tail(&priv->rx_task_skb_list,txskb);
						  /*Do rx task schedule,change task excute text*/
						  ///TODO atbm_net_RxTask
						  atbm_queue_work(priv->hw_priv, priv->rx_task_work);
					  }else{
						  atbm_dev_kfree_skb(txskb);
					  }
#else
					  atbmwifi_tx_start(txskb,priv);
#endif
	            }
			}
        }else{
#if ATBM_SUPPORT_BRIDGE
			if(atbm_memcmp(hdr->h_dest,peer_priv->mac_addr,6)
#else
			if(atbm_memcmp(hdr->h_dest,priv->mac_addr,6)
#endif
			){
				if(priv->config.disallow_unicast == ATBM_FALSE){
	                sta_priv = atbmwifi_sta_find(priv,hdr->h_dest);
	                if (sta_priv) {
						txskb = atbm_dev_alloc_skb(ATBM_OS_SKB_LEN(skb));
		                if (txskb) {
							txskb->priority = 0;
							atbm_memcpy(ATBM_OS_SKB_DATA(txskb), ATBM_OS_SKB_DATA(skb), ATBM_OS_SKB_LEN(skb));
							atbm_skb_put(txskb,ATBM_OS_SKB_LEN(skb));
#if (ATBM_RX_TASK_QUEUE == 2)
							if(priv->rx_task_skb_list.qlen < ATBM_MAX_FORWARD_NUM){
								atbm_skb_queue_tail(&priv->rx_task_skb_list,txskb);
								/*Do rx task schedule,change task excute text*/
								///TODO atbm_net_RxTask
								atbm_queue_work(priv->hw_priv, priv->rx_task_work);
							}else{
								atbm_dev_kfree_skb(txskb);
							}
#else
							atbmwifi_tx_start(txskb,priv);
#endif
		                }
						atbm_dev_kfree_skb(skb);
						return ATBM_NULL;
	                }
					//Upload unkouwn traffic
				}else{
					atbm_dev_kfree_skb(skb);
					return ATBM_NULL;
				}
			}
        }
#if ATBM_SUPPORT_BRIDGE
		if(peer_priv && peer_priv->connect_ok
			&& atbm_memcmp(peer_priv->mac_addr, ATBM_OS_SKB_DATA(skb), ATBM_ETH_ALEN)
			){
			struct atbmwifi_ieee8023_hdr *brhdr;
			struct atbm_buff *brskb;

			brskb = get_sta_deliver_skb(skb);
			if(brskb){
				atbmwifi_tx_start(brskb,peer_priv);
			}
		}
		return peer_priv;
#endif
	}
#if ATBM_SUPPORT_BRIDGE
	else{
		struct atbm_buff *brskb;
		if(!atbm_memcmp(hdr->h_dest,peer_priv->mac_addr,6)){
			wifi_printk(WIFI_DBG_ERROR, "error unexpected data!!\n");
			atbm_dev_kfree_skb(skb);
			return ATBM_FALSE;
		}
		//wifi_printk(WIFI_ALWAYS, "iftype:%d\n", peer_priv->iftype);
		if(peer_priv && peer_priv->connect_ok){
			brskb = get_ap_deliver_skb(skb);
			if(brskb){
				atbmwifi_tx_start(brskb,peer_priv);
			}
		}
	}
#endif
	return priv;
}

atbm_void atbmwifi_ieee80211_deliver_skb(struct atbmwifi_vif *priv,struct atbm_buff *skb,atbm_uint16 *need_free)
{
	struct atbmwifi_vif *tx_priv;
	if(!eapol_input(priv,skb)){
		if(priv->assoc_ok==1){
		//if ap mode ,we need forward mul-data or unicast data for other sta 
			tx_priv = atbmwifi_ieee80211_forward_skb(priv,skb);
			if(tx_priv){
				tcp_opt->net_rx(tx_priv->ndev,skb);
			}
			*need_free=0;
		}
	}else{
		*need_free=0;
	}
}

#if ATBM_SUPPORT_BRIDGE
int atbmwifi_br_tx_skb(struct atbmwifi_vif *priv,struct atbm_buff *skb){
	struct atbmwifi_vif *peer_priv;
	struct atbm_buff *txskb;
	struct atbmwifi_sta_priv * sta_priv =ATBM_NULL;
	struct atbmwifi_ieee8023_hdr *hdr = (struct atbmwifi_ieee8023_hdr *) ATBM_OS_SKB_DATA(skb);

	if(!atbmwifi_is_sta_mode(priv->iftype)){
		return 0;
	}

	peer_priv = _atbmwifi_hwpriv_to_vifpriv(priv->hw_priv, !(priv->if_id));
	if(peer_priv && peer_priv->connect_ok){
		if (atbm_is_multicast_ether_addr(hdr->h_dest)) {
			txskb = atbm_dev_alloc_skb(ATBM_OS_SKB_LEN(skb));
			if (txskb) {
				txskb->priority = 0;
				atbm_memcpy(ATBM_OS_SKB_DATA(txskb), ATBM_OS_SKB_DATA(skb), ATBM_OS_SKB_LEN(skb));
				atbm_skb_put(txskb,ATBM_OS_SKB_LEN(skb));
				atbmwifi_tx_start(txskb, peer_priv);
			}
		}else{
			sta_priv = atbmwifi_sta_find(peer_priv,hdr->h_dest);
			if (sta_priv) {
				txskb = atbm_dev_alloc_skb(ATBM_OS_SKB_LEN(skb));
				if (txskb) {
					txskb->priority = 0;
					atbm_memcpy(ATBM_OS_SKB_DATA(txskb), ATBM_OS_SKB_DATA(skb), ATBM_OS_SKB_LEN(skb));
					atbm_skb_put(txskb,ATBM_OS_SKB_LEN(skb));
					atbmwifi_tx_start(txskb, peer_priv);
				}
				atbm_dev_kfree_skb(skb);
				return 0;
			}
		}
	}
	return 1;
}
#endif

/**************************************************************************************************************
 * 函数名: atbmwifi_ieee80211_rx_h_amsdu
 *
 * 功能描述:
 *   AMSDU（Aggregated MAC Service Data Unit）解聚合处理函数。
 *   检测数据帧是否为 AMSDU 聚合帧，如果是则解聚合为多个子帧并分别提交到协议栈。
 *
 * 在 RX 流程中的位置:
 *   阶段 6: AMSDU 解聚合
 *   在 __atbmwifi_ieee80211_rx() 或 atbm_netrx_task() 中调用
 *
 * 参数说明:
 *   @priv: 虚拟接口私有数据指针
 *   @skb:  接收到的数据包（可能是 AMSDU 聚合帧）
 *
 * 返回值:
 *   0: 不是 AMSDU 或处理失败（调用者需要继续处理 skb）
 *   1: 是 AMSDU 且已处理（skb 已被释放或加入队列，调用者不需要释放）
 *
 * 处理流程:
 *   1. 检查 AMSDU 标志：检查 status->flag & ATBM_RX_FLAG_AMSDU
 *   2. 安全检查：
 *      a. 组播地址检查：AMSDU 不支持组播
 *      b. 加密类型检查：WEP/TKIP 不支持 AMSDU
 *   3. 初始化子帧队列：atbm_skb_queue_head_init(&frame_list)
 *   4. 解聚合：调用 atbmwifi_ieee80211_amsdu_to_8023s() 解聚合
 *   5. 循环处理子帧：
 *      a. 从队列取出子帧
 *      b. 检查帧是否允许
 *      c. 提交到协议栈：atbmwifi_ieee80211_deliver_skb()
 *      d. 释放子帧（如果需要）
 *
 * AMSDU 格式:
 *   ┌─────────────────────────────────────────┐
 *   │ 802.11 MAC Header                      │
 *   ├─────────────────────────────────────────┤
 *   │ Subframe 1: DA + SA + Length + MSDU    │
 *   ├─────────────────────────────────────────┤
 *   │ Subframe 2: DA + SA + Length + MSDU    │
 *   ├─────────────────────────────────────────┤
 *   │ ...                                    │
 *   └─────────────────────────────────────────┘
 *
 * 注意事项:
 *   - AMSDU 不支持组播地址
 *   - WEP/TKIP 加密不支持 AMSDU（安全原因）
 *   - 解聚合后的子帧分别提交到协议栈
 *   - 原始 skb 在解聚合过程中被释放或重用
 *
 * 相关函数:
 *   - atbmwifi_ieee80211_amsdu_to_8023s(): 实际的解聚合函数
 *   - atbmwifi_ieee80211_deliver_skb(): 提交到协议栈
 *   - atbmwifi_ieee80211_frame_allowed(): 检查帧是否允许
 *
 **************************************************************************************************************/
 int atbmwifi_ieee80211_rx_h_amsdu(struct atbmwifi_vif *priv,struct atbm_buff *skb)
{
	struct atbmwifi_ieee80211_hdr *hdr;
	atbm_uint16 fc;
	atbm_uint16 need_free = 1;
	struct atbm_buff_head frame_list;
	struct atbmwifi_ieee80211_rx_status *status = ATBM_IEEE80211_SKB_RXCB(skb);


	if (!(status->flag & ATBM_RX_FLAG_AMSDU)){
		return 0;
	}
	hdr = (struct atbmwifi_ieee80211_hdr *) ATBM_OS_SKB_DATA(skb);

	if (atbm_is_multicast_ether_addr(hdr->addr1))
			goto drop;

	if(status->flag & ATBM_RX_FLAG_DECRYPTED){
		switch(priv->connect.crypto_pairwise){
			case ATBM_WLAN_CIPHER_SUITE_WEP40:
			case ATBM_WLAN_CIPHER_SUITE_WEP104:
			case ATBM_WLAN_CIPHER_SUITE_TKIP:
				goto drop;
			default:
				break;
		}
	}

	fc = hdr->frame_control;

	atbm_skb_queue_head_init(&frame_list);

	atbmwifi_ieee80211_amsdu_to_8023s(skb, &frame_list, priv->mac_addr,
				 priv->iftype, ATBM_TRUE);

	while (!atbm_skb_queue_empty(&frame_list)) {
		skb = atbm_skb_dequeue(&frame_list);

		if (!atbmwifi_ieee80211_frame_allowed(skb, fc)) {
			wifi_printk(WIFI_ALWAYS,"%s %d frame not allowed\n",__func__,__LINE__);
			atbm_dev_kfree_skb(skb);
			continue;
		}
		atbmwifi_ieee80211_deliver_skb(priv,skb,&need_free);
		if(need_free){
			atbm_dev_kfree_skb(skb);
		}
		need_free = 1;
	}

	return 1;
drop:
	atbm_dev_kfree_skb(skb);
	return 1;
}



/**************************************************************************************************************
 * 函数名: atbmwifi_ieee80211_data_to_8023
 *
 * 功能描述:
 *   将 802.11 数据帧转换为 802.3 以太网帧格式。
 *   这是 RX 流程中的关键转换函数，负责将 WiFi 帧格式转换为以太网帧格式。
 *
 * 在 RX 流程中的位置:
 *   阶段 7: 802.11 到 802.3 格式转换
 *   在 __atbmwifi_ieee80211_rx() 或 atbm_netrx_task() 中调用
 *
 * 参数说明:
 *   @skb:    输入的 802.11 数据帧，输出为 802.3 以太网帧
 *   @addr:   本地 MAC 地址
 *   @iftype: 接口类型（STA/AP）
 *
 * 返回值:
 *   0:  成功
 *   -1: 失败（帧类型错误、地址错误等）
 *
 * 处理流程:
 *   1. 帧类型检查：确保是数据帧
 *   2. 计算 802.11 头部长度
 *   3. 提取地址：根据 ToDS/FromDS 标志提取 DA 和 SA
 *      - ToDS=0, FromDS=0: DA=Addr1, SA=Addr2, BSSID=Addr3
 *      - ToDS=0, FromDS=1: DA=Addr1, BSSID=Addr2, SA=Addr3
 *      - ToDS=1, FromDS=0: BSSID=Addr1, SA=Addr2, DA=Addr3
 *      - ToDS=1, FromDS=1: RA=Addr1, TA=Addr2, DA=Addr3, SA=Addr4
 *   4. 地址验证：根据接口类型验证地址
 *   5. LLC/SNAP 处理：
 *      - RFC1042 封装：移除 LLC/SNAP 头部，使用 ethertype
 *      - 其他封装：保留长度字段
 *   6. 构建 802.3 头部：DA + SA + Type/Length
 *
 * 802.11 帧格式:
 *   ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐
 *   │ Frame    │ Duration │ Addr1    │ Addr2    │ Addr3    │ Sequence │
 *   │ Control  │ ID       │          │          │          │ Control  │
 *   └──────────┴──────────┴──────────┴──────────┴──────────┴──────────┘
 *
 * 802.3 帧格式:
 *   ┌──────────┬──────────┬──────────┬──────────┐
 *   │ DA       │ SA       │ Type     │ Data     │
 *   │ (6 字节) │ (6 字节) │ (2 字节) │ (可变)   │
 *   └──────────┴──────────┴──────────┴──────────┘
 *
 * 注意事项:
 *   - 函数会修改 skb 的内容和长度
 *   - LLC/SNAP 头部会被移除（如果存在）
 *   - 转换失败时返回 -1，调用者需要释放 skb
 *
 * 相关函数:
 *   - atbmwifi_ieee80211_rx_h_amsdu(): 可能调用此函数
 *   - atbm_netrx_task(): 调用此函数
 *   - __atbmwifi_ieee80211_rx(): 调用此函数
 *
 **************************************************************************************************************/
 int atbmwifi_ieee80211_data_to_8023(struct atbm_buff *skb, const atbm_uint8 *addr,
			   enum atbm_nl80211_iftype iftype)
{
	struct atbmwifi_ieee80211_hdr *hdr = (struct atbmwifi_ieee80211_hdr *) ATBM_OS_SKB_DATA(skb);
	atbm_uint16 hdrlen, ethertype;
	atbm_uint8 *payload;
	atbm_uint8 dst[ATBM_ETH_ALEN];
	atbm_uint8 src[ATBM_ETH_ALEN];

	if (atbm_unlikely(!atbmwifi_ieee80211_is_data_present(hdr->frame_control)))
		return -1;

	hdrlen = atbmwifi_ieee80211_hdrlen(hdr->frame_control);

	/* convert IEEE 802.11 header + possible LLC headers into Ethernet
	 * header
	 * IEEE 802.11 address fields:
	 * ToDS FromDS Addr1 Addr2 Addr3 Addr4
	 *   0     0   DA    SA    BSSID n/a
	 *   0     1   DA    BSSID SA    n/a
	 *   1     0   BSSID SA    DA    n/a
	 *   1     1   RA    TA    DA    SA
	 */
	atbm_memcpy(dst, atbmwifi_ieee80211_get_DA(hdr), ATBM_ETH_ALEN);
	atbm_memcpy(src, atbmwifi_ieee80211_get_SA(hdr), ATBM_ETH_ALEN);

	switch (hdr->frame_control &
		atbm_cpu_to_le16(ATBM_IEEE80211_FCTL_TODS | ATBM_IEEE80211_FCTL_FROMDS)) {
	case atbm_cpu_to_le16(ATBM_IEEE80211_FCTL_TODS):
		if (atbm_unlikely(!atbmwifi_is_ap_mode(iftype)))
			return -1;
		break;
	case atbm_cpu_to_le16(ATBM_IEEE80211_FCTL_TODS | ATBM_IEEE80211_FCTL_FROMDS):
		//if (unlikely(iftype != NL80211_IFTYPE_WDS &&
		//	     iftype != ATBM_NL80211_IFTYPE_STATION))
			return -1;
		//break;
	case atbm_cpu_to_le16(ATBM_IEEE80211_FCTL_FROMDS):
		if (!atbmwifi_is_sta_mode(iftype) ||
		    (atbm_is_multicast_ether_addr(dst) &&
		     !atbm_compare_ether_addr(src, addr)))
			return -1;
		break;
	case atbm_cpu_to_le16(0):
		if (iftype != ATBM_NL80211_IFTYPE_ADHOC &&
		    iftype != ATBM_NL80211_IFTYPE_STATION)
				return -1;
		break;
	}

	payload = ATBM_OS_SKB_DATA(skb) + hdrlen;
	ethertype = (payload[6] << 8) | payload[7];
	if (atbm_likely((atbm_compare_ether_addr(payload, atbm_rfc1042_header) == 0 &&
		    ethertype != ATBM_ETH_P_AARP && ethertype != ATBM_ETH_P_IPX) ||
		   atbm_compare_ether_addr(payload, atbm_bridge_tunnel_header) == 0)) {
		/* remove RFC1042 or Bridge-Tunnel encapsulation and
		 * replace EtherType */
		atbm_skb_pull(skb, hdrlen + 6);
		atbm_memcpy(atbm_skb_push(skb, ATBM_ETH_ALEN), src, ATBM_ETH_ALEN);
		atbm_memcpy(atbm_skb_push(skb, ATBM_ETH_ALEN), dst, ATBM_ETH_ALEN);
	} else {
		struct atbmwifi_ieee8023_hdr *ehdr;
		atbm_uint16 len;

		atbm_skb_pull(skb, hdrlen);
		len = atbm_htons(ATBM_OS_SKB_LEN(skb));
		ehdr = (struct atbmwifi_ieee8023_hdr *) atbm_skb_push(skb, sizeof(struct atbmwifi_ieee8023_hdr));
		atbm_memcpy(ehdr->h_dest, dst, ATBM_ETH_ALEN);
		atbm_memcpy(ehdr->h_source, src, ATBM_ETH_ALEN);
		ehdr->h_proto = ethertype;
	}
	return 0;
}

/**************************************************************************************************************
 * 函数名: atbmwifi_ieee80211_data_from_8023
 *
 * 功能描述:
 *   将 802.3 以太网帧转换为 802.11 数据帧格式。
 *   这是 TX 流程中的关键转换函数，负责将以太网帧格式转换为 WiFi 帧格式。
 *
 * 在 TX 流程中的位置:
 *   阶段 2: 802.3 到 802.11 格式转换
 *   在 atbmwifi_tx_start() 或类似的发送入口函数中调用
 *
 * 参数说明:
 *   @skb:     输入的 802.3 以太网帧，输出为 802.11 数据帧
 *   @addr:    本地 MAC 地址（用于填充 802.11 头部）
 *   @iftype:  接口类型（STA/AP/ADHOC）
 *   @bssid:   BSSID 地址
 *   @qos:     是否为 QoS 数据帧
 *             - ATBM_TRUE: 添加 QoS 头部（802.11e）
 *             - ATBM_FALSE: 普通数据帧
 *   @encrype: 加密标志（保留参数，当前未使用）
 *
 * 返回值:
 *   0:  成功
 *   -ATBM_EINVAL: 输入帧长度不足
 *   -ATBM_EOPNOTSUPP: 不支持的接口类型
 *
 * 处理流程:
 *   1. 长度检查：确保帧长度 >= ATBM_ETH_HLEN（14 字节）
 *   2. 解析以太网类型：获取 ethertype
 *   3. 确定封装方式：
 *      - AARP/IPX: 使用 bridge_tunnel_header
 *      - ethertype > 0x600: 使用 RFC1042 封装
 *      - 其他: 无封装
 *   4. 移除以太网头部：atbm_skb_pull()
 *   5. 添加 LLC/SNAP 头部：如果需要封装
 *   6. 添加 802.11 头部：
 *      - 普通数据帧: 24 字节
 *      - QoS 数据帧: 26 字节（+2 字节 QoS 控制）
 *   7. 填充地址字段：根据接口类型设置 ToDS/FromDS 和地址
 *      - AP 模式: FromDS=1, Addr1=DA, Addr2=BSSID, Addr3=SA
 *      - STA 模式: ToDS=1, Addr1=BSSID, Addr2=SA, Addr3=DA
 *      - ADHOC 模式: ToDS=0, FromDS=0, Addr1=DA, Addr2=SA, Addr3=BSSID
 *   8. 设置 QoS 控制字段：如果是 QoS 帧，设置 TID
 *
 * 802.3 帧格式（输入）:
 *   ┌──────────┬──────────┬──────────┬──────────┐
 *   │ DA       │ SA       │ Type     │ Data     │
 *   │ (6 字节) │ (6 字节) │ (2 字节) │ (可变)   │
 *   └──────────┴──────────┴──────────┴──────────┘
 *
 * 802.11 帧格式（输出）:
 *   ┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐
 *   │ Frame    │ Duration │ Addr1    │ Addr2    │ Addr3    │ Sequence │ [QoS]    │ LLC/SNAP │
 *   │ Control  │ ID       │          │          │          │ Control  │ (可选)   │ + Data   │
 *   └──────────┴──────────┴──────────┴──────────┴──────────┴──────────┴──────────┴──────────┘
 *
 * 注意事项:
 *   - 函数会修改 skb 的内容和长度
 *   - LLC/SNAP 头部会被添加（如果需要）
 *   - QoS 帧会添加 2 字节的 QoS 控制字段
 *   - TID 从 skb->priority 获取
 *
 * 相关函数:
 *   - atbmwifi_ieee80211_data_to_8023(): 反向转换（RX 流程）
 *   - atbmwifi_tx_start(): 可能调用此函数
 *
 **************************************************************************************************************/
 int atbmwifi_ieee80211_data_from_8023(struct atbm_buff *skb, const atbm_uint8 *addr,
			     enum atbm_nl80211_iftype iftype, atbm_uint8 *bssid, ATBM_BOOL qos,atbm_uint8 encrype)
{
	struct atbmwifi_ieee80211_qos_hdr *  hdr;
	struct atbmwifi_ieee8023_hdr *ethdr = (struct atbmwifi_ieee8023_hdr *)ATBM_OS_SKB_DATA(skb);
	atbm_uint16 hdrlen, ethertype;
	atbm_uint8 * llchdr=ATBM_NULL;
	atbm_uint16 fc;
	const atbm_uint8 *encaps_data;
	int encaps_len, skip_header_bytes;
	
	if (atbm_unlikely(ATBM_OS_SKB_LEN(skb) < ATBM_ETH_HLEN))
		return -ATBM_EINVAL;
	/* convert Ethernet header to proper 802.11 header (based on
	 * operation mode) */
	ethertype = atbm_ntohs(ethdr->h_proto);
	
	
	fc = atbm_cpu_to_le16(ATBM_IEEE80211_FTYPE_DATA | ATBM_IEEE80211_STYPE_DATA);

	skip_header_bytes = ATBM_ETH_HLEN;
	if (ethertype == ATBM_ETH_P_AARP || ethertype == ATBM_ETH_P_IPX) {
		encaps_data = atbm_bridge_tunnel_header;
		encaps_len = sizeof(atbm_bridge_tunnel_header);
		skip_header_bytes -= 2;
	} else if (ethertype > 0x600) {
		encaps_data = atbm_rfc1042_header;
		encaps_len = sizeof(atbm_rfc1042_header);
		skip_header_bytes -= 2;
	} else {
		encaps_data = ATBM_NULL;
		encaps_len = 0;
	}
	atbm_skb_pull(skb, skip_header_bytes);
	
	if (encaps_data) {
		llchdr = atbm_skb_push(skb, encaps_len);		
	}

	hdrlen = 24;
	if (qos)
		hdrlen += 2;
	hdr= (struct atbmwifi_ieee80211_qos_hdr *)atbm_skb_push(skb, hdrlen);
	fc = atbm_cpu_to_le16(ATBM_IEEE80211_FTYPE_DATA | ATBM_IEEE80211_STYPE_DATA);

	switch (iftype) {
	case ATBM_NL80211_IFTYPE_AP:
	case ATBM_NL80211_IFTYPE_P2P_GO:
		fc |= atbm_cpu_to_le16(ATBM_IEEE80211_FCTL_FROMDS);
		/* DA BSSID SA */
		atbm_memcpy(hdr->addr1, ethdr->h_dest, ATBM_ETH_ALEN);
		atbm_memcpy(hdr->addr2, addr, ATBM_ETH_ALEN);
		atbm_memcpy(hdr->addr3, ethdr->h_source, ATBM_ETH_ALEN);
		break;
	case ATBM_NL80211_IFTYPE_STATION:
	case ATBM_NL80211_IFTYPE_P2P_CLIENT:
		fc |= atbm_cpu_to_le16(ATBM_IEEE80211_FCTL_TODS);
		/* BSSID SA DA */
		atbm_memcpy(hdr->addr1, bssid, ATBM_ETH_ALEN);
		atbm_memcpy(hdr->addr2, ethdr->h_source, ATBM_ETH_ALEN);
		atbm_memcpy(hdr->addr3, ethdr->h_dest, ATBM_ETH_ALEN);
		break;
#ifdef CONFIG_WIFI_IBSS
	case ATBM_NL80211_IFTYPE_ADHOC:
		/* DA SA BSSID */
		atbm_memcpy(hdr->addr1, ethdr->h_dest, ATBM_ETH_ALEN);
		atbm_memcpy(hdr->addr2, ethdr->h_source, ATBM_ETH_ALEN);
		atbm_memcpy(hdr->addr3, bssid, ATBM_ETH_ALEN);
		break;
#endif /*CONFIG_WIFI_IBSS*/
	default:
		return -ATBM_EOPNOTSUPP;
	}

	if (qos) {
		atbm_uint8 ack_policy=0, tid;
		fc |= atbm_cpu_to_le16(ATBM_IEEE80211_STYPE_QOS_DATA);
		/* use the data classifier to determine what 802.1d tag the
		 * data frame has */
		tid = skb->priority & ATBM_IEEE80211_QOS_CTL_TAG1D_MASK;
		
		/* qos header is 2 bytes */		
		hdr->qos_ctrl = 	ack_policy | tid;	
	}

	hdr->frame_control = fc;
	hdr->duration_id = 0;
	hdr->seq_ctrl = 0;
	if (encaps_data) {		
		atbm_memcpy(llchdr, encaps_data, encaps_len);
	}	
	return 0;
}

#if ATBM_DRIVER_PROCESS_BA
int ieee80211_tx_h_sequence(struct atbmwifi_sta_priv *sta_priv, struct atbm_buff *skb, int tid)
{
	struct atbmwifi_ieee80211_tx_info *info = ATBM_IEEE80211_SKB_TXCB(skb);
	struct atbmwifi_ieee80211_hdr *hdr = (struct atbmwifi_ieee80211_hdr *)ATBM_OS_SKB_DATA(skb);
	atbm_uint16 *seq;
	atbm_uint8 *qc;

	/*
	* Packet injection may want to control the sequence
	* number, if we have no matching interface then we
	* neither assign one ourselves nor ask the driver to.
	*/

	if (atbm_unlikely(atbmwifi_ieee80211_is_ctl(hdr->frame_control)))
		return TX_CONTINUE;

	if (atbmwifi_ieee80211_hdrlen(hdr->frame_control) < 24)
		return TX_CONTINUE;

	if (atbmwifi_ieee80211_is_qos_nullfunc(hdr->frame_control))
		return TX_CONTINUE;

	/*
	* Anything but QoS data that has a sequence number field
	* (is long enough) gets a sequence number from the global
	* counter.
	*/
	if (!atbmwifi_ieee80211_is_data_qos(hdr->frame_control)) {
	 /* driver should assign sequence number */
		return TX_CONTINUE;
	}
	info->flags |= ATBM_IEEE80211_TX_CTL_NOT_ASSIGN_SEQ;

	/*
	* This should be true for injected/management frames only, for
	* management frames we have set the IEEE80211_TX_CTL_ASSIGN_SEQ
	* above since they are not QoS-data frames.
	*/
	if (!sta_priv)
		 return TX_CONTINUE;

	/* include per-STA, per-TID sequence counter */

	//qc = ieee80211_get_qos_ctl(hdr);
	//tid = *qc & IEEE80211_QOS_CTL_TID_MASK;
	seq = &sta_priv->tid_seq[tid];

	hdr->seq_ctrl = atbm_cpu_to_le16(*seq);

	/* Increase the sequence number. */
	*seq = (*seq + 0x10) & ATBM_IEEE80211_SCTL_SEQ;

	return TX_CONTINUE;
}
#endif

 atbm_void atbmwifi_ieee80211_get_sta_rateinfo(struct atbmwifi_cfg80211_rate *sta,
 				struct atbmwifi_ieee80211_supported_band *band, atbm_uint8 *supp_rates,int supp_rates_len)
{
	int i,j;
	int rate =0;
	ATBM_BOOL is_basic;

	if(!(supp_rates && supp_rates_len > 0))
		return;

	for (i = 0; i < supp_rates_len; i++) {
		rate = (supp_rates[i] & 0x7f);
		is_basic = !!(supp_rates[i] & 0x80);
			
		for (j = 0; j < band->n_bitrates; j++) {
			if (band->bitrates[j].bitrate == rate) {
				sta->support_rates |= ATBM_BIT(j);
				sta->hw_support_rates |= band->bitrates[j].hw_value;
				if (is_basic)
					sta->basic_rates |= ATBM_BIT(j);
				break;
			}
		}
	}	

}

/*
add support rate ie
add exsupport rate ie
*/
 atbm_uint8 * atbmwifi_ieee80211_add_rate_ie(atbm_uint8 * pos,ATBM_BOOL no_cck,atbm_uint32 mask)
{

	int supp_rates_len;
	struct atbmwifi_ieee80211_rate *rate;
	int num_rates;
	int ext_rates_len;
	int i;
	atbm_uint8 * prate_len =0;
	
#if CONFIG_5G_SUPPORT
	if(no_cck){
		rate = atbmwifi_a_rates;
		num_rates = atbmwifi_a_rates_size;
	}else
#endif
	{
		rate = atbmwifi_g_rates;
		num_rates = atbmwifi_g_rates_size;		
	}
	//
	///build support rate
	//
	supp_rates_len = 0;
	*pos++ = ATBM_WLAN_EID_SUPP_RATES;
	//save rate len point
	prate_len = pos;
	*pos++ = supp_rates_len;
	
	for(i=0;i<num_rates;i++){
		if(ATBM_BIT(i) & mask){
			*pos++ = (atbm_uint8) (rate[i].bitrate)|(ATBM_IEEE80211_RT_BASIC & rate[i].rate_flag);
		}
		if (++supp_rates_len == 8){
			i++;
			break;
		}
	}

	*prate_len = (pos - prate_len) - 1;

	if(i == num_rates || mask < ATBM_BIT(i))
		return pos;

	//
	///buildex support rate
	//	

	ext_rates_len = 0;
	*pos++ = ATBM_WLAN_EID_EXT_SUPP_RATES;
	//save rate len point
	prate_len = pos;
	*pos++ = ext_rates_len;	
	for(;i<num_rates;i++){
		if(mask & ATBM_BIT(i)){
			*pos++ = (atbm_uint8) (rate[i].bitrate)|(ATBM_IEEE80211_RT_BASIC & rate[i].rate_flag);
			ext_rates_len++;
		}
	}
	*prate_len = ext_rates_len;
	
	return pos;
}

 /*
 only called when sending assoc req
 add support rate ie
 add exsupport rate ie
 */
  atbm_uint8 * atbmwifi_ieee80211_add_rate_ie_from_ap(atbm_uint8 * pos,ATBM_BOOL no_cck,atbm_uint32 mask, atbm_uint32 basic_mask)
 {
 
	 int supp_rates_len;
	 struct atbmwifi_ieee80211_rate *rate;
	 int num_rates;
	 int ext_rates_len;
	 int i;
	 atbm_uint8 * prate_len =0;
	 
#if CONFIG_5G_SUPPORT
	 if(no_cck){
		 rate = atbmwifi_a_rates;
		 num_rates = atbmwifi_a_rates_size;
	 }else
#endif
	 {
		 rate = atbmwifi_g_rates;
		 num_rates = atbmwifi_g_rates_size; 	 
	 }
	 //
	 ///build support rate
	 //
	 supp_rates_len = 0;
	 *pos++ = ATBM_WLAN_EID_SUPP_RATES;
	 //save rate len point
	 prate_len = pos;
	 *pos++ = supp_rates_len;
	 
	 for(i=0;i<num_rates;i++){
		 if(mask&ATBM_BIT(i)){
			 if(basic_mask & ATBM_BIT(i))
			 	*pos++ = (atbm_uint8) (rate[i].bitrate)|ATBM_IEEE80211_RT_BASIC;
			 else
			 	*pos++ = (atbm_uint8) (rate[i].bitrate);
			 if (++supp_rates_len == 8){
				i++;
				break;
			 }
		 }
	 }
	 *prate_len = supp_rates_len;
	 if(i==num_rates || mask < ATBM_BIT(i))
		 return pos;
 
	 //
	 ///buildex support rate
	 //  
	 ext_rates_len = 0;
	 *pos++ = ATBM_WLAN_EID_EXT_SUPP_RATES;
	 //save rate len point
	 prate_len = pos;
	 *pos++ = ext_rates_len; 
	 for(;i<num_rates;i++){
		 if(mask&ATBM_BIT(i)){
			 if(basic_mask & ATBM_BIT(i))
			 	*pos++ = (atbm_uint8) (rate[i].bitrate)|ATBM_IEEE80211_RT_BASIC;
			 else
			 	*pos++ = (atbm_uint8) (rate[i].bitrate);
			 ext_rates_len++;
		 }
	 }
	 *prate_len = ext_rates_len;
	 
	 return pos;
 }

/* ******************************************************************** */
/* AP API								*/
void atbm_sta_supp_rate(struct atbmwifi_ieee80211_supported_band *sband,struct atbmwifi_sta_priv *sta_priv,struct wsm_supp_rate *rate)
{
	struct atbmwifi_common	*hw_priv = sta_priv->priv->hw_priv;
    rate->supp_rates = atbm_cpu_to_le32(sta_priv->rate.hw_support_rates);
    rate->flags = 0;
#if CONFIG_5G_SUPPORT
    if(sband->band == ATBM_NL80211_BAND_5GHZ){
        rate->flags |= atbm_cpu_to_le16(WSM_SUPPORT_RATE_FLAGS__ENABLE_5G);
    }
#endif
    if(sta_priv->rate.ht_cap.ht_supported){
        rate->flags |= atbm_cpu_to_le16(WSM_SUPPORT_RATE_FLAGS__ENABLE_HT);
        rate->ht_flags = atbm_cpu_to_le32(sta_priv->rate.ht_cap.mcs.rx_mask[0]);
    }
#if CONFIG_VHT
	    if(sta_priv->rate.vht_cap.vht_supported){
	        atbm_uint8 peer_rx = atbm_le16_to_cpu(sta_priv->rate.vht_cap.vht_mcs.rx_mcs_map) &ATBM_IEEE80211_VHT_MCS_NOT_SUPPORTED; 
	        
	        switch(peer_rx){
	        case ATBM_IEEE80211_VHT_MCS_NOT_SUPPORTED:
	            rate->vht_flags = 0;
	            break;
	        case ATBM_IEEE80211_VHT_MCS_SUPPORT_0_7:
	            rate->vht_flags = 0xff;
	            break;
	        case ATBM_IEEE80211_VHT_MCS_SUPPORT_0_8:
	            rate->vht_flags = 0x1ff;
	            break;
	        case ATBM_IEEE80211_VHT_MCS_SUPPORT_0_9:
	            rate->vht_flags = 0x3ff;
	            break;
	        }

	        if(rate->vht_flags){
	            rate->flags |= atbm_cpu_to_le16(WSM_SUPPORT_RATE_FLAGS__ENABLE_VHT);
	        }
	    }
	
#endif
#if CONFIG_HE

	if(hw_priv->chip_version != OCEANUS_NO_WIFI6){
	    if(sta_priv->rate.he_cap.has_he){
	        switch(atbm_ieee80211_he_mcs_operation_rate(sta_priv->rate.he_cap.he_mcs_nss_supp.rx_mcs_80,1)){
	        case 7:
	           rate->he_flags = 0xff<<3;
	           break;
	        case 9:
	           rate->he_flags = 0x3ff<<3;
	           break;
	        case 11:
	           rate->he_flags = 0xfff<<3;
	           break;
	        default:
	           rate->he_flags = 0;
	           break;
	        }
	        
	        if(rate->he_flags){
	            if(!(sta_priv->rate.he_cap.he_operation.he_oper_params & ATBM_IEEE80211_HE_OPERATION_ER_SU_DISABLE)){
	                rate->he_flags |= 0x7;
	            }

	            rate->flags |= atbm_cpu_to_le16(WSM_SUPPORT_RATE_FLAGS__ENABLE_HE);
	        }
	    }
	}
#endif	
    if(atbmwifi_chtype_is_40M(sta_priv->rate.channel_type)){
        rate->flags |= atbm_cpu_to_le16(WSM_SUPPORT_RATE_FLAGS__ENABLE_40M);
    }

    wifi_printk(WIFI_ALWAYS, "%s:flags[%x],supp_rate[%x][%x],ht_flags[%x],vht_flags[%x],he_flags[%x]\n",__func__,
            rate->flags,rate->supp_rates, sta_priv->rate.support_rates,rate->ht_flags,rate->vht_flags,rate->he_flags);
}

/* Given a data frame determine the 802.1p/1d tag to use. */
  unsigned int atbm_cfg80211_classify8021d(struct atbm_buff *skb)
{
#ifdef CONFIG_OS_FREERTOS
	unsigned int dscp;
	struct atbm_ip_hdr *iphdr;
	struct atbmwifi_ieee8023_hdr *ethhdr;
	atbm_uint16 protocol = 0;
	
	ethhdr = (struct atbmwifi_ieee8023_hdr *)ATBM_OS_SKB_DATA(skb);
	protocol = atbm_htons(ethhdr->h_proto);

	if (protocol==atbm_htons(ATBM_ETH_P_IP)){
		//dscp = (unsigned int)(atbm_skb_pull(skb,ETH_HLEN+1)) & 0xfc;
		iphdr = (struct atbm_ip_hdr *)(((atbm_uint8 *)ATBM_OS_SKB_DATA(skb))+ATBM_ETH_HLEN);
		dscp =  ATBM_IPH_TOS(iphdr) & 0xfc;
	}
	else{
		return 0;
	}
	return dscp >> 5;
#else //#ifndef CONFIG_OS_FREERTOS
	return skb->priority;
#endif //#ifndef CONFIG_OS_FREERTOS
}

 atbm_uint8 *atbmwifi_find_ie(atbm_uint8 eid, const atbm_uint8 *ies, int len)
{
	while (len > 2 && ies[0] != eid) {
		len -= ies[1] + 2;
		ies += ies[1] + 2;
	}
	if (len < 2)
		return ATBM_NULL;
	if (len < 2 + ies[1])
		return ATBM_NULL;
	return (atbm_uint8 *)ies;
}

const atbm_uint8 *atbmwifi_find_ext_ie(atbm_uint8 ext_eid, const atbm_uint8 *ies, int len)
{
	return atbmwifi_find_ie(ext_eid, &ies[2], len-2);
}

const struct element *
atbmwifi_find_ext_elem(atbm_uint8 ext_eid, const atbm_uint8 *ies, int len)
{
	atbm_uint8 *ie = atbmwifi_find_ie(ATBM_WLAN_EID_EXTENSION, ies, len);
	return (ie && (ie[2] == ext_eid)) ? (const struct element *)&ie[2] : ATBM_NULL;
}

ATBM_BOOL atbmwifi_is_element_inherited(const struct element *elem,
				   const struct element *non_inherit_elem)
{
	atbm_uint8 id_len, ext_id_len, i, loop_len, id;
	const atbm_uint8 *list;

	if (elem->id == ATBM_WLAN_EID_MULTIPLE_BSSID)
		return ATBM_FALSE;

	if (!non_inherit_elem || non_inherit_elem->datalen < 2)
		return ATBM_TRUE;

	/*
	 * non inheritance element format is:
	 * ext ID (56) | IDs list len | list | extension IDs list len | list
	 * Both lists are optional. Both lengths are mandatory.
	 * This means valid length is:
	 * elem_len = 1 (extension ID) + 2 (list len fields) + list lengths
	 */
	id_len = non_inherit_elem->data[1];
	if (non_inherit_elem->datalen < 3 + id_len)
		return ATBM_TRUE;

	ext_id_len = non_inherit_elem->data[2 + id_len];
	if (non_inherit_elem->datalen < 3 + id_len + ext_id_len)
		return ATBM_TRUE;

	if (elem->id == ATBM_WLAN_EID_EXTENSION) {
		if (!ext_id_len)
			return ATBM_TRUE;
		loop_len = ext_id_len;
		list = &non_inherit_elem->data[3 + id_len];
		id = elem->data[0];
	} else {
		if (!id_len)
			return ATBM_TRUE;
		loop_len = id_len;
		list = &non_inherit_elem->data[2];
		id = elem->id;
	}

	for (i = 0; i < loop_len; i++) {
		if (list[i] == id)
			return ATBM_FALSE;
	}

	return ATBM_TRUE;
}

/*
 atbm_uint8 *atbm_cfg80211_find_vendor_ie(atbm_uint8 * oui,
				  const atbm_uint8 *ies, int len)
{
	struct atbmwifi_ieee80211_vendor_ie *ie;
	const atbm_uint8 *pos = ies, *end = ies + len;
	atbm_uint32  ie_oui;
	atbm_uint32  find_oui;
	
#define ATBM_WPA_GET_BE32(a) ((((atbm_uint32) (a)[0]) << 24) | (((atbm_uint32) (a)[1]) << 16) | \
				 (((atbm_uint32) (a)[2]) << 8) | ((atbm_uint32) (a)[3]))

	while (pos < end) {
		pos = atbmwifi_find_ie(ATBM_WLAN_EID_VENDOR_SPECIFIC, pos,
				       end - pos);
		if (!pos)
			return ATBM_NULL;

		if (end - pos < sizeof(*ie))
			return ATBM_NULL;

		ie = (struct atbmwifi_ieee80211_vendor_ie *)pos;
		ie_oui = ATBM_WPA_GET_BE32(&ie->oui[0]);
		find_oui = ATBM_WPA_GET_BE32(oui);
		if (ie_oui == find_oui)
			return(atbm_uint8 *) pos;

		pos += 2 + ie->len;
	}
	return ATBM_NULL;
}
*/
/*
 * Set the direction field and address fields of an outgoing
 * non-QoS frame.  Note this should be called early on in
 * constructing a frame as it sets i_fc[1]; other bits can
 * then be or'd in.
 */
#if 0
atbm_void atbmwifi_ieee80211_buid_machdr(
	struct atbmwifi_vif *priv,
	struct atbmwifi_ieee80211_hdr *wh,
	int type,
	const atbm_uint8 sa[ATBM_IEEE80211_ADDR_LEN],
	const atbm_uint8 da[ATBM_IEEE80211_ADDR_LEN],
	const atbm_uint8 bssid[ATBM_IEEE80211_ADDR_LEN])
{

	wh->i_fc[0] =/* IEEE80211_FC0_VERSION_0 |*/ type;
	if ((type & ATBM_IEEE80211_FCTL_FTYPE ) == ATBM_IEEE80211_FTYPE_DATA) {
		switch (priv->iftype) {
		case ATBM_NL80211_IFTYPE_STATION:
			wh->i_fc[1] = ATBM_IEEE80211_FCTL_TODS;
			ATBM_IEEE80211_ADDR_COPY(wh->addr1, bssid);
			ATBM_IEEE80211_ADDR_COPY(wh->addr2, sa);
			ATBM_IEEE80211_ADDR_COPY(wh->addr3, da);
			break;
		case ATBM_NL80211_IFTYPE_ADHOC:
			wh->i_fc[1] = 0;//IEEE80211_FC1_DIR_NODS;
			ATBM_IEEE80211_ADDR_COPY(wh->addr1, da);
			ATBM_IEEE80211_ADDR_COPY(wh->addr2, sa);
			ATBM_IEEE80211_ADDR_COPY(wh->addr3, bssid);
			break;
		case ATBM_NL80211_IFTYPE_AP:
			wh->i_fc[1] = ATBM_IEEE80211_FCTL_FROMDS;
			ATBM_IEEE80211_ADDR_COPY(wh->addr1, da);
			ATBM_IEEE80211_ADDR_COPY(wh->addr2, bssid);
			ATBM_IEEE80211_ADDR_COPY(wh->addr3, sa);
			break;
		default:
			break;
		}
	}
	else {	
		wh->i_fc[1] = 0;// IEEE80211_FC1_DIR_NODS;
		ATBM_IEEE80211_ADDR_COPY(wh->addr1, da);
		ATBM_IEEE80211_ADDR_COPY(wh->addr2, sa);
		ATBM_IEEE80211_ADDR_COPY(wh->addr3, bssid);
	}
	wh->duration_id= 0;
	/* NB: use non-QoS tid */
	wh->seq_ctrl = 0;
	
}

#endif


/* TODO: maintain separate sequence and fragment numbers for each AC
 * TODO: IGMP snooping to track which multicasts to forward - and use QOS-DATA
 * if only WMM stations are receiving a certain group */


static  atbm_uint8 wmm_aci_aifsn(int aifsn, int acm, int aci)
{
	atbm_uint8 ret;
	ret = (aifsn << ATBM_WMM_AC_AIFNS_SHIFT) & ATBM_WMM_AC_AIFSN_MASK;
	if (acm)
		ret |= ATBM_WMM_AC_ACM;
	ret |= (aci << ATBM_WMM_AC_ACI_SHIFT) & ATBM_WMM_AC_ACI_MASK;
	return ret;
}


static  atbm_uint8 wmm_ecw(int ecwmin, int ecwmax)
{
	return ((ecwmin << ATBM_WMM_AC_ECWMIN_SHIFT) & ATBM_WMM_AC_ECWMIN_MASK) |
		((ecwmax << ATBM_WMM_AC_ECWMAX_SHIFT) & ATBM_WMM_AC_ECWMAX_MASK);
}


/*
 * Add WMM Parameter Element to Beacon, Probe Response, and (Re)Association
 * Response frames.
 */
 atbm_uint8 * atbmwifi_ieee80211_add_wmm_param(struct atbmwifi_vif *priv, atbm_uint8 *eid)
{
	atbm_uint8 *pos = eid;
	struct wmm_parameter_element *wmm =
		(struct wmm_parameter_element *) (pos + 2);
	int e;
	
	if (!priv->bss.sta_priv.wmm_used){
		wifi_printk(WIFI_ALWAYS,"%s %d ,pos:%x wmm:%x\n",__func__,__LINE__,pos,wmm);
		return eid;
	}
	
	eid[0] = ATBM_WLAN_EID_VENDOR_SPECIFIC;
	wmm->oui[0] = 0x00;
	wmm->oui[1] = 0x50;
	wmm->oui[2] = 0xf2;
	wmm->oui_type = ATBM_WMM_OUI_TYPE;
	wmm->oui_subtype = ATBM_WMM_OUI_SUBTYPE_PARAMETER_ELEMENT;
	wmm->version = WMM_VERSION;
	wmm->qos_info = priv->bss.parameter_set_count & 0xf;

	if (priv->bss.sta_priv.uapsd_supported)
		wmm->qos_info |= ATBM_IEEE80211_WMM_IE_AP_QOSINFO_UAPSD;

	wmm->reserved = 0;

	/* fill in a parameter set record for each AC */
	for (e = 0; e < 4; e++) {
		struct wmm_ac_parameter *ac = &wmm->ac[e];		
		struct config_edca_params *wmm_param = &priv->wmm_params[e];
		

		ac->aci_aifsn = wmm_aci_aifsn(wmm_param->aifns,
					      wmm_param->wmep_acm,
					      e);
		ac->cw = wmm_ecw(wmm_param->cwMin, wmm_param->cwMax);
		ac->txop_limit = atbm_host_to_le16( wmm_param->txOpLimit);
	}

	pos = (atbm_uint8 *) (wmm + 1);
	eid[1] = pos - eid - 2; /* element length */

	return pos;
}

#define	WME_OUI_BYTES		0x00, 0x50, 0xf2
/*
 * Add a WME Info element to a frame.
 */
 atbm_uint8 *atbmwifi_ieee80211_add_wme(struct atbmwifi_vif *priv, atbm_uint8 *eid)
{
	atbm_uint8 *pos = eid;
	struct wmm_information_element *wmm =
		(struct wmm_information_element *) (pos + 2);
	//int e;

	if (!priv->bss.sta_priv.wmm_used)
		return eid;
	
	eid[0] = ATBM_WLAN_EID_VENDOR_SPECIFIC;
	wmm->oui[0] = 0x00;
	wmm->oui[1] = 0x50;
	wmm->oui[2] = 0xf2;
	wmm->oui_type = ATBM_WMM_OUI_TYPE;
	wmm->oui_subtype = ATBM_WMM_OUI_SUBTYPE_INFORMATION_ELEMENT;
	wmm->version = WMM_VERSION;

	/* QoS Info field depends on operating mode */
	switch (priv->iftype) {
	case ATBM_NL80211_IFTYPE_AP:
	case ATBM_NL80211_IFTYPE_P2P_GO:
		wmm->qos_info = priv->bss.parameter_set_count & 0xf;
		if (priv->bss.sta_priv.uapsd_supported)
			wmm->qos_info |= ATBM_IEEE80211_WMM_IE_AP_QOSINFO_UAPSD;
		break;
	case ATBM_NL80211_IFTYPE_STATION:
	case ATBM_NL80211_IFTYPE_P2P_CLIENT:
		if (priv->bss.sta_priv.uapsd_supported){
			wmm->qos_info = priv->wmm_params[0].uapsdEnable
				|( priv->wmm_params[1].uapsdEnable<<1)
				|( priv->wmm_params[2].uapsdEnable<<2)
				|( priv->wmm_params[3].uapsdEnable<<3);
			wmm->qos_info |= (priv->bss.uapsd_max_sp_len <<
				 ATBM_IEEE80211_WMM_IE_STA_QOSINFO_SP_SHIFT);
		}
		else{
			wmm->qos_info = 0;
		}
		break;
	default:
		wmm->qos_info = 0;
	}
	pos = (atbm_uint8 *) (wmm + 1);
	eid[1] = pos - eid - 2; /* element length */

	return pos;
}

 atbm_uint8 * atbmwifi_ieee80211_add_wpa_ie(struct atbmwifi_vif *priv, atbm_uint8 *eid)
{
	return 0;
}

 atbm_uint8 * atbmwifi_ieee80211_add_ht_operation(struct atbmwifi_vif *priv, atbm_uint8 *eid)
{
#if BW_40M_SUPPORT
	struct atbmwifi_cfg *config = atbmwifi_get_config(priv);
#endif
	struct atbmwifi_ieee80211_ht_operation *oper;
	atbm_uint8 *pos = eid;

	*pos++ = ATBM_WLAN_EID_HT_OPERATION;
	*pos++ = sizeof(*oper);
	wifi_printk(WIFI_DBG_MSG,"atbm: atbmwifi_ieee80211_add_ht_operation() ===>\n");

	oper = (struct atbmwifi_ieee80211_ht_operation *) pos;
	atbm_memset(oper, 0, sizeof(*oper));
	oper->control_chan = priv->config.channel_index;
	oper->ht_param = ATBM_IEEE80211_HT_PARAM_CHA_SEC_NONE
						/*|IEEE80211_HT_PARAM_SPSMP_SUPPORT*/;
	oper->operation_mode = ATBM_IEEE80211_HT_OP_MODE_PROTECTION_NONHT_MIXED
							|ATBM_IEEE80211_HT_OP_MODE_NON_GF_STA_PRSNT
							|ATBM_IEEE80211_HT_OP_MODE_BURST_TX_LIMIT;
#if BW_40M_SUPPORT
	if (config->secondary_channel == 1){
		oper->ht_param |= ATBM_IEEE80211_HT_PARAM_CHA_SEC_ABOVE |
			ATBM_IEEE80211_HT_PARAM_CHAN_WIDTH_ANY;
	}
	else if (config->secondary_channel == -1){
		oper->ht_param |= ATBM_IEEE80211_HT_PARAM_CHA_SEC_BELOW |
			ATBM_IEEE80211_HT_PARAM_CHAN_WIDTH_ANY;
	}
	//enable rifs mode
	oper->ht_param |=ATBM_IEEE80211_HT_PARAM_RIFS_MODE;
#endif
	atbm_memcpy(pos, oper, sizeof(*oper));
	pos += sizeof(*oper);
	wifi_printk(WIFI_DBG_MSG,"atbm: atbmwifi_ieee80211_add_ht_operation() <===\n");
	return pos;
}

 atbm_uint8 * atbmwifi_ieee80211_add_ht_ie(struct atbmwifi_vif *priv,
 			struct atbmwifi_ieee80211_supported_band *band, atbm_uint8 * pos)
{
	//struct atbmwifi_ieee80211_ht_info *ht_info;
	atbm_uint16 cap = 0;
	atbm_uint16 tmp;


	//case IEEE80211_SMPS_OFF:
	cap = band->ht_cap.cap;
		
	/* reserve and fill IE */
	*pos++ = ATBM_WLAN_EID_HT_CAPABILITY;
	*pos++ = sizeof(struct atbmwifi_ieee80211_ht_cap);
	atbm_memset(pos, 0, sizeof(struct atbmwifi_ieee80211_ht_cap));

	/* capability flags */
	tmp = atbm_cpu_to_le16(cap);
	atbm_memcpy(pos, &tmp, sizeof(atbm_uint16));
	pos += sizeof(atbm_uint16);

	/* AMPDU parameters */
	*pos++ = band->ht_cap.ampdu_factor |
		 ( band->ht_cap.ampdu_density<<
			ATBM_IEEE80211_HT_AMPDU_PARM_DENSITY_SHIFT);

	/* MCS set */
	atbm_memcpy(pos, &band->ht_cap.mcs, sizeof(band->ht_cap.mcs));
	pos += sizeof(band->ht_cap.mcs);

	/* extended capabilities */
	pos += sizeof(atbm_uint16);

	/* BF capabilities */
	pos += sizeof(atbm_uint32);

	/* antenna selection */
	pos += sizeof(atbm_uint8);
	return pos;
}

#if CONFIG_HE
/* 802.11ax HE PPE Thresholds */
#define ATBM_IEEE80211_PPE_THRES_NSS_SUPPORT_2NSS			(1)
#define ATBM_IEEE80211_PPE_THRES_NSS_POS				(0)
#define ATBM_IEEE80211_PPE_THRES_NSS_MASK				(7)
#define ATBM_IEEE80211_PPE_THRES_RU_INDEX_BITMASK_2x966_AND_966_RU	\
	(ATBM_BIT(5) | ATBM_BIT(6))
#define ATBM_IEEE80211_PPE_THRES_RU_INDEX_BITMASK_MASK		0x78
#define ATBM_IEEE80211_PPE_THRES_RU_INDEX_BITMASK_POS		(3)
#define ATBM_IEEE80211_PPE_THRES_INFO_PPET_SIZE			(3)

atbm_uint8 *atbmwifi_ieee80211_ie_build_he_cap(struct atbm_ieee80211_sta_he_cap *he_cap, atbm_uint8 *pos)
{
	atbm_uint8 n;
	atbm_uint8 ie_len;
	atbm_uint8 *orig_pos = pos;

	/* Make sure we have place for the IE */
	/*
	 * TODO: the 1 added is because this temporarily is under the EXTENSION
	 * IE. Get rid of it when it moves.
	 */
	if (!he_cap)
		return orig_pos;

	n = atbm_ieee80211_he_mcs_nss_size(&he_cap->he_cap_elem);
	ie_len = 2 + 1 +
		 sizeof(he_cap->he_cap_elem) + n +
		 atbm_ieee80211_he_ppe_size(he_cap->ppe_thres[0],
				       he_cap->he_cap_elem.phy_cap_info);

	*pos++ = ATBM_WLAN_EID_EXTENSION;
	pos++; /* We'll set the size later below */
	*pos++ = ATBM_WLAN_EID_EXT_HE_CAPABILITIES;

	/* Fixed data */
	atbm_memcpy(pos, &he_cap->he_cap_elem, sizeof(he_cap->he_cap_elem));
	pos += sizeof(he_cap->he_cap_elem);

	atbm_memcpy(pos, &he_cap->he_mcs_nss_supp, n);
	pos += n;

	/* Check if PPE Threshold should be present */
	if ((he_cap->he_cap_elem.phy_cap_info[6] &
	     ATBM_IEEE80211_HE_PHY_CAP6_PPE_THRESHOLD_PRESENT) == 0)
		goto end;

	/*
	 * Calculate how many PPET16/PPET8 pairs are to come. Algorithm:
	 * (NSS_M1 + 1) x (num of 1 bits in RU_INDEX_BITMASK)
	 */
	n = hweight8(he_cap->ppe_thres[0] &
		     ATBM_IEEE80211_PPE_THRES_RU_INDEX_BITMASK_MASK);
	n *= (1 + ((he_cap->ppe_thres[0] & ATBM_IEEE80211_PPE_THRES_NSS_MASK) >>
		   ATBM_IEEE80211_PPE_THRES_NSS_POS));

	/*
	 * Each pair is 6 bits, and we need to add the 7 "header" bits to the
	 * total size.
	 */
	n = (n * ATBM_IEEE80211_PPE_THRES_INFO_PPET_SIZE * 2) + 7;
	n = DIV_ROUND_UP(n, 8);

	/* Copy PPE Thresholds */
	atbm_memcpy(pos, &he_cap->ppe_thres, n);
	pos += n;

end:
	orig_pos[1] = (pos - orig_pos) - 2;
	return pos;
}

atbm_uint8 *ieee80211_ie_build_he_oper(atbm_uint8 *pos)
{
	struct atbm_ieee80211_he_operation *he_oper;
	//struct atbm_ieee80211_he_6ghz_oper *he_6ghz_op;
	atbm_uint32 he_oper_params;
	atbm_uint8 ie_len = 1 + offsetof(struct atbm_ieee80211_he_operation, optional);

	//if (chandef->chan->band == NL80211_BAND_6GHZ)
	//	ie_len += sizeof(struct atbm_ieee80211_he_6ghz_oper);

	*pos++ = ATBM_WLAN_EID_EXTENSION;
	*pos++ = ie_len;
	*pos++ = ATBM_WLAN_EID_EXT_HE_OPERATION;

	he_oper_params = 0;
	//he_oper_params |= u32_encode_bits(1023, /* disabled */IEEE80211_HE_OPERATION_RTS_THRESHOLD_MASK);
	he_oper_params |= 1023<<ATBM_IEEE80211_HE_OPERATION_RTS_THRESHOLD_OFFSET;

	//he_oper_params |= ATBM_IEEE80211_HE_OPERATION_ER_SU_DISABLE;
		//u32_encode_bits(1,IEEE80211_HE_OPERATION_ER_SU_DISABLE);
	he_oper_params |= ATBM_IEEE80211_HE_OPERATION_BSS_COLOR_DISABLED;
	//u32_encode_bits(1	IEEE80211_HE_OPERATION_BSS_COLOR_DISABLED);
	#if 0
	if (chandef->chan->band == NL80211_BAND_6GHZ)
		he_oper_params |= IEEE80211_HE_OPERATION_6GHZ_OP_INFO;//u32_encode_bits(1,IEEE80211_HE_OPERATION_6GHZ_OP_INFO);
	#endif
	he_oper = (struct atbm_ieee80211_he_operation *)pos;
	he_oper->he_oper_params = atbm_cpu_to_le32(he_oper_params);

	/* don't require special HE peer rates */
	he_oper->he_mcs_nss_set = atbm_cpu_to_le16(0xfffe);
	pos += offsetof(struct atbm_ieee80211_he_operation, optional);
	return pos;
}

atbm_uint8 *atbmwifi_ieee80211_ie_build_ext_capab(atbm_uint8 *buf, atbm_uint8 *extended_capa, int extended_capa_len)
{
	atbm_uint8 len = extended_capa_len, i;
	atbm_uint8 *pos = buf;

	*pos++ = ATBM_WLAN_EID_EXT_CAPAB;
	*pos++ = len;

	for (i = 0; i < len; i++, pos++) {
		*pos = extended_capa[i];
	}

	while (len > 0 && buf[1 + len] == 0) {
		len--;
	}

	buf[1] = len;

	if (len == 0)
		return buf;

	return buf + 2 + len;
}
#endif

/* frame sending functions */
 atbm_void atbmwifi_ieee80211_send_deauth_disassoc(struct atbmwifi_vif *priv,
					   const atbm_uint8 *da,const atbm_uint8 *bssid, atbm_uint16 stype, atbm_uint16 reason,
					   atbm_void *cookie, ATBM_BOOL send_frame)
{
	struct atbmwifi_common * hw_priv = priv->hw_priv;
	struct atbm_buff *skb;
	struct atbmwifi_ieee80211_mgmt *mgmt;

	skb = atbm_dev_alloc_skb( sizeof(*mgmt));
	if (!skb)
		return;

	//atbm_skb_reserve(skb, local->hw.extra_tx_headroom);

	mgmt = (struct atbmwifi_ieee80211_mgmt *) atbm_skb_put(skb, 24);
	atbm_memset(mgmt, 0, 24);
	atbm_memcpy(mgmt->da, da,ATBM_ETH_ALEN);
	atbm_memcpy(mgmt->sa, priv->mac_addr, ATBM_ETH_ALEN);
	atbm_memcpy(mgmt->bssid, bssid, ATBM_ETH_ALEN);
	mgmt->frame_control = atbm_cpu_to_le16(ATBM_IEEE80211_FTYPE_MGMT | stype);
	atbm_skb_put(skb, 2);
	/* u.deauth.reason_code == u.disassoc.reason_code */
	mgmt->u.deauth.reason_code = atbm_cpu_to_le16(reason);

	//__cfg80211_send_deauth(sdata->dev, (atbm_uint8 *)mgmt,  skb->data_len);

	ATBM_IEEE80211_SKB_TXCB(skb)->flags |= ATBM_IEEE80211_TX_CTL_USE_MINRATE;

	atbmwifi_tx(hw_priv,skb,priv);

}

 atbm_void atbmwifi_ieee80211_connection_loss(struct atbmwifi_vif *priv)
{

	if (!priv->assoc_ok) {
		return;
	}

	/*
	 * must be outside lock due to cfg80211,
	 * but that's not a problem.
	 */
	atbmwifi_ieee80211_send_deauth_disassoc(priv, priv->daddr,priv->bssid,
				       ATBM_IEEE80211_STYPE_DEAUTH,
				       ATBM_WLAN_REASON_DISASSOC_DUE_TO_INACTIVITY,
				       ATBM_NULL, ATBM_TRUE);
}

atbm_void atbmwifi_ieee80211_ht_cap_ie_to_sta_ht_cap(struct atbmwifi_ieee80211_supported_band *sband,
				       struct atbmwifi_ieee80211_ht_cap *ht_cap_ie,
				       struct atbmwifi_ieee80211_sta_ht_cap *ht_cap)
{
	atbm_uint8 ampdu_info, tx_mcs_set_cap;
	int i, max_tx_streams;

	ATBM_BUG_ON(!ht_cap);

	atbm_memset(ht_cap, 0, sizeof(*ht_cap));

	if (!ht_cap_ie || !sband->ht_cap.ht_supported)
		return;

	ht_cap->ht_supported = ATBM_TRUE;

	/*
	 * The bits listed in this expression should be
	 * the same for the peer and us, if the station
	 * advertises more then we can't use those thus
	 * we mask them out.
	 */
	ht_cap->cap = atbm_le16_to_cpu(ht_cap_ie->cap_info) & sband->ht_cap.cap;
	/*
	 * The STBC bits are asymmetric -- if we don't have
	 * TX then mask out the peer's RX and vice versa.
	 */
	if (!(sband->ht_cap.cap & ATBM_IEEE80211_HT_CAP_TX_STBC))
		ht_cap->cap &= ~ATBM_IEEE80211_HT_CAP_RX_STBC;
	if (!(sband->ht_cap.cap & ATBM_IEEE80211_HT_CAP_RX_STBC))
		ht_cap->cap &= ~ATBM_IEEE80211_HT_CAP_TX_STBC;

	ampdu_info = ht_cap_ie->ampdu_params_info;
	ht_cap->ampdu_factor =
		ampdu_info & ATBM_IEEE80211_HT_AMPDU_PARM_FACTOR;
	ht_cap->ampdu_density =
		(ampdu_info & ATBM_IEEE80211_HT_AMPDU_PARM_DENSITY) >> 2;

	/* own MCS TX capabilities */
	tx_mcs_set_cap = sband->ht_cap.mcs.tx_params;

	/* Copy peer MCS TX capabilities, the driver might need them. */
	ht_cap->mcs.tx_params = ht_cap_ie->mcs.tx_params;

	/* can we TX with MCS rates? */
	if (!(tx_mcs_set_cap & ATBM_IEEE80211_HT_MCS_TX_DEFINED))
		return;

	/* Counting from 0, therefore +1 */
	if (tx_mcs_set_cap & ATBM_IEEE80211_HT_MCS_TX_RX_DIFF)
		max_tx_streams =
			((tx_mcs_set_cap & ATBM_IEEE80211_HT_MCS_TX_MAX_STREAMS_MASK)
				>> ATBM_IEEE80211_HT_MCS_TX_MAX_STREAMS_SHIFT) + 1;
	else
		max_tx_streams = ATBM_IEEE80211_HT_MCS_TX_MAX_STREAMS;

	/*
	 * 802.11n-2009 20.3.5 / 20.6 says:
	 * - indices 0 to 7 and 32 are single spatial stream
	 * - 8 to 31 are multiple spatial streams using equal modulation
	 *   [8..15 for two streams, 16..23 for three and 24..31 for four]
	 * - remainder are multiple spatial streams using unequal modulation
	 */
	for (i = 0; i < max_tx_streams; i++)
		ht_cap->mcs.rx_mask[i] =
			sband->ht_cap.mcs.rx_mask[i] & ht_cap_ie->mcs.rx_mask[i];

	if (tx_mcs_set_cap & ATBM_IEEE80211_HT_MCS_TX_UNEQUAL_MODULATION)
		for (i = ATBM_IEEE80211_HT_MCS_UNEQUAL_MODULATION_START_BYTE;
		     i < ATBM_IEEE80211_HT_MCS_MASK_LEN; i++)
			ht_cap->mcs.rx_mask[i] =
				sband->ht_cap.mcs.rx_mask[i] &
					ht_cap_ie->mcs.rx_mask[i];

	/* handle MCS rate 32 too */
	if (sband->ht_cap.mcs.rx_mask[32/8] & ht_cap_ie->mcs.rx_mask[32/8] & 1)
		ht_cap->mcs.rx_mask[32/8] |= 1;
}

#if CONFIG_VHT
atbm_uint8 *atbmwifi_ieee80211_ie_build_vht_cap(const struct atbm_ieee80211_sta_vht_cap *vht_cap, atbm_uint8 *pos, int nsts){
	atbm_uint32 tmp;

	*pos++ = ATBM_WLAN_EID_VHT_CAP;
	*pos++ = sizeof(struct atbm_ieee80211_vht_cap);
	
	tmp = atbm_cpu_to_le32(vht_cap->cap);
	atbm_memcpy(pos,&tmp,sizeof(atbm_uint32));
	if (nsts != 0) {
		struct atbm_ieee80211_vht_cap *cap = (struct atbm_ieee80211_vht_cap *)pos; 
		atbm_uint32 hapd_nsts;

		hapd_nsts = cap->vht_cap_info;
		hapd_nsts = (hapd_nsts >> ATBM_IEEE80211_VHT_CAP_BEAMFORMEE_STS_SHIFT) & 7;
		cap->vht_cap_info &=
			~(hapd_nsts << ATBM_IEEE80211_VHT_CAP_BEAMFORMEE_STS_SHIFT);
		cap->vht_cap_info |=
			atbm_cpu_to_le32(nsts << ATBM_IEEE80211_VHT_CAP_BEAMFORMEE_STS_SHIFT);
	}

	pos += sizeof(atbm_uint32);
	
	atbm_memcpy(pos,&vht_cap->vht_mcs,sizeof(vht_cap->vht_mcs));
	pos += sizeof(vht_cap->vht_mcs);
	return pos;
}

atbm_uint8 *ieee80211_add_vht_ie(struct atbmwifi_ieee80211_supported_band *sband,
               struct atbm_ieee80211_vht_cap *ap_vht_cap,
               atbm_uint8 *pos)
{
    atbm_uint32 cap;
    struct atbm_ieee80211_sta_vht_cap vht_cap;
    const struct atbm_ieee80211_sta_vht_cap *support_cap;
    atbm_uint32 mask, ap_bf_sts, our_bf_sts;

    if(ap_vht_cap == ATBM_NULL){
        return pos;
    }

	support_cap = &sband->vht_cap;
    memcpy(&vht_cap,support_cap, sizeof(vht_cap));

    cap = vht_cap.cap;
    if (!(ap_vht_cap->vht_cap_info &
            atbm_cpu_to_le32(ATBM_IEEE80211_VHT_CAP_SU_BEAMFORMER_CAPABLE)))
        cap &= ~(ATBM_IEEE80211_VHT_CAP_SU_BEAMFORMEE_CAPABLE |
             ATBM_IEEE80211_VHT_CAP_MU_BEAMFORMEE_CAPABLE);
    else if (!(ap_vht_cap->vht_cap_info &
            atbm_cpu_to_le32(ATBM_IEEE80211_VHT_CAP_MU_BEAMFORMER_CAPABLE)))
        cap &= ~ATBM_IEEE80211_VHT_CAP_MU_BEAMFORMEE_CAPABLE; 
    
    mask = ATBM_IEEE80211_VHT_CAP_BEAMFORMEE_STS_MASK;
    ap_bf_sts = atbm_le32_to_cpu(ap_vht_cap->vht_cap_info) & mask;
    our_bf_sts = cap & mask;

    if (ap_bf_sts < our_bf_sts) {
        cap &= ~mask;
        cap |= ap_bf_sts;
    }

    /* reserve and fill IE */
    return atbmwifi_ieee80211_ie_build_vht_cap(&vht_cap, pos, 0);
}

atbm_uint8 *ieee80211_add_op_mode_ie(struct atbmwifi_ieee80211_supported_band *sband, atbm_uint8 band_width, atbm_uint8 *pos){
	if(sband->band != ATBM_NL80211_BAND_5GHZ)
		return pos;

	*pos++ = ATBM_WLAN_EID_VHT_OPERATING_MODE_NOTIFICATION;
	*pos++ = 1;
	*pos++ = band_width;
	return pos;
}

atbm_uint8 * atbmwifi_ieee80211_ie_build_vht_oper(struct atbmwifi_vif *priv, atbm_uint8 *eid)
{
	struct atbm_ieee80211_vht_operation *oper;
	atbm_uint8 *pos = eid;

	*pos++ = ATBM_WLAN_EID_VHT_OPERATION;
	*pos++ = sizeof(*oper);

	oper = (struct atbm_ieee80211_vht_operation *) pos;
	atbm_memset(oper, 0, sizeof(*oper));

	/*
	 * center freq = 5 GHz + (5 * index)
	 * So index 42 gives center freq 5.210 GHz
	 * which is channel 42 in 5G band
	 */
	oper->center_freq_seg0_idx =
		priv->config.channel_index;
	oper->center_freq_seg1_idx =
		0; /*No 80M*/

	oper->chan_width = 0; /*No 80M*/

#if 0
	if (hapd->iconf->vht_oper_chwidth == 2) {
		/*
		 * Convert 160 MHz channel width to new style as interop
		 * workaround.
		 */
		oper->vht_op_info_chwidth = 1;
		oper->vht_op_info_chan_center_freq_seg1_idx =
			oper->vht_op_info_chan_center_freq_seg0_idx;
		if (hapd->iconf->channel <
		    hapd->iconf->vht_oper_centr_freq_seg0_idx)
			oper->vht_op_info_chan_center_freq_seg0_idx -= 8;
		else
			oper->vht_op_info_chan_center_freq_seg0_idx += 8;
	} else if (hapd->iconf->vht_oper_chwidth == 3) {
		/*
		 * Convert 80+80 MHz channel width to new style as interop
		 * workaround.
		 */
		oper->vht_op_info_chwidth = 1;
	}
#endif

	/* VHT Basic MCS set comes from hw */
	/* Hard code 1 stream, MCS0-7 is a min Basic VHT MCS rates */
	oper->basic_mcs_set = atbm_cpu_to_le16(0xfffe);
	pos += sizeof(*oper);

	return pos;
}

void atbmwifi_ieee80211_vht_cap_ie_to_sta_vht_cap(struct atbmwifi_ieee80211_supported_band *band,
					const struct atbm_ieee80211_vht_cap *vht_cap_ie,
				    struct atbm_ieee80211_sta_vht_cap *vht_cap)
{
	struct atbm_ieee80211_sta_vht_cap own_cap;
    const struct atbm_ieee80211_sta_vht_cap *sband_cap;
	atbm_uint32 cap_info, i;
	//bool have_80mhz;

	memset(vht_cap, 0, sizeof(*vht_cap));

    if(band == NULL)
        return;

	if (!band->ht_cap.ht_supported)
		return;

	if (!vht_cap_ie)
		return;

#if 0
	/* Allow VHT if at least one channel on the sband supports 80 MHz */
	have_80mhz = false;
	for (i = 0; i < sband->n_channels; i++) {
		if (sband->channels[i].flags & (IEEE80211_CHAN_DISABLED |
						IEEE80211_CHAN_NO_80MHZ))
			continue;

		have_80mhz = true;
		break;
	}

	if (!have_80mhz)
		return;
#endif
	/*
	 * A VHT STA must support 40 MHz, but if we verify that here
	 * then we break a few things - some APs (e.g. Netgear R6300v2
	 * and others based on the BCM4360 chipset) will unset this
	 * capability bit when operating in 20 MHz.
	 */

	vht_cap->vht_supported = ATBM_TRUE;

    atbm_memcpy(&own_cap,&band->vht_cap,sizeof(struct atbm_ieee80211_sta_vht_cap));
#if 0
	/*
	 * If user has specified capability overrides, take care
	 * of that if the station we're setting up is the AP that
	 * we advertised a restricted capability set to. Override
	 * our own capabilities and then use those below.
	 */
	if (sdata->vif.type == NL80211_IFTYPE_STATION &&
	    !test_sta_flag(sta, WLAN_STA_TDLS_PEER))
		ieee80211_apply_vhtcap_overrides(sdata, &own_cap);
#endif
	/* take some capabilities as-is */
	cap_info = atbm_le32_to_cpu(vht_cap_ie->vht_cap_info);
	vht_cap->cap = cap_info;
	vht_cap->cap &= ATBM_IEEE80211_VHT_CAP_RXLDPC |
			ATBM_IEEE80211_VHT_CAP_VHT_TXOP_PS |
			ATBM_IEEE80211_VHT_CAP_HTC_VHT |
			ATBM_IEEE80211_VHT_CAP_MAX_A_MPDU_LENGTH_EXPONENT_MASK |
			ATBM_IEEE80211_VHT_CAP_VHT_LINK_ADAPTATION_VHT_UNSOL_MFB |
			ATBM_IEEE80211_VHT_CAP_VHT_LINK_ADAPTATION_VHT_MRQ_MFB |
			ATBM_IEEE80211_VHT_CAP_RX_ANTENNA_PATTERN |
			ATBM_IEEE80211_VHT_CAP_TX_ANTENNA_PATTERN;

	vht_cap->cap |= atbm_min(cap_info & ATBM_IEEE80211_VHT_CAP_MAX_MPDU_MASK,
			      own_cap.cap & ATBM_IEEE80211_VHT_CAP_MAX_MPDU_MASK);

	/* and some based on our own capabilities */
	switch (own_cap.cap & ATBM_IEEE80211_VHT_CAP_SUPP_CHAN_WIDTH_MASK) {
	case ATBM_IEEE80211_VHT_CAP_SUPP_CHAN_WIDTH_160MHZ:
		vht_cap->cap |= cap_info &
				ATBM_IEEE80211_VHT_CAP_SUPP_CHAN_WIDTH_160MHZ;
		break;
	case ATBM_IEEE80211_VHT_CAP_SUPP_CHAN_WIDTH_160_80PLUS80MHZ:
		vht_cap->cap |= cap_info &
				ATBM_IEEE80211_VHT_CAP_SUPP_CHAN_WIDTH_MASK;
		break;
	default:
		/* nothing */
		break;
	}

	/* symmetric capabilities */
	vht_cap->cap |= cap_info & own_cap.cap &
			(ATBM_IEEE80211_VHT_CAP_SHORT_GI_80 |
			ATBM_IEEE80211_VHT_CAP_SHORT_GI_160);

	/* remaining ones */
	if (own_cap.cap & ATBM_IEEE80211_VHT_CAP_SU_BEAMFORMEE_CAPABLE)
		vht_cap->cap |= cap_info &
				(ATBM_IEEE80211_VHT_CAP_SU_BEAMFORMER_CAPABLE |
				 ATBM_IEEE80211_VHT_CAP_SOUNDING_DIMENSIONS_MASK);

	if (own_cap.cap & ATBM_IEEE80211_VHT_CAP_SU_BEAMFORMER_CAPABLE)
		vht_cap->cap |= cap_info &
				(ATBM_IEEE80211_VHT_CAP_SU_BEAMFORMEE_CAPABLE |
				 ATBM_IEEE80211_VHT_CAP_BEAMFORMEE_STS_MASK);

	if (own_cap.cap & ATBM_IEEE80211_VHT_CAP_MU_BEAMFORMER_CAPABLE)
		vht_cap->cap |= cap_info &
				ATBM_IEEE80211_VHT_CAP_MU_BEAMFORMEE_CAPABLE;

	if (own_cap.cap & ATBM_IEEE80211_VHT_CAP_MU_BEAMFORMEE_CAPABLE)
		vht_cap->cap |= cap_info &
				ATBM_IEEE80211_VHT_CAP_MU_BEAMFORMER_CAPABLE;

	if (own_cap.cap & ATBM_IEEE80211_VHT_CAP_TXSTBC)
		vht_cap->cap |= cap_info & ATBM_IEEE80211_VHT_CAP_RXSTBC_MASK;

	if (own_cap.cap & ATBM_IEEE80211_VHT_CAP_RXSTBC_MASK)
		vht_cap->cap |= cap_info & ATBM_IEEE80211_VHT_CAP_TXSTBC;

	/* Copy peer MCS info, the driver might need them. */
	atbm_memcpy(&vht_cap->vht_mcs, &vht_cap_ie->supp_mcs,
	       sizeof(struct atbm_ieee80211_vht_mcs_info));
#if 0
	/* copy EXT_NSS_BW Support value or remove the capability */
	if (ieee80211_hw_check(&sdata->local->hw, SUPPORTS_VHT_EXT_NSS_BW))
		vht_cap->cap |= (cap_info & IEEE80211_VHT_CAP_EXT_NSS_BW_MASK);
	else
#endif
		vht_cap->vht_mcs.tx_highest &=
			~atbm_cpu_to_le16(ATBM_IEEE80211_VHT_EXT_NSS_BW_CAPABLE);

	/* but also restrict MCSes */
	for (i = 0; i < 8; i++) {
		atbm_uint16 own_rx, own_tx, peer_rx, peer_tx;

		own_rx = atbm_le16_to_cpu(own_cap.vht_mcs.rx_mcs_map);
		own_rx = (own_rx >> i * 2) & ATBM_IEEE80211_VHT_MCS_NOT_SUPPORTED;

		own_tx = atbm_le16_to_cpu(own_cap.vht_mcs.tx_mcs_map);
		own_tx = (own_tx >> i * 2) & ATBM_IEEE80211_VHT_MCS_NOT_SUPPORTED;

		peer_rx = atbm_le16_to_cpu(vht_cap->vht_mcs.rx_mcs_map);
		peer_rx = (peer_rx >> i * 2) & ATBM_IEEE80211_VHT_MCS_NOT_SUPPORTED;

		peer_tx = atbm_le16_to_cpu(vht_cap->vht_mcs.tx_mcs_map);
		peer_tx = (peer_tx >> i * 2) & ATBM_IEEE80211_VHT_MCS_NOT_SUPPORTED;

		if (peer_tx != ATBM_IEEE80211_VHT_MCS_NOT_SUPPORTED) {
			if (own_rx == ATBM_IEEE80211_VHT_MCS_NOT_SUPPORTED)
				peer_tx = ATBM_IEEE80211_VHT_MCS_NOT_SUPPORTED;
			else if (own_rx < peer_tx)
				peer_tx = own_rx;
		}

		if (peer_rx != ATBM_IEEE80211_VHT_MCS_NOT_SUPPORTED) {
			if (own_tx == ATBM_IEEE80211_VHT_MCS_NOT_SUPPORTED)
				peer_rx = ATBM_IEEE80211_VHT_MCS_NOT_SUPPORTED;
			else if (own_tx < peer_rx)
				peer_rx = own_tx;
		}

		vht_cap->vht_mcs.rx_mcs_map &=
			~atbm_cpu_to_le16(ATBM_IEEE80211_VHT_MCS_NOT_SUPPORTED << i * 2);
		vht_cap->vht_mcs.rx_mcs_map |= atbm_cpu_to_le16(peer_rx << i * 2);

		vht_cap->vht_mcs.tx_mcs_map &=
			~atbm_cpu_to_le16(ATBM_IEEE80211_VHT_MCS_NOT_SUPPORTED << i * 2);
		vht_cap->vht_mcs.tx_mcs_map |= atbm_cpu_to_le16(peer_tx << i * 2);
	}

	/*
	 * This is a workaround for VHT-enabled STAs which break the spec
	 * and have the VHT-MCS Rx map filled in with value 3 for all eight
	 * spacial streams, an example is AR9462.
	 *
	 * As per spec, in section 22.1.1 Introduction to the VHT PHY
	 * A VHT STA shall support at least single spactial stream VHT-MCSs
	 * 0 to 7 (transmit and receive) in all supported channel widths.
	 */
	if (vht_cap->vht_mcs.rx_mcs_map == atbm_cpu_to_le16(0xFFFF)) {
		vht_cap->vht_supported = ATBM_FALSE;
		wifi_printk(WIFI_DBG_ERROR, "Ignoring VHT IE from %pM due to invalid rx_mcs_map\n");
		return;
	}
}
#endif

#if CONFIG_HE
static void atbmwifi_ieee80211_he_mcs_disable(atbm_uint16 *he_mcs)
{
	atbm_uint32 i;

	for (i = 0; i < 8; i++)
		*he_mcs |= atbm_cpu_to_le16(ATBM_IEEE80211_HE_MCS_NOT_SUPPORTED << i * 2);
}

static void atbmwifi_ieee80211_he_mcs_intersection(atbm_uint16 *he_own_rx, atbm_uint16 *he_peer_rx,
					  atbm_uint16 *he_own_tx, atbm_uint16 *he_peer_tx)
{
	atbm_uint32 i;
	atbm_uint16 own_rx, own_tx, peer_rx, peer_tx;

	for (i = 0; i < 8; i++) {
		own_rx = atbm_le16_to_cpu(*he_own_rx);
		own_rx = (own_rx >> i * 2) & ATBM_IEEE80211_HE_MCS_NOT_SUPPORTED;

		own_tx = atbm_le16_to_cpu(*he_own_tx);
		own_tx = (own_tx >> i * 2) & ATBM_IEEE80211_HE_MCS_NOT_SUPPORTED;

		peer_rx = atbm_le16_to_cpu(*he_peer_rx);
		peer_rx = (peer_rx >> i * 2) & ATBM_IEEE80211_HE_MCS_NOT_SUPPORTED;

		peer_tx = atbm_le16_to_cpu(*he_peer_tx);
		peer_tx = (peer_tx >> i * 2) & ATBM_IEEE80211_HE_MCS_NOT_SUPPORTED;

		if (peer_tx != ATBM_IEEE80211_HE_MCS_NOT_SUPPORTED) {
			if (own_rx == ATBM_IEEE80211_HE_MCS_NOT_SUPPORTED)
				peer_tx = ATBM_IEEE80211_HE_MCS_NOT_SUPPORTED;
			else if (own_rx < peer_tx)
				peer_tx = own_rx;
		}

		if (peer_rx != ATBM_IEEE80211_HE_MCS_NOT_SUPPORTED) {
			if (own_tx == ATBM_IEEE80211_HE_MCS_NOT_SUPPORTED)
				peer_rx = ATBM_IEEE80211_HE_MCS_NOT_SUPPORTED;
			else if (own_tx < peer_rx)
				peer_rx = own_tx;
		}

		*he_peer_rx &=
			~atbm_cpu_to_le16(ATBM_IEEE80211_HE_MCS_NOT_SUPPORTED << i * 2);
		*he_peer_rx |= atbm_cpu_to_le16(peer_rx << i * 2);

		*he_peer_tx &=
			~atbm_cpu_to_le16(ATBM_IEEE80211_HE_MCS_NOT_SUPPORTED << i * 2);
		*he_peer_tx |= atbm_cpu_to_le16(peer_tx << i * 2);
	}
}


atbm_void
atbmwifi_ieee80211_he_cap_ie_to_sta_he_cap(struct atbmwifi_ieee80211_supported_band *sband,
				  const atbm_uint8 *he_cap_ie, atbm_uint8 he_cap_len,
				  struct atbm_ieee80211_sta_he_cap *he_cap)
{
	struct atbm_ieee80211_sta_he_cap own_he_cap;
	struct atbm_ieee80211_he_cap_elem *he_cap_ie_elem = (void *)he_cap_ie;
	atbm_uint8 he_ppe_size;
	atbm_uint8 mcs_nss_size;
	atbm_uint8 he_total_size;
	ATBM_BOOL own_160, peer_160, own_80p80, peer_80p80;

	atbm_memset(he_cap, 0, sizeof(*he_cap));

	if (!he_cap_ie)
		return;

	atbm_memcpy(&own_he_cap, &sband->iftype_data.he_cap, sizeof(own_he_cap));

	/* Make sure size is OK */
	mcs_nss_size = atbm_ieee80211_he_mcs_nss_size(he_cap_ie_elem);
	he_ppe_size =
		atbm_ieee80211_he_ppe_size(he_cap_ie[sizeof(he_cap->he_cap_elem) +
						mcs_nss_size],
				      he_cap_ie_elem->phy_cap_info);
	he_total_size = sizeof(he_cap->he_cap_elem) + mcs_nss_size +
			he_ppe_size;
	if (he_cap_len < he_total_size)
		return;

	atbm_memcpy(&he_cap->he_cap_elem, he_cap_ie, sizeof(he_cap->he_cap_elem));

	/* HE Tx/Rx HE MCS NSS Support Field */
	atbm_memcpy(&he_cap->he_mcs_nss_supp,
	       &he_cap_ie[sizeof(he_cap->he_cap_elem)], mcs_nss_size);

	/* Check if there are (optional) PPE Thresholds */
	if (he_cap->he_cap_elem.phy_cap_info[6] &
	    ATBM_IEEE80211_HE_PHY_CAP6_PPE_THRESHOLD_PRESENT)
		atbm_memcpy(he_cap->ppe_thres,
		       &he_cap_ie[sizeof(he_cap->he_cap_elem) + mcs_nss_size],
		       he_ppe_size);

	he_cap->has_he = ATBM_TRUE;

/*
    if (he_cap->he_cap_elem.phy_cap_info[0] & ATBM_IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_40MHZ_IN_2G)
        sta->cur_max_bandwidth =  ATBM_IEEE80211_STA_RX_BW_40;
    else
        sta->cur_max_bandwidth = ATBM_IEEE80211_STA_RX_BW_20;
*/

	atbmwifi_ieee80211_he_mcs_intersection(&own_he_cap.he_mcs_nss_supp.rx_mcs_80,
				      &he_cap->he_mcs_nss_supp.rx_mcs_80,
				      &own_he_cap.he_mcs_nss_supp.tx_mcs_80,
				      &he_cap->he_mcs_nss_supp.tx_mcs_80);

	own_160 = own_he_cap.he_cap_elem.phy_cap_info[0] &
		  ATBM_IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_160MHZ_IN_5G;
	peer_160 = he_cap->he_cap_elem.phy_cap_info[0] &
		   ATBM_IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_160MHZ_IN_5G;

	if (peer_160 && own_160) {
		atbmwifi_ieee80211_he_mcs_intersection(&own_he_cap.he_mcs_nss_supp.rx_mcs_160,
					      &he_cap->he_mcs_nss_supp.rx_mcs_160,
					      &own_he_cap.he_mcs_nss_supp.tx_mcs_160,
					      &he_cap->he_mcs_nss_supp.tx_mcs_160);
	} else if (peer_160 && !own_160) {
		atbmwifi_ieee80211_he_mcs_disable(&he_cap->he_mcs_nss_supp.rx_mcs_160);
		atbmwifi_ieee80211_he_mcs_disable(&he_cap->he_mcs_nss_supp.tx_mcs_160);
		he_cap->he_cap_elem.phy_cap_info[0] &=
			~ATBM_IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_160MHZ_IN_5G;
	}

	own_80p80 = own_he_cap.he_cap_elem.phy_cap_info[0] &
		    ATBM_IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_80PLUS80_MHZ_IN_5G;
	peer_80p80 = he_cap->he_cap_elem.phy_cap_info[0] &
		     ATBM_IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_80PLUS80_MHZ_IN_5G;

	if (peer_80p80 && own_80p80) {
		atbmwifi_ieee80211_he_mcs_intersection(&own_he_cap.he_mcs_nss_supp.rx_mcs_80p80,
					      &he_cap->he_mcs_nss_supp.rx_mcs_80p80,
					      &own_he_cap.he_mcs_nss_supp.tx_mcs_80p80,
					      &he_cap->he_mcs_nss_supp.tx_mcs_80p80);
	} else if (peer_80p80 && !own_80p80) {
		atbmwifi_ieee80211_he_mcs_disable(&he_cap->he_mcs_nss_supp.rx_mcs_80p80);
		atbmwifi_ieee80211_he_mcs_disable(&he_cap->he_mcs_nss_supp.tx_mcs_80p80);
		he_cap->he_cap_elem.phy_cap_info[0] &=
			~ATBM_IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_80PLUS80_MHZ_IN_5G;
	}
}

atbm_void
ieee80211_he_op_ie_to_bss_conf(struct atbmwifi_vif *priv,
			const struct atbm_ieee80211_he_operation *he_op_ie)
{
	atbm_memset(&priv->bss.he_oper, 0, sizeof(priv->bss.he_oper));
	if (!he_op_ie)
		return;

	priv->bss.he_oper.params = atbm_le32_to_cpu(he_op_ie->he_oper_params);
	priv->bss.he_oper.nss_set = atbm_le32_to_cpu(he_op_ie->he_mcs_nss_set);
}

atbm_void
ieee80211_he_spr_ie_to_bss_conf(struct atbmwifi_vif *priv,
				const struct atbm_ieee80211_he_spr *he_spr_ie_elem)
{
	struct atbm_ieee80211_he_obss_pd *he_obss_pd =
					&priv->bss.he_obss_pd;
	const atbm_uint8 *data;

	atbm_memset(he_obss_pd, 0, sizeof(*he_obss_pd));

	if (!he_spr_ie_elem)
		return;
	data = he_spr_ie_elem->optional;

	if (he_spr_ie_elem->he_sr_control &
	    ATBM_IEEE80211_HE_SPR_NON_SRG_OFFSET_PRESENT)
		data++;
	if (he_spr_ie_elem->he_sr_control &
	    ATBM_IEEE80211_HE_SPR_SRG_INFORMATION_PRESENT) {
		he_obss_pd->max_offset = *data++;
		he_obss_pd->min_offset = *data++;
		he_obss_pd->enable = ATBM_TRUE;
	}
}
#endif

atbm_void atbmwifi_ieee80211_parse_qos(struct atbmwifi_vif *priv,struct atbm_buff *skb)
{
	struct atbmwifi_ieee80211_hdr *hdr = (struct atbmwifi_ieee80211_hdr *)ATBM_OS_SKB_DATA(skb);
	struct atbmwifi_ieee80211_rx_status *status = ATBM_IEEE80211_SKB_RXCB(skb);
	atbm_uint8 tid, seqno_idx;

	/* does the frame have a qos control field? */
	if (atbmwifi_ieee80211_is_data_qos(hdr->frame_control)) {
		atbm_uint8 *qc = atbmwifi_ieee80211_get_qos_ctl(hdr);
		/* frame has qos control */
		tid = *qc & ATBM_IEEE80211_QOS_CTL_TID_MASK;
		if (*qc & ATBM_IEEE80211_QOS_CTL_A_MSDU_PRESENT)
			status->flag |= ATBM_RX_FLAG_AMSDU;

		seqno_idx = tid;
		//security_idx = tid;
	} else {
		/*
		 * IEEE 802.11-2007, 7.1.3.4.1 ("Sequence Number field"):
		 *
		 *	Sequence numbers for management frames, QoS data
		 *	frames with a broadcast/multicast address in the
		 *	Address 1 field, and all non-QoS data frames sent
		 *	by QoS STAs are assigned using an additional single
		 *	modulo-4096 counter, [...]
		 *
		 * We also use that counter for non-QoS STAs.
		 */
		seqno_idx = ATBM_NUM_RX_DATA_QUEUES;
		//security_idx = 0;
		//if (atbmwifi_ieee80211_is_mgmt(hdr->frame_control))
		//	security_idx = ATBM_NUM_RX_DATA_QUEUES;
		tid = 0;
	}

	status->seqno_idx = seqno_idx;
	//status->security_idx = security_idx;
	/* Set skb->priority to 1d tag if highest order bit of TID is not set.
	 * For now, set skb->priority to 0 for other cases. */
	//status->priority = (tid > 7) ? 0 : tid;
}
int atbm_ieee80211_handle_bss_capability(struct atbmwifi_vif *priv,
					   atbm_uint16 capab, int erp_valid, atbm_uint8 erp)
{
	struct atbmwifi_cfg80211_bss * bss_conf = &priv->bss;
	atbm_uint32 changed = 0;
	int use_protection;
	int short_preamble;
	int use_short_slot;

	if (erp_valid) {
		use_protection = (erp & ATBM_WLAN_ERP_USE_PROTECTION) != 0;
		short_preamble = (erp & ATBM_WLAN_ERP_BARKER_PREAMBLE) == 0;
	} else {
		use_protection = ATBM_FALSE;
		short_preamble = !!(capab & ATBM_WLAN_CAPABILITY_SHORT_PREAMBLE);
	}
	use_short_slot = !!(capab & ATBM_WLAN_CAPABILITY_SHORT_SLOT_TIME);

	if (use_protection != bss_conf->use_cts_prot) {
		bss_conf->use_cts_prot = use_protection;
		changed |= ATBM_BSS_CHANGED_ERP_CTS_PROT;
	}

	if (short_preamble != bss_conf->sta_priv.short_preamble) {
		bss_conf->sta_priv.short_preamble = short_preamble;
		changed |= ATBM_BSS_CHANGED_ERP_PREAMBLE;
	}

	if (use_short_slot != bss_conf->use_short_slot) {
		bss_conf->use_short_slot = use_short_slot;
		changed |= ATBM_BSS_CHANGED_ERP_SLOT;
	}

	return changed;
}

static int atbmwifi_set_channel_type(struct atbmwifi_vif *priv)
{
	int ret;
	struct atbmwifi_common *hw_priv = _atbmwifi_vifpriv_to_hwpriv(priv);
	struct wsm_set_chantype set_channtype;
	set_channtype.band=WSM_PHY_BAND_2_4G;
	set_channtype.flag = 0;
	set_channtype.channelNumber = priv->bss.channel_num;
	set_channtype.channelType = priv->bss.channel_type;
	ret=wsm_set_chantype_func(hw_priv,&set_channtype,priv->if_id);
	if(ret){
		wifi_printk(WIFI_CONNECT,"atbmwifi_set_channel_type error\n");
	}
	return ret;
}
 void atbmwifi_set_channel_work(struct atbm_work_struct *work)
{
	struct atbmwifi_vif *priv=(struct atbmwifi_vif *)work;
	atbmwifi_set_channel_type(priv);
}

 atbm_void atbmwifi_sw_chntimeout(atbm_void *data1,atbm_void *data2)
{
	struct atbmwifi_vif *priv=(struct atbmwifi_vif*)data1;
	atbmwifi_set_channel_type(priv);
}
static int atbmwifi_ieee80211_stop_tx_queues(struct atbmwifi_vif *priv)
{
	/*Stop tx queue until set channtype success*/
	tcp_opt->net_stop_queue(priv->ndev,1);
	return 0;
}

static int atbmwifi_ieee80211_start_tx_queues(struct atbmwifi_vif *priv)
{
	/*Start tx queue after set channtype success*/
	tcp_opt->net_start_queue(priv->ndev,1);
	return 0;
}

/*
 * ieee80211_enable_ht should be called only after the operating band
 * has been determined as ht configuration depends on the hw's
 * HT abilities for a specific band.
 */
int atbmwifi_ieee80211_enable_ht(struct atbmwifi_ieee80211_supported_band *sband,
			       struct atbmwifi_ieee80211_ht_info *hti,
			       struct atbmwifi_vif *priv, atbm_uint16 ap_ht_cap_flags,
			       int beacon_htcap_ie)
{
	struct atbmwifi_common *hw_priv = _atbmwifi_vifpriv_to_hwpriv(priv);
	atbm_uint8 ht_opmode;
	int enable_ht = 1;
	atbm_uint32 changed = 0;
	enum atbm_nl80211_channel_type prev_chantype;
	enum atbm_nl80211_channel_type channel_type = ATBM_NL80211_CHAN_NO_HT;

	prev_chantype = (enum atbm_nl80211_channel_type)priv->bss.channel_type;

	/* HT is not supported */
	if (!sband->ht_cap.ht_supported)
		enable_ht = ATBM_FALSE;
	//wifi_printk(WIFI_CONNECT,"hti->ht_param=%x,enable_ht=%d\n",hti->ht_param,enable_ht);
	if (enable_ht) {
		channel_type = ATBM_NL80211_CHAN_HT20;		
		if (!(ap_ht_cap_flags & ATBM_IEEE80211_HT_CAP_40MHZ_INTOLERANT) &&
		    (sband->ht_cap.cap & ATBM_IEEE80211_HT_CAP_SUP_WIDTH_20_40) &&
		    (hti->ht_param & ATBM_IEEE80211_HT_PARAM_CHAN_WIDTH_ANY)) {
			switch(hti->ht_param & ATBM_IEEE80211_HT_PARAM_CHA_SEC_OFFSET) {
			case ATBM_IEEE80211_HT_PARAM_CHA_SEC_ABOVE:
					channel_type = ATBM_NL80211_CHAN_HT40PLUS;
				break;
			case ATBM_IEEE80211_HT_PARAM_CHA_SEC_BELOW:
					channel_type = ATBM_NL80211_CHAN_HT40MINUS;
				break;
			}
		}
		/*TxRate with shortGI*/
		if (ap_ht_cap_flags & (ATBM_IEEE80211_HT_CAP_SGI_20|ATBM_IEEE80211_HT_CAP_SGI_40)){
			priv->bss.short_gi = 1;
		}
	}	
	if(hw_priv->channel_type >=ATBM_NL80211_CHAN_HT40MINUS){			
		priv->bss.channel_type = channel_type;
	}
	else {
		priv->bss.channel_type = hw_priv->channel_type;
	}

	if(priv->iftype == ATBM_NL80211_IFTYPE_STATION){		
		if (prev_chantype != channel_type) {
			priv->bss.channel_type = channel_type;
			ht_opmode = atbm_le16_to_cpu(hti->operation_mode);
			priv->ht_operation_mode = ht_opmode;
			changed |= ATBM_BSS_CHANGED_HT;
			atbm_queue_work(hw_priv,priv->chantype_switch_work);	
		}
		//wifi_printk(WIFI_CONNECT,"priv->bss.channel_type=%d\n",priv->bss.channel_type);
		if (priv->bss.channel_type>=ATBM_NL80211_CHAN_HT40MINUS){
			priv->bss.ht_40M = 1;
		}else{
			priv->bss.ht_40M = 0;
		}
	}
	
	return changed;
}

atbm_void atbmwifi_ieee80211_bss_info_change_notify(struct atbmwifi_vif *priv,
				      atbm_uint32 changed)
{
	//struct atbmwifi_common *hw_priv = _atbmwifi_vifpriv_to_hwpriv(priv);
	//TODO
	
}
int atbmwifi_ieee80211_frequency_to_channel(int freq)
{
	/* see 802.11 17.3.8.3.2 and Annex J */
	if (freq == 2484){
		return 14;
	}else if (freq < 2484){
		return (freq - 2407) / 5;
	}else if (freq >= 4910 && freq <= 4980){
		return (freq - 4000) / 5;
	}else{
		return (freq - 5000) / 5;
	}
}
int atbmwifi_ieee80211_channel_to_frequency(int chan, enum atbmwifi_ieee80211_band band)
{
	/* see 802.11 17.3.8.3.2 and Annex J
	 * there are overlapping channel numbers in 5GHz and 2GHz bands */
	if (band == ATBM_IEEE80211_BAND_5GHZ) {
		if (chan >= 182 && chan <= 196){
			return 4000 + chan * 5;
		}else{
			return 5000 + chan * 5;
		}
	} else { /* ATBM_IEEE80211_BAND_2GHZ */
		if (chan == 14){
			return 2484;
		}else if (chan < 14){
			return 2407 + chan * 5;
		}else{
			return 0; /* not supported */
		}
	}
}
struct atbmwifi_ieee80211_channel * atbmwifi_ieee80211_get_channel(struct atbmwifi_common *hw_priv,
					int new_freq){
	struct atbmwifi_ieee80211_supported_band *sband;
	
	enum atbmwifi_ieee80211_band band;
	int i;
	for (band = ATBM_IEEE80211_BAND_2GHZ; band < ATBM_IEEE80211_NUM_BANDS; band++) {
		sband = hw_priv->bands[band];
		if (!sband){
			continue;
		}
		for (i = 0; i < sband->n_channels; i++) {
			if (sband->channels[i].center_freq == new_freq)
				return &sband->channels[i];
		}
	}
	return ATBM_NULL;

}
static int atbmwifi_ieee802_11_parse_vendor_specific(atbm_uint8 *pos, atbm_size_t elen,
					    struct atbmwifi_ieee802_11_elems *elems,
					    int show_errors)
{
	atbm_uint32 oui;

	/* first 3 bytes in vendor specific information element are the IEEE
	 * OUI of the vendor. The following byte is used a vendor specific
	 * sub-type. */
	if (elen < 4) {
		if (show_errors) {
			wifi_printk(WIFI_DBG_MSG,"short vendor specific "
				   "information element ignored (len=%lu)",
				   (unsigned long) elen);
		}
		return -1;
	}

	oui = ATBM_WPA_GET_BE24(pos);
	switch (oui) {
	case ATBM_OUI_MICROSOFT:
		/* Microsoft/Wi-Fi information elements are further typed and
		 * subtyped */
		switch (pos[3]) {
		case 1:
			/* Microsoft OUI (00:50:F2) with OUI Type 1:
			 * real WPA information element */
			elems->wpa = pos;
			elems->wpa_len = elen;
			break;
		case ATBM_WMM_OUI_TYPE:
			/* WMM information element */
			if (elen < 5) {
				wifi_printk(WIFI_DBG_MSG,"short WMM "
					   "information element ignored "
					   "(len=%lu)",
					   (unsigned long) elen);
				return -1;
			}
			switch (pos[4]) {
			case ATBM_WMM_OUI_SUBTYPE_INFORMATION_ELEMENT:
				elems->wmm_info = pos;
				elems->wmm_info_len = elen;
				break;
			case ATBM_WMM_OUI_SUBTYPE_PARAMETER_ELEMENT:
				elems->wmm_param = pos;
				elems->wmm_param_len = elen;
				break;
			default:
				wifi_printk(WIFI_DBG_MSG,"unknown WMM "
					   "information element ignored "
					   "(subtype=%d len=%lu)",
					   pos[4], (unsigned long) elen);
				return -1;
			}
			break;
#if CONFIG_WPS
		case 4:
			/* Wi-Fi Protected Setup (WPS) IE */
			elems->wps_ie = pos;
			elems->wps_ie_len = elen;
			break;
#endif
		default:
			wifi_printk(WIFI_DBG_MSG,"Unknown Microsoft "
				   "information element ignored "
				   "(type=%d len=%lu)",
				   pos[3], (unsigned long) elen);
			return -1;
		}
		break;

	case ATBM_OUI_WFA:
		switch (pos[3]) {
#if CONFIG_P2P
		case ATBM_P2P_OUI_TYPE:
		case ATBM_WFD_OUI_TYPE:
			/* Wi-Fi Alliance - P2P IE */
			if(!elems->p2p_ie){
				elems->p2p_ie = pos;
				elems->p2p_ie_len = elen;
			}
			break;
#endif
		default:
			wifi_printk(WIFI_DBG_MSG,"Unknown WFA "
				   "information element ignored "
				   "(type=%d len=%lu)\n",
				   pos[3], (unsigned long) elen);
			return -1;
		}
#if CONFIG_P2P
	case ATBM_OUI_QCA:
		if(pos[3] == QCA_VENDOR_ELEM_P2P_PREF_CHAN_LIST){
			elems->pref_freq_list = pos;
			elems->pref_freq_list_len = elen;
		}
		break;
#endif
	default:
		/*
		printk(KERN_DEBUG "unknown vendor specific "
			   "information element ignored (vendor OUI "
			   "%02x:%02x:%02x len=%lu)",
			   pos[0], pos[1], pos[2], (unsigned long) elen);*/
		return -1;
	}

	return 0;
}

static void ieee80211_parse_extension_element(atbm_uint8 *elem_data, atbm_uint8 elem_data_len,
						   struct atbmwifi_ieee802_11_elems *elems)
{
	 const void *data = elem_data + 1;
	 atbm_uint8 len = elem_data_len - 1;
	 switch (elem_data[0]) {
	 case ATBM_WLAN_EID_EXT_HE_MU_EDCA_PARAMS:
		 if (len >= sizeof(*elems->mu_edca_param_set)) {
			 elems->mu_edca_param_set = data;
		 }
		 break;
	 case ATBM_WLAN_EID_EXT_HE_CAPABILITIES:
		 elems->he_cap = data;
		 elems->he_cap_len = len;
		 break;
	 case ATBM_WLAN_EID_EXT_HE_OPERATION:
		 if (len >= offsetof(struct atbm_ieee80211_he_operation, optional) &&
			 len >= atbm_ieee80211_he_oper_size(data) - 1) {
			 //if (crc)
			 //  *crc = crc32_be(*crc, (void *)elem,
			 // 		 elem->datalen + 2);
			 elems->he_operation = data;
		 }
		 break;
	 case ATBM_WLAN_EID_EXT_UORA:
		 if (len >= 1)
			 elems->uora_element = data;
		 break;
	 case ATBM_WLAN_EID_EXT_MAX_CHANNEL_SWITCH_TIME:
		 if (len == 3)
			 elems->max_channel_switch_time = data;
		 break;
	 case ATBM_WLAN_EID_EXT_MULTIPLE_BSSID_CONFIGURATION:
		 if (len >= sizeof(*elems->mbssid_config_ie))
			 elems->mbssid_config_ie = data;
		 break;
	 case ATBM_WLAN_EID_EXT_SPATIAL_REUSE:
		 if (len >= offsetof(struct atbm_ieee80211_he_spr, optional) &&
			 len >= atbm_ieee80211_he_spr_size(data))
			 elems->he_spr = data;
		 break;
	 case ATBM_WLAN_EID_EXT_HE_6GHZ_CAPA:
		 //if (len >= sizeof(*elems->he_6ghz_capa))
		 //  elems->he_6ghz_capa = data;
		 break;
	 }
}


atbm_void atbm_ieee802_11_parse_elems(atbm_uint8 *start, int len,
			       struct atbmwifi_ieee802_11_elems *elems)
{
	atbm_size_t left = len;
	atbm_uint8 *pos = start;

	atbm_memset(elems, 0, sizeof(struct atbmwifi_ieee802_11_elems ));
	while (left >= 2) {
		atbm_uint8 id, elen;

		id = *pos++;
		elen = *pos++;
		left -= 2;

		if (elen > left)
			break;

		switch (id) {
		case ATBM_WLAN_EID_SSID:
			elems->ssid = pos;
			elems->ssid_len = elen;
			if(elems->ssid_len > 32)
				elems->ssid_len =32;
			break;
		case ATBM_WLAN_EID_SUPP_RATES:
			elems->supp_rates = pos;
			elems->supp_rates_len = elen;
			break;
		case ATBM_WLAN_EID_DS_PARAMS:
			elems->ds_params = pos;
			elems->ds_params_len = elen;
			break;
		case ATBM_WLAN_EID_TIM:
			if (elen >= sizeof(struct atbmwifi_ieee80211_tim_ie)) {
				elems->tim = (atbm_void *)pos;
				elems->tim_len = elen;
			}
			break;
		case ATBM_WLAN_EID_VENDOR_SPECIFIC:
			atbmwifi_ieee802_11_parse_vendor_specific(pos,elen,elems,1);
			break;
		case ATBM_WLAN_EID_RSN:
			elems->rsn = pos;
			elems->rsn_len = elen;
			break;
		case ATBM_WLAN_EID_ERP_INFO:
			elems->erp_info = pos;
			elems->erp_info_len = elen;
			break;
		case ATBM_WLAN_EID_EXT_SUPP_RATES:
			elems->ext_supp_rates = pos;
			elems->ext_supp_rates_len = elen;
			break;
		case ATBM_WLAN_EID_HT_CAPABILITY:
			if (elen >= sizeof(struct atbmwifi_ieee80211_ht_cap))
				elems->ht_cap_elem = (atbm_void *)pos;
			break;
		case ATBM_WLAN_EID_HT_OPERATION:
			if (elen >= sizeof(struct atbmwifi_ieee80211_ht_info))
				elems->ht_info_elem = (atbm_void *)pos;
			break;
		case ATBM_WLAN_EID_CHANNEL_SWITCH:
			elems->ch_switch_elem = pos;
			elems->ch_switch_elem_len = elen;
			break;
		case ATBM_WLAN_EID_EXT_CHANSWITCH_ANN:
			elems->extended_ch_switch_elem = pos;
			elems->extended_ch_switch_elem_len = elen;
			break;
		case ATBM_WLAN_EID_SECONDARY_CH_OFFSET:
			elems->secondary_ch_elem=pos;
			elems->secondary_ch_elem_len=elen;
			break;
		case ATBM_WLAN_EID_VHT_CAP:
			elems->vht_cap_elem = (struct atbm_ieee80211_vht_cap *)pos;
			break;
		case ATBM_WLAN_EID_VHT_OPERATION:
			elems->vht_operation = (struct atbm_ieee80211_vht_operation *)pos;
			break;
		case ATBM_WLAN_EID_EXTENSION:
			ieee80211_parse_extension_element(pos, elen, elems);
			break;
		default:
			break;
		}
		left -= elen;
		pos += elen;
	}

	return ;
}

#if FAST_CONNECT_NO_SCAN
 int atbm_wifi_reserve_key_ie(atbm_uint8 *buf, int buf_len, atbm_uint8 *ie, int ie_len){
	atbm_size_t left = ie_len;
	atbm_uint8 *pos = ie;
	atbm_size_t len = 0;

	while (left >= 2) {
		atbm_uint8 id, elen;
		id = *pos++;
		elen = *pos++;
		left -= 2;

		if (elen > left)
			break;
		switch (id) {
			case ATBM_WLAN_EID_SSID:
			case ATBM_WLAN_EID_SUPP_RATES:
			case ATBM_WLAN_EID_DS_PARAMS:
			case ATBM_WLAN_EID_EXT_SUPP_RATES:
			case ATBM_WLAN_EID_HT_CAPABILITY:
			case ATBM_WLAN_EID_HT_OPERATION:
			case ATBM_WLAN_EID_RSN:
				if(len + 2 + elen <= buf_len){
					atbm_memcpy(&buf[len], pos - 2, elen + 2);
					len = len + 2 + elen;
				}else{
					return -1;
				}
				break;
			case ATBM_WLAN_EID_VENDOR_SPECIFIC:
				if (elen >= 4 && pos[0] == 0x00 && pos[1] == 0x50 &&
			    	pos[2] == 0xf2){
					if(pos[3] == 1 || (pos[3] == 2 && elen > 5)){
						if(len + 2 + elen <= buf_len){
							atbm_memcpy(&buf[len], pos - 2, elen + 2);
							len = len + 2 + elen;
						}else{
							return -1;
						}
					}
				}
				break;
			default:
				break;
		}
		left -= elen;
		pos += elen;
	}
	return len;
}
#endif

/* spectrum management related things */
#if CONFIG_ATBM_SUPPORT_CSA
#if CONFIG_ATBM_SUPPORT_CHANSWITCH_TEST
unsigned char *ieee80211_sta_swbeacon_add(struct atbmwifi_vif *priv,struct atbm_buff *skb)
{
	if(priv->sw_ie.count && (atbm_skb_headroom(skb) >= 5)){				
		u8 *data = skb->abuf;
		size_t len = skb->dlen;
		skb->abuf -= 5;
		atbm_memmove(skb->abuf, data, len);
		atbm_skb_set_tail_pointer(skb, len);
		/*
		*set sw ie
		*/
		data = atbm_skb_put(skb,5);
		data[0] = ATBM_WLAN_EID_CHANNEL_SWITCH;
		data[1] = 3;
		data[2] = priv->sw_ie.mode;
		data[3] = priv->sw_ie.new_ch_num;
		data[4] = priv->sw_ie.count -- ;

		wifi_printk(WIFI_ERROR, "%s:%d\n",__func__,skb->totalLen);
	}

	return skb->abuf;
}
#endif /* CONFIG_ATBM_SUPPORT_CHANSWITCH_TEST */

int atbm_do_csa_concurrent(struct atbmwifi_common *hw_priv, struct list_head *head)
{
	struct wsm_csa_concurrent concurrent;
	struct ieee80211_csa_work *work, *tmp;
	struct wsm_csa_concurrent_vif *vif;
	
	wifi_printk(WIFI_DBG_MSG, "csa concurrent");

	if(atbm_bh_is_term(hw_priv)){
		return -ENOENT;
	}

	memset(&concurrent,0,sizeof(struct wsm_csa_concurrent));
	concurrent.vifs[0].if_id = 0xff;
	concurrent.vifs[1].if_id = 0xff;
	
	vif = &concurrent.vifs[0];
	
	atbm_list_for_each_entry_safe(work, tmp, head, list, struct ieee80211_csa_work){
		struct atbmwifi_vif *priv = work->vif;

		if(!priv->enabled){
			continue;
		}

		vif->if_id    = priv->if_id;
		vif->channel  = work->chan;
		vif->chantype = work->chan_type;

		if(vif == &concurrent.vifs[1]){
			break;
		}

		vif++;
	}

	if(concurrent.vifs[0].if_id == 0xff){
		return -ENOENT;
	}

	return 	wsm_csa_concurrent_req(hw_priv,&concurrent,0);
}

static bool atbm_ieee80211_channel_fixed(struct atbmwifi_common *hw_priv)
{
	int i, count = 0;
	struct atbmwifi_vif *priv;

	for (i = 0; i < ATBM_WIFI_MAX_VIFS; i++) {
		priv = hw_priv->vif_list[i];
		if (!priv->enabled) {
			continue;
		}

		if ((priv->iftype == ATBM_NL80211_IFTYPE_AP &&
		    (priv->join_status != ATBMWIFI__JOIN_STATUS_AP)) || 
			(priv->iftype == ATBM_NL80211_IFTYPE_STATION &&
				(priv->join_status != ATBMWIFI__JOIN_STATUS_STA))) {
			count++;
		}
	}

	return count == ATBM_WIFI_MAX_VIFS ? true : false;
}

static int atbm_ieee80211_csa_running(struct atbmwifi_common *hw_priv)
{
	return !atbm_list_empty(&hw_priv->csa_pending) || !atbm_list_empty(&hw_priv->csa_req);
}

static struct ieee80211_csa_work *ieee880211_chan_csa_work_alloc(
	struct atbmwifi_vif *priv, struct atbm_ieee80211_csa_request *csa)
{
	struct atbmwifi_common *hw_priv = _atbmwifi_vifpriv_to_hwpriv(priv);
	struct ieee80211_csa_work *work;

	work = atbm_kzalloc(sizeof(struct ieee80211_csa_work),GFP_KERNEL);

	if(work){
		work->vif   = priv;
		work->mactime = csa->mactime;
		work->block   = csa->ie.mode;
		work->count   = csa->ie.count;
		work->chan    = csa->chan;
		work->chan_type  = csa->chantype;
		work->cfg80211   = csa->cfg80211;
		priv->csa_state = ATBM_IEEE80211_CSA_MLME_STATE_IDLE;
	}

	return work;
}

bool atbm_ieee80211_chan_ct_coexists(enum atbm_nl80211_channel_type wk_ct,
				       enum atbm_nl80211_channel_type oper_ct)
{
	switch (wk_ct) {
	case ATBM_NL80211_CHAN_NO_HT:
	case ATBM_NL80211_CHAN_HT20:
		return true;
	case ATBM_NL80211_CHAN_HT40MINUS:
	case ATBM_NL80211_CHAN_HT40PLUS:
		if((oper_ct == ATBM_NL80211_CHAN_NO_HT) || (oper_ct == ATBM_NL80211_CHAN_HT20))
			return true;
		return (wk_ct == oper_ct);
	}
	ATBM_WARN_ON(1); /* shouldn't get here */
	return false;
}

struct ieee80211_csa_work *ieee880211_chan_csa_ap_requst(
	struct atbmwifi_vif *priv,struct atbm_ieee80211_csa_request *csa)
{
	struct ieee80211_csa_work *work = NULL;
	struct atbm_ieee80211_csa_mlme *csa_mlme;

	if(priv->iftype != ATBM_NL80211_IFTYPE_AP){
		goto err;
	}

	work = ieee880211_chan_csa_work_alloc(priv, csa);

	if(work == NULL){
		goto err;
	}

	//ieee80211_ap_flush_csa(sdata);
	
	csa_mlme = &priv->csa_mlme;

	csa_mlme->block_tx = csa->ie.mode;
	csa_mlme->count    = csa->ie.count;
	csa_mlme->chan     = csa->chan;
	priv->csa_state = ATBM_IEEE80211_CSA_MLME_STATE_WK;

	return work;
err:
	return NULL;
}

static void ieee80211_chan_csa_concurrent_work_alloc(struct atbmwifi_vif *priv,
			struct atbm_ieee80211_csa_request *csa,struct list_head *works)
{
	/*
	*only support ap + sta/ ap + ap csa , so find ap mode
	*/
	struct atbmwifi_common *hw_priv = _atbmwifi_vifpriv_to_hwpriv(priv);
	struct atbmwifi_vif *concurrent;
	struct atbmwifi_vif *ap;
	struct atbm_ieee80211_csa_request ap_csa;
	struct ieee80211_csa_work *work;
	struct ieee80211_csa_work *work_depend;
	int i;

	for (i = 0; i < ATBM_WIFI_MAX_VIFS; i++) {
		concurrent = hw_priv->vif_list[i];
		if (concurrent == priv)
			continue;

		if (!concurrent->enabled) {
			continue;
		}

		if (concurrent->iftype != ATBM_NL80211_IFTYPE_AP){
			continue;
		}

		if (concurrent->iftype == ATBM_NL80211_IFTYPE_AP &&
		    (concurrent->join_status != ATBMWIFI__JOIN_STATUS_AP))
			continue;

		if(atbm_ieee80211_chan_ct_coexists(csa->chantype,concurrent->bss.channel_type) == false){
			wifi_printk(WIFI_DBG_ERROR, "chantype[%d][%d] err\n",csa->chantype,concurrent->bss.channel_type);
			break;
		}

		ap = concurrent;
		break;
	}
	
	if(ap == NULL){
		goto work_err;
	}

	memset(&ap_csa,0,sizeof(ap_csa));

	ap_csa.chan     = csa->chan;
	ap_csa.chantype = ap->bss.channel_type;
	ap_csa.mactime  = csa->mactime;
	ap_csa.ie.mode  = csa->ie.mode;
	ap_csa.ie.count = csa->ie.count;
	ap_csa.ie.new_ch_num = csa->ie.new_ch_num;

	if(ap_csa.ie.count <= 5){
		ap_csa.ie.count = 5;
	}

	work = ieee880211_chan_csa_ap_requst(ap,&ap_csa);

	if(work == NULL){
		goto work_err;
	}

	work_depend = ieee880211_chan_csa_work_alloc(priv,csa);

	if(work_depend == NULL){
		goto work_depend_err;
	}

	atbm_list_add_tail(&work->list,works);
	atbm_list_add_tail(&work_depend->list,works);
	return;
work_depend_err:
	//ieee80211_ap_flush_csa(ap);
	atbm_kfree(work);
work_err:
	return;
}

int atbm_ieee80211_csa_busy(struct atbmwifi_common *hw_priv)
{
	return !atbm_list_empty(&hw_priv->csa_req);
}

static void atbm_ieee80211_chan_prepare_csa_work(struct atbmwifi_vif *priv, struct atbm_ieee80211_csa_request *csa,struct list_head *works)
{
	int i;
	struct atbmwifi_common *hw_priv = _atbmwifi_vifpriv_to_hwpriv(priv);
	struct atbmwifi_vif *tmp_priv;

	for (i = 0; i < ATBM_WIFI_MAX_VIFS; i++) {
		tmp_priv = hw_priv->vif_list[i];
		if (tmp_priv == priv) {
			continue;
		}

		if (!priv->enabled) {
			continue;
		}

		if ((tmp_priv->iftype == ATBM_NL80211_IFTYPE_AP &&
		    (tmp_priv->join_status == ATBMWIFI__JOIN_STATUS_AP)) || 
			(tmp_priv->iftype == ATBM_NL80211_IFTYPE_STATION &&
				(tmp_priv->join_status == ATBMWIFI__JOIN_STATUS_STA))) {
			ieee80211_chan_csa_concurrent_work_alloc(priv, csa, works);
			return;
		}
	}

	struct ieee80211_csa_work *work = NULL;
	work = ieee880211_chan_csa_work_alloc(priv, csa);
	if(work){
		wifi_printk(WIFI_DBG_MSG, "start csa\n");
		atbm_list_add_tail(&work->list,works);
	}else {
		wifi_printk(WIFI_DBG_ERROR, "alloc work err");
	}
}

static void __atbm_ieee80211_start_csa_work(struct atbmwifi_common *hw_priv)
{
	struct ieee80211_csa_work *csawork, *tmp;

	atbm_list_for_each_entry_safe(csawork, tmp, &hw_priv->csa_req, list, struct ieee80211_csa_work){
		csawork->vif->csa_state = ATBM_IEEE80211_CSA_MLME_STATE_WK;
	}

	hw_priv->next_csa_state = CSA_WAIT_START;
	atbm_queue_work(hw_priv, hw_priv->csa_work);
}

static void __atbm_ieee80211_scan_cancel(struct atbmwifi_common *hw_priv)
{
	struct atbmwifi_vif *priv = NULL;
	struct wpa_supplicant *wpa_s = NULL;
	int i;

	for (i = 0; i < ATBM_WIFI_MAX_VIFS; i++) {
		priv = hw_priv->vif_list[i];
		wpa_s = priv->appdata;

		if (!priv->enabled) {
			wifi_printk(WIFI_DBG_MSG, "[%s] not enable now\n", priv->if_name);
			return;
		}
	
		/* cancel scan in csa work */
		if (!priv->scan.in_progress && !priv->scan.ApScan_in_process) {
			wifi_printk(WIFI_DBG_MSG, "[%s] not scan now\n", priv->if_name);
			continue;
		}

		ATBM_WARN_ON(wsm_stop_scan(hw_priv,priv->scan.if_id ? 1 : 0));
		/* set priv to scan complete state */
		priv->scan_no_connect_back = priv->scan_no_connect;
		priv->scan_no_connect == 1;
		priv->scan.status = ATBMWIFI_SCAN_CONNECT_AP_SUCCESS;
		atbmwifi_scan_comlete(priv);
		priv->scan.in_progress = 1;
		priv->scan.ApScan_in_process = 1;
	
		if (atbmwifi_is_ap_mode(priv->iftype)) {
			atbm_cancel_work(hw_priv, priv->scan.ap_scan_work);
		}
	
		if (atbmwifi_is_sta_mode(priv->iftype)) {
#if CONFIG_WPS
			if((wpa_s != ATBM_NULL) && (wpa_s->wps_mode != WPS_MODE_UNKNOWN))
				atbmwifi_eloop_cancel_timeout(wpas_wps_timeout, wpa_s, NULL);
#endif
			atbmwifi_eloop_cancel_timeout(sta_scan_start_timer_func, (atbm_void *)priv, ATBM_NULL);
			atbm_cancel_work(hw_priv, priv->scan.scan_work);
			atbm_cancel_work(hw_priv, priv->join_work);
		}
	}
}

static void atbm_ieee880211_start_pending_scan(struct atbmwifi_common *hw_priv)
{
	struct atbmwifi_vif *priv = NULL;
	struct atbmwifi_work_struct *work;
	int i;

	for (i = 0; i < ATBM_WIFI_MAX_VIFS; i++) {
		priv = hw_priv->vif_list[i];

		if (!priv->enabled) {
			wifi_printk(WIFI_DBG_MSG, "[%s] not enable now\n", priv->if_name);
			return;
		}
	
		/* set priv to scan init state */
		priv->scan_no_connect == priv->scan_no_connect_back;
		priv->scan.status = 0;
		priv->scan.in_progress = 0;
		priv->scan.ApScan_in_process = 0;
	
		if (atbmwifi_is_ap_mode(priv->iftype)) {
			work = &hw_priv->work_queue_table[priv->scan.ap_scan_work];
			if (!work->valid) {
				priv->scan.ap_scan_work = atbm_init_work(hw_priv,atbmwifi_ap_scan_start,priv);
			}
		}

		if (atbmwifi_is_sta_mode(priv->iftype)) {
			work = &hw_priv->work_queue_table[priv->scan.scan_work];
			if (!work->valid) {
				priv->scan.scan_work = atbm_init_work(hw_priv,atbm_scan_work,priv);
			}
			work = &hw_priv->work_queue_table[priv->join_work];
			if (!work->valid) {
				priv->join_work = atbm_init_work(hw_priv,atbm_join_work,priv);
			}
		}
	
		/* process pending scan requests */
		atbm_req_scan_start(priv);
	}
}

int atbm_ieee80211_start_csa_work(struct atbmwifi_vif *priv, 
				    struct atbm_ieee80211_csa_request *csa)
{
	int ret = 0;
	struct atbmwifi_common *hw_priv = _atbmwifi_vifpriv_to_hwpriv(priv);
	struct atbm_list_head works;

	ATBM_INIT_LIST_HEAD(&works);
	
	if(atbm_ieee80211_csa_running(hw_priv)){
		wifi_printk(WIFI_ERROR, "csa_req hold \n");
		goto exit;
	}
	
	atbm_ieee80211_chan_prepare_csa_work(priv,csa,&works);
exit:
	if(atbm_list_empty(&works)){
		ret = -1;
	}else {
		/*
		*work busy,so wait work complete,because work maybe change operation channel.
		*/		
		if(!atbm_ieee80211_csa_busy(hw_priv)){
			atbm_list_splice_tail_init(&works,&hw_priv->csa_req);
			/*
			*cancle scan,because scaning will cost long time
			*/
			__atbm_ieee80211_scan_cancel(hw_priv);
			__atbm_ieee80211_start_csa_work(hw_priv);
		}else {
			/*
			*pending csa work
			*/
			atbm_list_splice_tail_init(&works,&hw_priv->csa_pending);
		}
	}
	
	return ret;
}

static atbm_void atbm_csa_timerout(atbm_void *data1,atbm_void *data2)
{
	struct atbmwifi_vif *priv =	(struct atbmwifi_vif *)data1;
	struct atbmwifi_common *hw_priv = (struct atbmwifi_common *)data2;

	if (priv->csa_state != ATBM_IEEE80211_CSA_MLME_STATE_START)
		return;

	priv->csa_state = ATBM_IEEE80211_CSA_MLME_STATE_TIMEOUT;
	atbm_queue_work(hw_priv, hw_priv->csa_work);
}

static int atbm_ieee80211_chan_csa_start(struct atbmwifi_common *hw_priv,
				struct ieee80211_csa_work *csa_req)
{
	struct atbmwifi_vif *priv = csa_req->vif;

	if(!priv->enabled){
		wifi_printk(WIFI_ERROR, "priv %p not enabled\n", priv);
		goto fail;
	}
	
	if(atbm_ieee80211_channel_fixed(hw_priv)){
		wifi_printk(WIFI_ERROR, "priv %p csa state [%d], exit \n", priv, priv->csa_state);
		goto fail;
	}

	if(csa_req->block) {
		atbmwifi_ieee80211_stop_tx_queues(priv);
	}
	
	switch(priv->iftype){
	case ATBM_NL80211_IFTYPE_STATION:
		{
			unsigned long timeout = csa_req->mactime + 
							(csa_req->count * priv->bss.sta_priv.beacon_interval);

			atbmwifi_eloop_cancel_timeout(atbm_csa_timerout,(atbm_void *)priv,(atbm_void *)hw_priv);
			if(csa_req->count <= 1){
				priv->csa_state = ATBM_IEEE80211_CSA_MLME_STATE_FINISHED;
				break;
			}
			
			if(atbm_GetOsTimeMs() >= timeout){
				priv->csa_state = ATBM_IEEE80211_CSA_MLME_STATE_FINISHED;
				break;
			}

			priv->csa_state = ATBM_IEEE80211_CSA_MLME_STATE_START;
			atbmwifi_eloop_register_timeout(0,(csa_req->count * priv->bss.sta_priv.beacon_interval),atbm_csa_timerout,(atbm_void *)priv,(atbm_void *)hw_priv);
			break;
		}
	case ATBM_NL80211_IFTYPE_AP:
		{
			struct atbm_ieee80211_csa_request request;

			atbmwifi_eloop_cancel_timeout(atbm_csa_timerout,(atbm_void *)priv,(atbm_void *)hw_priv);
			request.chan     = csa_req->chan;
			request.chantype = csa_req->chan_type;
			request.start    = true;
			request.cfg80211 = csa_req->cfg80211;
			request.ie.count = csa_req->count;
			request.ie.mode  = csa_req->block;
			request.ie.new_ch_num = csa_req->chan;

			wifi_printk(WIFI_CSA, "[%s] ap csa[%d][%d][%d][%d]\n",priv->if_name,request.chantype,
							request.ie.new_ch_num,request.ie.count,request.ie.mode);

			csa_req->mactime = atbm_GetOsTimeMs();
			priv->csa_state = ATBM_IEEE80211_CSA_MLME_STATE_START;
			atbmwifi_eloop_register_timeout(0,(csa_req->count + 5) * (priv->config.beaconInterval),
				atbm_csa_timerout,(atbm_void *)priv,(atbm_void *)hw_priv);

			if(atbmwifi_do_csa(priv, &request)){
				goto fail;
			}

			break;
		}
	default:
		wifi_printk(WIFI_ERROR, "%s csa type err(%d)\n",priv->if_name,priv->iftype);
		BUG_ON(1);
		break;
	}
	return 0;
fail:
	priv->csa_state = ATBM_IEEE80211_CSA_MLME_STATE_FAIL;
	return -1;
}

static bool atbm_ieee80211_chan_complete_csa(struct ieee80211_csa_work *csa_req)
{
	struct atbmwifi_vif *priv = csa_req->vif;
	bool success = false;
	
	switch(priv->iftype){
	case ATBM_NL80211_IFTYPE_STATION:
		if(priv->csa_state >= ATBM_IEEE80211_CSA_MLME_STATE_START){
			success = true;
		}
		break;
	case ATBM_NL80211_IFTYPE_AP:
		switch(priv->csa_state){
		case ATBM_IEEE80211_CSA_MLME_STATE_IDLE:
		case ATBM_IEEE80211_CSA_MLME_STATE_WK:
		case ATBM_IEEE80211_CSA_MLME_STATE_START:
			success = false;
			wifi_printk(WIFI_ERROR, "ap csa state[%d] is false\n",priv->csa_state);
			atbm_fallthrough;
		case ATBM_IEEE80211_CSA_MLME_STATE_TIMEOUT:
			/*timeout can set true*/
			success = true;
			atbm_fallthrough;
		case ATBM_IEEE80211_CSA_MLME_STATE_FAIL:
		case ATBM_IEEE80211_CSA_MLME_STATE_STOP:
			{
				struct atbm_ieee80211_csa_request request;
				/*
				*cancle channel sw,this is very diffical,because some 
				*stations maybe has channged channel.
				*/
				memset(&request,0,sizeof(struct atbm_ieee80211_csa_request));
				request.start = false;
				atbmwifi_do_csa(priv,&request);
			}
			break;
		case ATBM_IEEE80211_CSA_MLME_STATE_FINISHED:
			success = true;
		}
		break;		
	default:
		wifi_printk(WIFI_ERROR, "%s csa type err(%d)\n",priv->if_name,priv->iftype);
		BUG_ON(1);
		break;
	}

	if(csa_req->block) {
		atbmwifi_ieee80211_start_tx_queues(priv);
	}

	wifi_printk(WIFI_CSA, "[%s-%s]:%d\n",__func__,priv->if_name,success);

	return success;
}

static void _atbm_ieee80211_csa_completed_notify(struct atbmwifi_vif *priv)
{
	switch(priv->csa_state){
	case ATBM_IEEE80211_CSA_MLME_STATE_IDLE:
		break;
	case ATBM_IEEE80211_CSA_MLME_STATE_WK:
		break;
	case ATBM_IEEE80211_CSA_MLME_STATE_START:
		break;
	case ATBM_IEEE80211_CSA_MLME_STATE_STOP:
		break;
	case ATBM_IEEE80211_CSA_MLME_STATE_TIMEOUT:
	case ATBM_IEEE80211_CSA_MLME_STATE_FAIL:
	case ATBM_IEEE80211_CSA_MLME_STATE_FINISHED:
		{
			/* CSA is finished, check if we need to notify */
		}
	}
	
	priv->csa_state = ATBM_IEEE80211_CSA_MLME_STATE_IDLE;
}

static void atbm_ieee80211_csa_completed_notify(struct atbmwifi_common *hw_priv)
{
	struct ieee80211_csa_work *csa_req, *tmp;
	struct atbmwifi_vif *priv = NULL;

	atbm_list_for_each_entry_safe(csa_req, tmp, &hw_priv->csa_req, list, struct ieee80211_csa_work){
		priv = csa_req->vif;
		if(priv == NULL){
			continue;
		}

		_atbm_ieee80211_csa_completed_notify(priv);
	}
}

static void atbm_ieee80211_chan_pre_complete_csa(struct atbmwifi_common *hw_priv,struct atbm_ieee80211_csa_status *status)
{
	struct ieee80211_csa_work *csa_req, *tmp;

	status->n_fails = 0;
	status->n_suces = 0;
	atbm_list_for_each_entry_safe(csa_req, tmp, &hw_priv->csa_req, list, struct ieee80211_csa_work){
		if(atbm_ieee80211_chan_complete_csa(csa_req) == true){
			status->n_suces++;
		}else {
			status->n_fails++;
		}
	}
}

bool atbm_ieee80211_set_channel_type(struct atbmwifi_common *hw_priv,
				struct atbmwifi_vif *priv,
				enum atbm_nl80211_channel_type chantype)
{
	int i;
	struct atbmwifi_vif *tmp;
	enum atbm_nl80211_channel_type superchan = ATBM_NL80211_CHAN_NO_HT;
	bool result;

	//mutex_lock(&local->iflist_mtx);
	for (i = 0; i < ATBM_WIFI_MAX_VIFS; i++) {
		tmp = hw_priv->vif_list[i];

		if (tmp == priv) {
			continue;
		}

		if (!tmp->enabled) {
			continue;
		}

		switch (tmp->bss.channel_type) {
		case ATBM_NL80211_CHAN_NO_HT:
		case ATBM_NL80211_CHAN_HT20:
			if (superchan > tmp->bss.channel_type)
				break;

			superchan = tmp->bss.channel_type;
			break;
		case ATBM_NL80211_CHAN_HT40PLUS:
			ATBM_WARN_ON(superchan == ATBM_NL80211_CHAN_HT40MINUS);
			superchan = ATBM_NL80211_CHAN_HT40PLUS;
			break;
		case ATBM_NL80211_CHAN_HT40MINUS:
			ATBM_WARN_ON(superchan == ATBM_NL80211_CHAN_HT40PLUS);
			superchan = ATBM_NL80211_CHAN_HT40MINUS;
			break;
		}
	}

	switch (superchan) {
	case ATBM_NL80211_CHAN_NO_HT:
	case ATBM_NL80211_CHAN_HT20:
		/*
		 * allow any change that doesn't go to no-HT
		 * (if it already is no-HT no change is needed)
		 */
		if (chantype == ATBM_NL80211_CHAN_NO_HT)
			break;
		superchan = chantype;
		break;
	case ATBM_NL80211_CHAN_HT40PLUS:
	case ATBM_NL80211_CHAN_HT40MINUS:
		/* allow smaller bandwidth and same */
		if (chantype == ATBM_NL80211_CHAN_NO_HT)
			break;
		if (chantype == ATBM_NL80211_CHAN_HT20)
			break;
		if (superchan == chantype)
			break;
		result = false;
		goto out;
	}

	if (priv) {
		priv->bss.channel_type = chantype;
	}
	result = true;
 out:
	return result;
}

static void atbm_ieee80211_chan_post_complete_csa(struct atbmwifi_common *hw_priv,struct atbm_ieee80211_csa_status *status)
{
	struct ieee80211_csa_work *csa_req, *tmp;
	struct atbmwifi_vif *priv;
	bool result = ATBM_FALSE;

	if(status->n_fails){
		wifi_printk(WIFI_CSA, "csa fail,return\n");
		return;
	}

	if(status->n_suces == 0){
		wifi_printk(WIFI_CSA, "csa empty,do nothing\n");
		return;
	}
	
	csa_req = atbm_list_first_entry(&hw_priv->csa_req, struct ieee80211_csa_work,list);
	priv = csa_req->vif;
	wifi_printk(WIFI_CSA, "csa_req[%p] dump: count %d, chan %d chantype %d\n", 
		csa_req, csa_req->count, csa_req->chan, csa_req->chan_type);
	atbm_list_for_each_entry_safe(csa_req, tmp, &hw_priv->csa_req, list, struct ieee80211_csa_work){
		priv = csa_req->vif;

		priv->config.channel_index = csa_req->chan;
		priv->bss.channel_num=csa_req->chan;
		priv->bss.channel_type=csa_req->chan_type;
		result = atbm_ieee80211_set_channel_type(hw_priv,priv,csa_req->chan_type);
		switch(priv->iftype){
		case ATBM_NL80211_IFTYPE_STATION:
			wifi_printk(WIFI_CSA, "sta [%s]:csa done\n",priv->if_name);
			break;
		case ATBM_NL80211_IFTYPE_AP:
			wifi_printk(WIFI_CSA, "ap [%s]:csa done\n",priv->if_name);
			wpa_comm_init_extra_ie(priv);
			atbmwifi_ap_start_beacon(priv);
			if (priv->extra_ie) {
				atbm_kfree(priv->extra_ie);
				priv->extra_ie = ATBM_NULL;
				priv->extra_ie_len = 0;
			}
			break;
		default:
			wifi_printk(WIFI_CSA, "%s csa type err(%d)\n",priv->if_name,priv->iftype);
			ATBM_BUG_ON(1);
			break;
		}
	}

	if(status->n_suces == 1){
		/* sta and ap mode change channel or channel type */
		if (priv->assoc_ok || (priv->join_status == ATBMWIFI__JOIN_STATUS_AP)) {
			struct wsm_set_chantype wsm_chantype;
			struct atbmwifi_cfg *config = NULL;
			config = atbmwifi_get_config(priv);

			atbm_memset(&wsm_chantype, 0, sizeof(struct wsm_set_chantype));
			wsm_chantype.band = config->band;
			wsm_chantype.channelNumber = priv->config.channel_index;
			wsm_chantype.channelType = priv->bss.channel_type;
			wsm_chantype.flag = 0;
			wifi_printk(WIFI_CSA, "success 1, set chantype: band %d channelNumber %d channeltype %d\n", 
				wsm_chantype.band, wsm_chantype.channelNumber, wsm_chantype.channelType);
			wsm_set_chantype_func(hw_priv, &wsm_chantype, priv->if_id);
		}
	}else {
		atbm_do_csa_concurrent(hw_priv, &hw_priv->csa_req);
	}
}

void atbm_ieee80211_csa_flush(struct atbmwifi_common *hw_priv)
{
	while(!atbm_list_empty(&hw_priv->csa_req)){
		struct ieee80211_csa_work *csawork =
			atbm_list_first_entry(&hw_priv->csa_req, struct ieee80211_csa_work, list);
		atbm_list_del(&csawork->list);
		atbm_kfree(csawork);
	}

	while(!atbm_list_empty(&hw_priv->csa_pending)){
		struct ieee80211_csa_work *csawork =
			atbm_list_first_entry(&hw_priv->csa_pending, struct ieee80211_csa_work, list);
		atbm_list_del(&csawork->list);
		atbm_kfree(csawork);
	}
}

static void __ieee80211_csa_completed(struct atbmwifi_common *hw_priv)
{
	struct atbm_ieee80211_csa_status status;

	atbm_ieee80211_chan_pre_complete_csa(hw_priv,&status);
	atbm_ieee80211_chan_post_complete_csa(hw_priv,&status);
	atbm_ieee80211_csa_completed_notify(hw_priv);
	atbm_ieee80211_csa_flush(hw_priv);

	hw_priv->next_csa_state = CSA_IDLE;
	WARN_ON(atbm_list_empty(&hw_priv->csa_req) == 0);
	WARN_ON(atbm_list_empty(&hw_priv->csa_pending) == 0);
	
// #ifdef CONFIG_ATBM_SUPPORT_P2P
// 	ieee80211_start_next_roc(hw_priv);
// #endif
// 	ieee80211_queue_work(&hw_priv->hw, &hw_priv->work_work);
	atbm_ieee880211_start_pending_scan(hw_priv);
}

static void _ieee80211_csa_process_work(struct atbmwifi_common *hw_priv)
{
	struct ieee80211_csa_work *csawork, *tmp;
	bool  again = false;
	struct atbmwifi_vif *priv;
	enum atbm_ieee80211_csa_state next_csa_state;

	if(atbm_ieee80211_csa_busy(hw_priv) == 0){
		hw_priv->next_csa_state = CSA_IDLE;
		goto out;
	}
	
	do{
		again = false;
		switch (hw_priv->next_csa_state) {
		case CSA_WAIT_START:
			atbm_list_for_each_entry_reverse_safe(csawork,tmp,&hw_priv->csa_req, list){
				priv = csawork->vif;
				switch (priv->csa_state) {
				case ATBM_IEEE80211_CSA_MLME_STATE_IDLE:
				case ATBM_IEEE80211_CSA_MLME_STATE_START:
				case ATBM_IEEE80211_CSA_MLME_STATE_TIMEOUT:
				case ATBM_IEEE80211_CSA_MLME_STATE_FAIL:
				case ATBM_IEEE80211_CSA_MLME_STATE_FINISHED:
					BUG_ON(1);
					break;
				case ATBM_IEEE80211_CSA_MLME_STATE_WK:
					wifi_printk(WIFI_CSA, "%s:csa try start\n",priv->if_name);
					atbm_ieee80211_chan_csa_start(hw_priv,csawork);
					break;
				case ATBM_IEEE80211_CSA_MLME_STATE_STOP:
					wifi_printk(WIFI_CSA, "%s:csa start stop\n",priv->if_name);
					atbm_list_del(&csawork->list);
					_atbm_ieee80211_csa_completed_notify(priv);
					atbm_kfree(csawork);
					break;
				}
			}
			hw_priv->next_csa_state = CSA_WAIT_COMPLETE;
			again = true;
			break;
		case CSA_WAIT_COMPLETE:

			next_csa_state = CSA_IDLE;
			
			atbm_list_for_each_entry_reverse_safe(csawork,tmp,&hw_priv->csa_req, list){
				priv = csawork->vif;

				if(!priv->enabled){
					wifi_printk(WIFI_CSA, "[%s]:csa stop running\n",priv->if_name);
					atbmwifi_eloop_cancel_timeout(atbm_csa_timerout,(atbm_void *)priv,(atbm_void *)hw_priv);
					priv->csa_state = ATBM_IEEE80211_CSA_MLME_STATE_STOP;
				}
				
				switch(priv->csa_state){
				case ATBM_IEEE80211_CSA_MLME_STATE_IDLE:
				case ATBM_IEEE80211_CSA_MLME_STATE_WK:
					ATBM_BUG_ON(1);
					break;
				case ATBM_IEEE80211_CSA_MLME_STATE_START:
					next_csa_state = CSA_WAIT_COMPLETE;
					wifi_printk(WIFI_CSA, "[%s]:csa running\n",priv->if_name);
					break;
				case ATBM_IEEE80211_CSA_MLME_STATE_TIMEOUT:
				case ATBM_IEEE80211_CSA_MLME_STATE_FAIL:
				case ATBM_IEEE80211_CSA_MLME_STATE_FINISHED:
					wifi_printk(WIFI_CSA, "[%s] csa completed[%d]\n",priv->if_name,priv->csa_state);
					break;
				case ATBM_IEEE80211_CSA_MLME_STATE_STOP:
					atbm_list_del(&csawork->list);
					atbm_ieee80211_chan_complete_csa(csawork);
					_atbm_ieee80211_csa_completed_notify(priv);
					atbm_kfree(csawork);
					wifi_printk(WIFI_CSA, "[%s] csa cancle[%d]\n",priv->if_name,priv->csa_state);
					break;
				}
			}
			
			hw_priv->next_csa_state = next_csa_state;

			if(hw_priv->next_csa_state == CSA_IDLE)
				goto out_complete;
			else
				goto out;	
		default:
			goto out;
		}
	} while(again == true);
		
	goto out;
out_complete:
	__ieee80211_csa_completed(hw_priv);
out:
	return;
}

void atbm_ieee80211_event_csa_done(struct atbmwifi_vif *priv, atbm_uint32 status)
{
	struct atbmwifi_common *hw_priv = _atbmwifi_vifpriv_to_hwpriv(priv);

	atbmwifi_eloop_cancel_timeout(atbm_csa_timerout,(atbm_void *)priv,(atbm_void *)hw_priv);
	
	if(priv->csa_state != ATBM_IEEE80211_CSA_MLME_STATE_START){
		wifi_printk(WIFI_ERROR, "[%s] csa idle\n", priv->if_name);
		return;
	}

	priv->csa_state = status ? ATBM_IEEE80211_CSA_MLME_STATE_FINISHED : ATBM_IEEE80211_CSA_MLME_STATE_FAIL;
	wifi_printk(WIFI_ALWAYS, "[%s]:event_csa_done[%d]\n", priv->if_name, priv->csa_state);

	atbm_queue_work(hw_priv, hw_priv->csa_work);
}

void atbm_ieee80211_csa_process_work(struct atbm_work_struct *work)
{
	struct atbmwifi_common *hw_priv =(struct atbmwifi_common *)work;

	_ieee80211_csa_process_work(hw_priv);
}

atbm_void atbm_ieee80211_sta_process_chanswitch(struct atbmwifi_vif *priv,struct atbmwifi_cfg80211_bss *bss,
					  struct atbmwifi_ieee80211_channel_sw_packed_ie *sw_packed_ie,
					  atbm_uint64 timestamp)
{
	struct atbmwifi_common *hw_priv = _atbmwifi_vifpriv_to_hwpriv(priv);
	struct atbmwifi_cfg *config = atbmwifi_get_config(priv);

	struct atbmwifi_ieee80211_channel_sw_ie *sw_elem = sw_packed_ie->chan_sw_ie;
	struct atbmwifi_ieee80211_ext_chansw_ie *ex_sw_elem = sw_packed_ie->ex_chan_sw_ie;
	struct atbmwifi_ieee80211_sec_chan_offs_ie *sec_chan_ie = sw_packed_ie->sec_chan_offs_ie;
	enum atbmwifi_ieee80211_band new_band = ATBM_IEEE80211_NUM_BANDS;
	#define CHANGE_SW_CHANNEL_BIT		(1<<0)
	#define CHANGE_CHANNEL_TYPE_BIT		(1<<1)
	int new_freq = 0;
	struct atbmwifi_ieee80211_channel *new_ch = ATBM_NULL;
	enum atbm_nl80211_channel_type prev_chantype = (enum atbm_nl80211_channel_type)bss->channel_type;
	struct atbm_ieee80211_csa_request csa = {0};

	if (!priv->assoc_ok){
		return;
	}

	if (priv->csa_state != ATBM_IEEE80211_CSA_MLME_STATE_IDLE) {
		wifi_printk(WIFI_DBG_MSG, "csa state [%d], exit \n", priv->csa_state);
		return;
		}

	if(ex_sw_elem) {
		if(ieee80211_opclass_to_band(ex_sw_elem->new_operaring_class, &new_band) == ATBM_FALSE) {
			return;
	}
		csa.ie.new_ch_num= ex_sw_elem->new_ch_num;
		csa.ie.mode = ex_sw_elem->mode;
		csa.ie.count = ex_sw_elem->count;
		csa.chan = ex_sw_elem->new_ch_num;
	} else if(sw_elem) {
		new_band =  (enum atbmwifi_ieee80211_band)priv->hw_priv->band;
		csa.ie.new_ch_num = sw_elem->new_ch_num;
		csa.ie.mode = sw_elem->mode;
		csa.ie.count = sw_elem->count;
		csa.chan = sw_elem->new_ch_num;
	} 
	new_freq = atbmwifi_ieee80211_channel_to_frequency(csa.ie.new_ch_num, new_band);

	new_ch = atbmwifi_ieee80211_get_channel(hw_priv, new_freq);
	if (!new_ch || new_ch->flags & ATBM_IEEE80211_CHAN_DISABLED){
		return;
	}

	csa.chantype = prev_chantype;
	if(sec_chan_ie) {
		switch(sec_chan_ie->sec_chan_offs) {
			case ATBM_IEEE80211_HT_PARAM_CHA_SEC_NONE:
				csa.chantype = ATBM_NL80211_CHAN_NO_HT;
				break;
			case ATBM_IEEE80211_HT_PARAM_CHA_SEC_ABOVE:
				csa.chantype = ATBM_NL80211_CHAN_HT40PLUS;
				break;
			case ATBM_IEEE80211_HT_PARAM_CHA_SEC_BELOW:
				csa.chantype = ATBM_NL80211_CHAN_HT40MINUS;
				break;
			default:
				break;
		}
		if(prev_chantype == csa.chantype) {
			return;
		}
	}

	if((prev_chantype == csa.chantype) && (csa.ie.new_ch_num == bss->channel_num)) {
		wifi_printk(WIFI_DBG_MSG, "sta csa not change \n");
		return;
	}

	config->flags |= ATBM_IEEE80211_STA_CSA_RECEIVED;
	csa.mactime = timestamp;
	atbm_ieee80211_start_csa_work(priv, &csa);
	/* channel switch handled in software */
	if (csa.ie.new_ch_num!=bss->channel_num){
		bss->channel_num=csa.ie.new_ch_num;
			}
	bss->channel_type=csa.chantype;
			}
#endif /* CONFIG_ATBM_SUPPORT_CSA */

static int ecw2cw(int ecw)
{
	return (1 << ecw) - 1;
}
int atbmwifi_config_tx_wmmparam(struct atbmwifi_vif *priv)
{ 
	struct atbmwifi_common *hw_priv = _atbmwifi_vifpriv_to_hwpriv(priv);
	struct config_edca_params *wmm_param;
	struct wsm_edca_params		edca;
	ATBM_BOOL old_uapsdFlags;
	int ret;
	old_uapsdFlags=priv->uapsd_info.uapsdFlags;
	//VO
	wmm_param= &priv->wmm_params[ATBM_D11_ACI_AC_VO];
	WSM_EDCA_SET(&edca, ATBM_IEEE80211_AC_VO /*0*/, wmm_param->aifns, wmm_param->cwMin, wmm_param->cwMax,
			wmm_param->txOpLimit, 0xc8, wmm_param->uapsdEnable, wmm_param->mu_edca, wmm_param->mu_edca_param_rec.ecw_min_max,
			wmm_param->mu_edca_param_rec.aifsn, wmm_param->mu_edca_param_rec.mu_edca_timer); 
	//VI
	wmm_param= &priv->wmm_params[ATBM_D11_ACI_AC_VI];
	WSM_EDCA_SET(&edca, ATBM_IEEE80211_AC_VI /*1*/, wmm_param->aifns, wmm_param->cwMin, wmm_param->cwMax,
			wmm_param->txOpLimit, 0xc8, wmm_param->uapsdEnable, wmm_param->mu_edca,wmm_param->mu_edca_param_rec.ecw_min_max,
			wmm_param->mu_edca_param_rec.aifsn, wmm_param->mu_edca_param_rec.mu_edca_timer); 
	//BE
	wmm_param= &priv->wmm_params[ATBM_D11_ACI_AC_BE];
	WSM_EDCA_SET(&edca, ATBM_IEEE80211_AC_BE /*2*/, wmm_param->aifns, wmm_param->cwMin, wmm_param->cwMax,
			wmm_param->txOpLimit, 0xc8, wmm_param->uapsdEnable, wmm_param->mu_edca, wmm_param->mu_edca_param_rec.ecw_min_max,
			wmm_param->mu_edca_param_rec.aifsn, wmm_param->mu_edca_param_rec.mu_edca_timer); 
	//BK
	wmm_param= &priv->wmm_params[ATBM_D11_ACI_AC_BK];
	WSM_EDCA_SET(&edca, ATBM_IEEE80211_AC_BK /*3*/, wmm_param->aifns, wmm_param->cwMin, wmm_param->cwMax,
			wmm_param->txOpLimit, 0xc8, wmm_param->uapsdEnable, wmm_param->mu_edca_param_rec.ecw_min_max,
			wmm_param->mu_edca_param_rec.aifsn, wmm_param->mu_edca, wmm_param->mu_edca_param_rec.mu_edca_timer); 

	ret = wsm_set_edca_params(hw_priv, &edca, priv->if_id);
	if (ATBM_WARN_ON(ret)){
		goto out;
	}
	//wifi_printk(WIFI_CONNECT,"old_uapsdFlags=%d,priv->uapsd_info.uapsdFlags=%d\n",old_uapsdFlags,priv->uapsd_info.uapsdFlags);
	if (priv->iftype == ATBM_NL80211_IFTYPE_STATION) {
		ret = atbmwifi_set_uapsd_param(priv, &edca);
		if(!ret&&(old_uapsdFlags!=priv->uapsd_info.uapsdFlags)){
			atbmwifi_set_pm(priv,ATBM_TRUE,0);
		}
	}
	return ret;
out:
	return -1;

}
atbm_void atbmwifi_ieee80211_set_wmm_default(struct atbmwifi_vif *priv)
{
	struct atbmwifi_common *hw_priv = _atbmwifi_vifpriv_to_hwpriv(priv);
	ATBM_BOOL use_11b = ATBM_FALSE;
	int ac;
	int queue;
	int aCWmin, aCWmax;
	struct config_edca_params *qparam;
	if (hw_priv->hw_queues < ATBM_IEEE80211_NUM_ACS)
		return;
	qparam=&priv->wmm_params[0];
	if (!priv->bss.sta_priv.rate_11g){
		use_11b=ATBM_TRUE;
	}
	for (ac = 0; ac < ATBM_IEEE80211_NUM_ACS; ac++) {
	/* Set defaults according to 802.11-2007 Table 7-37 */
	aCWmax = 1023;
	if (use_11b)
		aCWmin = 31;
	else
		aCWmin = 15;

	switch (ac) {
	case 3: /* AC_BK */
		queue = 0;
		qparam[queue].cwMax = aCWmax;
		qparam[queue].cwMin = aCWmin;
		qparam[queue].txOpLimit= 0;
		qparam[queue].aifns= 7;
		break;
	default: /* never happens but let's not leave undefined */
	case 2: /* AC_BE */
		queue = 1;
		qparam[queue].cwMax = aCWmax;
		qparam[queue].cwMin = aCWmin;
		qparam[queue].txOpLimit = 0;
		qparam[queue].aifns = 3;
		break;
	case 1: /* AC_VI */
		queue = 2;
		qparam[queue].cwMax = aCWmin;
		qparam[queue].cwMin = (aCWmin + 1) / 2 - 1;
		if (use_11b)
			qparam[queue].txOpLimit = 6016/32;
		else
			qparam[queue].txOpLimit = 3008/32;
		qparam[queue].aifns = 2;
		break;
	case 0: /* AC_VO */
		queue = 3;
		qparam[queue].cwMax = (aCWmin + 1) / 2 - 1;
		qparam[queue].cwMin = (aCWmin + 1) / 4 - 1;
		if (use_11b)
			qparam[queue].txOpLimit = 3264/32;
		else
			qparam[queue].txOpLimit = 1504/32;
		qparam[queue].aifns = 2;
		break;
	}

	qparam[queue].uapsdEnable= ATBM_FALSE;
	/* enable WMM or activate new settings */
	atbmwifi_config_tx_wmmparam(priv);

	}
}

static unsigned short atbm_get_unaligned_le16(const unsigned char * ptr)
{
    return (((ptr[1]) << 8) | (ptr[0]));
}

int atbmwifi_ieee80211_sta_wmm_params(struct atbmwifi_vif *priv,
										atbm_uint8 *wmm_param, int wmm_param_len,
										const struct atbm_ieee80211_mu_edca_param_set * mu_edca)
{
	int left;
	//int count;
	atbm_uint8 *pos, uapsd_queues = 0;
	uapsd_queues = priv->uapsd_queues;
	struct atbm_ieee80211_he_mu_edca_param_ac_rec mu_edca_param_rec;
	
	if (!wmm_param){
		return ATBM_FALSE;
	}
	if (wmm_param_len < 8 || wmm_param[5] /* version */ != 1)
		return ATBM_FALSE;
	pos = wmm_param + 8;
	left = wmm_param_len - 8;
	
	for (; left >= 4; left -= 4, pos += 4) {
		int aci = (pos[0] >> 5) & 0x03;
		int acm = (pos[0] >> 4) & 0x01;
		int uapsd = ATBM_FALSE;
		//int queue;
		/*wifi_printk(WIFI_CONNECT,"aci=%d,acm=%d,aifns=%d,cwMax=%d,cwMin=%d,txOpLimit=%d,uapsdEnable=%d\n",aci,acm,(pos[0] & 0x0f),(ecw2cw((pos[1] & 0xf0) >> 4)),\
			(ecw2cw(pos[1] & 0x0f)),atbm_get_unaligned_le16(pos + 2),uapsd);*/
		switch (aci) {
		case 1: /* AC_BK */
			if (acm)
				priv->wmm_acm |= ATBM_BIT(1) | ATBM_BIT(2); /* BK/- */
			if (uapsd_queues & ATBM_IEEE80211_WMM_IE_STA_QOSINFO_AC_BK)
				uapsd = ATBM_TRUE;
			if(mu_edca)
				mu_edca_param_rec = mu_edca->ac_bk;
			break;
		case 2: /* AC_VI */
			if (acm)
				priv->wmm_acm |= ATBM_BIT(4) | ATBM_BIT(5); /* CL/VI */
			if (uapsd_queues & ATBM_IEEE80211_WMM_IE_STA_QOSINFO_AC_VI)
				uapsd = ATBM_TRUE;
			if(mu_edca)
				mu_edca_param_rec = mu_edca->ac_vi;
			break;
		case 3: /* AC_VO */
			if (acm)
				priv->wmm_acm |= ATBM_BIT(6) | ATBM_BIT(7); /* VO/NC */
			if (uapsd_queues & ATBM_IEEE80211_WMM_IE_STA_QOSINFO_AC_VO)
				uapsd = ATBM_TRUE;
			if(mu_edca)
				mu_edca_param_rec = mu_edca->ac_vo;
			break;
		case 0: /* AC_BE */
		default:
			if (acm)
				priv->wmm_acm |= ATBM_BIT(0) | ATBM_BIT(3); /* BE/EE */
			if (uapsd_queues & ATBM_IEEE80211_WMM_IE_STA_QOSINFO_AC_BE)
				uapsd = ATBM_TRUE;
			if(mu_edca)
				mu_edca_param_rec = mu_edca->ac_be;
			break;
		}
		/* enable WMM or activate new settings */
		priv->wmm_params[aci].wmep_acm=priv->wmm_acm;
		priv->wmm_params[aci].aifns= pos[0] & 0x0f;
		priv->wmm_params[aci].cwMax= ecw2cw((pos[1] & 0xf0) >> 4);
		priv->wmm_params[aci].cwMin= ecw2cw(pos[1] & 0x0f);
		priv->wmm_params[aci].txOpLimit= atbm_get_unaligned_le16(pos + 2);
		priv->wmm_params[aci].uapsdEnable= uapsd;
		priv->wmm_params[aci].mu_edca= !!mu_edca;
		if (mu_edca)
			priv->wmm_params[aci].mu_edca_param_rec = mu_edca_param_rec;
	}

	if(atbmwifi_config_tx_wmmparam(priv))
		return ATBM_FALSE;

	return ATBM_TRUE;
}
ATBM_BOOL atbmwifi_iee80211_check_combination(struct atbmwifi_vif *ignore_priv,atbm_uint8 combination_channel)
{
	struct atbmwifi_common	*hw_priv = ignore_priv->hw_priv;
	struct atbmwifi_vif *priv = ATBM_NULL;
	atbm_uint8 index = 0;
	for(index = 0;index<ATBM_WIFI_MAX_VIFS;index++){
		priv = hw_priv->vif_list[index];
		if(priv == ATBM_NULL){
			continue;
		}
		if(priv == ignore_priv){
			continue;
		}
		if(priv->iftype == ATBM_NUM_NL80211_IFTYPES){
			continue;
		}
		if((priv->join_status == ATBMWIFI__JOIN_STATUS_STA)||
		   (priv->join_status == ATBMWIFI__JOIN_STATUS_AP)){
		   if((atbm_uint8)priv->config.channel_index != combination_channel){
		   		wifi_printk(WIFI_ALWAYS, "channel conflict[%d]:[%d] \n", priv->config.channel_index, combination_channel);
		   		return ATBM_FALSE;
		   }
		}
	}

	return ATBM_TRUE;
}

#if BW_40M_SUPPORT
atbm_void atbmwifi_iee80211_unify_channel_type(struct atbmwifi_vif *ignore_priv, atbm_uint32 channel_type)
{
	struct atbmwifi_common	*hw_priv = ignore_priv->hw_priv;
	struct atbmwifi_vif *priv = ATBM_NULL;
	struct wsm_set_chantype set_channtype;
	atbm_uint8 index = 0;
	hw_priv->channel_type = channel_type;
	for(index = 0;index<ATBM_WIFI_MAX_VIFS;index++){
		priv = hw_priv->vif_list[index];
		if(priv == ATBM_NULL){
			continue;
		}
		if(priv == ignore_priv){
			continue;
		}
		if(priv->iftype == ATBM_NUM_NL80211_IFTYPES){
			continue;
		}
		if((priv->join_status == ATBMWIFI__JOIN_STATUS_STA)||
		   (priv->join_status == ATBMWIFI__JOIN_STATUS_AP)){
		   if(priv->bss.channel_type > ATBM_NL80211_CHAN_HT20
		   		&& channel_type > ATBM_NL80211_CHAN_HT20
		   		&& priv->bss.channel_type != channel_type){
		   		set_channtype.band = (hw_priv->band == ATBM_IEEE80211_BAND_5GHZ) ?
							 WSM_PHY_BAND_5G : WSM_PHY_BAND_2_4G;
				set_channtype.flag = 0;
				set_channtype.channelNumber = ignore_priv->config.channel_index;
				set_channtype.channelType = channel_type;
				priv->bss.channel_type = channel_type;
				atbmwifi_ap_start_beacon(priv);
		   		wsm_set_chantype_func(hw_priv,&set_channtype,priv->if_id);
		   }
		}
	}

	return;
}

int atbmwifi_iee80211_peerif_channel_type(struct atbmwifi_vif *ignore_priv)
{
	struct atbmwifi_common	*hw_priv = ignore_priv->hw_priv;
	struct atbmwifi_vif *priv = ATBM_NULL;
	atbm_uint8 index = 0;
	for(index = 0;index<ATBM_WIFI_MAX_VIFS;index++){
		priv = hw_priv->vif_list[index];
		if(priv == ATBM_NULL){
			continue;
		}
		if(priv == ignore_priv){
			continue;
		}
		if(priv->iftype == ATBM_NUM_NL80211_IFTYPES){
			continue;
		}
		if((priv->join_status == ATBMWIFI__JOIN_STATUS_STA)||
		   (priv->join_status == ATBMWIFI__JOIN_STATUS_AP)){
		   	return priv->bss.channel_type;
		}
	}
	return hw_priv->channel_type;
}
#endif

int atbmwifi_iee80211_peerif_channel(struct atbmwifi_vif *ignore_priv)
{
	struct atbmwifi_common	*hw_priv = ignore_priv->hw_priv;
	struct atbmwifi_vif *priv = ATBM_NULL;
	atbm_uint8 index = 0;
	for(index = 0;index<ATBM_WIFI_MAX_VIFS;index++){
		priv = hw_priv->vif_list[index];
		if(priv == ATBM_NULL){
			continue;
		}
		if(priv == ignore_priv){
			continue;
		}
		if((priv->join_status == ATBMWIFI__JOIN_STATUS_STA)||
		   (priv->join_status == ATBMWIFI__JOIN_STATUS_AP)){
		   if((atbm_uint8)priv->config.channel_index > 0
#if (CONFIG_5G_SUPPORT == 0)
		   	&& 	((atbm_uint8)priv->config.channel_index < 14)
#endif
		   	)
		   {
		   		return (atbm_uint8)priv->config.channel_index;
		   }
		}
	}

	return 0;
}

struct atbmwifi_vif * atbmwifi_iee80211_getvif_by_name
	(struct atbmwifi_common	*hw_priv,char *name)
{
	atbm_uint8 name_size = strlen(name);
	struct atbmwifi_vif *priv = ATBM_NULL;
	atbm_uint8 index = 0;
	wifi_printk(WIFI_ALWAYS,"name(%s),name_size(%d)\n",name,name_size);
	if(name_size>ATBM_IFNAMSIZ)
		return ATBM_NULL;
	for(index = 0;index<ATBM_WIFI_MAX_VIFS;index++){
		priv = hw_priv->vif_list[index];
		if(priv == ATBM_NULL)
			continue;
		if(memcmp(priv->if_name,name,name_size) == 0){
			wifi_printk(WIFI_ALWAYS,"name(%s),name_size(%d),if_id(%d)\n",name,name_size,priv->if_id);
			break;
		}
	}

	return priv;
}
ATBM_BOOL atbmwifi_ieee80211_check_alive_if(struct atbmwifi_common	*hw_priv,struct atbmwifi_vif *ignore_priv)
{
	struct atbmwifi_vif *priv = ATBM_NULL;
	atbm_uint8 index = 0;

	for(index = 0;index<ATBM_WIFI_MAX_VIFS;index++){
		priv = hw_priv->vif_list[index];
		if(priv == ATBM_NULL){
			continue;
		}
		if(priv == ignore_priv){
			continue;
		}

		if(priv->iftype != ATBM_NUM_NL80211_IFTYPES){
			return ATBM_TRUE;
		}
	}

	return ATBM_FALSE;
}

atbm_void atbmwifi_start_iftype(struct atbmwifi_vif *start_priv,enum atbm_nl80211_iftype start_type)
{
	if(start_priv->iftype != ATBM_NUM_NL80211_IFTYPES){
		return;
	}
	start_priv->iftype = start_type;

	if(atbmwifi_is_sta_mode(start_type)){
		atbmwifi_start_sta(start_priv);
	}
	else if(atbmwifi_is_ap_mode(start_type)){
		atbmwifi_start_ap(start_priv);	
	}
}

atbm_void atbmwifi_stop_iftype(struct atbmwifi_vif *stop_priv,enum atbm_nl80211_iftype stop_type)
{
	if(stop_priv->iftype == ATBM_NUM_NL80211_IFTYPES){
		return;
	}
	if(atbmwifi_is_sta_mode(stop_type)){
		atbmwifi_stop_sta(stop_priv);
	}
	else if(atbmwifi_is_ap_mode(stop_type)){
		atbmwifi_stop_ap(stop_priv);
	}

	stop_priv->iftype = ATBM_NUM_NL80211_IFTYPES;
}

static atbm_void __atbmwifi_start_wifimode(struct atbmwifi_vif *start_priv,enum atbm_nl80211_iftype start_type)
{
	ATBM_BOOL other_alive_if = ATBM_FALSE;
	ATBM_BOOL all_alive_if = ATBM_FALSE;
	
	ATBM_BOOL priv_need_change = ((start_type != start_priv->iftype)&&(start_priv->iftype != ATBM_NUM_NL80211_IFTYPES))? ATBM_TRUE:ATBM_FALSE;
	other_alive_if = atbmwifi_ieee80211_check_alive_if(start_priv->hw_priv,start_priv);
	all_alive_if = atbmwifi_ieee80211_check_alive_if(start_priv->hw_priv,ATBM_NULL);
	if(priv_need_change == ATBM_TRUE){
		atbmwifi_stop_iftype(start_priv,start_priv->iftype);
	}
	/*Intial/stop the Station/Ap iftype is ATBM_NUM_NL80211_IFTYPES*/
	if(start_priv->iftype == ATBM_NUM_NL80211_IFTYPES){
		atbmwifi_start_iftype(start_priv,start_type);
	}
	wifi_printk(WIFI_ALWAYS,"atbmwifi_start_wifimode:other_alive_if(%d),all_alive_if(%d)\n ",other_alive_if,all_alive_if);
}
atbm_void atbmwifi_start_wifimode(struct atbmwifi_vif *start_priv,enum atbm_nl80211_iftype start_type)
{
	atbmwifi_wpa_event_queue((atbm_wpa_event_data)start_priv,(atbm_wpa_event_data)start_type,(atbm_wpa_event_data)ATBM_NULL,WPA_EVENT__INIT,ATBM_WPA_EVENT_ACK);
}
atbm_void atbmwifi_wpa_event_mode_init(struct atbmwifi_vif *start_priv,enum atbm_nl80211_iftype start_type)
{	
	__atbmwifi_start_wifimode(start_priv,start_type);
}
static atbm_void __atbmwifi_stop_wifimode(struct atbmwifi_vif *stop_priv,enum atbm_nl80211_iftype stop_type)
{
	ATBM_BOOL other_alive_if = ATBM_FALSE;
	ATBM_BOOL all_alive_if = ATBM_FALSE;
	other_alive_if = atbmwifi_ieee80211_check_alive_if(stop_priv->hw_priv,stop_priv);
	all_alive_if = atbmwifi_ieee80211_check_alive_if(stop_priv->hw_priv,ATBM_NULL);
	atbmwifi_stop_iftype(stop_priv,stop_type);

	if(all_alive_if != other_alive_if){
		wifi_printk(WIFI_ALWAYS,"All Interface is down\n ");
	}
}
atbm_void atbmwifi_wpa_event_mode_deinit(struct atbmwifi_vif *stop_priv,enum atbm_nl80211_iftype stop_type)
{	
	__atbmwifi_stop_wifimode(stop_priv,stop_type);
}
atbm_void atbmwifi_stop_wifimode(struct atbmwifi_vif *stop_priv,enum atbm_nl80211_iftype stop_type)
{
	atbmwifi_wpa_event_queue((atbm_wpa_event_data)stop_priv,(atbm_wpa_event_data)stop_type,(atbm_wpa_event_data)ATBM_NULL,WPA_EVENT__DEINIT,ATBM_WPA_EVENT_ACK);
}
struct atbmwifi_cfg *atbmwifi_get_config(struct atbmwifi_vif *priv)
{
	return	&priv->config;
}
struct atbmwifi_vif *atbmwifi_config_get_priv(struct atbmwifi_cfg *config)
{
	return atbm_container_of(config,struct atbmwifi_vif,config);
}
struct hostapd_data *atbmiwifi_get_hostapd(struct atbmwifi_vif *priv)
{
	return (struct hostapd_data *)(priv->appdata);
}

#if CONFIG_WIFI_BT_COMB
int atbm_ble_do_ble_xmit(struct atbmwifi_common *hw_priv,atbm_uint8* xmit,atbm_size_t len)
{
	return wsm_ble_xmit(hw_priv,xmit,len);
}
#endif

#ifndef CONFIG_OS_FREERTOS
#if (BYTE_ORDER == LITTLE_ENDIAN)

/**
 * Convert an uint16 from host- to network byte order.
 *
 * @param n uint16 in host byte order
 * @return n in network byte order
 */
atbm_uint16 atbm_htons(atbm_uint16 n)
{
  return ((n & 0xff) << 8) | ((n & 0xff00) >> 8);
}

/**
 * Convert an uint16 from network- to host byte order.
 *
 * @param n uint16 in network byte order
 * @return n in host byte order
 */
atbm_uint16 atbm_ntohs(atbm_uint16 n)
{
  return atbm_htons(n);
}

/**
 * Convert an uint32 from host- to network byte order.
 *
 * @param n uint32 in host byte order
 * @return n in network byte order
 */
atbm_uint32 atbm_htonl(atbm_uint32 n)
{
  return ((n & 0xff) << 24) |
    ((n & 0xff00) << 8) |
    ((n & 0xff0000UL) >> 8) |
    ((n & 0xff000000UL) >> 24);
}

/**
 * Convert an uint32 from network- to host byte order.
 *
 * @param n uint32 in network byte order
 * @return n in host byte order
 */
atbm_uint32 atbm_ntohl(atbm_uint32 n)
{
  return atbm_htonl(n);
}
#endif /* (LWIP_PLATFORM_BYTESWAP == 0) && (BYTE_ORDER == LITTLE_ENDIAN) */


atbm_void dump_mem(const atbm_void *mem, int count)
{
	const unsigned char *p = mem;
	int i = 0;

	for(i = 0; i < count; i++){

		if( i % 16 == 0)
			ATBM_DEBUG(1, 0, "\n");

		ATBM_DEBUG(1, 0, "%02x ", p[i]);
	}
	ATBM_DEBUG(1, 0, "\n");
}

char ConvertHexChar(char ch){
	if(ch >= '0' && ch <= '9')
		return ch - '0';
	if(ch >= 'a' && ch <= 'f')
		return ch - 'a' + 10;
	if(ch >= 'A' && ch < 'F')
		return ch - 'A' + 10;
	return -1;
}

int atbm_hex2byte(const char *hex)
{
	int a, b;
	a = ConvertHexChar(*hex++);
	if(a < 0)
		return -1;
	b = ConvertHexChar(*hex++);
	if(b < 0)
		return -1;
	return (a << 4) | b;
}
int atbmwifi_hexstr2bin(atbm_uint8 *buf, const char *hex, atbm_size_t len)
{
	atbm_size_t i;
	int a;
	const char *ipos = hex;
	atbm_uint8 *opos = (atbm_uint8 *)buf;

	for(i = 0; i < len; i += 2){
		a = atbm_hex2byte(ipos);
		if(a < 0)
			return -1;
		*opos++ = a;
		ipos += 2;
	}
	return 0;
}

char *atbm_dup_binstr(const atbm_void *src, atbm_size_t len)
{
	char *res;

	if(src == ATBM_NULL)
		return ATBM_NULL;
	res = (char *)atbm_kmalloc(len + 1,GFP_KERNEL);
	if(res == ATBM_NULL)
		return res;
	atbm_memcpy(res, src, len);
	res[len] = '\0';

	return res;
}
#endif //CONFIG_OS_FREERTOS

/* Try to prevent most compilers from optimizing out clearing of memory that
 * becomes unaccessible after this function is called. This is mostly the case
 * for clearing local stack variables at the end of a function. This is not
 * exactly perfect, i.e., someone could come up with a compiler that figures out
 * the pointer is pointing to memset and then end up optimizing the call out, so
 * try go a bit further by storing the first octet (now zero) to make this even
 * a bit more difficult to optimize out. Once memset_s() is available, that
 * could be used here instead. */
static atbm_uint8 forced_memzero_val;
void atbm_forced_memzero(void *ptr, atbm_size_t len)
{
	atbm_memset(ptr, 0, len);
	if (len)
		forced_memzero_val = ((atbm_uint8 *) ptr)[0];
}


void atbm_bin_clear_free(void *bin, atbm_size_t len)
{
	if (bin) {
		atbm_forced_memzero(bin, len);
		atbm_kfree(bin);
	}
}

void atbm_buf_shift_right(atbm_uint8 *buf, atbm_size_t len, atbm_size_t bits)
{
	atbm_size_t i;

	for (i = len - 1; i > 0; i--)
		buf[i] = (buf[i - 1] << (8 - bits)) | (buf[i] >> bits);
	buf[0] >>= bits;
}

int random_get_bytes(atbm_uint8 *buf, atbm_size_t len)
{
	return atbmwifi_os_get_random(buf, len);
}

