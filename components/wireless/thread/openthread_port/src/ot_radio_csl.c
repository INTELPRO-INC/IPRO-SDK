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
 * @file ot_radio_csl.c
 * @brief CSL receiver (SSED) support of the IPRO7 OpenThread radio port:
 *        radio-side transmit security, CSL IE and the secured Enh-ACK.
 *
 * With a CSL receiver the radio declares OT_RADIO_CAPS_TRANSMIT_SEC:
 *  - every outgoing frame carrying a CSL IE needs the phase written at
 *    transmit time, after which it can only be secured here;
 *  - the Enh-ACK (with its CSL IE) is built and secured in the MAC interrupt
 *    (ieee802154_hal_lmac154.c prearm path) and must use the same frame
 *    counter as the frames sent from the task.
 * The security itself is the shared ipro154_sec.c (software AES-128, also
 * used by the Zephyr driver). Frames are secured in the HAL's transmit copy:
 * the stack's frame stays plaintext, so a retransmission gets a fresh CSL
 * phase and is secured again with the frame counter written on the first try.
 * The stack drives the CSL sample windows itself (no RECEIVE_TIMING) with the
 * microsecond alarm, through otPlatRadioReceive() / otPlatRadioSleep().
 */
#include "openthread-core-config.h"

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <openthread/platform/radio.h>
#include <openthread/platform/time.h>
#if OPENTHREAD_CONFIG_PLATFORM_KEY_REFERENCES_ENABLE
#include <openthread/platform/crypto.h>
#endif
#include "ipro154_sec.h"
#include "ieee802154_port.h"
#include "ot_radio_csl.h"
#if defined(CONFIG_IPRO_PM_THREAD) && (defined(CONFIG_IPRO_PM_IDLE_PDS) || defined(CONFIG_IPRO_PM_TIMEBASE))
#include "ipro_pm_radio.h"
#define OT_RADIO_CSL_LPCLK_PPM() pm_lpclk_ppm()
#else
#define OT_RADIO_CSL_LPCLK_PPM() 0
#endif

extern void ieee802154_hal_set_csl_ie(uint16_t period_10sym, uint32_t sample_time_us);
extern void ieee802154_hal_set_enh_ack_secure(int (*fn)(uint8_t *ack, uint8_t len, uint8_t max));
extern uint64_t ieee802154_hal_get_timestamp(void);

#define OT_RADIO_CSL_TX_EST_US 600u /* CSMA backoff + turnaround + SHR, typical (as the Zephyr driver) */
#define OT_RADIO_KEY_INDEX_MAX 128u

static struct ipro154_sec s_sec;
static uint8_t s_ext_addr[8]; /* over-the-air order, as the address filter takes it */
static uint32_t s_sec_tx_fail;

/* RISC-V machine interrupt enable: the frame counter is shared with the MAC
 * interrupt, whatever its priority against the RTOS critical sections. */
IEEE802154_HOT_SECTION
unsigned int ipro154_sec_lock(void)
{
    unsigned long mstatus;

    __asm volatile("csrrci %0, mstatus, 8" : "=r"(mstatus) : : "memory");
    return (unsigned int)mstatus;
}

IEEE802154_HOT_SECTION
void ipro154_sec_unlock(unsigned int key)
{
    if (key & 8u) {
        __asm volatile("csrsi mstatus, 8" : : : "memory");
    }
}

static int ot_radio_csl_secure_enh_ack(uint8_t *ack, uint8_t len, uint8_t max)
{
    return ipro154_sec_secure_enh_ack(&s_sec, ack, len, max, s_ext_addr);
}

void ot_radio_csl_init(void)
{
    ieee802154_hal_set_enh_ack_secure(ot_radio_csl_secure_enh_ack);
}

void ot_radio_csl_set_ext_addr(const uint8_t ext_addr[8])
{
    memcpy(s_ext_addr, ext_addr, sizeof(s_ext_addr));
}

