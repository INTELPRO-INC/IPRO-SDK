/**
 * @file ot_radio_ieee802154.c
 * @brief OpenThread Radio Platform Implementation using IEEE 802.15.4 Driver
 * 
 * This file implements the OpenThread platform radio abstraction layer
 * using the unified IEEE 802.15.4 driver API.
 * 
 * Architecture:
 *   OpenThread → ieee802154.c (Driver) → ieee802154_hal_ipro7.c (HAL) → Hardware
 * 
 * @copyright Copyright (c) 2025 INTELPRO Inc.
 */

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <ipro_log.h>

/* IEEE 802.15.4 unified driver */
#include "ieee802154.h"
#include "ieee802154_hal.h"   /* ieee802154_hal_transmit_exact */
/* LP-sleep quiescence holdoff (no-op stub when CONFIG_THREAD_LP_SLEEP=n) */
#include "lp_sleep.h"
/* Platform-specific includes */
#ifdef CONFIG_IPRO7
#include <ipro7.h>
#include <system_soc.h>
#include <hal_sys.h>

/*
 * IPRO7 RF init pattern — required for ZB TX to land at correct PA bias.
 * Without these, rf_set_tx_power_zb() is called by the lmac154 init path
 * with .noinit_data garbage values for tx_target_power_zb /
 * tx_pwr_offset_zb / tx_pwr_os_zb_mfg, the index calc clamps to 0 (max
 * PA bias), and TX at the configured 0 dBm draws ~60 mA instead of
 * ~10 mA. Verified on hardware 2026-04-30 in apps/networking/thread/
 * ipro_ieee154_sed_lp. See memory/ipro7_zb_tx_power_init.md for the
 * full root-cause writeup.
 */
extern void rf_set_bz_mode(uint8_t mode);                  /* 1 = MODE_ZB_ONLY */
extern void rf_set_pwr_offset_zb(int8_t tx_pwr_os[16]);    /* MFG offset table */
extern void rf_optimize_by_channel(uint32_t channel_mhz);  /* derive tx_pwr_offset_zb */
extern uint8_t rf_set_tx_power_zb(int32_t target_power);   /* program PA */
#define IPRO7_RF_MODE_ZB_ONLY  1
#endif

/* FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"

/* OpenThread */
#include <openthread-core-config.h>
#include <openthread/platform/diag.h>
#include <openthread/platform/radio.h>
#include <openthread/platform/time.h>
#include <openthread/tasklet.h>
#include <openthread_port.h>
#include "ot_pm.h"
#include "ot_radio_csl.h"
#if defined(CONFIG_COEX_BZ)
#include "coex_bz.h"
#endif

extern bool ieee802154_hal_rx_busy(void); /* a frame or our Enh-ACK is on the air */

/* OpenThread platform utilities */
#include "utils/mac_frame.h"

#ifdef CONFIG_SHELL
#include "shell.h"
#include <stdlib.h>     /* atoi */
#endif

#define TAG "OT-Radio-802154"

/*===========================================================================
 * Constants
 *===========================================================================*/

#define OTRADIO_MAX_PSDU                    IEEE802154_MAX_PHY_PACKET_SIZE
#define OTRADIO_RX_FRAME_BUFFER_NUM         16
#define OT_RADIO_2P4GHZ_OQPSK_CHANNEL_MIN   11
#define OT_RADIO_SYMBOL_TIME                16  /* Symbol time in µs */
#define OT_RADIO_SRC_MATCH_SHORT_ENTRIES    16  /* Max short address entries */
#define OT_RADIO_SRC_MATCH_EXT_ENTRIES      16  /* Max extended address entries */

/*===========================================================================
 * Data Structures
 *===========================================================================*/

typedef struct _otRadio_rxFrame_t {
    struct _otRadio_rxFrame_t *next;
    otRadioFrame frame;
} otRadio_rxFrame_t;

#define ALIGNED_RX_FRAME_SIZE ((sizeof(otRadio_rxFrame_t) + 3) & ~3)
#define TOTAL_RX_FRAME_SIZE   (ALIGNED_RX_FRAME_SIZE + OTRADIO_MAX_PSDU)

typedef struct {
    otInstance *aInstance;
    otRadio_rxFrame_t *rxFrameList;     /* Received frames pending */
    otRadio_rxFrame_t *freeFrameList;   /* Free frame buffers */
    otRadioFrame *pTxFrame;             /* Current TX frame */
    otRadioFrame *pAckFrame;            /* ACK frame storage */
    
    uint64_t tstx;                      /* TX timestamp */
    uint64_t tsIsr;                     /* ISR timestamp */
    
    /* Statistics */
    uint32_t dbgRxFrameNum;
    uint32_t dbgFrameNum;
    uint32_t dbgMaxPendingFrameNum;
    
    /* Frame buffers */
    uint8_t buffPool[TOTAL_RX_FRAME_SIZE * (OTRADIO_RX_FRAME_BUFFER_NUM + 2)];
    
    /* IEEE 802.15.4 driver state */
    ieee802154_config_t ieee802154_config;
    bool radio_enabled;
    uint8_t current_channel;
    int8_t current_tx_power;
    
    /* Source Address Match Table */
    bool src_match_enabled;
    otShortAddress src_match_short[OT_RADIO_SRC_MATCH_SHORT_ENTRIES];
    bool src_match_short_valid[OT_RADIO_SRC_MATCH_SHORT_ENTRIES];
    otExtAddress src_match_ext[OT_RADIO_SRC_MATCH_EXT_ENTRIES];
    bool src_match_ext_valid[OT_RADIO_SRC_MATCH_EXT_ENTRIES];
} otRadio_t;

/* Pinned into retained OCRAM (.ocram_bss), NOT PSRAM. s_radio_state holds the
 * 802.15.4 TX/RX/ACK frame buffers (buffPool) — otPlatRadioGetTransmitBuffer
 * hands OpenThread a pointer INTO this struct, so a PSRAM warm-wake corruption
 * of this region surfaces as "ieee802154_transmit ch=0 len=0" (a zeroed TX
 * frame) and the downstream OT-MAC wedge. The broad PSRAM probe reads clean
 * (flips=0) yet this region still got hit -> address-localized retention; move
 * the radio frame buffers off PSRAM entirely. RX fills buffPool via CPU memcpy
 * (ieee802154_hal_lmac154.c:614), not DMA, so OCRAM placement is safe. ~3.5KB;
 * OCRAM has ample room. .ocram_bss is zero-cleared at startup + HW-retained
 * across PDS31 (no NORET split). */
static __attribute__((section(".ocram_bss"))) otRadio_t s_radio_state;

/*===========================================================================
 * Debug instrumentation — OT-layer counters.
 *
 * Each counter is bumped at the OT-adapter layer. Compare against
 * ieee802154_hal_dbg_dump() to spot HAL→OT forwarding gaps. All
 * increments are in ISR or task context (no logging in either path);
 * dump from the shell command radio_stats.
 *===========================================================================*/
typedef struct {
    /* otPlatRadioTransmit() entries */
    volatile uint32_t tx_submit;            /* OT asked us to transmit */
    volatile uint32_t tx_submit_busy;       /* prev TX still in progress */
    volatile uint32_t tx_submit_hal_fail;   /* ieee802154_transmit() rejected */
    volatile uint32_t tx_ack_requested;     /* AR=1 in submitted frame */
    volatile uint32_t tx_no_ack_request;    /* AR=0 (broadcast/multicast) */

    /* TX completion paths reaching OT (from ISR) */
    volatile uint32_t tx_done_acked;        /* TX_DONE with ack_requested=true */
    volatile uint32_t tx_done_no_ack_req;   /* TX_DONE with ack_requested=false */
    volatile uint32_t tx_no_ack;            /* TX_NO_ACK */
    volatile uint32_t tx_done_with_frame;   /* HAL passed ACK frame body */
    volatile uint32_t rx_on_after_fp_ack;   /* RX switched on at an FP=1 ACK (CSL receiver) */
    volatile uint32_t tx_done_without_frame;/* HAL passed NULL → simple ACK */

    /* RX paths (ISR) */
    volatile uint32_t rx_done_isr;          /* HAL handed us an RX frame */
    volatile uint32_t rx_no_buf;            /* freelist empty, frame dropped */

    /* RX paths (task) */
    volatile uint32_t rx_delivered_to_ot;   /* otPlatRadioReceiveDone called */

    /* otPlatRadioReceive / Sleep / channel changes */
    volatile uint32_t rx_enable_calls;
    volatile uint32_t rx_channel_changes;
    volatile uint32_t sleep_calls;

    /* CCA failure — TX never went out (was previously silently swallowed
     * for broadcast frames). Now surfaced to OT as ChannelAccessFailure. */
    volatile uint32_t tx_csma_fail_at_ot;
} ot_radio_dbg_t;

/* Diagnostic: when set, otPlatRadioTransmit forces csma_ca_enabled=0 in
 * the HAL frame regardless of what OT requested. This bypasses the
 * lower-MAC CSMA-CA sequencer and the chip's CCA hardware entirely —
 * the radio just transmits immediately. If toggling this fixes the
 * CSMA failure storm, the lmac154 CSMA-CA hardware is the root cause
 * and we need to chase that. NEVER ship with this on (violates 15.4
 * spec, will collide with other devices). */
static volatile bool s_force_no_csma = false;

static ot_radio_dbg_t s_ot_dbg;

/* Timed (CSL) transmit: submit time - target SFD time (mTxDelayBaseTime +
 * mTxDelay, radio time), and how many asked for CSMA (overridden below). */
static struct {
    uint32_t n, csma;
    int32_t last, min, max;
    uint32_t hist[6]; /* <-1ms, <0, <1ms, <5ms, <20ms, >= */
} s_txtime_dbg;

/* Forward declare HAL dump helper (defined in ieee802154_hal_lmac154.c) */
extern void ieee802154_hal_dbg_dump(void);
extern void ieee802154_hal_dbg_reset(void);

