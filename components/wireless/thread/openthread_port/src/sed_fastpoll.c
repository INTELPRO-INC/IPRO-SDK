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

/**
 * @file sed_fastpoll.c
 * @brief OT-bypass secured Data Request TX for SED — implementation.
 *
 * This file is intentionally opaque at the API boundary: the header
 * exposes only function entry points; frame layout, crypto wiring, and
 * busy-wait policy live here and are subject to change without notice.
 *
 * Module is a singleton — there is at most one SED instance per device
 * and the state is hidden in `s_fp` below.
 */

#include "sed_fastpoll.h"
#include "lp_sleep.h"
#include "ot_fastpoll_shim.h"

#include <generated/autoconf.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "lp_uart.h"

#include "FreeRTOS.h"
#include "task.h"

#include "mbedtls/aes.h"

#include "hal_mtimer.h"
#include "ieee802154.h"
#include "ieee802154_hal.h"
#include "ieee802154_port.h"
#include "ipro_log.h"
#include "openthread_port.h"

#define LOG_TAG "SED_FP"

/* ---- Frame layout (private) ------------------------------------------ */

/*
 *   off  size  field
 *   0    2     FCF                       (0x986B little-endian, ver 2006)
 *   2    1     seq num
 *   3    2     dst PAN ID                (LE)
 *   5    2     dst short addr / parent   (LE)
 *   7    2     src short addr / us       (LE)
 *   9    1     sec ctl                   (0x0D = ENC_MIC_32 + KeyIdMode1)
 *   10   4     frame counter             (LE)
 *   14   1     key id                    ((keySeq & 0x7f) + 1)
 *   15   1     MAC Cmd ID                (0x04 Data Request — PLAINTEXT, in
 * AAD) 16   4     MIC-32
 *   ---  --    end of MPDU (length passed to lmac154 = 20)
 *   20   2     FCS                       (HW fills, appended after MPDU)
 *
 * Important — Cmd ID is authenticated but NOT encrypted.
 *   For 802.15.4-2003/2006 MAC Command frames, the Command Frame
 *   Identifier byte is part of the MHR (AAD), not the secured payload.
 *   OT mirrors this in `Frame::FindPayloadIndex()` by advancing past
 *   `kCommandIdSize` when `!IsVersion2015() && type == kTypeMacCmd`, so
 *   the receiver's `GetHeaderLength()` returns 16 (not 15) and
 *   `GetPayloadLength()` returns 0 for Data Request.
 *
 *   AAD therefore covers bytes 0..15 (16 bytes), there is nothing to
 *   encrypt, and the MIC at 16..19 authenticates the full 16-byte AAD.
 *
 *   Earlier draft had AAD=15 + ciphertext at offset 15: FTD's receive
 *   path computed AAD over 16 bytes (per spec) and read MIC at the
 *   same 16..19 window, so the MIC never matched and every poll
 *   logged `Frame rx failed, error:Security, type:Cmd(<random>)`.
 *   Fixed 2026-05-19 after instrumenting OT's poll dump.
 *
 * Note on length: `lmac154_triggerTxDelay` expects MPDU bytes WITHOUT FCS.
 * The HW appends its own 2-byte FCS after the bytes we hand it. OT's
 * adapter (`ot_radio_ieee802154.c` ~L587) subtracts 2 from
 * `aFrame->mLength` for exactly this reason — we mirror that.
 */
#define FP_MPDU_LEN 20
#define FP_PSDU_BUF_LEN 22 /* MPDU + 2 FCS placeholder */
#define FP_MAC_HDR_LEN 9
#define FP_AUX_SEC_LEN 6
#define FP_CMD_ID_LEN 1
/* AAD covers MHR + Aux Sec Hdr + Cmd ID byte (16 bytes for our Data Req). */
#define FP_AAD_LEN (FP_MAC_HDR_LEN + FP_AUX_SEC_LEN + FP_CMD_ID_LEN)
/* Data Request has no Cmd payload → nothing to encrypt; MIC only. */
#define FP_PAYLOAD_LEN 0
#define FP_MIC_LEN 4

#define FP_OFF_SEQ 2
#define FP_OFF_FCTR 10
#define FP_OFF_KEYID 14
#define FP_OFF_CMD_ID 15
#define FP_OFF_MIC 16

#define FP_FCF 0x986Bu
#define FP_SEC_CTL 0x0Du
#define FP_SEC_LEVEL 5u
#define FP_CMD_DATA_REQ 0x04u

/* ---- Singleton state (private) --------------------------------------- */

typedef struct {
    uint8_t psdu[FP_PSDU_BUF_LEN];
    uint8_t seq_num;
    uint32_t frame_counter;
    uint8_t our_ext[8]; /* MSB-first (OT m8 order) — CCM nonce convention */
    uint8_t mac_key[16];
    uint32_t key_sequence;
    uint8_t key_id;
    uint8_t channel;
    uint16_t pan_id;
    uint16_t our_short;
    uint16_t parent_short;
    bool ready;
} fp_state_t;

static fp_state_t s_fp;

/* Cached AES context — pinned to libopenthread_port.a .bss → ocram_high
 * retention. Replaces mbedtls_ccm_context: that struct has an inner
 * cipher_ctx pointer malloc'd from the FreeRTOS heap (ocram_noret,
 * wiped on PDS), so it can't survive PDS31 across cycles. With HW AES
 * the context is just key + keybits (~36 B); we can re-setkey only
 * when the OT MAC key actually changes and run CCM MIC inline using
 * mbedtls_aes_crypt_ecb. */
static mbedtls_aes_context s_aes;
static uint8_t s_cached_key[16];
static bool    s_aes_keyed;

/* ISR observer state — written from ISR, read by busy-wait loop. */
typedef struct {
    volatile bool fired;
    volatile bool ack_fp;
    volatile ieee802154_event_type_t event;
} fp_obs_t;
static fp_obs_t s_obs;

/* Post-PDS direct-TX handoff. Producer: sed_fastpoll_post_pds_tx running
 * on the retention stack post-wake. Consumer: sed_fastpoll_tick running
 * on the OT task once the scheduler resumes. Replaces this cycle's
 * normal TX path so the radio fires ~1-2 ms earlier (no scheduler /
 * alarm-callback overhead between wake and TX). */
static volatile bool s_post_tx_done;
static volatile sed_fastpoll_tx_result_t s_post_tx_result;
static volatile uint32_t s_post_tx_used_counter;

/* Early-close signal for the FP=YES RX window. `s_rx_window_open` is
 * set by tick() the moment we call ieee802154_hal_rx_start() and
 * cleared on close. The observer checks it on every RX event and, if
 * a frame arrives, sets `s_rx_window_close_pending`. tick() consumes
 * the pending flag and stops RX immediately instead of waiting for
 * the full FP_RX_AFTER_YES_MS timeout (typical parent indirect frame
 * arrives within 10-20 ms, vs the 150 ms safety budget).
 *
 * Early-close can be toggled at runtime (sed_fastpoll_set_rx_early_close
 * or via the `pds rx_early on/off` CLI). Default ON. */