uint32_t ot_radio_csl_tx_sec_failures(void)
{
    return s_sec_tx_fail;
}

/* How far ahead of the CSL sample time the receiver really came on: OT
 * opens the window (sample - ahead - semi-width) from the microsecond alarm
 * and the OT task, so this shows the port's wake-up latency eating into it. */
static struct {
    uint32_t n, deferred, no_rx;
    int32_t  min, max;
    int64_t  sum;
} s_rx_lead = { 0, 0, 0, INT32_MAX, INT32_MIN, 0 };
static bool s_window_opening; /* otPlatRadioUpdateCslSampleTime ran, its Receive not yet */

/* Arrival of the parent's CSL frames against our sample time (MHR start vs
 * the window's sample, as OpenThread's CSL debug log computes it), with the
 * time since our last frame carrying a CSL IE (the parent re-synchronizes
 * on it).  Its drift between syncs is our clock's error against the
 * parent's - across a platform PDS, the sleep-clock error of the rebase,
 * logged with the sleep-clock correction then in use (ipro_pm). */
#define OT_RADIO_CSL_DEV_RING 16u
static uint64_t s_window_sample_us;  /* sample time of the open window */
static uint64_t s_last_csl_ie_tx_us; /* SFD time of our last frame with a CSL IE */
static struct {
    uint32_t n;
    int32_t min, max;
    int64_t sum;
    struct { int32_t dev_us; uint32_t since_sync_ms; int32_t lpclk_ppm; } ring[OT_RADIO_CSL_DEV_RING];
} s_dev = { 0, INT32_MAX, INT32_MIN, 0, {{0, 0, 0}} };

void ot_radio_csl_note_rx_on(bool deferred)
{
    int64_t lead;
    uint32_t per_us = s_sec.csl_period * 160u;

    if (!s_window_opening || s_sec.csl_period == 0) {
        return;          /* a Receive outside a window opening (after a TX, a poll) */
    }
    s_window_opening = false;
    /* OT opens the window (sample - ahead) after advancing the sample time
     * to the next period: this window's sample is one period earlier */
    s_window_sample_us = s_sec.csl_rx_time_us - per_us;
    lead = (int64_t)(s_sec.csl_rx_time_us - per_us - otPlatTimeGet());
    s_rx_lead.n++;
    s_rx_lead.deferred += deferred; /* the HAL left the receiver as it was (busy) */
    s_rx_lead.sum += lead;
    if (lead < s_rx_lead.min) s_rx_lead.min = (int32_t)lead;
    if (lead > s_rx_lead.max) s_rx_lead.max = (int32_t)lead;
}

void ot_radio_csl_note_rx_frame(const uint8_t *psdu, uint8_t len, uint64_t sfd_time)
{
    uint32_t per_us = s_sec.csl_period * 160u;
    int64_t dev;
    uint32_t i;

    /* a unicast (ACK requested) frame in an open window */
    if (s_sec.csl_period == 0 || len < 1 || (psdu[0] & 0x20u) == 0 || s_window_sample_us == 0) {
        return;
    }
    dev = (int64_t)(sfd_time + 32u - s_window_sample_us); /* + PHR: the MHR start */
    if (dev < -(int64_t)(per_us / 2u) || dev > (int64_t)(per_us / 2u)) {
        return;
    }
    i = s_dev.n % OT_RADIO_CSL_DEV_RING;
    s_dev.ring[i].dev_us = (int32_t)dev;
    s_dev.ring[i].since_sync_ms =
        (s_last_csl_ie_tx_us != 0) ? (uint32_t)((sfd_time - s_last_csl_ie_tx_us) / 1000u) : UINT32_MAX;
    s_dev.ring[i].lpclk_ppm = OT_RADIO_CSL_LPCLK_PPM();
    s_dev.n++;
    s_dev.sum += dev;
    if (dev < s_dev.min) s_dev.min = (int32_t)dev;
    if (dev > s_dev.max) s_dev.max = (int32_t)dev;
}

