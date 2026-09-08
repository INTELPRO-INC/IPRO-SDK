/**************************************************************************************************************
 * altobeam RTOS wifi hmac source code 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 *
 *  The source code contains proprietary information of AltoBeam, and shall not be distributed, 
 *  copied, reproduced, or disclosed in whole or in part without prior written permission of AltoBeam.
*****************************************************************************************************************/

#include "atbm_hal.h"
#include "atbm_ratectrl.h"

int rate_for_gi = 0xff;
int wifi_special_freq = 2412;

struct atbm_rate_control_ops *global_ops = ATBM_NULL;

static inline int rate_supported(struct atbmwifi_sta_priv *sta_priv,
				 struct atbmwifi_ieee80211_supported_band *band,
				 int index)
{
	return (sta_priv == NULL || (sta_priv->rate.support_rates & ATBM_BIT(index)));
}

static inline s8
rate_lowest_index(struct atbmwifi_ieee80211_supported_band *sband,
		  struct atbmwifi_sta_priv *sta_priv)
{
	int i;

	//atbm_printk_rc( "x atbm_rate_control_send_low sband->n_bitrates:%d supp_rates:0x%x hw_value:%d\n", sband->n_bitrates, sta_priv->supp_rates[sband->band],sband->bitrates[0].hw_value);

	for (i = 0; i < sband->n_bitrates; i++)
		if (rate_supported(sta_priv, sband, i))
			return sband->bitrates[i].hw_value;

	/* warn when we cannot find a rate. */
	ATBM_WARN_ON(1);

	/* and return 0 (the lowest index) */
	return 0;
}

static inline s8
rate_lowest_non_cck_index(struct atbmwifi_ieee80211_supported_band *sband,
			  struct atbmwifi_sta_priv *sta)
{
	int i;

	for (i = 0; i < sband->n_bitrates; i++) {
		struct atbmwifi_ieee80211_rate *srate = &sband->bitrates[i];
		if ((srate->bitrate == 10) || (srate->bitrate == 20) ||
		    (srate->bitrate == 55) || (srate->bitrate == 110))
			continue;

		if (rate_supported(sta, sband, i))
			return  sband->bitrates[i].hw_value;
	}

	/* No matching rate found */
	return 0;
}


ATBM_BOOL atbm_rate_control_send_low(struct atbmwifi_sta_priv *sta_priv,
			   void *priv_sta,
			   struct ieee80211_tx_rate_control *txrc)
{

    struct atbmwifi_ieee80211_tx_info *info = txrc->info;
    
	struct atbmwifi_ieee80211_supported_band *sband = txrc->sband;
   // struct ieee80211_sta *sta = (struct ieee80211_sta *)atbm_sta;
    s8 idx;
 //  atbm_printk_rc( "x atbm_rate_control_send_low \n");
	if (txrc->txrc_in.lower) 
    {
      atbm_printk_rc( "x atbm_rate_control_send_low sband->band:%d\n", sband->band);
		if ((sband->band != ATBM_IEEE80211_BAND_2GHZ) ||
		    !(info->flags & ATBM_IEEE80211_TX_CTL_NO_CCK_RATE))
			idx = rate_lowest_index(sband, sta_priv);
		else
			idx =
				rate_lowest_non_cck_index(sband, sta_priv);
        //set rate   
        txrc->info->control.txrc_out.tx_max_rate = idx;        
		return ATBM_TRUE;       
	}
    
	return ATBM_FALSE;   //not use lowest rate.

}

void rate_control_get_rate(struct atbmwifi_sta_priv *sta_priv,
			   struct ieee80211_tx_rate_control *txrc)
{
	void *priv_sta = NULL;
    struct atbm_ieee80211_tx_rate_control atbm_txrc;
	//struct atbm_ieee80211_sta atbm_sta;  

