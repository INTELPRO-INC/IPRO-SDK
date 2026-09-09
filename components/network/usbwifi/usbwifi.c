/*
 * Copyright (c) 2026 IntelPro Technology Co., Ltd.
 * All rights reserved.
 *
 * This software and associated documentation files (the "Software") are
 * proprietary and confidential to IntelPro Technology Co., Ltd..
 *
 * Access to and use of this Software is limited exclusively to authorized 
 * customers and partners of IntelPro Technology Co., Ltd. who have entered into a 
 * valid license or non-disclosure agreement.
 *
 * Unauthorized copying, modification, distribution, or reverse engineering 
 * of this Software, via any medium, is strictly prohibited without 
 * prior written permission from IntelPro Technology Co., Ltd..
 *
 * SPDX-License-Identifier: Proprietary
 */

#include "usbwifi.h"
#include <assert.h>
#include <stdio.h>
#if defined(IPRO6LE) || defined(IPRO6)
#if defined(IPRO6LE) || defined(IPRO6)
#include "drv_l1c.h"
#else
#include <bl616_l1c.h>
#endif
#endif
#include "usb.h"
#include "utils.h"

#include <wifi_pkt_hooks.h>
#include <net_pkt_filter.h>

usbwifi_t g_usbwifi;

static int rnm_tx_data(void *arg, rnm_output_data_type_t type, const void *data, uint16_t data_length)
{
    struct usbwifi_rx_env_tag *env = arg;

    if (type == RNM_OUT_COMMAND) {
        size_t head = usbwifi_rx_buf_reserve_head();
        size_t len = head + data_length;
        void *copy;
        if ((copy = pvPortMalloc(len))) {
            L1C_DCache_Clean_Invalid_By_Addr((uintptr_t)copy, len);
            void *cp = (void *)MAWOC(copy);
            memcpy(cp + head, data, data_length);
            if (usbwifi_bulk_in(env, cp + head, data_length, USBWIFI_DATA_TYPE_CMD, vPortFree, copy)) {
                vPortFree(copy);
            }
        }
    }
    return 0;
}

/* Phase 2A.7 RX-parity instrumentation. Localizes the -43%/-55% RX-direction
 * gap vs legacy bl_iot_sdk by counting frames at each layer of the
 * lwIP -> ipro_dual_stack_peer_input -> usbwifi_bulk_in path, plus per-call
 * CPU time in the staging-copy hot path. Reset/print via dual_stack_diag
 * shell cmds. */
extern uint32_t inline_hal_machw_time(void);