void ot_radio_csl_dump(bool clear)
{
    printf("[ot ] csl windows: rx on before the sample time min/avg/max %ld/%ld/%ld us (n=%lu, rx-on left to a busy "
           "receiver %lu, windows without rx-on %lu), tx secure fail %lu\n",
           (long)(s_rx_lead.n ? s_rx_lead.min : 0), (long)(s_rx_lead.n ? (int32_t)(s_rx_lead.sum / s_rx_lead.n) : 0),
           (long)(s_rx_lead.n ? s_rx_lead.max : 0), (unsigned long)s_rx_lead.n, (unsigned long)s_rx_lead.deferred,
           (unsigned long)s_rx_lead.no_rx, (unsigned long)s_sec_tx_fail);
    if (s_dev.n != 0) {
        uint32_t k = (s_dev.n < OT_RADIO_CSL_DEV_RING) ? s_dev.n : OT_RADIO_CSL_DEV_RING;

        printf("[ot ] csl arrival (MHR - sample): min/avg/max %ld/%ld/%ld us n=%lu | last (dev us @ ms since our "
               "CSL IE / sleep clock ppm):", (long)s_dev.min, (long)(s_dev.sum / (int64_t)s_dev.n), (long)s_dev.max,
               (unsigned long)s_dev.n);
        for (uint32_t j = 0; j < k; j++) {
            uint32_t idx = (s_dev.n - k + j) % OT_RADIO_CSL_DEV_RING;

            printf(" %ld@%lu/%ld", (long)s_dev.ring[idx].dev_us, (unsigned long)s_dev.ring[idx].since_sync_ms,
                   (long)s_dev.ring[idx].lpclk_ppm);
        }
        printf("\n");
    }
    if (clear) {
        s_dev.n = 0;
        s_dev.sum = 0;
        s_dev.min = INT32_MAX;
        s_dev.max = INT32_MIN;
        s_rx_lead.n = s_rx_lead.deferred = s_rx_lead.no_rx = 0;
        s_rx_lead.sum = 0; s_rx_lead.min = INT32_MAX; s_rx_lead.max = INT32_MIN;
    }
}

/* Radio time (otPlatTimeGet, 64 bit) of a 32-bit radio time near now. */
static uint64_t ot_radio_csl_time64(uint32_t t)
{
    uint64_t now = otPlatTimeGet();

    return now + (int64_t)(int32_t)(t - (uint32_t)now);
}

bool ot_radio_csl_needs_exact_time(const uint8_t *psdu, uint8_t len)
{
    struct ipro154_frame_info fi;

    return s_sec.csl_period != 0 && ipro154_frame_parse(psdu, len, &fi) == 0 && fi.csl_ie_off >= 0;
}