	if (sta_priv) {
		atbm_spin_lock(&sta_priv->lock);
		priv_sta = sta_priv->sta_rc_priv;
		if(!priv_sta){
			atbm_spin_unlock(&sta_priv->lock);
			return;
		}
	}else
	{
	     wifi_printk(WIFI_DBG_MSG, "x get_rate err\n");
	}
	//atbm_sta = (struct atbm_ieee80211_sta *)sta;  

    /*copy from ieee80211_tx_rate_control to atbm_ieee80211_tx_rate_control  */
  //  atbm_txrc.manual       = txrc->manual;
  //  atbm_txrc.manual_rate  = txrc->manual_rate;
  //  atbm_txrc.max_rate_idx = txrc->max_rate_idx;
  //  atbm_txrc.lower        = txrc->lower;
 //    atbm_printk_rc( "x get_rate pre1\n");
	 	
     atbm_txrc.tx_rc_in = (struct atbm_tx_rc_input *)&txrc->txrc_in;
     atbm_txrc.info.control = (struct atbm_tx_rc_output *) &txrc->info->control.txrc_out;
	  //  atbm_printk_rc( "x get_rate pre2\n");
    //atbm_txrc.priv         = txrc->sband;
    //check low rate.
    if(atbm_rate_control_send_low(sta_priv,  priv_sta, txrc))
    {
         txrc->txrc_in.max_rate_idx = txrc->info->control.txrc_out.tx_max_rate;
         atbm_txrc.tx_rc_in->lower = 1;
         atbm_txrc.tx_rc_in->max_rate_idx = txrc->txrc_in.max_rate_idx;
         wifi_printk(WIFI_DBG_MSG,  "x get_rate low max_rate_idx:%d\n", atbm_txrc.tx_rc_in->max_rate_idx );
    }
    else
    {
           atbm_txrc.tx_rc_in->lower = 0;
    }

   // atbm_sta->he_cap = &ista->he_cap;
  //  atbm_sta->ht_cap = &ista->ht_cap;
  //  atbm_sta->vht_cap = &ista->vht_cap;
   // atbm_printk_rc( "x get_rate\n");
	global_ops->get_rate(global_ops->rc_priv, priv_sta, &atbm_txrc);     

   /*copy to ieee80211_tx_rate_control*/    
    /*
    txrc->info->control.tx_rate_sets     =  atbm_txrc.info->control.tx_rate_sets;
    txrc->info->control.tx_max_rate      =  atbm_txrc.info->control.tx_max_rate;
    txrc->info->control.tx_rc_flag       =  atbm_txrc.info->control.tx_rc_flag;
    txrc->info->control.force_policyid   =  atbm_txrc.info->control.force_policyid;      
    */
    txrc->info->control.tx_update_rate           =  atbm_txrc.info.tx_update_rate;
    txrc->info->control.sample_pkt_flag          =  atbm_txrc.info.sample_pkt_flag; 
	if(sta_priv){
		atbm_spin_unlock(&sta_priv->lock);
	}
}


int atbm_ieee80211_rate_control_register(struct atbm_rate_control_ops *ops)
{

	if (!ops->name)
		return -ATBM_EINVAL;

	global_ops = ops;

	atbm_printk_rc("iee80211_rate_control_register %s \n", ops->name);

	return 0;
}
//EXPORT_SYMBOL(ieee80211_rate_control_register);

void atbm_ieee80211_rate_control_unregister(struct atbm_rate_control_ops *ops)
{
	global_ops = ATBM_NULL;
}

int hmac_rc_init(struct atbmwifi_common *hw_priv){
	atbm_rc80211_hmac_init();
	if(!global_ops){
		wifi_printk(WIFI_DBG_ERROR, "hmac_rc_init err \n");
		return -1;
	}
	global_ops->rc_priv = global_ops->alloc(hw_priv->chip_priv->chip_cat);
	if(!global_ops->rc_priv)
		return -1;
	return 0;
}

void hmac_rc_deinit(){
	if(!global_ops){
		wifi_printk(WIFI_DBG_ERROR, "hmac_rc_init err \n");
		return;
	}
	global_ops->free(global_ops->rc_priv);
	atbm_rc80211_hmac_exit();
}


