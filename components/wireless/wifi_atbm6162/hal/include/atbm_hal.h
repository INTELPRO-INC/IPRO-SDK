/**************************************************************************************************************
 * altobeam RTOS wifi hmac source code 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 *
 *  The source code contains proprietary information of AltoBeam, and shall not be distributed, 
 *  copied, reproduced, or disclosed in whole or in part without prior written permission of AltoBeam.
*****************************************************************************************************************/

#ifndef ATBMWIFI_HAL_H
#define ATBMWIFI_HAL_H
#include "atbm_config.h"





#define APOLLO_1505  0
#define APOLLO_1601  1
#define APOLLO_1606  0
#define APOLLO_C     2
#define APOLLO_D     3
#define APOLLO_E     4
#define APOLLO_F     5
#define ATHENA_B     6
#define ATHENA_LITE  7
#define ATHENA_LITE_ECO  8
#define ARES_A  	 9
#define ARES_B  	 10
#define HERA         11
#define CRONUS       12
#define CRONUS_NO_HT40   13
#define CRONUS_NO_HT40_LDPC   14
#define CRONUS_NO_BLE   15
#define RHEA         16
#define OCEANUS      17
#define CRONUS_LITE      18
#define OCEANUS_NO_HT40   19
#define OCEANUS_NO_HT40_LDPC   20
#define OCEANUS_NO_BLE   21
#define CRONUS_LITE_NO_HT40   22
#define CRONUS_LITE_NO_HT40_LDPC   23
#define CRONUS_LITE_NO_BLE   24
#define OCEANUS_NO_WIFI6   25
	
#define CHIP_VERSION_MAX     (OCEANUS_NO_WIFI6)


#define ATBM_HZ  1000

#define atbm_packed __attribute__((packed))
#define atbm_align __attribute__((aligned(4)))
#define __INLINE inline
#define ATBM_KERN_DEBUG  
#define ATBM_KERN_ERR	
#define ATBM_KERN_INFO	

#define CH_OFF_20 ATBM_NL80211_CHAN_NO_HT

#define ATBM_WIFI_MAX_VIFS			2
#define ATBM_WIFI_GENERIC_IF_ID		(2)
#define ATBM_WIFI_MAX_QUEUE_SZ		(64)
#define ATBM_WIFI_HOST_VIF0_11N_THROTTLE	(ATBM_WIFI_MAX_QUEUE_SZ-2)
#define ATBM_WIFI_HOST_VIF1_11N_THROTTLE	(ATBM_WIFI_MAX_QUEUE_SZ-2)
#define WSM_KEY_MAX_IDX		20
#define ATBMWIFI__MAX_CTRL_FRAME_LEN	(0x1000)
#define WLAN_LINK_ID_MAX				16//(ATBMWIFI__MAX_STA_IN_AP_MODE + 3)
#define ATBMWIFI__LINK_ID_AUTH		(0)	//this is define in lmac LINKENABLE must 15,can't change 
#define ATBMWIFI__MAX_REQUEUE_ATTEMPTS	(5)
#define ATBMWIFI__MAX_TID			(8)
#define ATBMWIFI__TX_BLOCK_ACK_ENABLED_FOR_ALL_TID         (0xFF)
#define ATBMWIFI__RX_BLOCK_ACK_ENABLED_FOR_ALL_TID         (0xFF)
#define ATBMWIFI__RX_BLOCK_ACK_ENABLED_FOR_BE_TID \
	(ATBMWIFI__TX_BLOCK_ACK_ENABLED_FOR_ALL_TID & 0x01)
#define ATBMWIFI__TX_BLOCK_ACK_DISABLED_FOR_ALL_TID	(0)
#define ATBMWIFI__RX_BLOCK_ACK_DISABLED_FOR_ALL_TID	(0)
#define ATBMWIFI__BLOCK_ACK_CNT		(30)
#define ATBMWIFI__BLOCK_ACK_THLD		(800)
#define ATBMWIFI__BLOCK_ACK_HIST		(3)
#define ATBMWIFI__BLOCK_ACK_INTERVAL	(1 * ATBM_HZ / ATBMWIFI__BLOCK_ACK_HIST)
#define ATBM_WIFI_ALL_IFS			(-1)
#define WIFI_TIMBITMAP_LEN			((ATBMWIFI__MAX_STA_IN_AP_MODE/8)+2)
#define ATBMWIFI__INVALID_RATE_ID (0xFF)

#define ATBMWIFI_SCAN_CONNECT_AP_SUCCESS (0x99)

#define ATBMWLAN	"wlan0"
#define ATBMP2P		"p2p0"

/*0 - Mixed, 1 - Greenfield*/
#define MODE_11N_MIXED			0
#define MODE_11N_GREENFIELD		1

/* Please keep order */
enum atbmwifi_join_status {
	ATBMWIFI__JOIN_STATUS_PASSIVE = 0,
	ATBMWIFI__JOIN_STATUS_MONITOR,
	ATBMWIFI__JOIN_STATUS_STA,
	ATBMWIFI__JOIN_STATUS_AP,
};

enum atbmwifi_link_status {
	ATBMWIFI__LINK_OFF,
	ATBMWIFI__LINK_RESERVE,
	ATBMWIFI__LINK_SOFT,
	ATBMWIFI__LINK_HARD,
};

enum atbmwifi_bss_loss_status {
	ATBMWIFI__BSS_LOSS_NONE,
	ATBMWIFI__BSS_LOSS_CHECKING,
	ATBMWIFI__BSS_LOSS_CONFIRMING,
	ATBMWIFI__BSS_LOSS_CONFIRMED,
};

/**
 * enum sta_notify_cmd - sta notify command
 *
 * Used with the sta_notify() callback in &struct ieee80211_ops, this
 * indicates if an associated station made a power state transition.
 *
 * @STA_NOTIFY_SLEEP: a station is now sleeping
 * @STA_NOTIFY_AWAKE: a sleeping station woke up
 */
enum sta_notify_cmd {
	STA_NOTIFY_SLEEP, STA_NOTIFY_AWAKE,
};
/*region_id*/
enum {
	country_chinese =0,
	country_usa = 1,
	country_europe =2,
	country_japan =3,
	country_canada = 4,
	country_australia = 5,
	country_Israel =6,
	country_brazil =7,
	country_SINGAPORE =8,
	region_taiwan =9,
};
#include "atbm_type.h"
#include "atbm_sysops.h"
#include "os_net_type.h"
#include "atbm_thread.h"
#include "atbm_debug.h"
#include "atbm_skbuf.h"
#include "atbm_proto.h"
#include "atbm_wsm.h"
#include "atbm_queue.h"
#include "atbm_task.h"
#include "atbm_mac80211.h"
#include "atbm_api.h"
#include "smartconfig.h"
//#include "atbm_ratectrl.h"
#if ATBM_USB_BUS
#include "atbm_usb.h"
#endif