int ot_radio_csl_tx_prepare_at(otRadioFrame *aFrame, uint8_t *psdu, uint8_t len, uint64_t sfd_time)
{
    struct ipro154_frame_info fi;
    bool hdr_rdy;
    int rc;

    if (ipro154_frame_parse(psdu, len, &fi) != 0) {
        /* never send a frame the radio must secure in the clear */
        return (len > 0 && (psdu[0] & 0x08u) && !aFrame->mInfo.mTxInfo.mIsSecurityProcessed) ? -EINVAL : 0;
    }
    if (fi.csl_ie_off >= 0 && s_sec.csl_period != 0) {
        /* the phase for the frame's SFD time (radio time); 0 = not known,
         * then the typical CSMA delay from now */
        uint64_t t = sfd_time != 0 ? sfd_time : otPlatTimeGet() + OT_RADIO_CSL_TX_EST_US;

        ipro154_csl_ie_fill(psdu, &fi, &s_sec, t);
        s_last_csl_ie_tx_us = t;
    }
    if (!fi.sec_enabled || aFrame->mInfo.mTxInfo.mIsSecurityProcessed) {
        return 0;
    }
    /* A retransmission by the stack (a CSL or indirect frame to a sleepy
     * child) carries the first attempt's frame counter and key index: reuse
     * them, as OpenThread's SubMac does - with a new counter the child takes
     * the copy for a new frame (duplicate packets, 2026-09-29). */
    hdr_rdy = aFrame->mInfo.mTxInfo.mIsHeaderUpdated || aFrame->mInfo.mTxInfo.mIsARetx;
    if (hdr_rdy && fi.csl_ie_off >= 0 && s_sec.csl_period != 0) {
        /* a retransmission with a refreshed CSL IE: Thread 1.4.1 §3.2.6.3.7
         * wants it secured again with a new frame counter (same sequence
         * number); the new counter goes back into the stack's frame below */
        hdr_rdy = false;
    }
    rc = ipro154_sec_secure(&s_sec, psdu, len, &fi, s_ext_addr, hdr_rdy);
    if (rc != 0) {
        s_sec_tx_fail++;
        return rc;
    }
    if (!hdr_rdy) {
        /* keep the counter - and the key index, moved to the current key
         * after a key switch - in the stack's (plaintext) frame: retries of
         * a frame without CSL IE reuse them, and the stack reads them back as
         * the counter / key used */
        memcpy(&aFrame->mPsdu[fi.aux_off + 1], &psdu[fi.aux_off + 1], 5);
        aFrame->mInfo.mTxInfo.mIsHeaderUpdated = true;
    }
    return 0;
}

int ot_radio_csl_tx_prepare(otRadioFrame *aFrame, uint8_t *psdu, uint8_t len)
{
    return ot_radio_csl_tx_prepare_at(aFrame, psdu, len, 0);
}

int ot_radio_csl_prepare_exact(uint8_t *psdu, uint8_t len, uint64_t sfd_hal_us, void *ctx)
{
    uint64_t hal_now, ot_now;
    unsigned int key = ipro154_sec_lock();

    hal_now = ieee802154_hal_get_timestamp();
    ot_now = otPlatTimeGet();
    ipro154_sec_unlock(key);
    /* the HAL clock is the 32-bit symbol counter x 16 (wraps every 19.1 h):
     * its differences are right in 32 bits */
    return ot_radio_csl_tx_prepare_at((otRadioFrame *)ctx, psdu, len,
                                      ot_now + (int64_t)(int32_t)(uint32_t)(sfd_hal_us - hal_now));
}

/* Clock accuracy (ppm) and uncertainty (10 us units) this receiver announces
 * and sizes its sample windows with: OpenThread opens each window
 * elapsed x (ours + the parent's ppm) + both uncertainties around the sample
 * time, elapsed since the parent last synchronized on us.  Bench knobs
 * (`radio_csl_win`); 255 / 255 as OpenThread's defaults. */
volatile uint8_t ot_radio_csl_accuracy_ppm = 255u;
volatile uint8_t ot_radio_csl_uncertainty_10us = 255u;

uint8_t otPlatRadioGetCslAccuracy(otInstance *aInstance)
{
    (void)aInstance;
    return ot_radio_csl_accuracy_ppm;
}

uint8_t otPlatRadioGetCslUncertainty(otInstance *aInstance)
{
    (void)aInstance;
    return ot_radio_csl_uncertainty_10us;
}

otError otPlatRadioEnableCsl(otInstance *aInstance, uint32_t aCslPeriod, otShortAddress aShortAddr,
                             const otExtAddress *aExtAddr)
{
    (void)aInstance;
    (void)aShortAddr;
    (void)aExtAddr;

    /* aCslPeriod is already in units of 10 symbols */
    s_sec.csl_period = aCslPeriod;
    ieee802154_hal_set_csl_ie((uint16_t)aCslPeriod, 0);
    return OT_ERROR_NONE;
}

bool ot_radio_csl_next_sample_us(uint32_t *out)
{
    if ((s_sec.csl_period == 0U) || (s_sec.csl_rx_time_us == 0U)) {
        return false;
    }
    *out = (uint32_t)s_sec.csl_rx_time_us;
    return true;
}