static volatile bool s_rx_window_open;
static volatile bool s_rx_window_close_pending;
static volatile bool s_rx_early_close_enabled = true;

/* GPIO 28 marker for the RX window — pulse width on a logic analyser
 * tells us "actual indirect frame arrival latency vs the 150 ms
 * timeout budget". HIGH when ieee802154_hal_rx_start is called,
 * LOW the moment tick() (or observer-triggered early close) stops
 * RX. Raw MMIO so the observer ISR can toggle without function-call
 * cost. CFGCTL33 @ +0x188 carries GPIO 0-31 output values; OE for
 * pin 28 lives in CFGCTL35 @ +0x190. Pin set to OUTPUT once at
 * sed_fastpoll_enable; subsequent writes are bit-set/clear only.
 *
 * Set FP_DBG_GPIO_RX_WINDOW to 0 to compile this out for production. */
/* MUST stay 0 on boards with external SF3 flash: GPIO 28 is a flash
 * data/clock pin there (the same GPIO 28-33 range that needs
 * CONFIG_DISABLE_SDC_PINMUX). gpio_set_mode(28,...) + driving it
 * corrupts an in-flight XIP fetch → hard fault. Debug-only marker. */
#define FP_DBG_GPIO_RX_WINDOW 0
#if FP_DBG_GPIO_RX_WINDOW
#define FP_DBG_RX_HIGH() (*(volatile uint32_t *)0x30000188 |=  (1u << 28))
#define FP_DBG_RX_LOW()  (*(volatile uint32_t *)0x30000188 &= ~(1u << 28))
#define FP_DBG_RX_OE()   (*(volatile uint32_t *)0x30000190 |=  (1u << 28))
#else
#define FP_DBG_RX_HIGH() ((void)0)
#define FP_DBG_RX_LOW()  ((void)0)
#define FP_DBG_RX_OE()   ((void)0)
#endif

/* GPIO 31 marker for engine-side profiling. Sites:
 *   HIGH at post_pds_tx entry, LOW at post_pds_tx exit
 *      → first pulse width = "engine TX cost" (~4 ms)
 *   HIGH again at sed_fastpoll_tick consume-path entry,
 *   LOW at consume-path exit
 *      → gap from first LOW to second HIGH = "OT task wake latency"
 *      → second pulse width = "tick consume cost" (commit + autostats)
 * Pin must be OUTPUT-configured at engine enable; OE re-asserted in
 * lp_wakeup_hook on every PDS wake. */
/* MUST stay 0 on boards with external SF3 flash: GPIO 31 is a flash
 * pin there. Same XIP-corruption hazard as FP_DBG_GPIO_RX_WINDOW. */
#define FP_DBG_GPIO_TICK 0
#if FP_DBG_GPIO_TICK
#define FP_DBG_TICK_HIGH() (*(volatile uint32_t *)0x30000188 |=  (1u << 31))
#define FP_DBG_TICK_LOW()  (*(volatile uint32_t *)0x30000188 &= ~(1u << 31))
#define FP_DBG_TICK_OE()   (*(volatile uint32_t *)0x30000190 |=  (1u << 31))
#else
#define FP_DBG_TICK_HIGH() ((void)0)
#define FP_DBG_TICK_LOW()  ((void)0)
#define FP_DBG_TICK_OE()   ((void)0)
#endif

/* Telemetry — how often the post-PDS path is entered vs how often it
 * actually submits a TX. Helps diagnose "is post_pds_tx firing?" without
 * UART noise on the retention stack. Read via sed_fastpoll_log_debug
 * or the lp_stats shell command. */
static volatile uint32_t s_post_entries;
static volatile uint32_t s_post_bail_unready;
static volatile uint32_t s_post_bail_unarmed;
static volatile uint32_t s_post_bail_rxwin;
static volatile uint32_t s_post_bail_early;
static volatile uint32_t s_post_bail_busy;
static volatile uint32_t s_post_tx_submitted;

/* RX-window observer event counters — every observer fire during an
 * open RX window increments. Lets us tell whether the window is being
 * closed by (a) the observer setting close_pending, (b) the deadline
 * timer, or (c) some other path (PDS interfering, OT cycling radio
 * state, etc.) when early-close behavior looks wrong. */
static volatile uint32_t s_rx_obs_rx_done;
static volatile uint32_t s_rx_obs_rx_filt;
static volatile uint32_t s_rx_close_via_pending;
static volatile uint32_t s_rx_close_via_timeout;

/* ---- Helpers --------------------------------------------------------- */

static inline void put_u16_le(uint8_t *p, uint16_t v) {
    p[0] = (uint8_t)(v & 0xFF);
    p[1] = (uint8_t)((v >> 8) & 0xFF);
}

static inline void put_u32_le(uint8_t *p, uint32_t v) {
    p[0] = (uint8_t)(v & 0xFF);
    p[1] = (uint8_t)((v >> 8) & 0xFF);
    p[2] = (uint8_t)((v >> 16) & 0xFF);
    p[3] = (uint8_t)((v >> 24) & 0xFF);
}

static inline void put_u32_be(uint8_t *p, uint32_t v) {
    p[0] = (uint8_t)((v >> 24) & 0xFF);
    p[1] = (uint8_t)((v >> 16) & 0xFF);
    p[2] = (uint8_t)((v >> 8) & 0xFF);
    p[3] = (uint8_t)(v & 0xFF);
}

static void stamp_template(void) {
    memset(s_fp.psdu, 0, FP_PSDU_BUF_LEN);
    put_u16_le(&s_fp.psdu[0], FP_FCF);
    /* seq_num [2] — per cycle */
    put_u16_le(&s_fp.psdu[3], s_fp.pan_id);
    put_u16_le(&s_fp.psdu[5], s_fp.parent_short);
    put_u16_le(&s_fp.psdu[7], s_fp.our_short);
    s_fp.psdu[9] = FP_SEC_CTL;
    /* frame counter [10..13] — per cycle */
    s_fp.psdu[FP_OFF_KEYID] = s_fp.key_id;
    /* Cmd ID byte (0x04 Data Request) — plaintext, covered by AAD. */
    s_fp.psdu[FP_OFF_CMD_ID] = FP_CMD_DATA_REQ;
    /* MIC [16..19], FCS [20..21] — filled by mbedtls / HW respectively. */
}

