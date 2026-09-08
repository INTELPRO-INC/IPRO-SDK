/**************************************************************************************************************
 * altobeam RTOS wifi hmac source code 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 *
 *  The source code contains proprietary information of AltoBeam, and shall not be distributed, 
 *  copied, reproduced, or disclosed in whole or in part without prior written permission of AltoBeam.
*****************************************************************************************************************/
#ifndef LINUX_OS
#include <string.h>
#include <stdio.h>
#endif
#include "atbm_hal.h"
#include "svn_version.h"
#if ATBM_SUPPORT_BRIDGE
#include "atbm_bridge.h"
#endif

#define CRONUS_USB_WIFI_FIRMWARE_NAME "firmware_cronus_usb_notxconfirm.h"
#define CRONUS_USB_COMBO_FIRMWARE_NAME "firmware_cronus_usb_combo_notxconfirm.h"
#define CRONUS_SDIO_WIFI_FIRMWARE_NAME "firmware_cronus_sdio_notxconfirm.h"
#define CRONUS_SDIO_COMBO_FIRMWARE_NAME "firmware_cronus_sdio_combo_notxconfirm.h"
#define CRONUS_LITE_USB_WIFI_FIRMWARE_NAME "firmware_cronus_lite_usb_notxconfirm.h"
#define CRONUS_LITE_USB_COMBO_FIRMWARE_NAME "firmware_cronus_lite_usb_combo_notxconfirm.h"
#define CRONUS_LITE_SDIO_WIFI_FIRMWARE_NAME "firmware_cronus_lite_sdio_notxconfirm.h"
#define CRONUS_LITE_SDIO_COMBO_FIRMWARE_NAME "firmware_cronus_lite_sdio_combo_notxconfirm.h"
#define OCEANUS_USB_WIFI_FIRMWARE_NAME "firmware_oceanus_usb_notxconfirm.h"
#define OCEANUS_USB_COMBO_FIRMWARE_NAME "firmware_oceanus_usb_combo_notxconfirm.h"
#define OCEANUS_SDIO_WIFI_FIRMWARE_NAME "firmware_oceanus_sdio_notxconfirm.h"
#define OCEANUS_SDIO_COMBO_FIRMWARE_NAME "firmware_oceanus_sdio_combo_notxconfirm.h"
#define COEUS_USB_WIFI_FIRMWARE_NAME "firmware_coeus_usb_notxconfirm.h"
#define COEUS_SDIO_WIFI_FIRMWARE_NAME "firmware_coeus_sdio_notxconfirm.h"
#define OCEANUS_LITE_USB_WIFI_FIRMWARE_NAME "firmware_oceanus_lite_usb_notxconfirm.h"
#define OCEANUS_LITE_USB_COMBO_FIRMWARE_NAME "firmware_oceanus_lite_usb_combo_notxconfirm.h"
#define OCEANUS_LITE_SDIO_WIFI_FIRMWARE_NAME "firmware_oceanus_lite_sdio_notxconfirm.h"
#define OCEANUS_LITE_SDIO_COMBO_FIRMWARE_NAME "firmware_oceanus_lite_sdio_combo_notxconfirm.h"
#define DEMETER_USB_WIFI_FIRMWARE_NAME "firmware_demeter_usb_notxconfirm.h"
#define DEMETER_USB_COMBO_FIRMWARE_NAME "firmware_demeter_usb_combo_notxconfirm.h"
#define DEMETER_SDIO_WIFI_FIRMWARE_NAME "firmware_demeter_sdio_notxconfirm.h"
#define DEMETER_SDIO_COMBO_FIRMWARE_NAME "firmware_demeter_sdio_combo_notxconfirm.h"

#define FIRM_FILE_NAME_INF_TYPE(alias,inf,type) alias##_##inf##_##type##_FIRMWARE_NAME

#if CONFIG_WIFI_BT_COMB
#define FIRMTYPE COMBO
#define FIRM_HEADR_NAME(alias) firmware_headr##alias##BLE
#define FIRM_ICCM_NAME(alias) fw_code##alias##BLE
#define FIRM_DCCM_NAME(alias) fw_data##alias##BLE
#define FIRM_SRAM_NAME(alias) fw_sram##alias##BLE
#define FIRM_FILE_NAME_INF(alias,inf) FIRM_FILE_NAME_INF_TYPE(alias,inf,COMBO)
#else
#define FIRMTYPE WIFI
#define FIRM_HEADR_NAME(alias) firmware_headr##alias
#define FIRM_ICCM_NAME(alias) fw_code##alias
#define FIRM_DCCM_NAME(alias) fw_data##alias
#define FIRM_SRAM_NAME(alias) fw_sram##alias
#define FIRM_FILE_NAME_INF(alias,inf) FIRM_FILE_NAME_INF_TYPE(alias,inf,WIFI)
#endif

#if ATBM_SDIO_BUS
#define FIRM_FILE_NAME(alias) FIRM_FILE_NAME_INF(alias,SDIO)
#elif ATBM_USB_BUS
#define FIRM_FILE_NAME(alias) FIRM_FILE_NAME_INF(alias,USB)
#endif

atbm_uint8 default_macaddr[6] = {0x00,0x00,0x11,0x32,0x43,0x69};
extern struct tcpip_opt lwip_tcp_opt;
struct atbmwifi_common g_hw_prv;
#if ATBM_SDIO_BUS
#define SET_SDIO_DOWNLOAD_BLOCKSIZE(hw_priv,type,bs) do { \
			hw_priv->sbus_ops->lock(hw_priv->sbus_priv);\
			ATBM_WARN_ON_FUNC(hw_priv->sbus_ops->set_block_size(hw_priv->sbus_priv,type,bs));\
			hw_priv->sbus_ops->unlock(hw_priv->sbus_priv);  \
		}while(0)
#define INIT_SDIO_FUNCTION_4(func) atbm_sdio_check_init_func4(func)
#else
#define SET_SDIO_DOWNLOAD_BLOCKSIZE(hw_priv,type,bs) do{ }while(0)
#define INIT_SDIO_FUNCTION_4(func) (ATBM_NULL)
#endif

#define SET_SDIO_TRANSMIT_BLOCKSIZE(hw_priv) do{ \
			hw_priv->sbus_ops->lock(hw_priv->sbus_priv);\
			ATBM_WARN_ON_FUNC(hw_priv->sbus_ops->set_block_size(hw_priv->sbus_priv,0,ATBM_SDIO_BLOCK_SIZE));\
			hw_priv->sbus_ops->unlock(hw_priv->sbus_priv);\
	}while(0)

extern atbm_void atbm_get_efuse_data(struct atbmwifi_common *hw_priv);
extern atbm_void atbm_wifi_ticks_timer_init(atbm_void);
extern int atbm_wifi_add_interfaces(struct atbmwifi_common *hw_priv,enum atbm_nl80211_iftype iftype, char *if_name);
extern int atbm_wifi_remove_interfaces(struct atbmwifi_vif *priv);
extern atbm_int32 atbmwifi_enable_lmaclog(atbm_uint32 value );
extern atbm_void atbm_free_netdev(struct atbm_net_device * netdev);
extern 	int atbm_create_timerTask(struct atbmwifi_common *hw_priv);
struct atbmwifi_common *_atbmwifi_vifpriv_to_hwpriv(struct atbmwifi_vif *priv)
{
	return priv->hw_priv;
}
struct atbmwifi_vif *_atbmwifi_hwpriv_to_vifpriv(struct atbmwifi_common *hw_priv,int if_id)
{
	//struct atbmwifi_vif *vif;
	ATBM_WARN_ON_FUNC(-1 == if_id);
	if ((-1 == if_id) || (if_id >= ATBM_WIFI_MAX_VIFS)){
		return ATBM_NULL;
	}

	return hw_priv->vif_list[if_id];
}

static  int _atbmwifi_get_nr_hw_ifaces(struct atbmwifi_common *hw_priv)
{
	return 1;
}

/* TODO: use rates and channels from the device */
#define RATETAB_ENT(_rate, _rateid, _flags)		{_rate, _rateid, _flags}

/*
struct atbmwifi_ieee80211_rate {
atbm_uint16 bitrate;
atbm_uint8 hw_value;
atbm_uint8 rate_flag;
};

.bitrate	= (_rate),		\
.hw_value	= (_rateid),		\
.rate_flag	= (_flags),	\
*/

struct atbmwifi_ieee80211_rate atbmwifi_rates[ATBM_WIFI_RATE_SIZE] = {
	RATETAB_ENT(2,  0,   ATBM_IEEE80211_RT_BASIC|ATBM_IEEE80211_RT_11B),
	RATETAB_ENT(4,  1,   ATBM_IEEE80211_RT_BASIC|ATBM_IEEE80211_RT_11B),
	RATETAB_ENT(11,  2,   ATBM_IEEE80211_RT_BASIC|ATBM_IEEE80211_RT_11B),
	RATETAB_ENT(22, 3,   ATBM_IEEE80211_RT_BASIC|ATBM_IEEE80211_RT_11B),
	RATETAB_ENT(12,  6,   ATBM_IEEE80211_RT_11G),
	RATETAB_ENT(18,  7,  ATBM_IEEE80211_RT_11G),
	RATETAB_ENT(24, 8,  ATBM_IEEE80211_RT_11G),
	RATETAB_ENT(36, 9,  ATBM_IEEE80211_RT_11G),
	RATETAB_ENT(48, 10, ATBM_IEEE80211_RT_11G),
	RATETAB_ENT(72, 11, ATBM_IEEE80211_RT_11G),
	RATETAB_ENT(96, 12, ATBM_IEEE80211_RT_11G),
	RATETAB_ENT(108, 13, ATBM_IEEE80211_RT_11G),
};