void otPlatRadioUpdateCslSampleTime(otInstance *aInstance, uint32_t aCslSampleTime)
{
    uint32_t hal_now;
    uint32_t ot_now;

    (void)aInstance;
    if (s_window_opening) {
        s_rx_lead.no_rx++;   /* the previous window opened without a Receive (radio busy / other state) */
    }
    s_window_opening = true;
    s_sec.csl_rx_time_us = ot_radio_csl_time64(aCslSampleTime);
    /* the HAL builds the Enh-ACK's CSL IE on its own (symbol) clock; read
     * both clocks back to back, an interrupt in between would skew it */
    {
        unsigned int key = ipro154_sec_lock();

        hal_now = (uint32_t)ieee802154_hal_get_timestamp();
        ot_now = (uint32_t)otPlatTimeGet();
        ipro154_sec_unlock(key);
    }
    ieee802154_hal_set_csl_ie((uint16_t)s_sec.csl_period, hal_now + (aCslSampleTime - ot_now));
}

static const uint8_t *ot_radio_csl_key(const otMacKeyMaterial *aKey, otRadioKeyType aKeyType, uint8_t buf[16])
{
#if OPENTHREAD_CONFIG_PLATFORM_KEY_REFERENCES_ENABLE
    if (aKeyType == OT_KEY_TYPE_KEY_REF) {
        size_t len = 0;

        if (otPlatCryptoExportKey(aKey->mKeyMaterial.mKeyRef, buf, 16, &len) != OT_ERROR_NONE || len != 16) {
            return NULL;
        }
        return buf;
    }
#else
    (void)aKeyType;
    (void)buf;
#endif
    return aKey->mKeyMaterial.mKey.m8;
}

void otPlatRadioSetMacKey(otInstance *aInstance, uint8_t aKeyIdMode, uint8_t aKeyId, const otMacKeyMaterial *aPrevKey,
                          const otMacKeyMaterial *aCurrKey, const otMacKeyMaterial *aNextKey, otRadioKeyType aKeyType)
{
    const otMacKeyMaterial *keys[3] = {aCurrKey, aPrevKey, aNextKey};
    uint8_t index[3];
    uint8_t buf[16];

    (void)aInstance;
    (void)aKeyIdMode; /* key ID mode 1 only (Thread) */
    index[0] = aKeyId;
    index[1] = (aKeyId == 1u) ? OT_RADIO_KEY_INDEX_MAX : (uint8_t)(aKeyId - 1u);
    index[2] = (aKeyId == OT_RADIO_KEY_INDEX_MAX) ? 1u : (uint8_t)(aKeyId + 1u);
    static struct ipro154_sec_key made[IPRO154_SEC_MAX_KEYS]; /* ~540 B: off the OT task stack */

    for (unsigned int i = 0; i < IPRO154_SEC_MAX_KEYS; i++) {
        const uint8_t *k = keys[i] != NULL ? ot_radio_csl_key(keys[i], aKeyType, buf) : NULL;

        (void)ipro154_sec_key_make(&made[i], index[i], k); /* invalid on failure: its frames fail (counted) */
    }
    ipro154_sec_keys_commit(&s_sec, made, aKeyId);
}

void otPlatRadioSetMacFrameCounter(otInstance *aInstance, uint32_t aMacFrameCounter)
{
    unsigned int key = ipro154_sec_lock();

    (void)aInstance;
    ipro154_sec_set_frame_counter(&s_sec, aMacFrameCounter, false);
    ipro154_sec_unlock(key);
}

void otPlatRadioSetMacFrameCounterIfLarger(otInstance *aInstance, uint32_t aMacFrameCounter)
{
    unsigned int key = ipro154_sec_lock();

    (void)aInstance;
    ipro154_sec_set_frame_counter(&s_sec, aMacFrameCounter, true);
    ipro154_sec_unlock(key);
}

#endif /* OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE */
