/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * Zephyr-network-stack side of the fhost adapter: everything that needs
 * <zephyr/net/...> headers. Kept apart from net_al_zephyr.c because the SDK's
 * net_al.h declares net_if_get_name() / net_l2_send() with the same names
 * as Zephyr's own API.
 */
#include <zephyr/kernel.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_pkt.h>
#include <zephyr/net/ethernet.h>
#include <zephyr/net/dhcpv4.h>
#include <zephyr/net/net_mgmt.h>
#include <zephyr/net/net_event.h>
#include <zephyr/logging/log.h>
#include "net_al_zephyr.h"
#ifdef CONFIG_WIFI_IPRO_TCP_TRACE
#include "../../../drivers/wifi/ipro_wifi_tcptrace.h"
#endif

LOG_MODULE_REGISTER(ipro_wifi_zif, CONFIG_LOG_DEFAULT_LEVEL);

void ipro_zif_carrier(struct net_if *iface, bool up)
{
	if (iface == NULL) {
		return;
	}
	if (up) {
		net_if_dormant_off(iface);
	} else {
		net_if_dormant_on(iface);
	}
}

uint32_t ipro_zif_rx_ok, ipro_zif_rx_nopkt, ipro_zif_rx_err;

int ipro_zif_rx(struct net_if *iface, const uint8_t *frame, uint16_t len)
{
	/* A received A-MPDU lands here as a burst of up to 32 frames from the
	 * fhost RX thread; the RX pool is smaller than that, so wait briefly for
	 * the stack to drain instead of dropping (the MAC's own RX buffers hold
	 * the burst meanwhile). */
#ifdef CONFIG_WIFI_IPRO_TCP_TRACE
	ipro_tcptrace_rx(frame, len);
#endif
	struct net_pkt *pkt = net_pkt_rx_alloc_with_buffer(iface, len, AF_UNSPEC, 0,
							     K_MSEC(CONFIG_IPRO_WIFI_RX_PKT_WAIT_MS));

	if (pkt == NULL) {
		ipro_zif_rx_nopkt++;
		return -ENOMEM;
	}
	if (net_pkt_write(pkt, frame, len) != 0 || net_recv_data(iface, pkt) < 0) {
		net_pkt_unref(pkt);
		ipro_zif_rx_err++;
		return -EIO;
	}
	ipro_zif_rx_ok++;
	return 0;
}

int ipro_zif_pkt_len(struct net_pkt *pkt)
{
	return (int)net_pkt_get_len(pkt);
}

/* The frame's data if it sits in one fragment (the MTU fits one 1536 B
 * net_buf), else NULL. */
uint8_t *ipro_zif_pkt_single_frag(struct net_pkt *pkt, size_t *len)
{
	struct net_buf *frag = pkt->frags;
	extern uint32_t ipro_zc_miss_nofrag, ipro_zc_miss_multi, ipro_zc_frag0_len, ipro_zc_frag1_len;

	if (frag == NULL) {
		ipro_zc_miss_nofrag++;
		return NULL;
	}
	if (frag->frags != NULL) {
		ipro_zc_miss_multi++;
		ipro_zc_frag0_len = frag->len;
		ipro_zc_frag1_len = frag->frags->len;
		return NULL;
	}
	*len = frag->len;
	return frag->data;
}

void ipro_zif_pkt_ref(struct net_pkt *pkt)
{
	net_pkt_ref(pkt);
}

void ipro_zif_pkt_unref(struct net_pkt *pkt)
{
	net_pkt_unref(pkt);
}

int ipro_zif_pkt_read(struct net_pkt *pkt, uint8_t *dst, size_t len)
{
	net_pkt_cursor_init(pkt);
	return net_pkt_read(pkt, dst, len);
}

void ipro_zif_ip_clear(struct ipro_net_if *n)
{
	if (n->iface != NULL && n->ip != 0) {
		struct in_addr a = {.s_addr = n->ip};

		net_if_ipv4_addr_rm(n->iface, &a);
	}
	n->ip = n->mask = n->gw = n->dns = 0;
}

void ipro_zif_ip_static(struct ipro_net_if *n, uint32_t ip, uint32_t mask, uint32_t gw)
{
	struct in_addr a = {.s_addr = ip}, m = {.s_addr = mask}, g = {.s_addr = gw};

	net_dhcpv4_stop(n->iface);
	ipro_zif_ip_clear(n);
	net_if_ipv4_addr_add(n->iface, &a, NET_ADDR_MANUAL, 0);
	net_if_ipv4_set_netmask_by_addr(n->iface, &a, &m);
	net_if_ipv4_set_gw(n->iface, &g);
	n->ip = ip; n->mask = mask; n->gw = gw;
}

void ipro_zif_dhcp(struct ipro_net_if *n, bool start)
{
	if (n->iface == NULL) {
		return;
	}
	if (start) {
		net_dhcpv4_start(n->iface);
	} else {
		net_dhcpv4_stop(n->iface);
		ipro_zif_ip_clear(n);
	}
}

static struct net_mgmt_event_callback ipv4_cb;
static void (*addr_added)(struct ipro_net_if *n);

static void ipv4_event(struct net_mgmt_event_callback *cb, uint64_t mgmt_event,
		       struct net_if *iface)
{
	struct ipro_net_if *n = ipro_net_if_get(0);

	ARG_UNUSED(cb);
	if (n == NULL || iface != n->iface || mgmt_event != NET_EVENT_IPV4_ADDR_ADD) {
		return;
	}
	struct net_if_ipv4 *ipv4 = iface->config.ip.ipv4;

	if (ipv4 != NULL) {
		n->ip = ipv4->unicast[0].ipv4.address.in_addr.s_addr;
		n->mask = ipv4->unicast[0].netmask.s_addr;
		n->gw = ipv4->gw.s_addr;
	}
	if (addr_added != NULL) {
		addr_added(n);
	}
}

void ipro_zif_watch_ipv4(void (*cb)(struct ipro_net_if *n))
{
	static bool registered;

	addr_added = cb;
	if (!registered) {
		net_mgmt_init_event_callback(&ipv4_cb, ipv4_event, NET_EVENT_IPV4_ADDR_ADD);
		net_mgmt_add_event_callback(&ipv4_cb);
		registered = true;
	}
}