void ot_radio_dbg_dump(void)
{
    /* called from non-ISR (shell context) */
#if defined(CONFIG_THREAD_RCP_SOFTWARE_TIMING)
    extern void ot_usalarm_dbg_dump(void);
    ot_usalarm_dbg_dump();
#endif
    printf("[ot ] timed tx: n=%lu csma_req=%lu submit-target last/min/max=%ld/%ld/%ld us"
           "  hist <-1ms/<0/<1ms/<5ms/<20ms/>=: %lu/%lu/%lu/%lu/%lu/%lu\n",
           (unsigned long)s_txtime_dbg.n, (unsigned long)s_txtime_dbg.csma,
           (long)s_txtime_dbg.last, (long)s_txtime_dbg.min, (long)s_txtime_dbg.max,
           (unsigned long)s_txtime_dbg.hist[0], (unsigned long)s_txtime_dbg.hist[1],
           (unsigned long)s_txtime_dbg.hist[2], (unsigned long)s_txtime_dbg.hist[3],
           (unsigned long)s_txtime_dbg.hist[4], (unsigned long)s_txtime_dbg.hist[5]);
    printf("[ot ] tx submit=%lu busy=%lu halfail=%lu  ack_req=%lu no_ack_req=%lu\n",
           (unsigned long)s_ot_dbg.tx_submit,
           (unsigned long)s_ot_dbg.tx_submit_busy,
           (unsigned long)s_ot_dbg.tx_submit_hal_fail,
           (unsigned long)s_ot_dbg.tx_ack_requested,
           (unsigned long)s_ot_dbg.tx_no_ack_request);
    printf("[ot ] tx done: acked=%lu no_ack_req=%lu  no_ack=%lu  with_frame=%lu without=%lu  rx_on_at_fp_ack=%lu\n",
           (unsigned long)s_ot_dbg.tx_done_acked,
           (unsigned long)s_ot_dbg.tx_done_no_ack_req,
           (unsigned long)s_ot_dbg.tx_no_ack,
           (unsigned long)s_ot_dbg.tx_done_with_frame,
           (unsigned long)s_ot_dbg.tx_done_without_frame,
           (unsigned long)s_ot_dbg.rx_on_after_fp_ack);
    printf("[ot ] rx: isr=%lu nobuf=%lu task_to_ot=%lu  rx_en=%lu chg_ch=%lu sleep=%lu\n",
           (unsigned long)s_ot_dbg.rx_done_isr,
           (unsigned long)s_ot_dbg.rx_no_buf,
           (unsigned long)s_ot_dbg.rx_delivered_to_ot,
           (unsigned long)s_ot_dbg.rx_enable_calls,
           (unsigned long)s_ot_dbg.rx_channel_changes,
           (unsigned long)s_ot_dbg.sleep_calls);
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    ot_radio_csl_dump(false);
#endif
    printf("[ot ] state: ch=%u tx_pwr=%d enabled=%d pTxFrame=%p panid=%04x short=%04x\n",
           (unsigned)s_radio_state.current_channel,
           (int)s_radio_state.current_tx_power,
           (int)s_radio_state.radio_enabled,
           (void *)s_radio_state.pTxFrame,
           (unsigned)s_radio_state.ieee802154_config.pan_id,
           (unsigned)s_radio_state.ieee802154_config.short_addr);

    ieee802154_hal_dbg_dump();
}

void ot_radio_dbg_reset(void)
{
    memset((void *)&s_ot_dbg, 0, sizeof(s_ot_dbg));
    ieee802154_hal_dbg_reset();
}

/*===========================================================================
 * Forward Declarations
 *===========================================================================*/

static void ot_ieee802154_event_handler(ieee802154_event_type_t event,
                                         ieee802154_frame_t *frame,
                                         void *context);

/*===========================================================================
 * Frame List Management
 *===========================================================================*/

static otRadio_rxFrame_t* frame_list_pop(otRadio_rxFrame_t **list)
{
    otRadio_rxFrame_t *frame = *list;
    if (frame) {
        *list = frame->next;
        frame->next = NULL;
    }
    return frame;
}

static void frame_list_push(otRadio_rxFrame_t **list, otRadio_rxFrame_t *frame)
{
    frame->next = *list;
    *list = frame;
}

static void frame_list_push_tail(otRadio_rxFrame_t **list, otRadio_rxFrame_t *frame)
{
    frame->next = NULL;
    if (*list == NULL) {
        *list = frame;
    } else {
        otRadio_rxFrame_t *last = *list;
        while (last->next) {
            last = last->next;
        }
        last->next = frame;
    }
}

/*===========================================================================
 * Initialization
 *===========================================================================*/

void ot_radioInit(otInstance *aInstance)
{
    otRadio_rxFrame_t *pframe = NULL;
    
    memset(&s_radio_state, 0, offsetof(otRadio_t, buffPool));
    
    /* Register event callback */
    ieee802154_error_t err = ieee802154_register_callback(ot_ieee802154_event_handler, aInstance);
    if (err != IEEE802154_OK) {
        IPRO_LOGE(TAG, "Callback registration failed: %d", err);
    }   


    /* Setup ACK frame buffer */
    s_radio_state.pAckFrame = (otRadioFrame *)(s_radio_state.buffPool + TOTAL_RX_FRAME_SIZE);
    s_radio_state.pAckFrame->mPsdu = ((uint8_t *)s_radio_state.pAckFrame) + ALIGNED_RX_FRAME_SIZE;
    
    /* Initialize frame lists */
    OT_ENTER_CRITICAL();
    s_radio_state.rxFrameList = NULL;
    s_radio_state.freeFrameList = NULL;
    
    for (int i = 0; i < OTRADIO_RX_FRAME_BUFFER_NUM; i++) {
        pframe = (otRadio_rxFrame_t *)(s_radio_state.buffPool + TOTAL_RX_FRAME_SIZE * (i + 2));
        pframe->frame.mPsdu = ((uint8_t *)pframe) + ALIGNED_RX_FRAME_SIZE;
        frame_list_push(&s_radio_state.freeFrameList, pframe);
    }
    
    s_radio_state.dbgFrameNum = OTRADIO_RX_FRAME_BUFFER_NUM;
    OT_EXIT_CRITICAL();

    /* Initialize IEEE 802.15.4 config with defaults */
    s_radio_state.ieee802154_config.channel = 11;
    s_radio_state.ieee802154_config.tx_power = 0;
    s_radio_state.ieee802154_config.pan_id = 0xFFFF;
    s_radio_state.ieee802154_config.short_addr = 0xFFFF;
    memset(s_radio_state.ieee802154_config.ext_addr.m8, 0xFF, 8);
    s_radio_state.ieee802154_config.promiscuous = false;
    s_radio_state.ieee802154_config.auto_ack = true;
    s_radio_state.ieee802154_config.cca_mode = IEEE802154_CCA_MODE_ENERGY;
    s_radio_state.ieee802154_config.cca_threshold = -75;

}

/*===========================================================================
 * Re-init for low-power wake (PDS31 power-cycles M154)
 *
 * After PDS31, the M154 baseband is in POR — channel / PAN ID / short
 * address / ext address / filter bits / CCA mode are all wiped, the
 * NBR / FPT tables hold POR garbage, and (critically) the per-IRQ ECLIC
 * IP registers for M154_IRQn have lost their handler binding.
 *
 * We split the work in two:
 *
 *  1. ot_radio_reinit_for_lp_wake(): called from the LP wakeup hook
 *     (FreeRTOS scheduler is suspended). Just sets a flag — the body
 *     of ieee802154_reset() ends up calling lmac154_init() which, at
 *     module-init level, may want to call into FreeRTOS-protected
 *     paths (heap, vTaskDelay-style busy-waits inside RF cal, log
 *     mutex inside subroutines). All of those are unsafe in
 *     suspended-scheduler context, and empirically crashed the CPU
 *     with PC=0x200ddeac (jumped to garbage memory because some
 *     blocking primitive returned wrong values).
 *
 *  2. ot_radio_drain_pending_reinit(): called from the OT task at the
 *     top of every wake before otTaskletsProcess(). FreeRTOS is fully
 *     running, mutexes work, lmac154_init is happy. This is where the
 *     real ieee802154_reset() runs and re-applies the cached config.
 *
 * Between (1) and (2), the M154 register file is at POR (PDS power-
 * cycled it). RX is off because lmac154_disableRx() was called before
 * sleep and POR M154_INT_EN is 0, so no spurious IRQs are possible —
 * the deferred reset window is safe.
 *===========================================================================*/
static volatile bool s_radio_reinit_pending = false;
static volatile bool s_radio_reinit_keep_rf = false;

ieee802154_error_t ot_radio_reinit_for_lp_wake(void)
{
    s_radio_reinit_pending = true;
    return IEEE802154_OK;
}

/* the same MAC reset, the RF / BZ PHY left alone: BLE is live on it */
ieee802154_error_t ot_radio_reinit_for_lp_wake_keep_rf(void)
{
    s_radio_reinit_keep_rf = true;
    s_radio_reinit_pending = true;
    return IEEE802154_OK;
}

#if defined(CONFIG_IPRO_PM_THREAD)
/*===========================================================================
 * Platform PDS under ipro_pm (ot_pm.c)
 *
 * The idle-task executor keeps the MISC domain (the MAC and its register
 * file) alive but lets PDS power the RF down.  After such a sleep the RF is
 * warmed up again from this task, before the next radio operation: the wake
 * path only flags it (IRQs masked, scheduler suspended).
 *===========================================================================*/
static volatile bool s_rf_restore_pending = false;
volatile uint32_t ot_radio_rf_restores;

static bool ot_radio_scanning(void); /* energy scan state is further down */

uint32_t ot_radio_pm_busy(void)
{
    uint32_t why = 0u;

    if (s_radio_state.pTxFrame != NULL) {
        why |= OT_RADIO_PM_BUSY_TX;
    }
    if (s_radio_state.rxFrameList != NULL) {
        why |= OT_RADIO_PM_BUSY_RX_QUEUE;
    }
    if (ot_radio_scanning()) {
        why |= OT_RADIO_PM_BUSY_SCAN;
    }
    if (s_radio_reinit_pending || s_rf_restore_pending) {
        why |= OT_RADIO_PM_BUSY_RESTORE;
    }
    return why;
}

bool ot_radio_pm_enabled(void)
{
    return s_radio_state.radio_enabled;
}

void ot_radio_pm_request_rf_restore(void)
{
    s_rf_restore_pending = true;
}

/* The 802.15.4 PA setup otPlatRadioEnable() applies after the RF is up, on
 * the current channel and TX power (partial re-application leaves the PA bias
 * wrong).  Register writes only: safe with interrupts masked. */
void ot_radio_pm_rf_zb_config(void)
{
    rf_set_bz_mode(IPRO7_RF_MODE_ZB_ONLY);
    {
        int8_t zero_zb_offsets[16] = {0};
        rf_set_pwr_offset_zb(zero_zb_offsets);
    }
    rf_optimize_by_channel(2405u + 5u * (uint32_t)
        (s_radio_state.current_channel - OT_RADIO_2P4GHZ_OQPSK_CHANNEL_MIN));
    (void)rf_set_tx_power_zb((int32_t)s_radio_state.current_tx_power);
}