static int snapshot_and_apply(void) {
    ot_fastpoll_state_t st;
    int rc = ot_fastpoll_snapshot(&st);
    if (rc != 0) {
        return -1;
    }
    if (!st.parent_valid) {
        s_fp.ready = false;
        return -2;
    }

    s_fp.pan_id = st.pan_id;
    s_fp.our_short = st.our_short;
    s_fp.parent_short = st.parent_short;
    s_fp.key_sequence = st.key_sequence;
    s_fp.key_id = st.key_id;
    s_fp.channel = st.channel;
    s_fp.frame_counter = st.mac_frame_counter;
    memcpy(s_fp.our_ext, st.our_ext, 8);
    memcpy(s_fp.mac_key, st.mac_key, 16);

    /* Only re-program the AES key when OT actually rotated it. With
     * HW AES `mbedtls_aes_setkey_enc` is a fast register write (no
     * key schedule) but still costs a HAL call — skipping it when the
     * key is unchanged saves ~10-50 µs per cycle.
     *
     * `s_aes` is in retention (.ocram_bss → ocram_high via
     * libopenthread_port glob) so the key state survives PDS. */
    if (!s_aes_keyed || memcmp(s_cached_key, s_fp.mac_key, 16) != 0) {
        mbedtls_aes_init(&s_aes);
        int mret = mbedtls_aes_setkey_enc(&s_aes, s_fp.mac_key, 128);
        if (mret != 0) {
            return -3;
        }
        memcpy(s_cached_key, s_fp.mac_key, 16);
        s_aes_keyed = true;
    }

    stamp_template();
    s_fp.ready = true;
    return 0;
}

/* ---- ISR observer ---------------------------------------------------- */

IEEE802154_HOT_SECTION
static void fp_tx_observer(ieee802154_event_type_t event,
                           ieee802154_frame_t *frame, void *ctx) {
    (void)ctx;
    (void)frame;
    switch (event) {
    case IEEE802154_EVENT_TX_DONE:
        s_obs.event = event;
        s_obs.ack_fp = (frame != NULL && frame->info.rx.frame_pending);
        s_obs.fired = true;
        return;
    case IEEE802154_EVENT_TX_NO_ACK:
    case IEEE802154_EVENT_TX_CHANNEL_BUSY:
        s_obs.event = event;
        s_obs.ack_fp = false;
        s_obs.fired = true;
        return;
    case IEEE802154_EVENT_RX_DONE:
        if (s_rx_window_open) {
            s_rx_obs_rx_done++;
        }
        if (s_rx_window_open && s_rx_early_close_enabled) {
            s_rx_window_close_pending = true;
        }
        return;
    case IEEE802154_EVENT_RX_FILTERED:
        if (s_rx_window_open) {
            s_rx_obs_rx_filt++;
        }
        if (s_rx_window_open && s_rx_early_close_enabled) {
            s_rx_window_close_pending = true;
        }
        return;
    default:
        return;
    }
}

/* ---- Public API ------------------------------------------------------ */

int sed_fastpoll_init(void) {
    memset(&s_fp, 0, sizeof(s_fp));
    int rc = snapshot_and_apply();
    if (rc == 0) {
        s_fp.seq_num = (uint8_t)(s_fp.frame_counter & 0xFF);
        IPRO_LOGI(
            LOG_TAG,
            "init OK: ch=%u pan=0x%04x us=0x%04x parent=0x%04x keyId=%u fc=%u",
            (unsigned)s_fp.channel, (unsigned)s_fp.pan_id,
            (unsigned)s_fp.our_short, (unsigned)s_fp.parent_short,
            (unsigned)s_fp.key_id, (unsigned)s_fp.frame_counter);
    }
    return rc;
}

int sed_fastpoll_refresh(void) {
    // uint32_t prev_seq = s_fp.key_sequence;
    int rc = snapshot_and_apply();
    return rc;
}

/* ---- Manual CCM MIC over AAD (no encryption, payload = 0) ----
 *
 * Replaces mbedtls_ccm_encrypt_and_tag with an open-coded
 * CBC-MAC + CTR keystream using the cached AES context. Skips
 * mbedtls_cipher_setup's malloc dance every cycle (was ~1 ms of
 * CPU + heap walk into wiped noret memory).
 *
 * RFC 3610 § 2.2/2.3 for M=4, L=2, AAD=16 B:
 *   B0  = flags(1) || nonce(13) || L_M(2)
 *         flags = AAD_FLAG<<6 | ((M-2)/2)<<3 | (L-1) = 0x49
 *         L_M   = payload length BE = 0x0000
 *   B1  = AAD_length BE(2) || AAD[0..13]  (16 B)
 *   B2  = AAD[14..15] || zero-pad(14)     (16 B)
 *   T   = CBC-MAC over B0, B1, B2 (start IV = 0)
 *   A0  = (L-1)(1) || nonce(13) || ctr=0(2)
 *   S0  = AES(A0)
 *   MIC = (T XOR S0)[0..3]
 *
 * Shared by sed_fastpoll_build_one() (OT-task path) and
 * sed_fastpoll_post_pds_tx() (retention-stack post-PDS path); both
 * operate on the same cached `s_aes` so the wake-side TX doesn't pay
 * for re-keying. */
static int fp_compute_mic(uint8_t mic_out[FP_MIC_LEN]) {
    uint8_t nonce[13];
    memcpy(&nonce[0], s_fp.our_ext, 8);
    put_u32_be(&nonce[8], s_fp.frame_counter);
    nonce[12] = (uint8_t)FP_SEC_LEVEL;

    uint8_t T[16];
    uint8_t blk[16];

    blk[0]  = 0x49; /* AAD=1, M=4, L=2 */
    memcpy(&blk[1], nonce, 13);
    blk[14] = 0x00;
    blk[15] = 0x00; /* L_M = payload length = 0 */
    if (mbedtls_aes_crypt_ecb(&s_aes, MBEDTLS_AES_ENCRYPT, blk, T) != 0) {
        return -1;
    }

    blk[0] = (uint8_t)((FP_AAD_LEN >> 8) & 0xFF);
    blk[1] = (uint8_t)(FP_AAD_LEN & 0xFF);
    memcpy(&blk[2], &s_fp.psdu[0], 14);
    for (int i = 0; i < 16; i++) blk[i] ^= T[i];
    if (mbedtls_aes_crypt_ecb(&s_aes, MBEDTLS_AES_ENCRYPT, blk, T) != 0) {
        return -1;
    }

    blk[0] = s_fp.psdu[14];
    blk[1] = s_fp.psdu[15];
    memset(&blk[2], 0, 14);
    for (int i = 0; i < 16; i++) blk[i] ^= T[i];
    if (mbedtls_aes_crypt_ecb(&s_aes, MBEDTLS_AES_ENCRYPT, blk, T) != 0) {
        return -1;
    }

    uint8_t a0[16];
    a0[0] = 0x01; /* L-1 = 1 */
    memcpy(&a0[1], nonce, 13);
    a0[14] = 0x00;
    a0[15] = 0x00;
    uint8_t S0[16];
    if (mbedtls_aes_crypt_ecb(&s_aes, MBEDTLS_AES_ENCRYPT, a0, S0) != 0) {
        return -1;
    }

    for (int i = 0; i < FP_MIC_LEN; i++) {
        mic_out[i] = T[i] ^ S0[i];
    }
    return 0;
}

/* Stamp per-cycle fields (seq, frame counter, key id, Cmd ID) and
 * compute the MIC in-place. Used by both build_one and the
 * retention-stack post-PDS path. */
