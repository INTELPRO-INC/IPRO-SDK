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

/*
 * ipronet_sdio.c - SMID wire-protocol transport of the ipronet SDIO-WiFi host.
 *
 * IPRO7 is the SDIO host; the IPRO6S device runs the SMID (SDIO3.0) WiFi bridge
 * and presents an FBR std-IF 7 (WLAN) function. This file enumerates + sets up
 * F1 (4-bit / 25 MHz / block size 512), then moves WiFi L2 traffic over the SMID
 * self-framing wire protocol. It also owns the SDH pinmux, the device rail
 * (power-cycle), the doorbell-driven RX task and the data-ready GPIO IRQ.
 *
 * SMID wire protocol
 * ------------------
 * UPLOAD (device->host):
 *   - Doorbell: CMD52 read Fn1 0x00 (lo) + 0x01 (hi) -> u16 LE cnt = byte count
 *     of the head transfer. 0 = nothing pending. One doorbell = ONE transfer.
 *   - Read: ONE block-mode CMD53 of cnt bytes at the fixed Fn1 FIFO address 0
 *     (never byte-mode — that races the device READ_START ISR). cnt is already
 *     512-aligned; round up defensively and count an anomaly if it was not.
 *   - Content: exactly ONE 12-byte ipronet_top_msg header + payload. DATA (0xABCD)
 *     -> lwIP; CMD (0xEF01) -> ipronet_ctrl_handle.
 * DOWNLOAD (host->device):
 *   - Aggregate: [2B LE frame_len][body]... repeated, 0x0000 sentinel, zero-pad
 *     to a 512 multiple, ONE block-mode CMD53 at the fixed FIFO address 0.
 *   - body = [u16 type][u16 len][u16 subtype (DATA only)][payload].
 *
 * Flow control is OPEN-LOOP: there is NO host-visible TX credit. Fn1 0x60/0x70
 * ALIAS the read-count register — reading them is a proven catastrophic bug, so
 * NEVER read 0x60/0x70. The device back-pressures via DAT0 busy (the hal bounds
 * the CMD53 wait) and drops on pool-full like a NIC ring.
 *
 * Pin assignment (IPRO7 SDH bus):
 *   GPIO28..33 = SDH bus (CMD/CLK/DAT0..3) [GPIO_FUN_SDC]
 */

#include <stdio.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include "hal_sdio.h"
#include "hal_mtimer.h"          /* mtimer_get_time_us(): 64-bit microseconds */
#include "drv_glb.h"
#include "drv_gpio.h"
#include "compiler/common.h"     /* ATTR_PSRAM_NOINIT_SECTION */

#include "ipronet_priv.h"

/* SMID identify — the SMID CIS carries vendor/device 0:0 (CISTPL_MANFID
 * reads 0x0000/0x0310 on silicon), so the device is identified by the FBR
 * standard SDIO interface code instead (FBR1+0x00 bits[3:0], 0x07 = WLAN),
 * exactly like Linux ipronet's sdio_device_id { .class = 0x07 } match. */
#define IPRO6S_FBR_STD_IF_WLAN   0x07U
/* IPRONET_FUNC_DATA / IPRONET_FUNC_BLOCK_SIZE live in ipronet_host_diag.h.
 * 512 = sweep-verified best on SMID (Linux ipro_iotnet 27d0160: eliminates
 * intermittent upld bus-CRC, best stable throughput). */

/* SMID upload doorbell (Fn1 read-count register, IPRONET_DOORBELL_LO/HI).
 * CMD52 read addr 0x00 (lo) + 0x01 (hi) -> u16 LE = byte count of the current
 * head upload transfer. 0 = no upload pending. Reading it does NOT pop the
 * transfer; after the host reads the transfer's data the next transfer's count
 * appears. */

/* Fixed Fn1 FIFO address (IPRONET_FIFO_ADDR) for every CMD53 upload read and
 * download write. SMID has no per-port addressing — one FIFO, block-mode, fixed
 * address (OP_CODE=0).
 *
 * NEVER read Fn1 0x60/0x70: on SMID those ALIAS the read-count doorbell and
 * reading them is a proven catastrophic bug (see project_ipro6s_if_sustained_
 * load_stall). The custom-reg window 0x48/0x4C reads 0. There is therefore NO
 * host-visible TX credit — flow control is open-loop (see the TX section). */

/* Buffer sizing. ZCEN3 can chain four 2048-byte upload slots (IPRO6S_ZC3_*,
 * ipronet_priv.h) into one transfer (IPRONET_RX_BUF_SIZE); legacy upload
 * transfers still occupy only one slot. TX single-frame = one MTU frame plus
 * [2B len]+[2B sentinel], padded to 512. */
#define IPRO6S_TX_BUF_SIZE       2048U

static SDIO_Host_Type s_host;
static int s_initted = 0;

/* RX buffer: one SMID upload transfer. Device transfers are <= 2048 by
 * construction; the 6144 cap mirrors the Linux host (cheap insurance). Must be
 * 4-byte aligned for ADMA2 (CMD53 requirement); cache-line(64) aligned in PSRAM
 * like the in-tree SD-card DMA buffers. */
static ATTR_PSRAM_NOINIT_SECTION __attribute__((aligned(64))) uint8_t s_rx_buf[IPRONET_RX_BUF_SIZE];

/* RX counters — surface via ipronet_diag_rx_stats. */
static volatile uint32_t s_doorbell_polls    = 0;
static volatile uint32_t s_doorbell_zero     = 0;
static volatile uint32_t s_rx_transfers      = 0;   /* CMD53 upload reads */
static volatile uint64_t s_rx_bytes          = 0;   /* device-reported cnt bytes */
static volatile uint32_t s_rx_pdus           = 0;   /* valid ipronet_top_msg headers */
static volatile uint32_t s_rx_data_frames    = 0;   /* type=0xabcd subtype=0x2001 */
static volatile uint32_t s_rx_amsdu_drop     = 0;   /* is_amsdu=1 — skipped */
static volatile uint32_t s_rx_bad_type       = 0;
static volatile uint32_t s_rx_bad_subtype    = 0;
static volatile uint32_t s_rx_desync         = 0;   /* has_twin / pld_off / len violations */
static volatile uint32_t s_rx_unaligned_cnt  = 0;   /* doorbell cnt not 512-aligned */
static volatile uint16_t s_last_pdu_len      = 0;
static volatile uint16_t s_last_pdu_subtype  = 0;

/* ---- rx_task stage census (bench instrumentation) --------------------------
 * Where rx_task's time goes: each stage of the loop is bracketed with the
 * free-running us timer (sum/count/max), plus a census of how the loop went
 * idle (hot-spin tick, data-ready sem, backstop timeout, drain budget). The
 * cost is four timer reads per iteration. Printed off the hot path by
 * ipronet_diag_rx_profile; it is what showed the CMD53 wait eating the CPU. */
enum { RXP_CLAIM, RXP_DOORBELL, RXP_CMD53, RXP_DISPATCH, RXP_IDLE, RXP_N };
static volatile uint64_t s_rxp_us[RXP_N];
static volatile uint32_t s_rxp_cnt[RXP_N];
static volatile uint32_t s_rxp_max[RXP_N];
static volatile uint32_t s_rxp_wake_hotspin = 0;   /* doorbell 0 inside hot window */
static volatile uint32_t s_rxp_wake_sem_ok  = 0;   /* GPIO0 edge woke the sem */
static volatile uint32_t s_rxp_wake_sem_to  = 0;   /* backstop timeout */
static volatile uint32_t s_rxp_wake_budget  = 0;   /* drain budget yields */
static volatile uint32_t s_rxp_dispatch_1ms = 0;   /* dispatch calls >= 1 ms */

static inline void rxp_add(int stage, uint64_t t0, uint64_t t1)
{
    uint32_t d = (uint32_t)(t1 - t0);
    s_rxp_us[stage] += d;
    s_rxp_cnt[stage]++;
    if (d > s_rxp_max[stage]) s_rxp_max[stage] = d;
}

/* TX single-frame buffer: [2B len][body][0x0000 sentinel] padded to 512. */
static ATTR_PSRAM_NOINIT_SECTION __attribute__((aligned(64))) uint8_t s_tx_buf[IPRO6S_TX_BUF_SIZE];

/* TX counters. */
static volatile uint32_t s_tx_writes      = 0;   /* CMD53 writes (single + agg)   */
static volatile uint32_t s_tx_agg_frames  = 0;   /* frames delivered across writes */
static volatile uint64_t s_tx_bytes       = 0;   /* on-wire bytes (padded blocks)  */
static volatile uint32_t s_tx_errors      = 0;
static volatile uint32_t s_tx_cmd_frames  = 0;   /* type=0xef01 */
static volatile uint32_t s_tx_eth_frames  = 0;   /* type=0xabcd */

/* TX write latency (mtimer is a free-running us counter). */
static volatile uint64_t s_tx_write_us     = 0;
static volatile uint32_t s_tx_write_max_us = 0;

