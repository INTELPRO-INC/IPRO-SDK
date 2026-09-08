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

#include <sdiowifi_securec_wrap.h>
#include <stdbool.h>
#include <string.h>
#include <sdiowifi_platform_adapt.h>
#include <lwip/netif.h>
//#include <bl_wifi.h>
#include <wifi_mgmr_ext.h>
//#include <vfs.h>

#include <rnm_server.h>

#include <sdiowifi_config.h>
#include <sdiowifi_mgmr.h>
#include <sdiowifi_mgmr_type.h>
#include <sdiowifi_tty.h>

#include "sdio_port.h"

#if 0
#if defined(SDIO_REUSE_LP_RAM) && !(defined(CFG_IPRO_WIFI) && defined(LP_APP))
#error "SDIO lp ram reuse configuration error"
#endif
#endif

#define SDU_WORKER_EV_RESET (1 << 0)

void notify_sdu_worker(int event, int isr);
void sdu_software_reset_set(int enable);

sdiowifi_mgmr_t g_sdiowifi_mgmr;

/* DEBUG (default off): gate the SMID-dead credit scratch writes in
 * sdiowifi_update_tx_credit. The SMID host cannot read this s_reg scratch
 * window (offset 0 / hi / una_seq), so those 3 ipro_write_s_reg calls are
 * proven dead I/O (~12us/call). Default 0 = skip the writes entirely. Poke
 * g_sdio_credit_scratch_en = 1 via memory to restore the legacy ipro6le
 * scratch-credit path (non-SMID host that can read the window). */
volatile uint8_t g_sdio_credit_scratch_en = 0;

/* PM26 vector 1: SDIO credit throttle on sustained 0% MPDU.
 *
 * Under 100% MPDU TX failure (e.g. RF cliff like LAB02 ch10), MAC TX makes
 * forward progress only via AC_TMO recovery at ~10-20 MPDU/sec, while the
 * host bridge (RATELV10L ipronet) keeps pushing at line rate. Without a
 * back-pressure signal the sdiowifi_tx_buf pool fills, sdio_pool_drops
 * accumulate, RATELV10L queues block, and the bench wedges silently with
 * no clean recovery (only a reset clears it).
 *
 * This throttle samples (tx_mpdu_total, tx_mpdu_ok) at each credit update.
 * Credit update fires from tx_buf_free which runs once per host TX frame
 * BL616 finishes — naturally rate-paced with actual MAC TX progress.
 *
 * After PM26_BAD_STREAK_LIMIT consecutive samples with >= MIN_ATTEMPTS
 * MPDUs attempted and zero OK, drop the reported window to PM26_THROTTLED.
 * Any sample with d_ok > 0 immediately releases the throttle. The reduced
 * window makes the host back off and stops further SDIO TX from piling up.
 */
extern struct fhost_statistic fhost_statistic;

#define PM26_MIN_ATTEMPTS_PER_SAMPLE 16u
#define PM26_BAD_STREAK_LIMIT         3u
#define PM26_THROTTLED_WINDOW         4u

static uint32_t pm26_last_total = 0;
static uint32_t pm26_last_ok    = 0;
static uint32_t pm26_bad_streak = 0;
static uint32_t pm26_active     = 0;

/* Observable counters (read from tx_diag for verification). */
volatile uint32_t pm26_throttle_engaged_cnt  = 0;
volatile uint32_t pm26_throttle_released_cnt = 0;
volatile uint32_t pm26_throttle_active       = 0;

static uint32_t pm26_effective_window(uint32_t window_size)
{
    uint32_t cur_total = fhost_statistic.tx_mpdu_total;
    uint32_t cur_ok    = fhost_statistic.tx_mpdu_ok;
    uint32_t d_total   = cur_total - pm26_last_total; /* unsigned wrap-safe */
    uint32_t d_ok      = cur_ok    - pm26_last_ok;
    pm26_last_total = cur_total;
    pm26_last_ok    = cur_ok;

    if (d_ok > 0) {
        if (pm26_active) {
            pm26_throttle_released_cnt++;
            pm26_active = 0;
            pm26_throttle_active = 0;
        }
        pm26_bad_streak = 0;
    } else if (d_total >= PM26_MIN_ATTEMPTS_PER_SAMPLE) {
        if (pm26_bad_streak < UINT32_MAX) pm26_bad_streak++;
        if (pm26_bad_streak >= PM26_BAD_STREAK_LIMIT && !pm26_active) {
            pm26_throttle_engaged_cnt++;
            pm26_active = 1;
            pm26_throttle_active = 1;
        }
    }
    /* d_total < MIN: too few samples this slice; preserve state */

    if (pm26_active && window_size > PM26_THROTTLED_WINDOW)
        return PM26_THROTTLED_WINDOW;
    return window_size;
}

