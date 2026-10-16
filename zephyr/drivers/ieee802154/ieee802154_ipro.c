/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * IPRO7 IEEE 802.15.4 driver: Zephyr ieee802154_radio_api -> SDK unified
 * radio driver (components/wireless/ieee802154/include/ieee802154.h) ->
 * lmac154 lower MAC -> M154 hardware.
 *
 * The SDK side (radio bring-up order, RF front-end calls, CCA mode) lives in
 * zephyr/hal_glue/ipro_ieee802154_glue.c behind the ipro154_* facade because
 * the SDK header and Zephyr's radio header share several names.
 *
 * Events arrive from the M154 ISR through the facade callback;
 * RX frames are copied into a small ring and delivered to the network stack
 * from a driver thread, TX completion is signalled to the waiting tx() call.
 */

#define DT_DRV_COMPAT intelpro_ipro_ieee802154

#include <stdlib.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/net/ieee802154_radio.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_pkt.h>
#include <zephyr/drivers/clock_control.h>
#include <zephyr/random/random.h>
#include <zephyr/logging/log.h>
#include <ipro_zephyr/ipro_hal_154.h>
#include <zephyr/net/ieee802154_pkt.h>
#include "ipro154_sec.h"

/* Timed TX: measured SFD-end offset from the requested trigger time, and the
 * lead time given to a CSL-IE frame so its phase can be computed exactly. */
#define IPRO154_TXTIME_SFD_OFFSET_US 19u

/* Radio time = the kernel clock (k_cycle_get_64, 4 MHz here).
 *
 * The Zephyr OpenThread platform assumes its microsecond alarm (kernel clock)
 * and otPlatRadioGetNow() tick at the same rate. The MAC symbol counter runs
 * ~100-150 ppm off the kernel clock on IPRO7 (bench 2026-09-23, `ipro154 clk`),
 * so CSL windows scheduled on one and placed on the other drifted apart by
 * ~60 us per 500 ms period - tens of milliseconds within minutes. Radio time
 * is therefore the kernel clock; MAC symbol-counter times (RX timestamps, TX
 * targets, TX end) are converted through a paired reading of both clocks. */
static uint64_t radio_now_us(void)
{
	/* k_cycle_get_64() is not available on this port (returns 0): extend the
	 * 32-bit cycle counter against the kernel uptime, which is monotonic and
	 * within one tick of it (the wrap is unambiguous within +-2^31 cycles). */
	unsigned int key = irq_lock();
	uint32_t c32 = k_cycle_get_32();
	uint64_t up = k_ticks_to_cyc_floor64(k_uptime_ticks());
	uint64_t c64;

	irq_unlock(key);
	c64 = (up & ~(uint64_t)UINT32_MAX) | c32;
	if ((int64_t)(c64 - up) > (int64_t)INT32_MAX) {
		c64 -= (uint64_t)1 << 32;
	} else if ((int64_t)(up - c64) > (int64_t)INT32_MAX) {
		c64 += (uint64_t)1 << 32;
	}
	return k_cyc_to_us_floor64(c64);
}

static void radio_clk_pair(uint64_t *k_us, uint64_t *sym_us)
{
	unsigned int key = irq_lock();

	*k_us = radio_now_us();
	*sym_us = ipro154_timestamp_us();
	irq_unlock(key);
}

static uint64_t radio_sym_to_k(uint64_t sym_us)
{
	uint64_t k, sym;

	radio_clk_pair(&k, &sym);
	/* the symbol clock is the 32-bit counter x 16 (wraps every 19.1 h):
	 * its differences are right in 32 bits */
	return (uint64_t)((int64_t)k - (int64_t)(int32_t)(uint32_t)(sym - sym_us));
}

static uint64_t radio_k_to_sym(uint64_t k_us)
{
	uint64_t k, sym;

	radio_clk_pair(&k, &sym);
	return (uint64_t)((int64_t)sym + ((int64_t)k_us - (int64_t)k));
}
#define IPRO154_CSL_TX_EST_US        600u   /* CSMA backoff + turnaround + SHR, typical */
#define IPRO154_SLOT_END_RX_WAITS    6u     /* 1 ms ticks: 127 B frame + Enh-ACK */

LOG_MODULE_REGISTER(ieee802154_ipro, CONFIG_IEEE802154_DRIVER_LOG_LEVEL);

#define IPRO154_CHANNEL_MIN 11
#define IPRO154_CHANNEL_MAX 26
#define IPRO154_TX_POWER_MIN (-20)
#define IPRO154_TX_POWER_MAX 20

struct radio_rx_slot {
	uint8_t psdu[IPRO154_MAX_PSDU];
	uint8_t len;
	int8_t rssi;
	uint8_t lqi;
	uint32_t timestamp;
	bool ack_fpb;
};

struct radio_data {
	struct net_if *iface;
	uint8_t mac[8];
	uint8_t channel;
	int8_t tx_power;
	bool promiscuous;
	bool started;
	ieee802154_event_cb_t event_handler;
	uint8_t ext_addr[8];        /* as set through the filter: over-the-air (LE) order */
	struct ipro154_sec sec;     /* TX security keys/counter + CSL IE state */

	/* Timed receive window (IEEE802154_CONFIG_RX_SLOT): the CSL receiver's
	 * sampling. start_work turns the receiver on, end_work turns it off
	 * again and reports IEEE802154_EVENT_RX_OFF so the stack sleeps. */
	struct k_work_delayable rx_slot_start_work;
	struct k_work_delayable rx_slot_end_work;
	uint8_t rx_slot_channel;
	uint32_t rx_slot_duration_us;
	uint64_t slot_next_us;     /* start of the next armed window, 0 = none */
	uint64_t slot_open_us;     /* start of the window opened last, 0 = none */
	uint32_t st_slot_auto;     /* windows the driver re-armed on its own */
	uint32_t st_slot_absorbed; /* stack requests for a window already open/passed */

	/* A stop requested while a frame waits for its ACK is applied when
	 * the transmit completes (OpenThread sleeps the radio at the end of a
	 * CSL sample window even if the poll it just sent is still waiting). */
	bool tx_busy;
	bool stop_pending;
	uint32_t st_stop_deferred;
	uint32_t st_slot_end_deferred;
	/* RX window diagnostics */
	bool slot_active;
	bool slot_end_pending; /* window end deferred by a transmit */
	uint8_t slot_end_rx_waits; /* ticks the end waited for a frame on the air */
	uint32_t st_slot_end_rx_ext;
	struct k_spinlock slot_lock; /* window state: OT thread vs system workqueue */
	uint32_t st_slot_dur_us;     /* last requested duration */
	int32_t st_slot_open_err_us; /* last: actual open time - requested start (negative = early) */
	int32_t st_slot_open_err_max, st_slot_open_err_min;
	uint32_t st_rx_in_slot, st_rx_out_slot;
	/* timed TX accuracy: SFD end - target */
	int32_t st_txtime_err_last, st_txtime_err_max, st_txtime_err_min;
	uint32_t st_slot_open_hist[6];     /* open - requested start: <0,<1ms,<2ms,<5ms,<50ms,>= */

	uint32_t st_txtime_err_hist[6];    /* sfd - target: <250,<500,<1000,<5000,<20000,>= us */

	/* `ipro154 stats` counters */
	uint32_t st_txtime;      /* timed TX armed */
	uint32_t st_txtime_late; /* TXTIME target already passed: sent immediately */
	uint32_t st_txtime_fail; /* MAC refused the timed trigger */
	uint32_t st_rx_slots;    /* RX windows opened */