/* ---- TX aggregation --------------------------------------------------------
 * Pack up to a few DATA frames into ONE CMD53 write, amortising the per-write
 * SDH DATA_COMPLETED latency. Each frame unit = [2B body_len][body], identical
 * to the single-frame layout; the batch ends with a [2B 0x0000] sentinel and is
 * zero-padded to a 512 multiple. The device (trcver_sdio_ipro6.c smid_rx_process)
 * parses frames until the sentinel. All writes go to the fixed Fn1 FIFO address
 * 0 — SMID has no download ports. IPRO6S_AGG_TX_SIZE MUST equal the device
 * SDIO_DNLD_AGG_BUF_SIZE (6144). */
#define IPRO6S_AGG_TX_SIZE     6144U     /* MUST match device SDIO_DNLD_AGG_BUF_SIZE */
#define IPRO6S_AGG_FLUSH_MS    2U        /* deferred flush bound for a partial batch */
#define IPRO6S_AGG_SMALL_FRAME 256U      /* frames < this bypass agg (TCP ACKs) */
/* Frames/batch cap — runtime tunable (ipronet_diag_set_aggmax) for A/B. It caps
 * both negotiated ZCEN2 slots and the legacy batch; 0 bypasses aggregation. */
static uint32_t s_tx_agg_max = 8U;

/* ZCEN2 stride packing: one frame per fixed 2048B wire stride, each landing in
 * its own device zero-copy slot via the device's ADMA scatter chain — keeps the
 * zero-copy that ZCEN1 buys while amortising the per-CMD53 cost over nslot
 * frames. Armed only by a "ZCOK2 <nslot> <stride>" grant (IPRO6S_ZC2_*,
 * ipronet_priv.h). */
#define IPRO6S_TX_AGG_BUF_SIZE (IPRO6S_ZC2_MAX_SLOT * IPRO6S_ZC2_STRIDE)

static volatile uint32_t s_zc2_nslot  = 0;   /* 0/1 = stride packing off */
static volatile uint32_t s_zc2_stride = 0;
static volatile uint32_t s_zc2_warmup = 0;   /* first writes stay <= legacy cap */
static volatile uint32_t s_zc3_uslots = 0;   /* 0 = legacy one-PDU upload wire */

static ATTR_PSRAM_NOINIT_SECTION __attribute__((aligned(64)))
    uint8_t s_tx_agg_buf[IPRO6S_TX_AGG_BUF_SIZE];
static uint32_t   s_tx_agg_off = 0;       /* bytes packed (excl. sentinel) */
static uint32_t   s_tx_agg_cnt = 0;       /* frames packed */
static TickType_t s_tx_agg_t0  = 0;       /* tick of first frame in batch */
static SemaphoreHandle_t s_tx_agg_mtx = NULL;
static volatile uint32_t s_tx_agg_flushes  = 0;   /* CMD53 batch writes */
static volatile uint32_t s_tx_agg_to_flush = 0;   /* timeout-triggered flushes */

/* GPIO IRQ side-channel (device GPIO0 -> host GPIO0). The device pulses a rising
 * edge at each upload data-ready; the ISR gives this binary sem so rx_task wakes
 * immediately instead of waiting out the poll backstop. Default OFF (GPIO0 stays
 * masked in ipro6s_data_ready_irq_init); the sem timeout is then the doorbell
 * poll cadence. Keep the timeout bounded — it is the lost-wakeup backstop. */
#define IPRO6S_DR_GPIO_PIN        0
#define IPRO6S_RX_BACKSTOP_MS     2U   /* RX doorbell poll cadence when IRQ off */
#define IPRO6S_RX_HOT_SPIN_US     3000U /* yield-poll window after last data before sem-wait */
#define IPRO6S_RX_DRAIN_BUDGET    8U   /* transfers before tcpip gets one tick */
static SemaphoreHandle_t s_rx_data_ready_sem = NULL;

/* rx_task priority. IPRO7's SDH interrupt does not work (polling only), so
 * sdio_read_blocks() polls the controller for the whole bus transfer, ~300 us
 * for a 3 KB upload. Polled at this top priority that was ~65% of the CPU in
 * a UDP-RX overload cell; the lwIP consumer below it got ~8% and ~60% of the
 * datagrams that had already crossed the bus were thrown away at the socket
 * queue (MEMP_NETBUF exhausted in recv_udp). rx_task therefore registers
 * with the HAL (sdio_set_wait_demotion) to have that poll run at IDLE+1:
 * the command still goes out at top priority, the bus transfer overlaps the
 * consumer's work, and the poll only burns cycles nobody else wants. */
#define IPRO6S_RX_TASK_PRIO       (configMAX_PRIORITIES - 2)
static int s_rx_xfer_lowprio = 1;      /* A/B: 0 = poll at IPRO6S_RX_TASK_PRIO */

/* TX aggregation policy helpers (were the app's ipro6s_tx_agg_policy.h). */
static inline uint32_t ipro6s_tx_agg_grant_cap(uint32_t negotiated_slots)
{
    return negotiated_slots;
}

static inline uint32_t ipro6s_tx_agg_slot_limit(uint32_t negotiated_slots,
                                                uint32_t configured_max)
{
    if (configured_max < negotiated_slots)
        return configured_max;
    return negotiated_slots;
}

typedef struct {
    uint32_t configured_max;
    uint32_t stride_slots;
} ipro6s_tx_agg_snapshot_t;

typedef enum {
    IPRO6S_TX_AGG_SINGLE,
    IPRO6S_TX_AGG_LEGACY,
    IPRO6S_TX_AGG_STRIDE,
} ipro6s_tx_agg_mode_t;

static inline ipro6s_tx_agg_snapshot_t
ipro6s_tx_agg_snapshot(uint32_t configured_max, uint32_t negotiated_slots)
{
    ipro6s_tx_agg_snapshot_t snapshot = {
        .configured_max = configured_max,
        .stride_slots = ipro6s_tx_agg_slot_limit(negotiated_slots,
                                                 configured_max),
    };

    return snapshot;
}

static inline ipro6s_tx_agg_mode_t
ipro6s_tx_agg_mode(ipro6s_tx_agg_snapshot_t snapshot)
{
    if (snapshot.configured_max == 0)
        return IPRO6S_TX_AGG_SINGLE;
    if (snapshot.stride_slots != 0)
        return IPRO6S_TX_AGG_STRIDE;
    return IPRO6S_TX_AGG_LEGACY;
}

/* ------------------------------------------------------------------ */
/* Pinmux                                                              */
/* ------------------------------------------------------------------ */

static void sdh_pinmux_init(void)
{
    /* GPIO28-33 = SDC function. CMD line (GPIO30) stays without pull-up
     * — the SDH controller drives it; pull-up creates contention. */
    GLB_GPIO_Cfg_Type cfg = {
        .gpioMode = GPIO_MODE_AF,
        .pullType = GPIO_PULL_UP,
        .drive    = 2,
        .smtCtrl  = 1,
        .gpioFun  = GPIO_FUN_SDC,
    };
    const uint8_t pins[] = {
        GLB_GPIO_PIN_28, GLB_GPIO_PIN_29, GLB_GPIO_PIN_30,
        GLB_GPIO_PIN_31, GLB_GPIO_PIN_32, GLB_GPIO_PIN_33,
    };
    for (size_t i = 0; i < sizeof(pins) / sizeof(pins[0]); i++) {
        cfg.gpioPin  = pins[i];
        cfg.pullType = (pins[i] == GLB_GPIO_PIN_30) ? GPIO_PULL_NONE
                                                    : GPIO_PULL_UP;
        GLB_GPIO_Init(&cfg);
    }

    GLB_Set_SDH_CLK(1, GLB_SDH_CLK_PLL_96M, 1);
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_SDH);

    /* GLB_GPIO_Init() clears OE for AF-mode pins (see the SPI1 master
     * bring-up memo). For SPI1 we have to re-assert OE manually. For
     * GPIO_FUN_SDC, CMD52 already works before this line runs, so the
     * SDH pad-mux clearly drives CMD/CLK regardless of the OE bit.
     * Set OE on the data-direction pins anyway as defensive hygiene
     * — costs one register write per pin and is harmless if the SDC
     * function path already overrides the bit. */
    for (size_t i = 0; i < sizeof(pins) / sizeof(pins[0]); i++) {
        GLB_GPIO_OUTPUT_Enable(pins[i]);
    }
}

/* ------------------------------------------------------------------ */
/* IPRO6 device rail (no relay on this bench)                          */
/*                                                                     */
/* The IPRO6 DUT runs off the SD-card 3.3V rail, switched by an IPRO7  */
/* GPIO (EVB: GPIO15, SD PWR EN, 0 = rail ON; both come from the       */
/* config). "Reset" is a power cycle.                                  */
/*                                                                     */
/* During the rail-off window ALL six SDH pads are parked as floating */
/* inputs — the SDH pull-ups would otherwise back-power the dead DUT  */
/* through its IO ring and the rail never fully discharges.           */
/* ------------------------------------------------------------------ */

#define IPRO6_STRAP_HOLD_MS    200U             /* strap hold past power-on */

