/**************************************************************************************************************
 * altobeam RTOS wifi hmac source code 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 *
 *  The source code contains proprietary information of AltoBeam, and shall not be distributed, 
 *  copied, reproduced, or disclosed in whole or in part without prior written permission of AltoBeam.
*****************************************************************************************************************/

#ifndef __HMAC_RATE_CTROL__
#define __HMAC_RATE_CTROL__
#include "atbm_hal.h"


#define atbm_printk_rc(...) wifi_printk(WIFI_RATE,__VA_ARGS__)
#define atbm_printk_err(...) wifi_printk(WIFI_DBG_ERROR,__VA_ARGS__)

#ifndef u8
typedef atbm_uint8 u8;
#endif
#ifndef u16
typedef atbm_uint16 u16;
#endif
#ifndef u32
typedef atbm_uint32 u32;
#endif
#ifndef s8
typedef atbm_int8 s8;
#endif
#ifndef s16
typedef atbm_int16 s16;
#endif
#ifndef s32
typedef atbm_int32 s32;
#endif
#ifndef bool
#define bool ATBM_BOOL
#endif

#ifndef gfp_t
typedef atbm_uint32 gfp_t;
#endif

#ifndef true
#define true 1
#endif

#ifndef le16_to_cpu
#define le16_to_cpu atbm_le16_to_cpu
#endif

#define atbm_GetOsTick() atbm_GetOsTime()
#define RX_FLAG_11G ATBM_RX_FLAG_11G
#define FLAG_HE_ER RX_FLAG_HE_ER
#define RX_FLAG_HE ATBM_RX_FLAG_HE
#define RX_FLAG_VHT ATBM_RX_FLAG_VHT
#define RX_FLAG_HT ATBM_RX_FLAG_HT
#define IEEE80211_BAND_5GHZ ATBM_NL80211_BAND_5GHZ
#define IEEE80211_BAND_2GHZ ATBM_NL80211_BAND_2GHZ

struct atbmn_ieee80211_rx_status{
	int flag;
	atbm_uint8  rate_idx;
	atbm_uint8  signal;
	enum atbmwifi_ieee80211_band band;
};

#ifndef LINUX_OS
static inline ATBM_BOOL time_after(atbm_uint32 x, atbm_uint32 y){
	if(x >= y)
		return ATBM_TRUE;
	if(x < y/2)
		return ATBM_TRUE;
	return ATBM_FALSE;
}

/* IPRO7 port: renamed from fls() to avoid clashing with newlib <strings.h>
 * int fls(int) (pulled transitively). Unused in this port. */
static inline int atbm_ratectrl_fls(int x)
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
#endif
#define HE_MODE_DOWN_TO_11B  (ATBM_BIT(0))
#ifndef min
#define min(x,y) atbm_min(x,y)
#endif


struct atbm_tx_rc_input
{
    u8 max_rate_idx;
    u8 lower;
    u8 manual_rate;
    u8 manual;
};

struct ieee80211_tx_rate_control {
	struct atbmwifi_ieee80211_supported_band *sband;
	struct atbmwifi_ieee80211_tx_info* info;
	struct atbm_tx_rc_input txrc_in;
};

struct atbm_ieee80211_tx_info_rc {
	/* common information */
	u8   tx_update_rate :1,
	     sample_pkt_flag :1;	
	struct atbm_tx_rc_output *control;		
};

struct atbm_ieee80211_tx_rate_control {
	struct atbm_ieee80211_tx_info_rc info;
    struct atbm_tx_rc_input *tx_rc_in; 
    void *priv;
};

struct atbm_ieee80211_sta {
	atbm_uint32 supp_rates[ATBM_IEEE80211_NUM_BANDS];		
    struct atbmwifi_ieee80211_sta_ht_cap *ht_cap;
    struct atbm_ieee80211_sta_vht_cap *vht_cap;
	struct atbm_ieee80211_sta_he_cap  *he_cap;	
    atbm_uint8 chip_category;
	atbm_uint8 n_rates;    
	atbm_uint16 *txs_retrys;    
};

