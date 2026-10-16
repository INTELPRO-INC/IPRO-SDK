/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * The SDK fhost's network abstraction (wifi6_lwip_adapter/include/net_al.h,
 * net_al_ext.h) on Zephyr's network stack, replacing the lwIP adapter.
 *
 * TX buffers: the MAC reads frames by DMA from the shared Wi-Fi RAM and
 * needs NET_AL_TX_HEADROOM bytes in front of the payload for the fhost TX
 * descriptor, so every TX buffer (a frame from the IP stack, a management
 * frame from the supplicant, a forwarded RX payload) is a chain of
 * ipro_txbuf segments whose memory comes from a heap placed in WRAM.
 * RX: the MAC's buffer is copied into a net_pkt and returned at once; EAPOL
 * frames go to the supplicant's event loop instead of the IP stack.
 */
#include <zephyr/kernel.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/logging/log.h>
#include <string.h>
#include "net_al.h"
#include "net_al_ext.h"
#include "net_al_zephyr.h"
#include "fhost.h"
#include "fhost_api.h"
#include "wifi_mgmr_ext.h"
#include "rtos_al.h"
#include "platform_al.h"
/* The SoC header must come first: without its __DCACHE_PRESENT,
 * core_feature_cache.h compiles MFlushDCacheRange() to an empty inline and
 * the zero-copy TX clean below did nothing. */
#include "ipro_soc_regs.h"
#include "core_feature_cache.h"
#include "wifi_mgmr.h"

LOG_MODULE_REGISTER(ipro_wifi_net, CONFIG_LOG_DEFAULT_LEVEL);

/* fhost internals the lwIP adapter also reaches into */
extern int fhost_tx_start(net_al_if_t net_if, net_al_tx_t net_buf, cb_fhost_tx cfm_cb, void *arg);
extern int fhost_tx_req_do(net_al_if_t net_if, net_al_tx_t net_buf, int type, void *cfm_cb,
			   void *cfm_cb_arg, void *buf_rx, int no_cck);
extern void fhost_tx_release_buf(net_al_tx_t net_buf, uint32_t tx_status, void *cfm_cb,
				 void *cfm_cb_arg, void *buf_rx);
extern void fhost_rx_do_resend_req(void *buf, net_al_tx_t net_buf_tx, bool forward);
extern void fhost_rx_buf_push(void *net_buf);
extern int eloop_event_commit(int type, const char *inbuf, int inlen, int discardable);
#define ELOOP_EVT_WPA_L2_DATA 3

/* The lwIP adapter reserves PBUF_LINK_ENCAPSULATION_HLEN (500, lwipopts.h)
 * in front of every TX payload for the fhost TX descriptor; keep the same
 * reservation here. It must cover the SDK's NET_AL_TX_HEADROOM (net_al.h). */
#define NET_AL_ZEPHYR_TX_HEADROOM 500
BUILD_ASSERT(NET_AL_ZEPHYR_TX_HEADROOM >= NET_AL_TX_HEADROOM,
	     "TX headroom below the SDK's NET_AL_TX_HEADROOM");
#define NX_NB_L2_FILTER 2
#define VIF_MAX 2
#define ETH_HDR_LEN 14

struct ipro_txbuf {
	struct ipro_txbuf *next;
	uint8_t *payload;   /* start of this segment's data */
	uint16_t len;       /* this segment */
	uint16_t tot_len;   /* this and all following segments */
	uint8_t *mem;       /* heap block (headroom + data) or NULL for a reference */
	bool host;          /* a data frame from the Zephyr stack (counted for pacing) */
	struct net_pkt *pkt; /* zero-copy: the Zephyr packet this segment references */
};

/* OCRAM (cached, 0x11000000) is also visible uncached at +0x10000000, the
 * window the MAC DMA reads through (WRAM at 0x21050000 is the tail of it). */
#define IPRO_NOCACHE_OFFSET 0x10000000u

/* Pacing of the Zephyr -> MAC data path. Frames sit in the WRAM heap until
 * the MAC confirms them, so the heap depth is the TX window. Rather than
 * refill it one frame per confirmation (which feeds the A-MPDU builder a
 * trickle and keeps aggregates at 1-2 MPDUs), the sender is held once the
 * window is full and released only when IPRO_WIFI_TX_BURST slots are free
 * again, so every refill is a burst the MAC can aggregate. */
#define TX_SLOT_BYTES (sizeof(struct ipro_txbuf) + NET_AL_ZEPHYR_TX_HEADROOM + 1514 + 4 + 16)
#define TX_WINDOW     ((CONFIG_IPRO_WIFI_TX_HEAP_SIZE / TX_SLOT_BYTES) - 2)
BUILD_ASSERT(TX_WINDOW > CONFIG_IPRO_WIFI_TX_BURST, "TX heap too small for the burst size");
static atomic_t tx_inflight;
static K_SEM_DEFINE(tx_room, 0, 1);
static uint32_t tx_burst_left; /* frames left in the current deferred-kick burst */
uint32_t ipro_tx_zc_hit, ipro_tx_zc_miss, ipro_zc_miss_nofrag, ipro_zc_miss_multi, ipro_zc_frag0_len, ipro_zc_frag1_len, ipro_zc_miss_len;
extern void fhost_tx_sched_hold(void), fhost_tx_sched_release(void);
extern volatile uint8_t fhost_tx_sched_batch_en;

long ipro_tx_inflight_get(void)
{
	return atomic_get(&tx_inflight);
}

int ipro_tx_window_get(void)
{
	return TX_WINDOW;
}

/* Sender-thread CPU split of one data frame (execution cycles of the calling
 * thread, so preemption by the WiFi task is excluded). */
struct ipro_tx_prof ipro_tx_prof;

static inline uint64_t thread_cycles(void)
{
#ifdef CONFIG_IPRO_WIFI_TX_PROFILE
	k_thread_runtime_stats_t st;

	k_thread_runtime_stats_get(k_current_get(), &st);
	return st.execution_cycles;
#else
	return 0;
#endif
}

/* ---- WRAM heap for TX buffers ------------------------------------------- */
static uint8_t txheap_mem[CONFIG_IPRO_WIFI_TX_HEAP_SIZE] __aligned(32)
	__attribute__((section(".wifi_txheap")));
static struct k_heap txheap;
static bool txheap_ready;

static void txheap_init(void)
{
	if (!txheap_ready) {
		k_heap_init(&txheap, txheap_mem, sizeof(txheap_mem));
		txheap_ready = true;
	}
}

static struct ipro_net_if vifs[VIF_MAX];
static struct {
	struct ipro_net_if *net_if;
	uint16_t ethertype;
} l2_filter[NX_NB_L2_FILTER];
static rtos_semaphore l2_semaphore;
static rtos_mutex l2_mutex;

struct ipro_net_if *ipro_net_if_get(int vif_idx)
{
	return vif_idx >= 0 && vif_idx < VIF_MAX ? &vifs[vif_idx] : NULL;
}

void ipro_net_if_bind(int vif_idx, struct net_if *iface)
{
	struct ipro_net_if *n = ipro_net_if_get(vif_idx);

	if (n != NULL) {
		n->iface = iface;
	}
}

/* ---- checksum ------------------------------------------------------------ */

uint16_t net_ip_chksum(const void *dataptr, int len)
{
	const uint8_t *p = dataptr;
	uint32_t acc = 0;

	while (len > 1) {
		acc += (uint32_t)(p[0] << 8 | p[1]);
		p += 2;
		len -= 2;
	}
	if (len) {
		acc += (uint32_t)(p[0] << 8);
	}
	while (acc >> 16) {
		acc = (acc & 0xffff) + (acc >> 16);
	}
	return sys_cpu_to_be16((uint16_t)~acc);
}

/* ---- interfaces ---------------------------------------------------------- */