#if CONFIG_5G_SUPPORT
struct atbmwifi_ieee80211_rate atbmwifi_rates_5g[ATBM_WIFI_RATE_SIZE] = {	
	RATETAB_ENT(12,  6,   ATBM_IEEE80211_RT_BASIC|ATBM_IEEE80211_RT_11G),
	RATETAB_ENT(18,  7,  ATBM_IEEE80211_RT_11G),
	RATETAB_ENT(24, 8,  ATBM_IEEE80211_RT_BASIC|ATBM_IEEE80211_RT_11G),
	RATETAB_ENT(36, 9,  ATBM_IEEE80211_RT_11G),
	RATETAB_ENT(48, 10, ATBM_IEEE80211_RT_BASIC|ATBM_IEEE80211_RT_11G),
	RATETAB_ENT(72, 11, ATBM_IEEE80211_RT_11G),
	RATETAB_ENT(96, 12, ATBM_IEEE80211_RT_11G),
	RATETAB_ENT(108, 13, ATBM_IEEE80211_RT_11G),
};
#endif

//500k  unit
struct atbmwifi_ieee80211_rate atbm_mcs_rates[] = {
	RATETAB_ENT(13,  14, ATBM_IEEE80211_TX_RC_MCS),
	RATETAB_ENT(26, 15, ATBM_IEEE80211_TX_RC_MCS),
	RATETAB_ENT(39, 16, ATBM_IEEE80211_TX_RC_MCS),
	RATETAB_ENT(52, 17, ATBM_IEEE80211_TX_RC_MCS),
	RATETAB_ENT(78, 18, ATBM_IEEE80211_TX_RC_MCS),
	RATETAB_ENT(104, 19, ATBM_IEEE80211_TX_RC_MCS),
	RATETAB_ENT(117, 20, ATBM_IEEE80211_TX_RC_MCS),
	RATETAB_ENT(130, 21, ATBM_IEEE80211_TX_RC_MCS),
	RATETAB_ENT(12 , 22, ATBM_IEEE80211_TX_RC_MCS),
};
#define CHAN2G(_channel, _freq, _flags) {30, _channel, _flags, _freq}
#define CHAN5G(_channel, _flags) {				\
	.hw_value		= (_channel),			\
	.flags			= (_flags),			\
	.max_power		= 30,				\
}

const static struct atbmwifi_ieee80211_channel atbmwifi_2ghz_chantable_const[] = {
	CHAN2G(1, 2412, 0),
	CHAN2G(2, 2417, 0),
	CHAN2G(3, 2422, 0),
	CHAN2G(4, 2427, 0),
	CHAN2G(5, 2432, 0),
	CHAN2G(6, 2437, 0),
	CHAN2G(7, 2442, 0),
	CHAN2G(8, 2447, 0),
	CHAN2G(9, 2452, 0),
	CHAN2G(10, 2457, 0),
	CHAN2G(11, 2462, 0),
	CHAN2G(12, 2467, 0),
	CHAN2G(13, 2472, 0),
	CHAN2G(14, 2484, 0),
};

static struct atbmwifi_ieee80211_channel atbmwifi_2ghz_chantable[14] = {
	CHAN2G(1, 2412, 0),
	CHAN2G(2, 2417, 0),
	CHAN2G(3, 2422, 0),
	CHAN2G(4, 2427, 0),
	CHAN2G(5, 2432, 0),
	CHAN2G(6, 2437, 0),
	CHAN2G(7, 2442, 0),
	CHAN2G(8, 2447, 0),
	CHAN2G(9, 2452, 0),
	CHAN2G(10, 2457, 0),
	CHAN2G(11, 2462, 0),
	CHAN2G(12, 2467, 0),
	CHAN2G(13, 2472, 0),
	CHAN2G(14, 2484, 0),
};

#if CONFIG_5G_SUPPORT
static struct atbmwifi_ieee80211_channel atbm_5ghz_chantable[] = {
	CHAN5G(34, 0),		CHAN5G(36, 0),
	CHAN5G(38, 0),		CHAN5G(40, 0),
	CHAN5G(42, 0),		CHAN5G(44, 0),
	CHAN5G(46, 0),		CHAN5G(48, 0),
	CHAN5G(52, 0),		CHAN5G(56, 0),
	CHAN5G(60, 0),		CHAN5G(64, 0),
	CHAN5G(100, 0),		CHAN5G(104, 0),
	CHAN5G(108, 0),		CHAN5G(112, 0),
	CHAN5G(116, 0),		CHAN5G(120, 0),
	CHAN5G(124, 0),		CHAN5G(128, 0),
	CHAN5G(132, 0),		CHAN5G(136, 0),
	CHAN5G(140, 0),		
	CHAN5G(144, 0), 	CHAN5G(149, 0),
	CHAN5G(153, 0),		CHAN5G(157, 0),
	CHAN5G(161, 0),		CHAN5G(165, 0),
#if 0
	CHAN5G(184, 0),		CHAN5G(188, 0),
	CHAN5G(192, 0),		CHAN5G(196, 0),
	CHAN5G(200, 0),		CHAN5G(204, 0),
	CHAN5G(208, 0),		CHAN5G(212, 0),
	CHAN5G(216, 0),
#endif
};
#endif

#if CONFIG_HE
#if CONFIG_ATBM_HE_BEAMFORM
#define ATBM_IEEE80211_HE_PHY_CAP6_BEAMFORM_ENABLE ATBM_IEEE80211_HE_PHY_CAP6_TRIG_SU_BEAMFORMING_FB |\
				ATBM_IEEE80211_HE_PHY_CAP6_TRIG_MU_BEAMFORMING_PARTIAL_BW_FB|\
				ATBM_IEEE80211_HE_PHY_CAP6_TRIG_CQI_FB
				
#define ATBM_IEEE80211_HE_PHY_CAP4_BEAMFORM_ENABLE ATBM_IEEE80211_HE_PHY_CAP4_SU_BEAMFORMEE |\
					ATBM_IEEE80211_HE_PHY_CAP4_BEAMFORMEE_MAX_STS_UNDER_80MHZ_4
#else
#define ATBM_IEEE80211_HE_PHY_CAP6_BEAMFORM_ENABLE 0
#define ATBM_IEEE80211_HE_PHY_CAP4_BEAMFORM_ENABLE 0
#endif  //CONFIG_ATBM_HE_BEAMFORM
#endif

/**
 * @brief 2.4GHz 频段支持能力定义
 *
 * 该结构体定义了 WiFi 芯片在 2.4GHz 频段的支持能力，包括：
 * - 信道列表 (1-14 信道)
 * - 支持的速率 (802.11b/g/n/ac/ax)
 * - HT (802.11n) 能力
 * - VHT (802.11ac) 能力 (如果支持)
 * - HE (802.11ax/WiFi6) 能力 (如果支持)
 */
struct atbmwifi_ieee80211_supported_band atbmwifi_band_2ghz = {
	.channels = atbmwifi_2ghz_chantable,                    /* 2.4GHz 信道表 */
	.n_channels = ATBM_ARRAY_SIZE(atbmwifi_2ghz_chantable), /* 信道数量 (通常为 14 个) */
	.bitrates = atbmwifi_g_rates,                           /* 支持的速率表 (802.11b/g) */
	.n_bitrates = atbmwifi_g_rates_size,                    /* 速率数量 */
	.band = ATBM_IEEE80211_BAND_2GHZ,                       /* 频段标识: 2.4GHz */