static struct {
    /* Counter B: eth_input_hook fan-out */
    volatile uint32_t hook_total;
    volatile uint32_t hook_8021x;
    volatile uint32_t hook_host_in;
    volatile uint32_t hook_emb_in;
    volatile uint32_t hook_both;
    volatile uint32_t hook_dropped;
    /* Counter C: ipro_dual_stack_peer_input throughput */
    volatile uint32_t peer_calls;
    volatile uint32_t peer_submit_ok;
    volatile uint32_t peer_submit_fail;
    volatile uint32_t peer_malloc_fail;
    volatile uint32_t peer_bytes;
    /* Counter D: per-call CPU time (us) */
    volatile uint32_t peer_dur_total_us;
    volatile uint32_t peer_dur_max_us;
    volatile uint32_t peer_dur_lt10us;
    volatile uint32_t peer_dur_lt100us;
    volatile uint32_t peer_dur_lt1ms;
    volatile uint32_t peer_dur_ge1ms;
    /* Counter E (Phase 2A.7 follow-up): USB EP IN pipeline state */
    volatile uint32_t usb_submit_direct;   /* usbwifi_bulk_in -> usb_bulk_in immediate */
    volatile uint32_t usb_submit_queued;   /* usbwifi_bulk_in -> pendding_tx_list */
    volatile uint32_t usb_submit_drained;  /* ISR pops from pendding_tx_list */
    volatile uint32_t usb_complete;        /* completion ISR fires (frame done) */
    volatile uint32_t usb_pending_max;     /* peak pendding_tx_list depth */
    /* Counter F (Phase 2A.8 bisect): per-segment CPU time inside peer_input.
     * Totals divided by peer_submit_ok in print path. Localizes which of
     * malloc / flush / uncached-copy / usbwifi_bulk_in dominates the 196us
     * mean from Counter D, so Phase 2A.8 fix can target the slow segment. */
    volatile uint32_t seg_malloc_us_total;
    volatile uint32_t seg_malloc_us_max;
    volatile uint32_t seg_flush_us_total;
    volatile uint32_t seg_flush_us_max;
    volatile uint32_t seg_copy_us_total;
    volatile uint32_t seg_copy_us_max;
    volatile uint32_t seg_bulk_us_total;
    volatile uint32_t seg_bulk_us_max;
    /* Counter J (Phase 2A.10 follow-up): bisect 11ms RX buffer hold time.
     * (a) = submit_us -> complete_us : USB EP IN submit to ISR fire.
     *       Captures RPi xHCI completion latency + USB HS bulk physical
     *       transfer time. If dominant, single-URB-in-flight is the cap
     *       and Step 2 (multi-URB) is the right next move.
     * (b) = complete_us -> push      : ISR fire to fhost_rx_buf_push.
     *       Captures usb_worker wakeup latency + zerocp_pbuf_free +
     *       custom destructor. If dominant, the cap is in DUT-side ISR
     *       scheduling or pbuf chain handling. */
    volatile uint32_t dj_cnt;
    volatile uint32_t dj_a_sum_us;
    volatile uint32_t dj_a_max_us;
    volatile uint32_t dj_a_lt500us;
    volatile uint32_t dj_a_lt2ms;
    volatile uint32_t dj_a_lt5ms;
    volatile uint32_t dj_a_lt10ms;
    volatile uint32_t dj_a_ge10ms;
    volatile uint32_t dj_b_sum_us;
    volatile uint32_t dj_b_max_us;
    volatile uint32_t dj_b_lt100us;
    volatile uint32_t dj_b_lt500us;
    volatile uint32_t dj_b_lt1ms;
    volatile uint32_t dj_b_lt5ms;
    volatile uint32_t dj_b_ge5ms;
} g_dsd;

