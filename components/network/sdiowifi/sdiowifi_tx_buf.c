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

#include "sdiowifi_tx_buf.h"

#include <stddef.h>
#include <utils_list.h>
#include <lwip/pbuf.h>
#include "sdiowifi_mgmr.h"
#include <platform_al.h>

#include "sdiowifi_platform_adapt.h"
#include "sdio_port.h"
#include "abtrace_dbg.h"    /* DEBUG: AB_TRACE timeline events */

#define PTR_SZ_MSK (sizeof(void *) - 1)
#define ALIGN_PTR_PADD(x) ((sizeof(void *) - ((uintptr_t)(x) & PTR_SZ_MSK)) & PTR_SZ_MSK)
#define ALIGN_PTR_HIGH(x) (((uintptr_t)(x) + PTR_SZ_MSK) & ~PTR_SZ_MSK)
#ifndef container_of
#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})
#endif

#define TX_PBUF_FRAME_LEN   1514
#define TX_PBUF_PAYLOAD_LEN (PBUF_LINK_ENCAPSULATION_HLEN + TX_PBUF_FRAME_LEN)

#if 0
typedef struct {
    struct utils_list_hdr list_hdr;
    struct pbuf_custom pbuf;
    struct sdiowifi_tx_buf *tb;
    uint32_t payload_buf[(TX_PBUF_PAYLOAD_LEN + 3) / 4];
} tx_pbuf;
#endif

typedef struct {
    struct utils_list_hdr list_hdr;
    struct pbuf_custom pbuf;
    struct sdiowifi_tx_buf *tb;
    uint32_t *payload_buf;//[(TX_PBUF_PAYLOAD_LEN + 3) / 4];
} tx_pbuf_t;

typedef struct {
    uint32_t  txpbuf_payload[(TX_PBUF_PAYLOAD_LEN + 3) / 4];
} sdio_txbuf_payload_t;

/* IPRO6LE: ram_wifi 160KB, lwIP heap+memp ~37KB, RX 16KB, TX = N * 1904B.
 * 11 was sized for legacy bl_iot_sdk LP_FW layout (38KB - 16KB RX); 16 was
 * the largest that fit ram_wifi keeping the original lwIP heap. To get TCP
 * TX above the 16-slot ceiling (~10 Mbps observed) we trade lwIP heap for
 * more in-flight TX slots. 32 slots ~60KB pulls TX BDP up to ~25 Mbps to
 * match the USB transport (40 URBs). */
/* DEBUG F2/F2b experiments 2026-07-23 CLOSED, both NULL: pool 24->32 changes
 * nothing (the earlier "pool-32 regression" was a LAB03 per-MAC-throttle
 * confound, retested clean on LAB02: cap24 vs cap32 identical 8.4-9.0M);
 * hwq_credits staging cap (F2b) also null; A-MPDU depth never exceeds 16 at
 * ANY pool/credits/BAW setting because depth = frames producible per 8ms
 * cycle (SW-phase production limit). Kept at 24 for ram_wifi margin. */
/* THE DEPTH CLAIM ABOVE IS NO LONGER TRUE and must not be cited. It was
 * concluded at ~9 Mbps uplink, before the hot code moved to OCRAM. Production
 * rate has since roughly doubled, and the 2026-07-29 measurement of in-flight
 * zero-copy slot depth (g_zc_depth in trcver_sdio_ipro6.c) reads high-water 23
 * with depth > 16 on 37.5% of claims. The rest of this note - both F2 results
 * being null - still stands. */
/* Phase-Z v2a 2026-07-24: 24 -> 6. Zero-copy SMID slots (trcver, 24x2KB in
 * ram_wifi) are now the primary TX buffers; this pool remains only as the
 * bounce/pressure fallback for frames that cannot be zero-copy-claimed
 * (multi-frame aggregates, zc-pool pressure, gate off). The freed 35.4KB of
 * ram_wifi funds the zc slots. */
#define SDIO_TX_NORMAL_BUFFER_COUNT (6)

static tx_pbuf_t            g_txpbuf_context[SDIO_TX_NORMAL_BUFFER_COUNT];

