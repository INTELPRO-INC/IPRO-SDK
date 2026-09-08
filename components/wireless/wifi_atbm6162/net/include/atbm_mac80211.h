/**************************************************************************************************************
 * altobeam RTOS wifi hmac source code 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 *
 *  The source code contains proprietary information of AltoBeam, and shall not be distributed, 
 *  copied, reproduced, or disclosed in whole or in part without prior written permission of AltoBeam.
*****************************************************************************************************************/
#ifndef ATBMWIFI_NET_H
#define ATBMWIFI_NET_H
#include "atbm_type.h"
enum atbm_nl80211_iftype {
	ATBM_NL80211_IFTYPE_STATION,                          
	ATBM_NL80211_IFTYPE_AP,                               
	ATBM_NL80211_IFTYPE_ADHOC,                            
	ATBM_NL80211_IFTYPE_MONITOR,                          
	ATBM_NL80211_IFTYPE_P2P_CLIENT,                       
	ATBM_NL80211_IFTYPE_P2P_GO,                           
	/* keep last */                                       
	ATBM_NUM_NL80211_IFTYPES,                             
	ATBM_NL80211_IFTYPE_MAX = ATBM_NUM_NL80211_IFTYPES - 1
};
/**
 * enum nl80211_band - Frequency band
 * @NL80211_BAND_2GHZ: 2.4 GHz ISM band
 * @NL80211_BAND_5GHZ: around 5 GHz band (4.9 - 5.7 GHz)
 */
enum atbm_nl80211_band {
	ATBM_NL80211_BAND_2GHZ,
	ATBM_NL80211_BAND_5GHZ,
};
/**
 * enum atbmwifi_ieee80211_band - supported frequency bands
 *
 * The bands are assigned this way because the supported
 * bitrates differ in these bands.
 *
 * @ATBM_IEEE80211_BAND_2GHZ: 2.4GHz ISM band
 * @IEEE80211_BAND_5GHZ: around 5GHz band (4.9-5.7)
 * @ATBM_IEEE80211_NUM_BANDS: number of defined bands
 */
enum atbmwifi_ieee80211_band {
	ATBM_IEEE80211_BAND_2GHZ = ATBM_NL80211_BAND_2GHZ,
	ATBM_IEEE80211_BAND_5GHZ =ATBM_NL80211_BAND_5GHZ,
	/* keep last */
	ATBM_IEEE80211_NUM_BANDS
};
/**
 * enum atbmwifi_ieee80211_ac_numbers - AC numbers as used in mac80211
 * @IEEE80211_AC_VO: voice
 * @IEEE80211_AC_VI: video
 * @IEEE80211_AC_BE: best effort
 * @IEEE80211_AC_BK: background
 */
enum atbmwifi_ieee80211_ac_numbers {
	ATBM_IEEE80211_AC_VO		= 0,
	ATBM_IEEE80211_AC_VI		= 1,
	ATBM_IEEE80211_AC_BE		= 2,
	ATBM_IEEE80211_AC_BK		= 3,
};
/* Access Catagory Indices*/
#define ATBM_D11_ACI_AC_BE		       0	/* Best Effort*/
#define ATBM_D11_ACI_AC_BK		       1	/* Background*/
#define ATBM_D11_ACI_AC_VI		       2	/* Video*/
#define ATBM_D11_ACI_AC_VO		       3	/* Voice*/

/**
 * enum ieee80211_frame_release_type - frame release reason
 * @IEEE80211_FRAME_RELEASE_PSPOLL: frame released for PS-Poll
 * @IEEE80211_FRAME_RELEASE_UAPSD: frame(s) released due to
 *	frame received on trigger-enabled AC
 */
enum atbm_ieee80211_frame_release_type {
	ATBM_IEEE80211_FRAME_RELEASE_PSPOLL,
	ATBM_IEEE80211_FRAME_RELEASE_UAPSD,
};
/*Ap ps*/
#define ATBM_TOTAL_MAX_TX_BUFFER 512
#define ATBM_STA_MAX_TX_BUFFER 64
#define ATBM_AP_MAX_BC_BUFFER 128
/* Minimum buffered frame expiry time. If STA uses listen interval that is
 * smaller than this value, the minimum value here is used instead. */
#define ATBM_STA_TX_BUFFER_EXPIRE (5 * 1000) //ms
#define ATBM_STA_INFO_CLEANUP_INTERVAL (4 * 1000) //ms
/* there are 40 bytes if you don't need the rateset to be kept */
#define ATBM_IEEE80211_TX_INFO_DRIVER_DATA_SIZE 40

/* if you do need the rateset, then you have less space */
#define ATBM_IEEE80211_TX_INFO_RATE_DRIVER_DATA_SIZE 24


/* maximum number of rate stages */
#define ATBM_IEEE80211_TX_MAX_RATES	5

#define ATBM_IEEE80211_TX_CTL_STBC_SHIFT		23
/* maximum number of rate stages */

#define ATBM_IEEE80211_NUM_ACS	4

/**
 * struct ieee80211_mu_edca_param_set - MU EDCA Parameter Set element
 *
 * This structure is the "MU EDCA Parameter Set element" fields as
 * described in P802.11ax_D4.0 section 9.4.2.245
 */
struct atbm_ieee80211_mu_edca_param_set {
	atbm_uint8 mu_qos_info;
	struct atbm_ieee80211_he_mu_edca_param_ac_rec ac_be;
	struct atbm_ieee80211_he_mu_edca_param_ac_rec ac_bk;
	struct atbm_ieee80211_he_mu_edca_param_ac_rec ac_vi;
	struct atbm_ieee80211_he_mu_edca_param_ac_rec ac_vo;
} atbm_packed;

/**
 * struct atbm_ieee80211_tx_queue_params - transmit queue configuration
 *
 * The information provided in this structure is required for QoS
 * transmit queue configuration. Cf. IEEE 802.11 7.3.2.29.
 *
 * @aifs: arbitration interframe space [0..255]
 * @cw_min: minimum contention window [a value of the form
 *	2^n-1 in the range 1..32767]
 * @cw_max: maximum contention window [like @cw_min]
 * @txop: maximum burst time in units of 32 usecs, 0 meaning disabled
 * @uapsd: is U-APSD mode enabled for the queue
 */
struct atbm_ieee80211_tx_queue_params {
	atbm_uint16 txop;
	atbm_uint16 cw_min;
	atbm_uint16 cw_max;
	atbm_uint8 aifs;
	ATBM_BOOL acm;
	ATBM_BOOL uapsd;
    ATBM_BOOL mu_edca;
	struct atbm_ieee80211_he_mu_edca_param_ac_rec mu_edca_param_rec;
};

struct atbm_tx_rc_output
{
    atbm_uint32 tx_rate_sets;
	atbm_uint8	tx_max_rate;
	atbm_uint8	force_policyid;  // bit[4:6] set forced policy id
	atbm_uint16 tx_rc_flag;	
};

struct atbmwifi_ieee80211_tx_rate_control{
	struct atbm_tx_rc_output txrc_out;
	atbm_uint8	tx_update_rate:1,  
                gso : 1,
	            sample_pkt_flag : 1,
	            rsv : 1;  //bit[4:6]
	atbm_uint8 ampdu_ack_len;
	atbm_uint8 ampdu_len;
	atbm_uint8 rts_cts_rate_idx;
	int ack_signal;
	/* 15 bytes free */
};
struct atbmwifi_ieee80211_tx_info{	 
	atbm_uint32	rate_11g:1,
	    ht:1,
		short_preamble:1,
		use_short_slot:1,
		ht_40M:1,	
		greenfield:1,
		b_net:1,
		b_multi,
		short_gi:1,
		ht_ldpc:1,
		he:1,
		he_dcm:1,
		he_ldpc:1,
		he_not_40M:1,
		he_ER_106tone:1,
		he_pad8us:1,
		b_eapol:1,
		offset:1;
	atbm_uint8	reserve[2];
	atbm_uint8	link_id;
	atbm_uint8  hw_rate_id;
    atbm_uint8 lower;
    atbm_uint8  manual;
    atbm_uint8  manual_rate;	
	atbm_uint8  txpower;
	int flags;
#if ATBM_QUEUE_8023_FRAMES
	struct atbmwifi_sta_priv *sta_priv;
	atbm_uint8 hdrlen;
	atbm_uint8 ivlen; /*include dummy*/
	atbm_uint8 icvlen;
	atbm_uint8 qos;
	atbm_uint8 *smac;
	atbm_uint16 tid;
	atbm_uint16 align;
#endif
	/* only needed before rate control */
	struct atbmwifi_ieee80211_tx_rate_control control;
};
#define TX_CONTINUE	0
#define TX_QUEUED	1

#define ATBM_IEEE80211_TX_UNICAST		ATBM_BIT(1)
#define ATBM_IEEE80211_TX_PS_BUFFERED	ATBM_BIT(2)

struct atbmwifi_txinfo {
	struct atbm_buff *skb;
	unsigned queue;
	struct atbmwifi_ieee80211_tx_info *tx_info;
	//const struct atbmwifi_ieee80211_rate *rate;
	struct atbmwifi_ieee80211_hdr *hdr;
	atbm_size_t hdrlen;
	const atbm_uint8 *da;
	atbm_uint32 enc_flags;
	struct atbmwifi_sta_priv *sta_priv;
	struct atbmwifi_txpriv txpriv;
};

struct atbmwifi_ieee80211_rx_status {
	//u64 mactime;
	enum atbmwifi_ieee80211_band band;
	atbm_uint8 antenna;
	atbm_uint16 freq;

	
	atbm_int8 signal;
	atbm_uint8 rate_idx;
	/*
	 * Index into sequence numbers array, 0..16
	 * since the last (16) is used for non-QoS,
	 * will be 16 on non-QoS frames.
	 */
	atbm_uint8 seqno_idx;

	/*
	 * Index into the security IV/PN arrays, 0..16
	 * since the last (16) is used for CCMP-encrypted
	 * management frames, will be set to 16 on mgmt
	 * frames and 0 on non-QoS frames.
	 */
	//atbm_uint8 security_idx;
	//atbm_uint8 reserved[3];
	atbm_uint8 link_id;
	
	int flag;

	atbm_int16	rx_SNR;
};
enum mac80211_rx_flags {
	ATBM_RX_FLAG_MMIC_ERROR	= 1<<0,    
	ATBM_RX_FLAG_DECRYPTED	= 1<<1,      
	ATBM_RX_FLAG_MMIC_STRIPPED	= 1<<3,  
	ATBM_RX_FLAG_IV_STRIPPED	= 1<<4,    
	ATBM_RX_FLAG_FAILED_FCS_CRC	= 1<<5,
	ATBM_RX_FLAG_FAILED_PLCP_CRC = 1<<6,
	ATBM_RX_FLAG_MACTIME_MPDU	= 1<<7,  
	ATBM_RX_FLAG_SHORTPRE	= 1<<8,      
	ATBM_RX_FLAG_HT		= 1<<9,          
	ATBM_RX_FLAG_40MHZ		= 1<<10,       
	ATBM_RX_FLAG_SHORT_GI	= 1<<11,     
	ATBM_RX_FLAG_AMSDU	= 1<<12,  
	ATBM_RX_FLAG_11G	= 1<<13, 
	RX_FLAG_HE_ER	= 1<<14, 
	ATBM_RX_FLAG_HE	= 1<<15,
	ATBM_RX_FLAG_VHT 	= 1<<16, 
};

#define ATBM_BASIC_RATE_MASK 0x80
enum rate_flags {
	ATBM_IEEE80211_RT_11B 			=ATBM_BIT(0),
	ATBM_IEEE80211_RT_11B_SHORT		= ATBM_BIT(1),
	ATBM_IEEE80211_RT_11G		= ATBM_BIT(2),
	ATBM_IEEE80211_RT_11N		= ATBM_BIT(3),
	ATBM_IEEE80211_RATE_ERP_G	= ATBM_BIT(4),
	ATBM_IEEE80211_RT_BASIC		= ATBM_BASIC_RATE_MASK,/*must be BIT7*/
};
/**
 * struct atbmwifi_ieee80211_rate - bitrate definition
 *
 * This structure describes a bitrate that an 802.11 PHY can
 * operate with. The two values @hw_value and @hw_value_short
 * are only for driver use when pointers to this structure are
 * passed around.
 *
 * @flags: rate-specific flags
 * @bitrate: bitrate in units of 100 Kbps
 * @hw_value: driver/hardware value for this rate
 * @hw_value_short: driver/hardware value for this rate when
 *	short preamble is used
 */
struct atbmwifi_ieee80211_rate {
	//atbm_uint32 flags;
	atbm_uint16 bitrate;
	atbm_uint8 hw_value;
	atbm_uint8 rate_flag;
}atbm_packed;


