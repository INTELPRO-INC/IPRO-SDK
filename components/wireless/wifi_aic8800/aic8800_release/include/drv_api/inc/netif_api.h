/*
 * Copyright (C) 2018-2024 AICSemi Ltd.
 *
 * All Rights Reserved
 */

#ifndef _NETIF_API_H_
#define _NETIF_API_H_

#include "aic_porting_types.h"
#include "netif_def.h"

typedef void (*aic_tx_cb)(uint32_t frame_id, bool acknowledged, void *arg);

uint32_t aic_offsetof(void);

uint8_t* aic_get_mac_address(void);
void aic_set_mac_address(uint8_t *addr);
void *aic_get_vif_mac_addr(void);
net_if_t aic_get_vif_net_if(void);
int aic_tx_start(net_if_t net_if, net_buf_tx_t *net_buf, aic_tx_cb cfm_cb, void *cfm_cb_arg);
#ifdef CONFIG_FILTER_TCP_ACK
int aic_filter_send_tcp_ack(uint8_t *buf, uint32_t plen, void *msg);
#endif

/* Opaque access to fhost VIF for platform init */
void *aic_get_fhost_default_vif(void);
void  aic_fhost_vif_set_mac(void *vif, const uint8_t *mac);

/* Network interface lookup (from netif_co) */
net_if_t net_if_find_from_wifi_idx(unsigned int idx);

#endif /* _NETIF_API_H_ */