int sdiowifi_update_tx_credit(struct sdiowifi_tx_buf *tb)
{
    /* DEBUG (default off): skip the dead SMID scratch I/O below. See the
     * g_sdio_credit_scratch_en comment near the top of this file. Return
     * before pm26_effective_window / any ipro_write_s_reg so the whole
     * dead-I/O path costs nothing on SMID. */
    if (!g_sdio_credit_scratch_en)
        return 0;

    /* PM35: advertise free pool count, not una_seq+window. Host (ipronet
     * ipro_eth_tx_data_process) treats this uint16 as a *count* of free TX
     * pbuf slots: refills its local tx_credit from the scratch reg when local
     * drops to TX_CREDIT_THRESHOLD (3), decrements locally per push. Old
     * formula (una_seq + window) grew monotonically, so every refill read a
     * value far above actual free slots and the host kept over-pushing 20-28
     * frames per cycle. LAB02 48% MPDU rate: 61k drops / 2.2k accepted (~28x).
     * Clamp to min(window, free_size) so PM26 still caps the cliff case while
     * free_size is the truth in normal operation. */
    uint32_t window  = pm26_effective_window(tb->window_size);
    uint32_t cap     = (window < tb->free_size) ? window : tb->free_size;
    uint16_t credit  = (uint16_t)cap;

#if defined(IPRO6)
    /* SMID has no directly-mapped scratch region; route una_seq through the
     * custom-register scratch window (offset 7, matching the ipro6le layout
     * SDU_BASE+0x167). */
    ipro_write_s_reg(&g_sdiowifi_mgmr.trcver, UNA_SEQ_SCRATCH_OFFSET, (uint8_t)tb->una_seq);
#else
    *(volatile unsigned char *)(0x2000d167) = (unsigned char)tb->una_seq;
#endif
    /* Hi-first lo-second: torn read can only overshoot, never undershoot into
     * the [0..TX_CREDIT_THRESHOLD] poll-block window. With cap <= window_size
     * (24) the hi byte is always 0; ordering remains correct for future caps. */
    ipro_write_s_reg(&g_sdiowifi_mgmr.trcver, CREDIT_HI_SCRATCH_OFFSET, (uint8_t)(credit >> 8));
    ipro_write_s_reg(&g_sdiowifi_mgmr.trcver, 0,                        (uint8_t)credit);

    return 0;
}

static int rnm_tx_data(void *env, rnm_output_data_type_t type, const void *data, uint16_t data_length)
{
    int ret = RNM_OUTPUT_FN_SUCCESS;
    int tx_ret = 0;
    net_wifi_trcver_t *trcver = (net_wifi_trcver_t *)env;

    if (type == RNM_OUT_COMMAND) {
        tx_ret = ipro_msg_send(trcver, IPRO_MSG_TYPE_CMD, data, data_length);
    } else if (type == RNM_OUT_SNIFFER_FRAME) {
        // XXX not implemented
    }
    if (tx_ret == IPRO_MSG_ERR_DESC_USED) {
        HR_LOGW("IPRO_MSG_ERR_DESC_USED -> sdiowifi_mgmr_reinit -> SDU_WORKER_EV_RESET\r\n");
        sdiowifi_mgmr_reinit(false);
        ret = RNM_OUTPUT_FN_ERROR_OTHER;
    } else if (tx_ret == IPRO_MSG_ERR_TIMEOUT) {
        ret = RNM_OUTPUT_FN_TIMEOUT;
    } else if (tx_ret != 0) {
        ret = RNM_OUTPUT_FN_ERROR_OTHER;
    }
    return ret;
}

#ifdef SDIOWIFI_HEARTBEAT
static int heartbeat_failure(void *arg)
{
    sdiowifi_mgmr_t *sdm = (sdiowifi_mgmr_t *)arg;

    (void)sdm;

    HR_LOGW("heartbeat fail\r\n");
    sdiowifi_mgmr_reinit(false);
    return 0;
}
#endif