void dual_stack_diag_print(void)
{
    printf("[DSD] hook total=%lu 8021x=%lu host=%lu emb=%lu both=%lu drop=%lu\r\n",
           (unsigned long)g_dsd.hook_total,
           (unsigned long)g_dsd.hook_8021x,
           (unsigned long)g_dsd.hook_host_in,
           (unsigned long)g_dsd.hook_emb_in,
           (unsigned long)g_dsd.hook_both,
           (unsigned long)g_dsd.hook_dropped);
    printf("[DSD] peer calls=%lu sub_ok=%lu sub_fail=%lu malloc_fail=%lu bytes=%lu\r\n",
           (unsigned long)g_dsd.peer_calls,
           (unsigned long)g_dsd.peer_submit_ok,
           (unsigned long)g_dsd.peer_submit_fail,
           (unsigned long)g_dsd.peer_malloc_fail,
           (unsigned long)g_dsd.peer_bytes);
    uint32_t cnt = g_dsd.peer_submit_ok;
    uint32_t avg = cnt ? (g_dsd.peer_dur_total_us / cnt) : 0;
    printf("[DSD] dur_us avg=%lu max=%lu <10us=%lu <100us=%lu <1ms=%lu >=1ms=%lu\r\n",
           (unsigned long)avg,
           (unsigned long)g_dsd.peer_dur_max_us,
           (unsigned long)g_dsd.peer_dur_lt10us,
           (unsigned long)g_dsd.peer_dur_lt100us,
           (unsigned long)g_dsd.peer_dur_lt1ms,
           (unsigned long)g_dsd.peer_dur_ge1ms);
    printf("[DSD] usb direct=%lu queued=%lu drained=%lu complete=%lu pend_max=%lu\r\n",
           (unsigned long)g_dsd.usb_submit_direct,
           (unsigned long)g_dsd.usb_submit_queued,
           (unsigned long)g_dsd.usb_submit_drained,
           (unsigned long)g_dsd.usb_complete,
           (unsigned long)g_dsd.usb_pending_max);
    uint32_t mavg = cnt ? (g_dsd.seg_malloc_us_total / cnt) : 0;
    uint32_t favg = cnt ? (g_dsd.seg_flush_us_total  / cnt) : 0;
    uint32_t cavg = cnt ? (g_dsd.seg_copy_us_total   / cnt) : 0;
    uint32_t bavg = cnt ? (g_dsd.seg_bulk_us_total   / cnt) : 0;
    printf("[DSD] seg_us malloc=%lu/%lu flush=%lu/%lu copy=%lu/%lu bulk=%lu/%lu (avg/max)\r\n",
           (unsigned long)mavg, (unsigned long)g_dsd.seg_malloc_us_max,
           (unsigned long)favg, (unsigned long)g_dsd.seg_flush_us_max,
           (unsigned long)cavg, (unsigned long)g_dsd.seg_copy_us_max,
           (unsigned long)bavg, (unsigned long)g_dsd.seg_bulk_us_max);
    /* Counter J: 11ms hold-time bisect */
    uint32_t jcnt = g_dsd.dj_cnt;
    uint32_t aavg = jcnt ? (g_dsd.dj_a_sum_us / jcnt) : 0;
    uint32_t bavg2 = jcnt ? (g_dsd.dj_b_sum_us / jcnt) : 0;
    printf("[DSD] J cnt=%lu a(submit->compl) avg=%lu max=%lu <500us=%lu <2ms=%lu <5ms=%lu <10ms=%lu >=10ms=%lu\r\n",
           (unsigned long)jcnt,
           (unsigned long)aavg, (unsigned long)g_dsd.dj_a_max_us,
           (unsigned long)g_dsd.dj_a_lt500us, (unsigned long)g_dsd.dj_a_lt2ms,
           (unsigned long)g_dsd.dj_a_lt5ms,   (unsigned long)g_dsd.dj_a_lt10ms,
           (unsigned long)g_dsd.dj_a_ge10ms);
    printf("[DSD] J b(compl->push)   avg=%lu max=%lu <100us=%lu <500us=%lu <1ms=%lu <5ms=%lu >=5ms=%lu\r\n",
           (unsigned long)bavg2, (unsigned long)g_dsd.dj_b_max_us,
           (unsigned long)g_dsd.dj_b_lt100us, (unsigned long)g_dsd.dj_b_lt500us,
           (unsigned long)g_dsd.dj_b_lt1ms,   (unsigned long)g_dsd.dj_b_lt5ms,
           (unsigned long)g_dsd.dj_b_ge5ms);
}

/* Hooks called from txrx.c::usbwifi_bulk_in and usb.c::wifi_bulk_in
 * (completion ISR). Single-writer per call site, single-reader by shell cmd. */
void dsd_usb_direct_inc(void)   { g_dsd.usb_submit_direct++; }
void dsd_usb_queued_inc(uint32_t depth)
{
    g_dsd.usb_submit_queued++;
    if (depth > g_dsd.usb_pending_max) g_dsd.usb_pending_max = depth;
}
void dsd_usb_drained_inc(void)  { g_dsd.usb_submit_drained++; }
void dsd_usb_complete_inc(void) { g_dsd.usb_complete++; }

void dual_stack_diag_reset(void)
{
    memset((void *)&g_dsd, 0, sizeof(g_dsd));
}

static void zerocp_pbuf_free(void *arg)
{
    struct pbuf *p = arg;
    pbuf_free(p);
}