struct atbmwifi_band_info{
    u32 center_freq;
    enum atbm_nl80211_channel_type channel_type;
};

struct atbm_rate_control_ops {
	const char* name;
	atbm_void (*rx_status)();
	atbm_void (*hmac_tx_status)(atbm_void *rc_priv, struct atbm_ieee80211_sta *atbm_sta, atbm_void *priv_sta);
	atbm_void (*get_rate)(atbm_void *rc_priv, atbm_void *priv_sta,struct atbm_ieee80211_tx_rate_control *atbm_txrc);
	atbm_void (*rate_init)(atbm_void *rc_priv, struct atbmwifi_band_info *band_info, struct atbm_ieee80211_sta *atbm_sta,atbm_void *priv_sta);
	atbm_void (*rate_update)(atbm_void *rc_priv, struct atbmwifi_band_info *band_info, struct atbm_ieee80211_sta *atbm_sta,atbm_void *priv_sta,
					atbm_uint32 changed, enum atbm_nl80211_channel_type atbm_oper_chan_type);
	atbm_void *(*alloc_sta)(atbm_void *rc_priv, struct atbm_ieee80211_sta *atbm_sta, gfp_t gpf);
	atbm_void (*free_sta)(atbm_void *rc_priv, atbm_void *priv_sta);
	atbm_void *(*alloc)(atbm_uint8 chip_cat);
	atbm_void (*free)(atbm_void *priv);
	atbm_void *rc_priv;
};

extern struct atbm_rate_control_ops *global_ops;


static inline void rate_control_rx_status(struct atbmwifi_sta_priv *sta_priv,
					  struct atbmwifi_ieee80211_rx_status * atbm_status)
{
	struct atbmn_ieee80211_rx_status rc_status;

	if (!(global_ops && sta_priv))
		return;

	atbm_spin_lock(&sta_priv->lock);

	if(!sta_priv->sta_rc_priv){
		atbm_spin_unlock(&sta_priv->lock);
		return;
	}

	if(atbm_status->rate_idx >= 38){
		 rc_status.flag |= ATBM_RX_FLAG_VHT;
		 rc_status.rate_idx  =  atbm_status->rate_idx - 38;
	 }else if(atbm_status->rate_idx >= 26){
		 rc_status.flag |= ATBM_RX_FLAG_HE;
		 rc_status.rate_idx =  atbm_status->rate_idx - 26;
	 }else if ( atbm_status->rate_idx >= 23) {
		 rc_status.flag |= RX_FLAG_HE_ER;
		 rc_status.rate_idx =  atbm_status->rate_idx - 23;
	 }else if ( atbm_status->rate_idx >= 14) {
		 rc_status.flag |= ATBM_RX_FLAG_HT;
		 rc_status.rate_idx =  atbm_status->rate_idx - 14;
	 } else if (atbm_status->rate_idx >= 4) {
		 rc_status.flag |= ATBM_RX_FLAG_11G;
		 if (atbm_status->band == ATBM_IEEE80211_BAND_5GHZ)
			 rc_status.rate_idx =  atbm_status->rate_idx - 6;
		 else
			 rc_status.rate_idx = atbm_status->rate_idx - 2;
	 } else {
		 rc_status.rate_idx =  atbm_status->rate_idx;
	 }  
	rc_status.signal = atbm_status->signal;
	rc_status.band = atbm_status->band;
	global_ops->rx_status(global_ops->rc_priv, sta_priv->sta_rc_priv, &rc_status);
	atbm_spin_unlock(&sta_priv->lock);
}