	/* TX completion, with the ACK the HAL received for a unicast */
	struct k_sem tx_done;
	int tx_result;
	uint8_t ack_psdu[IPRO154_MAX_PSDU];
	uint8_t ack_len;
	int8_t ack_rssi;
	uint8_t ack_lqi;
	bool ack_fpb;
	uint32_t ack_ts;   /* ACK RX time, MAC symbol clock (us), 0 = unknown */

	/* Energy scan: sampled over the requested duration from a work item so
	 * the completion callback never runs inside ed_scan() itself. */
	struct k_work_delayable ed_work;
	energy_scan_done_cb_t ed_cb;
	uint16_t ed_duration;
	int16_t ed_max;
	uint32_t ed_started;

	/* RX ring filled from the ISR, drained by rx_thread */
	struct radio_rx_slot rx_ring[CONFIG_IEEE802154_IPRO_RX_QUEUE_DEPTH];
	uint32_t rx_dropped;          /* frames lost to a full ring (ISR-owned) */
	uint32_t rx_dropped_reported; /* last value logged by the RX thread */
	uint8_t rx_head;
	uint8_t rx_tail;
	struct k_sem rx_sem;
	struct k_thread rx_thread;
	K_KERNEL_STACK_MEMBER(rx_stack, CONFIG_IEEE802154_IPRO_RX_STACK_SIZE);
};

struct radio_config {
	const struct device *clock_dev;
	clock_control_subsys_t clock_id;
	int8_t tx_power;
	int8_t cca_threshold;
};

static struct radio_data radio_data_0;

/* ---------------------------------------------------------------------- */
/* ISR side                                                                */
/* ---------------------------------------------------------------------- */

static void radio_event(enum ipro154_event event, const struct ipro154_rx_frame *rx, void *ctx)
{
	struct radio_data *data = ctx;

	switch (event) {
	case IPRO154_EVT_TX_DONE:
		if (rx != NULL && rx->len >= 3) {
			data->ack_len = MIN(rx->len, IPRO154_MAX_PSDU);
			memcpy(data->ack_psdu, rx->psdu, data->ack_len);
			data->ack_rssi = rx->rssi;
			data->ack_lqi = rx->lqi;
			data->ack_fpb = rx->ack_fpb;
			data->ack_ts = rx->timestamp_us;
		}
		data->tx_result = 0;
		k_sem_give(&data->tx_done);
		break;
	case IPRO154_EVT_TX_NO_ACK:
		data->tx_result = -ENOMSG;
		k_sem_give(&data->tx_done);
		break;
	case IPRO154_EVT_TX_CHANNEL_BUSY:
		data->tx_result = -EBUSY;
		k_sem_give(&data->tx_done);
		break;
	case IPRO154_EVT_RX_DONE: {
		uint8_t next = (data->rx_head + 1) % CONFIG_IEEE802154_IPRO_RX_QUEUE_DEPTH;

		if (rx == NULL || rx->len == 0) {
			return;
		}
		if (next == data->rx_tail) {
			data->rx_dropped++; /* ring full; reported by the RX thread */
			return;
		}
		struct radio_rx_slot *s = &data->rx_ring[data->rx_head];

		s->len = MIN(rx->len, IPRO154_MAX_PSDU);
		memcpy(s->psdu, rx->psdu, s->len);
		s->rssi = rx->rssi;
		s->lqi = rx->lqi;
		s->timestamp = rx->timestamp_us;
		s->ack_fpb = rx->ack_fpb;
		data->rx_head = next;
		k_sem_give(&data->rx_sem);
		break;
	}
	default:
		break;
	}
}

/* ---------------------------------------------------------------------- */
/* RX thread                                                               */
/* ---------------------------------------------------------------------- */

static void radio_rx_thread(void *p1, void *p2, void *p3)
{
	const struct device *dev = p1;
	struct radio_data *data = dev->data;

	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	for (;;) {
		k_sem_take(&data->rx_sem, K_FOREVER);

		if (data->rx_dropped != data->rx_dropped_reported) {
			data->rx_dropped_reported = data->rx_dropped;
			LOG_WRN("rx ring full: %u frames dropped so far", data->rx_dropped_reported);
		}
		while (data->rx_tail != data->rx_head) {
			struct radio_rx_slot *s = &data->rx_ring[data->rx_tail];
			uint8_t len = s->len;
			struct net_pkt *pkt;

			#ifdef CONFIG_IEEE802154_IPRO_FRAME_DUMP
			LOG_HEXDUMP_INF(s->psdu, len, "RX");
			#endif
			if (!IS_ENABLED(CONFIG_IEEE802154_L2_PKT_INCL_FCS) && len > IPRO154_FCS_LEN) {
				len -= IPRO154_FCS_LEN;
			}

			if (data->slot_active) data->st_rx_in_slot++; else data->st_rx_out_slot++;
			pkt = net_pkt_rx_alloc_with_buffer(data->iface, len, AF_UNSPEC, 0, K_NO_WAIT);
			if (pkt == NULL) {
				LOG_DBG("no pkt buffer, dropping frame");
			} else if (net_pkt_write(pkt, s->psdu, len) != 0) {
				net_pkt_unref(pkt);
			} else {
				net_pkt_set_ieee802154_lqi(pkt, s->lqi);
				net_pkt_set_ieee802154_rssi_dbm(pkt, s->rssi);
				net_pkt_set_ieee802154_ack_fpb(pkt, s->ack_fpb);
#if defined(CONFIG_NET_PKT_TIMESTAMP)
				/* The HAL keeps 32 bits of the symbol clock; widen it
				 * against the current time (frames are far younger
				 * than the 71-minute wrap). */
				uint64_t now_sym = ipro154_timestamp_us();
				uint64_t rx_sym = now_sym - (uint32_t)((uint32_t)now_sym - s->timestamp);

				net_pkt_set_timestamp_ns(pkt, (net_time_t)radio_sym_to_k(rx_sym) * NSEC_PER_USEC);
#endif
				net_pkt_cursor_init(pkt);
				if (ieee802154_handle_ack(data->iface, pkt) == NET_OK) {
					net_pkt_unref(pkt);
				} else if (net_recv_data(data->iface, pkt) < 0) {
					net_pkt_unref(pkt);
				}
			}

			data->rx_tail = (data->rx_tail + 1) % CONFIG_IEEE802154_IPRO_RX_QUEUE_DEPTH;
		}
	}
}

/* ---------------------------------------------------------------------- */
/* Radio API                                                               */
/* ---------------------------------------------------------------------- */

static enum ieee802154_hw_caps radio_get_capabilities(const struct device *dev)
{
	ARG_UNUSED(dev);
	return IEEE802154_HW_FCS | IEEE802154_HW_FILTER | IEEE802154_HW_PROMISC |
	       IEEE802154_HW_CSMA | IEEE802154_HW_TX_RX_ACK | IEEE802154_HW_RX_TX_ACK |
	       IEEE802154_HW_ENERGY_SCAN | IEEE802154_HW_RXTIME | IEEE802154_HW_TXTIME |
	       (IS_ENABLED(CONFIG_IEEE802154_IPRO_TX_SEC) ? IEEE802154_HW_TX_SEC : 0);
}

static int radio_cca(const struct device *dev)
{
	const struct radio_config *cfg = dev->config;
	struct radio_data *data = dev->data;

	if (!data->started) {
		return -ENETDOWN;
	}
	return ipro154_cca_clear(cfg->cca_threshold) ? 0 : -EBUSY;
}

static int radio_set_channel(const struct device *dev, uint16_t channel)
{
	struct radio_data *data = dev->data;

	if (channel < IPRO154_CHANNEL_MIN || channel > IPRO154_CHANNEL_MAX) {
		return channel < 27 ? -ENOTSUP : -EINVAL;
	}
	if (channel == data->channel) {
		return -EALREADY;
	}
	if (ipro154_set_channel(channel) != 0) {
		return -EIO;
	}
	data->channel = channel;
	return 0;
}