static void ot_radio_rf_warm_restore(void)
{
    extern void rf_init(uint32_t xtalfreq_hz);

    rf_init(0);
    ot_radio_pm_rf_zb_config();
    /* CCA mode / ED threshold live in the BZ PHY the PDS reset */
    ieee802154_hal_phy_restore();
#if defined(CONFIG_IPRO7_XTAL_CAPCODE) && (CONFIG_IPRO7_XTAL_CAPCODE > 0)
    {
        extern int AON_Set_Xtal_CapCode(uint8_t capIn, uint8_t capOut);

        (void)AON_Set_Xtal_CapCode(CONFIG_IPRO7_XTAL_CAPCODE, CONFIG_IPRO7_XTAL_CAPCODE);
    }
#endif
    ot_radio_rf_restores++;
}
#endif /* CONFIG_IPRO_PM_THREAD */

bool ot_radio_drain_pending_reinit(void)
{
#if defined(CONFIG_IPRO_PM_THREAD)
    if (s_rf_restore_pending) {
        s_rf_restore_pending = false;
        ot_radio_rf_warm_restore();
    }
#endif
    if (!s_radio_reinit_pending) {
        return false;
    }
    s_radio_reinit_pending = false;
    ieee802154_hal_keep_rf_on_reset(s_radio_reinit_keep_rf);
    s_radio_reinit_keep_rf = false;

    /* Full HAL reset: lmac154_init (RF PHY, M154 clock, module init,
     * IRQ enable), FPT clear, frame filter, ack event, ECLIC re-register
     * for M154_IRQn. Then re-apply the cached channel / PAN / short /
     * ext / CCA / auto_ack from g_driver.config — the public
     * ieee802154_set_*() API keeps that in sync with whatever OT has
     * assigned during MLE attach. */
    (void)ieee802154_reset();
    ieee802154_hal_keep_rf_on_reset(false);
#if defined(CONFIG_COEX_BZ)
    /* the MAC reset left COEX_PRIORITY at its reset value */
    (void)coex_bz_resume();
#endif

    /* hal_reset wipes hal_state.callback during lmac154_init's reset
     * path, so re-bind the OT-layer event handler. */
    (void)ieee802154_register_callback(ot_ieee802154_event_handler,
                                        s_radio_state.aInstance);
    return true;
}

/*===========================================================================
 * Radio Task (Event Processing)
 *===========================================================================*/

void ot_radioTask(ot_system_event_t trxEvent)
{
    otRadio_rxFrame_t *pframe;
    otRadioFrame *txframe;

    /* Periodic radio monitor - recover from stuck RX state.
     * Called every task loop iteration (~5ms). The monitor itself
     * has a 5-second internal timeout so this is very lightweight. */
    extern void ieee802154_hal_monitor(void);
    ieee802154_hal_monitor();

    /* TX watchdog. tstx is armed in otPlatRadioTransmit (deadline = submit +
     * 1000 ms) but was never checked anywhere — it was dead code. A TX that
     * gets no terminal HAL event (seen on the Joiner's first pre-attach unicast
     * DTLS ClientHello in Joiner::Connect) strands pTxFrame forever; OT's SubMac
     * then waits on an otPlatRadioTxDone that never comes and the whole join
     * hangs. The monitor above only recovers stuck RX, never stuck TX. If a TX
     * is past its deadline, abort it: ABORT lets OT retry, and exhausted retries
     * surface as a visible failure instead of a dead device. Runs every loop
     * (before the no-event early-return below). */
    if (s_radio_state.pTxFrame != NULL && s_radio_state.tstx != 0 &&
        s_radio_state.aInstance != NULL &&
        (otPlatTimeGet() / 1000) > s_radio_state.tstx) {
        otRadioFrame *stuck = s_radio_state.pTxFrame;
        s_radio_state.pTxFrame = NULL;
        s_radio_state.tstx = 0;
        s_ot_dbg.tx_submit_hal_fail++;
        printf("[ot ] TX watchdog: stuck TX aborted (no TxDone within 1s)\r\n");
        extern void ieee802154_hal_clear_tx_pending(void);
        ieee802154_hal_clear_tx_pending();
        ieee802154_receive();
        otPlatRadioTxDone(s_radio_state.aInstance, stuck, NULL, OT_ERROR_ABORT);
    }

    if (!(OT_SYSTEM_EVENT_RADIO_ALL_MASK & trxEvent)) {
        return;
    }
    
    /* Handle TX events */
    if (s_radio_state.pTxFrame && (OT_SYSTEM_EVENT_RADIO_TX_ALL_MASK & trxEvent)) {
        txframe = s_radio_state.pTxFrame;
        s_radio_state.pTxFrame = NULL;
        s_radio_state.tstx = 0;

        /* A finished transmission leaves the radio receiving (the platform
         * contract), BEFORE the stack hears of it: in otPlatRadioTxDone() a
         * sleepy device puts the radio to sleep, and a receive() after it
         * left the receiver on until the next operation (seconds, on an
         * SED or SSED). */
        ieee802154_receive();

        if (trxEvent & OT_SYSTEM_EVENT_RADIO_TX_DONE_NO_ACK_REQ) {
            otPlatRadioTxDone(s_radio_state.aInstance, txframe, NULL, OT_ERROR_NONE);
        } else if (trxEvent & OT_SYSTEM_EVENT_RADIO_TX_ACKED) {
            otPlatRadioTxDone(s_radio_state.aInstance, txframe, s_radio_state.pAckFrame, OT_ERROR_NONE);
        } else if (trxEvent & OT_SYSTEM_EVENT_RADIO_TX_NO_ACK) {
            otPlatRadioTxDone(s_radio_state.aInstance, txframe, NULL, OT_ERROR_NO_ACK);
        } else if (trxEvent & OT_SYSTEM_EVENT_RADIO_TX_ERROR) {
            /* CCA-busy: TX never went out. OT's SubMac will increment its
             * CSMA backoff counter and retry up to MaxCsmaBackoffs. */
            otPlatRadioTxDone(s_radio_state.aInstance, txframe, NULL,
                              OT_ERROR_CHANNEL_ACCESS_FAILURE);
        }
    }
    
    /* Handle RX events */
    if (trxEvent & OT_SYSTEM_EVENT_RADIO_RX_DONE) {
        pframe = NULL;

        OT_ENTER_CRITICAL();
        pframe = frame_list_pop(&s_radio_state.rxFrameList);
        if (pframe) {
            s_radio_state.dbgRxFrameNum--;
        }
        OT_EXIT_CRITICAL();

        if (pframe) {
            s_ot_dbg.rx_delivered_to_ot++;
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
            ot_radio_csl_note_rx_frame(pframe->frame.mPsdu, (uint8_t)pframe->frame.mLength,
                                       pframe->frame.mInfo.mRxInfo.mTimestamp);
#endif
            /* Hold deep-PDS off for the quiescence holdoff after any received
             * downlink, so a multi-step exchange (CASE Sigma1/2/3) stays on WFI
             * and the next message is retrievable even before CHIP flips active.
             * No-op when LP sleep isn't compiled in. */
            lp_sleep_note_rx_activity();
            otPlatRadioReceiveDone(s_radio_state.aInstance, &pframe->frame, OT_ERROR_NONE);

            OT_ENTER_CRITICAL();
            frame_list_push(&s_radio_state.freeFrameList, pframe);
            s_radio_state.dbgFrameNum++;
            OT_EXIT_CRITICAL();

            /* Check for more pending frames */
            if (s_radio_state.rxFrameList != NULL) {
                OT_NOTIFY(OT_SYSTEM_EVENT_RADIO_RX_DONE);
            }
        }
    } else if (trxEvent & OT_SYSTEM_EVENT_RADIO_RX_NO_BUFF) {
        otPlatRadioReceiveDone(s_radio_state.aInstance, NULL, OT_ERROR_NO_BUFS);
    }
}

/* Radio time is otPlatTimeGet() (the mtimer): an RCP host syncs its radio
 * clock to it (SPINEL_PROP_RCP_TIMESTAMP) and sends CSL transmit targets in
 * it, and the sub-MAC's microsecond alarm runs on it. The HAL timestamps
 * frames on the MAC symbol counter - a different clock and epoch - so they
 * are moved onto the mtimer here, from a pair of readings of both clocks. */
extern uint64_t ieee802154_hal_get_timestamp(void); /* symbol counter x 16 */

static uint64_t ot_radio_hal_to_time(uint32_t hal_us)
{
    UBaseType_t key = OT_ENTER_CRITICAL_ISR();
    uint32_t hal_now = (uint32_t)ieee802154_hal_get_timestamp();
    uint64_t now = otPlatTimeGet();

    OT_EXIT_CRITICAL_ISR(key);
    return now - (uint32_t)(hal_now - hal_us);
}

/*===========================================================================
 * IEEE 802.15.4 Event Handler (called from ISR context)
 *===========================================================================*/