static inline void rate_control_tx_hmac(struct atbmwifi_sta_priv *sta_priv)
{
	void *priv_sta;
	struct atbm_ieee80211_sta atbm_sta; 

	if (!global_ops)
		return;

	if(sta_priv == NULL)
	{
		return;
	}

	atbm_spin_lock(&sta_priv->lock);
	priv_sta = sta_priv->sta_rc_priv;

	//atbm_sta = ( struct atbm_ieee80211_sta *)ista;
    
    atbm_sta.chip_category = sta_priv->rate.chip_category;
    atbm_sta.n_rates = sta_priv->rate.n_rates;
    atbm_sta.txs_retrys = sta_priv->rate.txs_retrys;
	//dump_mem(atbm_sta.txs_retrys, 144);

    atbm_sta.he_cap = &sta_priv->rate.he_cap;
    atbm_sta.ht_cap = &sta_priv->rate.ht_cap;
    atbm_sta.vht_cap = &sta_priv->rate.vht_cap;
    
	global_ops->hmac_tx_status(global_ops->rc_priv, &atbm_sta, priv_sta);
	atbm_spin_unlock(&sta_priv->lock);
}

static inline void rate_control_rate_init(struct atbmwifi_sta_priv *sta_priv,
			struct atbmwifi_ieee80211_supported_band *sband, atbm_uint16 channel_num,
			atbm_uint8 channel_type)
{
    struct atbmwifi_band_info band_info;
	struct atbm_ieee80211_sta atbm_sta; 

	atbm_spin_lock(&sta_priv->lock);
    band_info.center_freq = atbmwifi_ieee80211_channel_to_frequency(channel_num, sband->band);
    
    band_info.channel_type = (enum atbm_nl80211_channel_type)channel_type;

    atbm_sta.chip_category = sta_priv->rate.chip_category;
    atbm_sta.n_rates = sta_priv->rate.n_rates;
    atbm_sta.txs_retrys = sta_priv->rate.txs_retrys;
 
    atbm_sta.he_cap = &sta_priv->rate.he_cap;
    atbm_sta.ht_cap = &sta_priv->rate.ht_cap;
    atbm_sta.vht_cap = &sta_priv->rate.vht_cap;
    atbm_sta.supp_rates[sband->band] =  sta_priv->rate.support_rates;

#if (BW_40M_SUPPORT == 0)
    if((band_info.channel_type == ATBM_NL80211_CHAN_HT40PLUS)||(band_info.channel_type == ATBM_NL80211_CHAN_HT40MINUS))  
	{
		band_info.channel_type = ATBM_NL80211_CHAN_HT20;
	}
#endif

	//atbm_printk_rc(" rate_control_rate_init chip_category:%d\n", ista->chip_category);

	global_ops->rate_init(global_ops->rc_priv, &band_info, &atbm_sta, sta_priv->sta_rc_priv);
	atbm_spin_unlock(&sta_priv->lock);
}

#if 0
static inline void rate_control_rate_update(
					struct atbmwifi_vif *priv,
				    struct atbmwifi_ieee80211_supported_band *sband,
				    struct atbmwifi_sta_priv *sta_priv, atbm_uint32 changed,
				    enum atbm_nl80211_channel_type oper_chan_type)
{
	struct atbm_ieee80211_sta atbm_sta; 
	void *priv_sta = sta_priv->sta_rc_priv;
    enum atbm_nl80211_channel_type atbm_oper_chan_type = (enum atbm_nl80211_channel_type)oper_chan_type;
    struct atbmwifi_band_info band_info;  
   // struct ieee80211_channel_state *chan_state = ieee80211_get_channel_state(local, sta->sdata);


    /*Porting part: copy to support rates and capability to atbm_sta*/
	//atbm_sta = ( struct atbm_ieee80211_sta *)ista;
    memcpy((atbm_uint8*)(&atbm_sta.supp_rates[sband->band]), (atbm_uint8*)(&sta_priv->rate.support_rates),  sizeof(atbm_uint32));
    
   // memcpy((u8*)(atbm_sta.ht_cap), (u8*)(ista->ht_cap),  sizeof(struct ieee80211_sta_ht_cap));
    //memcpy((u8*)(atbm_sta.he_cap), (u8*)(ista->he_cap),  sizeof(struct atbm_ieee80211_sta_he_cap ));
   // memcpy((u8*)(atbm_sta.vht_cap), (u8*)(ista->vht_cap),  sizeof(struct ieee80211_sta_vht_cap));
   