#if ATBM_SDIO_BUS
#include "atbm_sdio.h"
#endif
#include "atbm_mgmt.h"
#if ATBM_USB_BUS
#include "atbm_usb_fwio.h"
#include "atbm_usb_hwio.h"
#endif

#if ATBM_SDIO_BUS
#include "atbm_sdio_fwio.h"
#include "atbm_sdio_hwio.h"
#endif
#include "atbm_sbus.h" 
#include "atbm_wsm.h"  
#include "atbm_wifi_driver_api.h"
#include "atbm_key.h"  

#include "aes.h"
#include "crypto.h"

#if CONFIG_WPS
#include "wps_i.h"
#include "dh_group5.h"
#endif
#if CONFIG_P2P
#include "p2p_main.h"
#endif

#include "hostapd_main.h"
#include "wpa_auth_i.h"
#include "atbm_timer.h"
#include "wpa_event.h"
#include "wpa_common.h"
#include "wpa_main.h"
#include "wpa_i.h"
#include "hostapd_main.h"

#if CONFIG_SAE
#include "sae.h"
#include "pmksa_cache.h"
#endif

#include "dh_groups.h"
#include "wpabuf.h"
#include "sha256.h"
#include "hostapd_sta_info.h"
#include "wpa_supplicant_i.h"
#include "atbm_sha1.h"
#include "wpa_debug.h"
#include "atbm_special.h"
/*************************/
/*timeout define*/
#if FAST_CONNECT_MODE || FAST_CONNECT_NO_SCAN
#define ATBM_WIFI_AUTH_TIMEOUT  (2500) //ms
#else
#define ATBM_WIFI_AUTH_TIMEOUT  (5*1000) //ms
#endif
#if FAST_CONNECT_MODE || FAST_CONNECT_NO_SCAN
#define ATBM_WIFI_AUTH_ASSOC_TIMEOUT  (2) //s
#else
#define ATBM_WIFI_AUTH_ASSOC_TIMEOUT  (11) //s
#endif

#define ATBMWIFI__LINK_ID_UNMAPPED		(17)

#ifndef MIN
#define MIN(x,y) ((x)<(y)?(x):(y))
#endif

struct atbmwifi_filter_retry{
	atbm_uint16 last_rx_seq[ATBM_NUM_RX_DATA_QUEUES + 1];
	unsigned long num_duplicates;
};

struct atbmwifi_link_entry {
	enum atbmwifi_link_status		status;
	atbm_uint8				mac[ATBM_ETH_ALEN];
	atbm_uint8				buffered[ATBMWIFI__MAX_TID];
	struct atbmwifi_filter_retry sta_retry;
	struct atbmwifi_sta_priv  sta_priv;
};

struct atbm_mac_address {
	atbm_uint8 addr[ATBM_ETH_ALEN];
};

struct atbmwifi_scan {
	atbm_int8	ApScan_in_process;
	atbm_int8 in_progress;
	struct wsm_ssid ssids;
	atbm_work scan_work;
	atbm_work ap_scan_work;
	atbm_int8 output_power;
	atbm_uint8 status;
	atbm_uint8 direct_probe;
	atbm_uint8 scan_smartconfig;
	atbm_uint8 if_id;
	struct atbm_list_head scan_req_list;
	struct atbm_scan_req *cur_scan_req;
	atbm_uint32 dfs_map[50];
	atbm_uint8 reserved[3];
};

#if CONFIG_P2P
#define P2P_DEFAULT_LISTEN_CHAN 1
#define P2P_DEFAULT_OPER_CHAN 1

#define CONFIG_MAX_NAME_LEN 32
#define P2P_DEFAULT_FIX_SSID "altobeam-p2p"
#define P2P_DEFAULT_FIX_SSID_LEN 12
#endif

#if CONFIG_ATBM_SUPPORT_CSA
enum atbm_ieee80211_csa_state {
	CSA_IDLE,
	CSA_WAIT_START,
	CSA_WAIT_COMPLETE,
};

enum atbm_ATBM_IEEE80211_CSA_MLME_STATE{
	ATBM_IEEE80211_CSA_MLME_STATE_IDLE,
	ATBM_IEEE80211_CSA_MLME_STATE_STOP,
	ATBM_IEEE80211_CSA_MLME_STATE_WK,
	ATBM_IEEE80211_CSA_MLME_STATE_START,
	ATBM_IEEE80211_CSA_MLME_STATE_TIMEOUT,
	ATBM_IEEE80211_CSA_MLME_STATE_FAIL,
	ATBM_IEEE80211_CSA_MLME_STATE_FINISHED,
};

struct atbm_ieee80211_csa_request{
	bool start;
	bool cfg80211;
	atbm_uint8 chan;
	enum atbm_nl80211_channel_type chantype;
	struct atbm_ieee80211_csa_ie ie;
	atbm_uint64 mactime;
};

struct ieee80211_csa_work{
	struct atbm_list_head list;
	struct atbmwifi_vif *vif;
	bool block;
	bool cfg80211;
	atbm_uint64  mactime;
	atbm_uint8   count;
	atbm_uint8 chan;
	enum atbm_nl80211_channel_type chan_type;
};

struct atbm_ieee80211_csa_mlme {
	enum atbm_ATBM_IEEE80211_CSA_MLME_STATE state;
	/* after csa channel and channel type */
	struct ieee80211_channel *chan;
	/* offset counter in beacon and presp */
	u16 counter_offsets_beacon;
	u16 counter_offsets_presp;
	/* use for csa */
	struct beacon_data *csa_beacon;
	/* use for csa after */
	struct beacon_data *beacon_after;
	/* default ATBM_PROBE_RESP_EXTRA_IE */
	struct proberesp_data *csa_proberesp;
	struct proberesp_data *proberesp_after;

	bool block_tx;
	u8 count;
};

struct atbm_ieee80211_csa_status
{
	atbm_uint8 n_suces;
	atbm_uint8 n_fails;
};
#endif /* CONFIG_ATBM_SUPPORT_CSA */