static void ot_ieee802154_event_handler(ieee802154_event_type_t event,
                                         ieee802154_frame_t *frame,
                                         void *context)
{
    (void)context;
    
    /* NOTE: This handler runs in ISR context - NO IPRO_LOG calls (log mutex assert) */

    switch (event) {
        case IEEE802154_EVENT_TX_DONE:
            if (s_radio_state.pTxFrame) {
                bool ack_requested = (s_radio_state.pTxFrame->mPsdu[0] & (1 << 5)) != 0;

                if (ack_requested) {
                    s_ot_dbg.tx_done_acked++;
                    /* Populate ACK frame data if HAL provided it
                     * (like lmac154 ackFrameEvent populates pAckFrame) */
                    if (frame && frame->length > 0 && s_radio_state.pAckFrame) {
                        s_ot_dbg.tx_done_with_frame++;
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
                        /* A sleepy node's ACK with Frame Pending: the parent
                         * sends the queued frame 2-8 ms later. The sleep path
                         * left the receiver off after the ACK; switch it on
                         * here rather than when the OT task gets to the TX
                         * done event, which can be after the frame (bench
                         * 2026-09-25: ~5 % of those frames missed). */
                        if (frame->info.rx.frame_pending) {
                            s_ot_dbg.rx_on_after_fp_ack++;
                            (void)ieee802154_receive();
                        }
#endif
                        memcpy(s_radio_state.pAckFrame->mPsdu, frame->psdu, frame->length);
                        s_radio_state.pAckFrame->mLength = frame->length;
                        s_radio_state.pAckFrame->mChannel = s_radio_state.current_channel;
                        s_radio_state.pAckFrame->mInfo.mRxInfo.mRssi = frame->info.rx.rssi;
                        s_radio_state.pAckFrame->mInfo.mRxInfo.mLqi = frame->info.rx.lqi;
                        s_radio_state.pAckFrame->mInfo.mRxInfo.mAckedWithFramePending =
                            frame->info.rx.frame_pending;
                        /* CSL: the transmitter anchors the phase in the
                         * ACK's CSL IE to the ACK's timestamp */
                        s_radio_state.pAckFrame->mInfo.mRxInfo.mTimestamp =
                            frame->info.rx.timestamp != 0
                                ? ot_radio_hal_to_time((uint32_t)frame->info.rx.timestamp) : 0;
                    } else {
                        s_ot_dbg.tx_done_without_frame++;
                    }
                    OT_NOTIFY_ISR(OT_SYSTEM_EVENT_RADIO_TX_ACKED);
                } else {
                    s_ot_dbg.tx_done_no_ack_req++;
                    OT_NOTIFY_ISR(OT_SYSTEM_EVENT_RADIO_TX_DONE_NO_ACK_REQ);
                }
            }
            break;

        case IEEE802154_EVENT_TX_NO_ACK:
            /* If the frame never requested an ACK (broadcast/multicast),
             * a NO_ACK from lmac154 is spurious — treat as success.
             * This guards against the lmac154 race condition where
             * lmac154_ack_request is set AFTER LMAC154_TX_TRIGGER(). */
            if (s_radio_state.pTxFrame &&
                !((s_radio_state.pTxFrame->mPsdu[0] >> 5) & 0x01)) {
                s_ot_dbg.tx_done_no_ack_req++;
                OT_NOTIFY_ISR(OT_SYSTEM_EVENT_RADIO_TX_DONE_NO_ACK_REQ);
            } else {
                s_ot_dbg.tx_no_ack++;
                OT_NOTIFY_ISR(OT_SYSTEM_EVENT_RADIO_TX_NO_ACK);
            }
            break;

        case IEEE802154_EVENT_TX_CHANNEL_BUSY:
            /* CCA failed — the frame did NOT go out. Surface this as a
             * TX_ERROR so OT's MAC layer can record CCA failure and
             * back off, instead of silently treating it as a successful
             * broadcast. The task handler maps TX_ERROR for a CCA-busy
             * frame to OT_ERROR_CHANNEL_ACCESS_FAILURE. */
            s_ot_dbg.tx_csma_fail_at_ot++;
            OT_NOTIFY_ISR(OT_SYSTEM_EVENT_RADIO_TX_ERROR);
            break;

        case IEEE802154_EVENT_RX_DONE:
            /* The pool holds OTRADIO_MAX_PSDU per frame; only a multi-rate
             * receiver (never enabled for Thread) could deliver more. */
            if (frame && frame->length <= OTRADIO_MAX_PSDU) {
                s_ot_dbg.rx_done_isr++;
                otRadio_rxFrame_t *p = NULL;

                UBaseType_t uxSavedInterruptStatus = OT_ENTER_CRITICAL_ISR();
                p = frame_list_pop(&s_radio_state.freeFrameList);
                if (p) {
                    s_radio_state.dbgFrameNum--;
                }
                OT_EXIT_CRITICAL_ISR(uxSavedInterruptStatus);

                if (p) {
                    /* Copy frame data */
                    memcpy(p->frame.mPsdu, frame->psdu, frame->length);
                    p->frame.mLength = frame->length;
                    p->frame.mChannel = s_radio_state.current_channel;
                    p->frame.mInfo.mRxInfo.mRssi = frame->info.rx.rssi;
                    p->frame.mInfo.mRxInfo.mLqi = frame->info.rx.lqi;
                    p->frame.mInfo.mRxInfo.mTimestamp =
                        ot_radio_hal_to_time((uint32_t)frame->info.rx.timestamp);
                    p->frame.mInfo.mRxInfo.mAckedWithFramePending = frame->info.rx.frame_pending;
                    /* the frame counter our secured Enh-ACK used (TRANSMIT_SEC) */
                    p->frame.mInfo.mRxInfo.mAckedWithSecEnhAck = frame->info.rx.ack_secured;
                    p->frame.mInfo.mRxInfo.mAckFrameCounter = frame->info.rx.ack_frame_counter;
                    p->frame.mInfo.mRxInfo.mAckKeyId = frame->info.rx.ack_key_index;

                    uxSavedInterruptStatus = OT_ENTER_CRITICAL_ISR();
                    frame_list_push_tail(&s_radio_state.rxFrameList, p);
                    s_radio_state.dbgRxFrameNum++;
                    if (s_radio_state.dbgMaxPendingFrameNum < s_radio_state.dbgRxFrameNum) {
                        s_radio_state.dbgMaxPendingFrameNum = s_radio_state.dbgRxFrameNum;
                    }
                    OT_EXIT_CRITICAL_ISR(uxSavedInterruptStatus);

                    OT_NOTIFY_ISR(OT_SYSTEM_EVENT_RADIO_RX_DONE);
                } else {
                    s_ot_dbg.rx_no_buf++;
                    OT_NOTIFY_ISR(OT_SYSTEM_EVENT_RADIO_RX_NO_BUFF);
                }
            }
            break;
            
        default:
            break;
    }
}

/*===========================================================================
 * OpenThread Platform Radio API Implementation
 *===========================================================================*/

otRadioFrame* otPlatRadioGetTransmitBuffer(otInstance *aInstance)
{
    s_radio_state.aInstance = aInstance;
    
    otRadioFrame *txframe = (otRadioFrame *)s_radio_state.buffPool;
    txframe->mPsdu = s_radio_state.buffPool + ALIGNED_RX_FRAME_SIZE;
    
    return txframe;
}

otError otPlatRadioTransmit(otInstance *aInstance, otRadioFrame *aFrame)
{
    s_ot_dbg.tx_submit++;
    if (aFrame->mInfo.mTxInfo.mTxDelay != 0) {
        int32_t err = (int32_t)((uint32_t)otPlatTimeGet() -
                                (aFrame->mInfo.mTxInfo.mTxDelayBaseTime + aFrame->mInfo.mTxInfo.mTxDelay));

        s_txtime_dbg.last = err;
        if (s_txtime_dbg.n == 0 || err < s_txtime_dbg.min) s_txtime_dbg.min = err;
        if (s_txtime_dbg.n == 0 || err > s_txtime_dbg.max) s_txtime_dbg.max = err;
        s_txtime_dbg.hist[err < -1000 ? 0 : err < 0 ? 1 : err < 1000 ? 2 : err < 5000 ? 3 : err < 20000 ? 4 : 5]++;
        s_txtime_dbg.n++;
        if (aFrame->mInfo.mTxInfo.mCsmaCaEnabled) s_txtime_dbg.csma++;
    }
    if (s_radio_state.pTxFrame != NULL) {
        s_ot_dbg.tx_submit_busy++;
        return OT_ERROR_INVALID_STATE;
    }

    s_radio_state.pTxFrame = aFrame;

    /* Set channel if different */
    if (aFrame->mChannel != s_radio_state.current_channel) {
        ieee802154_set_channel(aFrame->mChannel);
        s_radio_state.current_channel = aFrame->mChannel;
    }

    /* Prepare IEEE 802.15.4 frame */
    ieee802154_frame_t ieee_frame;
    memset(&ieee_frame, 0, sizeof(ieee_frame));

    memcpy(ieee_frame.psdu, aFrame->mPsdu, aFrame->mLength);
    ieee_frame.length = aFrame->mLength - 2;  /* Exclude FCS (handled by HW) */
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    bool exact_time = ot_radio_csl_needs_exact_time(ieee_frame.psdu, ieee_frame.length);

    if (!exact_time && ot_radio_csl_tx_prepare(aFrame, ieee_frame.psdu, ieee_frame.length) != 0) {
        /* no key for the frame's key index / unparsable secured frame: never
         * sent in the clear. Reported as a failed attempt - the SubMac
         * asserts on an error return from otPlatRadioTransmit. */
        s_ot_dbg.tx_submit_hal_fail++;
        OT_NOTIFY(OT_SYSTEM_EVENT_RADIO_TX_ERROR);
        return OT_ERROR_NONE;
    }
#endif
    ieee_frame.info.tx.channel = aFrame->mChannel;
    ieee_frame.info.tx.csma_ca_enabled = aFrame->mInfo.mTxInfo.mCsmaCaEnabled;
    ieee_frame.info.tx.max_csma_backoffs = aFrame->mInfo.mTxInfo.mMaxCsmaBackoffs;
#if defined(OPENTHREAD_CONFIG_MAC_SOFTWARE_RETRANSMIT_ENABLE) && OPENTHREAD_CONFIG_MAC_SOFTWARE_RETRANSMIT_ENABLE
    /* OpenThread retries (the caps below leave out TRANSMIT_RETRIES): one
     * attempt per call, or the hardware would retry on top of it. */
    ieee_frame.info.tx.max_frame_retries = 0;
#else
    ieee_frame.info.tx.max_frame_retries = aFrame->mInfo.mTxInfo.mMaxFrameRetries;
#endif
    ieee_frame.info.tx.ack_request = (aFrame->mPsdu[0] & (1 << 5)) != 0;

    /* A timed (CSL) frame is aimed at the receiver's sample window: no
     * CSMA backoff, which would push it out of the window (the HAL has no
     * CCA-only mode, so it goes without CCA, like the Zephyr driver's
     * timed transmit). */
    if (aFrame->mInfo.mTxInfo.mTxDelay != 0) {
        ieee_frame.info.tx.csma_ca_enabled = false;
    }

    /* Diagnostic override — force CSMA off when toggled by shell. */
    if (s_force_no_csma) {
        ieee_frame.info.tx.csma_ca_enabled = false;
    }

    if (ieee_frame.info.tx.ack_request) {
        s_ot_dbg.tx_ack_requested++;
    } else {
        s_ot_dbg.tx_no_ack_request++;
    }

    /* Transmit */
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    if (exact_time) {
        /* CSL IE: CSMA-CA in the HAL, the IE written and secured for the
         * exact send time (ot_radio_csl_prepare_exact) */
        ieee802154_error_t xerr = ieee802154_hal_transmit_exact(&ieee_frame, ot_radio_csl_prepare_exact, aFrame);

        if (xerr == IEEE802154_OK) {
            otPlatRadioTxStarted(aInstance, aFrame);
            s_radio_state.tstx = otPlatTimeGet() / 1000 + 1000;
            return OT_ERROR_NONE;
        }
        if (xerr == IEEE802154_ERROR_CHANNEL_BUSY || xerr == IEEE802154_ERROR_SECURITY) {
            /* busy channel, or the frame could not be secured: a failed
             * attempt (channel access failure) - the SubMac asserts on an
             * error return from otPlatRadioTransmit */
            s_ot_dbg.tx_submit_hal_fail++;
            OT_NOTIFY(OT_SYSTEM_EVENT_RADIO_TX_ERROR);
            return OT_ERROR_NONE;
        }
        s_radio_state.pTxFrame = NULL;
        return OT_ERROR_FAILED;
    }
#endif
    ieee802154_error_t err = ieee802154_transmit(&ieee_frame);
    if (err != IEEE802154_OK) {
        s_ot_dbg.tx_submit_hal_fail++;
        s_radio_state.pTxFrame = NULL;
        IPRO_LOGE(TAG, "ieee802154_transmit failed: err=%d, ch=%d, len=%d",
                  err, aFrame->mChannel, aFrame->mLength);
        return OT_ERROR_FAILED;
    }

    otPlatRadioTxStarted(aInstance, aFrame);
    s_radio_state.tstx = otPlatTimeGet() / 1000 + 1000;

    return OT_ERROR_NONE;
}

otError otPlatRadioEnable(otInstance *aInstance)
{
    s_radio_state.aInstance = aInstance;

#ifdef CONFIG_IPRO7
    /*
     * IPRO7 RF init pattern (must precede ieee802154_enable). See the
     * extern block at the top of this file for the root-cause
     * explanation. These three calls together prevent the noinit
     * garbage from making the PA land at max bias.
     */
    rf_set_bz_mode(IPRO7_RF_MODE_ZB_ONLY);
    {
        int8_t zero_zb_offsets[16] = {0};
        rf_set_pwr_offset_zb(zero_zb_offsets);
    }
#endif

    /* Enable radio */
    ieee802154_error_t err = ieee802154_enable();
    if (err != IEEE802154_OK) {
        IPRO_LOGE(TAG, "HAL enable failed: %d", err);
        return OT_ERROR_FAILED;
    }

    s_radio_state.radio_enabled = true;
    s_radio_state.current_channel = s_radio_state.ieee802154_config.channel;
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    ot_radio_csl_init();
#endif

#ifdef CONFIG_IPRO7
    /*
     * Re-derive the per-channel offset and reapply target TX power so
     * the PA gets clean values, not whatever the lmac154 cold-init
     * left behind (potentially with random offset).
     */
    rf_optimize_by_channel(2405u + 5u * (uint32_t)
        (s_radio_state.current_channel - OT_RADIO_2P4GHZ_OQPSK_CHANNEL_MIN));
    (void)rf_set_tx_power_zb((int32_t)s_radio_state.ieee802154_config.tx_power);
#endif

    return OT_ERROR_NONE;
}

otError otPlatRadioDisable(otInstance *aInstance)
{
    (void)aInstance;
    
    /* HAL will handle IRQ cleanup on deinit */
    ieee802154_disable();
    s_radio_state.radio_enabled = false;
    
    return OT_ERROR_NONE;
}

bool otPlatRadioIsEnabled(otInstance *aInstance)
{
    (void)aInstance;
    return s_radio_state.radio_enabled;
}

otError otPlatRadioSleep(otInstance *aInstance)
{
    (void)aInstance;
    s_ot_dbg.sleep_calls++;

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    /* End of a CSL sample window: a frame that started inside it is
     * received to the end and acknowledged (at most a 127-byte frame and
     * its Enh-ACK, ~6 ms). */
    {
        uint64_t t0 = otPlatTimeGet();

        while (ieee802154_hal_rx_busy() && otPlatTimeGet() - t0 < 6000u) {
        }
    }
#endif
    ieee802154_error_t err = ieee802154_sleep();
    return (err == IEEE802154_OK) ? OT_ERROR_NONE : OT_ERROR_FAILED;
}

otError otPlatRadioReceive(otInstance *aInstance, uint8_t aChannel)
{
    (void)aInstance;
    s_ot_dbg.rx_enable_calls++;

    if (aChannel != s_radio_state.current_channel) {
        s_ot_dbg.rx_channel_changes++;
        ieee802154_error_t err = ieee802154_set_channel(aChannel);
        if (err != IEEE802154_OK) {
            IPRO_LOGE(TAG, "Failed to set channel: %d", err);
            return OT_ERROR_FAILED;
        }
        s_radio_state.current_channel = aChannel;

#ifdef CONFIG_IPRO7
        /*
         * Re-derive per-channel TX offset + reapply PA bias on every
         * channel change. Skipping this leaves stale offset from the
         * previous channel's tempcorr — small fix vs power but keeps
         * behaviour symmetric with otPlatRadioSetTransmitPower above.
         */
        rf_optimize_by_channel(2405u + 5u * (uint32_t)
            (aChannel - OT_RADIO_2P4GHZ_OQPSK_CHANNEL_MIN));
        (void)rf_set_tx_power_zb((int32_t)s_radio_state.current_tx_power);
#endif
    }

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    bool rx_busy = ieee802154_hal_rx_busy();
#endif
    ieee802154_error_t err = ieee802154_receive();
    if (err == IEEE802154_ERROR_BUSY) {
        return OT_ERROR_NONE;  /* TX in progress, not a fatal error */
    }
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    if (err == IEEE802154_OK) {
        ot_radio_csl_note_rx_on(rx_busy);
    }
#endif

    return (err == IEEE802154_OK) ? OT_ERROR_NONE : OT_ERROR_FAILED;
}
otRadioCaps otPlatRadioGetCaps(otInstance *aInstance)
{
    (void)aInstance;
    
    otRadioCaps caps = OT_RADIO_CAPS_ACK_TIMEOUT | OT_RADIO_CAPS_CSMA_BACKOFF | OT_RADIO_CAPS_SLEEP_TO_TX;

#if !(defined(OPENTHREAD_CONFIG_MAC_SOFTWARE_RETRANSMIT_ENABLE) && OPENTHREAD_CONFIG_MAC_SOFTWARE_RETRANSMIT_ENABLE)
    caps |= OT_RADIO_CAPS_TRANSMIT_RETRIES;
#endif
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    /* CSL IE + security at transmit time, secured Enh-ACK (ot_radio_csl.c) */
    caps |= OT_RADIO_CAPS_TRANSMIT_SEC;
#endif
    return caps;
}

const char* otPlatRadioGetVersionString(otInstance *aInstance)
{
    (void)aInstance;
    return "ieee802154-ipro7-1.0.0";
}

int8_t otPlatRadioGetReceiveSensitivity(otInstance *aInstance)
{
    (void)aInstance;
    return -103;
}

/* App override hook for the factory IEEE EUI-64.
 *
 * Default (weak) returns non-zero => "no app value, use the efuse MAC". Boards
 * whose efuse has no unique factory EUI-64 (e.g. the ipro7_arduino dev board,
 * where every chip reads the same MAC) override this with a strong definition
 * that fills @p eui64 with a unique per-device value and returns 0. Keeping the
 * efuse fallback here means all other apps are unaffected. */
__attribute__((weak)) int ipro_ot_get_app_eui64(uint8_t eui64[8])
{
    (void)eui64;
    return -1;
}

void otPlatRadioGetIeeeEui64(otInstance *aInstance, uint8_t *aIeeeEui64)
{
    (void)aInstance;

    /* App-supplied unique EUI-64 wins when present (see weak hook above). */
    if (ipro_ot_get_app_eui64(aIeeeEui64) == 0) {
        return;
    }

#ifdef CONFIG_IPRO7
    extern void EF_Ctrl_Read_MAC_Address(uint8_t mac[8]);
    EF_Ctrl_Read_MAC_Address(aIeeeEui64);
#else
    memset(aIeeeEui64, 0xFF, 8);
#endif
}

void otPlatRadioSetPanId(otInstance *aInstance, otPanId aPanId)
{
    (void)aInstance;
    
    ieee802154_set_pan_id(aPanId);
    s_radio_state.ieee802154_config.pan_id = aPanId;
}

void otPlatRadioSetExtendedAddress(otInstance *aInstance, const otExtAddress *aExtAddress)
{
    (void)aInstance;
    
    ieee802154_ext_addr_t ext_addr;
    memcpy(ext_addr.m8, aExtAddress->m8, 8);
    ieee802154_set_ext_addr(&ext_addr);
    memcpy(s_radio_state.ieee802154_config.ext_addr.m8, aExtAddress->m8, 8);
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    ot_radio_csl_set_ext_addr(aExtAddress->m8);
#endif
}

void otPlatRadioSetShortAddress(otInstance *aInstance, otShortAddress aShortAddress)
{
    (void)aInstance;
    
    ieee802154_set_short_addr(aShortAddress);
    s_radio_state.ieee802154_config.short_addr = aShortAddress;
}

otError otPlatRadioGetTransmitPower(otInstance *aInstance, int8_t *aPower)
{
    (void)aInstance;
    
    *aPower = s_radio_state.current_tx_power;
    return OT_ERROR_NONE;
}

otError otPlatRadioSetTransmitPower(otInstance *aInstance, int8_t aPower)
{
    (void)aInstance;

    ieee802154_error_t err = ieee802154_set_tx_power(aPower);
    if (err == IEEE802154_OK) {
        s_radio_state.current_tx_power = aPower;
    }

#ifdef CONFIG_IPRO7
    /*
     * Re-derive offset for the current channel and rewrite PA bias.
     * ieee802154_set_tx_power above only updates an internal counter;
     * the actual PA register write must go through rf_set_tx_power_zb,
     * and that depends on tx_pwr_offset_zb which only gets refreshed
     * via rf_optimize_by_channel.
     */
    rf_optimize_by_channel(2405u + 5u * (uint32_t)
        (s_radio_state.current_channel - OT_RADIO_2P4GHZ_OQPSK_CHANNEL_MIN));
    (void)rf_set_tx_power_zb((int32_t)aPower);
#endif

    return (err == IEEE802154_OK) ? OT_ERROR_NONE : OT_ERROR_FAILED;
}

otError otPlatRadioGetCcaEnergyDetectThreshold(otInstance *aInstance, int8_t *aThreshold)
{
    (void)aInstance;
    
    *aThreshold = s_radio_state.ieee802154_config.cca_threshold;
    return OT_ERROR_NONE;
}

otError otPlatRadioSetCcaEnergyDetectThreshold(otInstance *aInstance, int8_t aThreshold)
{
    (void)aInstance;
    
    s_radio_state.ieee802154_config.cca_threshold = aThreshold;
    /* CCA threshold is applied during CSMA-CA */
    return OT_ERROR_NONE;
}

bool otPlatRadioGetPromiscuous(otInstance *aInstance)
{
    (void)aInstance;
    return s_radio_state.ieee802154_config.promiscuous;
}

void otPlatRadioSetPromiscuous(otInstance *aInstance, bool aEnable)
{
    (void)aInstance;

    s_radio_state.ieee802154_config.promiscuous = aEnable;
    ieee802154_set_promiscuous(aEnable);
}