	/**
	 * HT (802.11n) 能力配置
	 * 支持特性:
	 * - 20/40MHz 带宽 (如果 BW_40M_SUPPORT 使能)
	 * - 短保护间隔 (SGI) 20MHz/40MHz
	 * - RX STBC (空时分组码)
	 * - LDPC (低密度奇偶校验码)
	 * - AMPDU 聚合 (最大 32K)
	 */
	.ht_cap = {
		.cap =
#if BW_40M_SUPPORT
				ATBM_IEEE80211_HT_CAP_SUP_WIDTH_20_40|      /* 支持 40MHz 带宽 */
				ATBM_IEEE80211_HT_CAP_DSSSCCK40|            /* 40MHz 模式下支持 DSSS/CCK */
				ATBM_IEEE80211_HT_CAP_SGI_40 |              /* 40MHz 短保护间隔 */
#endif
				ATBM_IEEE80211_HT_CAP_SGI_20|                /* 20MHz 短保护间隔 */
				(1 << ATBM_IEEE80211_HT_CAP_RX_STBC_SHIFT)| /* RX STBC 支持 */
				ATBM_IEEE80211_HT_CAP_LDPC_CODING,          /* LDPC 编码支持 */
		.ht_supported = 1,                                  /* 支持 HT */
		.ampdu_factor = ATBM_IEEE80211_HT_MAX_AMPDU_32K,    /* AMPDU 最大长度 32K */
		.ampdu_density = ATBM_IEEE80211_HT_MPDU_DENSITY_NONE, /* MPDU 密度: 无限制 */
		.mcs.rx_mask[0] = 0xFF,                             /* MCS 0-7 支持 (单空间流) */
		.mcs.rx_highest = 0,                                /* 最高接收速率 (0 = 未指定) */
		.mcs.tx_params = ATBM_IEEE80211_HT_MCS_TX_DEFINED,  /* TX MCS 参数已定义 */
	},
#if CONFIG_VHT
	/**
	 * VHT (802.11ac) 能力配置
	 * 支持特性:
	 * - 最大 MPDU 长度 3895 字节
	 * - RX LDPC 编码
	 * - RX STBC (1 个空间流)
	 * - 单用户波束成形 (SU Beamformer/Beamformee)
	 * - HTC-VHT 支持
	 * - 最大 AMPDU 长度指数 2 (64K)
	 * - 天线图案支持
	 * - MCS 0-9 支持 (单空间流)
	 */
	.vht_cap =
	{
	   .vht_supported = ATBM_TRUE,                        /* 支持 VHT */
	   .cap = ATBM_IEEE80211_VHT_CAP_MAX_MPDU_LENGTH_3895 | /* 最大 MPDU 长度 3895 */
		   ATBM_IEEE80211_VHT_CAP_RXLDPC |	           /* RX LDPC 支持 */
		   ATBM_IEEE80211_VHT_CAP_RXSTBC_1 |            /* RX STBC 1 个空间流 */
		   ATBM_IEEE80211_VHT_CAP_SU_BEAMFORMER_CAPABLE | /* SU Beamformer */
		   ATBM_IEEE80211_VHT_CAP_SU_BEAMFORMEE_CAPABLE | /* SU Beamformee */
		   ATBM_IEEE80211_VHT_CAP_HTC_VHT |             /* HTC-VHT 支持 */
		   (1 << ATBM_IEEE80211_VHT_CAP_BEAMFORMEE_STS_SHIFT) | /* Beamformee STS = 1 */
		   /*IEEE80211_VHT_CAP_MAX_A_MPDU_LENGTH_EXPONENT_MASK |*/
		   (2 << ATBM_IEEE80211_VHT_CAP_MAX_A_MPDU_LENGTH_EXPONENT_SHIFT) | /* AMPDU 指数 2 */
		   ATBM_IEEE80211_VHT_CAP_RX_ANTENNA_PATTERN |  /* RX 天线图案 */
		   ATBM_IEEE80211_VHT_CAP_TX_ANTENNA_PATTERN |  /* TX 天线图案 */
		   0,
	   .vht_mcs = {
		   /* RX MCS 映射: 仅支持单空间流 MCS 0-9 */
		   .rx_mcs_map = atbm_cpu_to_le16(ATBM_IEEE80211_VHT_MCS_SUPPORT_0_9 << 0 |
		   ATBM_IEEE80211_VHT_MCS_NOT_SUPPORTED << 2 |
		   ATBM_IEEE80211_VHT_MCS_NOT_SUPPORTED << 4 |
		   ATBM_IEEE80211_VHT_MCS_NOT_SUPPORTED << 6 |
		   ATBM_IEEE80211_VHT_MCS_NOT_SUPPORTED << 8 |
		   ATBM_IEEE80211_VHT_MCS_NOT_SUPPORTED << 10 |
		   ATBM_IEEE80211_VHT_MCS_NOT_SUPPORTED << 12 |
		   ATBM_IEEE80211_VHT_MCS_NOT_SUPPORTED << 14),
		   .rx_highest = 0,                             /* 最高接收速率 (0 = 未指定) */
		   /* TX MCS 映射: 仅支持单空间流 MCS 0-9 */
		   .tx_mcs_map =  atbm_cpu_to_le16(ATBM_IEEE80211_VHT_MCS_SUPPORT_0_9 << 0 |
		   ATBM_IEEE80211_VHT_MCS_NOT_SUPPORTED << 2 |
		   ATBM_IEEE80211_VHT_MCS_NOT_SUPPORTED << 4 |
		   ATBM_IEEE80211_VHT_MCS_NOT_SUPPORTED << 6 |
		   ATBM_IEEE80211_VHT_MCS_NOT_SUPPORTED << 8 |
		   ATBM_IEEE80211_VHT_MCS_NOT_SUPPORTED << 10 |
		   ATBM_IEEE80211_VHT_MCS_NOT_SUPPORTED << 12 |
		   ATBM_IEEE80211_VHT_MCS_NOT_SUPPORTED << 14),
		   .tx_highest = 0,                             /* 最高发送速率 (0 = 未指定) */
	   },
	},
#endif
#if CONFIG_HE
	/**
	 * HE (802.11ax/WiFi6) 能力配置
	 * 支持特性:
	 * - HTC-HE 支持
	 * - 触发帧 MAC 填充时长 16us
	 * - 多 TID 聚合接收 QoS
	 * - BSR (Buffer Status Report) 支持
	 * - MU 级联支持
	 * - 40MHz 带宽 (2.4GHz)
	 * - RU 映射支持
	 * - LDPC 编码
	 * - STBC 接收
	 * - 波束成形支持
	 * - 1024-QAM 支持
	 */
	.iftype_data = {
		/* 支持的接口类型: STA 和 AP */
		.types_mask = ATBM_BIT(ATBM_NL80211_IFTYPE_STATION) |
				  ATBM_BIT(ATBM_NL80211_IFTYPE_AP),
		.he_cap = {
			.has_he = ATBM_TRUE,                         /* 支持 HE */
			.he_cap_elem = {
				/* MAC 能力信息 [0]: HTC-HE 支持 */
				.mac_cap_info[0] =
					ATBM_IEEE80211_HE_MAC_CAP0_HTC_HE,
				/* MAC 能力信息 [1]: 触发帧填充时长 + 多 TID 聚合 */
				.mac_cap_info[1] =
					ATBM_IEEE80211_HE_MAC_CAP1_TF_MAC_PAD_DUR_16US |
					ATBM_IEEE80211_HE_MAC_CAP1_MULTI_TID_AGG_RX_QOS_1,
				/* MAC 能力信息 [2]: BSR + MU 级联 */
				.mac_cap_info[2] =
					ATBM_IEEE80211_HE_MAC_CAP2_BSR |
					ATBM_IEEE80211_HE_MAC_CAP2_MU_CASCADING ,/*|
					ATBM_IEEE80211_HE_MAC_CAP2_ACK_EN,*/
				/* MAC 能力信息 [3]: 最大 AMPDU 长度扩展 */
				.mac_cap_info[3] =
					ATBM_IEEE80211_HE_MAC_CAP3_MAX_AMPDU_LEN_EXP_EXT_0,
				/* MAC 能力信息 [4]: AMPDU 中支持 AMSDU */
				.mac_cap_info[4] = ATBM_IEEE80211_HE_MAC_CAP4_AMSDU_IN_AMPDU,
				/* PHY 能力信息 [0]: 40MHz 带宽 + RU 映射 */
				.phy_cap_info[0] =
					ATBM_IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_40MHZ_IN_2G|
                ATBM_IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_RU_MAPPING_IN_2G,
				/* PHY 能力信息 [1]: 设备类别 A + LDPC */
				.phy_cap_info[1] =
					ATBM_IEEE80211_HE_PHY_CAP1_DEVICE_CLASS_A |
					ATBM_IEEE80211_HE_PHY_CAP1_LDPC_CODING_IN_PAYLOAD,
				/* PHY 能力信息 [2]: NDP 4x LTF + STBC RX */
				.phy_cap_info[2] =
					ATBM_IEEE80211_HE_PHY_CAP2_NDP_4x_LTF_AND_3_2US |
					ATBM_IEEE80211_HE_PHY_CAP2_STBC_RX_UNDER_80MHZ,
					//ATBM_IEEE80211_HE_PHY_CAP2_UL_MU_FULL_MU_MIMO |
					//ATBM_IEEE80211_HE_PHY_CAP2_UL_MU_PARTIAL_MU_MIMO,
				/* PHY 能力信息 [3]: DCM 最大星座图 (TX BPSK, RX 16-QAM) */
				.phy_cap_info[3] = ATBM_IEEE80211_HE_PHY_CAP3_DCM_MAX_CONST_TX_BPSK |
					ATBM_IEEE80211_HE_PHY_CAP3_DCM_MAX_CONST_RX_16_QAM,
				/* PHY 能力信息 [4]: 波束成形使能 */
				.phy_cap_info[4] = ATBM_IEEE80211_HE_PHY_CAP4_BEAMFORM_ENABLE /*IEEE80211_HE_PHY_CAP4_SU_BEAMFORMEE |
						ATBM_IEEE80211_HE_PHY_CAP4_BEAMFORMEE_MAX_STS_UNDER_80MHZ_4*/,
				/* MAC 能力信息 [5]: OM 控制 - UL MU 数据禁用 RX */
				.mac_cap_info[5] = ATBM_IEEE80211_HE_MAC_CAP5_OM_CTRL_UL_MU_DATA_DIS_RX,
				/* PHY 能力信息 [6]: 波束成形使能 */
				.phy_cap_info[6] = ATBM_IEEE80211_HE_PHY_CAP6_BEAMFORM_ENABLE,
					//ATBM_IEEE80211_HE_PHY_CAP6_PARTIAL_BW_EXT_RANGE,
					//ATBM_IEEE80211_HE_PHY_CAP6_PARTIAL_BANDWIDTH_DL_MUMIMO,
				/* PHY 能力信息 [7]: HE SU/MU PPDU 4x LTF + 0.8us GI */
				.phy_cap_info[7] =ATBM_IEEE80211_HE_PHY_CAP7_HE_SU_MU_PPDU_4XLTF_AND_08_US_GI,
				/* PHY 能力信息 [8]: HE ER SU PPDU + 20MHz in 40MHz + DCM RU 484 */
				.phy_cap_info[8] =ATBM_IEEE80211_HE_PHY_CAP8_HE_ER_SU_PPDU_4XLTF_AND_08_US_GI |
					ATBM_IEEE80211_HE_PHY_CAP8_20MHZ_IN_40MHZ_HE_PPDU_IN_2G|
					ATBM_IEEE80211_HE_PHY_CAP8_DCM_MAX_RU_484,
				/* PHY 能力信息 [9]: 1024-QAM 支持 + CQI 反馈 */
				.phy_cap_info[9] =	ATBM_IEEE80211_HE_PHY_CAP9_NON_TRIGGERED_CQI_FEEDBACK |
				ATBM_IEEE80211_HE_PHY_CAP9_TX_1024_QAM_LESS_THAN_242_TONE_RU|
					ATBM_IEEE80211_HE_PHY_CAP9_RX_1024_QAM_LESS_THAN_242_TONE_RU|
					ATBM_IEEE80211_HE_PHY_CAP9_RX_FULL_BW_SU_USING_MU_WITH_COMP_SIGB|
					ATBM_IEEE80211_HE_PHY_CAP9_RX_FULL_BW_SU_USING_MU_WITH_NON_COMP_SIGB|
					ATBM_IEEE80211_HE_PHY_CAP9_NOMIMAL_PKT_PADDING_8US,

				/*
				 * 其余 PHY 能力字节保持未设置
				 * 因为 DCM、波束成形、RU 和 PPE 门限信息
				 * 在此配置中不支持
				 */
			},
			/* HE MCS-NSS 支持: 仅支持单空间流 */
			.he_mcs_nss_supp = {
				/* 80MHz: 支持 MCS 0-11 (单空间流) */
				.rx_mcs_80 = atbm_cpu_to_le16(0xfffe),
				.tx_mcs_80 = atbm_cpu_to_le16(0xfffe),
				/* 160MHz: 不支持 (0xffff = 不支持) */
				.rx_mcs_160 = atbm_cpu_to_le16(0xffff),
				.tx_mcs_160 = atbm_cpu_to_le16(0xffff),
				/* 80+80MHz: 不支持 (0xffff = 不支持) */
				.rx_mcs_80p80 = atbm_cpu_to_le16(0xffff),
				.tx_mcs_80p80 = atbm_cpu_to_le16(0xffff),
			},
			/* PPE 门限: 未使用 (0 = 无 PPE) */
			.ppe_thres[0] = 0,
		},
	},
#endif
};


#if CONFIG_5G_SUPPORT
/**
 * @brief 5GHz 频段支持能力定义
 *
 * 该结构体定义了 WiFi 芯片在 5GHz 频段的支持能力，包括：
 * - 信道列表 (36-165 信道，UNII-1/2/2e/3 频段)
 * - 支持的速率 (802.11a/n/ac/ax)
 * - HT (802.11n) 能力
 * - VHT (802.11ac) 能力 (如果支持)
 * - HE (802.11ax/WiFi6) 能力 (如果支持)
 *
 * @note 与 2.4GHz 相比，5GHz 支持更宽的带宽 (40/80MHz)
 *       和更多的信道，但不支持 802.11b/g 的 DSSS/CCK 速率
 */
static struct atbmwifi_ieee80211_supported_band atbmwifi_band_5ghz = {
	.channels = atbm_5ghz_chantable,                    /* 5GHz 信道表 (UNII-1/2/2e/3) */
	.n_channels = ATBM_ARRAY_SIZE(atbm_5ghz_chantable), /* 信道数量 (通常为 25 个) */
	.bitrates = atbmwifi_a_rates,                       /* 支持的速率表 (802.11a) */
	.n_bitrates = atbmwifi_a_rates_size,                /* 速率数量 */
	.band = ATBM_IEEE80211_BAND_5GHZ,                   /* 频段标识: 5GHz */

	/**
	 * HT (802.11n) 能力配置
	 * 支持特性:
	 * - 20/40MHz 带宽 (如果 BW_40M_SUPPORT 使能)
	 * - 短保护间隔 (SGI) 20MHz/40MHz
	 * - RX STBC (空时分组码)
	 * - LDPC (低密度奇偶校验码)
	 * - AMPDU 聚合 (最大 32K)
	 */
	.ht_cap = {
		.cap =
#if BW_40M_SUPPORT
				ATBM_IEEE80211_HT_CAP_SUP_WIDTH_20_40|      /* 支持 40MHz 带宽 */
				ATBM_IEEE80211_HT_CAP_DSSSCCK40|            /* 40MHz 模式下支持 DSSS/CCK */
				ATBM_IEEE80211_HT_CAP_SGI_40 |              /* 40MHz 短保护间隔 */
#endif
				ATBM_IEEE80211_HT_CAP_SGI_20|                /* 20MHz 短保护间隔 */
				(1 << ATBM_IEEE80211_HT_CAP_RX_STBC_SHIFT)| /* RX STBC 支持 */
				ATBM_IEEE80211_HT_CAP_LDPC_CODING,          /* LDPC 编码支持 */
		.ht_supported = 1,                                  /* 支持 HT */
		.ampdu_factor = ATBM_IEEE80211_HT_MAX_AMPDU_32K,    /* AMPDU 最大长度 32K */
		.ampdu_density = ATBM_IEEE80211_HT_MPDU_DENSITY_NONE, /* MPDU 密度: 无限制 */
		.mcs = {
			.rx_mask[0] = 0xFF,                             /* MCS 0-7 支持 (单空间流) */
			.rx_highest = __atbm_cpu_to_le16(0),            /* 最高接收速率 (0 = 未指定) */
			.tx_params = ATBM_IEEE80211_HT_MCS_TX_DEFINED,  /* TX MCS 参数已定义 */
		},
	},
#if CONFIG_VHT
	/**
	 * VHT (802.11ac) 能力配置
	 * 支持特性:
	 * - 最大 MPDU 长度 3895 字节
	 * - RX LDPC 编码
	 * - RX STBC (1 个空间流)
	 * - 单用户波束成形 (SU Beamformer/Beamformee)
	 * - HTC-VHT 支持
	 * - 最大 AMPDU 长度指数 2 (64K)
	 * - 天线图案支持
	 * - MCS 0-9 支持 (单空间流)
	 */
	.vht_cap =
	{
	   .vht_supported = ATBM_TRUE,                        /* 支持 VHT */
	   .cap = ATBM_IEEE80211_VHT_CAP_MAX_MPDU_LENGTH_3895 | /* 最大 MPDU 长度 3895 */
		   ATBM_IEEE80211_VHT_CAP_RXLDPC |	           /* RX LDPC 支持 */
		   ATBM_IEEE80211_VHT_CAP_RXSTBC_1 |            /* RX STBC 1 个空间流 */
		   ATBM_IEEE80211_VHT_CAP_SU_BEAMFORMER_CAPABLE | /* SU Beamformer */
		   ATBM_IEEE80211_VHT_CAP_SU_BEAMFORMEE_CAPABLE | /* SU Beamformee */
		   ATBM_IEEE80211_VHT_CAP_HTC_VHT |             /* HTC-VHT 支持 */
		   (1 << ATBM_IEEE80211_VHT_CAP_BEAMFORMEE_STS_SHIFT) | /* Beamformee STS = 1 */
		   /*IEEE80211_VHT_CAP_MAX_A_MPDU_LENGTH_EXPONENT_MASK |*/
		   (2 << ATBM_IEEE80211_VHT_CAP_MAX_A_MPDU_LENGTH_EXPONENT_SHIFT) | /* AMPDU 指数 2 */
		   ATBM_IEEE80211_VHT_CAP_RX_ANTENNA_PATTERN |  /* RX 天线图案 */
		   ATBM_IEEE80211_VHT_CAP_TX_ANTENNA_PATTERN |  /* TX 天线图案 */
		   0,
	   .vht_mcs = {
		   /* RX MCS 映射: 仅支持单空间流 MCS 0-9 */
		   .rx_mcs_map = atbm_cpu_to_le16(ATBM_IEEE80211_VHT_MCS_SUPPORT_0_9 << 0 |
		   ATBM_IEEE80211_VHT_MCS_NOT_SUPPORTED << 2 |
		   ATBM_IEEE80211_VHT_MCS_NOT_SUPPORTED << 4 |
		   ATBM_IEEE80211_VHT_MCS_NOT_SUPPORTED << 6 |
		   ATBM_IEEE80211_VHT_MCS_NOT_SUPPORTED << 8 |
		   ATBM_IEEE80211_VHT_MCS_NOT_SUPPORTED << 10 |
		   ATBM_IEEE80211_VHT_MCS_NOT_SUPPORTED << 12 |
		   ATBM_IEEE80211_VHT_MCS_NOT_SUPPORTED << 14),
		   .rx_highest = 0,                             /* 最高接收速率 (0 = 未指定) */
		   /* TX MCS 映射: 仅支持单空间流 MCS 0-9 */
		   .tx_mcs_map =  atbm_cpu_to_le16(ATBM_IEEE80211_VHT_MCS_SUPPORT_0_9 << 0 |
		   ATBM_IEEE80211_VHT_MCS_NOT_SUPPORTED << 2 |
		   ATBM_IEEE80211_VHT_MCS_NOT_SUPPORTED << 4 |
		   ATBM_IEEE80211_VHT_MCS_NOT_SUPPORTED << 6 |
		   ATBM_IEEE80211_VHT_MCS_NOT_SUPPORTED << 8 |
		   ATBM_IEEE80211_VHT_MCS_NOT_SUPPORTED << 10 |
		   ATBM_IEEE80211_VHT_MCS_NOT_SUPPORTED << 12 |
		   ATBM_IEEE80211_VHT_MCS_NOT_SUPPORTED << 14),
		   .tx_highest = 0,                             /* 最高发送速率 (0 = 未指定) */
	   },
	},
#endif
#if CONFIG_HE
	/**
	 * HE (802.11ax/WiFi6) 能力配置
	 * 支持特性:
	 * - HTC-HE 支持
	 * - 触发帧 MAC 填充时长 16us
	 * - 多 TID 聚合接收 QoS
	 * - BSR (Buffer Status Report) 支持
	 * - MU 级联支持
	 * - 40/80MHz 带宽 (5GHz)
	 * - LDPC 编码
	 * - STBC 接收
	 * - 波束成形支持
	 * - 部分带宽扩展范围
	 * - 1024-QAM 支持
	 * - PPE (Packet Padding Extension) 门限
	 */
	.iftype_data = {
		/* 支持的接口类型: STA 和 AP */
		.types_mask = ATBM_BIT(ATBM_NL80211_IFTYPE_STATION) |
				  ATBM_BIT(ATBM_NL80211_IFTYPE_AP),
		.he_cap = {
			.has_he = ATBM_TRUE,                         /* 支持 HE */
			.he_cap_elem = {
				/* MAC 能力信息 [0]: HTC-HE 支持 */
				.mac_cap_info[0] =
					ATBM_IEEE80211_HE_MAC_CAP0_HTC_HE,
				/* MAC 能力信息 [1]: 触发帧填充时长 + 多 TID 聚合 */
				.mac_cap_info[1] =
					ATBM_IEEE80211_HE_MAC_CAP1_TF_MAC_PAD_DUR_16US |
					ATBM_IEEE80211_HE_MAC_CAP1_MULTI_TID_AGG_RX_QOS_1,
				/* MAC 能力信息 [2]: BSR + MU 级联 */
				.mac_cap_info[2] =
					ATBM_IEEE80211_HE_MAC_CAP2_BSR |
					ATBM_IEEE80211_HE_MAC_CAP2_MU_CASCADING ,/*|
					ATBM_IEEE80211_HE_MAC_CAP2_ACK_EN,*/
				/* MAC 能力信息 [3]: 最大 AMPDU 长度扩展 */
				.mac_cap_info[3] =
					ATBM_IEEE80211_HE_MAC_CAP3_MAX_AMPDU_LEN_EXP_EXT_0,
				/* MAC 能力信息 [4]: AMPDU 中支持 AMSDU */
				.mac_cap_info[4] = ATBM_IEEE80211_HE_MAC_CAP4_AMSDU_IN_AMPDU,
				/* PHY 能力信息 [0]: 40/80MHz 带宽 (5GHz) */
				.phy_cap_info[0] =
					ATBM_IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_40MHZ_80MHZ_IN_5G,
				/* PHY 能力信息 [1]: 设备类别 A + LDPC */
				.phy_cap_info[1] =
					ATBM_IEEE80211_HE_PHY_CAP1_DEVICE_CLASS_A |
					ATBM_IEEE80211_HE_PHY_CAP1_LDPC_CODING_IN_PAYLOAD,
				/* PHY 能力信息 [2]: NDP 4x LTF + STBC RX */
				.phy_cap_info[2] =
					ATBM_IEEE80211_HE_PHY_CAP2_NDP_4x_LTF_AND_3_2US |
					ATBM_IEEE80211_HE_PHY_CAP2_STBC_RX_UNDER_80MHZ,
					//ATBM_IEEE80211_HE_PHY_CAP2_UL_MU_FULL_MU_MIMO |
					//ATBM_IEEE80211_HE_PHY_CAP2_UL_MU_PARTIAL_MU_MIMO,
				/* PHY 能力信息 [3]: DCM 最大星座图 (TX BPSK, RX 16-QAM) */
				.phy_cap_info[3] = ATBM_IEEE80211_HE_PHY_CAP3_DCM_MAX_CONST_TX_BPSK |
					ATBM_IEEE80211_HE_PHY_CAP3_DCM_MAX_CONST_RX_16_QAM,
				/* PHY 能力信息 [4]: 波束成形使能 */
				.phy_cap_info[4] = ATBM_IEEE80211_HE_PHY_CAP4_BEAMFORM_ENABLE /*IEEE80211_HE_PHY_CAP4_SU_BEAMFORMEE |
						ATBM_IEEE80211_HE_PHY_CAP4_BEAMFORMEE_MAX_STS_UNDER_80MHZ_4*/,
				/* PHY 能力信息 [6]: 波束成形 + 部分带宽扩展范围 */
				.phy_cap_info[6] = ATBM_IEEE80211_HE_PHY_CAP6_BEAMFORM_ENABLE|
					ATBM_IEEE80211_HE_PHY_CAP6_PARTIAL_BW_EXT_RANGE,
					//ATBM_IEEE80211_HE_PHY_CAP6_PARTIAL_BANDWIDTH_DL_MUMIMO,
				/* PHY 能力信息 [7]: HE SU/MU PPDU 4x LTF + 0.8us GI */
				.phy_cap_info[7] =ATBM_IEEE80211_HE_PHY_CAP7_HE_SU_MU_PPDU_4XLTF_AND_08_US_GI,
				/* PHY 能力信息 [8]: HE ER SU PPDU + DCM RU 484 */
				.phy_cap_info[8] =ATBM_IEEE80211_HE_PHY_CAP8_HE_ER_SU_PPDU_4XLTF_AND_08_US_GI |
					//ATBM_IEEE80211_HE_PHY_CAP8_20MHZ_IN_40MHZ_HE_PPDU_IN_2G|
					ATBM_IEEE80211_HE_PHY_CAP8_DCM_MAX_RU_484,
				/* PHY 能力信息 [9]: 1024-QAM 支持 + CQI 反馈 + 16us 包填充 */
				.phy_cap_info[9] =	ATBM_IEEE80211_HE_PHY_CAP9_NON_TRIGGERED_CQI_FEEDBACK |
				ATBM_IEEE80211_HE_PHY_CAP9_TX_1024_QAM_LESS_THAN_242_TONE_RU|
					ATBM_IEEE80211_HE_PHY_CAP9_RX_1024_QAM_LESS_THAN_242_TONE_RU|
					//ATBM_IEEE80211_HE_PHY_CAP9_RX_FULL_BW_SU_USING_MU_WITH_COMP_SIGB|
					ATBM_IEEE80211_HE_PHY_CAP9_RX_FULL_BW_SU_USING_MU_WITH_NON_COMP_SIGB|
					ATBM_IEEE80211_HE_PHY_CAP9_NOMIMAL_PKT_PADDING_16US,

				/*
				 * 其余 PHY 能力字节保持未设置
				 * 因为 DCM、波束成形、RU 和 PPE 门限信息
				 * 在此配置中部分支持
				 */
			},
			/* HE MCS-NSS 支持: 仅支持单空间流 */
			.he_mcs_nss_supp = {
				/* 80MHz: 支持 MCS 0-11 (单空间流) */
				.rx_mcs_80 = atbm_cpu_to_le16(0xfffe),
				.tx_mcs_80 = atbm_cpu_to_le16(0xfffe),
				/* 160MHz: 不支持 (0xffff = 不支持) */
				.rx_mcs_160 = atbm_cpu_to_le16(0xffff),
				.tx_mcs_160 = atbm_cpu_to_le16(0xffff),
				/* 80+80MHz: 不支持 (0xffff = 不支持) */
				.rx_mcs_80p80 = atbm_cpu_to_le16(0xffff),
				.tx_mcs_80p80 = atbm_cpu_to_le16(0xffff),
			},
			/* PPE (Packet Padding Extension) 门限配置
			 * 用于触发帧的 PPE 门限，定义了不同 RU 大小下的门限值
			 * 这些值根据芯片能力和测试场景进行配置
			 */
			.ppe_thres[0] =0x39,
			.ppe_thres[1] =0x1c,
			.ppe_thres[2] =0xc7,
			.ppe_thres[3] =0x71,
			.ppe_thres[4] =0x1c,
			.ppe_thres[5] =0x07,
		},
	},
#endif
};
#endif

#if SUPPORT_CRONUS
#include FIRM_FILE_NAME(CRONUS)
struct atbm_chip_priv chip_priv_cronus = {
	.support_5g = 0,
	.support_vht = 0,
	.disable_tb = 1,
	.chip_cat = CRONUS_CATEGORY,
	.fw_altobeam = {
		.hdr = (struct firmware_headr *)FIRM_HEADR_NAME(),
		.fw_iccm = FIRM_ICCM_NAME(),
		.fw_dccm = FIRM_DCCM_NAME(),
		.fw_sram = FIRM_SRAM_NAME(),
	},
	.reset_reg = 0x16101000,
	.reset_bit = 0x100,
	.mem_access_bit = 0,
	.hif_version = ATBM_TXRX_OPS_V0,
	.download_bs = DOWNLOAD_BLOCK_SIZE_V0,
	.tx_channel_num = 64,
	.rx_channel_num = 4,
};
#endif

#if SUPPORT_CRONUS_LITE
#include FIRM_FILE_NAME(CRONUS_LITE)
struct atbm_chip_priv chip_priv_cronus_lite = {
	.support_5g = 0,
	.support_vht = 1,
	.disable_tb = 0,
	.chip_cat = CLITE_CATEGORY,
	.fw_altobeam = {
		.hdr = (struct firmware_headr *)FIRM_HEADR_NAME(CRONUS_LITE),
		.fw_iccm = FIRM_ICCM_NAME(CRONUS_LITE),
		.fw_dccm = FIRM_DCCM_NAME(CRONUS_LITE),
		.fw_sram = FIRM_SRAM_NAME(CRONUS_LITE),
	},
	.reset_reg = 0x16101000,
	.reset_bit = 0x100,
	.mem_access_bit = 0,
	.hif_version = ATBM_TXRX_OPS_V0,
	.download_bs = DOWNLOAD_BLOCK_SIZE_V0,
	.tx_channel_num = 64,
	.rx_channel_num = 4,
};
#endif

#if SUPPORT_DEMETER
#include FIRM_FILE_NAME(DEMETER)
struct atbm_chip_priv chip_priv_demeter = {
       .support_5g = 0,
       .support_vht = 1,
       .disable_tb = 0,
       .chip_cat = DEMETER_CATEGORY,
       .fw_altobeam = {
               .hdr = (struct firmware_headr *)FIRM_HEADR_NAME(DEMETER),
               .fw_iccm = FIRM_ICCM_NAME(DEMETER),
               .fw_dccm = FIRM_DCCM_NAME(DEMETER),
               .fw_sram = FIRM_SRAM_NAME(DEMETER),
       },
       .reset_reg = 0x16101000,
       .reset_bit = 0x100,
       .mem_access_bit = 0,
       .hif_version = ATBM_TXRX_OPS_V0,
       .download_bs = DOWNLOAD_BLOCK_SIZE_V0,
       .tx_channel_num = 64,
       .rx_channel_num = 4,
};
#endif

#if SUPPORT_OCEANUS
/* IPRO7 port: the computed `#include FIRM_FILE_NAME(OCEANUS)` (a ##-pasted macro
 * needing a directive-level rescan) is rejected by this GCC. Pinned to the
 * OCEANUS SDIO WiFi-only blob for Phase 0; re-enable the macro for combo/BLE. */
#include "firmware_oceanus_sdio_notxconfirm.h"
struct atbm_chip_priv chip_priv_oceanus = {
	.support_5g = 1,
	.support_vht = 1,
	.disable_tb = 0,
	.chip_cat = OCEANUS_CATEGORY,
	.fw_altobeam = {
		.hdr = (struct firmware_headr *)FIRM_HEADR_NAME(OCEANUS),
		.fw_iccm = FIRM_ICCM_NAME(OCEANUS),
		.fw_dccm = FIRM_DCCM_NAME(OCEANUS),
		.fw_sram = FIRM_SRAM_NAME(OCEANUS),
	},
	.reset_reg = 0x16101000,
	.reset_bit = 0x100,
	.mem_access_bit = 0,
	.hif_version = ATBM_TXRX_OPS_V0,
	.download_bs = DOWNLOAD_BLOCK_SIZE_V0,
	.tx_channel_num = 64,
	.rx_channel_num = 4,
};
#endif

#if SUPPORT_OCEANUS_LITE
#include FIRM_FILE_NAME(OCEANUS_LITE)
struct atbm_chip_priv chip_priv_oceanus_lite = {
       .support_5g = 1,
       .support_vht = 1,
       .disable_tb = 0,
       .chip_cat = OLITE_CATEGORY,
       .fw_altobeam = {
               .hdr = (struct firmware_headr *)FIRM_HEADR_NAME(OCEANUS_LITE),
               .fw_iccm = FIRM_ICCM_NAME(OCEANUS_LITE),
               .fw_dccm = FIRM_DCCM_NAME(OCEANUS_LITE),
               .fw_sram = FIRM_SRAM_NAME(OCEANUS_LITE),
       },
       .reset_reg = 0x16101000,
       .reset_bit = 0x100,
       .mem_access_bit = 0,
       .hif_version = ATBM_TXRX_OPS_V0,
       .download_bs = DOWNLOAD_BLOCK_SIZE_V0,
       .tx_channel_num = 64,
       .rx_channel_num = 4,
};
#endif

#if SUPPORT_COEUS
#include FIRM_FILE_NAME(COEUS)
struct atbm_chip_priv chip_priv_coeus = {
	.support_5g = 1,
	.support_vht = 1,
	.disable_tb = 0,
	.chip_cat = COEUS_CATEGORY,
	.fw_altobeam = {
		.hdr = (struct firmware_headr *)FIRM_HEADR_NAME(6165),
		.fw_iccm = FIRM_ICCM_NAME(6165),
		.fw_dccm = FIRM_DCCM_NAME(6165),
		.fw_sram = FIRM_SRAM_NAME(6165),
	},
	.reset_reg = 0x16100074,
	.reset_bit = 0x80000000,
	.mem_access_bit = 0x1000000,
	.hif_version = ATBM_TXRX_OPS_V1,
	.download_bs = DOWNLOAD_BLOCK_SIZE_V1,
	.tx_channel_num = 64,
	.rx_channel_num = 32,
};
#endif
void  atbm_efuse_read_byte(struct atbmwifi_common *priv,atbm_uint32 byteIndex, atbm_uint32 *value)
{
	//HW_WRITE_REG(0x16b00000, (byteIndex<<8));
	//*value = HW_READ_REG(0x16b00004);	
	if(priv->sbus_ops && priv->sbus_ops->lock)
		priv->sbus_ops->lock(priv->sbus_priv);
	atbm_direct_write_reg_32(priv,0x16b00000, (byteIndex<<8));
	atbm_direct_read_reg_32(priv,0x16b00004,value);
	if(priv->sbus_ops && priv->sbus_ops->unlock)
		priv->sbus_ops->unlock(priv->sbus_priv);
}

atbm_uint32 atbm_efuse_read_bit(struct atbmwifi_common *priv,atbm_uint32 bitIndex)
{
	atbm_uint32	efuseBitIndex = bitIndex;
	atbm_uint32 byteIndex;
	atbm_uint32 value = 0;

	{
		byteIndex = efuseBitIndex / 8;
		atbm_efuse_read_byte(priv,byteIndex, &value);
	}
	value = value >> (efuseBitIndex % 8);
	value &= 0x1;
	return value;
}

int atbm_check_chip_config(struct atbmwifi_common *hw_priv)
{

	switch(hw_priv->chip_id)
	{

		case 0x5a:
			if(atbm_efuse_read_bit(hw_priv,14) == 1){
	
				if(atbm_efuse_read_bit(hw_priv,15) == 1){
					wifi_printk(WIFI_ALWAYS,"not support ldpc & ht40!\n");
					hw_priv->chip_version = CRONUS_NO_HT40_LDPC;
				}else{
					wifi_printk(WIFI_ALWAYS,"not support ht40!\n");
					hw_priv->chip_version = CRONUS_NO_HT40;
				}
			}else if (atbm_efuse_read_bit(hw_priv,13) == 1){
				wifi_printk(WIFI_ALWAYS,"not support BLE!\n");
				hw_priv->chip_version = CRONUS_NO_BLE;
				hw_priv->loader_ble = 0;
			}
			break;

		case 0x30:
			if(atbm_efuse_read_bit(hw_priv,14) == 1){
			
				
				if(atbm_efuse_read_bit(hw_priv,15) == 1){
					wifi_printk(WIFI_ALWAYS,"not support ldpc & ht40!\n");
					hw_priv->chip_version = OCEANUS_NO_HT40_LDPC;
				}else{
					wifi_printk(WIFI_ALWAYS,"not support ht40!\n");
					hw_priv->chip_version = OCEANUS_NO_HT40;
				}
			}else if (atbm_efuse_read_bit(hw_priv,13) == 1){
				wifi_printk(WIFI_ALWAYS,"not support BLE!\n");
				hw_priv->chip_version = OCEANUS_NO_BLE;
				hw_priv->loader_ble = 0;
			}

			if(atbm_efuse_read_bit(hw_priv,16) == 1){
				hw_priv->chip_version = OCEANUS_NO_WIFI6;
				wifi_printk(WIFI_ALWAYS,"not support wifi6\n");
			}
			
			break;
		default:
			break;

	}


	return 0;
}

void atbm_init_chip_priv(struct atbmwifi_common *hw_priv){
	atbm_uint32 chipver = 0;

	hw_priv->sbus_priv->cmd_func = INIT_SDIO_FUNCTION_4(hw_priv->sbus_priv->func);
	if(hw_priv->sbus_priv->cmd_func != ATBM_NULL){
		hw_priv->chip_ver = 1;
	}
	atbm_direct_read_reg_32(hw_priv,0x0acc017c,&chipver);
    chipver&=0xff;
	hw_priv->chip_id = chipver;
	wifi_printk(WIFI_ALWAYS,"atbm_init_chip_priv : chip_id = %x \n",hw_priv->chip_id);
	switch(chipver){
#if SUPPORT_CRONUS
		case 0x5a:
			hw_priv->chip_priv =  &chip_priv_cronus;
			break;
#endif
#if SUPPORT_CRONUS_LITE
		case 0x5c:
			hw_priv->chip_priv =  &chip_priv_cronus_lite;
			break;
#endif
#if SUPPORT_OCEANUS
		case 0x30:
			hw_priv->chip_priv = &chip_priv_oceanus;
			atbm_check_chip_config(hw_priv);
			break;
#endif
#if SUPPORT_COEUS
		case 0x31:
			hw_priv->chip_priv = &chip_priv_coeus;
			break;
#endif
#if SUPPORT_OCEANUS_LITE
        case 0x34:
            hw_priv->chip_priv = &chip_priv_oceanus_lite;
            break;
#endif
#if SUPPORT_DEMETER
        case 0x5e:
		case 0x5f:
            hw_priv->chip_priv = &chip_priv_demeter;
            break;
#endif
		default:
			ATBM_BUG_ON(1);
	}
	wifi_printk(WIFI_ALWAYS,"%s, chipver=0x%x, g_wifi_chip_type[%d]\n",__func__, chipver,hw_priv->chip_version );
}

/****************************************************************************/
/**
 * @brief WiFi 驱动主入口函数 - 完成硬件初始化、固件下载和接口创建
 *
 * @param sbus 总线私有数据结构指针 (SDIO/USB)
 * @return int 0:成功, 其他:失败错误码
 *
 * @note 这是 WiFi 驱动的核心初始化函数，按以下顺序完成初始化：
 *       1. 初始化全局硬件结构体
 *       2. 创建工作队列和定时器任务
 *       3. 初始化各种锁和队列
 *       4. 配置无线频段和信道参数
 *       5. 配置块确认(AMPDU)参数
 *       6. 初始化 WSM 命令缓冲区
 *       7. 注册中断处理函数(BH - Bottom Half)
 *       8. 下载固件到芯片
 *       9. 等待固件启动完成
 *       10. 初始化发送队列
 *       11. 获取 MAC 地址和 EFUSE 数据
 *       12. 初始化网络协议栈接口
 *       13. 创建虚拟网络接口(STA 和 AP 模式)
 */
int Atbmwifi_halEntry(struct sbus_priv *sbus)
{ 
	int Status;
	int if_id;
	int i;
	int ret = 0;

	struct atbmwifi_common *hw_priv;
	struct wsm_operational_mode mode;

	/* 初始化操作模式结构体 - 配置电源管理 */
	atbm_memset(&mode, 0, sizeof(struct wsm_operational_mode));
#if SUPPORT_LIGHT_SLEEP
	mode.power_mode = wsm_power_mode_quiescent;  /* 轻睡眠模式 */
#else
	mode.power_mode = wsm_power_mode_active;     /* 活跃模式 */
#endif
	mode.disableMoreFlagUsage = ATBM_TRUE;
	wifi_printk(WIFI_ALWAYS,"atbm: Atbmwifi_halEntry() <===\n");

	/* 步骤 1: 初始化静态和全局结构体 */
	net_device_ops_init();

	/* 步骤 2: 初始化全局硬件私有数据结构 */
	hw_priv = &g_hw_prv;
	atbm_memset(&g_hw_prv,0 ,sizeof(struct atbmwifi_common));

	/* 步骤 3: 根据总线类型设置操作函数 (USB/SDIO) */
#if ATBM_USB_BUS
	hw_priv->sbus_ops = &atbm_usb_sbus_ops;
#else	
	hw_priv->sbus_ops = &atbm_sdio_sbus_ops;
#endif
	hw_priv->sbus_priv = sbus;
	sbus->core = hw_priv;

	/* 步骤 4: 创建工作队列和定时器任务 */
	atbm_create_workqueue(hw_priv);      /* 创建工作队列线程 */
	atbm_create_timerTask(hw_priv);      /* 创建定时器处理线程 */
	/* 步骤 5: 初始化各种同步机制和队列 */
	atbm_os_mutexLockInit(&hw_priv->wsm_cmd_mux);           /* WSM 命令互斥锁 */
	ATBM_INIT_LIST_HEAD(&hw_priv->event_queue);             /* 事件队列 */
	ATBM_INIT_LIST_HEAD(&hw_priv->tx_urb_cmp);              /* TX URB 完成队列 */
	atbm_spin_lock_init(&hw_priv->event_queue_lock);        /* 事件队列自旋锁 */
	atbm_spin_lock_init(&hw_priv->tx_com_lock);             /* TX 完成自旋锁 */
	atbm_spin_lock_init(&hw_priv->rx_com_lock);             /* RX 完成自旋锁 */
	atbm_skb_queue_head_init(&hw_priv->rx_frame_queue);     /* RX 帧队列 */
	atbm_skb_queue_head_init(&hw_priv->rx_frame_free);      /* RX 空闲帧队列 */
	atbm_skb_queue_head_init(&hw_priv->tx_frame_queue);     /* TX 帧队列 */
	atbm_skb_queue_head_init(&hw_priv->tx_frame_free);      /* TX 空闲帧队列 */

#if ATBM_SDIO_BUS
	/* SDIO 总线特定: 初始化信道同步事件和工作项 */
	atbm_os_init_waitevent(&hw_priv->wsm_synchanl_done);
	hw_priv->wsm_sync_channl=atbm_init_work(hw_priv, wsm_sync_channl_reset,hw_priv);
#endif

#if CONFIG_ATBM_SUPPORT_CSA
	/* 信道切换公告(CSA)支持: 初始化 CSA 工作项和队列 */
	hw_priv->csa_work = atbm_init_work(hw_priv, atbm_ieee80211_csa_process_work, hw_priv);
	ATBM_INIT_LIST_HEAD(&hw_priv->csa_pending);
	ATBM_INIT_LIST_HEAD(&hw_priv->csa_req);
	hw_priv->next_csa_state = CSA_IDLE;
#endif

	/* 步骤 6: 配置无线频段和信道参数 */
	if(atbmwifi_band_2ghz.ht_cap.cap & ATBM_IEEE80211_HT_CAP_SUP_WIDTH_20_40) {
		hw_priv->channel_type = ATBM_NL80211_CHAN_HT40PLUS;  /* HT40+ 模式 */
	}
	else {
		hw_priv->channel_type = ATBM_NL80211_CHAN_HT20;      /* HT20 模式 */
	}
	hw_priv->bands[ATBM_IEEE80211_BAND_2GHZ] = &atbmwifi_band_2ghz;  /* 2.4G 频段 */
#if CONFIG_5G_SUPPORT
	hw_priv->bands[ATBM_IEEE80211_BAND_5GHZ] = &atbmwifi_band_5ghz;  /* 5G 频段 */
#endif

	/* 步骤 7: 配置重传参数 */
	hw_priv->short_frame_max_tx_count  = TEST_SHORT_RETRY_NUM;  /* 短帧重传次数 */
	hw_priv->long_frame_max_tx_count  = TEST_LONG_RETRY_NUM;    /* 长帧重传次数 */

	/* 步骤 8: 配置块确认(AMPDU)参数 */
#if ATBM_PKG_REORDER
	hw_priv->ba_tid_tx_mask = ATBMWIFI__TX_BLOCK_ACK_ENABLED_FOR_ALL_TID;  /* TX AMPDU 使能 */
#if CONFIG_WPA2_REINSTALL_CERTIFICATION
	/* WPA2 重装认证需要关闭 AMPDU RX，因为 RX PN 重排序问题 */
	hw_priv->ba_tid_rx_mask = ATBMWIFI__RX_BLOCK_ACK_DISABLED_FOR_ALL_TID;
#else
	hw_priv->ba_tid_rx_mask = ATBMWIFI__TX_BLOCK_ACK_ENABLED_FOR_ALL_TID;  /* RX AMPDU 使能 */
#endif
#else
	hw_priv->ba_tid_tx_mask = ATBMWIFI__RX_BLOCK_ACK_DISABLED_FOR_ALL_TID;
	hw_priv->ba_tid_rx_mask = ATBMWIFI__RX_BLOCK_ACK_DISABLED_FOR_ALL_TID;
#endif
	hw_priv->max_rx_aggregation_subframes = 64;  /* 最大 RX 聚合子帧数 */
	hw_priv->max_tx_aggregation_subframes = 64;  /* 最大 TX 聚合子帧数 */
	atbm_spin_lock_init(&hw_priv->aggr_lock);    /* 聚合锁 */

	/* 步骤 9: 初始化 WSM 回调函数 */
	hw_priv->wsm_cbc.scan_complete = atbmwifi_scan_complete_cb;    /* 扫描完成回调 */
	hw_priv->wsm_cbc.suspend_resume = atbm_suspend_resume;          /* 挂起/恢复回调 */

	/* 步骤 10: 初始化 WSM 命令缓冲区 */
	wsm_buf_init(&hw_priv->wsm_cmd_buf);
	atbm_spin_lock_init(&hw_priv->wsm_cmd.lock);
	atbm_os_init_waitevent(&hw_priv->wsm_startup_done);  /* 固件启动完成事件 */

	/* 步骤 11: 获取芯片特定配置 */
	atbm_init_chip_priv(hw_priv);

	/* 步骤 12: 根据芯片类型设置 USB 端点 */
	SET_USB_EP_NUM(hw_priv);

	/* 步骤 13: 注册 Bottom Half (BH) 中断处理线程 */
	/*Register bh*/
	ret=atbm_register_bh(hw_priv);
	if (ret!=0){
		goto AtbmMain_ERR;
	}
	/* 步骤 14: 设置固件下载块大小 */
	/*Set DownLoad firmware Blocksize*/
	SET_SDIO_DOWNLOAD_BLOCKSIZE(hw_priv, hw_priv->chip_ver, hw_priv->chip_priv->download_bs);

	/* 步骤 15: 下载固件到芯片 */
	/*Start download fw*/
	ret = atbm_load_firmware(hw_priv);
	if (ret){
		wifi_printk(WIFI_ALWAYS,"DownLoad FwErr,Pls check\n");
		goto AtbmMain_ERR;
	}
	/* 步骤 16: 初始化定时器 */
	/*Timer Init*/
	atbm_timer_init(hw_priv);

#if ATBM_SDIO_BUS
	/* 步骤 17: SDIO 特定 - 设置传输帧块大小并注册中断 */
	/*Set Transimit Frame Blocksize*/
	SET_SDIO_TRANSMIT_BLOCKSIZE(hw_priv);
	hw_priv->init_done = 1;
	/* Register Interrupt Handler */
	ret = hw_priv->sbus_ops->irq_subscribe(hw_priv->sbus_priv,
		(sbus_irq_handler)atbm_irq_handler, hw_priv);
	if (ret < 0) {
		wifi_printk(WIFI_IF,
			"%s: can't register IRQ handler.\n", __FUNCTION__);
	}
#endif

	/* 步骤 18: 等待固件启动完成 */
__wait_start_up:
	if (atbm_os_wait_event_timeout(&hw_priv->wsm_startup_done,2*ATBM_HZ) < 0) {
		if(!hw_priv->wsm_caps.firmwareReady){
			wifi_printk(WIFI_ALWAYS,"wait_event_interruptible_timeout wsm_startup_done timeout ERROR !!\n");
			goto AtbmMain_ERR;
		}
	}
	else {
		if(!hw_priv->wsm_caps.firmwareReady){
			wifi_printk(WIFI_ALWAYS,"atbm: Atbmwifi_halEntry(), FW is not ready(%dms).\n", atbm_GetOsTimeMs());
			goto __wait_start_up;
		}
	}
	atbm_firmware_init_check(hw_priv);

#if CONFIG_HE
	/* 步骤 19: HE (WiFi6) 特定配置 */
	if(hw_priv->chip_version == OCEANUS_NO_WIFI6){
		memset(&atbmwifi_band_2ghz.iftype_data,0,sizeof(atbmwifi_band_2ghz.iftype_data));
		memset(&atbmwifi_band_5ghz.iftype_data,0,sizeof(atbmwifi_band_5ghz.iftype_data));
	}
#endif

	/* 步骤 20: 初始化发送队列统计 */
	/*Queue stats init*/
	if (atbm_unlikely(atbmwifi_queue_stats_init(&hw_priv->tx_queue_stats,
			WLAN_LINK_ID_MAX,
			hw_priv))) {
		ret = -2;
		goto AtbmMain_ERR;
	}

	/* 步骤 21: 初始化发送队列 (4 个 AC 队列) */
	/*Queue init*/
	hw_priv->vif0_throttle = ATBM_WIFI_MAX_QUEUE_SZ-16;
	for (i = 0; i < 4; ++i) {
		if (atbm_unlikely(atbmwifi_queue_init(&hw_priv->tx_queue[i],
				&hw_priv->tx_queue_stats, i, ATBM_WIFI_MAX_QUEUE_SZ))) { 
			ret = -3;
			goto AtbmMain_ERR;
		}
	}

	/* 步骤 22: 为所有接口设置操作模式 */
	for (if_id = 0; if_id < _atbmwifi_get_nr_hw_ifaces(hw_priv); if_id++) { 
		wsm_set_operational_mode(hw_priv, &mode, if_id);
		/* Enable multi-TX confirmation */
		//wsm_use_multi_tx_conf(hw_priv, ATBM_TRUE, if_id);
	}

	/* 步骤 23: 从 EFUSE 获取 MAC 地址和其他信息 */
	/*get mac addr from efuse*/
	atbm_get_mac_address(hw_priv);
	/*Get efuse other Information*/
	atbm_get_efuse_data(hw_priv);

	/* 步骤 24: 初始化系统定时器 */
	/*Timer list initial*/
	atbm_wifi_ticks_timer_init();

	/* 步骤 25: 设置国家/地区信道 */
	/*Intial Support coutry */
	atbmwifi_ieee80211_channel_country(hw_priv,country_chinese);

	/* 步骤 26: 初始化网络协议栈接口 */
	/*mac80211 stack control & initial*/
	atbmwifi_netstack_init(hw_priv);

	/* 步骤 27: 创建虚拟网络接口 (STA 和 AP 模式) */
	/*Init the two virtul interface,Here need intial two netDev*/
	/*1 Add Station interface */
	atbm_wifi_add_interfaces(hw_priv,ATBM_NL80211_IFTYPE_STATION,"wlan0");
	/*2 Add P2p Interface*/
	atbm_wifi_add_interfaces(hw_priv,ATBM_NL80211_IFTYPE_AP,"p2p0");
	/*MacAddr Set only one times*/
	atbmwifi_setup_mac(hw_priv);

	/* 步骤 28: 初始化完成，开启 LMAC 日志 */
	/* Intial End Open Lmac log*/
	atbmwifi_enable_lmaclog(0);

#if ATBM_SUPPORT_BRIDGE
	atbm_brpool_init(hw_priv);
#endif
#if CONFIG_WIFI_BT_COMB
	ieee80211_ble_dev_int(hw_priv);
	atbm_ble_start();
#endif
	return ret;
AtbmMain_ERR:
	wifi_printk(WIFI_ALWAYS,"atbm: Atbmwifi_halEntry() <===error\n");
	return ret;
}
/**
 * @brief WiFi 驱动资源释放函数 - 清理所有分配的资源
 *
 * @param hw_priv 全局硬件私有数据结构指针
 *
 * @note 这是 WiFi 驱动的资源释放函数，按以下顺序完成清理：
 *       1. 遍历所有 VIF，停止 STA/AP 模式并移除接口
 *       2. 反初始化网络协议栈
 *       3. 停止 BLE 并注销设备（如果支持 WiFi/BT 共存）
 *       4. 注销 Bottom Half (BH) 中断处理
 *       5. 销毁工作队列
 *       6. 销毁 WPA 事件队列
 *       7. 释放定时器资源
 *       8. 释放发送队列
 *       9. 释放 Link Map 缓存
 *       10. 释放 WSM 命令缓冲区
 *       11. 删除等待事件
 */