enum mac80211_tx_control_flags {
	ATBM_IEEE80211_TX_CTL_PS_POLL_RESP		= ATBM_BIT(0),
	ATBM_IEEE80211_TX_CTL_NOT_ASSIGN_SEQ	= ATBM_BIT(1),
	ATBM_IEEE80211_TX_CTL_NO_ACK			= ATBM_BIT(2),
	ATBM_IEEE80211_TX_CTL_CLEAR_PS_FILT		= ATBM_BIT(3),
	ATBM_IEEE80211_TX_CTL_FIRST_FRAGMENT		= ATBM_BIT(4),
	ATBM_IEEE80211_TX_CTL_SEND_AFTER_DTIM	= ATBM_BIT(5),
	ATBM_IEEE80211_TX_CTL_AMPDU			= ATBM_BIT(6),
	ATBM_IEEE80211_TX_CTL_INJECTED		= ATBM_BIT(7),
	ATBM_IEEE80211_TX_STAT_TX_FILTERED		= ATBM_BIT(8),
	ATBM_IEEE80211_TX_STAT_ACK			= ATBM_BIT(9),
	ATBM_IEEE80211_TX_STAT_AMPDU			= ATBM_BIT(10),
	ATBM_IEEE80211_TX_STAT_AMPDU_NO_BACK		= ATBM_BIT(11),
	ATBM_IEEE80211_TX_CTL_RATE_CTRL_PROBE	= ATBM_BIT(12),
	ATBM_IEEE80211_TX_INTFL_NEED_TXPROCESSING	= ATBM_BIT(14),
	ATBM_IEEE80211_TX_INTFL_RETRIED		= ATBM_BIT(15),
	ATBM_IEEE80211_TX_INTFL_DONT_ENCRYPT		= ATBM_BIT(16),
	ATBM_IEEE80211_TX_CTL_POLL_RESPONSE		= ATBM_BIT(17),
	ATBM_IEEE80211_TX_CTL_MORE_FRAMES		= ATBM_BIT(18),
	ATBM_IEEE80211_TX_INTFL_RETRANSMISSION	= ATBM_BIT(19),
	ATBM_IEEE80211_TX_HANDSHAKE			    = ATBM_BIT(20),
	/* hole at 20, use later */
	ATBM_IEEE80211_TX_INTFL_NL80211_FRAME_TX	= ATBM_BIT(21),
	ATBM_IEEE80211_TX_FIXED_RATE			= ATBM_BIT(22),
	ATBM_IEEE80211_TX_CTL_STBC			= ATBM_BIT(23) | ATBM_BIT(24),
	ATBM_IEEE80211_TX_CTL_TX_OFFCHAN		= ATBM_BIT(25),
	ATBM_IEEE80211_TX_INTFL_TKIP_MIC_FAILURE	= ATBM_BIT(26),
	ATBM_IEEE80211_TX_CTL_NO_CCK_RATE		= ATBM_BIT(27),
	ATBM_IEEE80211_TX_STATUS_EOSP		= ATBM_BIT(28),
	ATBM_IEEE80211_TX_CTL_USE_MINRATE		= ATBM_BIT(29),
	ATBM_IEEE80211_TX_CTL_DONTFRAG		= ATBM_BIT(30),
	ATBM_IEEE80211_TX_AP_HANDLE_STATUS	= ATBM_BIT(31),
};

#define ATBM_IEEE80211_TX_CTL_STBC_SHIFT		23
/* maximum number of rate stages */
//#define ATBM_IEEE80211_TX_STAT_ACK	5



/**
 * enum mac80211_rate_control_flags - per-rate flags set by the
 *	Rate Control algorithm.
 *
 * These flags are set by the Rate control algorithm for each rate during tx,
 * in the @flags member of struct ieee80211_tx_rate.
 *
 * @ATBM_IEEE80211_TX_RC_USE_RTS_CTS: Use RTS/CTS exchange for this rate.
 * @ATBM_IEEE80211_TX_RC_USE_CTS_PROTECT: CTS-to-self protection is required.
 *	This is set if the current BSS requires ERP protection.
 * @IEEE80211_TX_RC_USE_SHORT_PREAMBLE: Use short preamble.
 * @IEEE80211_TX_RC_MCS: HT rate.
 * @IEEE80211_TX_RC_GREEN_FIELD: Indicates whether this rate should be used in
 *	Greenfield mode.
 * @IEEE80211_TX_RC_40_MHZ_WIDTH: Indicates if the Channel Width should be 40 MHz.
 * @IEEE80211_TX_RC_DUP_DATA: The frame should be transmitted on both of the
 *	adjacent 20 MHz channels, if the current channel type is
 *	ATBM_NL80211_CHAN_HT40MINUS or ATBM_NL80211_CHAN_HT40PLUS.
 * @IEEE80211_TX_RC_SHORT_GI: Short Guard interval should be used for this rate.
 */
enum mac80211_rate_control_flags {
	ATBM_IEEE80211_TX_RC_VHT_LDPC		= ATBM_BIT(0),
	ATBM_IEEE80211_TX_RC_USE_CTS_PROTECT		= ATBM_BIT(1),
	ATBM_IEEE80211_TX_RC_USE_SHORT_PREAMBLE	= ATBM_BIT(2),

	/* rate index is an MCS rate number instead of an index */
	ATBM_IEEE80211_TX_RC_MCS			= ATBM_BIT(3),
	ATBM_IEEE80211_TX_RC_SHORT_GI_40M		= ATBM_BIT(4),
	ATBM_IEEE80211_TX_RC_40_MHZ_WIDTH		= ATBM_BIT(5),
	ATBM_IEEE80211_TX_RC_DUP_DATA		= ATBM_BIT(6),
	ATBM_IEEE80211_TX_RC_SHORT_GI		= ATBM_BIT(7),
	ATBM_IEEE80211_TX_RC_HT_LDPC			= ATBM_BIT(8),
	ATBM_IEEE80211_TX_RC_HE_LDPC			= ATBM_BIT(9),
	ATBM_IEEE80211_TX_RC_HE_DCM			= ATBM_BIT(10),
	ATBM_IEEE80211_TX_RC_HE_ER			= ATBM_BIT(11),
	ATBM_IEEE80211_TX_RC_40M_TO_20M		= ATBM_BIT(12), //support 40M, but Tx rate set 20M 
	ATBM_IEEE80211_TX_RC_20_MHZ_WIDTH    = ATBM_BIT(13), //support 20M, NOT HT40 only mode
	ATBM_IEEE80211_TX_RC_HE_SUPPORT_DCM	= ATBM_BIT(14),  //support DCM
	ATBM_IEEE80211_TX_RC_SHORT_GI_20M	= ATBM_BIT(15),  //should not add flag anymore, 16bit
    
};


typedef enum{
	RATE_B = 0,
	RATE_G,
	RATE_HT,
	RATE_HE,
	RATE_HE_ER,
	RATE_VHT,
	RATE_UNKNOWN
}RATE_MODE_TYPE;

#define RATE_MODE_MASK 0x07



#define TX_RATE_FIXED           ATBM_BIT(0)  //tx use fixed rate
#define TX_RATE_FIXED_AUTO_DOWN ATBM_BIT(1)  //tx use fixed rate but will auto down with retry
#define TX_RATE_FIXED_STATIC    ATBM_BIT(2)  //tx use fixed rate have rate statics
#define ATBM_HT_CAP_INFO_LDPC_CODING_CAP		((atbm_uint16) ATBM_BIT(0))
#define ATBM_HT_CAP_INFO_SUPP_CHANNEL_WIDTH_SET	((atbm_uint16) ATBM_BIT(1))
#define ATBM_HT_CAP_INFO_SMPS_MASK			((atbm_uint16) (ATBM_BIT(2) | ATBM_BIT(3)))
#define ATBM_HT_CAP_INFO_SMPS_STATIC			((atbm_uint16) 0)
#define ATBM_HT_CAP_INFO_SMPS_DYNAMIC		((atbm_uint16) ATBM_BIT(2))
#define ATBM_HT_CAP_INFO_SMPS_DISABLED		((atbm_uint16) (ATBM_BIT(2) | ATBM_BIT(3)))
#define ATBM_HT_CAP_INFO_GREEN_FIELD			((atbm_uint16) ATBM_BIT(4))
#define ATBM_HT_CAP_INFO_SHORT_GI20MHZ		((atbm_uint16) ATBM_BIT(5))
#define ATBM_HT_CAP_INFO_SHORT_GI40MHZ		((atbm_uint16) ATBM_BIT(6))
#define ATBM_HT_CAP_INFO_TX_STBC			    ((atbm_uint16) ATBM_BIT(7))
#define ATBM_HT_CAP_INFO_RX_STBC_MASK		((atbm_uint16) (ATBM_BIT(8) | ATBM_BIT(9)))
#define ATBM_HT_CAP_INFO_RX_STBC_1			((atbm_uint16) ATBM_BIT(8))
#define ATBM_HT_CAP_INFO_RX_STBC_12			((atbm_uint16) ATBM_BIT(9))
#define ATBM_HT_CAP_INFO_RX_STBC_123			((atbm_uint16) (ATBM_BIT(8) | ATBM_BIT(9)))
#define ATBM_HT_CAP_INFO_DELAYED_BA			((atbm_uint16) ATBM_BIT(10))
#define ATBM_HT_CAP_INFO_MAX_AMSDU_SIZE		((atbm_uint16) ATBM_BIT(11))
#define ATBM_HT_CAP_INFO_DSSS_CCK40MHZ		((atbm_uint16) ATBM_BIT(12))
#define ATBM_HT_CAP_INFO_PSMP_SUPP			((atbm_uint16) ATBM_BIT(13))
#define ATBM_HT_CAP_INFO_40MHZ_INTOLERANT		((atbm_uint16) ATBM_BIT(14))
#define ATBM_HT_CAP_INFO_LSIG_TXOP_PROTECT_SUPPORT	((atbm_uint16) ATBM_BIT(15))


#define ATBM_EXT_HT_CAP_INFO_PCO			((atbm_uint16) ATBM_BIT(0))
#define ATBM_EXT_HT_CAP_INFO_TRANS_TIME_OFFSET	1
#define ATBM_EXT_HT_CAP_INFO_MCS_FEEDBACK_OFFSET	8
#define ATBM_EXT_HT_CAP_INFO_HTC_SUPPORTED		((atbm_uint16) ATBM_BIT(10))
#define ATBM_EXT_HT_CAP_INFO_RD_RESPONDER		((atbm_uint16) ATBM_BIT(11))


enum ieee80211_hw_flags {
	ATBM_IEEE80211_HW_HAS_RATE_CONTROL			= 1<<0,
	ATBM_IEEE80211_HW_RX_INCLUDES_FCS			= 1<<1,
	ATBM_IEEE80211_HW_HOST_BROADCAST_PS_BUFFERING	= 1<<2,
	ATBM_IEEE80211_HW_2GHZ_SHORT_SLOT_INCAPABLE		= 1<<3,
	ATBM_IEEE80211_HW_2GHZ_SHORT_PREAMBLE_INCAPABLE	= 1<<4,
	ATBM_IEEE80211_HW_SIGNAL_UNSPEC			= 1<<5,
	ATBM_IEEE80211_HW_SIGNAL_DBM				= 1<<6,
	ATBM_IEEE80211_HW_NEED_DTIM_PERIOD			= 1<<7,
	ATBM_IEEE80211_HW_SPECTRUM_MGMT			= 1<<8,
	ATBM_IEEE80211_HW_AMPDU_AGGREGATION			= 1<<9,
	ATBM_IEEE80211_HW_SUPPORTS_PS			= 1<<10,
	ATBM_IEEE80211_HW_PS_NULLFUNC_STACK			= 1<<11,
	ATBM_IEEE80211_HW_SUPPORTS_DYNAMIC_PS		= 1<<12,
	ATBM_IEEE80211_HW_MFP_CAPABLE			= 1<<13,
	ATBM_IEEE80211_HW_BEACON_FILTER			= 1<<14,
	ATBM_IEEE80211_HW_SUPPORTS_STATIC_SMPS		= 1<<15,
	ATBM_IEEE80211_HW_SUPPORTS_DYNAMIC_SMPS		= 1<<16,
	ATBM_IEEE80211_HW_SUPPORTS_UAPSD			= 1<<17,
	ATBM_IEEE80211_HW_REPORTS_TX_ACK_STATUS		= 1<<18,
	ATBM_IEEE80211_HW_CONNECTION_MONITOR			= 1<<19,
	ATBM_IEEE80211_HW_SUPPORTS_CQM_RSSI			= 1<<20,
	ATBM_IEEE80211_HW_SUPPORTS_PER_STA_GTK		= 1<<21,
	ATBM_IEEE80211_HW_AP_LINK_PS				= 1<<22,
	ATBM_IEEE80211_HW_TX_AMPDU_SETUP_IN_HW		= 1<<23,
	ATBM_IEEE80211_HW_SUPPORTS_CQM_BEACON_MISS		= 1<<24,
	ATBM_IEEE80211_HW_SUPPORTS_CQM_TX_FAIL		= 1<<25,
	ATBM_IEEE80211_HW_SUPPORTS_P2P_PS			= 1<<26,
	ATBM_IEEE80211_HW_SUPPORTS_MULTI_CHANNEL		= 1<<27,
	ATBM_IEEE80211_HW_QUEUE_CONTROL			= 1<<28,

};

/* Rx MCS bitmask is in the first 77 bits of supported_mcs_set */
#define ATBM_IEEE80211_HT_MCS_MASK_LEN 10

struct atbmwifi_ieee80211_ht_capabilities {
	atbm_uint16 ht_capabilities_info;
	atbm_uint8 a_mpdu_params;
	atbm_uint8 supported_mcs_set[16];
	atbm_uint16 ht_extended_capabilities;
	atbm_uint32 tx_bf_capability_info;
	atbm_uint8 asel_capabilities;
}atbm_packed;

struct atbmwifi_ieee80211_ht_operation {
	atbm_uint8 control_chan;
	atbm_uint8 ht_param;
	atbm_uint16 operation_mode;
	atbm_uint16 stbc_param;
	atbm_uint8 basic_set[16];
}atbm_packed;

static inline atbm_uint16 atbm_is_ht(int channleType)
{
	return channleType != ATBM_NL80211_CHAN_NO_HT;
}


/**
 * struct atbmwifi_ieee80211_channel - channel definition
 *
 * This structure describes a single channel for use
 * with cfg80211.
 *
 * @center_freq: center frequency in MHz
 * @hw_value: hardware-specific value for the channel
 * @flags: channel flags from &enum atbmwifi_ieee80211_channel_flags.
 * @orig_flags: channel flags at registration time, used by regulatory
 *	code to support devices with additional restrictions
 * @band: band this channel belongs to.
 * @max_antenna_gain: maximum antenna gain in dBi
 * @max_power: maximum transmission power (in dBm)
 * @beacon_found: helper to regulatory code to indicate when a beacon
 *	has been found on this channel. Use regulatory_hint_found_beacon()
 *	to enable this, this is useful only on 5 GHz band.
 * @orig_mag: internal use
 * @orig_mpwr: internal use
 */