static int radio_filter(const struct device *dev, bool set, enum ieee802154_filter_type type,
			  const struct ieee802154_filter *filter)
{
	struct radio_data *data = dev->data;

	if (!set) {
		return -ENOTSUP;
	}
	switch (type) {
	case IEEE802154_FILTER_TYPE_IEEE_ADDR:
		memcpy(data->ext_addr, filter->ieee_addr, sizeof(data->ext_addr));
		/* Both the L2 link address and OpenThread's otExtAddress arrive
		 * here in "as printed" order (MSB first), which is exactly what
		 * the SDK's ieee802154_set_ext_addr() takes (its own OpenThread
		 * adapter passes otExtAddress::m8 straight through). Reversing it
		 * here programmed the RX address filter backwards: broadcasts and
		 * scans worked, but no unicast to this node - Parent Responses,
		 * pings - ever passed the filter or got an ACK. */
		return ipro154_set_ext_addr_le(filter->ieee_addr) == 0 ? 0 : -EIO;
	case IEEE802154_FILTER_TYPE_SHORT_ADDR:
		return ipro154_set_short_addr(filter->short_addr) == 0 ? 0 : -EIO;
	case IEEE802154_FILTER_TYPE_PAN_ID:
		return ipro154_set_pan_id(filter->pan_id) == 0 ? 0 : -EIO;
	default:
		return -ENOTSUP;
	}
}

static int radio_set_txpower(const struct device *dev, int16_t dbm)
{
	struct radio_data *data = dev->data;

	if (dbm < IPRO154_TX_POWER_MIN || dbm > IPRO154_TX_POWER_MAX) {
		return -EINVAL;
	}
	if (ipro154_set_tx_power((int8_t)dbm) != 0) {
		return -EIO;
	}
	data->tx_power = (int8_t)dbm;
	return 0;
}

static void radio_deliver_ack(struct radio_data *data)
{
	uint8_t len = data->ack_len;
	struct net_pkt *pkt;

	/* The HAL strips the FCS from ACKs; the stack expects it present. */
	if (IS_ENABLED(CONFIG_IEEE802154_L2_PKT_INCL_FCS) && len == 3) {
		data->ack_psdu[3] = 0;
		data->ack_psdu[4] = 0;
		len = 5;
	}
	pkt = net_pkt_rx_alloc_with_buffer(data->iface, len, AF_UNSPEC, 0, K_NO_WAIT);
	if (pkt == NULL) {
		return;
	}
	if (net_pkt_write(pkt, data->ack_psdu, len) == 0) {
		net_pkt_set_ieee802154_lqi(pkt, data->ack_lqi);
		net_pkt_set_ieee802154_rssi_dbm(pkt, data->ack_rssi);
		net_pkt_set_ieee802154_ack_fpb(pkt, data->ack_fpb);
#if defined(CONFIG_NET_PKT_TIMESTAMP)
		if (data->ack_ts != 0) {
			/* CSL: the peer's phase in an Enh-ACK's CSL IE is relative
			 * to this time. Widen the 32-bit MAC time, convert. */
			uint64_t now_sym = ipro154_timestamp_us();
			uint64_t ts_sym = now_sym - (uint32_t)((uint32_t)now_sym - data->ack_ts);

			net_pkt_set_timestamp_ns(pkt, (net_time_t)radio_sym_to_k(ts_sym) * NSEC_PER_USEC);
		}
#endif
		/* The L2 reads the ACK from the cursor; rewind after the write. */
		net_pkt_cursor_init(pkt);
		(void)ieee802154_handle_ack(data->iface, pkt);
	}
	net_pkt_unref(pkt);
}

static int radio_stop(const struct device *dev);
static int radio_secure_enh_ack(uint8_t *ack, uint8_t len, uint8_t max);

struct radio_exact_ctx {
	struct radio_data *data;
	struct ipro154_frame_info fi;
	uint8_t *frag;
};

/* ipro154_transmit_exact() callback: CSL IE for the frame's SFD time, then
 * security with a new frame counter (every attempt: the IE differs), copied
 * back into the stack's frame. */
static int radio_prepare_exact(uint8_t *psdu, uint8_t len, uint64_t sfd_sym_us, void *arg)
{
	struct radio_exact_ctx *c = arg;
	int rc;

	ipro154_csl_ie_fill(psdu, &c->fi, &c->data->sec, radio_sym_to_k(sfd_sym_us));
	if (!c->fi.sec_enabled) {
		return 0;
	}
	rc = ipro154_sec_secure(&c->data->sec, psdu, len, &c->fi, c->data->ext_addr, false);
	if (rc == 0) {
		/* frame counter + key index (moved to the current key after a key
		 * switch) back into the stack's frame */
		memcpy(&c->frag[c->fi.aux_off + 1], &psdu[c->fi.aux_off + 1], 5);
	}
	return rc;
}

static int radio_tx(const struct device *dev, enum ieee802154_tx_mode mode, struct net_pkt *pkt,
		      struct net_buf *frag)
{
	struct radio_data *data = dev->data;
	struct ipro154_tx_frame tx;
	uint8_t len = frag->len;

	if (!data->started) {
		return -ENETDOWN;
	}
	if (len > IPRO154_MAX_PSDU - IPRO154_FCS_LEN) {
		return -EMSGSIZE;
	}
	/* A transmit takes over from a pending receive window: the window's
	 * end must not switch the receiver off while this frame waits for
	 * its ACK (a CSL child answering inside its sampling window lost
	 * every Enh-ACK that way). The stack sleeps the radio itself once
	 * the transmit has completed. */
	(void)k_work_cancel_delayable(&data->rx_slot_end_work);

	switch (mode) {
	case IEEE802154_TX_MODE_DIRECT:
	case IEEE802154_TX_MODE_CCA:
	case IEEE802154_TX_MODE_CSMA_CA:
	case IEEE802154_TX_MODE_TXTIME:
	case IEEE802154_TX_MODE_TXTIME_CCA:
		break;
	default:
		return -ENOTSUP;
	}

	/* Transmit security and CSL IE (IEEE802154_HW_TX_SEC): the stack hands
	 * secured frames over unencrypted; a frame carrying a CSL IE goes out
	 * as a timed transmit so the phase written into the IE is exact. */
	struct ipro154_frame_info fi;
	uint64_t txtime_target = 0;
	bool csl_frame = false;

	if (ipro154_frame_parse(frag->data, len, &fi) == 0) {
		csl_frame = fi.csl_ie_off >= 0 && data->sec.csl_period != 0;
	}
	/* A CSL-IE frame in CSMA mode goes out at a time fixed before it is
	 * written: "The CslPhase field value contained in a packet MUST be
	 * correct relative to the exact moment the frame was sent out" (Thread
	 * 1.4.1 §3.2.6.3.5). The HAL does the CSMA-CA and calls
	 * radio_prepare_exact() to write the IE and secure its copy; the
	 * stack's frame keeps the plaintext and gets the frame counter used. */
	bool exact = csl_frame && mode == IEEE802154_TX_MODE_CSMA_CA;
	struct radio_exact_ctx xctx = { .data = data, .fi = fi, .frag = frag->data };