static int8_t  s_pwr_gpio = -1;
static uint8_t s_pwr_active_low = 1;

void ipronet_sdio_power_configure(int8_t gpio, uint8_t active_low)
{
    s_pwr_gpio = gpio;
    s_pwr_active_low = active_low;
}

static void ipro6_gpio_out(uint8_t pin, uint32_t level)
{
    GLB_GPIO_Cfg_Type cfg = {
        .gpioPin  = pin,
        .gpioFun  = GPIO_FUN_GPIO,
        .gpioMode = GPIO_MODE_OUTPUT,
        .pullType = GPIO_PULL_NONE,
        .drive    = 0,
        .smtCtrl  = 1,
    };
    GLB_GPIO_Write(pin, level);   /* data reg first — no glitch on OE */
    GLB_GPIO_Init(&cfg);
    GLB_GPIO_OUTPUT_Enable(pin);
}

static void ipro6_pwr_set(int on)
{
    uint32_t level = on ? 1U : 0U;
    if (s_pwr_active_low) level ^= 1U;
    ipro6_gpio_out((uint8_t)s_pwr_gpio, level);
}

static void ipro6_sdh_pads_park(void)
{
    GLB_GPIO_Cfg_Type cfg = {
        .gpioFun  = GPIO_FUN_GPIO,
        .gpioMode = GPIO_MODE_INPUT,
        .pullType = GPIO_PULL_NONE,
        .drive    = 0,
        .smtCtrl  = 1,
    };
    static const uint8_t pins[] = {
        GLB_GPIO_PIN_28, GLB_GPIO_PIN_29, GLB_GPIO_PIN_30,
        GLB_GPIO_PIN_31, GLB_GPIO_PIN_32, GLB_GPIO_PIN_33,
    };
    for (size_t i = 0; i < sizeof(pins) / sizeof(pins[0]); i++) {
        cfg.gpioPin = pins[i];
        GLB_GPIO_Init(&cfg);
    }
}

/* Rail off parks the six SDH pads first (their pull-ups would back-power the
 * dead device through its IO ring). Rail on holds 200 ms, then restores the
 * SDH pinmux; that hold is also the window an app-level BROM strap needs. */
int ipronet_sdio_power(int on)
{
    if (s_pwr_gpio < 0) return IPRONET_ERR_STATE;
    if (!on) {
        ipro6_sdh_pads_park();
        ipro6_pwr_set(0);
        return IPRONET_OK;
    }
    ipro6_pwr_set(1);
    vTaskDelay(pdMS_TO_TICKS(IPRO6_STRAP_HOLD_MS));
    sdh_pinmux_init();
    return IPRONET_OK;
}

/* ------------------------------------------------------------------ */
/* RX dispatch                                                         */
/* ------------------------------------------------------------------ */

/* Parse one PDU. Caller has just CMD53-read the transfer. Walks the
 * ipronet_top_msg header, classifies, and dispatches: CMD -> the control plane
 * (ipronet_ctrl_handle, pld points at the ipronet_ctrl_hdr_t), DATA -> lwIP. */
static void ipro6s_rx_handle_pdu(const uint8_t *pdu, uint16_t cnt)
{
    const struct ipronet_top_msg *m = (const struct ipronet_top_msg *)pdu;

    /* SMID transfer = exactly ONE 12-byte header + payload. Validate the header
     * geometry before dispatch; any violation is a stream desync (drop + count). */
    if (cnt < sizeof(struct ipronet_top_msg)) {
        s_rx_desync++;
        return;
    }

    uint16_t type    = (uint16_t)m->type_lsb | ((uint16_t)m->type_msb << 8);
    uint16_t len     = (uint16_t)m->len_lsb  | ((uint16_t)m->len_msb  << 8);
    uint16_t subtype = (uint16_t)m->subtype_lsb | ((uint16_t)m->subtype_msb << 8);
    uint8_t  pld_off = m->pld_off;

    /* has_twin is always 0 on SMID; a set bit means the stream desynced. */
    if (m->has_twin ||
        pld_off < sizeof(struct ipronet_top_msg) ||
        (uint32_t)pld_off + len > cnt) {
        s_rx_desync++;
        return;
    }

    s_rx_pdus++;
    s_last_pdu_len     = len;
    s_last_pdu_subtype = subtype;

    /* Control plane: device->host CMD/EVENT frame (connect/disconnect/IP/scan). */
    if (type == IPRONET_MSG_TYPE_CMD) {
        if (len >= sizeof(ipronet_ctrl_hdr_t)) {
            ipronet_ctrl_handle(pdu + pld_off, len);
        } else {
            s_rx_bad_type++;
        }
        return;
    }

    if (type != IPRONET_MSG_TYPE_DATA) {
        s_rx_bad_type++;
        return;
    }
    if (m->is_amsdu) {
        s_rx_amsdu_drop++;
        return;
    }
    if (subtype != IPRONET_MSG_SUBTYPE_STA_RX) {
        s_rx_bad_subtype++;
        return;
    }

    s_rx_data_frames++;
    const uint8_t *eth = pdu + pld_off;

    /* Once iproeth0 is up, hand the frame to lwIP. Otherwise (pre-netif
     * bring-up) log DA/SA/ethertype for the first 8 frames as a sniffer. */
    if (ipronet_netif_is_up()) {
        ipronet_netif_input(eth, len);
    } else if (s_rx_data_frames <= 8U && len >= 14U) {
        printf("[rx] len=%u pld_off=%u "
               "DA=%02X:%02X:%02X:%02X:%02X:%02X "
               "SA=%02X:%02X:%02X:%02X:%02X:%02X "
               "eth=0x%02X%02X\r\n",
               len, pld_off,
               eth[0], eth[1], eth[2], eth[3], eth[4], eth[5],
               eth[6], eth[7], eth[8], eth[9], eth[10], eth[11],
               eth[12], eth[13]);
    }
}

/* ZCEN3 changes every device->host transfer to fixed 2048-byte strides:
 * [2B LE PDU length][ipronet_top_msg + payload][padding]. The ZCOK3 response is
 * still legacy wire; its handler arms s_zc3_uslots before the RX task reads the
 * next transfer, so this dispatcher has one stable format per transfer. */
static void ipro6s_rx_handle_transfer(const uint8_t *pdu, uint16_t cnt)
{
    if (s_zc3_uslots == 0) {
        ipro6s_rx_handle_pdu(pdu, cnt);
        return;
    }

    uint32_t slot = 0;
    for (uint32_t off = 0;
         off + 2U <= cnt && slot < s_zc3_uslots;
         off += IPRO6S_ZC3_STRIDE, slot++) {
        uint16_t wlen = (uint16_t)pdu[off] | ((uint16_t)pdu[off + 1U] << 8);

        if (wlen == 0) {
            break;
        }
        if (wlen > IPRO6S_ZC3_STRIDE - 2U || off + 2U + wlen > cnt) {
            s_rx_desync++;
            return;
        }
        ipro6s_rx_handle_pdu(pdu + off + 2, wlen);
    }
}

/* ------------------------------------------------------------------ */
/* SDH host init / enumerate / setup                                   */
/* ------------------------------------------------------------------ */

int ipronet_sdio_init(void)
{
    if (s_initted) return IPRONET_OK;
    sdh_pinmux_init();

    SDIO_Status_Type st = sdio_host_init(&s_host, 0);
    if (st != Status_SDIO_Success) {
        printf("ipronet: host_init fail (%d)\r\n", (int)st);
        return IPRONET_ERR_SETUP;
    }
    s_initted = 1;
    if (s_tx_agg_mtx == NULL) s_tx_agg_mtx = xSemaphoreCreateMutex();  /* TX aggregation */
    printf("ipronet: host_init OK (sd_id=0)\r\n");
    return IPRONET_OK;
}

int ipronet_sdio_enum(void)
{
    if (!s_initted) return IPRONET_ERR_STATE;

    SDIO_Status_Type st = sdio_card_enumerate(&s_host);
    if (st != Status_SDIO_Success) {
        printf("ipro6s: enumerate fail (%d) — is DUT in `sdiowifi` state?\r\n",
               (int)st);
        return IPRONET_ERR_NO_DEVICE;
    }
    printf("ipro6s: enumerate OK\r\n");
    printf("  rca       = 0x%04X\r\n", s_host.rca);
    printf("  num_funcs = %u\r\n", s_host.num_funcs);
    printf("  ocr       = 0x%08lX\r\n", (unsigned long)s_host.ocr);
    printf("  cis_ptr   = 0x%06lX\r\n", (unsigned long)s_host.common_cis_ptr);

    int ok = 1;
    for (uint8_t f = 1; f <= s_host.num_funcs; f++) {
        SDIO_Func_Info_Type *fi = &s_host.funcs[f];
        printf("  F%u: cis=0x%06lX class=0x%02X manf=0x%04X prod=0x%04X "
               "max_blk=%u\r\n",
               f, (unsigned long)fi->cis_ptr, fi->func_class,
               fi->manf_id, fi->prod_id, fi->max_blk_size);

        if (f == IPRONET_FUNC_DATA) {
            /* NOTE: func_class printed above is CISTPL_FUNCID body[0], which
             * is always 0x0C ("SDIO function") per PC-Card FUNCID codes — it
             * is NOT the std-IF code. The WLAN class lives in FBR bits[3:0]
             * (0x0F there = extended code at FBR+0x01). */
            uint8_t fbr0 = 0, std_if = 0;
            st = sdio_readb(&s_host, SDIO_FUNC_0, SDIO_FBR_BASE(f), &fbr0);
            if (st != Status_SDIO_Success) {
                printf("  F%u: FBR std-IF read fail (%d)\r\n", f, (int)st);
                ok = 0;
                continue;
            }
            std_if = fbr0 & 0x0FU;
            if (std_if == 0x0FU) {
                (void)sdio_readb(&s_host, SDIO_FUNC_0, SDIO_FBR_BASE(f) + 1U,
                                 &std_if);
            }
            if (std_if == IPRO6S_FBR_STD_IF_WLAN) {
                printf("  F%u: FBR std-IF=0x%02X (WLAN) — SMID device identified\r\n",
                       f, std_if);
            } else {
                printf("  F%u WARN: FBR std-IF=0x%02X, expected 0x07 (WLAN/SMID)\r\n",
                       f, std_if);
                ok = 0;
            }
        }
    }
    return ok ? IPRONET_OK : IPRONET_ERR_NO_DEVICE;
}

int ipronet_sdio_setup(uint32_t bus_hz)
{
    if (!s_initted || s_host.num_funcs == 0) {
        printf("run ipro6s_init + ipro6s_enum first\r\n");
        return IPRONET_ERR_STATE;
    }
    if (bus_hz != 25000000U && bus_hz != 50000000U) return IPRONET_ERR_PARAM;

    /* 50 MHz -> SDR50 / 50 MHz high-speed (2x CMD53 transfer rate).
     * Default stays SDR25 / 25 MHz. */
    bool want_hs = (bus_hz >= 50000000U);
    SDIO_Status_Type st;

    if (want_hs) {
        /* High Speed = 50 MHz @ 3.3V via device EHS + host HI_SPEED_EN (NOT UHS
         * SDR50, which needs 1.8V). Set EHS on the device while still at 25 MHz;
         * sdio_set_bus_width below applies HI_SPEED_EN + 50 MHz in one
         * SDH_Ctrl_Init (its CCCR CMD52 still runs at 25 MHz). Raising the clock
         * before HI_SPEED_EN (as sdio_set_high_speed does) fails the next
         * CMD52 with 211. */
        uint8_t hs = 0;
        st = sdio_readb(&s_host, SDIO_FUNC_0, SDIO_CCCR_HIGHSPEED, &hs);
        if (st == Status_SDIO_Success && (hs & 0x01U)) {
            st = sdio_writeb(&s_host, SDIO_FUNC_0, SDIO_CCCR_HIGHSPEED, hs | 0x02U);
            printf("  high-speed: device EHS set (CCCR HS 0x%02X) -> %d\r\n",
                   hs, (int)st);
            if (st == Status_SDIO_Success) s_host.high_speed = 1;
            else want_hs = false;
        } else {
            printf("  high-speed: card rejects HS (CCCR HS 0x%02X) — using 25 MHz\r\n", hs);
            want_hs = false;
        }
    }

    st = sdio_set_bus_width(&s_host, SDIO_BUS_WIDTH_4BIT);
    printf("  set_bus_width(4%s) -> %d\r\n",
           s_host.high_speed ? ", HS 50 MHz" : ", 25 MHz", (int)st);
    if (st != Status_SDIO_Success) return IPRONET_ERR_SETUP;

    if (!want_hs) {
        st = sdio_set_clock(&s_host, 25000000U);
        printf("  set_clock(25 MHz) -> %d\r\n", (int)st);
        if (st != Status_SDIO_Success) return IPRONET_ERR_SETUP;
    }

    st = sdio_set_block_size(&s_host, IPRONET_FUNC_DATA, IPRONET_FUNC_BLOCK_SIZE);
    printf("  set_block_size(F1, %u) -> %d\r\n", IPRONET_FUNC_BLOCK_SIZE, (int)st);
    if (st != Status_SDIO_Success) return IPRONET_ERR_SETUP;

    st = sdio_enable_func(&s_host, IPRONET_FUNC_DATA);
    printf("  enable_func(F1) -> %d\r\n", (int)st);
    if (st != Status_SDIO_Success) return IPRONET_ERR_SETUP;

    /* SMID F1 init is COMPLETE here — mirror Linux ipronet SMID attach,
     * which does ONLY enable_func + set_block_size (ipro_sdio_eth.c:613-628).
     * The legacy host_int_rsr/mask/status + card_misc_cfg writes were removed:
     * those Fn1 offsets do not exist on SMID (0x00/0x01 is the read-count
     * doorbell; nearby offsets alias other registers) — never write them. */
    printf("ipro6s: setup complete (4-bit, %u MHz, F1 blk=%u, F1 enabled)\r\n",
           s_host.high_speed ? 50U : 25U, IPRONET_FUNC_BLOCK_SIZE);
    return IPRONET_OK;
}

bool ipronet_sdio_is_setup(void)
{
    return s_initted && s_host.funcs[IPRONET_FUNC_DATA].enabled;
}

SDIO_Host_Type *ipronet_diag_host(void) { return s_initted ? &s_host : NULL; }
int ipronet_diag_sdio_init(void)            { return ipronet_sdio_init(); }
int ipronet_diag_sdio_enum(void)            { return ipronet_sdio_enum(); }
int ipronet_diag_sdio_setup(uint32_t hz)    { return ipronet_sdio_setup(hz); }

/* ------------------------------------------------------------------ */
/* TX path (SMID) — open-loop; one block-mode CMD53 write @ FIFO 0     */
/* ------------------------------------------------------------------ */
/* Flow control is OPEN-LOOP by design: SMID exposes NO host-visible credit
 * (Fn1 0x60/0x70 alias the read-count doorbell — reading them is a proven
 * catastrophic bug; the custom-reg window 0x48/0x4C reads 0). The device
 * back-pressures via DAT0 busy — a full CMD53 just takes longer, and the hal
 * bounds that wait (g_sdh_data_poll_timeout_ms) — and drops on pool-full like a
 * NIC ring. So there is no credit poll and no wr_bitmap/port dance: build the
 * frame, pad it to a 512 multiple, and write to the fixed FIFO address. */

/* Build + send one PDU as a self-framed SMID download.
 *
 *   [0..1]  body_len LE   (bytes from offset 2 onward, no padding)
 *   [2..3]  msg.type  LE  (0xabcd = ETH_WIFI_FRAME, 0xef01 = CMD)
 *   [4..5]  msg.len   LE  (= body_len)
 *   [6..7]  subtype   LE  (DATA only; CMD elides it)
 *   [N..]   payload
 *   [..]    0x0000 sentinel, then zero pad to a 512 multiple
 *
 * The hal bounds the SDH busy-wait. Returns IPRONET_OK on success,
 * IPRONET_ERR_* on error. */
int ipronet_sdio_send_pdu(uint16_t msg_type, uint16_t subtype,
                          const void *payload, uint16_t payload_len)
{
    if (!ipronet_sdio_is_setup()) return IPRONET_ERR_STATE;

    int subtype_bytes = (msg_type == IPRONET_MSG_TYPE_DATA) ? 2 : 0;
    uint32_t body_len = 4U + (uint32_t)subtype_bytes + payload_len;
    uint32_t total    = 2U + body_len + 2U;          /* prefix + body + sentinel */
    if (total > IPRO6S_TX_BUF_SIZE) {
        printf("[tx] frame too large (body=%lu)\r\n", (unsigned long)body_len);
        s_tx_errors++;
        return IPRONET_ERR_PARAM;
    }

    uint32_t blocks = (total + IPRONET_FUNC_BLOCK_SIZE - 1U) / IPRONET_FUNC_BLOCK_SIZE;
    uint32_t padded = blocks * IPRONET_FUNC_BLOCK_SIZE;

    /* zeroes the [2B] prefix room, the 0x0000 sentinel, and the pad tail */
    memset(s_tx_buf, 0, padded);
    s_tx_buf[0] = body_len & 0xff;
    s_tx_buf[1] = (body_len >> 8) & 0xff;
    s_tx_buf[2] = msg_type & 0xff;
    s_tx_buf[3] = (msg_type >> 8) & 0xff;
    s_tx_buf[4] = body_len & 0xff;
    s_tx_buf[5] = (body_len >> 8) & 0xff;
    uint8_t *pld = &s_tx_buf[6];
    if (subtype_bytes) {
        *pld++ = subtype & 0xff;
        *pld++ = (subtype >> 8) & 0xff;
    }
    if (payload_len) memcpy(pld, payload, payload_len);

    sdio_claim_host(&s_host);
    /* fixed_addr=true (OP_CODE=0) — the FIFO address must not auto-increment.
     * incrementing-mode leaves SMID never asserting DATA_COMPLETED. */
    uint64_t t_w0 = mtimer_get_time_us();
    SDIO_Status_Type st = sdio_write_blocks(&s_host, IPRONET_FUNC_DATA,
                                            IPRONET_FIFO_ADDR, s_tx_buf,
                                            (uint16_t)blocks, true);
    {
        uint32_t w = (uint32_t)(mtimer_get_time_us() - t_w0);
        s_tx_write_us += w;
        if (w > s_tx_write_max_us) s_tx_write_max_us = w;
    }
    if (st != Status_SDIO_Success) {
        printf("[tx] CMD53 write fail (%d)\r\n", (int)st);
        s_tx_errors++;
        sdio_release_host(&s_host);
        return IPRONET_ERR_IO;
    }

    s_tx_writes++;
    s_tx_agg_frames++;
    s_tx_bytes += padded;
    if (msg_type == IPRONET_MSG_TYPE_CMD)       s_tx_cmd_frames++;
    else if (msg_type == IPRONET_MSG_TYPE_DATA) s_tx_eth_frames++;

    sdio_release_host(&s_host);
    return IPRONET_OK;
}