int net_if_add(net_al_if_t *net_if, const uint8_t *mac_addr, const uint32_t *ipaddr,
	       const uint32_t *netmask, const uint32_t *gw, void *vif_priv)
{
	ARG_UNUSED(ipaddr); ARG_UNUSED(netmask); ARG_UNUSED(gw);
	for (int i = 0; i < VIF_MAX; i++) {
		if (!vifs[i].added) {
			vifs[i].added = true;
			vifs[i].vif_idx = i;
			vifs[i].vif_priv = vif_priv;
			memcpy(vifs[i].mac, mac_addr, 6);
			*net_if = &vifs[i];
			LOG_INF("vif %d added, %02x:%02x:%02x:%02x:%02x:%02x", i, mac_addr[0],
				mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
			return 0;
		}
	}
	return -1;
}

const uint8_t *net_if_get_mac_addr(net_al_if_t net_if)
{
	return ((struct ipro_net_if *)net_if)->mac;
}

net_al_if_t net_if_find_from_name(const char *name)
{
	if (name == NULL) {
		return NULL;
	}
	if (strncmp(name, "wl0", 3) == 0 || strncmp(name, "sta", 3) == 0) {
		return &vifs[0];
	}
	if (strncmp(name, "wl1", 3) == 0 || strncmp(name, "ap", 2) == 0) {
		return &vifs[1];
	}
	return NULL;
}

int net_if_get_name(net_al_if_t net_if, char *buf, int len)
{
	struct ipro_net_if *n = net_if;

	return snprintk(buf, len, "wl%u", n->vif_idx);
}

void net_if_up_cb(net_al_if_t net_if)
{
	struct ipro_net_if *n = net_if;

	n->up = true;
	ipro_zif_carrier(n->iface, true);
}

/* WPA_COMPLETED: keys installed, the link carries traffic now */
void net_if_up_link(void)
{
	net_if_up_cb(&vifs[0]);
}

void net_if_down_cb(net_al_if_t net_if)
{
	struct ipro_net_if *n = net_if;

	n->up = false;
	ipro_zif_carrier(n->iface, false);
}

void *net_if_vif_info(net_al_if_t net_if)
{
	return ((struct ipro_net_if *)net_if)->vif_priv;
}

/* ---- RX ------------------------------------------------------------------- */

void net_al_input(net_al_rx_t net_buf, void *payload, net_al_if_t net_if, uint16_t length,
		  uint8_t offset, uint8_t skip_after_eth_hdr, net_buf_free_fn free_fn)
{
	struct ipro_net_if *n = net_if;
	uint8_t *frame = (uint8_t *)payload + offset;
	uint16_t frame_len = length;

	if (skip_after_eth_hdr != 0) {
		/* the Ethernet header is followed by a gap: close it */
		memmove(frame + skip_after_eth_hdr, frame, ETH_HDR_LEN); /* header first */
		frame += skip_after_eth_hdr;
		frame_len -= skip_after_eth_hdr;
	}
	/* Our own broadcasts come back through the AP. */
	if (memcmp(frame + 6, n->mac, 6) == 0) {
		free_fn(net_buf);
		return;
	}
	uint16_t ethertype = (uint16_t)(frame[12] << 8 | frame[13]);

	for (int i = 0; i < NX_NB_L2_FILTER; i++) {
		if (l2_filter[i].net_if == n && l2_filter[i].ethertype == ethertype) {
			if (eloop_event_commit(ELOOP_EVT_WPA_L2_DATA, (const char *)frame,
					       frame_len, 0)) {
				LOG_WRN("L2 frame 0x%04x dropped", ethertype);
			}
			free_fn(net_buf);
			return;
		}
	}
	if (n->iface == NULL || !n->up) {
		free_fn(net_buf);
		return;
	}
	(void)ipro_zif_rx(n->iface, frame, frame_len);
	free_fn(net_buf);
}

/* ---- TX buffers ---------------------------------------------------------- */

static struct ipro_txbuf *txbuf_new_wait(uint32_t length, bool with_mem, k_timeout_t wait)
{
	struct ipro_txbuf *b;
	size_t need = sizeof(*b) + (with_mem ? NET_AL_ZEPHYR_TX_HEADROOM + length + 4 : 0);

	txheap_init();
	b = k_heap_alloc(&txheap, need, wait);
	if (b == NULL) {
		return NULL;
	}
	memset(b, 0, sizeof(*b));
	b->len = length;
	b->tot_len = length;
	if (with_mem) {
		b->mem = (uint8_t *)(b + 1);
		b->payload = b->mem + NET_AL_ZEPHYR_TX_HEADROOM;
	}
	return b;
}

static struct ipro_txbuf *txbuf_new(uint32_t length, bool with_mem)
{
	return txbuf_new_wait(length, with_mem, K_NO_WAIT);
}

net_al_tx_t net_buf_tx_alloc(uint32_t length)
{
	return txbuf_new(length, true);
}

net_al_tx_t net_buf_tx_alloc_fill(const uint8_t *frame, uint32_t length)
{
	struct ipro_txbuf *b = txbuf_new(length, true);

	if (b != NULL) {
		memcpy(b->payload, frame, length);
	}
	return b;
}

net_al_tx_t net_buf_tx_alloc_ref(uint32_t length)
{
	return txbuf_new(length, false);
}

bool net_buf_tx_all_shram(net_al_tx_t net_buf)
{
	for (struct ipro_txbuf *b = net_buf; b != NULL; b = b->next) {
		uintptr_t p = (uintptr_t)b->payload;

		if (p < (uintptr_t)txheap_mem || p >= (uintptr_t)txheap_mem + sizeof(txheap_mem)) {
			/* MAC RX buffers referenced by a resend also live in WRAM */
			extern uint8_t _sshram[], _eshram[];

			if ((p < (uintptr_t)_sshram || p >= (uintptr_t)_eshram) &&
			    b->pkt == NULL /* zero-copy payload: OCRAM through the uncached alias */) {
				return false;
			}
		}
	}
	return true;
}

void *net_buf_tx_info(net_al_tx_t *net_buf, uint16_t *tot_len, int *seg_cnt, uint32_t seg_addr[],
		      uint16_t seg_len[], uint32_t *headroom_len)
{
	struct ipro_txbuf *b = (struct ipro_txbuf *)net_buf;
	int max = *seg_cnt, idx = 1;
	uint16_t left = b->tot_len;
	uint8_t *headroom;

	if (b->mem == NULL) {
		return NULL; /* the first segment must own its headroom */
	}
	*tot_len = left;
	seg_addr[0] = (uint32_t)b->payload;
	seg_len[0] = b->len;
	left -= b->len;
	headroom = (uint8_t *)ROUND_UP((uintptr_t)b->mem, 4);
	if (headroom_len != NULL) {
		*headroom_len = (uint32_t)(b->payload - headroom);
	}
	for (struct ipro_txbuf *s = b->next; left != 0 && s != NULL && idx < max; s = s->next) {
		seg_addr[idx] = (uint32_t)s->payload;
		seg_len[idx] = s->len;
		left -= s->len;
		idx++;
	}
	*seg_cnt = idx;
	return left == 0 ? headroom : NULL;
}

void net_buf_tx_free(net_al_tx_t net_buf)
{
	struct ipro_txbuf *b = net_buf;

	while (b != NULL) {
		struct ipro_txbuf *next = b->next;

		if (b->pkt != NULL) {
			ipro_zif_pkt_unref(b->pkt);
		}
		if (b->host) {
			/* Wake the sender only once a whole burst fits. */
			if (atomic_dec(&tx_inflight) - 1 == TX_WINDOW - CONFIG_IPRO_WIFI_TX_BURST) {
				k_sem_give(&tx_room);
			}
		}
		k_heap_free(&txheap, b);
		b = next;
	}
}

void net_buf_tx_cat(net_al_tx_t net_buf1, net_al_tx_t net_buf2)
{
	struct ipro_txbuf *a = net_buf1, *b = net_buf2, *last = a;

	while (last->next != NULL) {
		last->tot_len += b->tot_len;
		last = last->next;
	}
	last->tot_len += b->tot_len;
	last->next = b;
}

void net_buf_rx_free(net_al_rx_t net_buf)
{
	fhost_rx_buf_push(net_buf);
}

int net_al_tx_req(struct net_al_tx_req req)
{
	if (net_buf_tx_all_shram(req.net_buf)) {
		return fhost_tx_req_do(req.net_if, req.net_buf, req.type, req.cfm_cb, req.cfm_cb_arg,
				       req.buf_rx, req.no_cck);
	}
	fhost_tx_release_buf(req.net_buf, 0, req.cfm_cb, req.cfm_cb_arg, req.buf_rx);
	return -1;
}

void net_al_tx_cfm(void)
{
}

void net_al_tx_do_sta_del(uint8_t sta_id, release_buf_cb release_buf)
{
	ARG_UNUSED(sta_id); ARG_UNUSED(release_buf);
}

void net_al_tx_init(void)
{
	txheap_init();
	fhost_tx_sched_batch_en = 1; /* deferred kicks, see ipro_net_if_send() */
}

void net_al_rx_resend(bool rx_buf_forward, void *buf, void *payload, int length, int offset,
		      int mac_hdr_len)
{
	struct ipro_txbuf *hdr = net_buf_tx_alloc(mac_hdr_len);
	struct ipro_txbuf *body = net_buf_tx_alloc_ref(length - mac_hdr_len);

	if (hdr == NULL || body == NULL) {
		if (hdr != NULL) {
			net_buf_tx_free(hdr);
		}
		if (body != NULL) {
			net_buf_tx_free(body);
		}
		if (!rx_buf_forward) {
			fhost_rx_buf_push(buf);
		}
		return;
	}
	body->payload = (uint8_t *)payload + offset + mac_hdr_len;
	memcpy(hdr->payload, (uint8_t *)payload + offset, mac_hdr_len);
	net_buf_tx_cat(hdr, body);
	fhost_rx_do_resend_req(buf, hdr, rx_buf_forward);
}

/* Ethernet frame from the Zephyr stack: copy into a WRAM TX buffer and hand
 * it to the fhost. */
#ifdef CONFIG_IPRO_WIFI_TX_CSUM_OFFLOAD
/* Zephyr leaves the IPv4 header checksum to us (ipro_wifi_get_config). */
static void ipv4_hdr_csum_fill(uint8_t *frame, size_t len)
{
	uint8_t *ip = frame + ETH_HDR_LEN;
	uint32_t sum = 0;
	unsigned int ihl;

	if (len < ETH_HDR_LEN + 20 || frame[12] != 0x08 || frame[13] != 0x00) {
		return;
	}
	ihl = (ip[0] & 0x0f) * 4;
	if (ihl < 20 || len < ETH_HDR_LEN + ihl) {
		return;
	}
	ip[10] = 0;
	ip[11] = 0;
	for (unsigned int i = 0; i < ihl; i += 2) {
		sum += ((uint32_t)ip[i] << 8) | ip[i + 1];
	}
	sum = (sum & 0xffff) + (sum >> 16);
	sum = (sum & 0xffff) + (sum >> 16);
	ip[10] = (uint8_t)(~sum >> 8);
	ip[11] = (uint8_t)~sum;
}
#else
#define ipv4_hdr_csum_fill(f, l) do { } while (0)
#endif

#ifdef CONFIG_IPRO_WIFI_TX_PROFILE
extern void ipro_zp_mark(int i);
#else
#define ipro_zp_mark(i) do { } while (0)
#endif

int ipro_net_if_send(int vif_idx, struct net_pkt *pkt)
{
	struct ipro_net_if *n = ipro_net_if_get(vif_idx);
	size_t len = (size_t)ipro_zif_pkt_len(pkt);
	struct ipro_txbuf *b;

	if (n == NULL || !n->up) {
		return -ENETDOWN;
	}
	/* Backpressure rather than drop: the TX heap holds the frames the MAC
	 * has not confirmed yet, so a full heap means "the air is behind"; a
	 * short wait paces the sender (and lets TCP keep its window) instead of
	 * turning every burst into loss. Never from an ISR. */
	uint64_t c0 = thread_cycles(), c1, c2, c3;
	uint32_t w0 = k_cycle_get_32();

	if (!k_is_in_isr()) {
		while (atomic_get(&tx_inflight) >= TX_WINDOW) {
			if (tx_burst_left != 0) {
				/* about to block: let the MAC have what we queued */
				tx_burst_left = 0;
				fhost_tx_sched_release();
			}
			if (k_sem_take(&tx_room, K_TICKS(k_ms_to_ticks_ceil32(CONFIG_IPRO_WIFI_TX_WAIT_MS))) != 0) {
				return -ENOMEM;
			}
			/* A burst of slots just opened: queue them with one MAC kick
			 * at the end instead of one per frame (fewer WiFi-task
			 * wake-ups, and the MAC sees the burst at once). */
			tx_burst_left = CONFIG_IPRO_WIFI_TX_BURST;
			fhost_tx_sched_hold();
		}
	}
	ipro_tx_prof.wait_wall += k_cycle_get_32() - w0;
	c1 = thread_cycles();
	ipro_zp_mark(10);
	/* The window gate above guarantees room; K_NO_WAIT avoids a timeout setup. */
	{
		size_t flen = 0;
		uint8_t *fdata = IS_ENABLED(CONFIG_IPRO_WIFI_TX_ZERO_COPY)
					 ? ipro_zif_pkt_single_frag(pkt, &flen) : NULL;

		if (fdata != NULL && flen == len && len > ETH_HDR_LEN) {
			/* Zero copy: only the Ethernet header goes into a WRAM
			 * buffer (the MAC needs its headroom in front of it); the
			 * payload stays in the cached OCRAM net_buf, cleaned to
			 * memory and handed to the DMA through the uncached alias.
			 * Writing 1.5 KB into WRAM from the CPU costs ~180 us per
			 * frame under load. */
			struct ipro_txbuf *body;

			b = txbuf_new_wait(ETH_HDR_LEN, true, K_NO_WAIT);
			body = txbuf_new_wait(len - ETH_HDR_LEN, false, K_NO_WAIT);
			if (b == NULL || body == NULL) {
				if (b != NULL) {
					net_buf_tx_free(b);
				}
				if (body != NULL) {
					net_buf_tx_free(body);
				}
				return -ENOMEM;
			}
			ipro_zp_mark(11);
			ipro_tx_zc_hit++;
			ipv4_hdr_csum_fill(fdata, len);
			memcpy(b->payload, fdata, ETH_HDR_LEN);
			{
				/* a CCM range op is a begin-address CSR plus N commands
				 * that nothing saves across a preemption */
				unsigned int key = irq_lock();

				MFlushDCacheRange((uintptr_t)fdata, len);
				irq_unlock(key);
			}
			body->payload = fdata + ETH_HDR_LEN + IPRO_NOCACHE_OFFSET;
			ipro_zif_pkt_ref(pkt);
			body->pkt = pkt;
			net_buf_tx_cat(b, body);
			b->host = true;
			atomic_inc(&tx_inflight);
			c2 = thread_cycles();
		} else {
			ipro_tx_zc_miss++;
			if (fdata != NULL) {
				ipro_zc_miss_len = (uint32_t)flen << 16 | (uint32_t)len;
			}
			b = txbuf_new_wait(len, true, K_NO_WAIT);
			if (b == NULL) {
				return -ENOMEM;
			}
			b->host = true;
			atomic_inc(&tx_inflight);
			c2 = thread_cycles();
			if (ipro_zif_pkt_read(pkt, b->payload, len) != 0) {
				net_buf_tx_free(b);
				return -EIO;
			}
			ipv4_hdr_csum_fill(b->payload, len);
		}
	}
	c3 = thread_cycles();
	ipro_zp_mark(12);
	if (fhost_tx_start(n, b, NULL, NULL) != 0) {
		/* fhost_tx_start releases the buffer on every error */
		if (tx_burst_left != 0) {
			tx_burst_left = 0;
			fhost_tx_sched_release();
		}
		return -EIO;
	}
	ipro_zp_mark(13);
	if (tx_burst_left != 0 && --tx_burst_left == 0) {
		fhost_tx_sched_release();
	}
	ipro_tx_prof.n++;
	ipro_tx_prof.wait += c1 - c0;
	ipro_tx_prof.alloc += c2 - c1;
	ipro_tx_prof.copy += c3 - c2;
	ipro_tx_prof.fhost += thread_cycles() - c3;
	return 0;
}

/* ---- L2 (EAPOL) ---------------------------------------------------------- */

int net_init(void)
{
	for (int i = 0; i < NX_NB_L2_FILTER; i++) {
		l2_filter[i].net_if = NULL;
	}
	if (rtos_semaphore_create(&l2_semaphore, 1, 0) != 0 || rtos_mutex_create(&l2_mutex) != 0) {
		return -1;
	}
	txheap_init();
	fhost_task_ready(IP_TASK);
	return 0;
}

static void net_l2_send_cfm(uint32_t frame_id, bool acknowledged, void *arg)
{
	ARG_UNUSED(frame_id);
	if (arg != NULL) {
		*((bool *)arg) = acknowledged;
	}
	rtos_semaphore_signal(l2_semaphore, false);
}

static int net_l2_send_do(net_al_if_t net_if, const uint8_t *data, int data_len,
			  uint16_t ethertype, const uint8_t *dst_addr, bool *ack)
{
	struct ipro_net_if *n = net_if;
	struct ipro_txbuf *b;
	int res;

	if (n == NULL || data == NULL || !n->up) {
		return -1;
	}
	b = net_buf_tx_alloc(data_len + (dst_addr != NULL ? ETH_HDR_LEN : 0));
	if (b == NULL) {
		return -1;
	}
	if (dst_addr != NULL) {
		memcpy(b->payload, dst_addr, 6);
		memcpy(b->payload + 6, n->mac, 6);
		b->payload[12] = ethertype >> 8;
		b->payload[13] = ethertype & 0xff;
		memcpy(b->payload + ETH_HDR_LEN, data, data_len);
	} else {
		memcpy(b->payload, data, data_len);
	}
	rtos_mutex_lock(l2_mutex);
	res = fhost_tx_start(n, b, net_l2_send_cfm, ack);
	if (rtos_semaphore_wait(l2_semaphore, 5000)) {
		if (ack != NULL) {
			*ack = false;
		}
		rtos_mutex_unlock(l2_mutex);
		return -1;
	}
	rtos_mutex_unlock(l2_mutex);
	return res;
}

int net_l2_send(net_al_if_t net_if, const uint8_t *data, int data_len, uint16_t ethertype,
		const uint8_t *dst_addr, bool *ack)
{
	for (int tries = 0; tries < 3; tries++) {
		bool acked = false;

		if (net_l2_send_do(net_if, data, data_len, ethertype, dst_addr, &acked)) {
			return -1;
		}
		if (acked) {
			*ack = true;
			return 0;
		}
	}
	*ack = false;
	return 0;
}

int net_l2_socket_create(net_al_if_t net_if, uint16_t ethertype)
{
	int free_slot = -1;

	for (int i = 0; i < NX_NB_L2_FILTER; i++) {
		if (l2_filter[i].net_if == net_if && l2_filter[i].ethertype == ethertype) {
			return -1;
		}
		if (free_slot < 0 && l2_filter[i].net_if == NULL) {
			free_slot = i;
		}
	}
	if (free_slot < 0) {
		return -1;
	}
	l2_filter[free_slot].net_if = net_if;
	l2_filter[free_slot].ethertype = ethertype;
	return 0;
}

int net_l2_socket_delete(void)
{
	for (int i = 0; i < NX_NB_L2_FILTER; i++) {
		if (l2_filter[i].net_if != NULL) {
			l2_filter[i].net_if = NULL;
			return 0;
		}
	}
	return -1;
}

/* ---- IP configuration (net_al_ext) --------------------------------------- */

int net_al_ext_set_vif_ip(int fvif_idx, struct net_al_ext_ip_addr_cfg *cfg)
{
	struct ipro_net_if *n = ipro_net_if_get(fvif_idx);

	if (n == NULL || n->iface == NULL) {
		return -1;
	}
	switch (cfg->mode) {
	case IP_ADDR_NONE:
		ipro_zif_dhcp(n, false);
		return 0;
	case IP_ADDR_STATIC_IPV4:
		ipro_zif_ip_static(n, cfg->ipv4.addr, cfg->ipv4.mask, cfg->ipv4.gw);
		n->dns = cfg->ipv4.dns;
		return 0;
	case IP_ADDR_DHCP_CLIENT:
		ipro_zif_dhcp(n, true);
		cfg->ipv4.addr = n->ip; cfg->ipv4.mask = n->mask; cfg->ipv4.gw = n->gw;
		cfg->ipv4.dns = n->dns;
		return 0;
	default:
		return -1;
	}
}

int net_al_ext_get_vif_ip(int fvif_idx, struct net_al_ext_ip_addr_cfg *cfg)
{
	struct ipro_net_if *n = ipro_net_if_get(fvif_idx);

	if (n == NULL) {
		return -1;
	}
	cfg->mode = n->ip != 0 ? IP_ADDR_STATIC_IPV4 : IP_ADDR_NONE;
	cfg->ipv4.addr = n->ip; cfg->ipv4.mask = n->mask; cfg->ipv4.gw = n->gw;
	cfg->ipv4.dns = n->dns;
	return 0;
}

int net_al_link_set(net_al_if_t net_if)
{
	ARG_UNUSED(net_if);
	return 0;
}

int net_al_dhcpd_start(net_al_if_t net_if, int start, int limit)
{
	ARG_UNUSED(net_if); ARG_UNUSED(start); ARG_UNUSED(limit);
	return -1; /* no DHCP server (AP mode) yet */
}

int net_al_dhcpd_stop(net_al_if_t net_if)
{
	ARG_UNUSED(net_if);
	return 0;
}

int net_al_gw_service_enable(net_al_if_t net_if)
{
	ARG_UNUSED(net_if);
	return 0;
}

int net_al_gw_service_disable(net_al_if_t net_if)
{
	ARG_UNUSED(net_if);
	return 0;
}

/* DHCP client on connect/disconnect; the address event becomes the Wi-Fi
 * manager's CODE_WIFI_ON_GOT_IP. */
static void got_ip(struct ipro_net_if *n)
{
	ARG_UNUSED(n);
	LOG_INF("IPv4 address acquired");
	platform_post_event(0, CODE_WIFI_ON_GOT_IP, 0);
}

void net_al_ext_dhcp_connect(void)
{
	struct ipro_net_if *n = &vifs[0];

	ipro_zif_watch_ipv4(got_ip);
	if (wifiMgmr.sta_connect_param.use_dhcp) {
		ipro_zif_dhcp(n, true);
	}
}

void net_al_ext_dhcp_disconnect(void)
{
	struct ipro_net_if *n = &vifs[0];

	if (wifiMgmr.sta_connect_param.use_dhcp) {
		ipro_zif_dhcp(n, false);
	}
}

/* STA reconnect backstop: the supplicant's own autoconnect does the work
 * here; keep the generation token semantics the manager expects. */
static uint32_t sta_reconnect_gen;
static bool sta_reconnect_suppressed;

bool net_al_ext_sta_reconnect_valid(uint32_t generation)
{
	return !sta_reconnect_suppressed && generation == sta_reconnect_gen;
}

void net_al_ext_sta_reconnect_arm(void)
{
	sta_reconnect_suppressed = false;
}

void net_al_ext_sta_reconnect_reset(void)
{
	sta_reconnect_gen++;
}

void net_al_ext_sta_reconnect_suppress(void)
{
	sta_reconnect_suppressed = true;
	sta_reconnect_gen++;
}