struct atbmwifi_cfg {
	atbm_uint8 ssid[ATBM_IEEE80211_MAX_SSID_LEN];
	atbm_uint8 ssid_len;
	atbm_uint8 bssid[ATBM_ETH_ALEN];
	atbm_uint8 password_len;
	atbm_uint8 password[64];
	atbm_uint8 privacy;
	atbm_uint8	psk_set;
	atbm_uint8	bssid_set;
	atbm_uint8	fixrate_set;
	atbm_uint8 mode;
	atbm_uint8 hw_fix_rate_id;
	atbm_uint8 reserve;
	atbm_uint8 channel_index;
	ATBM_BOOL hide_ssid;
	atbm_uint8 	beaconInterval;
	atbm_uint8 	DTIMPeriod;
	atbm_uint8 	band;
	atbm_uint8 	preambleType;
	unsigned long 	basicRateSet;
//#ifdef WPA_SUPPLICANT
	int pairwise_cipher;
	int n_pairwise_cipher;
	int group_cipher;
	int wpa;
	atbm_uint8  psk[32];
//#endif
	atbm_uint32 key_mgmt;
	atbm_uint8 key_id;
	atbm_uint16 auth_alg;
#if CONFIG_WPS
	atbm_uint8 dev_password[12];
	atbm_int32 dev_password_len;
	atbm_int32 pbc;
	atbm_int32 pin;
#endif
#if CONFIG_P2P
	//int proto;
	atbm_uint32 p2p_listen_channel_set:1,
		p2p_oper_channel_set:1;
	int p2p_listen_channel;
	int p2p_oper_channel;
	int p2p_listen_reg_class;
	int p2p_oper_reg_class;
	char device_name[CONFIG_MAX_NAME_LEN];
	char manufacturer[CONFIG_MAX_NAME_LEN];
	char model_name[CONFIG_MAX_NAME_LEN];
	char model_number[CONFIG_MAX_NAME_LEN];
	char serial_number[CONFIG_MAX_NAME_LEN];
	char pri_dev_type[WPS_DEV_TYPE_LEN];
	char country[3];
	int p2p_go_intent;
#endif /*CONFIG_P2P*/
	int secondary_channel;
	int flags;
	char *sae_password_id;
#if CONFIG_IEEE80211W
	enum atbm_mfp_options ieee80211w;
	int group_mgmt_cipher;
	int sae_require_mfp;
#endif /* CONFIG_IEEE80211W */
	int max_sta_count;
	ATBM_BOOL filter_enable;
    atbm_uint16 filter_mode; //0 white 1 black
    atbm_uint16 mac_count;
    atbm_uint8 mac_list[16][ATBM_ETH_ALEN];
	ATBM_BOOL disable_ofdm;
	ATBM_BOOL disable_ht;
	ATBM_BOOL disable_vht;
	ATBM_BOOL disable_he;
	ATBM_BOOL disallow_broadcast;
	ATBM_BOOL disallow_unicast;
#if CONFIG_ATBM_SUPPORT_CSA
	atbm_uint8 cs_count;
	atbm_uint8 block_tx;
#endif
};

#define MAX_SCAN_INFO_NUM 80
struct atbmwifi_scan_result_info{
	atbm_uint8 ssid[33];
	atbm_uint8 ssidlen;
	atbm_uint8 channel;
	atbm_uint8 dtim_period;
	atbm_int8 security;
	atbm_int8 rssi;
	atbm_uint8 BSSID[6];
	atbm_uint16 capability;
	atbm_uint16 beacon_interval;
	atbm_uint32 ht:1,
	   wpa:1,
	   rsn:1,
	   wps:1,
	   p2p:1,
	   b40M:1,
	   encrypt:1;
};

struct atbmwifi_scan_result{
	int len;
	int taken;
	struct atbmwifi_scan_result_info *info;
};
enum{
	WAIT_AUTH_1,
	WAIT_AUTH_2,
	WAIT_AUTH_3,
	WAIT_AUTH_4,
};

enum ieee80211_packet_rx_flags {
	IEEE80211_RX_IN_SCAN			= ATBM_BIT(0),
	IEEE80211_RX_RA_MATCH			= ATBM_BIT(1),
	IEEE80211_RX_FRAGMENTED			= ATBM_BIT(2),
	IEEE80211_RX_AMSDU			= ATBM_BIT(3),
	IEEE80211_RX_MALFORMED_ACTION_FRM	= ATBM_BIT(4),
	IEEE80211_RX_DEFERRED_RELEASE		= ATBM_BIT(5),
	IEEE80211_RX_ERP_BEACON			= ATBM_BIT(6),
};


#define RX_CONTINUE		 (0)
#define RX_DROP_UNUSABLE (1)

#define RX_DROP_MONITOR	 (2)
#define RX_QUEUED		 (3)


/* IEEE 802.11 (Ch. 9.5 Defragmentation) requires support for concurrent
 * reception of at least three fragmented frames. This limit can be increased
 * by changing this define, at the cost of slower frame reassembly and
 * increased memory use (about 2 kB of RAM per entry). */
#define IEEE80211_FRAGMENT_MAX 4

struct ieee80211_fragment_entry {
	unsigned long first_frag_time;
	unsigned int seq;
	unsigned int rx_queue;
	unsigned int last_frag;
	unsigned int extra_len;
	struct atbm_buff_head skb_list;
	int ccmp; /* Whether fragments were encrypted with CCMP */
	atbm_uint8 last_pn[6]; /* PN of the last fragment if CCMP was used */
};

#ifdef PACK_STRUCT_USE_INCLUDES
#include "arch/bpstruct.h"
#endif //PACK_STRUCT_BEGIN


#if ATBM_PKG_REORDER
#define BUFF_STORED_LEN		64
#define ATBM_RX_DATA_QUEUES	8
#define REORDER_TIMER_RUNING	(0)
#define BAR_TID_EN				(0)
#define REORDER_TIMEOUT				(ATBM_HZ/10)
#define SEQ_NUM_MASKER		(0xfff)
#define PER_PKG_TIME_INTERVAL	(10)
#define AMPDU_REORDER_TIME_INTERVAL	(200)

#define ATBM_BA__ACTION_RX_ADDBR		(1)
#define ATBM_BA__ACTION_RX_DELBA		(2)
#define ATBM_BA__ACTION_RX_BAR		(3)
struct atbm_ba_params
{
	atbm_uint8 action;
	atbm_uint8 link_id;
	atbm_uint8 win_size;
	atbm_uint8 tid;
	atbm_uint16 ssn;
	atbm_uint16 timeout;
	struct atbmwifi_sta_priv *sta_priv;
};

struct atbm_ba_tid_params
{
	struct atbm_buff *skb_reorder_buff[BUFF_STORED_LEN];
	struct atbm_buff_head header;
	atbm_uint32 frame_rx_time[BUFF_STORED_LEN];
	OS_TIMER overtime_timer;
	atbm_mutex	skb_reorder_spinlock;
	atbm_uint32   timer_running;
	atbm_uint16 ssn;
	atbm_uint8  wind_size;
	atbm_uint8  index_tail;
	atbm_uint8  skb_buffed;
	atbm_uint8  reserved[3];
	atbm_uint16	start_seq;
	atbm_uint16 timeout;
	atbm_uint16 session_timeout;
	atbm_uint32  tid_en;
	atbm_void *reorder_priv;
	struct atbmwifi_sta_priv *sta_priv;
};
struct atbm_reorder_queue_comm
{	
	struct atbm_ba_tid_params		atbm_rx_tid[ATBM_RX_DATA_QUEUES];
	atbm_uint8			link_id;
	atbm_mutex	reorder_mutex;
	//struct work_struct		  reorder_work;
};
atbm_void atbm_updata_ba_tid_params(struct atbmwifi_vif *priv,struct atbm_ba_params *ba_params);
#define skb_move_to(src,des,tskb)		\
			while((tskb = __atbm_skb_dequeue(src)))	\
				__atbm_skb_queue_tail(des, tskb)