/* Write the current aggregation batch (s_tx_agg_buf[0..off) + 0x0000 sentinel,
 * zero-padded to a 512 multiple) to the fixed FIFO address in ONE CMD53. Caller
 * MUST hold s_tx_agg_mtx. Open-loop — no credit/bitmap/port. Always resets the
 * batch state (success or drop) so the buffer is reusable. */
static int ipro6s_tx_agg_write_locked(void)
{
    if (s_tx_agg_cnt == 0) return 0;
    uint32_t ncnt = s_tx_agg_cnt;
    if (!s_initted || !s_host.funcs[IPRONET_FUNC_DATA].enabled) {
        s_tx_agg_off = 0; s_tx_agg_cnt = 0; return -1;   /* drop the batch */
    }

    /* sentinel terminates the device parse loop */
    s_tx_agg_buf[s_tx_agg_off]     = 0;
    s_tx_agg_buf[s_tx_agg_off + 1] = 0;
    uint32_t total  = s_tx_agg_off + 2U;
    uint32_t blocks = (total + IPRONET_FUNC_BLOCK_SIZE - 1U) / IPRONET_FUNC_BLOCK_SIZE;
    uint32_t padded = blocks * IPRONET_FUNC_BLOCK_SIZE;
    if (padded > total) memset(&s_tx_agg_buf[total], 0, padded - total);

    sdio_claim_host(&s_host);
    uint64_t t_w0 = mtimer_get_time_us();
    SDIO_Status_Type st = sdio_write_blocks(&s_host, IPRONET_FUNC_DATA,
                                            IPRONET_FIFO_ADDR, s_tx_agg_buf,
                                            (uint16_t)blocks, true);
    {
        uint32_t w = (uint32_t)(mtimer_get_time_us() - t_w0);
        s_tx_write_us += w;
        if (w > s_tx_write_max_us) s_tx_write_max_us = w;
    }
    if (st != Status_SDIO_Success) {
        printf("[tx-agg] CMD53 write fail (%d) cnt=%lu\r\n",
               (int)st, (unsigned long)ncnt);
        s_tx_errors++;
        sdio_release_host(&s_host); s_tx_agg_off = 0; s_tx_agg_cnt = 0; return -1;
    }

    s_tx_writes++;
    s_tx_agg_frames += ncnt;
    s_tx_eth_frames += ncnt;
    s_tx_bytes      += padded;
    s_tx_agg_flushes++;
    if (s_zc2_warmup) s_zc2_warmup--;
    sdio_release_host(&s_host);
    s_tx_agg_off = 0; s_tx_agg_cnt = 0;
    return 0;
}

/* Deferred flush of a partial batch (called on timeout from rx_task so a lone
 * sparse frame — ping/ACK — does not stall in the agg buffer). Takes the lock. */
static void ipro6s_tx_agg_flush(void)
{
    if (s_tx_agg_mtx == NULL) return;
    xSemaphoreTake(s_tx_agg_mtx, portMAX_DELAY);
    if (s_tx_agg_cnt) { s_tx_agg_to_flush++; ipro6s_tx_agg_write_locked(); }
    xSemaphoreGive(s_tx_agg_mtx);
}

static void ipro6s_tx_agg_accept_grant(uint32_t nslot, uint32_t stride)
{
    if (s_tx_agg_mtx != NULL) {
        xSemaphoreTake(s_tx_agg_mtx, portMAX_DELAY);
    }
    s_zc2_stride = stride;
    s_zc2_warmup = 8;
    s_zc2_nslot  = nslot;
    s_tx_agg_max = ipro6s_tx_agg_grant_cap(nslot);
    if (s_tx_agg_mtx != NULL) {
        xSemaphoreGive(s_tx_agg_mtx);
    }
}

/* Egress pacer: token bucket on the host->device data path. SDIO carries
 * 68-91 Mbps while the device's air tops out at ~11-13, and sustained ingress
 * past ~20 Mbps drives the device MAC into the AC_TMO/auth-TX wedge that only
 * a power-cycle clears (30M offer = reproducible kill; 20M = survives). Shed
 * the excess HERE, where a drop costs nothing, instead of inside the device.
 * Default 16 Mbps: above every measured delivery ceiling, below the wedge
 * region. Single producer (lwIP tcpip thread), so no locking. 0 = off. */
static volatile uint32_t s_tx_pace_kbps  = 16000;
static volatile uint32_t s_tx_pace_drops = 0;
#define IPRO6S_PACE_BURST_BYTES (24U * 1024U)

static int ipro6s_pace_admit(uint16_t len)
{
    static uint32_t credit, last_us;
    uint32_t kbps = s_tx_pace_kbps;

    if (kbps == 0) return 1;
    uint32_t now = (uint32_t)mtimer_get_time_us();
    uint64_t add = (uint64_t)(now - last_us) * kbps / 8000U;   /* -> bytes */
    last_us = now;
    if (add > IPRO6S_PACE_BURST_BYTES) add = IPRO6S_PACE_BURST_BYTES;
    credit += (uint32_t)add;
    if (credit > IPRO6S_PACE_BURST_BYTES) credit = IPRO6S_PACE_BURST_BYTES;
    if (credit < len) {
        s_tx_pace_drops++;
        return 0;
    }
    credit -= len;
    return 1;
}

/* TX entry for the lwIP netif (ipronet_netif.c). Appends one ethernet frame to
 * the aggregation batch and flushes when the batch is full (by size or count).
 * A partial batch is flushed by rx_task's timeout. Returns 0 (lwIP treats the
 * frame as sent; it is copied into the agg buffer). */
int ipronet_sdio_eth_tx(const uint8_t *frame, uint16_t len)
{
    uint32_t body_len = 4U + 2U + (uint32_t)len;         /* type+msg.len+subtype+payload */
    uint32_t fsz      = 2U + body_len;                   /* [2B len][body] */

    if (!ipro6s_pace_admit(len)) {
        return 0;            /* shed: cheaper here than inside the device MAC */
    }

    if (s_tx_agg_mtx == NULL) {
        return ipronet_sdio_send_pdu(IPRONET_MSG_TYPE_DATA,
                                     IPRONET_MSG_SUBTYPE_STA_TX,
                                     frame, len);
    }

    /* Mode changes and aggregate writes share this lock. A ZCEN request can
     * therefore drain the old wire format, disable aggregation, and send the
     * opt-in without an old-mode TX escaping after it. */
    xSemaphoreTake(s_tx_agg_mtx, portMAX_DELAY);
    ipro6s_tx_agg_snapshot_t tx_agg =
        ipro6s_tx_agg_snapshot(s_tx_agg_max, s_zc2_nslot);
    ipro6s_tx_agg_mode_t tx_mode = ipro6s_tx_agg_mode(tx_agg);
    uint32_t nslot = tx_agg.stride_slots;

    /* ZCEN2 granted: pack at the fixed wire stride instead of back-to-back, so
     * every frame lands in its own device zero-copy slot. The tail of each
     * stride is zeroed — that zero length doubles as the walker's sentinel. */
    if (tx_mode == IPRO6S_TX_AGG_STRIDE &&
        (2U + body_len + 2U) <= s_zc2_stride) {
        uint32_t stride = s_zc2_stride;
        uint32_t cap    = nslot * stride;

        /* Warm-up: stay within the legacy buffer size until the elements the
         * device armed before the grant have been consumed (a bigger write
         * landing on one truncates -> the -110 card-removed cascade). */
        if (s_zc2_warmup && cap > IPRO6S_AGG_TX_SIZE) {
            cap = IPRO6S_AGG_TX_SIZE;
        }

        if ((s_tx_agg_cnt + 1U) * stride > cap) {
            ipro6s_tx_agg_write_locked();
        }
        uint8_t *p = &s_tx_agg_buf[s_tx_agg_cnt * stride];
        p[0] = body_len & 0xff;        p[1] = (body_len >> 8) & 0xff;
        p[2] = IPRONET_MSG_TYPE_DATA & 0xff;   p[3] = (IPRONET_MSG_TYPE_DATA >> 8) & 0xff;
        p[4] = body_len & 0xff;        p[5] = (body_len >> 8) & 0xff;
        p[6] = IPRONET_MSG_SUBTYPE_STA_TX & 0xff;
        p[7] = (IPRONET_MSG_SUBTYPE_STA_TX >> 8) & 0xff;
        if (len) memcpy(&p[8], frame, len);
        memset(&p[2 + body_len], 0, stride - 2U - body_len);

        if (s_tx_agg_cnt == 0) s_tx_agg_t0 = xTaskGetTickCount();
        s_tx_agg_cnt++;
        /* byte count the write path sends: full strides + this frame's tail */
        s_tx_agg_off = (s_tx_agg_cnt - 1U) * stride + 2U + body_len;

        if (s_tx_agg_cnt * stride >= cap) {
            ipro6s_tx_agg_write_locked();
        }
        xSemaphoreGive(s_tx_agg_mtx);
        return 0;
    }

    /* Fallback to the single-frame path if aggregation isn't up yet, the frame
     * + sentinel can't fit one (empty) batch buffer (MTU < 2048 so never in
     * practice), or aggregation is disabled (aggmax 0 = the true
     * pre-aggregation send_pdu path, for A/B: the agg buffer/mutex/rx_task
     * timeout-flush are all bypassed since s_tx_agg_cnt stays 0). */
    if (tx_mode != IPRO6S_TX_AGG_LEGACY ||
        fsz + 2U > IPRO6S_AGG_TX_SIZE) {
        xSemaphoreGive(s_tx_agg_mtx);
        return ipronet_sdio_send_pdu(IPRONET_MSG_TYPE_DATA, IPRONET_MSG_SUBTYPE_STA_TX,
                                     frame, len);
    }

    /* Small frames (TCP pure-ACKs ~54B, control) BYPASS the aggregation path
     * entirely and go straight out via send_pdu — like aggmax=0. Routing an
     * ACK through the agg buffer instead leaves s_tx_agg_cnt>0 momentarily,
     * which lets rx_task's timeout hook grab the agg mutex while the flush
     * holds it across the SDH write → rx_task's RX drain stalls. Flush any
     * queued bulk batch FIRST to preserve in-order delivery (cnt>0 only in
     * mixed TX+ACK, never in pure RX). Large DATA frames aggregate. */
    if (len < IPRO6S_AGG_SMALL_FRAME) {
        if (s_tx_agg_cnt) ipro6s_tx_agg_write_locked();
        xSemaphoreGive(s_tx_agg_mtx);
        return ipronet_sdio_send_pdu(IPRONET_MSG_TYPE_DATA, IPRONET_MSG_SUBTYPE_STA_TX,
                                     frame, len);
    }

    /* If this frame won't fit the current batch (reserve +2 for the sentinel),
     * flush what we have first, then start a fresh batch with this frame. */
    if (s_tx_agg_off + fsz + 2U > IPRO6S_AGG_TX_SIZE) {
        ipro6s_tx_agg_write_locked();
    }
    uint8_t *d = &s_tx_agg_buf[s_tx_agg_off];
    d[0] = body_len & 0xff;        d[1] = (body_len >> 8) & 0xff;
    d[2] = IPRONET_MSG_TYPE_DATA & 0xff;   d[3] = (IPRONET_MSG_TYPE_DATA >> 8) & 0xff;
    d[4] = body_len & 0xff;        d[5] = (body_len >> 8) & 0xff;
    d[6] = IPRONET_MSG_SUBTYPE_STA_TX & 0xff;
    d[7] = (IPRONET_MSG_SUBTYPE_STA_TX >> 8) & 0xff;
    if (len) memcpy(&d[8], frame, len);
    if (s_tx_agg_cnt == 0) s_tx_agg_t0 = xTaskGetTickCount();
    s_tx_agg_off += fsz;
    s_tx_agg_cnt++;

    if (s_tx_agg_cnt >= tx_agg.configured_max) {
        ipro6s_tx_agg_write_locked();
    }
    xSemaphoreGive(s_tx_agg_mtx);
    return 0;
}

/* ZCEN ordering contract (used by ipronet_ctrl_zcen): commit to single-frame TX
 * and drain the pending batch BEFORE telling the device, and keep the same lock
 * across the opt-in so no aggregate selected under the old mode can trail it.
 * The dnld queue is in-order, so once the device processes ZCEN no aggregate
 * may follow it into a 1604B-capped zero-copy slot (arming it under an
 * aggregating host is the documented -110 card-removed cascade). */
void ipronet_sdio_tx_single_frame_lock(void)
{
    if (s_tx_agg_mtx != NULL) xSemaphoreTake(s_tx_agg_mtx, portMAX_DELAY);
    if (s_tx_agg_cnt) ipro6s_tx_agg_write_locked();
    s_zc2_nslot  = 0;
    s_tx_agg_max = 0;
}

void ipronet_sdio_tx_single_frame_unlock(void)
{
    if (s_tx_agg_mtx != NULL) xSemaphoreGive(s_tx_agg_mtx);
}

void ipronet_sdio_tx_agg_flush(void)                              { ipro6s_tx_agg_flush(); }
void ipronet_sdio_tx_agg_accept_grant(uint32_t n, uint32_t s)     { ipro6s_tx_agg_accept_grant(n, s); }
void ipronet_sdio_set_zc3_uslots(uint32_t uslots)                 { s_zc3_uslots = uslots; }
void ipronet_sdio_set_pace(uint32_t kbps)                         { s_tx_pace_kbps = kbps; }
int  ipronet_diag_set_pace(uint32_t kbps)                         { s_tx_pace_kbps = kbps; return IPRONET_OK; }
void ipronet_diag_tx_agg_flush(void)                              { ipro6s_tx_agg_flush(); }
int  ipronet_diag_eth_tx(const uint8_t *f, uint16_t l)            { return ipronet_sdio_eth_tx(f, l); }

/* TX-aggregation batch cap (A/B). 1 flushes every frame; higher values pack up
 * to N frames/CMD53, bounded by the negotiated ZCEN2 slots; 0 bypasses. */
int ipronet_diag_set_aggmax(uint32_t n)
{
    if (s_tx_agg_mtx != NULL) {
        xSemaphoreTake(s_tx_agg_mtx, portMAX_DELAY);
    }
    if (s_tx_agg_cnt) {
        ipro6s_tx_agg_write_locked();
    }
    s_tx_agg_max = n;
    if (s_tx_agg_mtx != NULL) {
        xSemaphoreGive(s_tx_agg_mtx);
    }
    return IPRONET_OK;
}

/* ------------------------------------------------------------------ */
/* RX worker task — SMID doorbell-driven drain                         */
/* ------------------------------------------------------------------ */

static TaskHandle_t s_rx_task = NULL;
static volatile int s_rx_stop = 0;
static void ipro6s_data_ready_irq_init(void);  /* forward decl — defined near GPIO ISR */