#if !OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE /* else ot_radio_csl.c (TRANSMIT_SEC) */
void otPlatRadioSetMacKey(otInstance *aInstance, uint8_t aKeyIdMode,
                          uint8_t aKeyId, const otMacKeyMaterial *aPrevKey,
                          const otMacKeyMaterial *aCurrKey, const otMacKeyMaterial *aNextKey,
                          otRadioKeyType aKeyType)
{
    /* MAC key management - handled by OpenThread upper layers */
    (void)aInstance;
    (void)aKeyIdMode;
    (void)aKeyId;
    (void)aPrevKey;
    (void)aCurrKey;
    (void)aNextKey;
    (void)aKeyType;
}

void otPlatRadioSetMacFrameCounter(otInstance *aInstance, uint32_t aMacFrameCounter)
{
    (void)aInstance;
    (void)aMacFrameCounter;
}
#endif

uint64_t otPlatRadioGetNow(otInstance *aInstance)
{
    (void)aInstance;
    /* the clock of every radio time here, see ot_radio_hal_to_time() */
    return otPlatTimeGet();
}

void otPlatRadioSetRxOnWhenIdle(otInstance *aInstance, bool aEnable)
{
    (void)aInstance;

    if (aEnable) {
        ieee802154_receive();
    } else {
        /* When RX-on-when-idle is disabled, radio should go to sleep
         * between transmissions (sleepy end device behavior) */
        ieee802154_sleep();
    }
}

otRadioState otPlatRadioGetState(otInstance *aInstance)
{
    (void)aInstance;
    
    if (!s_radio_state.radio_enabled) {
        return OT_RADIO_STATE_DISABLED;
    }
    
    if (s_radio_state.pTxFrame) {
        return OT_RADIO_STATE_TRANSMIT;
    }
    
    ieee802154_state_t state;
    ieee802154_get_state(&state);
    switch (state) {
        case IEEE802154_STATE_DISABLED:
            return OT_RADIO_STATE_DISABLED;
        case IEEE802154_STATE_SLEEP:
            return OT_RADIO_STATE_SLEEP;
        case IEEE802154_STATE_RX:
            return OT_RADIO_STATE_RECEIVE;
        case IEEE802154_STATE_TX:
            return OT_RADIO_STATE_TRANSMIT;
        default:
            return OT_RADIO_STATE_RECEIVE;
    }
}

int8_t otPlatRadioGetRssi(otInstance *aInstance)
{
    (void)aInstance;
    int8_t rssi = 0;
    ieee802154_get_rssi(&rssi);
    return rssi;
}

/* Energy scan state */
static struct {
    bool scanning;
    uint8_t channel;
    TimerHandle_t timer;
    otInstance *instance;
} sScanState = {0};

#if defined(CONFIG_IPRO_PM_THREAD)
static bool ot_radio_scanning(void)
{
    return sScanState.scanning;
}
#endif

static void energy_scan_timer_callback(TimerHandle_t xTimer)
{
    (void)xTimer;
    
    if (!sScanState.scanning) {
        return;
    }
    
    /* Get energy level - use minimal duration since timer already waited */
    int8_t energy = 0;
    ieee802154_error_t err = ieee802154_energy_detect(sScanState.channel, 1, &energy);
    if (err != IEEE802154_OK) {
        energy = -100;  /* Report minimum energy on error */
    }
    
    /* Mark scan as complete */
    sScanState.scanning = false;

    /* Restore RX on original channel */
    if (s_radio_state.radio_enabled) {
        ieee802154_set_channel(s_radio_state.current_channel);
        ieee802154_receive();
    }

    /* Notify OpenThread */
    otPlatRadioEnergyScanDone(sScanState.instance, energy);
}

otError otPlatRadioEnergyScan(otInstance *aInstance, uint8_t aScanChannel, uint16_t aScanDuration)
{
    if (sScanState.scanning) {
        return OT_ERROR_BUSY;
    }
    
    /* Store instance for callback */
    sScanState.instance = aInstance;
    
    /* Set channel for scanning */
    ieee802154_error_t err = ieee802154_set_channel(aScanChannel);
    if (err != IEEE802154_OK) {
        IPRO_LOGE(TAG, "Failed to set scan channel: %d", err);
        return OT_ERROR_FAILED;
    }
    
    /* Start RX to measure energy */
    err = ieee802154_receive();
    if (err != IEEE802154_OK) {
        IPRO_LOGE(TAG, "Failed to start RX for scan: %d", err);
        return OT_ERROR_FAILED;
    }
    
    /* Create timer if not exists */
    if (sScanState.timer == NULL) {
        sScanState.timer = xTimerCreate("EnergyScan", pdMS_TO_TICKS(100), pdFALSE, NULL, energy_scan_timer_callback);
        if (sScanState.timer == NULL) {
            IPRO_LOGE(TAG, "Failed to create scan timer");
            return OT_ERROR_FAILED;
        }
    }
    
    /* Start scan timer */
    sScanState.scanning = true;
    sScanState.channel = aScanChannel;
    
    if (xTimerChangePeriod(sScanState.timer, pdMS_TO_TICKS(aScanDuration), 0) != pdPASS) {
        sScanState.scanning = false;
        return OT_ERROR_FAILED;
    }
    
    if (xTimerStart(sScanState.timer, 0) != pdPASS) {
        sScanState.scanning = false;
        return OT_ERROR_FAILED;
    }
    
    return OT_ERROR_NONE;
}

/*===========================================================================
 * Frame Pending Table (for indirect transmission)
 *===========================================================================*/

void otPlatRadioSetFramePending(otInstance *aInstance, const otExtAddress *aAddress, bool aPending)
{
    (void)aInstance;
    
    ieee802154_set_frame_pending(aAddress->m8, IEEE802154_ADDR_MODE_EXTENDED, aPending);
}

void otPlatRadioSetShortFramePending(otInstance *aInstance, uint16_t aShortAddress, bool aPending)
{
    (void)aInstance;
    
    uint8_t addr[2] = {aShortAddress & 0xFF, (aShortAddress >> 8) & 0xFF};
    ieee802154_set_frame_pending(addr, IEEE802154_ADDR_MODE_SHORT, aPending);
}

/*===========================================================================
 * CSL (Coordinated Sampled Listening) Support
 *===========================================================================*/

/* otPlatRadioEnableCsl / otPlatRadioUpdateCslSampleTime: ot_radio_csl.c */

/*===========================================================================
 * Source Address Match Table
 *===========================================================================*/

void otPlatRadioEnableSrcMatch(otInstance *aInstance, bool aEnable)
{
    (void)aInstance;
    s_radio_state.src_match_enabled = aEnable;
}

/*
 * Source-address match / Frame Pending support
 *
 * OpenThread calls AddSrcMatchEntry(child_addr) when it has indirect TX
 * queued for that child. The radio MUST then set the Frame Pending bit
 * in the MAC ACK whenever a Data Request frame arrives from that source.
 * The sleepy child uses FP=1 to decide to stay in RX for the queued
 * frame; FP=0 means "no data, you can sleep". Without this, the parent
 * never delivers Child ID Response (or any indirect frame) to a sleepy
 * child — the child sends Data Request, gets FP=0 ACK, sleeps, and
 * misses the indirect TX entirely. Symptom: SED (rxOnWhenIdle=0) cannot
 * complete attach.
 *
 * The IPRO7 lmac154 driver maintains its own HW-side address list via
 * ieee802154_set_frame_pending(). We mirror entries to both the SW
 * cache (for OT visibility / debug) and the HAL (which the radio
 * consults when forming the ACK).
 */

otError otPlatRadioAddSrcMatchShortEntry(otInstance *aInstance, otShortAddress aShortAddress)
{
    (void)aInstance;

    /* OT may call Add for an already-cached address without an intervening
     * Clear (e.g. on duplicate sleepy-child supervision setup). Without this
     * de-dup check we'd allocate a second slot for the same address; later
     * Clear only finds the first slot, leaving the duplicate behind, which
     * keeps FP=1 in every auto-ACK forever and traps the child in fast-poll. */
    for (int i = 0; i < OT_RADIO_SRC_MATCH_SHORT_ENTRIES; i++) {
        if (s_radio_state.src_match_short_valid[i] &&
            s_radio_state.src_match_short[i] == aShortAddress) {
            return OT_ERROR_NONE;
        }
    }

    for (int i = 0; i < OT_RADIO_SRC_MATCH_SHORT_ENTRIES; i++) {
        if (!s_radio_state.src_match_short_valid[i]) {
            s_radio_state.src_match_short[i] = aShortAddress;
            s_radio_state.src_match_short_valid[i] = true;

            uint8_t addr[2] = { (uint8_t)(aShortAddress & 0xff),
                                (uint8_t)(aShortAddress >> 8) };
            ieee802154_set_frame_pending(addr, IEEE802154_ADDR_MODE_SHORT, true);
            return OT_ERROR_NONE;
        }
    }
    return OT_ERROR_NO_BUFS;
}

otError otPlatRadioAddSrcMatchExtEntry(otInstance *aInstance, const otExtAddress *aExtAddress)
{
    (void)aInstance;

    /* See AddSrcMatchShortEntry comment — same de-dup guard. */
    for (int i = 0; i < OT_RADIO_SRC_MATCH_EXT_ENTRIES; i++) {
        if (s_radio_state.src_match_ext_valid[i] &&
            memcmp(&s_radio_state.src_match_ext[i], aExtAddress, sizeof(otExtAddress)) == 0) {
            return OT_ERROR_NONE;
        }
    }

    for (int i = 0; i < OT_RADIO_SRC_MATCH_EXT_ENTRIES; i++) {
        if (!s_radio_state.src_match_ext_valid[i]) {
            memcpy(&s_radio_state.src_match_ext[i], aExtAddress, sizeof(otExtAddress));
            s_radio_state.src_match_ext_valid[i] = true;

            ieee802154_set_frame_pending(aExtAddress->m8,
                                         IEEE802154_ADDR_MODE_EXTENDED, true);
            return OT_ERROR_NONE;
        }
    }
    return OT_ERROR_NO_BUFS;
}