static int fp_stamp_and_mic(void) {
    s_fp.psdu[FP_OFF_SEQ] = s_fp.seq_num;
    put_u32_le(&s_fp.psdu[FP_OFF_FCTR], s_fp.frame_counter);
    s_fp.psdu[FP_OFF_KEYID] = s_fp.key_id;
    s_fp.psdu[FP_OFF_CMD_ID] = FP_CMD_DATA_REQ;
    return fp_compute_mic(&s_fp.psdu[FP_OFF_MIC]);
}

int sed_fastpoll_build_one(void) {
    if (!s_fp.ready || !s_aes_keyed) {
        return -1;
    }
    return (fp_stamp_and_mic() == 0) ? 0 : -2;
}

int sed_fastpoll_tx(uint32_t wait_us, sed_fastpoll_tx_result_t *result_out) {
    if (result_out == NULL || !s_fp.ready) {
        if (result_out)
            *result_out = SED_FP_TX_ERROR;
        return -1;
    }

    ieee802154_frame_t tx;
    memset(&tx, 0, sizeof(tx));
    memcpy(tx.psdu, s_fp.psdu, FP_PSDU_BUF_LEN);
    /* HAL `length` is MPDU bytes only — lmac154 appends its own FCS. */
    tx.length = FP_MPDU_LEN;
    tx.info.tx.channel = s_fp.channel;
    tx.info.tx.tx_power = 0;
    tx.info.tx.ack_request = true;
    /* CSMA off — SED has a single parent, channel is effectively private
     * (parent-managed RX schedule), and the ~1 ms average CSMA backoff
     * window dominates the active power envelope. Skipping it cuts the
     * radio-on time per cycle by ~1-2 ms with no measurable collision
     * rate at the bench. */
    tx.info.tx.csma_ca_enabled = false;
    tx.info.tx.frame_pending = false;
    tx.info.tx.max_csma_backoffs = 0;
    tx.info.tx.max_frame_retries = 0;

    s_obs.event = IEEE802154_EVENT_TX_NO_ACK;
    s_obs.ack_fp = false;
    s_obs.fired = false;
    __asm__ volatile("" ::: "memory");

    /* Observer is bound at engine enable-time; no per-TX rebind. */
    ieee802154_error_t err = ieee802154_hal_transmit(&tx);
    if (err != IEEE802154_OK) {
        *result_out = SED_FP_TX_ERROR;
        return -3;
    }

    /* WFI between flag checks — CPU sleeps during the CSMA backoff +
     * TX + ACK window (~3-4 ms) instead of spinning at full clock.
     * Any IRQ wakes us: the M154 TX_DONE IRQ (which fires the observer
     * callback and sets s_obs.fired) is the fast path; the FreeRTOS
     * SysTimer tick (~1 ms) is the slow path used to enforce the
     * deadline. CPU current drops from ~4 mA (active spin at XTAL 32M)
     * to ~µA (WFI) for the duration — saves ~12 µA average. */
    uint64_t deadline = mtimer_get_time_us() + wait_us;
    while (!s_obs.fired && ieee802154_hal_is_tx_pending()) {
        if (mtimer_get_time_us() >= deadline) {
            break;
        }
        __asm__ volatile("wfi" ::: "memory");
    }

    if (!s_obs.fired) {
        *result_out = SED_FP_TX_TIMEOUT;
        return -4;
    }

    switch (s_obs.event) {
    case IEEE802154_EVENT_TX_DONE:
        *result_out = s_obs.ack_fp ? SED_FP_TX_FP_YES : SED_FP_TX_FP_NO;
        break;
    case IEEE802154_EVENT_TX_NO_ACK:
        *result_out = SED_FP_TX_NO_ACK;
        break;
    case IEEE802154_EVENT_TX_CHANNEL_BUSY:
        *result_out = SED_FP_TX_BUSY;
        break;
    default:
        *result_out = SED_FP_TX_ERROR;
        break;
    }
    return 0;
}

int sed_fastpoll_commit(void) {
    if (!s_fp.ready) {
        return -1;
    }
    uint32_t used = s_fp.frame_counter;
    int rc = ot_fastpoll_commit_counter(used);
    if (rc != 0) {
        return -2;
    }
    s_fp.frame_counter = used + 1;
    s_fp.seq_num = (uint8_t)(s_fp.seq_num + 1);
    return 0;
}

bool sed_fastpoll_is_ready(void) { return s_fp.ready; }

uint8_t sed_fastpoll_get_channel(void) {
    return s_fp.ready ? s_fp.channel : 0;
}

void sed_fastpoll_set_rx_early_close(bool enable) {
    s_rx_early_close_enabled = enable;
    if (!enable) {
        s_rx_window_close_pending = false;
    }
}

bool sed_fastpoll_get_rx_early_close(void) {
    return s_rx_early_close_enabled;
}

void sed_fastpoll_log_debug(const char *log_tag) {
    const char *tag = (log_tag != NULL) ? log_tag : LOG_TAG;
    if (!s_fp.ready) {
        IPRO_LOGI(tag, "fastpoll not ready");
        return;
    }
    IPRO_LOGI(tag,
              "fastpoll: ch=%u pan=0x%04x us=0x%04x parent=0x%04x keyId=%u "
              "fc=%u seq=%u",
              (unsigned)s_fp.channel, (unsigned)s_fp.pan_id,
              (unsigned)s_fp.our_short, (unsigned)s_fp.parent_short,
              (unsigned)s_fp.key_id, (unsigned)s_fp.frame_counter,
              (unsigned)s_fp.seq_num);
    IPRO_LOGI(tag,
              "psdu: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x "
              "%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
              s_fp.psdu[0], s_fp.psdu[1], s_fp.psdu[2], s_fp.psdu[3],
              s_fp.psdu[4], s_fp.psdu[5], s_fp.psdu[6], s_fp.psdu[7],
              s_fp.psdu[8], s_fp.psdu[9], s_fp.psdu[10], s_fp.psdu[11],
              s_fp.psdu[12], s_fp.psdu[13], s_fp.psdu[14], s_fp.psdu[15],
              s_fp.psdu[16], s_fp.psdu[17], s_fp.psdu[18], s_fp.psdu[19]);
    IPRO_LOGI(tag,
              "post_pds: entries=%lu tx=%lu bail(unready=%lu unarmed=%lu "
              "rxwin=%lu early=%lu busy=%lu)",
              (unsigned long)s_post_entries,
              (unsigned long)s_post_tx_submitted,
              (unsigned long)s_post_bail_unready,
              (unsigned long)s_post_bail_unarmed,
              (unsigned long)s_post_bail_rxwin,
              (unsigned long)s_post_bail_early,
              (unsigned long)s_post_bail_busy);
    IPRO_LOGI(tag,
              "rx_window: early_close=%s obs_rx_done=%lu obs_rx_filt=%lu "
              "close_pending=%lu close_timeout=%lu",
              s_rx_early_close_enabled ? "ON" : "OFF",
              (unsigned long)s_rx_obs_rx_done,
              (unsigned long)s_rx_obs_rx_filt,
              (unsigned long)s_rx_close_via_pending,
              (unsigned long)s_rx_close_via_timeout);
}