static void rx_task(void *arg)
{
    (void)arg;
    /* SMID doorbell-driven RX. Poll the Fn1 read-count doorbell; when it is
     * non-zero, do ONE block-mode CMD53 read of that many bytes at the fixed
     * FIFO address, validate + dispatch the single ipronet_top_msg, then poll
     * again immediately to drain. When the doorbell reads 0, idle-wait on the
     * GPIO0 data-ready sem (edge -> immediate wake) with a short backstop
     * timeout that doubles as the poll cadence when the IRQ side-channel is
     * off (default). */
    printf("[rx] task started (SMID doorbell drain)\r\n");
    if (s_rx_xfer_lowprio)
        sdio_set_wait_demotion(xTaskGetCurrentTaskHandle(), IPRO6S_RX_TASK_PRIO);
    uint64_t last_data_us = 0;
    uint32_t drain_budget = 0;
    while (!s_rx_stop) {
        uint64_t t_top = mtimer_get_time_us();
        /* Flush a partial TX aggregation batch that has waited too long so a
         * sparse DATA frame (ping/ACK) cannot stall in the agg buffer. */
        if (s_tx_agg_cnt &&
            (xTaskGetTickCount() - s_tx_agg_t0) >= pdMS_TO_TICKS(IPRO6S_AGG_FLUSH_MS)) {
            ipro6s_tx_agg_flush();
        }

        sdio_claim_host(&s_host);
        uint64_t t_claimed = mtimer_get_time_us();
        rxp_add(RXP_CLAIM, t_top, t_claimed);

        uint8_t lo = 0, hi = 0;
        s_doorbell_polls++;
        if (sdio_readb(&s_host, IPRONET_FUNC_DATA, IPRONET_DOORBELL_LO, &lo)
                != Status_SDIO_Success ||
            sdio_readb(&s_host, IPRONET_FUNC_DATA, IPRONET_DOORBELL_HI, &hi)
                != Status_SDIO_Success) {
            sdio_release_host(&s_host);
            vTaskDelay(1);
            continue;
        }
        uint16_t cnt = (uint16_t)lo | ((uint16_t)hi << 8);
        uint64_t t_door = mtimer_get_time_us();
        rxp_add(RXP_DOORBELL, t_claimed, t_door);

        if (cnt == 0) {
            s_doorbell_zero++;
            sdio_release_host(&s_host);
            /* Hot window: within an A-MPDU burst the device refills its 8-deep
             * upld queue (~8x1.5KB ~ 1.3ms at air burst rate) FASTER than a
             * tick-granular sem-wait resumes (2 ticks) — the queue overflows
             * and the device drops (sdio_upld_fail). After recent data, keep
             * polling after one scheduler tick, which lets the lower-priority
             * tcpip task consume the pbuf batch we just queued. */
            if ((mtimer_get_time_us() - last_data_us) < IPRO6S_RX_HOT_SPIN_US) {
                drain_budget = 0;
                s_rxp_wake_hotspin++;
                vTaskDelay(1);
                rxp_add(RXP_IDLE, t_door, mtimer_get_time_us());
                continue;
            }
            /* Idle-wait: GPIO0 data-ready edge (if IRQ on) or backstop timeout. */
            if (s_rx_data_ready_sem != NULL) {
                if (xSemaphoreTake(s_rx_data_ready_sem,
                                   pdMS_TO_TICKS(IPRO6S_RX_BACKSTOP_MS)) == pdTRUE)
                    s_rxp_wake_sem_ok++;
                else
                    s_rxp_wake_sem_to++;
            } else {
                vTaskDelay(1);
                s_rxp_wake_sem_to++;
            }
            rxp_add(RXP_IDLE, t_door, mtimer_get_time_us());
            continue;
        }

        /* Round the count up to a 512 multiple (the device pads; count an
         * anomaly if it was not already aligned) and read the whole transfer
         * in ONE block-mode CMD53 at the fixed FIFO address. */
        if (cnt & (IPRONET_FUNC_BLOCK_SIZE - 1U)) s_rx_unaligned_cnt++;
        uint32_t rd = ((uint32_t)cnt + IPRONET_FUNC_BLOCK_SIZE - 1U)
                      & ~(IPRONET_FUNC_BLOCK_SIZE - 1U);
        if (rd == 0 || rd > IPRONET_RX_BUF_SIZE) {
            s_rx_desync++;                       /* impossible by construction */
            sdio_release_host(&s_host);
            continue;
        }
        SDIO_Status_Type st = sdio_read_blocks(&s_host, IPRONET_FUNC_DATA,
                                               IPRONET_FIFO_ADDR, s_rx_buf,
                                               (uint16_t)(rd / IPRONET_FUNC_BLOCK_SIZE),
                                               true);
        if (st != Status_SDIO_Success) {
            printf("[rx] CMD53 read fail cnt=%u (%d)\r\n", cnt, (int)st);
            sdio_release_host(&s_host);
            vTaskDelay(1);
            continue;
        }
        s_rx_transfers++;
        s_rx_bytes += cnt;
        last_data_us = mtimer_get_time_us();
        rxp_add(RXP_CMD53, t_door, last_data_us);

        /* Release the SDIO bus BEFORE handing the frame to lwIP: ipronet_netif_input
         * -> (inline, CORE_LOCKING_INPUT=1) tcpip_input takes lock_tcpip_core, while
         * the TX path takes the locks in the opposite order (lock_tcpip_core ->
         * ipronet_sdio_send_pdu -> sdio_claim_host), so holding the bus across input
         * is an AB-BA deadlock that froze TCP connect. The CMD53 transfer is already
         * complete; input needs no SDIO access. */
        sdio_release_host(&s_host);
        ipro6s_rx_handle_transfer(s_rx_buf, cnt);
        uint64_t t_disp = mtimer_get_time_us();
        rxp_add(RXP_DISPATCH, last_data_us, t_disp);
        if ((t_disp - last_data_us) >= 1000U) s_rxp_dispatch_1ms++;
        /* Bound high-priority draining so the async tcpip consumer runs even
         * while the upload doorbell never reaches zero. */
        if (++drain_budget >= IPRO6S_RX_DRAIN_BUDGET) {
            drain_budget = 0;
            s_rxp_wake_budget++;
            vTaskDelay(1);
            rxp_add(RXP_IDLE, t_disp, mtimer_get_time_us());
        }
        /* Otherwise loop back immediately to drain the next transfer. */
    }
    printf("[rx] task exiting\r\n");
    sdio_set_wait_demotion(NULL, 0);
    s_rx_task = NULL;
    vTaskDelete(NULL);
}

int ipronet_sdio_rx_start(void)
{
    if (!ipronet_sdio_is_setup()) return IPRONET_ERR_STATE;
    if (s_rx_task != NULL) return IPRONET_OK;
    s_rx_stop = 0;

    if (s_rx_data_ready_sem == NULL) {
        s_rx_data_ready_sem = xSemaphoreCreateBinary();
        if (s_rx_data_ready_sem == NULL) return IPRONET_ERR_NOMEM;
    }
    ipro6s_data_ready_irq_init();   /* GPIO0 input + async-rising IRQ (default masked) */

    /* rx_task stack: 3072 words (the agg buffer is static PSRAM, not on-stack). */
    if (xTaskCreate(rx_task, "ipro6s_rx", 3072, NULL,
                    IPRO6S_RX_TASK_PRIO, &s_rx_task) != pdPASS) {
        s_rx_task = NULL;
        return IPRONET_ERR_NOMEM;
    }
    return IPRONET_OK;
}

void ipronet_sdio_rx_stop(void)
{
    if (s_rx_task == NULL) return;
    s_rx_stop = 1;
    for (int i = 0; i < 100 && s_rx_task != NULL; i++) vTaskDelay(pdMS_TO_TICKS(1));
    /* rx_task nulls s_rx_task and deletes itself; 100 ms covers a 5 s stuck
     * CMD53 only if it has already returned - a wedged bus is out of scope. */
}

/* Disable F1, reset the aggregation / zero-copy state and put the SDH
 * controller back into 1-bit ID-mode clock for the next start. The device the
 * next start() enumerates has been power-cycled or rebooted, i.e. is in
 * 1-bit / no-high-speed state itself; keeping the controller at the last
 * session's 4-bit 25/50 MHz would enumerate it out of spec. sdio_host_init()
 * leaks RTOS objects if called twice, so this is sdio_host_reinit(), which
 * redoes only the HW sequence and voids the enumeration state. */
void ipronet_sdio_teardown(void)
{
    ipronet_sdio_rx_stop();
    if (ipronet_sdio_is_setup()) (void)sdio_disable_func(&s_host, IPRONET_FUNC_DATA);
    if (s_tx_agg_mtx != NULL) xSemaphoreTake(s_tx_agg_mtx, portMAX_DELAY);
    s_tx_agg_max = 8U;   /* the compile-time default, restored for the next start */
    s_zc2_nslot = 0; s_zc2_stride = 0; s_zc2_warmup = 0;
    s_tx_agg_off = 0; s_tx_agg_cnt = 0;
    if (s_tx_agg_mtx != NULL) xSemaphoreGive(s_tx_agg_mtx);
    s_zc3_uslots = 0;
    if (s_initted) (void)sdio_host_reinit(&s_host);
}

uint8_t *ipronet_diag_rx_buf(void)
{
    return s_rx_buf;
}

bool ipronet_diag_rx_task_running(void) { return s_rx_task != NULL; }
int  ipronet_diag_rx_start(void)        { return ipronet_sdio_rx_start(); }

/* ---- GPIO IRQ side-channel -----------------------------------------------
 * Wire: device GPIO0 (driven by the device's sdu_send_data data-ready pulse)
 * -> host GPIO0 (this input + IRQ). Replaces (augments) the rx_task RD-bitmap
 * CMD52 polling — the workaround for the IPRO7 SDH never raising SDH_INT_CARD
 * for the device's in-band DAT1 interrupt. The ISR wakes rx_task immediately;
 * the 2 ms sem backstop in rx_task remains the lost-wakeup fallback. */
static volatile uint32_t s_gpi_irq_count = 0;
/* Real data-ready ISR — owns ONLY GPIO0 (all other GPIO ints masked). Clears
 * the GPIO int, counts it, and gives the rx sem. Mirrors hal_sdio.c
 * sdio_sdh_isr (xSemaphoreGiveFromISR + portYIELD_FROM_ISR). */
static void ipro6s_rx_data_ready_isr(void)
{
    BaseType_t woken = pdFALSE;
    GLB_GPIO_IntClear(IPRO6S_DR_GPIO_PIN, 1);   /* w1p: clear the edge latch */
    GLB_GPIO_IntClear(IPRO6S_DR_GPIO_PIN, 0);
    s_gpi_irq_count++;
    if (s_rx_data_ready_sem != NULL)
        xSemaphoreGiveFromISR(s_rx_data_ready_sem, &woken);
    portYIELD_FROM_ISR(woken);
}
/* Arm GPIO0 as input + ASYNC rising-edge IRQ for the device data-ready
 * side-channel. Mask ALL other GPIO ints (GPIO_INT0_IRQn is shared across
 * GPIO0-31; SDIO/UART pins would false-trigger).
 *
 * IRQ mode = ASYNC + POS_PULSE (reg 0x300001B4 field {async[2],trig[1:0]} =
 * 0x5), fires exactly once per device rising edge. Do NOT use SYNC rising
 * (reg 0x1): Interrupt_Handler_Register registers the ECLIC vector as
 * LEVEL_TRIGGER and also enables it; under SYNC the GPIO detect stays asserted
 * into the ECLIC and GLB_GPIO_IntClear cannot clear it -> immediate IRQ storm
 * that hangs the CPU at registration (verified on-target 2026-06-02). ASYNC is
 * edge-clearable. (The earlier "0x5 = both-edge / double-count" note was a
 * misread of the field — it is async-rising, a single edge per pulse.) */