struct atbmwifi_ieee80211_channel {
	//enum atbmwifi_ieee80211_band band;
	atbm_int8 max_power;
	atbm_uint8 hw_value;
	//ATBM_BOOL beacon_found;
	//atbm_uint16 center_freq;
	atbm_uint16 flags;
	atbm_uint16 center_freq;
	//int max_antenna_gain;
	//atbm_uint32 orig_flags;
	//int orig_mag, orig_mpwr;
};
enum atbm_ieee80211_channel_flags {
	ATBM_IEEE80211_CHAN_DISABLED		= 1<<0,
	ATBM_IEEE80211_CHAN_PASSIVE_SCAN	= 1<<1,
	ATBM_IEEE80211_CHAN_NO_IBSS		= 1<<2,
	ATBM_IEEE80211_CHAN_RADAR		= 1<<3,
	ATBM_IEEE80211_CHAN_NO_HT40PLUS	= 1<<4,
	ATBM_IEEE80211_CHAN_NO_HT40MINUS	= 1<<5,

};
/**
 * struct atbmwifi_ieee80211_sta_ht_cap - STA's HT capabilities
 *
 * This structure describes most essential parameters needed
 * to describe 802.11n HT capabilities for an STA.
 *
 * @ht_supported: is HT supported by the STA
 * @cap: HT capabilities map as described in 802.11n spec
 * @ampdu_factor: Maximum A-MPDU length factor
 * @ampdu_density: Minimum A-MPDU spacing
 * @mcs: Supported MCS rates
 */
struct atbmwifi_ieee80211_sta_ht_cap {
	atbm_uint16 cap; /* use IEEE80211_HT_CAP_ */
	ATBM_BOOL ht_supported;
	atbm_uint8 ampdu_factor;

	atbm_uint8 ampdu_density;
	atbm_uint8 reserved[3];
	struct atbmwifi_ieee80211_mcs_info mcs;
}atbm_packed;

/**
 * struct atbm_ieee80211_sta_vht_cap - STA's VHT capabilities
 *
 * This structure describes most essential parameters needed
 * to describe 802.11ac VHT capabilities for an STA.
 *
 * @vht_supported: is VHT supported by the STA
 * @cap: VHT capabilities map as described in 802.11ac spec
 * @vht_mcs: Supported VHT MCS rates
 */
struct atbm_ieee80211_sta_vht_cap {
	ATBM_BOOL vht_supported;
	atbm_uint32 cap; /* use IEEE80211_VHT_CAP_ */
	struct atbm_ieee80211_vht_mcs_info vht_mcs;
};

/**
 * struct ieee80211_he_obss_pd - AP settings for spatial reuse
 *
 * @enable: is the feature enabled.
 * @sr_ctrl: The SR Control field of SRP element.
 * @non_srg_max_offset: non-SRG maximum tx power offset
 * @min_offset: minimal tx power offset an associated station shall use
 * @max_offset: maximum tx power offset an associated station shall use
 * @bss_color_bitmap: bitmap that indicates the BSS color values used by
 *	members of the SRG
 * @partial_bssid_bitmap: bitmap that indicates the partial BSSID values
 *	used by members of the SRG
 */
struct atbm_ieee80211_he_obss_pd {
	ATBM_BOOL enable;
	atbm_uint8 sr_ctrl;
	atbm_uint8 non_srg_max_offset;
	atbm_uint8 min_offset;
	atbm_uint8 max_offset;
	atbm_uint8 bss_color_bitmap[8];
	atbm_uint8 partial_bssid_bitmap[8];
};

/**
 * struct cfg80211_he_bss_color - AP settings for BSS coloring
 *
 * @color: the current color.
 * @enabled: HE BSS color is used
 * @partial: define the AID equation.
 */
struct atbm_he_bss_color {
	atbm_uint8 color;
	ATBM_BOOL enabled;
	ATBM_BOOL partial;
};

/**
 * struct atbm_ieee80211_he_cap_elem - HE capabilities element
 *
 * This structure is the "HE capabilities element" fixed fields as
 * described in P802.11ax_D4.0 section 9.4.2.242.2 and 9.4.2.242.3
 */
struct atbm_ieee80211_he_cap_elem {
	atbm_uint8 mac_cap_info[6];
	atbm_uint8 phy_cap_info[11];
} atbm_packed;

#define ATBM_IEEE80211_TX_RX_MCS_NSS_DESC_MAX_LEN	5

/**
 * enum ieee80211_he_mcs_support - HE MCS support definitions
 * @IEEE80211_HE_MCS_SUPPORT_0_7: MCSes 0-7 are supported for the
 *	number of streams
 * @IEEE80211_HE_MCS_SUPPORT_0_9: MCSes 0-9 are supported
 * @IEEE80211_HE_MCS_SUPPORT_0_11: MCSes 0-11 are supported
 * @IEEE80211_HE_MCS_NOT_SUPPORTED: This number of streams isn't supported
 *
 * These definitions are used in each 2-bit subfield of the rx_mcs_*
 * and tx_mcs_* fields of &struct ieee80211_he_mcs_nss_supp, which are
 * both split into 8 subfields by number of streams. These values indicate
 * which MCSes are supported for the number of streams the value appears
 * for.
 */
enum atbm_ieee80211_he_mcs_support {
	ATBM_IEEE80211_HE_MCS_SUPPORT_0_7	= 0,
	ATBM_IEEE80211_HE_MCS_SUPPORT_0_9	= 1,
	ATBM_IEEE80211_HE_MCS_SUPPORT_0_11	= 2,
	ATBM_IEEE80211_HE_MCS_NOT_SUPPORTED	= 3,
};


/**
 * struct atbm_ieee80211_he_mcs_nss_supp - HE Tx/Rx HE MCS NSS Support Field
 *
 * This structure holds the data required for the Tx/Rx HE MCS NSS Support Field
 * described in P802.11ax_D2.0 section 9.4.2.237.4
 *
 * @rx_mcs_80: Rx MCS map 2 bits for each stream, total 8 streams, for channel
 *     widths less than 80MHz.
 * @tx_mcs_80: Tx MCS map 2 bits for each stream, total 8 streams, for channel
 *     widths less than 80MHz.
 * @rx_mcs_160: Rx MCS map 2 bits for each stream, total 8 streams, for channel
 *     width 160MHz.
 * @tx_mcs_160: Tx MCS map 2 bits for each stream, total 8 streams, for channel
 *     width 160MHz.
 * @rx_mcs_80p80: Rx MCS map 2 bits for each stream, total 8 streams, for
 *     channel width 80p80MHz.
 * @tx_mcs_80p80: Tx MCS map 2 bits for each stream, total 8 streams, for
 *     channel width 80p80MHz.
 */
struct atbm_ieee80211_he_mcs_nss_supp {
	atbm_uint16 rx_mcs_80;
	atbm_uint16 tx_mcs_80;
	atbm_uint16 rx_mcs_160;
	atbm_uint16 tx_mcs_160;
	atbm_uint16 rx_mcs_80p80;
	atbm_uint16 tx_mcs_80p80;
} atbm_packed;

/**
 * struct ieee80211_he_operation - HE capabilities element
 *
 * This structure is the "HE operation element" fields as
 * described in P802.11ax_D4.0 section 9.4.2.243
 */
struct atbm_ieee80211_he_operation {
	atbm_uint32 he_oper_params;
	atbm_uint16 he_mcs_nss_set;
	/* Optional 0,1,3,4,5,7 or 8 bytes: depends on @he_oper_params */
	atbm_uint8 optional[ATBM_ZEROSIZE];
} atbm_packed;

/**
 * struct ieee80211_he_spr - HE spatial reuse element
 *
 * This structure is the "HE spatial reuse element" element as
 * described in P802.11ax_D4.0 section 9.4.2.241
 */
struct atbm_ieee80211_he_spr {
	atbm_uint8 he_sr_control;
	/* Optional 0 to 19 bytes: depends on @he_sr_control */
	atbm_uint8 optional[ATBM_ZEROSIZE];
} atbm_packed;
struct atbm_ieee80211_he_6ghz_capa {
	/* uses IEEE80211_HE_6GHZ_CAP_* below */
	atbm_uint16 capa;
} atbm_packed;
/**
 * struct atbm_ieee80211_aid_response_ie
 *
 * AID Response element
 */
struct atbm_ieee80211_aid_response_ie {
	atbm_uint16 aid;
	atbm_uint8 switch_count;
	atbm_uint16 response_int;
} atbm_packed;

/**
 * struct atbm_ieee80211_bssid_index
 *
 * This structure refers to "Multiple BSSID-index element"
 *
 * @bssid_index: BSSID index
 * @dtim_period: optional, overrides transmitted BSS dtim period
 * @dtim_count: optional, overrides transmitted BSS dtim count
 */
struct atbm_ieee80211_bssid_index {
	atbm_uint8 bssid_index;
	atbm_uint8 dtim_period;
	atbm_uint8 dtim_count;
};

/**
 * struct atbm_ieee80211_multiple_bssid_configuration
 *
 * This structure refers to "Multiple BSSID Configuration element"
 *
 * @bssid_count: total number of active BSSIDs in the set
 * @profile_periodicity: the least number of beacon frames need to be received
 *	in order to discover all the nontransmitted BSSIDs in the set.
 */
struct atbm_ieee80211_multiple_bssid_configuration {
	atbm_uint8 bssid_count;
	atbm_uint8 profile_periodicity;
};

#define ATBM_IEEE80211_HE_PPE_THRES_MAX_LEN		25
/**
 * struct atbm_ieee80211_sta_he_cap - STA's HE capabilities
 *
 * This structure describes most essential parameters needed
 * to describe 802.11ax HE capabilities for a STA.
 *
 * @has_he: ATBM_TRUE iff HE data is valid.
 * @he_cap_elem: Fixed portion of the HE capabilities element.
 * @he_mcs_nss_supp: The supported NSS/MCS combinations.
 * @ppe_thres: Holds the PPE Thresholds data.
 */
struct atbm_ieee80211_sta_he_cap {
	ATBM_BOOL has_he;
	struct atbm_ieee80211_he_cap_elem he_cap_elem;
	struct atbm_ieee80211_he_mcs_nss_supp he_mcs_nss_supp;
	atbm_uint8 ppe_thres[ATBM_IEEE80211_HE_PPE_THRES_MAX_LEN];
    struct atbm_ieee80211_he_operation  he_operation;
};

/* 802.11ax HE MAC capabilities */
#define ATBM_IEEE80211_HE_MAC_CAP0_HTC_HE				0x01
#define ATBM_IEEE80211_HE_MAC_CAP0_TWT_REQ				0x02
#define ATBM_IEEE80211_HE_MAC_CAP0_TWT_RES				0x04
#define ATBM_IEEE80211_HE_MAC_CAP0_DYNAMIC_FRAG_NOT_SUPP		0x00
#define ATBM_IEEE80211_HE_MAC_CAP0_DYNAMIC_FRAG_LEVEL_1		0x08
#define ATBM_IEEE80211_HE_MAC_CAP0_DYNAMIC_FRAG_LEVEL_2		0x10
#define ATBM_IEEE80211_HE_MAC_CAP0_DYNAMIC_FRAG_LEVEL_3		0x18
#define ATBM_IEEE80211_HE_MAC_CAP0_DYNAMIC_FRAG_MASK			0x18
#define ATBM_IEEE80211_HE_MAC_CAP0_MAX_NUM_FRAG_MSDU_1		0x00
#define ATBM_IEEE80211_HE_MAC_CAP0_MAX_NUM_FRAG_MSDU_2		0x20
#define ATBM_IEEE80211_HE_MAC_CAP0_MAX_NUM_FRAG_MSDU_4		0x40
#define ATBM_IEEE80211_HE_MAC_CAP0_MAX_NUM_FRAG_MSDU_8		0x60
#define ATBM_IEEE80211_HE_MAC_CAP0_MAX_NUM_FRAG_MSDU_16		0x80
#define ATBM_IEEE80211_HE_MAC_CAP0_MAX_NUM_FRAG_MSDU_32		0xa0
#define ATBM_IEEE80211_HE_MAC_CAP0_MAX_NUM_FRAG_MSDU_64		0xc0
#define ATBM_IEEE80211_HE_MAC_CAP0_MAX_NUM_FRAG_MSDU_UNLIMITED	0xe0
#define ATBM_IEEE80211_HE_MAC_CAP0_MAX_NUM_FRAG_MSDU_MASK		0xe0

#define ATBM_IEEE80211_HE_MAC_CAP1_MIN_FRAG_SIZE_UNLIMITED		0x00
#define ATBM_IEEE80211_HE_MAC_CAP1_MIN_FRAG_SIZE_128			0x01
#define ATBM_IEEE80211_HE_MAC_CAP1_MIN_FRAG_SIZE_256			0x02
#define ATBM_IEEE80211_HE_MAC_CAP1_MIN_FRAG_SIZE_512			0x03
#define ATBM_IEEE80211_HE_MAC_CAP1_MIN_FRAG_SIZE_MASK		0x03
#define ATBM_IEEE80211_HE_MAC_CAP1_TF_MAC_PAD_DUR_0US		0x00
#define ATBM_IEEE80211_HE_MAC_CAP1_TF_MAC_PAD_DUR_8US		0x04
#define ATBM_IEEE80211_HE_MAC_CAP1_TF_MAC_PAD_DUR_16US		0x08
#define ATBM_IEEE80211_HE_MAC_CAP1_TF_MAC_PAD_DUR_MASK		0x0c
#define ATBM_IEEE80211_HE_MAC_CAP1_MULTI_TID_AGG_RX_QOS_1		0x00
#define ATBM_IEEE80211_HE_MAC_CAP1_MULTI_TID_AGG_RX_QOS_2		0x10
#define ATBM_IEEE80211_HE_MAC_CAP1_MULTI_TID_AGG_RX_QOS_3		0x20
#define ATBM_IEEE80211_HE_MAC_CAP1_MULTI_TID_AGG_RX_QOS_4		0x30
#define ATBM_IEEE80211_HE_MAC_CAP1_MULTI_TID_AGG_RX_QOS_5		0x40
#define ATBM_IEEE80211_HE_MAC_CAP1_MULTI_TID_AGG_RX_QOS_6		0x50
#define ATBM_IEEE80211_HE_MAC_CAP1_MULTI_TID_AGG_RX_QOS_7		0x60
#define ATBM_IEEE80211_HE_MAC_CAP1_MULTI_TID_AGG_RX_QOS_8		0x70
#define ATBM_IEEE80211_HE_MAC_CAP1_MULTI_TID_AGG_RX_QOS_MASK		0x70

/* Link adaptation is split between byte HE_MAC_CAP1 and
 * HE_MAC_CAP2. It should be set only if IEEE80211_HE_MAC_CAP0_HTC_HE
 * in which case the following values apply:
 * 0 = No feedback.
 * 1 = reserved.
 * 2 = Unsolicited feedback.
 * 3 = both
 */
#define ATBM_IEEE80211_HE_MAC_CAP1_LINK_ADAPTATION			0x80

#define ATBM_IEEE80211_HE_MAC_CAP2_LINK_ADAPTATION			0x01
#define ATBM_IEEE80211_HE_MAC_CAP2_ALL_ACK				0x02
#define ATBM_IEEE80211_HE_MAC_CAP2_TRS				0x04
#define ATBM_IEEE80211_HE_MAC_CAP2_BSR				0x08
#define ATBM_IEEE80211_HE_MAC_CAP2_BCAST_TWT				0x10
#define ATBM_IEEE80211_HE_MAC_CAP2_32BIT_BA_BITMAP			0x20
#define ATBM_IEEE80211_HE_MAC_CAP2_MU_CASCADING			0x40
#define ATBM_IEEE80211_HE_MAC_CAP2_ACK_EN				0x80

#define ATBM_IEEE80211_HE_MAC_CAP3_OMI_CONTROL			0x02
#define ATBM_IEEE80211_HE_MAC_CAP3_OFDMA_RA				0x04

/* The maximum length of an A-MDPU is defined by the combination of the Maximum
 * A-MDPU Length Exponent field in the HT capabilities, VHT capabilities and the
 * same field in the HE capabilities.
 */
#define ATBM_IEEE80211_HE_MAC_CAP3_MAX_AMPDU_LEN_EXP_EXT_0		0x00
#define ATBM_IEEE80211_HE_MAC_CAP3_MAX_AMPDU_LEN_EXP_EXT_1		0x08
#define ATBM_IEEE80211_HE_MAC_CAP3_MAX_AMPDU_LEN_EXP_EXT_2		0x10
#define ATBM_IEEE80211_HE_MAC_CAP3_MAX_AMPDU_LEN_EXP_EXT_3		0x18
#define ATBM_IEEE80211_HE_MAC_CAP3_MAX_AMPDU_LEN_EXP_MASK		0x18
#define ATBM_IEEE80211_HE_MAC_CAP3_AMSDU_FRAG			0x20
#define ATBM_IEEE80211_HE_MAC_CAP3_FLEX_TWT_SCHED			0x40
#define ATBM_IEEE80211_HE_MAC_CAP3_RX_CTRL_FRAME_TO_MULTIBSS		0x80

#define ATBM_IEEE80211_HE_MAC_CAP4_BSRP_BQRP_A_MPDU_AGG		0x01
#define ATBM_IEEE80211_HE_MAC_CAP4_QTP				0x02
#define ATBM_IEEE80211_HE_MAC_CAP4_BQR				0x04
#define ATBM_IEEE80211_HE_MAC_CAP4_PSR_RESP				0x08
#define ATBM_IEEE80211_HE_MAC_CAP4_NDP_FB_REP			0x10
#define ATBM_IEEE80211_HE_MAC_CAP4_OPS				0x20
#define ATBM_IEEE80211_HE_MAC_CAP4_AMSDU_IN_AMPDU			0x40
/* Multi TID agg TX is split between byte #4 and #5
 * The value is a combination of B39,B40,B41
 */
#define ATBM_IEEE80211_HE_MAC_CAP4_MULTI_TID_AGG_TX_QOS_B39		0x80

#define ATBM_IEEE80211_HE_MAC_CAP5_MULTI_TID_AGG_TX_QOS_B40		0x01
#define ATBM_IEEE80211_HE_MAC_CAP5_MULTI_TID_AGG_TX_QOS_B41		0x02
#define ATBM_IEEE80211_HE_MAC_CAP5_SUBCHAN_SELECTIVE_TRANSMISSION	0x04
#define ATBM_IEEE80211_HE_MAC_CAP5_UL_2x996_TONE_RU			0x08
#define ATBM_IEEE80211_HE_MAC_CAP5_OM_CTRL_UL_MU_DATA_DIS_RX		0x10
#define ATBM_IEEE80211_HE_MAC_CAP5_HE_DYNAMIC_SM_PS			0x20
#define ATBM_IEEE80211_HE_MAC_CAP5_PUNCTURED_SOUNDING		0x40
#define ATBM_IEEE80211_HE_MAC_CAP5_HT_VHT_TRIG_FRAME_RX		0x80

#define ATBM_IEEE80211_HE_VHT_MAX_AMPDU_FACTOR	20
#define ATBM_IEEE80211_HE_HT_MAX_AMPDU_FACTOR	16

/* 802.11ax HE PHY capabilities */
#define ATBM_IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_40MHZ_IN_2G		0x02
#define ATBM_IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_40MHZ_80MHZ_IN_5G	0x04
#define ATBM_IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_160MHZ_IN_5G		0x08
#define ATBM_IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_80PLUS80_MHZ_IN_5G	0x10
#define ATBM_IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_RU_MAPPING_IN_2G	0x20
#define ATBM_IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_RU_MAPPING_IN_5G	0x40
#define ATBM_IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_MASK			0xfe

#define ATBM_IEEE80211_HE_PHY_CAP1_PREAMBLE_PUNC_RX_80MHZ_ONLY_SECOND_20MHZ	0x01
#define ATBM_IEEE80211_HE_PHY_CAP1_PREAMBLE_PUNC_RX_80MHZ_ONLY_SECOND_40MHZ	0x02
#define ATBM_IEEE80211_HE_PHY_CAP1_PREAMBLE_PUNC_RX_160MHZ_ONLY_SECOND_20MHZ	0x04
#define ATBM_IEEE80211_HE_PHY_CAP1_PREAMBLE_PUNC_RX_160MHZ_ONLY_SECOND_40MHZ	0x08
#define ATBM_IEEE80211_HE_PHY_CAP1_PREAMBLE_PUNC_RX_MASK			0x0f
#define ATBM_IEEE80211_HE_PHY_CAP1_DEVICE_CLASS_A				0x10
#define ATBM_IEEE80211_HE_PHY_CAP1_LDPC_CODING_IN_PAYLOAD			0x20
#define ATBM_IEEE80211_HE_PHY_CAP1_HE_LTF_AND_GI_FOR_HE_PPDUS_0_8US		0x40
/* Midamble RX/TX Max NSTS is split between byte #2 and byte #3 */
#define ATBM_IEEE80211_HE_PHY_CAP1_MIDAMBLE_RX_TX_MAX_NSTS			0x80

#define ATBM_IEEE80211_HE_PHY_CAP2_MIDAMBLE_RX_TX_MAX_NSTS			0x01
#define ATBM_IEEE80211_HE_PHY_CAP2_NDP_4x_LTF_AND_3_2US			0x02
#define ATBM_IEEE80211_HE_PHY_CAP2_STBC_TX_UNDER_80MHZ			0x04
#define ATBM_IEEE80211_HE_PHY_CAP2_STBC_RX_UNDER_80MHZ			0x08
#define ATBM_IEEE80211_HE_PHY_CAP2_DOPPLER_TX				0x10
#define ATBM_IEEE80211_HE_PHY_CAP2_DOPPLER_RX				0x20

/* Note that the meaning of UL MU below is different between an AP and a non-AP
 * sta, where in the AP case it indicates support for Rx and in the non-AP sta
 * case it indicates support for Tx.
 */
#define ATBM_IEEE80211_HE_PHY_CAP2_UL_MU_FULL_MU_MIMO			0x40
#define ATBM_IEEE80211_HE_PHY_CAP2_UL_MU_PARTIAL_MU_MIMO			0x80

#define ATBM_IEEE80211_HE_PHY_CAP3_DCM_MAX_CONST_TX_NO_DCM			0x00
#define ATBM_IEEE80211_HE_PHY_CAP3_DCM_MAX_CONST_TX_BPSK			0x01
#define ATBM_IEEE80211_HE_PHY_CAP3_DCM_MAX_CONST_TX_QPSK			0x02
#define ATBM_IEEE80211_HE_PHY_CAP3_DCM_MAX_CONST_TX_16_QAM			0x03
#define ATBM_IEEE80211_HE_PHY_CAP3_DCM_MAX_CONST_TX_MASK			0x03
#define ATBM_IEEE80211_HE_PHY_CAP3_DCM_MAX_TX_NSS_1				0x00
#define ATBM_IEEE80211_HE_PHY_CAP3_DCM_MAX_TX_NSS_2				0x04
#define ATBM_IEEE80211_HE_PHY_CAP3_DCM_MAX_CONST_RX_NO_DCM			0x00
#define ATBM_IEEE80211_HE_PHY_CAP3_DCM_MAX_CONST_RX_BPSK			0x08
#define ATBM_IEEE80211_HE_PHY_CAP3_DCM_MAX_CONST_RX_QPSK			0x10
#define ATBM_IEEE80211_HE_PHY_CAP3_DCM_MAX_CONST_RX_16_QAM			0x18
#define ATBM_IEEE80211_HE_PHY_CAP3_DCM_MAX_CONST_RX_MASK			0x18
#define ATBM_IEEE80211_HE_PHY_CAP3_DCM_MAX_RX_NSS_1				0x00
#define ATBM_IEEE80211_HE_PHY_CAP3_DCM_MAX_RX_NSS_2				0x20
#define ATBM_IEEE80211_HE_PHY_CAP3_RX_PARTIAL_BW_SU_IN_20MHZ_MU		0x40
#define ATBM_IEEE80211_HE_PHY_CAP3_SU_BEAMFORMER				0x80

#define ATBM_IEEE80211_HE_PHY_CAP4_SU_BEAMFORMEE				0x01
#define ATBM_IEEE80211_HE_PHY_CAP4_MU_BEAMFORMER				0x02

/* Minimal allowed value of Max STS under 80MHz is 3 */
#define ATBM_IEEE80211_HE_PHY_CAP4_BEAMFORMEE_MAX_STS_UNDER_80MHZ_4		0x0c
#define ATBM_IEEE80211_HE_PHY_CAP4_BEAMFORMEE_MAX_STS_UNDER_80MHZ_5		0x10
#define ATBM_IEEE80211_HE_PHY_CAP4_BEAMFORMEE_MAX_STS_UNDER_80MHZ_6		0x14
#define ATBM_IEEE80211_HE_PHY_CAP4_BEAMFORMEE_MAX_STS_UNDER_80MHZ_7		0x18
#define ATBM_IEEE80211_HE_PHY_CAP4_BEAMFORMEE_MAX_STS_UNDER_80MHZ_8		0x1c
#define ATBM_IEEE80211_HE_PHY_CAP4_BEAMFORMEE_MAX_STS_UNDER_80MHZ_MASK	0x1c

/* Minimal allowed value of Max STS above 80MHz is 3 */
#define ATBM_IEEE80211_HE_PHY_CAP4_BEAMFORMEE_MAX_STS_ABOVE_80MHZ_4		0x60
#define ATBM_IEEE80211_HE_PHY_CAP4_BEAMFORMEE_MAX_STS_ABOVE_80MHZ_5		0x80
#define ATBM_IEEE80211_HE_PHY_CAP4_BEAMFORMEE_MAX_STS_ABOVE_80MHZ_6		0xa0
#define ATBM_IEEE80211_HE_PHY_CAP4_BEAMFORMEE_MAX_STS_ABOVE_80MHZ_7		0xc0
#define ATBM_IEEE80211_HE_PHY_CAP4_BEAMFORMEE_MAX_STS_ABOVE_80MHZ_8		0xe0
#define ATBM_IEEE80211_HE_PHY_CAP4_BEAMFORMEE_MAX_STS_ABOVE_80MHZ_MASK	0xe0

#define ATBM_IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_UNDER_80MHZ_1	0x00
#define ATBM_IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_UNDER_80MHZ_2	0x01
#define ATBM_IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_UNDER_80MHZ_3	0x02
#define ATBM_IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_UNDER_80MHZ_4	0x03
#define ATBM_IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_UNDER_80MHZ_5	0x04
#define ATBM_IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_UNDER_80MHZ_6	0x05
#define ATBM_IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_UNDER_80MHZ_7	0x06
#define ATBM_IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_UNDER_80MHZ_8	0x07
#define ATBM_IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_UNDER_80MHZ_MASK	0x07

#define ATBM_IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_ABOVE_80MHZ_1	0x00
#define ATBM_IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_ABOVE_80MHZ_2	0x08
#define ATBM_IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_ABOVE_80MHZ_3	0x10
#define ATBM_IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_ABOVE_80MHZ_4	0x18
#define ATBM_IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_ABOVE_80MHZ_5	0x20
#define ATBM_IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_ABOVE_80MHZ_6	0x28
#define ATBM_IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_ABOVE_80MHZ_7	0x30
#define ATBM_IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_ABOVE_80MHZ_8	0x38
#define ATBM_IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_ABOVE_80MHZ_MASK	0x38

#define ATBM_IEEE80211_HE_PHY_CAP5_NG16_SU_FEEDBACK				0x40
#define ATBM_IEEE80211_HE_PHY_CAP5_NG16_MU_FEEDBACK				0x80

#define ATBM_IEEE80211_HE_PHY_CAP6_CODEBOOK_SIZE_42_SU			0x01
#define ATBM_IEEE80211_HE_PHY_CAP6_CODEBOOK_SIZE_75_MU			0x02
#define ATBM_IEEE80211_HE_PHY_CAP6_TRIG_SU_BEAMFORMING_FB			0x04
#define ATBM_IEEE80211_HE_PHY_CAP6_TRIG_MU_BEAMFORMING_PARTIAL_BW_FB		0x08
#define ATBM_IEEE80211_HE_PHY_CAP6_TRIG_CQI_FB				0x10
#define ATBM_IEEE80211_HE_PHY_CAP6_PARTIAL_BW_EXT_RANGE			0x20
#define ATBM_IEEE80211_HE_PHY_CAP6_PARTIAL_BANDWIDTH_DL_MUMIMO		0x40
#define ATBM_IEEE80211_HE_PHY_CAP6_PPE_THRESHOLD_PRESENT			0x80

#define ATBM_IEEE80211_HE_PHY_CAP7_PSR_BASED_SR				0x01
#define ATBM_IEEE80211_HE_PHY_CAP7_POWER_BOOST_FACTOR_SUPP			0x02
#define ATBM_IEEE80211_HE_PHY_CAP7_HE_SU_MU_PPDU_4XLTF_AND_08_US_GI		0x04
#define ATBM_IEEE80211_HE_PHY_CAP7_MAX_NC_1					0x08
#define ATBM_IEEE80211_HE_PHY_CAP7_MAX_NC_2					0x10
#define ATBM_IEEE80211_HE_PHY_CAP7_MAX_NC_3					0x18
#define ATBM_IEEE80211_HE_PHY_CAP7_MAX_NC_4					0x20
#define ATBM_IEEE80211_HE_PHY_CAP7_MAX_NC_5					0x28
#define ATBM_IEEE80211_HE_PHY_CAP7_MAX_NC_6					0x30
#define ATBM_IEEE80211_HE_PHY_CAP7_MAX_NC_7					0x38
#define ATBM_IEEE80211_HE_PHY_CAP7_MAX_NC_MASK				0x38
#define ATBM_IEEE80211_HE_PHY_CAP7_STBC_TX_ABOVE_80MHZ			0x40
#define ATBM_IEEE80211_HE_PHY_CAP7_STBC_RX_ABOVE_80MHZ			0x80

#define ATBM_IEEE80211_HE_PHY_CAP8_HE_ER_SU_PPDU_4XLTF_AND_08_US_GI		0x01
#define ATBM_IEEE80211_HE_PHY_CAP8_20MHZ_IN_40MHZ_HE_PPDU_IN_2G		0x02
#define ATBM_IEEE80211_HE_PHY_CAP8_20MHZ_IN_160MHZ_HE_PPDU			0x04
#define ATBM_IEEE80211_HE_PHY_CAP8_80MHZ_IN_160MHZ_HE_PPDU			0x08
#define ATBM_IEEE80211_HE_PHY_CAP8_HE_ER_SU_1XLTF_AND_08_US_GI		0x10
#define ATBM_IEEE80211_HE_PHY_CAP8_MIDAMBLE_RX_TX_2X_AND_1XLTF		0x20
#define ATBM_IEEE80211_HE_PHY_CAP8_DCM_MAX_RU_242				0x00
#define ATBM_IEEE80211_HE_PHY_CAP8_DCM_MAX_RU_484				0x40
#define ATBM_IEEE80211_HE_PHY_CAP8_DCM_MAX_RU_996				0x80
#define ATBM_IEEE80211_HE_PHY_CAP8_DCM_MAX_RU_2x996				0xc0
#define ATBM_IEEE80211_HE_PHY_CAP8_DCM_MAX_RU_MASK				0xc0

#define ATBM_IEEE80211_HE_PHY_CAP9_LONGER_THAN_16_SIGB_OFDM_SYM		0x01
#define ATBM_IEEE80211_HE_PHY_CAP9_NON_TRIGGERED_CQI_FEEDBACK		0x02
#define ATBM_IEEE80211_HE_PHY_CAP9_TX_1024_QAM_LESS_THAN_242_TONE_RU		0x04
#define ATBM_IEEE80211_HE_PHY_CAP9_RX_1024_QAM_LESS_THAN_242_TONE_RU		0x08
#define ATBM_IEEE80211_HE_PHY_CAP9_RX_FULL_BW_SU_USING_MU_WITH_COMP_SIGB	0x10
#define ATBM_IEEE80211_HE_PHY_CAP9_RX_FULL_BW_SU_USING_MU_WITH_NON_COMP_SIGB	0x20
#define ATBM_IEEE80211_HE_PHY_CAP9_NOMIMAL_PKT_PADDING_0US			0x00
#define ATBM_IEEE80211_HE_PHY_CAP9_NOMIMAL_PKT_PADDING_8US			0x40
#define ATBM_IEEE80211_HE_PHY_CAP9_NOMIMAL_PKT_PADDING_16US			0x80
#define ATBM_IEEE80211_HE_PHY_CAP9_NOMIMAL_PKT_PADDING_RESERVED		0xc0
#define ATBM_IEEE80211_HE_PHY_CAP9_NOMIMAL_PKT_PADDING_MASK			0xc0
//add copy,compile error
#define ATBM_IEEE80211_HE_PHY_CAP9_NOMINAL_PKT_PADDING_0US			0x00
#define ATBM_IEEE80211_HE_PHY_CAP9_NOMINAL_PKT_PADDING_8US			0x40
#define ATBM_IEEE80211_HE_PHY_CAP9_NOMINAL_PKT_PADDING_16US			0x80
#define ATBM_IEEE80211_HE_PHY_CAP9_NOMINAL_PKT_PADDING_RESERVED		0xc0
#define ATBM_IEEE80211_HE_PHY_CAP9_NOMINAL_PKT_PADDING_MASK			0xc0

#define ATBM_IEEE80211_HE_PHY_CAP10_HE_MU_M1RU_MAX_LTF			0x01

/* 802.11ax HE TX/RX MCS NSS Support  */
#define ATBM_IEEE80211_TX_RX_MCS_NSS_SUPP_HIGHEST_MCS_POS			(3)
#define ATBM_IEEE80211_TX_RX_MCS_NSS_SUPP_TX_BITMAP_POS			(6)
#define ATBM_IEEE80211_TX_RX_MCS_NSS_SUPP_RX_BITMAP_POS			(11)
#define ATBM_IEEE80211_TX_RX_MCS_NSS_SUPP_TX_BITMAP_MASK			0x07c0
#define ATBM_IEEE80211_TX_RX_MCS_NSS_SUPP_RX_BITMAP_MASK			0xf800

#define ATBM_IEEE80211_ADDBA_EXT_FRAG_LEVEL_MASK	GENMASK(2, 1)
#define ATBM_IEEE80211_ADDBA_EXT_FRAG_LEVEL_SHIFT	1
#define ATBM_IEEE80211_ADDBA_EXT_NO_FRAG		ATBM_BIT(0)


/* TX/RX HE MCS Support field Highest MCS subfield encoding */
enum atbm_ieee80211_he_highest_mcs_supported_subfield_enc {
	ATBM_HIGHEST_MCS_SUPPORTED_MCS7 = 0,
	ATBM_HIGHEST_MCS_SUPPORTED_MCS8,
	ATBM_HIGHEST_MCS_SUPPORTED_MCS9,
	ATBM_HIGHEST_MCS_SUPPORTED_MCS10,
	ATBM_HIGHEST_MCS_SUPPORTED_MCS11,
};

/* Calculate 802.11ax HE capabilities IE Tx/Rx HE MCS NSS Support Field size */
static inline atbm_uint8
atbm_ieee80211_he_mcs_nss_size(const struct atbm_ieee80211_he_cap_elem *he_cap)
{
	atbm_uint8 count = 4;

	if (he_cap->phy_cap_info[0] &
	    ATBM_IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_160MHZ_IN_5G)
		count += 4;

	if (he_cap->phy_cap_info[0] &
	    ATBM_IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_80PLUS80_MHZ_IN_5G)
		count += 4;

	return count;
}
static inline int atbm_ieee80211_he_mcs_rate_support(atbm_uint16 mcs_map,atbm_uint8 nss)
{
	atbm_uint16 mcs_mask = atbm_le16_to_cpu(mcs_map) >> ((nss-1)*2);
	return 	(mcs_mask & 0x03) != ATBM_IEEE80211_HE_MCS_NOT_SUPPORTED;
}
static inline atbm_uint8 atbm_ieee80211_he_mcs_operation_rate(atbm_uint16 mcs_map,atbm_uint8 nss)
{
	atbm_uint16 mcs_mask = atbm_le16_to_cpu(mcs_map) >> ((nss-1)*2);

	switch(mcs_mask & 0x03){
	case ATBM_IEEE80211_HE_MCS_SUPPORT_0_7: 	return 7;
	case ATBM_IEEE80211_HE_MCS_SUPPORT_0_9: 	return 9;
	case ATBM_IEEE80211_HE_MCS_SUPPORT_0_11: 	return 11;
	default :
		ATBM_BUG_ON(1);
	}

	return 0;
}
/* 802.11ax HE PPE Thresholds */
#define ATBM_IEEE80211_PPE_THRES_NSS_SUPPORT_2NSS			(1)
#define ATBM_IEEE80211_PPE_THRES_NSS_POS				(0)
#define ATBM_IEEE80211_PPE_THRES_NSS_MASK				(7)
#define ATBM_IEEE80211_PPE_THRES_RU_INDEX_BITMASK_2x966_AND_966_RU	\
	(ATBM_BIT(5) | ATBM_BIT(6))
#define ATBM_IEEE80211_PPE_THRES_RU_INDEX_BITMASK_MASK		0x78
#define ATBM_IEEE80211_PPE_THRES_RU_INDEX_BITMASK_POS		(3)
#define ATBM_IEEE80211_PPE_THRES_INFO_PPET_SIZE			(3)

#define hweight8(w)		\
	((unsigned int)			\
	 ((!!((w) & (1ULL << 0))) +	\
	  (!!((w) & (1ULL << 1))) +	\
	  (!!((w) & (1ULL << 2))) +	\
	  (!!((w) & (1ULL << 3))) +	\
	  (!!((w) & (1ULL << 4))) +	\
	  (!!((w) & (1ULL << 5))) +	\
	  (!!((w) & (1ULL << 6))) +	\
	  (!!((w) & (1ULL << 7)))))


/*
 * Calculate 802.11ax HE capabilities IE PPE field size
 * Input: Header byte of ppe_thres (first byte), and HE capa IE's PHY cap atbm_uint8*
 */
static inline atbm_uint8
atbm_ieee80211_he_ppe_size(atbm_uint8 ppe_thres_hdr, const atbm_uint8 *phy_cap_info)
{
	atbm_uint8 n;

	if ((phy_cap_info[6] &
	     ATBM_IEEE80211_HE_PHY_CAP6_PPE_THRESHOLD_PRESENT) == 0)
		return 0;

	n = hweight8(ppe_thres_hdr &
		     ATBM_IEEE80211_PPE_THRES_RU_INDEX_BITMASK_MASK);
	n *= (1 + ((ppe_thres_hdr & ATBM_IEEE80211_PPE_THRES_NSS_MASK) >>
		   ATBM_IEEE80211_PPE_THRES_NSS_POS));

	/*
	 * Each pair is 6 bits, and we need to add the 7 "header" bits to the
	 * total size.
	 */
	n = (n * ATBM_IEEE80211_PPE_THRES_INFO_PPET_SIZE * 2) + 7;
	n = DIV_ROUND_UP(n, 8);

	return n;
}

/* HE Operation defines */
#define ATBM_IEEE80211_HE_OPERATION_DFLT_PE_DURATION_MASK		0x00000007
#define ATBM_IEEE80211_HE_OPERATION_TWT_REQUIRED			0x00000008
#define ATBM_IEEE80211_HE_OPERATION_RTS_THRESHOLD_MASK		0x00003ff0
#define ATBM_IEEE80211_HE_OPERATION_RTS_THRESHOLD_OFFSET		4
#define ATBM_IEEE80211_HE_OPERATION_VHT_OPER_INFO			0x00004000
#define ATBM_IEEE80211_HE_OPERATION_CO_HOSTED_BSS			0x00008000
#define ATBM_IEEE80211_HE_OPERATION_ER_SU_DISABLE			0x00010000
#define ATBM_IEEE80211_HE_OPERATION_6GHZ_OP_INFO			0x00020000
#define ATBM_IEEE80211_HE_OPERATION_BSS_COLOR_MASK			0x3f000000
#define ATBM_IEEE80211_HE_OPERATION_BSS_COLOR_OFFSET			24
#define ATBM_IEEE80211_HE_OPERATION_PARTIAL_BSS_COLOR		0x40000000
#define ATBM_IEEE80211_HE_OPERATION_BSS_COLOR_DISABLED		0x80000000

#define ATBM_IEEE80211_6GHZ_CTRL_REG_LPI_AP	0
#define ATBM_IEEE80211_6GHZ_CTRL_REG_SP_AP	1

/**
 * atbm_ieee80211_he_6ghz_oper - HE 6 GHz operation Information field
 * @primary: primary channel
 * @control: control flags
 * @ccfs0: channel center frequency segment 0
 * @ccfs1: channel center frequency segment 1
 * @minrate: minimum rate (in 1 Mbps units)
 */
struct atbm_ieee80211_he_6ghz_oper {
	atbm_uint8 primary;
#define ATBM_IEEE80211_HE_6GHZ_OPER_CTRL_CHANWIDTH	0x3
#define		ATBM_IEEE80211_HE_6GHZ_OPER_CTRL_CHANWIDTH_20MHZ	0
#define		ATBM_IEEE80211_HE_6GHZ_OPER_CTRL_CHANWIDTH_40MHZ	1
#define		ATBM_IEEE80211_HE_6GHZ_OPER_CTRL_CHANWIDTH_80MHZ	2
#define		ATBM_IEEE80211_HE_6GHZ_OPER_CTRL_CHANWIDTH_160MHZ	3
#define ATBM_IEEE80211_HE_6GHZ_OPER_CTRL_DUP_BEACON	0x4
#define ATBM_IEEE80211_HE_6GHZ_OPER_CTRL_REG_INFO	0x38
	atbm_uint8 control;
	atbm_uint8 ccfs0;
	atbm_uint8 ccfs1;
	atbm_uint8 minrate;
} atbm_packed;

/*
 * In "9.4.2.161 Transmit Power Envelope element" of "IEEE Std 802.11ax-2021",
 * it show four types in "Table 9-275a-Maximum Transmit Power Interpretation
 * subfield encoding", and two category for each type in "Table E-12-Regulatory
 * Info subfield encoding in the United States".
 * So it it totally max 8 Transmit Power Envelope element.
 */
#define ATBM_IEEE80211_TPE_MAX_IE_COUNT	8
/*
 * In "Table 9-277—Meaning of Maximum Transmit Power Count subfield"
 * of "IEEE Std 802.11ax?‐2021", the max power level is 8.
 */
#define ATBM_IEEE80211_MAX_NUM_PWR_LEVEL	8

#define ATBM_IEEE80211_TPE_MAX_POWER_COUNT	8

/* transmit power interpretation type of transmit power envelope element */
enum atbm_ieee80211_tx_power_intrpt_type {
	ATBM_IEEE80211_TPE_LOCAL_EIRP,
	ATBM_IEEE80211_TPE_LOCAL_EIRP_PSD,
	ATBM_IEEE80211_TPE_REG_CLIENT_EIRP,
	ATBM_IEEE80211_TPE_REG_CLIENT_EIRP_PSD,
};

/**
 * struct ieee80211_tx_pwr_env
 *
 * This structure represents the "Transmit Power Envelope element"
 */
struct atbm_ieee80211_tx_pwr_env {
	atbm_uint8 tx_power_info;
	atbm_int8 tx_power[ATBM_IEEE80211_TPE_MAX_POWER_COUNT];
} atbm_packed;

#define ATBM_IEEE80211_TX_PWR_ENV_INFO_COUNT 0x7
#define ATBM_IEEE80211_TX_PWR_ENV_INFO_INTERPRET 0x38
#define ATBM_IEEE80211_TX_PWR_ENV_INFO_CATEGORY 0xC0

/*
 * atbm_ieee80211_he_oper_size - calculate 802.11ax HE Operations IE size
 * @he_oper_ie: byte data of the He Operations IE, stating from the byte
 *	after the ext ID byte. It is assumed that he_oper_ie has at least
 *	sizeof(struct ieee80211_he_operation) bytes, the caller must have
 *	validated this.
 * @return the actual size of the IE data (not including header), or 0 on error
 */
static inline atbm_uint8
atbm_ieee80211_he_oper_size(const atbm_uint8 *he_oper_ie)
{
	struct atbm_ieee80211_he_operation *he_oper = (void *)he_oper_ie;
	atbm_uint8 oper_len = offsetof(struct atbm_ieee80211_he_operation, optional);
	atbm_uint32 he_oper_params;

	/* Make sure the input is not NULL */
	if (!he_oper_ie)
		return 0;

	/* Calc required length */
	he_oper_params = atbm_le32_to_cpu(he_oper->he_oper_params);
	if (he_oper_params & ATBM_IEEE80211_HE_OPERATION_VHT_OPER_INFO)
		oper_len += 3;
	if (he_oper_params & ATBM_IEEE80211_HE_OPERATION_CO_HOSTED_BSS)
		oper_len++;
	if (he_oper_params & ATBM_IEEE80211_HE_OPERATION_6GHZ_OP_INFO)
		oper_len += sizeof(struct atbm_ieee80211_he_6ghz_oper);

	/* Add the first byte (extension ID) to the total length */
	oper_len++;

	return oper_len;
}

/**
 * atbm_ieee80211_he_6ghz_oper - obtain 6 GHz operation field
 * @he_oper: HE operation element (must be pre-validated for size)
 *	but may be %NULL
 *
 * Return: a pointer to the 6 GHz operation field, or %NULL
 */
static inline const struct atbm_ieee80211_he_6ghz_oper *
atbm_ieee80211_he_6ghz_oper(const struct atbm_ieee80211_he_operation *he_oper)
{
	const atbm_uint8 *ret = (void *)&he_oper->optional;
	atbm_uint32 he_oper_params;

	if (!he_oper)
		return NULL;

	he_oper_params = atbm_le32_to_cpu(he_oper->he_oper_params);

	if (!(he_oper_params & ATBM_IEEE80211_HE_OPERATION_6GHZ_OP_INFO))
		return NULL;
	if (he_oper_params & ATBM_IEEE80211_HE_OPERATION_VHT_OPER_INFO)
		ret += 3;
	if (he_oper_params & ATBM_IEEE80211_HE_OPERATION_CO_HOSTED_BSS)
		ret++;

	return (void *)ret;
}

/**
 * ether_addr_to_u64 - Convert an Ethernet address into a u64 value.
 * @addr: Pointer to a six-byte array containing the Ethernet address
 *
 * Return a u64 value of the address
 */
static inline atbm_uint64 atbm_ether_addr_to_u64(const atbm_uint8 *addr)
{
	atbm_uint64 u = 0;
	int i;

	for (i = 0; i < ATBM_ETH_ALEN; i++)
		u = u << 8 | addr[i];

	return u;
}

/**
 * u64_to_ether_addr - Convert a u64 to an Ethernet address.
 * @u: u64 to convert to an Ethernet MAC address
 * @addr: Pointer to a six-byte array to contain the Ethernet address
 */
static inline void atbm_u64_to_ether_addr(atbm_uint64 u, atbm_uint8 *addr)
{
	int i;

	for (i = ATBM_ETH_ALEN - 1; i >= 0; i--) {
		addr[i] = u & 0xff;
		u = u >> 8;
	}
}

#define BITS_PER_LONG_LONG 64
#define GENMASK_ULL(h, l) \
	(((~0ULL) << (l)) & (~0ULL >> (BITS_PER_LONG_LONG - 1 - (h))))

/**
 * cfg80211_gen_new_bssid - generate a nontransmitted BSSID for multi-BSSID
 * @bssid: transmitter BSSID
 * @max_bssid: max BSSID indicator, taken from Multiple BSSID element
 * @mbssid_index: BSSID index, taken from Multiple BSSID index element
 * @new_bssid: calculated nontransmitted BSSID
 */
static inline void atbmwifi_gen_new_bssid(const atbm_uint8 *bssid, atbm_uint8 max_bssid,
					  atbm_uint8 mbssid_index, atbm_uint8 *new_bssid)
{
	atbm_uint64 bssid_u64 = atbm_ether_addr_to_u64(bssid);
	atbm_uint64 mask = GENMASK_ULL(max_bssid - 1, 0);
	atbm_uint64 new_bssid_u64;

	new_bssid_u64 = bssid_u64 & ~mask;

	new_bssid_u64 |= ((bssid_u64 & mask) + mbssid_index) & mask;

	atbm_u64_to_ether_addr(new_bssid_u64, new_bssid);
}


/* HE Spatial Reuse defines */
#define ATBM_IEEE80211_HE_SPR_PSR_DISALLOWED				ATBM_BIT(0)
#define ATBM_IEEE80211_HE_SPR_NON_SRG_OBSS_PD_SR_DISALLOWED		ATBM_BIT(1)
#define ATBM_IEEE80211_HE_SPR_NON_SRG_OFFSET_PRESENT			ATBM_BIT(2)
#define ATBM_IEEE80211_HE_SPR_SRG_INFORMATION_PRESENT		ATBM_BIT(3)
#define ATBM_IEEE80211_HE_SPR_HESIGA_SR_VAL15_ALLOWED		ATBM_BIT(4)

/*
 * atbm_ieee80211_he_spr_size - calculate 802.11ax HE Spatial Reuse IE size
 * @he_spr_ie: byte data of the He Spatial Reuse IE, stating from the byte
 *	after the ext ID byte. It is assumed that he_spr_ie has at least
 *	sizeof(struct ieee80211_he_spr) bytes, the caller must have validated
 *	this
 * @return the actual size of the IE data (not including header), or 0 on error
 */
static inline atbm_uint8
atbm_ieee80211_he_spr_size(const atbm_uint8 *he_spr_ie)
{
	struct atbm_ieee80211_he_spr *he_spr = (void *)he_spr_ie;
	atbm_uint8 spr_len = offsetof(struct atbm_ieee80211_he_spr, optional);
	atbm_uint8 he_spr_params;

	/* Make sure the input is not NULL */
	if (!he_spr_ie)
		return 0;

	/* Calc required length */
	he_spr_params = he_spr->he_sr_control;
	if (he_spr_params & ATBM_IEEE80211_HE_SPR_NON_SRG_OFFSET_PRESENT)
		spr_len++;
	if (he_spr_params & ATBM_IEEE80211_HE_SPR_SRG_INFORMATION_PRESENT)
		spr_len += 18;

	/* Add the first byte (extension ID) to the total length */
	spr_len++;

	return spr_len;
}

enum {
	ATBM_HE_4X32=1,	
	ATBM_HE_4X08=2,		
	ATBM_HE_2X16=3,
	ATBM_HE_2X08=4,
};

/**
 * struct ieee80211_sband_iftype_data - sband data per interface type
 *
 * This structure encapsulates sband data that is relevant for the
 * interface types defined in @types_mask.  Each type in the
 * @types_mask must be unique across all instances of iftype_data.
 *
 * @types_mask: interface types mask
 * @he_cap: holds the HE capabilities
 * @he_6ghz_capa: HE 6 GHz capabilities, must be filled in for a
 *	6 GHz band channel (and 0 may be valid value).
 * @vendor_elems: vendor element(s) to advertise
 * @vendor_elems.data: vendor element(s) data
 * @vendor_elems.len: vendor element(s) length
 */
struct ieee80211_sband_iftype_data {
	atbm_uint16 types_mask;
	struct atbm_ieee80211_sta_he_cap he_cap;
	struct {
		const atbm_uint8 *data;
		unsigned int len;
	} vendor_elems;
};

/**
 * struct atbmwifi_ieee80211_supported_band - frequency band definition
 *
 * This structure describes a frequency band a wiphy
 * is able to operate in.
 *
 * @channels: Array of channels the hardware can operate in
 *	in this band.
 * @band: the band this structure represents
 * @n_channels: Number of channels in @channels
 * @bitrates: Array of bitrates the hardware can operate with
 *	in this band. Must be sorted to give a valid "supported
 *	rates" IE, i.e. CCK rates first, then OFDM.
 * @n_bitrates: Number of bitrates in @bitrates
 * @ht_cap: HT capabilities in this band
  * @n_iftype_data: number of iftype data entries
 * @iftype_data: interface type data entries.  Note that the bits in
 *	@types_mask inside this structure cannot overlap (i.e. only
 *	one occurrence of each type is allowed across all instances of
 *	iftype_data).
 */
struct atbmwifi_ieee80211_supported_band {
	struct atbmwifi_ieee80211_channel *channels;
	struct atbmwifi_ieee80211_rate *bitrates;
	enum atbmwifi_ieee80211_band band;
	int n_channels;
	int n_bitrates;
	struct atbmwifi_ieee80211_sta_ht_cap ht_cap;
	struct atbm_ieee80211_sta_vht_cap vht_cap;
	atbm_uint16 n_iftype_data;
	struct ieee80211_sband_iftype_data iftype_data;
};

#define ATBM_IEEE80211_MAX_RX_AGGR			128
#define ATBM_IEEE80211_MAX_TX_AGGR			128

#define STA_TID_NUM 16
#define ADDBA_RESP_INTERVAL ATBM_HZ
#define HT_AGG_MAX_RETRIES		0x3

#define HT_AGG_STATE_DRV_READY		0
#define HT_AGG_STATE_RESPONSE_RECEIVED	1
#define HT_AGG_STATE_OPERATIONAL	2
#define HT_AGG_STATE_STOPPING		3
#define HT_AGG_STATE_WANT_START		4
#define HT_AGG_STATE_WANT_STOP		5

#define HT_AGG_TX_INTREVAL			10*ATBM_HZ
#define HT_AGG_TX_BURST				5


/**
 * struct tid_ampdu_tx - TID aggregation information (Tx).
 *
 * @rcu_head: rcu head for freeing structure
 * @addba_resp_timer: timer for peer's response to addba request
 * @pending: pending frames queue -- use sta's spinlock to protect
 * @dialog_token: dialog token for aggregation session
 * @timeout: session timeout value to be filled in ADDBA requests
 * @state: session state (see above)
 * @stop_initiator: initiator of a session stop
 * @tx_stop: TX DelBA frame when stopping
 * @buf_size: reorder buffer size at receiver
 * @failed_bar_ssn: ssn of the last failed BAR tx attempt
 * @bar_pending: BAR needs to be re-sent
 *
 * This structure's lifetime is managed by RCU, assignments to
 * the array holding it must hold the aggregation mutex.
 *
 * The TX path can access it under RCU lock-free if, and
 * only if, the state has the flag %HT_AGG_STATE_OPERATIONAL
 * set. Otherwise, the TX path must also acquire the spinlock
 * and re-check the state, see comments in the tx code
 * touching it.
 */
struct tid_ampdu_tx {
	//struct wpa_timer addba_resp_timer;
	struct atbm_buff_head pending;
	atbm_uint32 state;
	atbm_uint16 timeout;
	atbm_uint8 dialog_token;
	atbm_uint8 stop_initiator;
	ATBM_BOOL tx_stop;
	atbm_uint8 buf_size;

	atbm_uint16 failed_bar_ssn;
	ATBM_BOOL bar_pending;
	atbm_uint8 hw_token;
};

/**
 * struct tid_ampdu_rx - TID aggregation information (Rx).
 *
 * @reorder_buf: buffer to reorder incoming aggregated MPDUs
 * @reorder_time: jiffies when skb was added
 * @session_timer: check if peer keeps Tx-ing on the TID (by timeout value)
 * @reorder_timer: releases expired frames from the reorder buffer.
 * @head_seq_num: head sequence number in reordering buffer.
 * @stored_mpdu_num: number of MPDUs in reordering buffer
 * @ssn: Starting Sequence Number expected to be aggregated.
 * @buf_size: buffer size for incoming A-MPDUs
 * @timeout: reset timer value (in TUs).
 * @dialog_token: dialog token for aggregation session
 * @rcu_head: RCU head used for freeing this struct
 * @reorder_lock: serializes access to reorder buffer, see below.
 *
 * This structure's lifetime is managed by RCU, assignments to
 * the array holding it must hold the aggregation mutex.
 *
 * The @reorder_lock is used to protect the members of this
 * struct, except for @timeout, @buf_size and @dialog_token,
 * which are constant across the lifetime of the struct (the
 * dialog token being used only for debugging).
 */
struct tid_ampdu_rx {
	atbm_spinlock_t reorder_lock;
	struct atbm_buff **reorder_buf;
	unsigned long *reorder_time;
	//struct atbm_timer_list session_timer;
	//struct atbm_timer_list reorder_timer;
	atbm_uint16 head_seq_num;
	atbm_uint16 stored_mpdu_num;
	atbm_uint16 ssn;
	atbm_uint16 buf_size;
	atbm_uint16 timeout;
	atbm_uint8 dialog_token;
	atbm_uint8 hw_token;
};

/**
 * struct sta_ampdu_mlme - STA aggregation information.
 *
 * @tid_rx: aggregation info for Rx per TID -- RCU protected
 * @tid_tx: aggregation info for Tx per TID
 * @tid_start_tx: sessions where start was requested
 * @addba_req_num: number of times addBA request has been sent.
 * @dialog_token_allocator: dialog token enumerator for each new session;
 * @work: work struct for starting/stopping aggregation
 * @tid_rx_timer_expired: bitmap indicating on which TIDs the
 *	RX timer expired until the work for it runs
 * @tid_rx_stop_requested:  bitmap indicating which BA sessions per TID the
 *	driver requested to close until the work for it runs
 * @mtx: mutex to protect all TX data (except non-NULL assignments
 *	to tid_tx[idx], which are protected by the sta spinlock)
 */
struct sta_ampdu_mlme {
	int init;
	atbm_mutex mtx;
	/* rx */
	//struct tid_ampdu_rx *tid_rx[STA_TID_NUM];
	atbm_uint32 tid_rx_timer_expired[BITS_TO_U32S(STA_TID_NUM)];
	atbm_uint32 tid_rx_stop_requested[BITS_TO_U32S(STA_TID_NUM)];
	/* tx */
	atbm_work work;
	struct tid_ampdu_tx *tid_tx[STA_TID_NUM];
	struct tid_ampdu_tx *tid_start_tx[STA_TID_NUM];
	atbm_int8 addba_req_num[STA_TID_NUM];
	atbm_int8 dialog_token_allocator;
	int trylimit[STA_TID_NUM];
	atbm_uint32 timestamp[STA_TID_NUM];
};

/**
 * enum ieee80211_ampdu_mlme_action - A-MPDU actions
 *
 * These flags are used with the ampdu_action() callback in
 * &struct ieee80211_ops to indicate which action is needed.
 *
 * Note that drivers MUST be able to deal with a TX aggregation
 * session being stopped even before they OK'ed starting it by
 * calling ieee80211_start_tx_ba_cb_irqsafe, because the peer
 * might receive the addBA frame and send a delBA right away!
 *
 * @IEEE80211_AMPDU_RX_START: start Rx aggregation
 * @IEEE80211_AMPDU_RX_STOP: stop Rx aggregation
 * @IEEE80211_AMPDU_TX_START: start Tx aggregation
 * @IEEE80211_AMPDU_TX_STOP: stop Tx aggregation
 * @IEEE80211_AMPDU_TX_OPERATIONAL: TX aggregation has become operational
 */
enum atbmwifi_ieee80211_ampdu_mlme_action {
	ATBM_IEEE80211_AMPDU_RX_START,
	ATBM_IEEE80211_AMPDU_RX_STOP,
	ATBM_IEEE80211_AMPDU_TX_START,
	ATBM_IEEE80211_AMPDU_TX_STOP,
	ATBM_IEEE80211_AMPDU_TX_OPERATIONAL,
};

enum ATBM_SCAN_TYPE{
	ATBM_SCAN_24G_ACTIVE,
	ATBM_SCAN_24G_PASSAVE,
	ATBM_SCAN_5G_PASSAVE_DFS,
	ATBM_SCAN_5G_PASSAVE,
	ATBM_SCAN_5G_ACTIVE,
	ATBM_SCAN_5G_ACTIVE_DFS,
	ATBM_SCAN_FAST_LINK,
	ATBM_SCAN_24G_WPS,
	ATBM_SCAN_5G_WPS,
	ATBM_SCAN_P2P,
	ATBM_SCAN_MAX_TYPE,
};

#define ATBM_MAX_SCAN_CHANNELS 14

struct atbm_scan_req{
	struct atbm_list_head list;
	struct wsm_scan scan;
	enum ATBM_SCAN_TYPE req_type;
	ATBM_BOOL scan_again;
	ATBM_BOOL scan_end;
};

/**/
struct atbm_cfg80211_connect_params {
	atbm_uint32 crypto_pairwise;
	atbm_uint32 crypto_group;
	atbm_uint32 crypto_igtkgroup;
	atbm_uint8 *bssid;
	atbm_uint8 key[32];//ap or sta 
	atbm_uint8 ptk[32];//sta just
	atbm_uint8 gtk[32];//sta just
	atbm_uint8 ptk_pn[8][8];//sta just
	atbm_uint8 ptk_pn_init[8];
	atbm_uint8 gtk_pn[8];//sta just
	atbm_uint8 gtk_pn_init;
	atbm_uint8 ptk_noqos_pn[8];//sta just
	atbm_uint8 ptk_noqos_pn_init;
	atbm_uint8 key_len;//ap or sta 
	atbm_uint8 key_idx;//ap or sta 
	atbm_uint8 key_idx_igtk;
	atbm_uint8 encrype;//ap or sta 
	atbm_uint8 update;
};

struct atbmwifi_cfg80211_rate {
	atbm_uint32 basic_rates;
	atbm_uint32 support_rates;
	atbm_uint32 hw_support_rates;
	struct atbmwifi_ieee80211_sta_ht_cap ht_cap;
	struct atbm_ieee80211_sta_vht_cap vht_cap;
	struct atbm_ieee80211_sta_he_cap he_cap;
	atbm_uint16 ht;
	atbm_uint16 channel_type; /*enum atbm_nl80211_channel_type*/
	atbm_uint8 chip_category;
    atbm_uint8 n_rates;       //add for tx status
	atbm_uint16 *txs_retrys;
};

struct atbmwifi_sta_priv {
	int link_id;
	struct atbmwifi_vif *priv;
	
	atbm_uint8 mac[6];
	atbm_int8 rssi;
	//struct ieee80211_sta_ht_cap ht_cap;
	ATBM_BOOL wme;
	atbm_uint8 uapsd_support_queues;
	atbm_uint8 max_sp;
	atbm_uint8 reserved2[2];
	atbm_uint32 driver_buffered_tids;
	atbm_uint32 flags;
	///

	atbm_uint16 beacon_interval;
	atbm_uint16 capability;
	
	atbm_uint32 wpa:1,
		wps:1,
	    p2p:1,
	    bcm_ap:1,
	    uapsd_supported:1,
	    wmm_used:1,
	    has_erp_value:1,
	    rate_11g:1,
	    ht:1,
	    short_preamble:1,
	    sgi:1,
		ieee_80211w:1,
		he_used:1,
		vht:1;

	struct atbmwifi_cfg80211_rate rate;
	
	atbm_void * sta_rc_priv;/*rate control priv for station link to my AP*/

	atbm_spinlock_t lock;
	atbm_uint16 sequence_number;
	atbm_uint8 timer_to_tid[STA_TID_NUM];
	int rx_token[STA_TID_NUM];
	atbm_uint16 tid_seq[ATBM_IEEE80211_QOS_CTL_TID_MASK + 1];
	struct sta_ampdu_mlme ampdu_mlme;

	enum atbmwifi_ieee80211_band band;
	atbm_void *reserved; //struct hostapd_sta_info * sta; initial in  hostapd_link_sta_sm()

};

struct atbmwifi_cfg80211_bss {
	atbm_uint16 channel_num;	
	atbm_uint8 channel_type/*enum atbm_nl80211_channel_type*/; //now used channel
	atbm_uint8 dtim_period;

	atbm_uint8 bssid[ATBM_ETH_ALEN];
	atbm_uint8 aid;	
	atbm_uint8 parameter_set_count; 
	/*
	 * Maximum number of buffered frames AP can deliver during a
	 * service period, IEEE80211_WMM_IE_STA_QOSINFO_SP_ALL or similar.
	 * Needs a new association to take effect.
	 */
	atbm_uint8 uapsd_max_sp_len; //IEEE80211_DEFAULT_MAX_SP_LEN;
	atbm_int8 rssi;
	atbm_uint8 reserved[2];

	atbm_uint32 use_cts_prot:1,
		use_short_slot:1,
	    ps_enabled:1,
	    arp_filter_enabled:1,
		ht_40M:1,	
		greenfield:1,
		short_gi:1,
		b_eapol:1,
		privacy:1,
		qos:1,
		nontransmit:1,
		is_wifi6_ap:1,
		uora_exists:1,
		ema_ap:1;
	atbm_uint8 transbssid[ATBM_ETH_ALEN];
	atbm_uint8 bssid_index;
	atbm_uint8 max_bssid_indicator;
	atbm_uint8 htc_trig_based_pkt_ext;
	atbm_uint8 uora_ocw_range;
	atbm_uint8 profile_periodicity;
	struct atbm_he_bss_color he_bss_color;
	struct {
		atbm_uint32 params;
		atbm_uint16 nss_set;
	} he_oper;
	struct atbm_ieee80211_he_obss_pd he_obss_pd;
	atbm_uint16 frame_time_rts_th;
	int dynamic_ps_timeout;
	int retry_short;
	int retry_long;
	struct atbmwifi_sta_priv sta_priv; 
	int len_information_elements;
	atbm_uint8 *information_elements;
};

struct response{
		atbm_uint32 len;
		atbm_uint32 respbuff[31];
};
struct atbmwifi_mac80211 {
	const char *rate_control_algorithm;
	atbm_void *priv;
	atbm_uint32 flags;
	unsigned int extra_tx_headroom;
	int channel_change_time;
	int vif_data_size;
	int sta_data_size;
	int napi_weight;
	atbm_uint16 queues;
	atbm_uint16 max_listen_interval;
	atbm_int8 max_signal;
	atbm_uint8 max_rates;
	atbm_uint8 max_report_rates;
	atbm_uint8 max_rate_tries;
	atbm_uint8 max_rx_aggregation_subframes;
	atbm_uint8 max_tx_aggregation_subframes;
	atbm_uint8 offchannel_tx_hw_queue;
	atbm_uint8 vendcmd_nl80211;
	struct response vendreturn;
};
/* flags used in struct ieee80211_if_managed.flags */
enum ieee80211_sta_flags {
	ATBM_IEEE80211_STA_BEACON_POLL	= ATBM_BIT(0),
	ATBM_IEEE80211_STA_CONNECTION_POLL	= ATBM_BIT(1),
	ATBM_IEEE80211_STA_CONTROL_PORT	= ATBM_BIT(2),
	ATBM_IEEE80211_STA_DISABLE_11N	= ATBM_BIT(4),
	ATBM_IEEE80211_STA_CSA_RECEIVED	= ATBM_BIT(5),
	ATBM_IEEE80211_STA_MFP_ENABLED	= ATBM_BIT(6),
	ATBM_IEEE80211_STA_UAPSD_ENABLED	= ATBM_BIT(7),
	ATBM_IEEE80211_STA_NULLFUNC_ACKED	= ATBM_BIT(8),
	ATBM_IEEE80211_STA_RESET_SIGNAL_AVE	= ATBM_BIT(9),
};
enum ieee80211_bss_change {
	ATBM_BSS_CHANGED_ASSOC		= 1<<0,
	ATBM_BSS_CHANGED_ERP_CTS_PROT	= 1<<1,
	ATBM_BSS_CHANGED_ERP_PREAMBLE	= 1<<2,
	ATBM_BSS_CHANGED_ERP_SLOT		= 1<<3,
	ATBM_BSS_CHANGED_HT 		= 1<<4,
	ATBM_BSS_CHANGED_BASIC_RATES		= 1<<5,
	ATBM_BSS_CHANGED_BEACON_INT 	= 1<<6,
	ATBM_BSS_CHANGED_BSSID		= 1<<7,
	ATBM_BSS_CHANGED_BEACON 	= 1<<8,
	ATBM_BSS_CHANGED_BEACON_ENABLED = 1<<9,
	ATBM_BSS_CHANGED_CQM			= 1<<10,
	ATBM_BSS_CHANGED_IBSS		= 1<<11,
	ATBM_BSS_CHANGED_ARP_FILTER 	= 1<<12,
	ATBM_BSS_CHANGED_QOS			= 1<<13,
	ATBM_BSS_CHANGED_IDLE		= 1<<14,
	ATBM_BSS_CHANGED_SSID		= 1<<15,
	ATBM_BSS_CHANGED_PS 		= 1<<16,
	ATBM_BSS_CHANGED_CHANNEL		= 1<<17, // XXX: COMBO: should this be merged with _HT?
	ATBM_BSS_CHANGED_RETRY_LIMITS	= 1<<18,
	ATBM_BSS_CHANGED_P2P_PS 	= 1<<19,
#ifdef IPV6_FILTERING
	ATBM_BSS_CHANGED_NDP_FILTER 	= 1<<20,

#endif /*IPV6_FILTERING*/

	/* when adding here, make sure to change ieee80211_reconfig */
};

#if CONFIG_ATBM_SUPPORT_CSA
struct atbm_ieee80211_csa_ie {
	atbm_uint8 mode;
	atbm_uint8 new_ch_num;
	atbm_uint8 count;
} __packed;
#endif /* CONFIG_ATBM_SUPPORT_CSA */

atbm_uint8 * atbmwifi_ieee80211_add_wpa_ie(struct atbmwifi_vif *priv,atbm_uint8 * eid);
atbm_uint8 * atbmwifi_ieee80211_add_ht_ie(struct atbmwifi_vif *priv,struct atbmwifi_ieee80211_supported_band *band,atbm_uint8 * eid);
atbm_uint8 * atbmwifi_ieee80211_add_ht_operation(struct atbmwifi_vif *priv, atbm_uint8 *eid);
int atbmwifi_ieee80211_data_to_8023(struct atbm_buff *skb, const atbm_uint8 *addr,enum atbm_nl80211_iftype iftype);
unsigned int  atbmwifi_ieee80211_hdrlen(atbm_uint16 fc);
int atbmwifi_ieee80211_frequency_to_channel(int freq);
int atbmwifi_ieee80211_channel_to_frequency(int chan, enum atbmwifi_ieee80211_band band);
int atbmwifi_ieee80211_data_from_8023(struct atbm_buff *skb, const atbm_uint8 *addr,
			     enum atbm_nl80211_iftype iftype, atbm_uint8 *bssid, ATBM_BOOL qos,atbm_uint8 encrype);
atbm_void atbmwifi_ieee80211_connection_loss(struct atbmwifi_vif *priv);
int atbmwifi_ieee80211_rx_irqsafe(struct atbmwifi_vif *priv,struct atbm_buff *skb);
atbm_void atbmwifi_ieee80211_ht_cap_ie_to_sta_ht_cap(struct atbmwifi_ieee80211_supported_band *sband,
				       struct atbmwifi_ieee80211_ht_cap *ht_cap_ie,
				       struct atbmwifi_ieee80211_sta_ht_cap *ht_cap);
atbm_uint8 *atbmwifi_find_ie(atbm_uint8 eid, const atbm_uint8 *ies, int len);
int ieee80211_tx_h_sequence(struct atbmwifi_sta_priv *sta_priv, struct atbm_buff *skb, int tid);
atbm_void atbmwifi_ieee80211_get_sta_rateinfo(struct atbmwifi_cfg80211_rate *rate,struct atbmwifi_ieee80211_supported_band *band,atbm_uint8 *supp_rates,int supp_rates_len);
atbm_uint8 * atbmwifi_ieee80211_add_rate_ie(atbm_uint8 * pos,ATBM_BOOL no_cck,atbm_uint32 mask);
atbm_uint8 * atbmwifi_ieee80211_add_rate_ie_from_ap(atbm_uint8 * pos,ATBM_BOOL no_cck,atbm_uint32 mask, atbm_uint32 basic_mask);
atbm_void atbmwifi_ieee80211_channel_country(struct atbmwifi_common *hw_priv,int country);
#if CONFIG_VHT
atbm_uint8 *atbmwifi_ieee80211_ie_build_vht_cap(const struct atbm_ieee80211_sta_vht_cap *vht_cap, atbm_uint8 *pos, int nsts);
atbm_uint8 *ieee80211_add_vht_ie(          struct atbmwifi_ieee80211_supported_band *sband,
               struct atbm_ieee80211_vht_cap *ap_vht_cap,
               atbm_uint8 *pos);
atbm_uint8 * atbmwifi_ieee80211_ie_build_vht_oper(struct atbmwifi_vif *priv, atbm_uint8 *eid);
void atbmwifi_ieee80211_vht_cap_ie_to_sta_vht_cap(struct atbmwifi_ieee80211_supported_band *band,
					const struct atbm_ieee80211_vht_cap *vht_cap_ie,
				    struct atbm_ieee80211_sta_vht_cap *vht_cap);
#endif
atbm_uint8 *ieee80211_add_op_mode_ie(struct atbmwifi_ieee80211_supported_band *sband, atbm_uint8 band_width, atbm_uint8 *pos);
#if CONFIG_HE
atbm_uint8 *atbmwifi_ieee80211_ie_build_he_cap(struct atbm_ieee80211_sta_he_cap *he_cap, atbm_uint8 *pos);
atbm_uint8 *ieee80211_ie_build_he_oper(atbm_uint8 *pos);
atbm_uint8 *atbmwifi_ieee80211_ie_build_ext_capab(atbm_uint8 *buf, atbm_uint8 *extended_capa, int extended_capa_len);
atbm_void atbmwifi_ieee80211_ht_cap_ie_to_sta_ht_cap(struct atbmwifi_ieee80211_supported_band *sband,
				       struct atbmwifi_ieee80211_ht_cap *ht_cap_ie,
				       struct atbmwifi_ieee80211_sta_ht_cap *ht_cap);
atbm_void atbmwifi_ieee80211_he_cap_ie_to_sta_he_cap(struct atbmwifi_ieee80211_supported_band *sband,
				  const atbm_uint8 *he_cap_ie, atbm_uint8 he_cap_len,
				  struct atbm_ieee80211_sta_he_cap *he_cap);
atbm_void ieee80211_he_op_ie_to_bss_conf(struct atbmwifi_vif *priv,
			const struct atbm_ieee80211_he_operation *he_op_ie);
atbm_void ieee80211_he_spr_ie_to_bss_conf(struct atbmwifi_vif *priv,
				const struct atbm_ieee80211_he_spr *he_spr_ie_elem);
#endif
atbm_void atbmwifi_ieee80211_parse_qos(struct atbmwifi_vif *priv,struct atbm_buff *skb);
atbm_void atbmwifi_ieee80211_deliver_skb(struct atbmwifi_vif *priv,struct atbm_buff *skb,atbm_uint16 *need_free);
struct atbmwifi_cfg *atbmwifi_get_config(struct atbmwifi_vif *priv);
atbm_void atbmwifi_start_wifimode(struct atbmwifi_vif *start_priv,enum atbm_nl80211_iftype start_type);
atbm_void atbmwifi_stop_wifimode(struct atbmwifi_vif *stop_priv,enum atbm_nl80211_iftype stop_type);
struct atbmwifi_vif *atbmwifi_config_get_priv(struct atbmwifi_cfg *config);
ATBM_BOOL atbmwifi_iee80211_check_combination(struct atbmwifi_vif *ignore_priv,atbm_uint8 combination_channel);
atbm_void atbmwifi_iee80211_unify_channel_type(struct atbmwifi_vif *ignore_priv, atbm_uint32 channel_type);
int atbmwifi_iee80211_peerif_channel_type(struct atbmwifi_vif *ignore_priv);
struct atbmwifi_vif * atbmwifi_iee80211_getvif_by_name
	(struct atbmwifi_common	*hw_priv,char *name);
ATBM_BOOL atbmwifi_ieee80211_check_alive_if(struct atbmwifi_common	*hw_priv,struct atbmwifi_vif *ignore_priv);

int atbmwifi_buffed_timeout(struct atbmwifi_vif *priv);
#if CONFIG_WPS
atbm_uint8 * atbmwifi_ieee80211_add_preq_wps_ie(struct atbmwifi_vif *priv,atbm_uint8 * pos);
atbm_uint8 * atbmwifi_ieee80211_add_assocreq_wps_ie(struct atbmwifi_vif *priv,atbm_uint8 * pos);
#endif
void ___ieee80211_stop_rx_ba_session(struct atbmwifi_sta_priv *sta_priv, atbm_uint16 tid,
				     atbm_uint16 initiator, atbm_uint16 reason, ATBM_BOOL stop);
void __ieee80211_stop_rx_ba_session(struct atbmwifi_sta_priv *sta_priv, atbm_uint16 tid,
				    atbm_uint16 initiator, atbm_uint16 reason, ATBM_BOOL stop);

void ieee80211_sta_tear_down_BA_sessions(struct atbmwifi_sta_priv *sta_priv, ATBM_BOOL tx);
void ieee80211_process_delba(struct atbmwifi_common *hw_priv,
			     struct atbmwifi_sta_priv *sta_priv,
			     struct atbmwifi_ieee80211_mgmt *mgmt, int len);
void ieee80211_process_addba_request(struct atbmwifi_common *hw_priv,
				     struct atbmwifi_sta_priv *sta_priv,
				     struct atbmwifi_ieee80211_mgmt *mgmt,
				     int len);
void ieee80211_process_addba_resp(struct atbmwifi_common *hw_priv,
				  struct atbmwifi_sta_priv *sta_priv,
				  struct atbmwifi_ieee80211_mgmt *mgmt,
				  atbm_size_t len);
int __ieee80211_stop_tx_ba_session(struct atbmwifi_sta_priv *sta_priv, atbm_uint16 tid,
				   enum atbmwifi_ieee80211_back_parties initiator,
				   ATBM_BOOL tx);
int ___ieee80211_stop_tx_ba_session(struct atbmwifi_sta_priv *sta_priv, atbm_uint16 tid,
				    enum atbmwifi_ieee80211_back_parties initiator,
				    ATBM_BOOL tx);
int ieee80211_stop_tx_ba_session(struct atbmwifi_sta_priv *sta_priv, atbm_uint16 tid);
void ieee80211_tx_ba_session_handle_start(struct atbmwifi_sta_priv *sta_priv, int tid);
void ieee80211_ba_session_work(struct atbm_work_struct *work);
int ieee80211_start_tx_ba_session(struct atbmwifi_sta_priv *sta_priv, atbm_uint16 tid,
				  atbm_uint16 timeout);
ATBM_BOOL ieee80211_tx_prep_agg(struct atbmwifi_sta_priv *sta_priv,
			   struct atbm_buff *skb,
			   struct atbmwifi_ieee80211_tx_info * tx_info,
			   struct tid_ampdu_tx *tid_tx,
			   int tid);

atbm_void atbmwifi_tx_aggr_queued_nolock(struct atbmwifi_vif *priv, struct atbmwifi_sta_priv *sta_priv,
		struct tid_ampdu_tx *tid_tx, int tid);
atbm_void atbmwifi_tx_aggr_queue_purge(struct atbmwifi_sta_priv  *sta_priv);


void sta_addba_resp_timer_expired(atbm_void *data1,atbm_void *data2);
void sta_rx_agg_session_timer_expired(atbm_void *data1,atbm_void *data2);

struct tcpip_opt{
	atbm_void (*net_init)(struct atbm_net_device *dev);
	atbm_void (*net_enable)(struct atbm_net_device *dev);
	atbm_void (*net_disable)(struct atbm_net_device *dev);
	atbm_uint32 (*net_rx)(struct atbm_net_device *dev, struct atbm_buff * skb);
	atbm_void (*net_tx_done)(struct atbm_net_device *dev);
	atbm_void (*net_start_queue)(struct atbm_net_device *dev,int);
	atbm_void (*net_stop_queue)(struct atbm_net_device *dev,int);
	atbm_void (*net_task_event)(struct atbm_net_device *dev);	
};
struct atbm_net_device_ops{
	int	(*ndo_open)(struct atbmwifi_vif *priv);
	int (*ndo_stop)(struct atbmwifi_vif *priv);
	int  (*ndo_start_xmit)(struct atbmwifi_vif *priv,struct atbm_buff *skb );
	int (*ndo_set_mac_address)(struct atbmwifi_vif *priv,atbm_uint8 *addr);
};
extern struct tcpip_opt * tcp_opt;

static inline ATBM_BOOL ieee80211_opclass_to_band(atbm_uint8 operating_class,
	enum atbmwifi_ieee80211_band *band)
{
	switch (operating_class) {
	case 112:
	case 115 ... 127:
	case 128 ... 130:
		*band = ATBM_IEEE80211_BAND_2GHZ;
		return ATBM_TRUE;
	case 81:
	case 82:
	case 83:
	case 84:
		*band = ATBM_IEEE80211_BAND_5GHZ;
		return ATBM_TRUE;
	}

	return ATBM_FALSE;
}

/**
 * ffs - find first bit set
 * @x: the word to search
 *
 * This is defined the same way as
 * the libc and compiler builtin ffs routines, therefore
 * differs in spirit from the above ffz (man ffs).
 */
static inline int atbm_ffs(int x)
{
	int r = 1;

	if (!x)
		return 0;
	if (!(x & 0xffff)) {
		x >>= 16;
		r += 16;
	}
	if (!(x & 0xff)) {
		x >>= 8;
		r += 8;
	}
	if (!(x & 0xf)) {
		x >>= 4;
		r += 4;
	}
	if (!(x & 3)) {
		x >>= 2;
		r += 2;
	}
	if (!(x & 1)) {
		x >>= 1;
		r += 1;
	}
	return r;
}
static inline int atbm_fls(int x)
{
	int r = 32;

	if (!x)
		return 0;
	if (!(x & 0xffff0000u)) {
		x <<= 16;
		r -= 16;
	}
	if (!(x & 0xff000000u)) {
		x <<= 8;
		r -= 8;
	}
	if (!(x & 0xf0000000u)) {
		x <<= 4;
		r -= 4;
	}
	if (!(x & 0xc0000000u)) {
		x <<= 2;
		r -= 2;
	}
	if (!(x & 0x80000000u)) {
		x <<= 1;
		r -= 1;
	}
	return r;
}

#endif   /*ATBMWIFI_NET_H*/

