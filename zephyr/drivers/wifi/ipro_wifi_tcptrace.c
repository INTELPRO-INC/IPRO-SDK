/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * TCP timing trace at the driver boundary (CONFIG_WIFI_IPRO_TCP_TRACE): every
 * IPv4/TCP frame received from or sent to the MAC goes into a ring as
 * (time, direction, seq, ack, payload length, flags). A received data segment
 * whose sequence number is below the highest one seen so far (a
 * retransmission or a reordered frame) of the tracked flow (the first one to
 * carry data after arming, or after a SYN) triggers the trace; armed with
 * `tx`, a data segment we send below the highest one sent so far (our own
 * retransmission) does instead. Recording stops
 * once the ring holds WIFI_IPRO_TCP_TRACE_POST entries after that segment,
 * so `wifi_ipro tcptrace` shows what led up to it and what followed.
 */
#include <zephyr/kernel.h>
#include <zephyr/net/net_pkt.h>
#include <zephyr/shell/shell.h>
#include <zephyr/sys/byteorder.h>
#include "ipro_wifi_tcptrace.h"

#define RING CONFIG_WIFI_IPRO_TCP_TRACE_ENTRIES
#define POST CONFIG_WIFI_IPRO_TCP_TRACE_POST
#define HDR_MAX (14 + 60 + 20) /* Ethernet + IPv4 with options + TCP */

struct tt_entry {
	uint32_t t_us;
	uint32_t seq;
	uint32_t ack;
	uint16_t len;
	uint16_t win; /* advertised window, unscaled */
	uint8_t flags;
	uint8_t tx;
};

static struct tt_entry ring[RING];
static uint32_t head;        /* entries written since arming */
static uint32_t trig_at;     /* index of the triggering entry, valid if triggered */
static bool triggered;
static bool armed;
static bool watch_tx;        /* trigger on our retransmissions instead of received ones */
static uint32_t rx_high;     /* highest end of received data (seq + len) */
static bool rx_high_valid;
static uint32_t flow_saddr;  /* the tracked flow: sender address and both ports */
static uint32_t flow_ports;
static struct k_spinlock lock;

/* The MAC's reorder-check ring (rxu_cntrl.c), armed and frozen with this one. */
struct rxu_reord_trace_ent {
	uint32_t t_us;
	uint16_t sn;
	int16_t off;
	uint8_t tid;
	uint8_t what;
};
extern struct rxu_reord_trace_ent macsw_rxu_reord_trace[CONFIG_WIFI_IPRO_TCP_TRACE_MAC_ENTRIES];
extern volatile uint32_t macsw_rxu_reord_trace_head;
extern volatile bool macsw_rxu_reord_trace_on;

uint32_t ipro_tcptrace_now_us(void)
{
	return k_cyc_to_us_floor32(k_cycle_get_32());
}

static void record(bool tx, const uint8_t *f, size_t n)
{
	if (!armed || n < 14 + 20 + 20 || sys_get_be16(f + 12) != 0x0800 || f[14 + 9] != 6) {
		return;
	}
	const uint8_t *ip = f + 14;
	size_t ihl = (ip[0] & 0x0f) * 4;
	size_t ip_len = sys_get_be16(ip + 2);

	if (14 + ihl + 20 > n) {
		return;
	}
	const uint8_t *th = ip + ihl;
	size_t doff = (th[12] >> 4) * 4;
	uint32_t saddr = sys_get_be32(ip + 12);
	uint32_t ports = sys_get_be32(th);
	struct tt_entry e = {
		.t_us = ipro_tcptrace_now_us(),
		.seq = sys_get_be32(th + 4),
		.ack = sys_get_be32(th + 8),
		.len = (ip_len > ihl + doff) ? (uint16_t)(ip_len - ihl - doff) : 0,
		.win = sys_get_be16(th + 14),
		.flags = th[13],
		.tx = tx,
	};
	k_spinlock_key_t key = k_spin_lock(&lock);

	if (armed) {
		ring[head % RING] = e;
		head++;
		if ((e.flags & 0x02) != 0) {
			rx_high_valid = false; /* SYN: a new connection, track what carries data next */
		}
		if (tx == watch_tx && e.len > 0 && !rx_high_valid) {
			flow_saddr = saddr;
			flow_ports = ports;
		}
		if (tx == watch_tx && e.len > 0 && saddr == flow_saddr && ports == flow_ports) {
			uint32_t end = e.seq + e.len;

			if (rx_high_valid && (int32_t)(e.seq - rx_high) < 0 && !triggered) {
				triggered = true;
				trig_at = head - 1;
			}
			if (!rx_high_valid || (int32_t)(end - rx_high) > 0) {
				rx_high = end;
				rx_high_valid = true;
			}
		}
		if (triggered && head - trig_at > POST) {
			armed = false;
			macsw_rxu_reord_trace_on = false;
		}
	}
	k_spin_unlock(&lock, key);
}

void ipro_tcptrace_rx(const uint8_t *frame, size_t len)
{
	record(false, frame, len);
}

void ipro_tcptrace_tx(struct net_pkt *pkt)
{
	uint8_t hdr[HDR_MAX];
	struct net_pkt_cursor backup;
	size_t n = MIN(net_pkt_get_len(pkt), sizeof(hdr));

	if (!armed) {
		return;
	}
	net_pkt_cursor_backup(pkt, &backup);
	net_pkt_cursor_init(pkt);
	if (net_pkt_read(pkt, hdr, n) == 0) {
		record(true, hdr, n);
	}
	net_pkt_cursor_restore(pkt, &backup);
}

int ipro_tcptrace_cmd(const struct shell *sh, size_t argc, char **argv)
{
	if (argc > 1 && argv[1][0] == 'a') {
		k_spinlock_key_t key = k_spin_lock(&lock);

		watch_tx = argc > 2 && argv[2][0] == 't';
		head = 0;
		triggered = false;
		rx_high_valid = false;
		macsw_rxu_reord_trace_head = 0;
		macsw_rxu_reord_trace_on = true;
		armed = true;
		k_spin_unlock(&lock, key);
		shell_print(sh, "tcptrace armed on %s (%d entries, %d after the trigger)",
			    watch_tx ? "our retransmissions" : "late received data", RING, POST);
		return 0;
	}
	k_spinlock_key_t key = k_spin_lock(&lock);
	bool was_armed = armed;

	armed = false;
	macsw_rxu_reord_trace_on = false;
	k_spin_unlock(&lock, key);
	uint32_t n = MIN(head, RING);

	shell_print(sh, "tcptrace %s, %u entries, trigger %s", was_armed ? "stopped" : "done", n,
		    triggered ? "hit" : "not hit");
	for (uint32_t i = head - n; i != head; i++) {
		const struct tt_entry *e = &ring[i % RING];

		shell_print(sh, "tt %u %c %u %u %u %02x w%u%s", e->t_us, e->tx ? 'T' : 'R', e->seq,
			    e->ack, e->len, e->flags, e->win, triggered && i == trig_at ? " <" : "");
	}
	uint32_t mh = macsw_rxu_reord_trace_head;
	uint32_t mn = MIN(mh, CONFIG_WIFI_IPRO_TCP_TRACE_MAC_ENTRIES);

	for (uint32_t i = mh - mn; i != mh; i++) {
		const struct rxu_reord_trace_ent *m =
			&macsw_rxu_reord_trace[i % CONFIG_WIFI_IPRO_TCP_TRACE_MAC_ENTRIES];

		shell_print(sh, "mt %u %u %d %u %u", m->t_us, m->sn, m->off, m->tid, m->what);
	}
	return 0;
}