static void ipro6s_data_ready_irq_init(void)
{
    GLB_GPIO_Cfg_Type cfg = {
        .gpioPin  = IPRO6S_DR_GPIO_PIN, .gpioFun = GPIO_FUN_GPIO,
        .gpioMode = GPIO_MODE_INPUT,     .pullType = GPIO_PULL_DOWN,
        .drive = 0, .smtCtrl = 1,
    };
    /* GPIO_INT0_IRQn is a SINGLE ECLIC vector shared by ALL GPIOs, including
     * the SDC data pins. The line = OR of per-pin latched status that is NOT
     * gated unless the pin's int is correctly MASKED. Mask everything except
     * GPIO0, then clear stale status, so the vector only ever sees GPIO0.
     *
     * GOTCHA (verified on-target 2026-06-02): the SDK GLB_GPIO_IntMask() writes
     * the WRONG register for pins >= 32 — it pokes 0x1A4, but the real GPIO32-43
     * mask is 0x300001AC[11:0]. So SDC D3=GPIO32 / D2=GPIO33 stayed UNMASKED and
     * kept latching status from 25 MHz SDIO traffic -> shared line stuck high ->
     * enabling the vector stormed and hung the CPU. (GPIO28-31 SDC pins, incl.
     * the CLK, are in the 0-31 range and ARE correctly gated by 0x1A0, so they
     * never latched — that asymmetry pinned the bug.) Mask 32-43 DIRECTLY at
     * 0x1AC[11:0] and clear their latched status via int_clr2 = 0x1AC[23:12]. */
    GLB_GPIO_Init(&cfg);
    for (int p = 0; p < 32; p++) GLB_GPIO_IntMask(p, 1);     /* GPIO0-31 mask (0x1A0 — correct) */
    {
        volatile uint32_t *m2clr2 = (volatile uint32_t *)0x300001AC;
        *m2clr2 |= 0x00000FFFu;                  /* mask2[11:0]: mask GPIO32-43 */
        *m2clr2 |= (0xFFFu << 12);               /* int_clr2[23:12] w1p: assert clear */
        *m2clr2 &= ~(0xFFFu << 12);              /* release clear (keep mask bits) */
    }
    /* Clear stale GPIO0-31 status (int_clr1 = 0x1B0) for all but GPIO0. */
    for (int p = 1; p < 32; p++) { GLB_GPIO_IntClear(p, 1); GLB_GPIO_IntClear(p, 0); }
    GLB_GPIO_IntMask(IPRO6S_DR_GPIO_PIN, 1);                /* GPIO0 MASKED = IRQ default-OFF
                                                               (rx_task uses backstop poll; run
                                                               `ipro6s_girq 1` to enable IRQ-wake).
                                                               No clean good-RF throughput benefit
                                                               measured (interleaved A/B ~tie); IRQ
                                                               only helps in degrading/lossy RF. */
    GLB_Set_GPIO_IntMod(IPRO6S_DR_GPIO_PIN, GLB_GPIO_INT_CONTROL_ASYNC,
                        GLB_GPIO_INT_TRIG_POS_PULSE);
    GLB_GPIO_IntClear(IPRO6S_DR_GPIO_PIN, 1);
    GLB_GPIO_IntClear(IPRO6S_DR_GPIO_PIN, 0);
    CPU_Interrupt_Pending_Clear(GPIO_INT0_IRQn);  /* drop any stale latched ECLIC pending */
    Interrupt_Handler_Register(GPIO_INT0_IRQn, ipro6s_rx_data_ready_isr);
    CPU_Interrupt_Enable(GPIO_INT0_IRQn);   /* redundant: ECLIC_Register_IRQ already enabled it */
}

/* A/B knob for the data-ready IRQ side-channel: on = IRQ-wake (unmask GPIO0),
 * off = mask GPIO0 -> rx_task falls back to the pure IPRO6S_RX_BACKSTOP_MS
 * poll. The GPIO is configured by ipronet_sdio_rx_start; run that first. */
int ipronet_diag_girq(int on)
{
    if (s_rx_task == NULL) return IPRONET_ERR_STATE;
    GLB_GPIO_IntMask(IPRO6S_DR_GPIO_PIN, on ? 0 : 1);
    return IPRONET_OK;
}

/* ------------------------------------------------------------------ */
/* Stats getters (bench)                                               */
/* ------------------------------------------------------------------ */

/* A/B knob for the CMD53 wait demotion (sdio_set_wait_demotion): 1/0,
 * -1 = query. Returns the setting in force. */
int ipronet_diag_rx_xfer_lowprio(int on)
{
    if (on >= 0) {
        s_rx_xfer_lowprio = (on != 0);
        if (s_rx_task != NULL)
            sdio_set_wait_demotion(s_rx_xfer_lowprio ? s_rx_task : NULL,
                                   IPRO6S_RX_TASK_PRIO);
    }
    return s_rx_xfer_lowprio;
}

/* rx_task stage census. Printed here, off the hot path; the app just calls it.
 * `reset` zeroes the accumulators so a single cell can be bracketed. */
void ipronet_diag_rx_profile(int reset)
{
    static const char *const names[RXP_N] = {
        "claim", "doorbell", "cmd53", "dispatch", "idle",
    };
    printf("[rxp] stage        n        sum_us    avg_us   max_us\r\n");
    for (int i = 0; i < RXP_N; i++) {
        uint32_t n = s_rxp_cnt[i];
        uint64_t s = s_rxp_us[i];
        printf("[rxp] %-9s %8lu %12llu %9lu %8lu\r\n", names[i],
               (unsigned long)n, (unsigned long long)s,
               (unsigned long)(n ? (uint32_t)(s / n) : 0U),
               (unsigned long)s_rxp_max[i]);
    }
    printf("[rxp] wake hotspin=%lu sem_ok=%lu sem_to=%lu budget=%lu disp>=1ms=%lu\r\n",
           (unsigned long)s_rxp_wake_hotspin, (unsigned long)s_rxp_wake_sem_ok,
           (unsigned long)s_rxp_wake_sem_to, (unsigned long)s_rxp_wake_budget,
           (unsigned long)s_rxp_dispatch_1ms);
    printf("[rxp] now_us=%llu\r\n", (unsigned long long)mtimer_get_time_us());
    if (reset) {
        for (int i = 0; i < RXP_N; i++) {
            s_rxp_us[i] = 0; s_rxp_cnt[i] = 0; s_rxp_max[i] = 0;
        }
        s_rxp_wake_hotspin = 0; s_rxp_wake_sem_ok = 0; s_rxp_wake_sem_to = 0;
        s_rxp_wake_budget = 0; s_rxp_dispatch_1ms = 0;
        printf("[rxp] reset\r\n");
    }
}

void ipronet_diag_tx_stats(ipronet_tx_stats_t *o)
{
    o->writes = s_tx_writes; o->agg_frames = s_tx_agg_frames;
    o->cmd_frames = s_tx_cmd_frames; o->eth_frames = s_tx_eth_frames;
    o->bytes = s_tx_bytes; o->errors = s_tx_errors;
    o->agg_flushes = s_tx_agg_flushes; o->agg_to_flush = s_tx_agg_to_flush;
    o->write_us = s_tx_write_us; o->write_max_us = s_tx_write_max_us;
    o->agg_max = s_tx_agg_max; o->zc2_nslot = s_zc2_nslot; o->zc3_uslots = s_zc3_uslots;
    o->pace_kbps = s_tx_pace_kbps; o->pace_drops = s_tx_pace_drops;
}

void ipronet_diag_rx_stats(ipronet_rx_stats_t *o)
{
    o->doorbell_polls = s_doorbell_polls; o->doorbell_zero = s_doorbell_zero;
    o->transfers = s_rx_transfers; o->bytes = s_rx_bytes; o->pdus = s_rx_pdus;
    o->data_frames = s_rx_data_frames; o->amsdu_drop = s_rx_amsdu_drop;
    o->bad_type = s_rx_bad_type; o->bad_subtype = s_rx_bad_subtype;
    o->desync = s_rx_desync; o->unaligned_cnt = s_rx_unaligned_cnt;
    o->pool_drop = ipronet_netif_pool_drops(); o->mbox_drop = ipronet_netif_mbox_drops();
    o->last_pdu_len = s_last_pdu_len; o->last_pdu_subtype = s_last_pdu_subtype;
    o->girq_count = s_gpi_irq_count;
}