int atbm_reorder_skb_queue(struct atbmwifi_vif *priv,struct atbm_buff *skb,atbm_uint8 link_id);
atbm_void atbm_reorder_func_init(struct atbmwifi_vif *priv);
atbm_void atbm_reorder_func_deinit(struct atbmwifi_vif *priv);
atbm_void atbm_reorder_func_reset(struct atbmwifi_vif *priv,atbm_uint8 link_id);

#endif
/*rateCtrl struct*/
struct tx_policy {
	union {
		atbm_uint32 tbl[3];
		atbm_uint8 raw[12];
	};
	atbm_uint8  defined;		/* TODO: atbm_uint32 or atbm_uint8, profile and select best */
	atbm_uint8  usage_count;	/* --// -- */
	atbm_uint8  retry_count;	/* --// -- */
	atbm_uint8  uploaded;
};

struct tx_policy_cache_entry {
	struct tx_policy policy;
	struct atbm_list_head link;
};

#define TX_POLICY_CACHE_SIZE	(8)
struct tx_policy_cache {
	struct tx_policy_cache_entry cache[TX_POLICY_CACHE_SIZE];
	struct atbm_list_head used;
	struct atbm_list_head free;
	atbm_spinlock_t lock;
	
};
atbm_void tx_policy_init(struct atbmwifi_common *hw_priv);
atbm_void tx_policy_upload_work(struct atbm_work_struct *work);
struct atbmwifi_key_t
{
	struct wsm_add_key		key;
	atbm_uint8  linkid;
	atbm_uint8  if_id;
	atbm_uint8 	valid;
	atbm_uint8  reserved;
};

enum CHIP_CATEGORY{
	CRONUS_CATEGORY,
	OCEANUS_CATEGORY,
	OLITE_CATEGORY,
	CLITE_CATEGORY,
	DEMETER_CATEGORY,
	COEUS_CATEGORY
};

enum ATBM_TXRX_OPS_VERSION{
	ATBM_TXRX_OPS_V0,
	ATBM_TXRX_OPS_V1,
	ATBM_TXRX_OPS_MAX,
};

struct atbm_chip_priv{
	ATBM_BOOL support_5g;
	ATBM_BOOL support_vht;
	ATBM_BOOL disable_tb; /*Workaround for mubar trigger*/
	ATBM_BOOL hdr_rollback;
	enum CHIP_CATEGORY chip_cat;
	struct firmware_altobeam fw_altobeam;
	atbm_uint32 reset_reg;
	atbm_uint32 reset_bit;
	atbm_uint32 mem_access_bit;
	enum ATBM_TXRX_OPS_VERSION hif_version;
	int download_bs;
	int tx_channel_num;
	int rx_channel_num;
};

struct atbmwifi_common{	
	struct atbmwifi_queue		tx_queue[4];
	struct atbmwifi_queue_stats	tx_queue_stats;	
	struct atbmwifi_vif		*vif_list[ATBM_WIFI_MAX_VIFS];
	atbm_uint8						vif_current;
	/*Thread struct*/
	atbm_thread_internal_t *bh_thread;
	atbm_thread_internal_t * tx_bh_thread;
	atbm_thread_internal_t * rx_bh_thread;
	atbm_thread_internal_t *cmpl_bh_thread;
	atbm_thread_internal_t *atbm_sdio_tx_thread;
	atbm_thread_internal_t *atbm_sdio_rx_thread;
	//struct atbmwifi_mac80211 *atbm_net;
	/*connect param*/
	//struct atbmwifi_cfg *config;
	atbm_os_wait_queue_head_t wsm_startup_done;
	atbm_os_wait_queue_head_t wsm_cmd_wq;
	atbm_os_wait_queue_head_t wake_up_wq;
	ATBM_BOOL suspend_rx;
	atbm_mutex wsm_cmd_mux;
	atbm_atomic_t bh_rx;
	atbm_atomic_t bh_tx;
	atbm_atomic_t queue_num;
	atbm_mutex sleep_mutex;
	atbm_atomic_t wait_timer_start;
	atbm_atomic_t urb_comp;
	
	struct atbm_list_head tx_urb_cmp;
	int bh_term;
	int bh_suspend;
	int bh_error;
	/* BBP/MAC state */
	atbm_uint8 mac_addr[ATBM_ETH_ALEN];
	struct atbm_mac_address		addresses[ATBM_WIFI_MAX_VIFS];
	atbm_uint8 reserved1[2];
	struct atbmwifi_ieee80211_rate		*rates;
	struct atbmwifi_ieee80211_rate		*mcs_rates;
	atbm_uint8 ba_tid_tx_mask;
	atbm_uint8 ba_tid_rx_mask;	
	atbm_uint8 max_rx_aggregation_subframes;
	atbm_uint8 max_tx_aggregation_subframes;
	atbm_uint16 max_hw_support_rx_aggs;
	atbm_uint16 max_hw_support_tx_aggs;
	atbm_spinlock_t aggr_lock;
	atbm_uint32 rx_aggr_map[BITS_TO_U32S(ATBM_IEEE80211_MAX_RX_AGGR)];
	atbm_uint32 tx_aggr_map[BITS_TO_U32S(ATBM_IEEE80211_MAX_TX_AGGR)];

    /*country XXX*/
	atbm_uint8 			   region_id;
	atbm_uint8 			   region_code[3];
	/*config*/
	atbm_uint8 				band;
	atbm_uint8				long_frame_max_tx_count;
	atbm_uint8				short_frame_max_tx_count;
	atbm_uint8				channel_type;
	atbm_uint8 				max_rates;
	atbm_uint8 				max_rate_tries;
	atbm_uint8				conf_listen_interval;
	atbm_uint8				wsm_rx_seq;	/* byte */
	atbm_uint8				wsm_tx_seq;	/* byte */
	atbm_uint8				hw_bufs_used;/*have send tx or cmd to lmac, and lmac not complete*/
	int             hw_bufs_free;
	ATBM_BOOL				hw_buf_enough;
	atbm_uint32 			n_xmits;
	atbm_uint32				hw_xmits;

	ATBM_BOOL				device_can_sleep;

	ATBM_BOOL	bh_rx_nobuffer;
	atbm_uint8		bh_rx_nobuffer_cnt;
	atbm_uint8 reserved2[2];
	struct wsm_buf			wsm_cmd_buf;
	struct wsm_cmd			wsm_cmd;
	struct wsm_cbc			wsm_cbc;
	atbm_uint32				pending_frame_id;
	/* TX/RX */
	unsigned long		rx_timestamp;
	atbm_uint32       		if_id_selected;
	atbm_uint32				key_map;
	struct atbmwifi_key_t		keys[WSM_KEY_MAX_INDEX + 2];
	atbm_uint16				vif0_throttle;
	atbm_int16				tx_burst_idx;
	
	//atbm_uint16				vif1_throttle;
	struct sbus_ops *sbus_ops;
	struct sbus_priv		*sbus_priv;
	int init_done;
	atbm_spinlock_t				event_queue_lock;
	struct atbm_list_head 		event_queue;	
	struct atbm_buff_head			rx_frame_queue;
	struct atbm_buff_head			rx_frame_free;
	struct atbm_buff_head			tx_frame_queue;
	struct atbm_buff_head			tx_frame_free;
	atbm_spinlock_t tx_com_lock;
	atbm_spinlock_t rx_com_lock;
	atbm_uint32 wsm_txconfirm_num;
	atbm_uint32 channel_switch_in_progress;
	atbm_uint32 wsm_txframe_num;
	///////////////////////////////////////////
	#if QUEUE_LIST
	struct atbmwifi_work_struct workqueue;
	struct atbm_list_head work_Link;	
	#else
	struct atbmwifi_work_struct work_queue_table[ATBM_WIFI_MAX_WORKQUEUE];
	atbm_uint32 		work_map;
	#endif
	atbm_thread_internal_t *work_queue_thread;
	int hw_queues;
	
	/////////atbm timer
	atbm_thread_internal_t *timer_thread;
	int timer_term;
	////////////////////////////


	
	//////////////////////////////////////
	//ETF
	atbm_uint32 etf_channel;
	atbm_uint32 etf_channel_type;
	atbm_uint32 etf_rate;
	atbm_uint32 etf_len;	
	atbm_uint32 etf_greedfiled;
	atbm_uint8 bStartTx;
	atbm_uint8 bStartTxWantCancel;
	atbm_uint8 etf_test_v2;
	OS_TIMER etf_tx_end_work;
	//////////////////////////////////////
	//SDIO
	atbm_uint32 buf_id_rx;
	atbm_uint32 buf_id_tx;
	atbm_uint32 buf_id_tx_offset;
	atbm_uint32 buf_id_rx_offset;
	struct wsm_caps	 wsm_caps;
	atbm_int8 fw_lable[130];
	struct atbm_buff *skb_cache;
	atbm_uint32 syncChanl_done;
	#if ATBM_SDIO_BUS && ATBM_SDIO_READ_ENHANCE
	atbm_mutex sdio_rx_process_lock;
	#endif
	atbm_work wsm_sync_channl;
	atbm_os_wait_queue_head_t wsm_synchanl_done;
	/*Ap Auto channle select*/	
	atbm_uint8 busy_ratio[14];
	/*For AP ScanEnd Flag */
	atbm_uint32 ApScan_process_flag;
#if CONFIG_P2P
	int p2p_if_id;
#endif
	atbm_uint8 *xmit_buff;
    struct efuse_headr efuse;
	ATBM_BOOL driver_setup_ba;
	ATBM_BOOL lmc_wakeup_host;
	struct atbmwifi_ieee80211_supported_band *bands[ATBM_IEEE80211_NUM_BANDS];
#if CONFIG_WIFI_BT_COMB
	struct ieee80211_ble_local ble_local;
#endif
#if SUPPORT_LIGHT_SLEEP
	atbm_uint32 ps_flag;
	atbm_uint32 last_active_time;
#endif
	struct atbm_chip_priv *chip_priv;
	int chip_id;
	int chip_ver;
	int chip_version;
	char loader_ble;
#if CONFIG_ATBM_SUPPORT_CSA
	atbm_work csa_work;
	enum atbm_ieee80211_csa_state next_csa_state;
	struct atbm_list_head csa_pending;
	struct atbm_list_head csa_req;
	//struct atbm_list_head csa_notify;
#endif
	///////////////////////////
};