atbm_void atbm_core_release(struct atbmwifi_common *hw_priv)
{
	struct atbmwifi_vif *priv;
	int queue_id;
	int i;

	/* 步骤 1: 遍历所有 VIF，停止 STA/AP 模式并移除接口 */
	atbm_for_each_vif(hw_priv,priv,i){
		if(priv== NULL){
			continue;
		}

		//tcp_opt->net_disable(priv->ndev);
		/*Flush the queue */
		//__atbm_flush(hw_priv, ATBM_TRUE, priv->if_id);
		if(priv->enabled){
			/*Free wpa supplicant & hostapd mem*/
			if(atbmwifi_is_sta_mode(priv->iftype))
				atbmwifi_stop_sta(priv);
			if(atbmwifi_is_ap_mode(priv->iftype))
				atbmwifi_stop_ap(priv);
		}
		/*Free netdev*/
		atbm_wifi_remove_interfaces(priv);
	}

	/* 步骤 2: 反初始化网络协议栈 */
	//hmac_rc_deinit();
	atbmwifi_netstack_deinit();

#if CONFIG_WIFI_BT_COMB
	/* 步骤 3: 停止 BLE 并注销设备（WiFi/BT 共存支持） */
	atbm_ble_stop();
	ieee80211_ble_dev_deregister(hw_priv);
#endif

	/* 步骤 4: 注销 Bottom Half (BH) 中断处理 */
	/*Destory the atbm_bh|*/
	atbm_unregister_bh(hw_priv);

	/* 步骤 5: 销毁工作队列 */
	/*Destory the atbm_workQueue*/
	atbm_destory_task_work(hw_priv);

	/* 步骤 6: 销毁 WPA 事件队列 */
	/*Destory the atbm_wpa_event*/
	atbmwifi_wpa_event_destory();

	/* 步骤 7: 释放定时器资源 */
	/*Free atbm timer*/
	atbm_timer_free(hw_priv);

	/* 步骤 8: 释放发送队列 (4 个 AC 队列) */
	/*Free TxQueue Timer*/
	for (queue_id = 0; queue_id < 4; ++queue_id){
		atbmwifi_queue_deinit(&hw_priv->tx_queue[queue_id]);
	}

	/* 步骤 9: 释放 Link Map 缓存 */
	for (i = 0; i < ATBM_WIFI_MAX_VIFS; i++) {
		if (hw_priv->tx_queue_stats.link_map_cache[i]) {
			atbm_kfree(hw_priv->tx_queue_stats.link_map_cache[i]);
			hw_priv->tx_queue_stats.link_map_cache[i] = ATBM_NULL;
		}
	}

	/* 步骤 10: 释放 WSM 命令缓冲区 */
	wsm_buf_deinit(&hw_priv->wsm_cmd_buf);

	/* 步骤 11: 删除等待事件 */
	atbm_os_delete_waitevent(&hw_priv->wsm_synchanl_done);
	atbm_os_delete_waitevent(&hw_priv->wsm_startup_done);
}