	if (!exact && fi.csl_ie_off >= 0 && net_pkt_ieee802154_frame_secured(pkt) &&
	    ipro154_sec_is_retransmission(&data->sec, frag->data, len, &fi)) {
		/* Retransmission: the CSL phase must be recomputed for this
		 * attempt, so start again from the plaintext and secure it
		 * with a new frame counter (Thread 1.4.1 §3.2.6.3.7: a changed
		 * CSL IE needs a new counter, same sequence number). Keyed on the
		 * frame's CSL IE, not the CSL state: a frame the exact path sent
		 * (plaintext left in the stack's buffer, marked secured) is
		 * secured again even if CSL was switched off since. */
		memcpy(frag->data, data->sec.plain_psdu, len);
		net_pkt_set_ieee802154_frame_secured(pkt, false);
		net_pkt_set_ieee802154_mac_hdr_rdy(pkt, false);
	}
	if (mode == IEEE802154_TX_MODE_TXTIME || mode == IEEE802154_TX_MODE_TXTIME_CCA) {
		txtime_target = (uint64_t)net_pkt_timestamp_ns(pkt) / NSEC_PER_USEC;
	} else if (csl_frame && !exact) {
		/* the phase for the expected SFD time (typical CSMA delay) */
		ipro154_csl_ie_fill(frag->data, &fi, &data->sec,
				    radio_now_us() + IPRO154_CSL_TX_EST_US);
	}
	if (!exact && fi.sec_enabled && !net_pkt_ieee802154_frame_secured(pkt)) {
		int rc = ipro154_sec_secure(&data->sec, frag->data, len, &fi, data->ext_addr,
					    net_pkt_ieee802154_mac_hdr_rdy(pkt));

		if (rc != 0) {
			LOG_WRN("tx security failed: %d", rc);
			return -EIO;
		}
		net_pkt_set_ieee802154_frame_secured(pkt, true);
		net_pkt_set_ieee802154_mac_hdr_rdy(pkt, true);
	}

	tx.psdu = frag->data;
	tx.len = len;
#ifdef CONFIG_IEEE802154_IPRO_FRAME_DUMP
	LOG_HEXDUMP_INF(frag->data, len, "TX");
#endif
	tx.channel = data->channel;
	tx.tx_power = data->tx_power;
	tx.ack_request = ieee802154_is_ar_flag_set(frag);
	tx.csma_ca = mode == IEEE802154_TX_MODE_CSMA_CA;
	tx.max_csma_backoffs = 4;
	tx.max_frame_retries = 3;

	/* The MAC's CSMA-CA gives up after max_csma_backoffs failed CCAs and
	 * reports CHANNEL_BUSY. On the bench (2.4 GHz crowded by Wi-Fi) one in
	 * four beacon requests hit that on channel 11 and the whole scan failed
	 * with -EBUSY, so a CSMA frame gets a few whole extra CSMA rounds
	 * before the busy result is passed up. */
	data->tx_busy = true;
	data->stop_pending = false;
	for (int attempt = 0;; attempt++) {
		k_timeout_t wait = K_MSEC(100);

		k_sem_reset(&data->tx_done);
		data->tx_result = -EIO;
		data->ack_len = 0;

		if (txtime_target != 0) {
			/* Timed frame (CSL transmitter, or any TXTIME request): the
			 * target is on the same symbol-counter clock as get_time().
			 * The MAC cannot do CCA for a timed frame, so TXTIME_CCA is
			 * sent without it. */
			uint64_t target_us = txtime_target;
			uint64_t now_us = radio_now_us();
			int rc;


			if (target_us <= now_us + IPRO154_TXTIME_GUARD_US) {
				data->st_txtime_late++;
				rc = ipro154_transmit(&tx);
			} else {
				data->st_txtime++;
				rc = ipro154_transmit_at(&tx, radio_k_to_sym(target_us));
				wait = K_USEC(target_us - now_us + 100 * USEC_PER_MSEC);
			}
			if (rc != 0) {
				data->st_txtime_fail++;
				data->tx_busy = false;
			return -EIO;
			}
		} else if (exact) {
			int rc = ipro154_transmit_exact(&tx, radio_prepare_exact, &xctx);

			if (rc == -2) {
				data->tx_result = -EBUSY;   /* channel access failure */
				if (attempt >= CONFIG_IEEE802154_IPRO_CSMA_RETRIES) {
					break;
				}
				k_sleep(K_MSEC(1 + (sys_rand32_get() & 3U)));
				continue;
			}
			if (rc != 0) {
				data->tx_busy = false;
				return -EIO;
			}
			if (fi.sec_enabled) {
				/* the counter used is in the stack's (plaintext) frame */
				net_pkt_set_ieee802154_frame_secured(pkt, true);
				net_pkt_set_ieee802154_mac_hdr_rdy(pkt, true);
			}
		} else if (ipro154_transmit(&tx) != 0) {
			data->tx_busy = false;
			return -EIO;
		}
		if (k_sem_take(&data->tx_done, wait) != 0) {
			LOG_WRN("tx timeout");
			data->tx_busy = false;
			return -EIO;
		}
		if (txtime_target != 0 && data->tx_result == 0) {
			/* Where did the end of the SFD land vs the requested time?
			 * TX_END - (PHR + PSDU + FCS) * 32 us; +19 us with one attempt. */
			uint64_t sfd_end = radio_sym_to_k(ipro154_last_tx_end_us()) - ((uint64_t)len + 3u) * 32u;

			int32_t err = (int32_t)(sfd_end - txtime_target);

			data->st_txtime_err_last = err;
			data->st_txtime_err_hist[err < 250 ? 0 : err < 500 ? 1 : err < 1000 ? 2 : err < 5000 ? 3 : err < 20000 ? 4 : 5]++;
			if (data->st_txtime == 1 || err > data->st_txtime_err_max) data->st_txtime_err_max = err;
			if (data->st_txtime == 1 || err < data->st_txtime_err_min) data->st_txtime_err_min = err;
		}
		if (data->tx_result != -EBUSY || mode != IEEE802154_TX_MODE_CSMA_CA ||
		    txtime_target != 0 || attempt >= CONFIG_IEEE802154_IPRO_CSMA_RETRIES) {
			break;
		}
		k_sleep(K_MSEC(1 + (sys_rand32_get() & 3U)));
	}
	if (data->tx_result == 0 && tx.ack_request && data->ack_len != 0) {
		/* IEEE802154_HW_TX_RX_ACK: the ACK must reach the upper layer
		 * (ieee802154_handle_ack) before tx() returns - OpenThread reads
		 * its Frame Pending bit right after the transmit to decide whether
		 * a sleepy child stays awake for the parent's indirect frame. */
		radio_deliver_ack(data);
	} else if (data->tx_result == 0 && tx.ack_request) {
		LOG_DBG("acked tx without ack frame");
	}
	{
		k_spinlock_key_t key = k_spin_lock(&data->slot_lock);

		data->tx_busy = false;
		if (data->slot_end_pending) {
			/* the window ended while this frame waited for its ACK */
			data->slot_end_pending = false;
			k_work_reschedule(&data->rx_slot_end_work, K_NO_WAIT);
		}
		k_spin_unlock(&data->slot_lock, key);
	}
	if (data->stop_pending) {
		data->stop_pending = false;
		(void)radio_stop(dev);
	}
	return data->tx_result;
}

static int radio_start(const struct device *dev)
{
	struct radio_data *data = dev->data;

	/* An explicit start (the stack wants the receiver on) also ends any
	 * pending receive-window bookkeeping: the window's end must not turn
	 * the receiver off behind the stack's back. */
	(void)k_work_cancel_delayable(&data->rx_slot_end_work);
	data->slot_end_pending = false;
	/* Idempotent: the L2 restarts the radio for every scan/association
	 * (ieee802154_mgmt.c) and treats any error as "could not start". */
	if (data->started) {
		return 0;
	}
	if (ipro154_enable() != 0) {
		return -EIO;
	}
	data->started = true;
	return 0;
}

static int radio_stop(const struct device *dev)
{
	struct radio_data *data = dev->data;

	if (!data->started) {
		return 0;
	}
	if (data->tx_busy) {
		data->stop_pending = true;
		data->st_stop_deferred++;
		return 0;
	}
	(void)ipro154_disable();
	data->started = false;
	return 0;
}

/* CSL sample period in µs, 0 when this node is not a CSL receiver. */
static uint64_t radio_csl_period_us(const struct radio_data *data)
{
	return (uint64_t)data->sec.csl_period * IPRO154_CSL_US_PER_TEN_SYMBOLS;
}

/* Arm the start work so the window at start_us opens one tick early. */
static void radio_arm_slot(struct radio_data *data, uint64_t start_us)
{
	uint32_t slack_us = USEC_PER_SEC / CONFIG_SYS_CLOCK_TICKS_PER_SEC;
	uint64_t now_us = radio_now_us();

	data->slot_next_us = start_us;
	k_work_reschedule(&data->rx_slot_start_work,
			  K_USEC(start_us > now_us + slack_us ? start_us - now_us - slack_us : 0));
}

static void radio_rx_slot_start(struct k_work *work)
{
	struct k_work_delayable *dw = k_work_delayable_from_work(work);
	struct radio_data *data = CONTAINER_OF(dw, struct radio_data, rx_slot_start_work);
	uint64_t period_us = radio_csl_period_us(data);
	uint64_t now_us;

	if (data->rx_slot_channel != data->channel &&
	    ipro154_set_channel(data->rx_slot_channel) == 0) {
		data->channel = data->rx_slot_channel;
	}
	if (!data->started && ipro154_enable() == 0) {
		data->started = true;
	}
	k_spinlock_key_t key = k_spin_lock(&data->slot_lock);

	now_us = radio_now_us();
	data->st_rx_slots++;
	data->slot_active = true;
	data->slot_end_pending = false;
	data->slot_end_rx_waits = 0;
	data->slot_open_us = data->slot_next_us;
	{
		int32_t err = (int32_t)(now_us - data->slot_open_us);

		data->st_slot_open_err_us = err;
		data->st_slot_open_hist[err < 0 ? 0 : err < 1000 ? 1 : err < 2000 ? 2 : err < 5000 ? 3 : err < 50000 ? 4 : 5]++;
		if (data->st_rx_slots == 1 || err > data->st_slot_open_err_max) data->st_slot_open_err_max = err;
		if (data->st_rx_slots == 1 || err < data->st_slot_open_err_min) data->st_slot_open_err_min = err;
	}
	k_work_schedule(&data->rx_slot_end_work, K_USEC(data->rx_slot_duration_us));

	/* CSL receiver: arm the next sample window one period later right away.
	 * The stack requests every window from its own thread, which is often
	 * 5-50 ms late under traffic, and the window end is skipped while a
	 * transmit is in flight or the stack turned the receiver on itself -
	 * chaining from here keeps the schedule whatever happens in between.
	 * The stack's request for a window armed here only refines its time
	 * (radio_rx_slot). */
	if (period_us != 0 && data->slot_open_us != 0) {
		uint64_t next = data->slot_open_us + period_us;

		while (next <= now_us) {
			next += period_us;
		}
		data->st_slot_auto++;
		radio_arm_slot(data, next);
	}
	k_spin_unlock(&data->slot_lock, key);
}

static void radio_rx_slot_end(struct k_work *work)
{
	struct k_work_delayable *dw = k_work_delayable_from_work(work);
	struct radio_data *data = CONTAINER_OF(dw, struct radio_data, rx_slot_end_work);
	k_spinlock_key_t key = k_spin_lock(&data->slot_lock);

	if (data->tx_busy) {
		/* A frame sent from inside the window is waiting for its ACK:
		 * closing the window now would switch the receiver off under
		 * it. radio_tx() replays the end once the transmit completes. */
		data->st_slot_end_deferred++;
		data->slot_end_pending = true;
		k_spin_unlock(&data->slot_lock, key);
		return;
	}
	if (ipro154_rx_busy() && data->slot_end_rx_waits < IPRO154_SLOT_END_RX_WAITS) {
		/* The window only has to catch the SFD: a frame that started
		 * inside it is received to the end and acknowledged. */
		data->slot_end_rx_waits++;
		data->st_slot_end_rx_ext++;
		k_work_reschedule(&data->rx_slot_end_work, K_USEC(USEC_PER_SEC / CONFIG_SYS_CLOCK_TICKS_PER_SEC));
		k_spin_unlock(&data->slot_lock, key);
		return;
	}
	data->slot_active = false;
	k_spin_unlock(&data->slot_lock, key);
	if (data->started) {
		(void)ipro154_disable();
		data->started = false;
	}
	if (data->event_handler != NULL) {
		data->event_handler(DEVICE_DT_INST_GET(0), IEEE802154_EVENT_RX_OFF, NULL);
	}
}

/* Schedule a receive window. start is on the get_time() clock; a start in
 * the past opens the window immediately. The kernel tick is 1 ms, so the
 * window is opened one tick early and held one tick longer rather than
 * risk missing the sampled frame. */
static int radio_rx_slot(const struct device *dev, const struct ieee802154_config *config)
{
	struct radio_data *data = dev->data;
	const __typeof__(config->rx_slot) *slot = &config->rx_slot;
	uint64_t start_us = (uint64_t)slot->start / NSEC_PER_USEC;
	uint64_t period_us = radio_csl_period_us(data);
	uint32_t slack_us = USEC_PER_SEC / CONFIG_SYS_CLOCK_TICKS_PER_SEC;

	if (slot->channel < IPRO154_CHANNEL_MIN || slot->channel > IPRO154_CHANNEL_MAX) {
		return -EINVAL;
	}
	if (slot->duration <= 0) {
		data->slot_next_us = 0;
		data->slot_open_us = 0;
		(void)k_work_cancel_delayable(&data->rx_slot_start_work);
		(void)k_work_cancel_delayable(&data->rx_slot_end_work);
		return 0;
	}
	k_spinlock_key_t key = k_spin_lock(&data->slot_lock);

	data->rx_slot_channel = slot->channel;
	data->rx_slot_duration_us = (uint32_t)((uint64_t)slot->duration / NSEC_PER_USEC) + slack_us;
	data->st_slot_dur_us = (uint32_t)((uint64_t)slot->duration / NSEC_PER_USEC);
	if (period_us != 0 && data->slot_open_us != 0) {
		int64_t quarter = (int64_t)(period_us / 4);
		int64_t d_open = (int64_t)(start_us - data->slot_open_us);

		if (d_open > -quarter && d_open < quarter) {
			/* A late request for the window already opened here: take
			 * over its edges and re-anchor the chain on them. The
			 * stack's lead before the sample shrinks after every resync
			 * with the parent, so the window chained from the previous
			 * one opens early and would close before the sample. */
			uint64_t end_us = start_us + data->rx_slot_duration_us;
			uint64_t now_us = radio_now_us();

			data->st_slot_absorbed++;
			if (!data->slot_active) {
				if (end_us > now_us) {
					radio_arm_slot(data, start_us); /* reopen now */
					k_spin_unlock(&data->slot_lock, key);
					return 0;
				}
			} else {
				data->slot_open_us = start_us;
				k_work_reschedule(&data->rx_slot_end_work,
						  K_USEC(end_us > now_us ? end_us - now_us : 0));
			}
			radio_arm_slot(data, start_us + period_us);
			k_spin_unlock(&data->slot_lock, key);
			return 0;
		}
	}
	radio_arm_slot(data, start_us);
	k_spin_unlock(&data->slot_lock, key);
	return 0;
}

/* Frame counter shared by the transmit path and the Enh-ACKs secured from
 * the MAC interrupt (ipro154_sec.c). */
unsigned int ipro154_sec_lock(void)
{
	return irq_lock();
}

void ipro154_sec_unlock(unsigned int key)
{
	irq_unlock(key);
}