const char *sed_fastpoll_tx_result_str(sed_fastpoll_tx_result_t r) {
    switch (r) {
    case SED_FP_TX_FP_NO:
        return "FP_NO";
    case SED_FP_TX_FP_YES:
        return "FP_YES";
    case SED_FP_TX_NO_ACK:
        return "NO_ACK";
    case SED_FP_TX_BUSY:
        return "BUSY";
    case SED_FP_TX_TIMEOUT:
        return "TIMEOUT";
    case SED_FP_TX_ERROR:
        return "ERROR";
    default:
        return "?";
    }
}

/* ---- Engine scheduling (driven from ot_freertos task loop) ---------- */

#define FP_RX_AFTER_YES_MS                                                     \
    150u /* RX window after FP_YES — long enough for                         \
          * parent's indirect frame to arrive                                  \
          * (matches OT's kDataPollTimeout-ish budget). */

/* Default poll cadence when caller passes 0 to sed_fastpoll_enable().
 * Tracks the app's CONFIG_THREAD_SED_POLL_PERIOD_MS so the engine and
 * OT's own DataPollSender stay in sync — same Kconfig drives both
 * `otLinkSetPollPeriod` (fallback when engine off) and the engine's
 * period, so the air-time cadence doesn't change when toggling
 * `pds on/off`. Falls back to 1000 ms when the app doesn't define
 * CONFIG_THREAD_SED_POLL_PERIOD_MS. */
#ifdef CONFIG_THREAD_SED_POLL_PERIOD_MS
#define FP_DEFAULT_POLL_MS ((uint32_t)CONFIG_THREAD_SED_POLL_PERIOD_MS)
#else
#define FP_DEFAULT_POLL_MS 1000u
#endif

/* Time source: FreeRTOS tick count, NOT mtimer/SysTimer.
 *
 * SysTimer (used by `mtimer_get_time_us`) sits inside the CPU domain and
 * is reset by every PDS31 wake (cpuRst=1 path power-cycles it). Using it
 * as an absolute deadline makes the engine appear to never progress —
 * each PDS cycle resets `now()` while `next_poll_us` stays at its
 * pre-sleep value (observed 2026-05-19: `next_poll-now` stuck at
 * 7748 ms across 14 s of real time, engine TXed once after attach and
 * then never again).
 *
 * FreeRTOS ticks are corrected by `vTaskStepTick(ticks_slept)` in the
 * lp_sleep wake hook, so they advance monotonically across PDS sleeps.
 * 32-bit wrap at 1 kHz tick rate = ~49 days, which is fine here; we
 * use signed-difference comparisons that survive wrap. */

typedef struct {
    bool enabled;
    uint32_t period_ms;
    TickType_t
        next_poll_tick; /* absolute FreeRTOS tick; valid only if `armed` */
    TickType_t rx_window_tick; /* absolute FreeRTOS tick; 0 = no window open */
    bool armed;                /* false = run on next tick */
    /* Telemetry — light enough to maintain on every cycle even when
     * the heartbeat log is silent. Wraps at 2^32 polls (~136 years at
     * 1 Hz), which is fine. */
    uint32_t cycle_count;
    uint32_t fp_no_count;
    uint32_t fp_yes_count;
    /* Tick (in FreeRTOS ticks) when ieee802154_hal_is_tx_pending() first
     * blocked a poll. Used to bound how long the engine defers to an
     * in-flight TX before force-clearing a stranded flag (see tick). 0 = not
     * currently blocked. */
    TickType_t tx_pending_since;
} fp_sched_t;

/* Heartbeat cadence — emit one summary log per FP_HB_EVERY_N polls.
 * 30 lines up nicely with lp_sleep's 30-cycle PDS31 stat log so the two
 * appear interleaved in the steady-state console. Bumped down to 10 on
 * 2026-05-19 while diagnosing the "PDS exits after 0.8 ms" plateau —
 * need denser data points within a 30-60 s PPK capture window. */
#define FP_HB_EVERY_N 10u

/* RETAINED across PDS31: the engine's schedule (enabled/period/next_poll_tick/
 * armed). If wiped on a PDS wake the tick reports not-ready and the OT task
 * sleeps the full LP_MAX_SLEEP cap (~10 s) without polling — the "zombie"
 * symptom seen on the no-PSRAM Matter SED. The OT task + its stack are already
 * retained, so retaining this is what lets the engine keep driving polls
 * across deep sleep. */
static __attribute__((section(".ocram_bss"))) fp_sched_t s_sched;

/* Does the engine OWN the PDS31 gate (call lp_sleep_enable_pds)? Default true
 * for Thread-only apps (thread_mtd/fall_neck), where the engine's readiness is
 * the only gate signal. A Matter ICD sets this FALSE via
 * sed_fastpoll_set_drive_gate() so CHIP's _SetPollingInterval is the sole gate
 * owner (active=OFF / idle=ON); otherwise the engine forces the gate ON every
 * tick and PDS fires during ACTIVE mode. Set in sed_fastpoll_enable() (default)
 * and retained across PDS so the choice survives deep sleep. */
static __attribute__((section(".ocram_bss"))) bool s_engine_drives_gate;

void sed_fastpoll_set_drive_gate(bool enable) { s_engine_drives_gate = enable; }

/* True iff tick `a` >= tick `b` accounting for 32-bit wrap. Equivalent
 * to FreeRTOS's own wrap-aware comparisons. */
static inline bool tick_ge(TickType_t a, TickType_t b) {
    return (int32_t)(a - b) >= 0;
}