/* Virtual Interface State. One copy per VIF */
struct atbmwifi_vif {
	int			enabled;
	int				if_id;
	struct atbm_net_device *ndev;
	struct atbmwifi_common	*hw_priv;
	atbm_spinlock_t			vif_lock;
	atbm_spinlock_t 			ps_state_lock;
	/* BBP/MAC state */
	atbm_uint8 bssid[ATBM_ETH_ALEN];
	atbm_uint8 daddr[ATBM_ETH_ALEN];
	atbm_uint8 mac_addr[ATBM_ETH_ALEN]; //self mac addr
	atbm_uint8 ssid[ATBM_IEEE80211_MAX_SSID_LEN];
	atbm_uint8 ssid_length;
	atbm_uint8 whitelist_enable;
	atbm_uint8 whitelist[ATBMWIFI__MAX_STA_IN_AP_MODE][ATBM_ETH_ALEN];
	struct wsm_edca_params		edca;
	struct wsm_association_mode	association_mode;
	//struct wsm_set_bss_params	bss_params;
	struct wsm_set_pm		powersave_mode;
	struct wsm_uapsd_info		uapsd_info;
	atbm_uint32				cqm_use_rssi:1,
					enable_beacon:1,
	 				listening:1,
	 				disable_beacon_filter:1,	
	 				aid0_bit_set:1,/* multicast  frame int beacon DTIM have been up data to lmac*/
	 				buffered_multicasts:1,/*have buffered multicast  frame*/
	 				tx_multicast:1,/*must send multicast  now*/
	 				htcap:1,
	 				powersave_enabled:1,
	 				assoc_ok:1,
	 				connect_ok:1,
	 				scan_no_connect:1,
					auto_connect_when_lost:1,
					connect_expire:1,
					scan_no_connect_back:1,
#if CONFIG_WPS
					pbc:1,
					pin:1,
#endif
					ht_operation_mode:1,
					doing_stop_off:1,
					special_scan:1,
                    special_beacon_set:1;
	atbm_uint8				user_pm_mode;
	enum atbm_nl80211_iftype 	iftype;
	/* Scan status */
	struct atbmwifi_scan scan;
	/* WSM Join */
	enum atbmwifi_join_status	join_status;
	/* Security */
	atbm_int8					wep_default_key_id;
	atbm_uint8 *           	 	extra_ie;
	int             	extra_ie_len;
	atbm_uint8 *                    special_extra_ie;
    int                     special_extra_ie_len;
	/*connect param*/
	struct atbmwifi_scan_result scan_ret;
	struct wsm_rx_filter		rx_filter;
	struct wsm_beacon_filter_control bf_control;
	atbm_work	join_work;
	atbm_work event_handler;
	atbm_work rx_task_work;	
	atbm_work chantype_switch_work;
	atbm_uint8		net_enable;
	atbm_uint8		net_queue_enable;
	struct atbmwifi_cfg config;
	/*wmm*/
	atbm_uint8 uapsd_queues;
	atbm_uint8 uapsd_max_sp_len;
	atbm_uint8 wmm_acm;
	struct config_edca_params		 wmm_params[4]; 
	/* AP powersave */	
	atbm_uint8			tim_vbitmap[4];
	atbm_uint8			tid_id_uapsd;
	atbm_uint8			reserved11;
	int 		pspending_sta_num;
	atbm_uint32			link_id_map;
	atbm_uint32			link_id_after_dtim;/*default :0*/
	atbm_uint32			link_id_uapsd_mask;
	atbm_uint32			link_id_max;	
	atbm_uint32			link_id_uapsd;
	struct atbmwifi_link_entry link_id_db[ATBMWIFI__MAX_STA_IN_AP_MODE+1];//link_table_index = aid-1=link_id-1;
	atbm_uint32				sta_asleep_mask;
	atbm_uint32				buffered_set_mask;
	atbm_uint32				pspoll_mask;
	OS_TIMER  mcast_timeout;
	atbm_work	set_tim_work;
	atbm_work 	event_work;
	atbm_work 	set_omi_work;
	struct atbmwifi_cfg80211_bss  bss;//used to save the config for the direct ,not save self config here
	struct atbm_cfg80211_connect_params connect;
	struct atbm_buff_head rx_task_skb_list;	
	/*ATBM_BIT 0~bit6 expire delay count*/
	atbm_uint8		scan_expire;
	atbm_uint8      connect_timer_linkid;
	atbm_uint8      connect_state;
	atbm_uint8      reserved12;
	OS_TIMER connect_expire_timer;
	OS_TIMER scan_expire_timer;
	OS_TIMER chswitch_timer;
	atbm_uint8	 if_name[ATBM_IFNAMSIZ];
	atbm_void *appdata;//for wpa_supplicant or hostapd;
#if CONFIG_WPS
	int wpa_key_mgmt;
	atbm_uint8 *wps_beacon_ie;
	atbm_uint32 wps_beacon_ie_len;
	atbm_uint8 *wps_probe_resp_ie;
	atbm_uint32 wps_probe_resp_ie_len;
#endif
#if CONFIG_P2P
	char p2p_pin[10];
	int p2p_wps_method;
	atbm_uint32 p2p_auto_join:1,
			p2p_go_neg_process:1,
			user_initiated_pd:1,
			p2p_join:1,
			p2p_scan:1,
			p2p_listen:1,
			p2p_ap:1,
			p2p_auto_go_req:1;
	int p2p_go_intent;
	int p2p_join_scan_count;
	atbm_void *p2p_scan_ie;
	atbm_void *p2p_go_beacon_ie;
	atbm_void *p2p_go_probe_resp_ie;
	atbm_uint8 *p2p_assoc_req_ie;
	atbm_uint16 p2p_assoc_req_ie_len;
	atbm_uint8 join_iface_addr[ATBM_ETH_ALEN];
	atbm_uint8 join_dev_addr[ATBM_ETH_ALEN];
	atbm_void *p2p_wps_privkey;
	atbm_void *p2p_wps_pubkey;
	atbm_os_msgq p2p_task_msg;
	atbm_void *p2p_task_msg_stack;
	atbm_void *p2p_task;
#endif
#if ATBM_PKG_REORDER
	struct atbm_reorder_queue_comm atbm_reorder_link_id[ATBMWIFI__MAX_STA_IN_AP_MODE];
#endif
#if FAST_CONNECT_MODE
	int fast_connect;
	atbm_uint8 fast_channel;
#endif
#if FAST_CONNECT_NO_SCAN
	int fast_conn_noscan;
	int auth_retry;
#endif
	/*SmartConfig varible*/
	struct smartconfig_config  st_cfg;
	enum smartconfig_status    st_status;
	int   st_configchannel;
	OS_TIMER smartconfig_expire_timer;
#if CONFIG_P2P
	atbm_void *p2pdata;
	atbm_void *p2pgroup;
#ifdef CONFIG_WIFI_DISPLAY
	int wifi_display;
#define MAX_WFD_SUBELEMS 12
	struct wpabuf *wfd_subelem[MAX_WFD_SUBELEMS];
#endif
#endif
	/* CSA configuration */
#if CONFIG_ATBM_SUPPORT_CSA
	enum atbm_ieee80211_csa_state csa_state;
	struct atbm_ieee80211_csa_mlme csa_mlme;
#if CONFIG_ATBM_SUPPORT_CHANSWITCH_TEST
	struct atbmwifi_ieee80211_channel_sw_ie sw_ie;
#endif
#endif
};

#ifdef PACK_STRUCT_USE_INCLUDES
#include "arch/epstruct.h"
#endif

#ifdef fallthrough
#define atbm_fallthrough fallthrough
#else
#define atbm_fallthrough
#endif

extern  struct atbmwifi_ieee80211_supported_band atbmwifi_band_2ghz;
extern  struct atbmwifi_ieee80211_rate atbmwifi_rates[];
extern  struct atbmwifi_ieee80211_rate atbmwifi_rates_5g[];
extern atbm_uint8 default_macaddr[];
extern const int ieee802_1d_to_ac[8];

enum atbmwifi_data_filterid {
	IPV4ADDR_FILTER_ID = 0,
};
#define ATBM_WIFI_B_RATE_SIZE   4
#define ATBM_WIFI_RATE_SIZE   	12
#define atbmwifi_a_rates		(atbmwifi_rates_5g)
#define atbmwifi_a_rates_size	(ATBM_WIFI_RATE_SIZE - ATBM_WIFI_B_RATE_SIZE)
#define atbmwifi_g_rates		(atbmwifi_rates + 0)
#define atbmwifi_g_rates_size	ATBM_WIFI_RATE_SIZE//(ATBM_ARRAY_SIZE(atbmwifi_rates))
#define atbm_n_rates		(atbm_mcs_rates)
#define atbm_n_rates_size	(ATBM_ARRAY_SIZE(atbm_mcs_rates))

#if CONFIG_P2P
#define atbmwifi_is_sta_mode(iftype) (((iftype) == ATBM_NL80211_IFTYPE_STATION)||((iftype) == ATBM_NL80211_IFTYPE_P2P_CLIENT))
#define atbmwifi_is_ap_mode(iftype)	(((iftype) == ATBM_NL80211_IFTYPE_AP)||((iftype) == ATBM_NL80211_IFTYPE_P2P_GO))
#else
#define atbmwifi_is_sta_mode(iftype) ((iftype) == ATBM_NL80211_IFTYPE_STATION)
#define atbmwifi_is_ap_mode(iftype)	((iftype) == ATBM_NL80211_IFTYPE_AP)
#endif