otError otPlatRadioClearSrcMatchShortEntry(otInstance *aInstance, otShortAddress aShortAddress)
{
    (void)aInstance;

    /* Defensive: if a duplicate ever slipped through (e.g. older firmware),
     * clear ALL matching entries, not just the first. */
    bool found = false;
    for (int i = 0; i < OT_RADIO_SRC_MATCH_SHORT_ENTRIES; i++) {
        if (s_radio_state.src_match_short_valid[i] &&
            s_radio_state.src_match_short[i] == aShortAddress) {
            s_radio_state.src_match_short_valid[i] = false;
            found = true;
        }
    }
    if (found) {
        uint8_t addr[2] = { (uint8_t)(aShortAddress & 0xff),
                            (uint8_t)(aShortAddress >> 8) };
        ieee802154_set_frame_pending(addr, IEEE802154_ADDR_MODE_SHORT, false);
        return OT_ERROR_NONE;
    }
    return OT_ERROR_NO_ADDRESS;
}

otError otPlatRadioClearSrcMatchExtEntry(otInstance *aInstance, const otExtAddress *aExtAddress)
{
    (void)aInstance;

    bool found = false;
    for (int i = 0; i < OT_RADIO_SRC_MATCH_EXT_ENTRIES; i++) {
        if (s_radio_state.src_match_ext_valid[i] &&
            memcmp(&s_radio_state.src_match_ext[i], aExtAddress, sizeof(otExtAddress)) == 0) {
            s_radio_state.src_match_ext_valid[i] = false;
            found = true;
        }
    }
    if (found) {
        ieee802154_set_frame_pending(aExtAddress->m8,
                                     IEEE802154_ADDR_MODE_EXTENDED, false);
        return OT_ERROR_NONE;
    }
    return OT_ERROR_NO_ADDRESS;
}

void otPlatRadioClearSrcMatchShortEntries(otInstance *aInstance)
{
    (void)aInstance;

    for (int i = 0; i < OT_RADIO_SRC_MATCH_SHORT_ENTRIES; i++) {
        if (s_radio_state.src_match_short_valid[i]) {
            otShortAddress addr16 = s_radio_state.src_match_short[i];
            uint8_t addr[2] = { (uint8_t)(addr16 & 0xff), (uint8_t)(addr16 >> 8) };
            ieee802154_set_frame_pending(addr, IEEE802154_ADDR_MODE_SHORT, false);
            s_radio_state.src_match_short_valid[i] = false;
        }
    }
}

void otPlatRadioClearSrcMatchExtEntries(otInstance *aInstance)
{
    (void)aInstance;

    for (int i = 0; i < OT_RADIO_SRC_MATCH_EXT_ENTRIES; i++) {
        if (s_radio_state.src_match_ext_valid[i]) {
            ieee802154_set_frame_pending(s_radio_state.src_match_ext[i].m8,
                                         IEEE802154_ADDR_MODE_EXTENDED, false);
            s_radio_state.src_match_ext_valid[i] = false;
        }
    }
}

/*===========================================================================
 * Enhanced ACK Support
 *===========================================================================*/

#if defined(OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE) && OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
otError otPlatRadioConfigureEnhAckProbing(otInstance *aInstance, otLinkMetrics aLinkMetrics,
                                           const otShortAddress aShortAddress,
                                           const otExtAddress *aExtAddress)
{
    (void)aInstance;
    (void)aShortAddress;
    (void)aExtAddress;

    bool metrics_requested = aLinkMetrics.mPduCount || aLinkMetrics.mLqi ||
                             aLinkMetrics.mLinkMargin || aLinkMetrics.mRssi;

    ieee802154_enh_ack_config_t enh_config = {
        .enabled = metrics_requested,
    };
    ieee802154_configure_enh_ack(&enh_config);

    return OT_ERROR_NONE;
}
#endif

/*===========================================================================
 * Channel Mask Support
 *===========================================================================*/

uint32_t otPlatRadioGetSupportedChannelMask(otInstance *aInstance)
{
    (void)aInstance;
    return OT_RADIO_2P4GHZ_OQPSK_CHANNEL_MASK;
}

uint32_t otPlatRadioGetPreferredChannelMask(otInstance *aInstance)
{
    (void)aInstance;
    return OT_RADIO_2P4GHZ_OQPSK_CHANNEL_MASK;
}

/*===========================================================================
 * Shell commands for debugging — only compiled when CONFIG_SHELL=y.
 *
 *   radio_stats        — dump TX/RX/ACK counters at OT and HAL layers
 *   radio_stats_reset  — zero all counters
 *
 * Use the standard sequence to diagnose NoAck loops:
 *   1. radio_stats_reset
 *   2. trigger TX (e.g. ot ping <peer>)
 *   3. radio_stats — compare HAL ack-simple/ack-frame counts vs OT
 *      tx_done_acked. If HAL says ack received but OT doesn't, forwarding
 *      is broken. If HAL says ack not received, the radio really isn't
 *      seeing the peer's ACK (auto-ACK off on peer, address filter, RF).
 *===========================================================================*/

#ifdef CONFIG_SHELL
static int cmd_radio_stats(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    ot_radio_dbg_dump();
    return 0;
}

/* radio_trace on|off|clear|dump|stream on|off: the HAL frame trace
 * (sniffer-free capture, CONFIG_IEEE802154_FRAME_TRACE). "stream on"
 * prints the records as they come from a low-priority task, so a host can
 * capture a long test without polling. */
extern void ieee802154_hal_trace_enable(bool on);
extern void ieee802154_hal_trace_clear(void);
extern void ieee802154_hal_trace_dump(void);
extern unsigned ieee802154_hal_trace_pump(unsigned max);
#if defined(CONFIG_IEEE802154_FRAME_TRACE)
static volatile bool s_trace_stream;
static TaskHandle_t s_trace_task;

/* Woken by each new record and by `stream on` (not a 10 ms poll, which would
 * keep the part out of deep sleep while streaming). */
void ieee802154_hal_trace_added(void)
{
    TaskHandle_t task = s_trace_task;

    if (!s_trace_stream || (task == NULL)) {
        return;
    }
    if (xPortIsInsideInterrupt()) {
        BaseType_t woken = pdFALSE;

        vTaskNotifyGiveFromISR(task, &woken);
        portYIELD_FROM_ISR(woken);
    } else {
        xTaskNotifyGive(task);
    }
}

static void radio_trace_task(void *arg)
{
    (void)arg;
    for (;;) {
        if (!s_trace_stream || ieee802154_hal_trace_pump(8) == 0u) {
            (void)ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        }
    }
}
#endif

static int cmd_radio_trace(int argc, char **argv)
{
    const char *op = argc >= 2 ? argv[1] : "dump";

    if (strcmp(op, "on") == 0) {
        ieee802154_hal_trace_enable(true);
    } else if (strcmp(op, "off") == 0) {
        ieee802154_hal_trace_enable(false);
    } else if (strcmp(op, "clear") == 0) {
        ieee802154_hal_trace_clear();
    } else if (strcmp(op, "dump") == 0) {
        ieee802154_hal_trace_dump();
    } else if (strcmp(op, "stream") == 0 && argc >= 3) {
#if defined(CONFIG_IEEE802154_FRAME_TRACE)
        s_trace_stream = strcmp(argv[2], "on") == 0;
        if (s_trace_stream && s_trace_task == NULL &&
            xTaskCreate(radio_trace_task, "rtrace", 512, NULL, tskIDLE_PRIORITY + 1, &s_trace_task) != pdPASS) {
            s_trace_task = NULL;
            s_trace_stream = false;
            printf("radio_trace: task create failed\r\n");
        }
        if (s_trace_stream && (s_trace_task != NULL)) {
            xTaskNotifyGive(s_trace_task); /* records already waiting */
        }
#else
        ieee802154_hal_trace_enable(false); /* prints the "not built in" note */
#endif
    } else {
        printf("usage: radio_trace on|off|clear|dump|stream on|off\r\n");
    }
    return 0;
}

static int cmd_radio_stats_reset(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    ot_radio_dbg_reset();
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    ot_radio_csl_dump(true);
#endif
    printf("radio counters cleared\n");
    return 0;
}

extern void ieee802154_hal_set_promisc(bool enable);
extern void ieee802154_hal_cca_probe(unsigned samples);
extern void ieee802154_hal_cca_set_threshold(int threshold_dbm);
extern void ieee802154_hal_cca_set_mode(uint8_t mode);
extern void ieee802154_hal_csma_set_be(uint8_t max_be, uint8_t min_be);
extern void ieee802154_hal_csma_set_max_backoff(uint8_t max_backoff);
extern void ieee802154_hal_dump_fpt(void);

static int cmd_radio_fpt(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    /* Dump OT-side cache so the comparison is on one screen. */
    printf("[ot ] SrcMatch SW cache:\n");
    printf("       enabled=%d\n", (int)s_radio_state.src_match_enabled);
    int short_count = 0;
    for (int i = 0; i < OT_RADIO_SRC_MATCH_SHORT_ENTRIES; i++) {
        if (s_radio_state.src_match_short_valid[i]) {
            printf("       short[%d]=0x%04x\n", i,
                   s_radio_state.src_match_short[i]);
            short_count++;
        }
    }
    int ext_count = 0;
    for (int i = 0; i < OT_RADIO_SRC_MATCH_EXT_ENTRIES; i++) {
        if (s_radio_state.src_match_ext_valid[i]) {
            const uint8_t *e = s_radio_state.src_match_ext[i].m8;
            printf("       ext[%d]=%02x%02x%02x%02x%02x%02x%02x%02x\n", i,
                   e[0], e[1], e[2], e[3], e[4], e[5], e[6], e[7]);
            ext_count++;
        }
    }
    printf("       (cache: %d short, %d ext)\n", short_count, ext_count);

    /* Now read the actual HW table — these MUST match the cache for
     * Frame Pending bit in MAC ACK to work for sleepy children. */
    ieee802154_hal_dump_fpt();
    return 0;
}

static int cmd_radio_promisc(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: radio_promisc on|off\n");
        return 0;
    }
    bool on = (argv[1][0] == 'o' && argv[1][1] == 'n');
    ieee802154_hal_set_promisc(on);
    printf("promisc %s — frames now %s the address/PAN filter; "
           "watch radio_stats rx done counter\n",
           on ? "ENABLED" : "disabled",
           on ? "BYPASS" : "go through");
    return 0;
}

static int cmd_radio_cca_probe(int argc, char **argv)
{
    unsigned n = (argc >= 2) ? (unsigned)atoi(argv[1]) : 16;
    ieee802154_hal_cca_probe(n);
    return 0;
}

static int cmd_radio_cca_threshold(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: radio_cca_th <dBm>  (range -122..5, default -75)\n");
        return 0;
    }
    ieee802154_hal_cca_set_threshold(atoi(argv[1]));
    return 0;
}

static int cmd_radio_cca_mode(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: radio_cca_mode <0|1|2|3>  "
               "(0=ED 1=CS 2=ED&CS 3=ED|CS, IEEE default = 2)\n");
        return 0;
    }
    ieee802154_hal_cca_set_mode((uint8_t)atoi(argv[1]));
    return 0;
}