static void ready_cb(void *arg)
{
    sdiowifi_mgmr_t *sdm = (sdiowifi_mgmr_t *)arg;
    HR_LOGI("device ready\r\n");

    ipro_net_wifi_trcver_set_present(&sdm->trcver, true);

    sdiowifi_update_tx_credit(&sdm->tx_desc);
#ifdef DHCP_IN_EMB
    rnms_notify_sta_ip_addr(sdm->rnm);
#endif

#ifdef SDIOWIFI_HEARTBEAT
    rnms_heartbeat_register_failure_cb(sdm->rnm, heartbeat_failure, sdm);
    rnms_heartbeat_start(sdm->rnm, 200, 2);
#endif

#if SDIOWIFI_TTY
    sdiowifi_tty_send_bsr(sdm, 0, true, 0);
#endif
}

static void tx_buf_init(sdiowifi_mgmr_t *sdm)
{
    sdiowifi_tx_buf_init(&sdm->tx_desc);

    sdiowifi_tx_buf_lpmem_register(&sdm->tx_desc);
    
#if CONFIG_SDIO_HIGH_PERFORMANCE
    extern uint8_t _heap_wifi_start;
    extern uint8_t _heap_wifi_size;
    sdiowifi_tx_buf_mem_register(&sdm->tx_desc, &_heap_wifi_start, (size_t)&_heap_wifi_size);
#endif
#if 0
#ifdef CFG_IPRO_WIFI
#ifdef LP_APP
#ifdef SDIO_REUSE_LP_RAM
    sdiowifi_tx_buf_mem_register(&sdm->tx_desc, (void *)SW_TX_BUF_START, SW_TX_BUF_SIZE);
#else
    extern uint8_t __LD_CONFIG_EM_SEL;
    volatile uint32_t em_size;

    em_size = (uint32_t)&__LD_CONFIG_EM_SEL;
    if (em_size == 0) {
        sdiowifi_tx_buf_mem_register(&sdm->tx_desc, (void *)(0x23010000 + (160 - 32) * 1024), 32 * 1024);
    }
#endif // SDIO_REUSE_LP_RAM
#endif // LP_APP

#ifndef BL_SDIO_BUFF_REDUCTION
    extern uint8_t _heap_wifi_start;
    extern uint8_t _heap_wifi_size;
    sdiowifi_tx_buf_mem_register(&sdm->tx_desc, &_heap_wifi_start, (size_t)&_heap_wifi_size);
#endif

#else
    static uint32_t tx_buf[22 * 1024 / 4] __attribute__((section(".wifi_ram")));
    sdiowifi_tx_buf_mem_register(&sdm->tx_desc, tx_buf, sizeof(tx_buf));
#endif
#endif
    sdiowifi_tx_buf_init_done(&sdm->tx_desc);
}

int sdiowifi_mgmr_start(sdiowifi_mgmr_t *sdm)
{
    if (!sdm || sdm->init) {
        return -1;
    }
    MEMSET_SAFE(sdm,  sizeof(*sdm),  0,  sizeof(*sdm));
    if ((sdm->rnm = rnm_server_init(&sdm->trcver, rnm_tx_data, sdm, ready_cb)) == NULL) {
        return -1;
    }
    ipro_net_wifi_trcver_init(&sdm->trcver, sdm);
    tx_buf_init(sdm);

#if SDIOWIFI_TTY
    sdiowifi_tty_init(sdm, 0, SDIOWIFI_TTY_PATH);
#endif

    sdm->init = true;

    return 0;
}

int sdiowifi_mgmr_reinit(bool init_tx_buf)
{
    (void)init_tx_buf;

    sdiowifi_mgmr_t *sdm = &g_sdiowifi_mgmr;
    if (!sdm->init) {
        HR_LOGW("%s(%d): sdm is not initialized\r\n", __func__, __LINE__);
        return -1;
    }
    sdu_software_reset_set(true);
    notify_sdu_worker(SDU_WORKER_EV_RESET, 0);
    return 0;
}

int sdiowifi_tx_buf_init_after_lp(void)
{
    sdiowifi_mgmr_t *sdm = &g_sdiowifi_mgmr;
    if (!sdm->init) {
        HR_LOGW("%s(%d): sdm is not initialized\r\n", __func__, __LINE__);
        return -1;
    }

    tx_buf_init(sdm);
    return 0;
}

int sdiowifi_mgmr_ps_prepare(void)
{
    sdiowifi_mgmr_t *sdm = &g_sdiowifi_mgmr;

    ipro_net_wifi_trcver_set_present(&sdm->trcver, false);
    ipro_sdio_tx_timer_stop();

#ifdef SDIOWIFI_HEARTBEAT
    rnms_heartbeat_stop(sdm->rnm);
#endif
    return 0;
}
