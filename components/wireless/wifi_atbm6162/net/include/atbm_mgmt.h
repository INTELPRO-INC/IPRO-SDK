/**************************************************************************************************************
 * altobeam RTOS wifi hmac source code 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 *
 *  The source code contains proprietary information of AltoBeam, and shall not be distributed, 
 *  copied, reproduced, or disclosed in whole or in part without prior written permission of AltoBeam.
*****************************************************************************************************************/

#ifndef __ATBM_MGMT_H__
#define  __ATBM_MGMT_H__

int atbmwifi_ieee80211_drop_unencrypted_mgmt(struct atbmwifi_vif *priv,struct atbm_buff *skb);

atbm_void atbmwifi_rx_actionFrame(struct atbmwifi_vif *priv,struct atbm_buff *skb);

 atbm_void atbmwifi_rx_beacon(struct atbmwifi_vif *priv,struct atbm_buff *skb);
 atbm_void atbmwifi_rx_probe_resp(struct atbmwifi_vif *priv,struct atbm_buff *skb);

 int atbmwifi_rx_authen(struct atbmwifi_vif *priv,struct atbm_buff *skb);

 int atbmwifi_rx_assoc_rsp(struct atbmwifi_vif *priv,struct atbm_buff *skb);


 atbm_void atbmwifi_rx_sta_mgmtframe(struct atbmwifi_vif *priv,struct atbm_buff *skb);

 int atbmwifi_ieee80211_build_preq_ies(struct atbmwifi_vif *priv,atbm_uint8 *buffer,
			     const atbm_uint8 *ie, atbm_size_t ie_len,atbm_uint8 channel);

 struct atbm_buff *atbmwifi_ieee80211_build_probe_req(struct atbmwifi_vif *priv, 
					  atbm_uint8 *dst, const atbm_uint8 *ie, atbm_size_t ie_len);

 struct atbm_buff * atbmwifi_ieee80211_send_probe_req(struct atbmwifi_vif *priv, atbm_uint8 *dst,
			      const atbm_uint8 *ie, atbm_size_t ie_len, ATBM_BOOL no_cck);


 struct atbm_buff * atbmwifi_ieee80211_send_assoc_req(struct atbmwifi_vif *priv);


 atbm_void atbmwifi_tx_sta_mgmtframe(struct atbmwifi_vif *priv,atbm_uint16 stype,atbm_uint16 transaction );

 int atbmwifi_rx_assoc_req(struct atbmwifi_vif *priv, struct atbm_buff *skb, struct atbmwifi_ieee80211_tx_info * tx_info);

 atbm_void atbmwifi_rx_ap_mgmtframe(struct atbmwifi_vif *priv,struct atbm_buff *skb);


 atbm_void atbmwifi_tx_ap_mgmtframe(struct atbmwifi_vif *priv,atbm_uint16 stype,atbm_uint16 transaction );

 struct atbm_buff * atbmwifi_ieee80211_send_assoc_resp(struct atbmwifi_vif *priv,atbm_uint16 status_code, int reassoc, const atbm_uint8 *ies, atbm_size_t ies_len,atbm_uint16 aid);

 struct atbm_buff *atbmwifi_ieee80211_send_proberesp(struct atbmwifi_vif *priv,const atbm_uint8 *ies,atbm_size_t ies_len);

 struct atbm_buff *atbmwifi_ieee80211_send_beacon(struct atbmwifi_vif  *priv,const atbm_uint8 *ies, atbm_size_t ies_len);

 struct atbm_buff * atbmwifi_ieee80211_send_auth(struct atbmwifi_vif *priv, atbm_uint16 transaction, atbm_uint16 auth_alg,  const atbm_uint8 *da,const atbm_uint8 *bssid,atbm_uint16 resp);

 struct atbm_buff * atbmwifi_ieee80211_send_deauth(struct atbmwifi_vif *priv,const atbm_uint8 *da,const atbm_uint8 *bssid,atbm_uint16 reason);

 struct atbm_buff *atbmwifi_ieee80211_send_saquery(struct atbmwifi_vif *priv,const atbm_uint8 *da,const atbm_uint8 *bssid, int dir, const atbm_uint8 *trans_id);
 int atbmwifi_ieee80211_tx_mgmt_deauth(struct atbmwifi_vif *priv,const atbm_uint8 *da,const atbm_uint8 *bssid,atbm_uint16 reason);
 int atbmwifi_tx_data_Null_response(struct atbmwifi_vif *priv,const atbm_uint8 *da,const atbm_uint8 *bssid,int tid,enum atbm_ieee80211_frame_release_type reason);
 int atbmwifi_ieee80211_tx_data_QosNullData(struct atbmwifi_vif *priv,const atbm_uint8 *da,const atbm_uint8 *bssid,atbm_uint8 tid);
 	int atbmwifi_set_tim(struct atbmwifi_vif  *priv, atbm_uint16 aid, ATBM_BOOL set);
 atbm_uint8 *atbmwifi_add_tim(atbm_uint8 *frm, struct atbmwifi_vif  *priv,atbm_uint8 mcast);
 struct atbm_buff * atbmwifi_ieee80211_send_NullData(struct atbmwifi_vif *priv,const atbm_uint8 *da,const atbm_uint8 *bssid);
 atbm_void atbmwifi_ieee80211_ht_cap_ie_to_sta_ht_cap(struct atbmwifi_ieee80211_supported_band *sband,
				       struct atbmwifi_ieee80211_ht_cap *ht_cap_ie,
				       struct atbmwifi_ieee80211_sta_ht_cap *ht_cap);
 atbm_void atbmwifi_ieee80211_assoc_success(struct atbmwifi_vif *priv,struct atbm_buff *skb);