    atbm_sta.ht_cap =  (struct atbmwifi_ieee80211_sta_ht_cap*)&sta_priv->rate.ht_cap;
    atbm_sta.vht_cap = (struct atbm_ieee80211_sta_vht_cap*)&sta_priv->rate.vht_cap;
    atbm_sta.he_cap =  (struct atbm_ieee80211_sta_he_cap*)&sta_priv->rate.he_cap;
   
    //atbm_sta.chip_category = ista->chip_category;
    
    band_info.center_freq = atbmwifi_ieee80211_channel_to_frequency(sband, priv->bss.channel_num);    
    band_info.channel_type = oper_chan_type; //for tx used.

#if BW_40M_SUPPORT
   if((band_info.channel_type == ATBM_NL80211_CHAN_HT40PLUS)||(band_info.channel_type == ATBM_NL80211_CHAN_HT40MINUS))  
    {
         band_info.channel_type = ATBM_NL80211_CHAN_HT20;
    }
#endif
   /*end of porting part*/
    //atbm_printk_rc(" rate_control_rate_update 0x%x chip_category:%d\n", atbm_sta.vht_cap, ista->chip_category);
	if (global_ops && global_ops->rate_update)
		global_ops->rate_update(global_ops->rc_priv, &band_info, &atbm_sta,
				      priv_sta, changed, atbm_oper_chan_type);
}
#endif

static inline void *rate_control_alloc_sta(struct atbmwifi_sta_priv *sta_priv,
					   gfp_t gfp)
{
    struct atbm_ieee80211_sta atbm_sta;  

    void *p;
	/*
	struct atbm_sta_priv *sta_priv =
			(struct atbm_sta_priv *)&sta->drv_priv; //ap interface
	struct atbm_vif *priv_vif = (struct atbm_vif *)sta_priv->priv;    
    struct atbm_common *hw_priv = (struct atbm_common *)priv_vif->hw_priv;
*/
	
	atbm_printk_rc(" rate_control_alloc_sta chip_category\n");
   // atbm_sta = (struct atbm_ieee80211_sta *)sta;  
	//atbm_sta->chip_category = hw_priv->chip_cat;
	atbm_spin_lock(&sta_priv->lock);
	ATBM_BUG_ON(!(global_ops && global_ops->rc_priv));
    //atbm_printk_rc(" rate_control_alloc_sta atbm_sta:0x%x\n", atbm_sta);
	p = global_ops->alloc_sta(global_ops->rc_priv, &atbm_sta, gfp);
    /*pass txs_retrys and n_rates to sta*/
	sta_priv->rate.txs_retrys = atbm_sta.txs_retrys;
	sta_priv->rate.n_rates =  atbm_sta.n_rates;

	atbm_spin_unlock(&sta_priv->lock);
	//atbm_printk_rc(" rate_control_alloc_sta atbm_sta:0x%x txs_retrys:0x%x 0x%x\n", atbm_sta, atbm_sta.txs_retrys, sta->txs_retrys);
	return p;
}

static inline void rate_control_free_sta(struct atbmwifi_sta_priv *sta_priv)
{
	void *priv_sta = sta_priv->sta_rc_priv;

	atbm_spin_lock(&sta_priv->lock);
	ATBM_BUG_ON(!(global_ops && global_ops->rc_priv));
	global_ops->free_sta(global_ops->rc_priv, priv_sta);
	sta_priv->sta_rc_priv = ATBM_NULL;
	atbm_spin_unlock(&sta_priv->lock);
}

void rate_control_get_rate(struct atbmwifi_sta_priv *sta_priv,
			   struct ieee80211_tx_rate_control *txrc);

int atbm_ieee80211_rate_control_register(struct atbm_rate_control_ops *ops);
void atbm_ieee80211_rate_control_unregister(struct atbm_rate_control_ops *ops);

int hmac_rc_init(struct atbmwifi_common *hw_priv);
void hmac_rc_deinit(void);


int  atbm_rc80211_hmac_init(void);
void atbm_rc80211_hmac_exit(void);

#endif /*__HMAC_RATE_CTROL_*/