#if defined(IPRO6LE) || defined(IPRO6)
/* IPRO6LE/IPRO6: place TX payload storage in .lwip_mem.sdiowifi (linker rule
 * `*(.lwip_mem*)` catches it into ram_wifi after lwIP heap + memp pools).
 * Original hardcoded TXBUF_PAYLOAD_START_ADDR=0x2302A800 was for bl_iot_sdk's
 * LP_FW layout (LP_FW_START_ADDR + 16KB RX). In ipro6le_demo that address
 * sits inside lwIP memp pools (memp_memory_TCP_SEG_base 0x230282CC ..
 * memp_memory_RAW_PCB_base 0x2302B210), so SDIO TX writes corrupt the
 * UDP/TCP PCB free lists. After DHCP PCB struct (in MEMP_UDP_PCB pool)
 * gets its `next` field overwritten by frame data, udp_input faults on
 * the dangling pcb->next dereference.
 * On IPRO6 the hardcoded 0x2302A800 is even worse: it is OUTSIDE the SHRAM
 * window [_sshram, _eshram] = ram_wifi (0x21050000..0x21078000), so
 * net_buf_tx_all_shram() fails and net_al_tx_req() drops every host->WiFi TX
 * frame with -1 (FHOST_BUFFER_COPY is not built) -- the iproeth0 ping never
 * reaches the air. The .lwip_mem.sdiowifi placement lands in ram_wifi/SHRAM so
 * the injected frame takes the direct fhost_tx_req_do path.
 * NOTE 2026-05-07: ram_nocache (0x22FC0000+64KB) was tried for >16 slots
 * but SDU DMA cannot reach it (only ram_wifi has SDIO_MAWOC alias
 * 0x23xxxxxx → 0x22xxxxxx). Stay in ram_wifi; for >16 slots, shrink
 * lwIP heap (mem.c) or memp pools (memp.c) to free ram_wifi. */
static __attribute__((aligned(32), section(".lwip_mem.sdiowifi")))
    sdio_txbuf_payload_t g_txpbuf_payload_normal[SDIO_TX_NORMAL_BUFFER_COUNT];
#else
#define TXBUF_PAYLOAD_START_ADDR    (0x2302A800)
static sdio_txbuf_payload_t *g_txpbuf_payload_normal = (sdio_txbuf_payload_t *)ALIGN_PTR_HIGH(TXBUF_PAYLOAD_START_ADDR);
#endif

struct sdiowifi_tx_buf *s_tb_printf = NULL;

void sdiowifi_txbuf_stats_display(void)
{
    struct sdiowifi_tx_buf *tb = s_tb_printf;

    if (tb) {
        HR_LOGI("current:%ld, una_seq:%ld-%d, free_size:%ld, window:%ld\r\n",
            (tb->una_seq + tb->window_size),
            tb->una_seq, tb->una_seq%256,
            tb->free_size,
            tb->window_size);
    }
}

int sdiowifi_tx_buf_init(struct sdiowifi_tx_buf *tb)
{
    if (!tb) {
        return -1;
    }

    s_tb_printf = tb;
    sdiowifi_enter_critical();
    utils_list_init(&tb->buf_list);
    tb->una_seq = 0;
    tb->free_size = 0;
    tb->pool_empty = 0;
    sdiowifi_exit_critical(0);

    return 0;
}

void sdiowifi_tx_buf_update_credit(struct sdiowifi_tx_buf *tb)
{
    HR_LOGD("update:%ld, una_seq:%ld-%d, free_size:%ld, window:%ld\r\n",
        (tb->una_seq + tb->window_size),
        tb->una_seq, tb->una_seq%256,
        tb->free_size,
        tb->window_size);
    sdiowifi_update_tx_credit(tb);
}

void sdiowifi_tx_buf_init_done(struct sdiowifi_tx_buf *tb)
{
}

int sdiowifi_tx_buf_lpmem_register(struct sdiowifi_tx_buf *tb)
{
    if (!tb) {
        return -1;
    }

    // register lpmem
    sdiowifi_enter_critical();
    // context.payload point payload
    for (int i = 0; i < SDIO_TX_NORMAL_BUFFER_COUNT; i++) {
        g_txpbuf_context[i].payload_buf = (uint32_t *)&(g_txpbuf_payload_normal[i]);
        g_txpbuf_context[i].tb = tb;
        // add
        utils_list_push_back(&tb->buf_list, (struct utils_list_hdr *)(&(g_txpbuf_context[i])));
        // ++free_size
        ++tb->free_size;
    }

    // update window_size
    tb->window_size = tb->free_size;
    sdiowifi_exit_critical(0);

    HR_LOGI("lpmem_register tx slot:%ld,unseq:%ld-%d, free:%ld, window:%ld\r\n",
            (tb->una_seq + tb->window_size),
            tb->una_seq, tb->una_seq%256,
            tb->free_size,
            tb->window_size);

    sdiowifi_tx_buf_update_credit(tb);

    return 0;
}