#define atbmwifi_vht_supported(chip_priv) ((chip_priv)->support_vht)
#define atbmwifi_5g_supported(chip_priv) ((chip_priv)->support_5g)
#define atbmwifi_disbale_ap_tx_trigger(chip_priv) ((chip_priv)->disable_tb)

atbm_void atbmwifi_ieee80211_buid_machdr(
	struct atbmwifi_vif *priv,
	struct atbmwifi_ieee80211_hdr *wh,
	int type,
	const atbm_uint8 sa[ATBM_IEEE80211_ADDR_LEN],
	const atbm_uint8 da[ATBM_IEEE80211_ADDR_LEN],
	const atbm_uint8 bssid[ATBM_IEEE80211_ADDR_LEN]);
//struct atbmwifi_sta_priv *atbmwifi_sta_find(struct atbmwifi_vif *priv,const atbm_uint8 *mac);
int atbmwifi_find_link_id(struct atbmwifi_vif *priv, const atbm_uint8 *mac);
int atbm_inital_common(struct atbmwifi_vif *priv);
int  atbm_wifi_initialed(atbm_uint8 if_id);
atbm_void  atbmwifi_netstack_init(struct atbmwifi_common *hw_priv);
atbm_void  atbmwifi_netstack_deinit(atbm_void);
atbm_void net_device_ops_init(atbm_void);
int atbmwifi_tx_policy_init(struct atbmwifi_common *hw_priv);
struct atbmwifi_sta_priv * atbmwifi_sta_find(struct atbmwifi_vif *priv,const atbm_uint8 *sta_mac);
#define atbm_for_each_vif(_hw_priv, _priv, _i)			\
		for (									\
			_i = 0;								\
			(_i < ATBM_WIFI_MAX_VIFS) && (_priv =_atbmwifi_hwpriv_to_vifpriv(_hw_priv,_i)); 			\
			_i++								\
		)
struct atbmwifi_common *_atbmwifi_vifpriv_to_hwpriv(struct atbmwifi_vif *priv);
struct atbmwifi_vif *_atbmwifi_hwpriv_to_vifpriv(struct atbmwifi_common *hw_priv,int if_id);
atbm_void atbm_ps_notify(struct atbmwifi_vif *priv,int link_id, ATBM_BOOL ps);
int atbmwifi_scan_start(struct atbmwifi_vif *priv);
int atbmwifi_autoconnect(struct atbmwifi_vif *priv, int time);
void atbm_scan_work(struct atbm_work_struct *work);
void atbm_join_work(struct atbm_work_struct *work);
int atbm_policy_upload_work(struct atbm_work_struct *work);
int atbm_event_handler(struct atbm_work_struct *work);
atbm_void atbm_ap_set_tim_work(struct atbm_work_struct *work);
atbm_void atbmwifi_scan_comlete(struct atbmwifi_vif *priv);
atbm_void atbm_clear_scan_req(struct atbmwifi_vif *priv);
int atbm_req_scan_start(struct atbmwifi_vif *priv);
 atbm_void atbmwifi_init_scan_req(struct atbmwifi_vif *priv, enum ATBM_SCAN_TYPE type);
 atbm_void atbmwifi_join_start(struct atbmwifi_vif *priv);
 atbm_void atbmwifi_skb_dtor(struct atbmwifi_common *hw_priv,
		     struct atbm_buff *skb,
		     const struct atbmwifi_txpriv *txpriv);
atbm_void atbmwifi_sta_set_buffered(struct atbmwifi_sta_priv *sta_priv,
				atbm_uint32 tid, ATBM_BOOL buffered);
int smartconfig_start_rx(struct atbmwifi_vif *priv,struct atbm_buff *skb,int channel);
 atbm_void atbmwifi_scan_complete_cb(struct atbmwifi_common *hw_priv,
				int interface_link_id,struct wsm_scan_complete *arg);
 atbm_void atbmwifi_tx_confirm_cb(struct atbmwifi_common *hw_priv,
			  struct wsm_tx_confirm *arg,int if_id,	int link_id);
 atbm_void atbmwifi_rx_cb(struct atbmwifi_vif *priv,
		  struct wsm_rx *arg,
		  struct atbm_buff **skb_p,int link_id);
atbm_void atbm_suspend_resume(struct atbmwifi_vif *priv,
			   struct wsm_suspend_resume *arg);
atbm_void atbm_rx_task_work(struct atbm_work_struct *work);

atbm_void atbmwifi_set_channel_work(struct atbm_work_struct *work);
int atbmwifi_ieee80211_sta_wmm_params(struct atbmwifi_vif *priv,
										atbm_uint8 *wmm_param, int wmm_param_len,
										const struct atbm_ieee80211_mu_edca_param_set * mu_edca);
 atbm_void atbmwifi_sw_chntimeout(atbm_void *data1,atbm_void *data2);

 int atbmwifi_alloc_link_id(struct atbmwifi_vif *priv, const atbm_uint8 *mac);
 int atbmwifi_sta_del(struct atbmwifi_vif *priv, atbm_uint8 * staMacAddr, ATBM_BOOL tx);
 int atbmwifi_find_hard_link_id(struct atbmwifi_vif *priv, const atbm_uint8 *mac);
 atbm_void atbmwifi_link_id_lmac(struct atbmwifi_vif *priv,int link_id, ATBM_BOOL mfp);
 int _atbmwifi_unmap_link(struct atbmwifi_vif *priv, int link_id);

 struct atbmwifi_sta_priv *atbmwifi_sta_find_form_hard_linkid(struct atbmwifi_vif *priv,const atbm_uint8 linkid);
 struct atbmwifi_sta_priv *atbmwifi_sta_find_form_linkid(struct atbmwifi_vif *priv,const atbm_uint8 linkid);
int atbm_set_tim_impl(struct atbmwifi_vif *priv);
/*wsm.c*/
atbm_void atbm_wsm_event_work(struct atbm_work_struct *work);
void wsm_sync_channl_reset(struct atbm_work_struct *work);

/********sta.c****/
atbm_void atbmwifi_set_pm(struct atbmwifi_vif *priv,ATBM_BOOL ps_enabled,atbm_uint8 dynamic_ps_timeout);
atbm_void atbm_sta_deauth(struct atbmwifi_vif *priv, ATBM_BOOL tx);
int atbmwifi_sta_scan(struct atbmwifi_vif *priv);
atbm_void atbmwifi_stop_sta(struct atbmwifi_vif *priv);
atbm_void atbmwifi_start_sta(struct atbmwifi_vif *priv);
atbm_void atbm_set_omi_work(struct atbm_work_struct *work);
/********ap.c****/
atbm_void atbmwifi_stop_ap(struct atbmwifi_vif *priv);
 atbm_void atbmwifi_start_ap(struct atbmwifi_vif *priv);