static int radio_set_mac_keys(struct radio_data *data, const struct ieee802154_key *keys)
{
	static struct ipro154_sec_key made[IPRO154_SEC_MAX_KEYS];
	/* OpenThread (modules/openthread/platform/radio.c) passes prev, current, next */
	uint8_t current = (keys != NULL && keys[0].key_value != NULL && keys[1].key_value != NULL &&
			   keys[1].key_id != NULL) ? keys[1].key_id[0] : 0;

	memset(made, 0, sizeof(made));
	for (unsigned int i = 0; keys != NULL && keys[i].key_value != NULL && i < IPRO154_SEC_MAX_KEYS; i++) {
		if (keys[i].key_id_mode != 1 || keys[i].key_id == NULL) {
			LOG_WRN("key %u: only key id mode 1 is supported", i);
			continue;
		}
		if (ipro154_sec_key_make(&made[i], keys[i].key_id[0], keys[i].key_value) == -EIO) {
			LOG_ERR("AES-128 self-test failed");
			return -EIO;
		}
	}
	ipro154_sec_keys_commit(&data->sec, made, current);
	return 0;
}

static int radio_configure(const struct device *dev, enum ieee802154_config_type type,
			     const struct ieee802154_config *config)
{
	struct radio_data *data = dev->data;

	switch (type) {
	case IEEE802154_CONFIG_AUTO_ACK_FPB:
		/* Hardware source-match table: enabled/disabled globally. */
		return 0;
	case IEEE802154_CONFIG_ACK_FPB:
		if (config->ack_fpb.addr == NULL) {
			/* addr == NULL means "clear every entry" (both tables). */
			return ipro154_clear_frame_pending() == 0 ? 0 : -EIO;
		}
		return ipro154_set_frame_pending(config->ack_fpb.addr, config->ack_fpb.extended,
						 config->ack_fpb.enabled) == 0
			       ? 0
			       : -EIO;
	case IEEE802154_CONFIG_PROMISCUOUS:
		data->promiscuous = config->promiscuous;
		return ipro154_set_promiscuous(config->promiscuous) == 0 ? 0 : -EIO;
	case IEEE802154_CONFIG_PAN_COORDINATOR:
		return 0;
	case IEEE802154_CONFIG_EVENT_HANDLER:
		data->event_handler = config->event_handler;
		return 0;
	case IEEE802154_CONFIG_RX_SLOT:
		return radio_rx_slot(dev, config);
	case IEEE802154_CONFIG_CSL_PERIOD:
		data->sec.csl_period = config->csl_period;
		if (config->csl_period == 0) {
			data->slot_next_us = 0; /* CSL off: stop re-arming windows */
			data->slot_open_us = 0;
			(void)k_work_cancel_delayable(&data->rx_slot_start_work);
		}
		ipro154_set_csl_ie((uint16_t)data->sec.csl_period, (uint32_t)radio_k_to_sym(data->sec.csl_rx_time_us));
		return 0;
	case IEEE802154_CONFIG_EXPECTED_RX_TIME:
		data->sec.csl_rx_time_us = (uint64_t)config->expected_rx_time / NSEC_PER_USEC;
		ipro154_set_csl_ie((uint16_t)data->sec.csl_period, (uint32_t)radio_k_to_sym(data->sec.csl_rx_time_us));
		return 0;
	case IEEE802154_CONFIG_ENH_ACK_HEADER_IE:
		/* The software Enh-ACK (SDK HAL) carries no IEs: the parent
		 * learns our CSL phase from the IE in the frames we send it. */
		return 0;
	case IEEE802154_CONFIG_MAC_KEYS:
		return radio_set_mac_keys(data, config->mac_keys);
	case IEEE802154_CONFIG_FRAME_COUNTER:
		ipro154_sec_set_frame_counter(&data->sec, config->frame_counter, false);
		return 0;
	case IEEE802154_CONFIG_FRAME_COUNTER_IF_LARGER:
		ipro154_sec_set_frame_counter(&data->sec, config->frame_counter, true);
		return 0;
	default:
		return -ENOTSUP;
	}
}

static void radio_ed_work(struct k_work *work)
{
	struct k_work_delayable *dw = k_work_delayable_from_work(work);
	struct radio_data *data = CONTAINER_OF(dw, struct radio_data, ed_work);
	int8_t energy = -127;

	if (ipro154_energy_detect(data->channel, 1, &energy) == 0 && energy > data->ed_max) {
		data->ed_max = energy;
	}
	if (k_uptime_get_32() - data->ed_started < data->ed_duration) {
		k_work_reschedule(dw, K_MSEC(2));
		return;
	}
	if (data->ed_cb != NULL) {
		energy_scan_done_cb_t cb = data->ed_cb;

		data->ed_cb = NULL;
		cb(DEVICE_DT_INST_GET(0), data->ed_max);
	}
}

static int radio_ed_scan(const struct device *dev, uint16_t duration, energy_scan_done_cb_t done_cb)
{
	struct radio_data *data = dev->data;

	if (!data->started) {
		return -ENETDOWN;
	}
	if (data->ed_cb != NULL) {
		return -EBUSY;
	}
	/* The HAL's energy detect is a single RSSI read; sample it for the
	 * requested duration and report the maximum. The callback must not
	 * run from here: OpenThread's platform starts the next channel's scan
	 * from inside the completion handler and then clears the completion
	 * flag - a synchronous callback lost every second channel and left
	 * `ot scan energy` hanging after channel 11. */
	data->ed_cb = done_cb;
	data->ed_duration = duration == 0 ? 1 : duration;
	data->ed_max = -127;
	data->ed_started = k_uptime_get_32();
	k_work_reschedule(&data->ed_work, K_NO_WAIT);
	return 0;
}

static net_time_t radio_get_time(const struct device *dev)
{
	ARG_UNUSED(dev);
	return (net_time_t)radio_now_us() * NSEC_PER_USEC;
}

IEEE802154_DEFINE_PHY_SUPPORTED_CHANNELS(drv_attr, 11, 26);

static int radio_attr_get(const struct device *dev, enum ieee802154_attr attr,
			    struct ieee802154_attr_value *value)
{
	ARG_UNUSED(dev);
	return ieee802154_attr_get_channel_page_and_range(
		attr, IEEE802154_ATTR_PHY_CHANNEL_PAGE_ZERO_OQPSK_2450_BPSK_868_915,
		&drv_attr.phy_supported_channels, value);
}

/* ---------------------------------------------------------------------- */
/* Init                                                                    */
/* ---------------------------------------------------------------------- */

static void radio_iface_init(struct net_if *iface)
{
	const struct device *dev = net_if_get_device(iface);
	struct radio_data *data = dev->data;

	data->iface = iface;
	net_if_set_link_addr(iface, data->mac, 8, NET_LINK_IEEE802154);
	ieee802154_init(iface);
}

static int radio_init(const struct device *dev)
{
	const struct radio_config *cfg = dev->config;
	struct radio_data *data = dev->data;
	struct ipro154_init_cfg rcfg = {
		.channel = IPRO154_CHANNEL_MIN,
		.tx_power = cfg->tx_power,
		.cca_threshold = cfg->cca_threshold,
		.ext_addr = data->mac,
	};
	int err;

	k_sem_init(&data->tx_done, 0, 1);
	k_sem_init(&data->rx_sem, 0, K_SEM_MAX_LIMIT);
	k_work_init_delayable(&data->ed_work, radio_ed_work);
	k_work_init_delayable(&data->rx_slot_start_work, radio_rx_slot_start);
	ipro154_set_enh_ack_secure(radio_secure_enh_ack);
	k_work_init_delayable(&data->rx_slot_end_work, radio_rx_slot_end);
	data->channel = rcfg.channel;
	data->tx_power = cfg->tx_power;

	(void)clock_control_on(cfg->clock_dev, cfg->clock_id);

	/* EUI-64 from the efuse MAC slot; a locally administered random one if
	 * the part is unprogrammed (all zero / all ones). */
	if (ipro154_read_eui64(data->mac) != 0) {
		sys_rand_get(data->mac, sizeof(data->mac));
		data->mac[0] = (data->mac[0] & ~0x01) | 0x02;
		LOG_WRN("no efuse MAC, using a random EUI-64");
	}

	/* The RF init inside loads the crystal trim (CONFIG_IPRO7_XTAL_CAPCODE,
	 * set through SOC_IPRO7_XTAL_CAPCODE), as does every HAL radio reset. */
	err = ipro154_init(&rcfg, radio_event, data);
	if (err != 0) {
		LOG_ERR("radio init failed: %d", err);
		return -EIO;
	}

	k_thread_create(&data->rx_thread, data->rx_stack, K_KERNEL_STACK_SIZEOF(data->rx_stack),
			radio_rx_thread, (void *)dev, NULL, NULL, K_PRIO_COOP(2), 0, K_NO_WAIT);
	k_thread_name_set(&data->rx_thread, "ipro154_rx");

	LOG_INF("IPRO7 802.15.4 radio ready");
	return 0;
}

/* Clock accuracy (ppm) of the timebase behind delayed TX/RX. OpenThread reads
 * it (otPlatRadioGetCslAccuracy) while building a Parent Response for any
 * Thread 1.2+ child, so the hook must exist even without CSL support. */
static uint8_t radio_get_sch_acc(const struct device *dev)
{
	ARG_UNUSED(dev);

	return CONFIG_IEEE802154_IPRO_CSL_ACCURACY;
}

#ifdef CONFIG_SHELL
#include <zephyr/shell/shell.h>

static int cmd_ipro154_stats(const struct shell *sh, size_t argc, char **argv)
{
	const struct device *dev = DEVICE_DT_INST_GET(0);
	struct radio_data *data = dev->data;

	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
	shell_print(sh, "driver: txtime=%u late=%u fail=%u rx_slots=%u rx_dropped=%u started=%d ch=%u stop_deferred=%u",
		    data->st_txtime, data->st_txtime_late, data->st_txtime_fail, data->st_rx_slots,
		    data->rx_dropped, data->started, data->channel, data->st_stop_deferred);
	shell_print(sh, "driver: slot_end_deferred=%u rx_ext=%u slot_dur=%u us open_err last/min/max=%d/%d/%d us rx in/out slot=%u/%u",
		    data->st_slot_end_deferred, data->st_slot_end_rx_ext, data->st_slot_dur_us, data->st_slot_open_err_us,
		    data->st_slot_open_err_min, data->st_slot_open_err_max, data->st_rx_in_slot, data->st_rx_out_slot);
	shell_print(sh, "driver: window opened late <0/<1ms/<2ms/<5ms/<50ms/>=: %u/%u/%u/%u/%u/%u  re-armed=%u absorbed=%u",
		    data->st_slot_open_hist[0], data->st_slot_open_hist[1], data->st_slot_open_hist[2],
		    data->st_slot_open_hist[3], data->st_slot_open_hist[4], data->st_slot_open_hist[5],
		    data->st_slot_auto, data->st_slot_absorbed);
	shell_print(sh, "driver: txtime sfd err last/min/max=%d/%d/%d us", data->st_txtime_err_last,
		    data->st_txtime_err_min, data->st_txtime_err_max);
	shell_print(sh, "driver: timed TX sfd error <250/<500/<1k/<5k/<20k/>= us: %u/%u/%u/%u/%u/%u",
		    data->st_txtime_err_hist[0], data->st_txtime_err_hist[1], data->st_txtime_err_hist[2],
		    data->st_txtime_err_hist[3], data->st_txtime_err_hist[4], data->st_txtime_err_hist[5]);
	shell_print(sh, "sec: ext_addr %02x%02x%02x%02x%02x%02x%02x%02x fc=%u keys=%d/%d/%d idx=%u/%u/%u csl_period=%u",
		    data->ext_addr[0], data->ext_addr[1], data->ext_addr[2], data->ext_addr[3],
		    data->ext_addr[4], data->ext_addr[5], data->ext_addr[6], data->ext_addr[7],
		    data->sec.frame_counter, data->sec.keys[0].valid, data->sec.keys[1].valid,
		    data->sec.keys[2].valid, data->sec.keys[0].key_index, data->sec.keys[1].key_index,
		    data->sec.keys[2].key_index, data->sec.csl_period);
	shell_print(sh, "sec: frame counter lowered by stack %u times (last by %u)",
		    data->sec.fc_lowered, data->sec.fc_lowered_by);
	if (data->sec.last_len) {
		char hex[2 * 127 + 1];

		for (int i = 0; i < data->sec.last_len; i++) {
			snprintk(&hex[2 * i], 3, "%02x", data->sec.last_psdu[i]);
		}
		shell_print(sh, "sec: last psdu %s", hex);
		for (int i = 0; i < IPRO154_SEC_NONCE_LEN; i++) {
			snprintk(&hex[2 * i], 3, "%02x", data->sec.last_nonce[i]);
		}
		shell_print(sh, "sec: last nonce %s", hex);
	}
	ipro154_dbg_dump();
	return 0;
}

/* ipro154 clk : compare the radio clock (MAC symbol counter) with the kernel clock over 5 s */
static int cmd_ipro154_clk(const struct shell *sh, size_t argc, char **argv)
{
	uint64_t r0 = ipro154_timestamp_us();
	int64_t k0 = k_uptime_ticks();

	k_msleep(5000);
	uint64_t r1 = ipro154_timestamp_us();
	int64_t k1 = k_uptime_ticks();
	int64_t kus = k_ticks_to_us_floor64(k1 - k0);
	int64_t rus = (int64_t)(r1 - r0);

	shell_print(sh, "radio %lld us, kernel %lld us, diff %lld us (%lld ppm)", rus, kus, rus - kus,
		    kus ? (rus - kus) * 1000000 / kus : 0);
	return 0;
}

/* ipro154 prearm <0|1|2> [lead us] : Enh-ACK prepared during the reception
 * (0 off, 1 measure the RX-end prediction only, 2 on) and the timed-trigger lead */
static int cmd_ipro154_prearm(const struct shell *sh, size_t argc, char **argv)
{
	if (argc < 2 || argc > 3) {
		shell_print(sh, "usage: ipro154 prearm <0|1|2> [lead us]");
		return -EINVAL;
	}
	ipro154_set_enh_ack_prearm((uint8_t)strtoul(argv[1], NULL, 10));
	if (argc == 3) {
		ipro154_set_enh_ack_prearm_lead((uint16_t)strtoul(argv[2], NULL, 10));
	}
	return 0;
}

/* `ipro154 crc [on|off]`: have the MAC hand CRC-failed frames to the HAL,
 * which counts and drops them (`[hal] rx: crc_fail=`) - the link's frame
 * error rate. */
static int cmd_ipro154_crc(const struct shell *sh, size_t argc, char **argv)
{
	int on = argc < 2 ? -1 : strcmp(argv[1], "on") == 0;

	shell_print(sh, "CRC-failed frames %s", ipro154_crc_report(on) ? "reported" : "dropped");
	return 0;
}

#ifdef CONFIG_IEEE802154_IPRO_FRAME_TRACE
/* `ipro154 trace stream on|off`: records printed as they come, from a
 * low-priority thread, so a host captures a long test without polling.
 * The lines go through the shell that issued the trace command: its output
 * lock keeps them whole against shell, OT CLI and log output. */
static volatile bool trace_stream;
static const struct shell *volatile trace_sh;

static void trace_emit(const char *line)
{
	const struct shell *sh = trace_sh;

	if (sh != NULL) {
		shell_fprintf(sh, SHELL_NORMAL, "%s", line);
	} else {
		printk("%s", line);
	}
}

static void trace_stream_thread(void *a, void *b, void *c)
{
	ARG_UNUSED(a); ARG_UNUSED(b); ARG_UNUSED(c);
	for (;;) {
		if (!trace_stream || ipro154_trace_pump(8) == 0u) {
			k_msleep(10);
		}
	}
}
K_THREAD_DEFINE(ipro154_trace_tid, 1024, trace_stream_thread, NULL, NULL, NULL,
		K_LOWEST_APPLICATION_THREAD_PRIO, 0, 0);
#endif

/* `ipro154 trace on|off|clear|dump|stream on|off`: HAL frame trace (sniffer-free capture) */
static int cmd_ipro154_trace(const struct shell *sh, size_t argc, char **argv)
{
	const char *op = argc >= 2 ? argv[1] : "dump";

#ifdef CONFIG_IEEE802154_IPRO_FRAME_TRACE
	if (trace_sh == NULL) {
		trace_sh = sh;
		ipro154_trace_emit(trace_emit);
	}
	if (strcmp(op, "stream") == 0 && argc >= 3) {
		trace_stream = strcmp(argv[2], "on") == 0;
		return 0;
	}
#endif
	if (strcmp(op, "on") == 0) {
		ipro154_trace(1);
	} else if (strcmp(op, "off") == 0) {
		ipro154_trace(0);
	} else if (strcmp(op, "clear") == 0) {
		ipro154_trace(2);
	} else if (strcmp(op, "dump") == 0) {
		ipro154_trace(3);
	} else {
		shell_print(sh, "usage: ipro154 trace on|off|clear|dump|stream on|off");
	}
	return 0;
}

/* `ipro154 cfo [clr]`: per-sender frequency offset, SFD correlation, RSSI */
static int cmd_ipro154_cfo(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(sh);
	ipro154_cfo_dump(argc >= 2 && strcmp(argv[1], "clr") == 0);
	return 0;
}

/* `ipro154 xtal [in [out]]`: RF crystal capcode trim */
static int cmd_ipro154_xtal(const struct shell *sh, size_t argc, char **argv)
{
	int in = argc >= 2 ? (int)strtoul(argv[1], NULL, 0) : -1;
	int out = argc >= 3 ? (int)strtoul(argv[2], NULL, 0) : -1;
	uint32_t v = ipro154_xtal_capcode(in, out);

	shell_print(sh, "xtal capcode in=%u out=%u", (unsigned int)(v >> 8), (unsigned int)(v & 0xffu));
	return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_ipro154,
	SHELL_CMD(stats, NULL, "802.15.4 driver and HAL counters", cmd_ipro154_stats),
	SHELL_CMD(prearm, NULL, "Enh-ACK prepared during RX: 0 off, 1 measure, 2 on [lead us]", cmd_ipro154_prearm),
	SHELL_CMD(clk, NULL, "radio vs kernel clock rate", cmd_ipro154_clk),
	SHELL_CMD(cfo, NULL, "per-sender frequency offset, SFD correlation, RSSI [clr]", cmd_ipro154_cfo),
	SHELL_CMD(xtal, NULL, "RF crystal capcode trim [in [out]]", cmd_ipro154_xtal),
	SHELL_CMD(crc, NULL, "report CRC-failed frames to the HAL counters [on|off]", cmd_ipro154_crc),
	SHELL_CMD(trace, NULL, "HAL frame trace: on|off|clear|dump", cmd_ipro154_trace),
	SHELL_SUBCMD_SET_END);
SHELL_CMD_REGISTER(ipro154, &sub_ipro154, "IPRO 802.15.4 radio", NULL);
#endif

/* Enh-ACK security hook (MAC interrupt context). */
static int radio_secure_enh_ack(uint8_t *ack, uint8_t len, uint8_t max)
{
	struct radio_data *data = DEVICE_DT_INST_GET(0)->data;

	return ipro154_sec_secure_enh_ack(&data->sec, ack, len, max, data->ext_addr);
}

static const struct ieee802154_radio_api radio_radio_api = {
	.iface_api.init = radio_iface_init,
	.get_capabilities = radio_get_capabilities,
	.cca = radio_cca,
	.set_channel = radio_set_channel,
	.filter = radio_filter,
	.set_txpower = radio_set_txpower,
	.start = radio_start,
	.stop = radio_stop,
	.tx = radio_tx,
	.configure = radio_configure,
	.ed_scan = radio_ed_scan,
	.get_time = radio_get_time,
	.attr_get = radio_attr_get,
	.get_sch_acc = radio_get_sch_acc,
};

static const struct radio_config radio_config_0 = {
	.clock_dev = DEVICE_DT_GET(DT_INST_CLOCKS_CTLR(0)),
	.clock_id = (clock_control_subsys_t)DT_INST_CLOCKS_CELL(0, id),
	.tx_power = DT_INST_PROP(0, tx_power),
	.cca_threshold = DT_INST_PROP(0, cca_threshold),
};

#if defined(CONFIG_NET_L2_OPENTHREAD) && defined(CONFIG_OPENTHREAD_CSL_RECEIVER) && \
	(CONFIG_IEEE802154_IPRO_OT_CSL_PERIOD_US > 0)
#include <zephyr/net/openthread.h>
#include <openthread/link.h>

/* OpenThread keeps the CSL period in RAM only: set it again after every boot
 * (IEEE802154_IPRO_OT_CSL_PERIOD_US), so a reset SSED re-establishes CSL
 * synchronization instead of polling while its parent sends by CSL. */
static int ipro_ot_csl_boot(void)
{
	struct otInstance *ot = openthread_get_default_instance();

	if (ot == NULL) {
		return 0;
	}
	openthread_mutex_lock();
	if (otLinkSetCslPeriod(ot, CONFIG_IEEE802154_IPRO_OT_CSL_PERIOD_US) != OT_ERROR_NONE) {
		LOG_WRN("CSL period %u us not applied", CONFIG_IEEE802154_IPRO_OT_CSL_PERIOD_US);
	}
#if CONFIG_IEEE802154_IPRO_OT_CSL_TIMEOUT_S > 0
	(void)otLinkSetCslTimeout(ot, CONFIG_IEEE802154_IPRO_OT_CSL_TIMEOUT_S);
#endif
	openthread_mutex_unlock();
	return 0;
}
SYS_INIT(ipro_ot_csl_boot, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
#endif

#if defined(CONFIG_IEEE802154_RAW_MODE)
DEVICE_DT_INST_DEFINE(0, radio_init, NULL, &radio_data_0, &radio_config_0, POST_KERNEL,
		      CONFIG_IEEE802154_IPRO_INIT_PRIO, &radio_radio_api);
#else
#if defined(CONFIG_NET_L2_OPENTHREAD)
#include <zephyr/net/openthread.h>
#define RADIO_L2       OPENTHREAD_L2
#define RADIO_L2_CTX   NET_L2_GET_CTX_TYPE(OPENTHREAD_L2)
#define RADIO_MTU      1280
#else
#define RADIO_L2       IEEE802154_L2
#define RADIO_L2_CTX   NET_L2_GET_CTX_TYPE(IEEE802154_L2)
#define RADIO_MTU      IEEE802154_MTU
#endif
NET_DEVICE_DT_INST_DEFINE(0, radio_init, NULL, &radio_data_0, &radio_config_0,
			  CONFIG_IEEE802154_IPRO_INIT_PRIO, &radio_radio_api, RADIO_L2, RADIO_L2_CTX,
			  RADIO_MTU);
#endif
