/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * What the Zephyr Wi-Fi driver (drivers/wifi/ipro) and the fhost adapter
 * share: the per-VIF record and the driver's hooks.
 */
#ifndef IPRO_WIFI_NET_AL_ZEPHYR_H
#define IPRO_WIFI_NET_AL_ZEPHYR_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

struct net_if;
struct net_pkt;

struct ipro_net_if {
	struct net_if *iface;   /* set by the Zephyr driver when it binds */
	uint8_t mac[6];
	void *vif_priv;         /* fhost's VIF */
	uint8_t vif_idx;
	bool added;
	bool up;
	uint32_t ip, mask, gw, dns; /* network byte order, 0 = none */
};

/* Zephyr-stack helpers (net_al_zephyr_iface.c) */
void ipro_zif_carrier(struct net_if *iface, bool up);
int ipro_zif_rx(struct net_if *iface, const uint8_t *frame, uint16_t len);
int ipro_zif_pkt_len(struct net_pkt *pkt);
int ipro_zif_pkt_read(struct net_pkt *pkt, uint8_t *dst, size_t len);
uint8_t *ipro_zif_pkt_single_frag(struct net_pkt *pkt, size_t *len);
void ipro_zif_pkt_ref(struct net_pkt *pkt);
void ipro_zif_pkt_unref(struct net_pkt *pkt);
void ipro_zif_ip_clear(struct ipro_net_if *n);
void ipro_zif_ip_static(struct ipro_net_if *n, uint32_t ip, uint32_t mask, uint32_t gw);
void ipro_zif_dhcp(struct ipro_net_if *n, bool start);
void ipro_zif_watch_ipv4(void (*cb)(struct ipro_net_if *n));

/* VIF records in fhost order (0 = STA, 1 = AP). */
struct ipro_net_if *ipro_net_if_get(int vif_idx);
/* Bind the Zephyr interface of a VIF (the driver's iface init). */
void ipro_net_if_bind(int vif_idx, struct net_if *iface);
/* Send an Ethernet frame held in a net_pkt on a VIF (copied into WRAM). */
int ipro_net_if_send(int vif_idx, struct net_pkt *pkt);


/* Sender-thread CPU cycles per stage of ipro_net_if_send() (see wifi_ipro txsplit). */
struct ipro_tx_prof {
	uint32_t n;
	uint64_t wait, alloc, copy, fhost; /* thread execution cycles */
	uint64_t wait_wall;                /* k_cycle units spent blocked on the TX window */
};
extern struct ipro_tx_prof ipro_tx_prof;
long ipro_tx_inflight_get(void);
int ipro_tx_window_get(void);

#endif