/***key**/
 atbm_void atbmwifi_del_key(struct atbmwifi_vif *priv,int pairwise,int linkid);
 int atbmwifi_set_key(struct atbmwifi_vif *priv,int pairwise,int linkid);
/***main**/

 int Atbmwifi_halEntry(struct sbus_priv *sbus);
/*txrx*/
 int atbmwifi_ieee80211_rx_h_amsdu(struct atbmwifi_vif *priv,struct atbm_buff *skb);
 atbm_void atbmwifi_tx_start(struct atbm_buff *skb,struct atbmwifi_vif *priv);
 int atbmwifi_tx_fast(struct atbm_buff *skb, struct atbm_buff *prev_skb, struct atbmwifi_vif *priv);
 int atbmwifi_tx_slow(struct atbm_buff *skb,struct atbmwifi_vif *priv, const struct atbmwifi_txpriv *txpriv);
 int atbmwifi_tx_queue(struct atbmwifi_common *hw_priv, struct atbm_buff *skb,struct atbmwifi_vif *priv);
void atbm_sta_supp_rate(struct atbmwifi_ieee80211_supported_band *sband,struct atbmwifi_sta_priv *sta_priv,struct wsm_supp_rate *rate);
unsigned int atbm_cfg80211_classify8021d(struct atbm_buff *skb);
#if NEW_SUPPORT_PS
atbm_void atbmwifi_recal_tim_to_lmac(struct atbmwifi_vif *priv,int link_id);
int atbmwifi_deliver_poll_response(struct atbmwifi_vif *priv,struct atbmwifi_ieee80211_hdr * hdr,int link_id);
int atbmwifi_deliver_uapsd_response(struct atbmwifi_vif *priv,struct atbmwifi_ieee80211_hdr *hdr,int link_id,int tid);
#endif
//util.c
 atbm_uint8 * atbmwifi_ieee80211_add_wmm_param(struct atbmwifi_vif *priv, atbm_uint8 *eid);
 atbm_void atbmwifi_ieee80211_send_deauth_disassoc(struct atbmwifi_vif *priv,
					   const atbm_uint8 *da,const atbm_uint8 *bssid, atbm_uint16 stype, atbm_uint16 reason,
					   atbm_void *cookie, ATBM_BOOL send_frame);

 atbm_uint8 *atbmwifi_ieee80211_add_wme(struct atbmwifi_vif *priv, atbm_uint8 *eid);
 atbm_void wifi_ChangePsMode(struct atbmwifi_vif *priv,atbm_uint8 enable,atbm_uint8 ds_timeout);
 atbm_void atbmwifi_disconnect_vif(struct atbmwifi_vif *priv);
 atbm_void AT_WDisConnect(char *pLine);
 int atbmwifi_scan_process(struct atbmwifi_vif	*priv);

//ap api
 int atbmwifi_sta_alloc(struct atbmwifi_vif *priv, atbm_uint8 *sta_mac);
 int atbmwifi_sta_add(struct atbmwifi_vif *priv,  atbm_uint8 *sta_mac);
int atbm_set_tim(struct atbmwifi_vif *priv, struct atbmwifi_sta_priv  *sta_priv,ATBM_BOOL set);
 int atbm_start_ap(struct atbmwifi_vif *priv);
int	atbm_autoChann_Select(struct atbmwifi_vif *priv,atbm_uint8 *SetChan);
atbm_void atbmwifi_ap_scan_start(struct atbm_work_struct *work);
int atbmwifi_ap_deauth(struct atbmwifi_vif *priv, atbm_uint8 * staMacAddr, ATBM_BOOL tx);
int atbmwifi_ap_start_proberesp(struct atbmwifi_vif *priv);
int atbmwifi_ap_start_beacon(struct atbmwifi_vif *priv);

// sta api
 int atbmwifi_assoc_success(struct atbmwifi_vif *priv,struct atbm_buff *skb);
//
 int atbmwifi_get_hard_linked_macs(struct atbmwifi_vif *priv,  atbm_uint8 *mac, atbm_uint32 maclen);

struct atbm_net_device *atbm_netintf_init(atbm_void);
atbm_void atbmwifi_tx(struct atbmwifi_common *hw_priv, struct atbm_buff *skb,struct atbmwifi_vif *priv);
 int atbmwifi_set_uapsd_param(struct atbmwifi_vif *priv,
				const struct wsm_edca_params *arg);
int atbmwifi_setup_mac(struct atbmwifi_common *hw_priv);
int atbmwifi_vif_setup(struct atbmwifi_vif *priv);
int atbmwifi_setup_mac_pvif(struct atbmwifi_vif *priv);
atbm_void atbmwifi_update_filtering(struct atbmwifi_vif *priv);

int atbmwifi_iee80211_peerif_channel(struct atbmwifi_vif *ignore_priv);

void atbm_forced_memzero(void *ptr, atbm_size_t len);
void atbm_bin_clear_free(void *bin, atbm_size_t len);
void atbm_buf_shift_right(atbm_uint8 *buf, atbm_size_t len, atbm_size_t bits);
int random_get_bytes(atbm_uint8 *buf, atbm_size_t len);

#if CONFIG_ATBM_SUPPORT_CSA
atbm_void atbm_ieee80211_sta_process_chanswitch(struct atbmwifi_vif *priv,struct atbmwifi_cfg80211_bss *bss,
	struct atbmwifi_ieee80211_channel_sw_packed_ie *sw_packed_ie, atbm_uint64 timestamp);
#if CONFIG_ATBM_SUPPORT_CHANSWITCH_TEST
unsigned char *ieee80211_sta_swbeacon_add(struct atbmwifi_vif *priv,struct atbm_buff *skb);
#endif
int atbm_do_csa_concurrent(struct atbmwifi_common *hw_priv, struct list_head *head);
int atbm_ieee80211_csa_busy(struct atbmwifi_common *hw_priv);
int atbm_ieee80211_start_csa_work(struct atbmwifi_vif *priv, struct atbm_ieee80211_csa_request *csa);
bool atbm_ieee80211_set_channel_type(struct atbmwifi_common *hw_priv,
	struct atbmwifi_vif *priv,enum atbm_nl80211_channel_type chantype);
void atbm_ieee80211_csa_flush(struct atbmwifi_common *hw_priv);
void atbm_ieee80211_csa_process_work(struct atbm_work_struct *work);
void atbm_ieee80211_event_csa_done(struct atbmwifi_vif *priv, atbm_uint32 status);
int atbmwifi_do_csa(struct atbmwifi_vif *priv,struct atbm_ieee80211_csa_request *request);
#endif /* CONFIG_ATBM_SUPPORT_CSA */
#endif /* ATBMWIFI_HAL_H */