static int cmd_radio_csma_be(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: radio_csma_be <max_be> <min_be>  (IEEE default 5 3)\n");
        return 0;
    }
    ieee802154_hal_csma_set_be((uint8_t)atoi(argv[1]),
                                (uint8_t)atoi(argv[2]));
    return 0;
}

static int cmd_radio_csma_max(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: radio_csma_max <max_backoff>  (IEEE default 4)\n");
        return 0;
    }
    ieee802154_hal_csma_set_max_backoff((uint8_t)atoi(argv[1]));
    return 0;
}

/* `radio_crc_report on|off`: have the MAC hand CRC-failed frames to the HAL
 * (which counts and drops them: `[hal] rx: crc_fail=`) - a frame error
 * rate for the link. */
extern void lmac154_setRxAcceptPolicy(uint32_t policy);
extern uint32_t lmac154_getRxAcceptPolicy(void);
static int cmd_radio_crc_report(int argc, char **argv)
{
    uint32_t p = lmac154_getRxAcceptPolicy();

    if (argc >= 2) {
        bool on = argv[1][0] == 'o' && argv[1][1] == 'n';
        p = on ? (p | 0x1u) : (p & ~0x1u); /* LMAC154_RX_ACCEPT_CRC_ERR */
        lmac154_setRxAcceptPolicy(p);
    }
    printf("rx accept policy 0x%02lx (CRC-failed frames %s)\n", (unsigned long)p, (p & 0x1u) ? "reported" : "dropped");
    return 0;
}

static int cmd_radio_force_no_csma(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: radio_force_no_csma on|off  "
               "(diagnostic: bypass CSMA entirely)\n");
        return 0;
    }
    s_force_no_csma = (argv[1][0] == 'o' && argv[1][1] == 'n');
    printf("force_no_csma=%s — TX will %s honour the CSMA-CA flag from "
           "the upper MAC. Spec-violating, do NOT leave on.\n",
           s_force_no_csma ? "ON" : "off",
           s_force_no_csma ? "IGNORE" : "honour");
    return 0;
}

#ifdef CONFIG_IPRO7
/* drv_aon.h / drv_ef_ctrl.h clash with this file's own EF_Ctrl_Read_MAC_Address
 * declaration, so declare what is used here. */
#define RADIO_XTAL_CFG2_REG  (0x3008F000u + 0x888u) /* AON_BASE + AON_XTAL_CFG2_OFFSET */
#define RADIO_XTAL_CAPIN_POS  8u                    /* AON_XTAL_HF_CAPCODE_IN_AON */
#define RADIO_XTAL_CAPOUT_POS 0u                    /* AON_XTAL_HF_CAPCODE_OUT_AON */
extern int AON_Set_Xtal_CapCode(uint8_t capIn, uint8_t capOut);
extern uint8_t EF_Ctrl_Is_CapCode_Empty(uint8_t slot, uint8_t reload);
extern int EF_Ctrl_Read_CapCode_Opt(uint8_t slot, uint8_t *code, uint8_t reload);
/* `radio_xtal [capin [capout]]`: RF crystal load-capacitance trim (the
 * carrier frequency): show the live value and the eFuse calibration slots,
 * or set a new value (runtime only, not stored). */
static int cmd_radio_xtal(int argc, char **argv)
{
    uint32_t cfg2 = *(volatile uint32_t *)RADIO_XTAL_CFG2_REG;

    if (argc >= 2) {
        uint8_t in  = (uint8_t)atoi(argv[1]);
        uint8_t out = (argc >= 3) ? (uint8_t)atoi(argv[2]) : in;
        AON_Set_Xtal_CapCode(in, out);
        cfg2 = *(volatile uint32_t *)RADIO_XTAL_CFG2_REG;
    }
    printf("xtal capcode in=%lu out=%lu\n",
           (unsigned long)((cfg2 >> RADIO_XTAL_CAPIN_POS) & 0xffu),
           (unsigned long)((cfg2 >> RADIO_XTAL_CAPOUT_POS) & 0xffu));
    for (uint8_t slot = 0; slot < 3; slot++) {
        uint8_t code = 0;
        uint8_t empty = EF_Ctrl_Is_CapCode_Empty(slot, 1);
        int rc = EF_Ctrl_Read_CapCode_Opt(slot, &code, 1);
        printf("efuse capcode slot %u: %s code=%u rc=%d\n", slot, empty ? "empty" : "set", code, rc);
    }
    return 0;
}
#endif

extern void ieee802154_hal_set_ack_wait(uint16_t imm_us, uint16_t enh_us);
/* `radio_ack_wait [imm_us [enh_us]]`: MAC ACK wait per ACK kind. */
static int cmd_radio_ack_wait(int argc, char **argv)
{
    ieee802154_hal_set_ack_wait(argc >= 2 ? (uint16_t)atoi(argv[1]) : 0u,
                                argc >= 3 ? (uint16_t)atoi(argv[2]) : 0u);
    return 0;
}

extern void ieee802154_hal_cfo_dump(bool clear);
/* `radio_cfo [clr]`: per-sender frequency offset / SFD correlation / RSSI. */
static int cmd_radio_cfo(int argc, char **argv)
{
    ieee802154_hal_cfo_dump(argc >= 2 && argv[1][0] == 'c');
    return 0;
}

extern volatile bool g_hal_rx_dump;
static int cmd_radio_rx_log(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: radio_rx_log on|off  (dump first 24 bytes of every RX MPDU)\n");
        printf("currently: %s\n", g_hal_rx_dump ? "ON" : "off");
        return 0;
    }
    g_hal_rx_dump = (argv[1][0] == 'o' && argv[1][1] == 'n');
    printf("radio_rx_log=%s\n", g_hal_rx_dump ? "ON" : "off");
    return 0;
}

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
static int cmd_radio_csl_win(int argc, char **argv)
{
    extern volatile uint8_t ot_radio_csl_accuracy_ppm;
    extern volatile uint8_t ot_radio_csl_uncertainty_10us;

    if (argc >= 3) {
        ot_radio_csl_accuracy_ppm = (uint8_t)strtoul(argv[1], NULL, 0);
        ot_radio_csl_uncertainty_10us = (uint8_t)strtoul(argv[2], NULL, 0);
    }
    printf("csl accuracy %u ppm uncertainty %u x 10 us\n", (unsigned)ot_radio_csl_accuracy_ppm,
           (unsigned)ot_radio_csl_uncertainty_10us);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_radio_csl_win, radio_csl_win, CSL receiver accuracy ppm and uncertainty x10us [acc unc]);
#endif

/* Software Enh-ACK: least time from its trigger to TX_2 (the MAC and the RF
 * switch from the receiver restarted at RX end to TX in it) - bench tuning. */
static int cmd_radio_ack_lead(int argc, char **argv)
{
    extern void ieee802154_hal_set_enh_ack_prearm_lead(uint16_t us);
    extern uint16_t ieee802154_hal_get_enh_ack_prearm_lead(void);
    extern void ieee802154_hal_set_enh_ack_prearm(uint8_t mode);
    extern uint8_t ieee802154_hal_get_enh_ack_prearm(void);

    if (argc >= 2) {
        ieee802154_hal_set_enh_ack_prearm_lead((uint16_t)strtoul(argv[1], NULL, 0));
    }
    if (argc >= 3) {
        ieee802154_hal_set_enh_ack_prearm((uint8_t)strtoul(argv[2], NULL, 0));
    }
    printf("enh-ack lead %u us, prearm mode %u (0 off, 1 measure only, 2 prearm, 3 prearm only)\n",
           (unsigned)ieee802154_hal_get_enh_ack_prearm_lead(), (unsigned)ieee802154_hal_get_enh_ack_prearm());
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_radio_ack_lead, radio_ack_lead, Enh-ACK least trigger lead us and prearm mode [us [mode]]);

SHELL_CMD_EXPORT_ALIAS(cmd_radio_stats, radio_stats,
    Dump 802.15.4 TX/RX/ACK counters at OT adapter and lmac154 HAL);
SHELL_CMD_EXPORT_ALIAS(cmd_radio_stats_reset, radio_stats_reset,
    Reset 802.15.4 debug counters);
SHELL_CMD_EXPORT_ALIAS(cmd_radio_trace, radio_trace,
    HAL frame trace: on off clear dump);
SHELL_CMD_EXPORT_ALIAS(cmd_radio_fpt, radio_fpt,
    Dump SrcMatch / Frame Pending Table both SW cache and HW state);
SHELL_CMD_EXPORT_ALIAS(cmd_radio_promisc, radio_promisc,
    Toggle radio promiscuous mode on|off (bypass MAC address filter));
SHELL_CMD_EXPORT_ALIAS(cmd_radio_cca_probe, radio_cca,
    Run lmac154 CCA N times and report idle ratio + RSSI distribution);
SHELL_CMD_EXPORT_ALIAS(cmd_radio_cca_threshold, radio_cca_th,
    Set CCA ED threshold in dBm (live, no rebuild));
SHELL_CMD_EXPORT_ALIAS(cmd_radio_cca_mode, radio_cca_mode,
    Set CCA mode 0=ED 1=CS 2=ED-AND-CS 3=ED-OR-CS);
SHELL_CMD_EXPORT_ALIAS(cmd_radio_csma_be, radio_csma_be,
    Set CSMA-CA backoff exponents max min);
SHELL_CMD_EXPORT_ALIAS(cmd_radio_csma_max, radio_csma_max,
    Set max CSMA backoff iterations);
SHELL_CMD_EXPORT_ALIAS(cmd_radio_crc_report, radio_crc_report, Report CRC-failed frames to the HAL counters on|off);
SHELL_CMD_EXPORT_ALIAS(cmd_radio_force_no_csma, radio_force_no_csma,
    DIAGNOSTIC: bypass CSMA-CA entirely on TX (spec violating));
#ifdef CONFIG_IPRO7
SHELL_CMD_EXPORT_ALIAS(cmd_radio_xtal, radio_xtal, RF crystal capcode trim and eFuse slots [capin [capout]]);
#endif
SHELL_CMD_EXPORT_ALIAS(cmd_radio_ack_wait, radio_ack_wait, MAC ACK wait in us for imm-ACK and Enh-ACK frames [imm [enh]]);
SHELL_CMD_EXPORT_ALIAS(cmd_radio_cfo, radio_cfo, Per-sender RX frequency offset and SFD correlation [clr]);
SHELL_CMD_EXPORT_ALIAS(cmd_radio_rx_log, radio_rx_log,
    Toggle hex dump of every received MPDU on|off (diagnostic));
#endif /* CONFIG_SHELL */