int ipro_dual_stack_peer_input(void *pkt, void *arg)
{
    struct usbwifi_rx_env_tag *env = &g_usbwifi.rx_env;
    struct pbuf *p = (struct pbuf *)pkt;
    uint32_t t_enter = inline_hal_machw_time();

    /* Guard standalone WiFi mode: when usbwifi shell cmd was never run,
     * g_usbwifi is BSS-zero so env->usbwifi == NULL. lwIP still calls this
     * hook on every "not for us" packet (udp.c / tcp_in.c / icmp.c), and
     * usbwifi_bulk_in -> usbwifi_get_port_state(NULL) faults at NULL+76.
     * Trigger: iperf -s exits, next frame hits the no-listener branch.
     * Return non-zero so caller pbuf_free's its extra ref. */
    if (!env->usbwifi) {
        g_dsd.peer_submit_fail++;
        return 1;
    }

    g_dsd.peer_calls++;

    /* Phase 2A.8 path delta.fix: zero-copy passthrough mirroring legacy
     * bl_iot_sdk. Earlier Phase 2A.5 staging-buffer copy added 108us per
     * call under iperf3 -b 50M load (Counter F bisect 2026-04-25), which
     * caps UDP RX at ~16 Mbps. usbwifi_bulk_in writes 40 bytes BEFORE the
     * payload as usb_data_emb_t header; for WiFi RX pbufs (PBUF_REF on
     * net_al inet_buf_rx_t), those bytes overlap the radiotap / MAC
     * header region of the WiFi RX buffer that has already been consumed
     * by upper layers and is safe to overwrite. Hold a pbuf ref so the
     * RX buffer stays valid until USB DMA completes; release via the
     * async free_fn callback (matches legacy pkt_free pattern). */
    uint16_t plen = p->len;
    pbuf_ref(p);
    uint32_t t_after_ref = inline_hal_machw_time();
    int ret = usbwifi_bulk_in(env, p->payload, plen,
                              USBWIFI_DATA_TYPE_PKT, zerocp_pbuf_free, p);
    uint32_t t_after_bulk = inline_hal_machw_time();
    if (ret) {
        pbuf_free(p);
        g_dsd.peer_submit_fail++;
        return ret;
    }

    pbuf_free(p);

    uint32_t dur = inline_hal_machw_time() - t_enter;
    uint32_t s_malloc = t_after_ref  - t_enter;     /* now: pbuf_ref cost */
    uint32_t s_flush  = 0;
    uint32_t s_copy   = 0;
    uint32_t s_bulk   = t_after_bulk - t_after_ref;
    g_dsd.seg_malloc_us_total += s_malloc;
    g_dsd.seg_flush_us_total  += s_flush;
    g_dsd.seg_copy_us_total   += s_copy;
    g_dsd.seg_bulk_us_total   += s_bulk;
    if (s_malloc > g_dsd.seg_malloc_us_max) g_dsd.seg_malloc_us_max = s_malloc;
    if (s_bulk   > g_dsd.seg_bulk_us_max  ) g_dsd.seg_bulk_us_max   = s_bulk;
    g_dsd.peer_submit_ok++;
    g_dsd.peer_bytes += plen;
    g_dsd.peer_dur_total_us += dur;
    if (dur > g_dsd.peer_dur_max_us)
        g_dsd.peer_dur_max_us = dur;
    if      (dur <    10) g_dsd.peer_dur_lt10us++;
    else if (dur <   100) g_dsd.peer_dur_lt100us++;
    else if (dur <  1000) g_dsd.peer_dur_lt1ms++;
    else                  g_dsd.peer_dur_ge1ms++;
    return 0;
}

static void *eth_input_hook(bool is_sta, void *pkt, void *arg)
{
    struct pbuf *p = (struct pbuf *)pkt;

    g_dsd.hook_total++;
    if (npf_is_8021X(p)) {
        g_dsd.hook_8021x++;
        return pkt;
    }

    bool input_emb = true;
    bool input_host = false;
    struct pbuf *p_dup = NULL;
    struct pbuf *ret = NULL;

#ifdef DHCP_IN_EMB
    if (npf_is_arp(p)) {
        /* ARP duplicated: emb keeps the WiFi association ARP cache warm,
         * host needs it so its iproeth0 ARP table resolves. */
        input_emb = true;
        input_host = true;
    } else if (npf_is_dhcp4(p)) {
        /* DHCP terminates in emb (the DUT lwIP runs the DHCP client). */
        input_emb = true;
        input_host = false;
    } else {
        /* Everything else is data traffic for the host's iproeth0 stack;
         * the DUT-side lwIP never holds matching sockets so handing the
         * pbuf to emb is a guaranteed drop (or worse, an RST that breaks
         * inbound TCP). Forward to host only. */
        input_emb = false;
        input_host = true;
    }
#else
    input_emb = false;
    input_host = true;
#endif

    // TODO Add custom filter here
    // Example of handling some TCP traffic in emb:
#if 0
    if (npf_is_tcp4_port(p, 50001) || npf_is_udp4_port(p, 50001)) {
        input_emb = true;
        input_host = false;
    }
#endif

    if (!input_emb && !input_host) {
        g_dsd.hook_dropped++;
        pbuf_free(p);
        return NULL;
    }

    if (input_emb && input_host) {
        g_dsd.hook_both++;
        p_dup = pbuf_alloc(PBUF_RAW, p->tot_len, PBUF_RAM);
        if (p_dup == NULL) {
            pbuf_free(p);
            return NULL;
        }
        pbuf_copy(p_dup, p);
    } else if (input_host) {
        g_dsd.hook_host_in++;
    } else {
        g_dsd.hook_emb_in++;
    }

    if (input_host) {
        // XXX distinguish STA/AP
        int ret = ipro_dual_stack_peer_input(p, NULL);
        if (ret) {
            if (p_dup) {
                pbuf_free(p_dup);
            }
            pbuf_free(p);
            return NULL;
        }
    }

    if (p_dup) {
        // both emb & host
        ret = p_dup;
    } else if (input_emb) {
        ret = p;
    } else {
        ret = NULL;
    }
    return ret;
}

static void tx_buf_register(struct usbwifi_tx_env_tag *env)
{
#if defined(IPRO6LE) || defined(IPRO6)
#ifdef LP_APP
    extern uint8_t __LD_CONFIG_EM_SEL;
    volatile uint32_t em_size;

    em_size = (uint32_t)&__LD_CONFIG_EM_SEL;
    if (em_size == 0) {
        usbwifi_tx_buf_mem_register(env, (void *)(0x23010000 + (160 - 32) * 1024), 32 * 1024);
    }
#endif

    extern uint8_t _heap_wifi_start;
    extern uint8_t _heap_wifi_size;
    usbwifi_tx_buf_mem_register(env, &_heap_wifi_start, (size_t)&_heap_wifi_size);
#endif
}

void usbwifi_register_cdc_cb(struct usbwifi_cdc_env_tag *env, usbwifi_cdc_data_cb_t in_cb, void *in_cb_arg, usbwifi_cdc_data_cb_t out_cb, void *out_cb_arg)
{
    env->bulk_in_cb = in_cb;
    env->bulk_in_cb_arg = in_cb_arg;
    env->bulk_out_cb = out_cb;
    env->bulk_out_cb_arg = out_cb_arg;
}

void usbwifi_configured()
{
    usbwifi_t *env = &g_usbwifi;

    usbwifi_set_port_state(env, USBWIFI_PORT_CONTROLLED);
}