int sed_fastpoll_enable(uint32_t period_ms) {
    if (period_ms == 0) {
        period_ms = FP_DEFAULT_POLL_MS;
    }

    /* Default: engine owns the PDS gate (Thread-only apps). A Matter ICD calls
     * sed_fastpoll_set_drive_gate(false) AFTER this to hand the gate to CHIP. */
    s_engine_drives_gate = true;

    /* Stop OT's internal DataPollSender so it no longer competes with us
     * for the radio. otLinkSetPollPeriod() alone is insufficient because
     * OT's fast-poll burst mode (kFastPollPeriod = 188 ms) ignores the
     * external period. */
    ot_fastpoll_stop_ot_poll_sender();

    /* CRITICAL: stopping the DataPollSender does NOT abort a Data Request it
     * already submitted to the radio. If one is in flight awaiting an ACK,
     * its terminal ISR is now orphaned (OT won't service it) and the HAL's
     * s_tx_ack_pending / s_tx_in_flight stay TRUE forever — which makes the
     * engine's tick early-return on ieee802154_hal_is_tx_pending() every cycle,
     * spinning in a sub-gate WFI loop that never polls and never enters PDS
     * (flat-high current, no heartbeat, child detaches). Drop that stranded
     * TX state so the engine starts from a clean radio. */
    extern void ieee802154_hal_clear_tx_pending(void);
    ieee802154_hal_clear_tx_pending();

    s_sched.enabled = true;
    s_sched.period_ms = period_ms;
    s_sched.next_poll_tick = 0; /* irrelevant until armed=true */
    s_sched.rx_window_tick = 0;
    s_sched.tx_pending_since = 0;
    s_sched.armed = false; /* fire on the very next tick */
    s_sched.cycle_count = 0;
    s_sched.fp_no_count = 0;
    s_sched.fp_yes_count = 0;

    /* Engine-level observer bind — stays installed for the lifetime of
     * the engine so RX frames during the FP=YES RX window also reach
     * us (set the close-pending flag) without us having to re-bind
     * around every TX. The HAL only has one observer slot; the
     * primary OT callback (registered separately via
     * ieee802154_register_callback) still gets every event after the
     * observer returns. */
    s_obs.fired = false;
    s_rx_window_open = false;
    s_rx_window_close_pending = false;
    (void)ieee802154_set_tx_observer(fp_tx_observer, NULL);

#if FP_DBG_GPIO_RX_WINDOW
    /* Configure GPIO 28 as OUTPUT once. gpio_set_mode(pin, 0) =
     * GPIO_OUTPUT_MODE. CFGCTL35 OE bit redundant after this but
     * harmless. Pin starts LOW. */
    extern void gpio_set_mode(uint32_t pin, uint32_t mode);
    gpio_set_mode(28u, 0u);
    FP_DBG_RX_OE();
    FP_DBG_RX_LOW();
#endif
#if FP_DBG_GPIO_TICK
    extern void gpio_set_mode(uint32_t pin, uint32_t mode);
    gpio_set_mode(31u, 0u);
    FP_DBG_TICK_OE();
    FP_DBG_TICK_LOW();
#endif
    return 0;
}

void sed_fastpoll_disable(void) {
    s_sched.enabled = false;
    s_sched.armed = false;
    s_sched.next_poll_tick = 0;
    s_sched.rx_window_tick = 0;
    s_rx_window_close_pending = false;
    (void)ieee802154_set_tx_observer(NULL, NULL);
    /* Hand the poll cadence back to OT, and close the PDS31 gate so the
     * radio doesn't get power-cycled mid-handshake if the app keeps the
     * OT task alive after disable. No-op when LP isn't compiled in. */
    ot_fastpoll_start_ot_poll_sender();
    if (s_engine_drives_gate) {
        lp_sleep_enable_pds(false);
    }
}

bool sed_fastpoll_post_pds_tx(void) {
    FP_DBG_TICK_HIGH();   /* mark engine TX window */
    /* Caller is lp_pds_sleep_impl on the retention stack — FreeRTOS
     * scheduler hasn't resumed yet, M154 is up via lp_wakeup_hook, IRQs
     * are enabled. We bypass the OT task entirely: stamp + MIC + radio
     * submit, then spin/WFI until the TX-done observer fires. The result
     * lands in `s_post_tx_*` and the OT task's next tick consumes it.
     *
     * Bail if there's nothing useful we can do without OT:
     *   - engine disabled or not ready (no parent, no key)
     *   - previous cycle's result not yet consumed (OT task lagging)
     *   - we just woke up for some non-poll reason and the parent isn't
     *     stale enough to need a poll yet
     * In all those cases the existing OT-task tick path takes over. */
    s_post_entries++;
    if (!s_sched.enabled || !s_fp.ready || !s_aes_keyed) {
        s_post_bail_unready++;
        FP_DBG_TICK_LOW();
        return false;
    }
    if (s_post_tx_done) {
        s_post_bail_busy++;
        FP_DBG_TICK_LOW();
        return false;
    }
    if (!s_sched.armed) {
        /* Cycle 0 (just enabled) — let OT task's tick() do the first
         * TX so we don't fire before snapshot+state is fully primed. */
        s_post_bail_unarmed++;
        FP_DBG_TICK_LOW();
        return false;
    }
    if (s_sched.rx_window_tick != 0) {
        /* RX-after-FP_YES window still open — don't TX over our own
         * pending receive. Tick() will close it on schedule. */
        s_post_bail_rxwin++;
        FP_DBG_TICK_LOW();
        return false;
    }

    /* Only direct-TX when the schedule actually wants a poll. Use the
     * RAW pre-step tick value — vTaskStepTick hasn't run yet, so
     * xTaskGetTickCount() reads the pre-sleep value. The PDS sleep is
     * scheduled by FreeRTOS exactly because some task wants to unblock;
     * if `next_poll_tick` is in the past relative to the post-sleep
     * tick, this wake IS the poll. We approximate by checking against
     * an estimate = pre_sleep_tick + s_pds_sleep_ticks; in practice
     * since the engine paces itself to the same period as the PDS
     * sleeps, `next_poll_tick` will land within one tick of this. */
    extern uint32_t s_pds_sleep_ticks;
    TickType_t now_est = xTaskGetTickCount() + s_pds_sleep_ticks;
    if ((int32_t)(now_est - s_sched.next_poll_tick) < 0) {
        /* Wake fired earlier than next poll — let OT task handle it. */
        s_post_bail_early++;
        FP_DBG_TICK_LOW();
        return false;
    }
    s_post_tx_submitted++;

    if (fp_stamp_and_mic() != 0) {
        FP_DBG_TICK_LOW();
        return false;
    }

    /* TX submit — same payload + flags as the OT-task path. CSMA off
     * (same trade-off rationale as sed_fastpoll_tx). */
    ieee802154_frame_t tx;
    memset(&tx, 0, sizeof(tx));
    memcpy(tx.psdu, s_fp.psdu, FP_PSDU_BUF_LEN);
    tx.length = FP_MPDU_LEN;
    tx.info.tx.channel = s_fp.channel;
    tx.info.tx.tx_power = 0;
    tx.info.tx.ack_request = true;
    tx.info.tx.csma_ca_enabled = false;
    tx.info.tx.frame_pending = false;
    tx.info.tx.max_csma_backoffs = 0;
    tx.info.tx.max_frame_retries = 0;

    s_obs.event = IEEE802154_EVENT_TX_NO_ACK;
    s_obs.ack_fp = false;
    s_obs.fired = false;
    __asm__ volatile("" ::: "memory");

    /* Observer is bound at engine enable-time; no per-TX rebind. */
    if (ieee802154_hal_transmit(&tx) != IEEE802154_OK) {
        FP_DBG_TICK_LOW();
        return false;
    }

    /* WFI loop — IRQs are live, M154 TX_DONE will trip the observer.
     * Deadline 50 ms covers worst-case (single retry on parent miss). */
    uint64_t deadline = mtimer_get_time_us() + 50000ULL;
    while (!s_obs.fired && ieee802154_hal_is_tx_pending()) {
        if (mtimer_get_time_us() >= deadline) break;
        __asm__ volatile("wfi" ::: "memory");
    }

    sed_fastpoll_tx_result_t r;
    if (!s_obs.fired) {
        r = SED_FP_TX_TIMEOUT;
    } else {
        switch (s_obs.event) {
        case IEEE802154_EVENT_TX_DONE:
            r = s_obs.ack_fp ? SED_FP_TX_FP_YES : SED_FP_TX_FP_NO;
            break;
        case IEEE802154_EVENT_TX_NO_ACK:
            r = SED_FP_TX_NO_ACK;
            break;
        case IEEE802154_EVENT_TX_CHANNEL_BUSY:
            r = SED_FP_TX_BUSY;
            break;
        default:
            r = SED_FP_TX_ERROR;
            break;
        }
    }

    /* Local counter advance — OT side is updated when the task tick
     * consumes `s_post_tx_used_counter`. If we never get to commit
     * (e.g. power loss), OT's persisted counter is at most one cycle
     * behind, same risk as OT's own batched persistence. */
    if (r == SED_FP_TX_FP_NO || r == SED_FP_TX_FP_YES) {
        s_post_tx_used_counter = s_fp.frame_counter;
        s_fp.frame_counter += 1;
        s_fp.seq_num = (uint8_t)(s_fp.seq_num + 1);
        if (r == SED_FP_TX_FP_YES) {
            /* Parent has indirect data — open RX so the follow-up
             * frame can arrive before we PDS again. Observer marks
             * `s_rx_window_close_pending` on first RX event; tick()
             * either consumes that or closes on FP_RX_AFTER_YES_MS. */
            (void)ieee802154_hal_rx_start(s_fp.channel);
            s_rx_window_open = true;
            s_rx_window_close_pending = false;
            FP_DBG_RX_HIGH();
        }
    }
    s_post_tx_result = r;
    s_post_tx_done = true;
    FP_DBG_TICK_LOW();   /* engine TX done */
    return true;
}