atbm_void atbm_ieee802_11_parse_elems(atbm_uint8 *start, int len,
			    struct atbmwifi_ieee802_11_elems *elems);
const atbm_uint8 *atbmwifi_find_ext_ie(atbm_uint8 ext_eid, const atbm_uint8 *ies, int len);
const struct element *atbmwifi_find_ext_elem(atbm_uint8 ext_eid, const atbm_uint8 *ies, int len);
ATBM_BOOL atbmwifi_is_element_inherited(const struct element *elem,
				   const struct element *non_inherit_elem);

atbm_void atbmwifi_ieee80211_bss_info_change_notify(struct atbmwifi_vif *priv,
				      atbm_uint32 changed);
int atbmwifi_ieee80211_enable_ht(struct atbmwifi_ieee80211_supported_band *sband,
			       struct atbmwifi_ieee80211_ht_info *hti,
			       struct atbmwifi_vif *priv, atbm_uint16 ap_ht_cap_flags,
			       int beacon_htcap_ie);
int atbm_ieee80211_handle_bss_capability(struct atbmwifi_vif *priv,
					   atbm_uint16 capab, int erp_valid, atbm_uint8 erp);
atbm_void atbmwifi_ieee80211_send_htc_qosnullfunc(struct atbmwifi_vif *priv, atbm_uint32 htc);
void ieee80211_send_delba(struct atbmwifi_vif *priv,
		const atbm_uint8 *da, atbm_uint16 tid,
		atbm_uint16 initiator, atbm_uint16 reason_code);
 void ieee80211_send_addba_resp(struct atbmwifi_sta_priv *sta_priv, atbm_uint8 *da, atbm_uint16 tid,
						atbm_uint8 dialog_token, atbm_uint16 status, atbm_uint16 policy,
						atbm_uint16 buf_size, atbm_uint16 timeout,
						const struct atbm_ieee80211_addba_ext_ie *addbaext);
 void atbmwifi_ieee80211_send_addba_request(struct atbmwifi_vif *priv,
			   const atbm_uint8 *da, atbm_uint16 tid,
			   atbm_uint8 dialog_token, atbm_uint16 start_seq_num,
			   atbm_uint16 agg_size, atbm_uint16 timeout);

#if NEW_SUPPORT_PS
 struct atbm_buff * atbmwifi_ieee80211_NullData(struct atbmwifi_vif *priv,const atbm_uint8 *da,const atbm_uint8 *bssid);
 struct atbm_buff * atbmwifi_ieee80211_QosNullData(struct atbmwifi_vif *priv,const atbm_uint8 *da,
	const atbm_uint8 *bssid,atbm_uint8 tid);

#endif
#if FAST_CONNECT_NO_SCAN
 int atbm_wifi_reserve_key_ie(atbm_uint8 *buf, int buf_len, atbm_uint8 *ie, int ie_len);
#endif
#endif  //__ATBM_MGMT_H__