int sdiowifi_tx_buf_mem_register(struct sdiowifi_tx_buf *tb, void *mem, size_t size)
{
    if (!tb) {
        return -1;
    }

    if (!mem) {
        return 0;
    }
    static tx_pbuf_t *tx_buffer_ctx = NULL;
    long s = size;
    uint8_t *p = (uint8_t *)ALIGN_PTR_HIGH(mem);
    s -= ALIGN_PTR_PADD(mem);
    sdiowifi_enter_critical();
    
    uint32_t tx_count = s / sizeof(sdio_txbuf_payload_t);
    if (0 == tx_count) {
        HR_LOGI("no enough mem for high performance\r\n");
        return -1;
    }
    tx_buffer_ctx = pvPortMalloc(tx_count * sizeof(tx_pbuf_t));
    if (!tx_buffer_ctx) {
        return -1;
    }
    memset(tx_buffer_ctx, 0, tx_count * sizeof(tx_pbuf_t));
    for (int i = 0; i < tx_count; i++) {
        tx_buffer_ctx[i].payload_buf = (uint32_t *)(p + sizeof(sdio_txbuf_payload_t) * i);
        tx_buffer_ctx[i].tb = tb;
        utils_list_push_back(&tb->buf_list, (struct utils_list_hdr *)(&(tx_buffer_ctx[i])));
        // ++free_size
        ++tb->free_size;
    }
    
    // update window size.
    tb->window_size = tb->free_size;

    sdiowifi_exit_critical(0);

    HR_LOGI("mem_register:%p-%u, slot:%ld,unseq:%ld-%d, free:%ld, window:%ld\r\n",
            mem, size,
            (tb->una_seq + tb->window_size),
            tb->una_seq, tb->una_seq%256,
            tb->free_size,
            tb->window_size);

    sdiowifi_tx_buf_update_credit(tb);

    return 0;
}

static inline void notify_tx_buf_status(struct sdiowifi_tx_buf *tb)
{
#ifdef SDIO_REUSE_LP_RAM
    /* IPRO6LE: ipro_sdk's enum PSM_EVENT (platform_al.h) lacks LP_BUF_REUSED.
     * Don't gate WiFi LP sleep on SDIO TX buf state; SDIO bridge isn't LP-aware. */
    (void)tb;
#endif
}

static void tx_buf_free(struct pbuf *p)
{
    tx_pbuf_t *tp = container_of((struct pbuf_custom *)p, tx_pbuf_t, pbuf);
    struct sdiowifi_tx_buf *tb = tp->tb;

    sdiowifi_enter_critical();
    {
        utils_list_push_back(&tb->buf_list, &tp->list_hdr);
        ++tb->free_size;
        ++tb->una_seq;
        tb->pool_empty = 0;
        sdiowifi_tx_buf_update_credit(tb);
        ABDBG_TRACE(0x54, (uint16_t)tb->free_size); /* L5: pool slot freed */
    }
    sdiowifi_exit_critical(0);
    //sdm_log("%s %u\r\n", __func__, tb->free_size);

    notify_tx_buf_status(tb);
}

/* DEBUG 2026-07-23 ZC-probe: stable slot index for a pool pbuf (0..COUNT-1),
 * -1 if the pbuf is not from the static pool. The index is the natural key
 * for per-slot staging areas whose lifetime must equal the slot's. */
int sdiowifi_tx_buf_slot(struct pbuf *p)
{
    tx_pbuf_t *tp = container_of((struct pbuf_custom *)p, tx_pbuf_t, pbuf);
    int idx = (int)(tp - g_txpbuf_context);
    return (idx >= 0 && idx < SDIO_TX_NORMAL_BUFFER_COUNT) ? idx : -1;
}

struct pbuf *sdiowifi_tx_buf_alloc(struct sdiowifi_tx_buf *tb)
{
    tx_pbuf_t *tp;
    struct pbuf *p = NULL;

    sdiowifi_enter_critical();
    {
        tp = (tx_pbuf_t *)utils_list_pop_front(&tb->buf_list);
        if (tp) {
            --tb->free_size;
            /* PM35: propagate alloc-side decrement to host. Without this the
             * host's refill reads a stale free_size from the last tx_buf_free
             * and over-pushes; pairs with the count-based credit in
             * sdiowifi_update_tx_credit. */
            sdiowifi_tx_buf_update_credit(tb);
        } else {
            tb->pool_empty = 1;   /* genuine exhaustion, decided under lock */
        }
    }
    sdiowifi_exit_critical(0);

    if (tp) {
        tp->pbuf.custom_free_function = tx_buf_free;
#if 0
        p = pbuf_alloced_custom(PBUF_RAW_TX, TX_PBUF_FRAME_LEN, PBUF_RAM, &tp->pbuf, tp->payload_buf, TX_PBUF_PAYLOAD_LEN);
#else
        p = pbuf_alloced_custom(PBUF_RAW_TX, TX_PBUF_FRAME_LEN, 
                (PBUF_ALLOC_FLAG_DATA_CONTIGUOUS | PBUF_TYPE_ALLOC_SRC_MASK_STD_HEAP), 
                &tp->pbuf, tp->payload_buf, TX_PBUF_PAYLOAD_LEN);
#endif

    }
    //sdm_log("%s %u\r\n", __func__, tb->free_size);

    notify_tx_buf_status(tb);
    return p;
}

bool sdiowifi_tx_buf_is_all_free(struct sdiowifi_tx_buf *tb)
{
    sdiowifi_enter_critical();
    bool free = tb->window_size == tb->free_size;
    sdiowifi_exit_critical(0);
    return free;
}