void sed_fastpoll_tick(void) {
    if (!s_sched.enabled) {
        return;
    }

    /* Consume direct-TX result from lp_pds_sleep_impl's post-wake fast
     * path, if any. When `s_post_tx_done` is set, the radio already
     * fired this cycle's Data Request before the scheduler resumed; we
     * just need to settle the OT-side bookkeeping and reschedule. */
    if (s_post_tx_done) {
        FP_DBG_TICK_HIGH();   /* mark consume path */
        sed_fastpoll_tx_result_t r = s_post_tx_result;
        uint32_t used = s_post_tx_used_counter;
        s_post_tx_done = false;

        s_sched.cycle_count++;
        if (r == SED_FP_TX_FP_YES) {
            s_sched.fp_yes_count++;
        } else if (r == SED_FP_TX_FP_NO) {
            s_sched.fp_no_count++;
        }

        if (r == SED_FP_TX_FP_NO || r == SED_FP_TX_FP_YES) {
            (void)ot_fastpoll_commit_counter(used);
            if (r == SED_FP_TX_FP_YES) {
                /* RX window already opened in post_pds_tx — record its
                 * deadline so tick() closes the radio on schedule. */
                s_sched.rx_window_tick =
                    xTaskGetTickCount() + pdMS_TO_TICKS(FP_RX_AFTER_YES_MS);
            }
        }
        /* Absolute scheduling: next poll is `period_ms` after the last
         * scheduled poll, NOT after "now". `now` here is post-PDS wake
         * + active processing (~5-200 ms depending on what OT did this
         * tick) — anchoring to it lets each cycle's housekeeping drift
         * push the cadence later, observed as ~1.2 s poll interval on a
         * 1 s configured period. Anchoring to the previous slot keeps
         * the air-time cadence locked to `period_ms`. If we somehow
         * missed several slots (e.g., long blocking call), snap forward
         * to the next slot ≥ now so we don't burst-fire to catch up. */
        TickType_t now = xTaskGetTickCount();
        TickType_t period = pdMS_TO_TICKS(s_sched.period_ms);
        TickType_t target = s_sched.next_poll_tick + period;
        if ((int32_t)(target - now) < 0) {
            target = now + period;
        }
        s_sched.next_poll_tick = target;
        s_sched.armed = true;

        /* Auto-stats every cycle via lp_uart_puts (raw UART writes,
         * known to drain reliably post-PDS — no FreeRTOS mutex,
         * unlike printf). Compact format. */
        {
            extern volatile uint32_t g_ot_alarm_fire_count;
            lp_uart_puts(0, "[fp] cyc=");
            lp_uart_putdec(0, s_sched.cycle_count);
            lp_uart_puts(0, " tx=");
            lp_uart_putdec(0, s_post_tx_submitted);
            lp_uart_puts(0, " bail(rxw=");
            lp_uart_putdec(0, s_post_bail_rxwin);
            lp_uart_puts(0, " early=");
            lp_uart_putdec(0, s_post_bail_early);
            lp_uart_puts(0, " busy=");
            lp_uart_putdec(0, s_post_bail_busy);
            lp_uart_puts(0, ") rxd=");
            lp_uart_putdec(0, s_rx_obs_rx_done);
            lp_uart_puts(0, " alm=");
            lp_uart_putdec(0, g_ot_alarm_fire_count);
            extern volatile uint32_t g_ot_notified_wake_count;
            extern volatile uint32_t g_ot_timeout_wake_count;
            lp_uart_puts(0, " nwk=");
            lp_uart_putdec(0, g_ot_notified_wake_count);
            lp_uart_puts(0, "/");
            lp_uart_putdec(0, g_ot_timeout_wake_count);
            lp_uart_puts(0, "\r\n");
        }
        FP_DBG_TICK_LOW();   /* consume path done */
        return;
    }

    /* Only re-suppress OT's DataPollSender when we have a stable parent.
     * Calling StopPolling unconditionally also clears `mAttachMode` /
     * `mRemainingFastPolls` etc., which breaks OT's normal attach /
     * reattach flow (it needs fast polls to receive Child ID Response).
     * Observed 2026-05-19: detach + reattach loop after we stopped
     * polling, because each tick clobbered OT's attach state. */
    const bool ready = sed_fastpoll_is_ready();
    if (ready) {
        ot_fastpoll_stop_ot_poll_sender();
    }

    /* Drive the PDS31 entry gate from the same readiness signal — UNLESS the
     * app owns the gate (Matter). In a Matter ICD, CHIP's _SetPollingInterval
     * drives lp_sleep_enable_pds (idle=ON / active=OFF); if the engine also
     * drove it from `ready` it would force the gate ON every tick and PDS
     * would engage even during ICD ACTIVE mode (mid-CASE / command exchange).
     * For Thread-only apps (thread_mtd/fall_neck) the engine remains the sole
     * gate owner (s_engine_drives_gate defaults true). */
    if (s_engine_drives_gate) {
        lp_sleep_enable_pds(ready);
    }

    TickType_t now = xTaskGetTickCount();

    /* Close the FP=YES RX window — either because the observer signaled
     * a frame arrived (typical ~10-20 ms after open, vs full 150 ms
     * timeout) or because we hit the deadline. Both paths gate on
     * `rx_window_tick != 0` so we don't double-stop. */
    if (s_sched.rx_window_tick != 0 &&
        (s_rx_window_close_pending || tick_ge(now, s_sched.rx_window_tick))) {
        if (s_rx_window_close_pending) s_rx_close_via_pending++;
        else s_rx_close_via_timeout++;
        (void)ieee802154_hal_rx_stop();
        s_sched.rx_window_tick = 0;
        s_rx_window_open = false;
        s_rx_window_close_pending = false;
        FP_DBG_RX_LOW();
    }

    if (s_sched.armed && !tick_ge(now, s_sched.next_poll_tick)) {
        return;
    }

    /* Some OT-driven TX (Child Update Request, IPv6) is in flight — skip
     * this cycle to avoid clobbering the radio. The HAL flag covers the
     * window from triggerTx to the matching TX_DONE / ACK / NO_ACK
     * dispatch.
     *
     * BUT bound the deferral: a real TX clears within ~10 ms (CSMA + TX +
     * ACK/timeout). If the flag is still set after the worst-case window it is
     * STRANDED (an abandoned TX whose terminal ISR will never arrive) — left
     * unbounded this spins a 5 ms reschedule loop forever, so the engine never
     * polls and the windows stay < the PDS gate (flat-high WFI, child detaches).
     * Force-clear after the window so the engine reclaims the radio and polls. */
    if (ieee802154_hal_is_tx_pending()) {
        if (s_sched.tx_pending_since == 0) {
            s_sched.tx_pending_since = now;
        } else if ((int32_t)(now - s_sched.tx_pending_since) >
                   (int32_t)pdMS_TO_TICKS(50)) {
            extern void ieee802154_hal_clear_tx_pending(void);
            ieee802154_hal_clear_tx_pending();
            s_sched.tx_pending_since = 0;
            lp_uart_puts(0, "[fp] tx_pending stranded >50ms -> force-clear\r\n");
            /* fall through to poll this cycle */
        }
        if (ieee802154_hal_is_tx_pending()) {
            s_sched.next_poll_tick = now + pdMS_TO_TICKS(5);
            s_sched.armed = true;
            return;
        }
    } else {
        s_sched.tx_pending_since = 0;
    }

    /* Snapshot / refresh before each poll — picks up key rotation and
     * post-attach parent updates. Bail (will retry next tick) if not
     * yet attached as Child with a valid parent. */
    if (!sed_fastpoll_is_ready()) {
        if (sed_fastpoll_init() != 0) {
            s_sched.next_poll_tick = now + pdMS_TO_TICKS(s_sched.period_ms);
            s_sched.armed = true;
            return;
        }
    } else {
        (void)sed_fastpoll_refresh();
        if (!sed_fastpoll_is_ready()) {
            s_sched.next_poll_tick = now + pdMS_TO_TICKS(s_sched.period_ms);
            s_sched.armed = true;
            return;
        }
    }

    if (sed_fastpoll_build_one() != 0) {
        s_sched.next_poll_tick = now + pdMS_TO_TICKS(s_sched.period_ms);
        s_sched.armed = true;
        return;
    }

    /* wait_us must cover worst-case CSMA backoff: with default BE=3 and
     * max_csma_backoffs=4 the backoff sum can reach ~28 ms, plus TX
     * (~706 µs at 250 kbps for 22 bytes) + ack turnaround (~864 µs) +
     * ACK frame (~160 µs). 50 ms gives comfortable headroom. */

    sed_fastpoll_tx_result_t result;
    int rc = sed_fastpoll_tx(/*wait_us=*/50000u, &result);

    /* Maintain lightweight cycle counters so a heartbeat log every
     * `FP_HB_EVERY_N` polls can confirm the engine is alive without
     * spamming on every successful poll. NO_ACK / BUSY / TIMEOUT / ERROR
     * are logged inline so any sustained anomaly remains visible. */
    s_sched.cycle_count++;
    if (result == SED_FP_TX_FP_YES) {
        s_sched.fp_yes_count++;
    } else if (result == SED_FP_TX_FP_NO) {
        s_sched.fp_no_count++;
    }

    if (rc == 0 && (result == SED_FP_TX_FP_NO || result == SED_FP_TX_FP_YES)) {
        (void)sed_fastpoll_commit();
        if (result == SED_FP_TX_FP_YES) {
            (void)ieee802154_hal_rx_start(s_fp.channel);
            s_sched.rx_window_tick =
                xTaskGetTickCount() + pdMS_TO_TICKS(FP_RX_AFTER_YES_MS);
            s_rx_window_open = true;
            s_rx_window_close_pending = false;
            FP_DBG_RX_HIGH();
        }
    }

    /* Absolute scheduling — same rationale as the consume path above:
     * anchor the next slot to the previous schedule, not to "now after
     * active processing", so the air-time cadence stays locked to
     * `period_ms`. */
    {
        TickType_t now = xTaskGetTickCount();
        TickType_t period = pdMS_TO_TICKS(s_sched.period_ms);
        TickType_t target = (s_sched.armed ? s_sched.next_poll_tick : now)
                          + period;
        if ((int32_t)(target - now) < 0) {
            target = now + period;
        }
        s_sched.next_poll_tick = target;
    }
    s_sched.armed = true;

    /* Auto-stats every cycle from the normal-TX path (mirrors the
     * consume path above). */
    {
        extern volatile uint32_t g_ot_alarm_fire_count;
        lp_uart_puts(0, "[fp] cyc=");
        lp_uart_putdec(0, s_sched.cycle_count);
        lp_uart_puts(0, " tx=");
        lp_uart_putdec(0, s_post_tx_submitted);
        lp_uart_puts(0, " rxd=");
        lp_uart_putdec(0, s_rx_obs_rx_done);
        lp_uart_puts(0, " alm=");
        lp_uart_putdec(0, g_ot_alarm_fire_count);
        lp_uart_puts(0, "\r\n");
    }
}

uint32_t sed_fastpoll_next_wake_ms(void) {
    if (!s_sched.enabled) {
        return 0xFFFFFFFFu; /* engine off → no preference, use heartbeat cap */
    }
    if (!s_sched.armed) {
        return 0; /* enabled but never run — fire next iteration */
    }
    TickType_t now = xTaskGetTickCount();

    /* `next_poll_tick` is always present once armed; rx_window may be 0. */
    TickType_t soonest_diff = (TickType_t)(s_sched.next_poll_tick - now);
    if (tick_ge(now, s_sched.next_poll_tick)) {
        return 0;
    }
    if (s_sched.rx_window_tick != 0) {
        if (tick_ge(now, s_sched.rx_window_tick)) {
            return 0;
        }
        TickType_t rx_diff = (TickType_t)(s_sched.rx_window_tick - now);
        if ((int32_t)(rx_diff - soonest_diff) < 0) {
            soonest_diff = rx_diff;
        }
    }
    /* Convert ticks → ms. With 1 kHz tick rate this is identity. */
    uint64_t ms = ((uint64_t)soonest_diff * 1000u) / configTICK_RATE_HZ;
    return ms > 0xFFFFFFFFu ? 0xFFFFFFFFu : (uint32_t)ms;
}