void usbwifi_disconnected()
{
    usbwifi_t *uw = &g_usbwifi;
    struct usbwifi_rx_env_tag *rx = &uw->rx_env;

    usbwifi_set_port_state(uw, USBWIFI_PORT_CLOSED);

    const usb_data_emb_t *ud = rx->curr_attached;
    if (ud) {
        struct utils_list_hdr *hdr = (struct utils_list_hdr *)&ud->list_hdr;
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        rx->curr_attached = NULL;
        utils_list_push_back(&rx->pendding_free_list, hdr);
        while ((hdr = utils_list_pop_front(&rx->pendding_tx_list))) {
            utils_list_push_back(&rx->pendding_free_list, hdr);
        }
        vTaskNotifyGiveFromISR(uw->usb_worker_task, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

void usbwifi_set_port_state(usbwifi_t *ctx, uint8_t state)
{
    ctx->port_state = state;
}

uint8_t usbwifi_get_port_state(usbwifi_t *ctx)
{
    return ctx->port_state;
}

static void ready_cb(void *arg)
{
    usbwifi_t *uw = arg;

    usbwifi_set_port_state(uw, USBWIFI_PORT_OPEN);
#ifdef DHCP_IN_EMB
    rnms_notify_sta_ip_addr(uw->rnm);
#endif
}

int usbwifi_start(usbwifi_t *ctx)
{
    if (!ctx) {
        return -1;
    }

    memset(ctx, 0, sizeof(*ctx));
    usbwifi_tx_buf_init(&ctx->tx_env);
    tx_buf_register(&ctx->tx_env);
    usbwifi_rx_buf_init(&ctx->rx_env);
    ctx->rx_env.usbwifi = ctx;
    if ((ctx->rnm = rnm_server_init(&ctx->rx_env, rnm_tx_data, ctx, ready_cb)) == NULL) {
        return -1;
    }
    usbwifi_usb_init();
    wifi_pkt_eth_input_hook_register(eth_input_hook, &ctx->rx_env);
#if !defined(IPRO6)
    /* CDC-ACM data channel dropped on IPRO6 (no spare USB FIFO). */
    usbwifi_dc_init(&ctx->data_channel, &ctx->cdc_env);
    usbwifi_dc_test(&ctx->data_channel);
#endif
    return 0;
}

typedef void (*wifi_custom_tx_callback_t)(void *cb_arg, bool tx_ok);

struct wifi_custom_tx_cfm {
    wifi_custom_tx_callback_t cb;
    void *cb_arg;
};
int wifi_eth_tx(struct pbuf *p, bool is_sta, struct wifi_custom_tx_cfm *custom_cfm);

static void reply_get_mac(usbwifi_t *mgr, const void *input)
{
    rnm_mac_addr_ind_msg_t resp;
    rnms_build_get_mac_resp(mgr->rnm, input, &resp);
    rnm_tx_data(&mgr->rx_env, RNM_OUT_COMMAND, &resp, sizeof(resp));
}

int usbwifi_usb_data_process(void)
{
    usbwifi_t *mgr = &g_usbwifi;
    struct usbwifi_tx_env_tag *tx_env = &g_usbwifi.tx_env;
    struct usbwifi_rx_env_tag *rx_env = &g_usbwifi.rx_env;
    struct utils_list_hdr *node;

    while ((node = atomic_list_pop_front(&tx_env->ready_list))) {
        tx_buf *tb = (tx_buf *)node;
        usb_data_t *data = (usb_data_t *)TX_BUF_GET_USB_BUF(tb);
        struct pbuf *p;
        bool is_ap;

        switch (data->type) {
        case USBWIFI_DATA_TYPE_CMD:
            if (rnms_msg_is_get_mac(data->payload)) {
                reply_get_mac(mgr, data->payload);
            } else {
                rnms_cmd_input_raw(mgr->rnm, data->payload, data->length);
            }
            usbwifi_tx_buf_free(tb);
            break;
        case USBWIFI_DATA_TYPE_PKT:
            p = (struct pbuf *)&tb->pbuf;
            p->len = data->length;
            p->tot_len = data->length;
            is_ap = data->flags & USB_DATA_FLAG_AP_PKT;
            wifi_eth_tx(p, !is_ap, &(struct wifi_custom_tx_cfm){});
            pbuf_free(p);
            break;
        }
    }

    while ((node = atomic_list_pop_front(&rx_env->pendding_free_list))) {
        usb_data_emb_t *ud = container_of((uint32_t *)node, usb_data_emb_t, list_hdr);
        usbwifi_bulk_in_free_t free_fn = (usbwifi_bulk_in_free_t)ud->free_fn;
        void *free_arg = (void *)ud->free_arg;
        /* Counter J: read submit_us/complete_us BEFORE free_fn() because
         * the free path (zerocp_pbuf_free -> pbuf_free -> fhost_rx_buf_push)
         * may release the buffer carrying ud. */
        uint32_t s = ud->submit_us;
        uint32_t c = ud->complete_us;
        if (s != 0 && c != 0) {
            uint32_t now = inline_hal_machw_time();
            uint32_t a = c - s;
            uint32_t b = now - c;
            g_dsd.dj_cnt++;
            g_dsd.dj_a_sum_us += a;
            g_dsd.dj_b_sum_us += b;
            if (a > g_dsd.dj_a_max_us) g_dsd.dj_a_max_us = a;
            if (b > g_dsd.dj_b_max_us) g_dsd.dj_b_max_us = b;
            if      (a <   500) g_dsd.dj_a_lt500us++;
            else if (a <  2000) g_dsd.dj_a_lt2ms++;
            else if (a <  5000) g_dsd.dj_a_lt5ms++;
            else if (a < 10000) g_dsd.dj_a_lt10ms++;
            else                g_dsd.dj_a_ge10ms++;
            if      (b <   100) g_dsd.dj_b_lt100us++;
            else if (b <   500) g_dsd.dj_b_lt500us++;
            else if (b <  1000) g_dsd.dj_b_lt1ms++;
            else if (b <  5000) g_dsd.dj_b_lt5ms++;
            else                g_dsd.dj_b_ge5ms++;
        }
        free_fn(free_arg);
    }

    return 0;
}
