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
 * ATBM6162 SDIO transport binding: implements the vendor os-sdio port contract
 * (atbm_os_sdio.h) on top of the IPRO7 SDIO host stack (bsp/hal/hal_sdio.c).
 *
 * Replaces the vendor os/fh_rtt/sdio/atbm_os_sdio.c (which targeted a different
 * SDIO host API). IRQ servicing is polling-mode via sdio_poll_irq (the IPRO7 SDH
 * card-int vector is not exposed as an IRQn — see aic8800d80-integration.md).
 *
 * IPRO7 port — Phase 0. See .claude/plans/atbm6162-integration.md.
 */
#include "atbm_hal.h"      /* struct atbm_sdio_func / driver / device_id, atbm types */
#include "hal_sdio.h"
#include "drv_sdh.h"      /* bsp SDIO host stack */

#include "FreeRTOS.h"
#include "task.h"

/* The wlan core (hal/sdio/atbm_sdio.c) supplies these. */
extern int atbm_sdio_probe(struct atbm_sdio_func *func,
                           const struct atbm_sdio_device_id *id);
extern int atbm_sdio_disconnect(struct atbm_sdio_func *func);

/* ---- Module state ---------------------------------------------------------- */
static SDIO_Host_Type        g_host;
static struct atbm_sdio_func func0;
static struct atbm_sdio_func func1;
static struct atbm_sdio_driver atmbwifi_driver;
static struct atbm_sdio_device_id atbm_sdio_ids[] = {
    { { 0 } }, /* terminator (ATBM matches by CIS, not SDIO product id) */
};

#define ATBM_SDIO_CLK_HZ        25000000u   /* default-speed operating clock */
#define ATBM_SDIO_DEF_BLKSIZE   256         /* ATBM operating block size (ATBM_SDIO_BLOCK_SIZE).
                                             * Was 288 = stale fh_rtt "256 + 32 swap-buffer slack"
                                             * default inherited from the non-ATBM port; ATBM's
                                             * real func0/func1 operating size is 256. */
#define ATBM_SDIO_XFER_RETRIES  4           /* re-issue a CMD53 on transient timeout (marginal
                                             * flywire SI: st=0xd1 glitches recover on retry) */
/* Bus width: 1-bit @ 25MHz is the proven shipping config on the flywire prototype.
 * 4-bit needs proper PCB routing. HW-characterized 2026-06-29: 4-bit WRITES work
 * (164KB fw download, any clock) but 4-bit READS fail (st=0xd1) due to flywire
 * round-trip read-timing — the host clocks CLK out, the chip drives data back, and
 * the round trip over the long flywire exceeds the host's read sample window.
 * Confirmed by isolating every lever:
 *   - clock-sensitive: 6MHz reads fail, 2MHz reads recover (the ONLY lever that helped);
 *   - drive-strength-insensitive: max GPIO drive (3) @ 6MHz fails identically
 *     (edge sharpness != propagation delay);
 *   - byte-vs-block-insensitive: the vendor's block-mode-short (atbm_sdio_rem_xfer)
 *     fails the same (identical 8-nibble data phase / blk_size=4 at the SDH).
 * On flywire 4-bit only survives <=2MHz (slower than 1-bit@25MHz -> no gain). The real
 * 4x win needs a PCB (short traces) or SDH read-sampling-phase tuning. Set 1 to retest. */
#define ATBM_SDIO_BUS_4BIT      0   /* 0 = 1-bit @ 25MHz (proven shipping config); 1 = 4-bit TX + 1-bit RX (reads-1bit) */
/* Exposed to hal/ (atbm_sdio_fwio.c) which can't see this define. */
int atbm_port_sdio_bus_4bit(void) { return ATBM_SDIO_BUS_4BIT; }
/* IPRO7 SDH read-capture phase taps (GLB smih_0_io_dly_0 @ 0x30000438). SDR-3.3V has
 * no SDHCI tuning; these GLB taps shift WHEN the host samples read data to compensate
 * the flywire round-trip delay (post-fw 4-bit reads mis-sample at the default 0 phase).
 * Sweep: start clk_inv 0/1, then clk_dly 0..3, then dat_dly 0..3. */
#define ATBM_SDIO_RD_CLK_INV    0   /* invert read-capture clock edge (0/1) — sweep via atbm_tap */
#define ATBM_SDIO_RD_CLK_DLY    0   /* capture-clock delay tap (0..3) */
#define ATBM_SDIO_RD_DAT_DLY    0   /* DAT0-3 input delay tap (0..3) */

/* Vendor IRQ callback shim: hal SDIO_IRQ_Handler is (uint8_t,void*); the vendor
 * callback is void(*)(struct atbm_sdio_func*). Forward via a static trampoline. */
static void (*s_vendor_irq_cb)(struct atbm_sdio_func *func);
static struct atbm_sdio_func *s_irq_func;
static TaskHandle_t   s_irq_task;
static volatile int   s_irq_run;

static void atbm_irq_trampoline(uint8_t func_num, void *arg)
{
    (void)func_num;
    (void)arg;
    if (s_vendor_irq_cb && s_irq_func) {
        s_vendor_irq_cb(s_irq_func);
    }
}

static void atbm_sdio_irq_task(void *arg)
{
    (void)arg;
    while (s_irq_run) {
        sdio_poll_irq(&g_host);
        vTaskDelay(pdMS_TO_TICKS(1));
    }
    s_irq_task = NULL;
    vTaskDelete(NULL);
}

/* ---- SDH read-capture phase taps (GLB smih_0_io_dly_0 @ 0x30000438) ---------
 * Runtime-tunable for sweeping the post-fw 4-bit read sample phase without reflashing
 * (atbm6162_set_rdphase / shell atbm_tap). Defaults from ATBM_SDIO_RD_* above. */
static uint8_t g_rd_clk_inv = ATBM_SDIO_RD_CLK_INV;
static uint8_t g_rd_clk_dly = ATBM_SDIO_RD_CLK_DLY;
static uint8_t g_rd_dat_dly = ATBM_SDIO_RD_DAT_DLY;

/* High-Speed sampling toggle. HS shifts the read sample point ~half a clock and needs a
 * few clocks to settle -> short 8-nibble 4-bit blocks fail CRC while long ones pass. DS
 * (no shift) may capture short blocks cleanly. Settable via atbm6162_set_hs / shell atbm_hs.
 * Default follows the bus config: the 4-bit experiments ran with host HS on, but the
 * HW-validated 1-bit@25M config never set HI_SPEED_EN (and the card CCCR EHS is never
 * written), so 1-bit must keep the host in default-speed timing. */
static int g_atbm_hs = ATBM_SDIO_BUS_4BIT;
void atbm6162_set_hs(int en) { g_atbm_hs = en ? 1 : 0; }

/* Select the io_dly-delayed sampling clock so the read-capture taps actually take effect
 * (without this the SDH samples reads on the fixed clock and the taps do nothing). */
static int g_atbm_samp = 0;
void atbm6162_set_samp(int en) { g_atbm_samp = en ? 1 : 0; }

/* On a 4-bit read that returns CRC error, re-read at 1-bit and compare the actually-
 * received bytes — tells us whether the data was corrupt (real SI) or correct (false CRC). */
static int g_crc_diag = 0;
void atbm6162_set_crcdiag(int en) { g_crc_diag = en ? 1 : 0; }

void atbm6162_set_ignorecrc(int en) { sdio_set_ignore_rd_crc(en); }

/* reads-1bit is the SHIPPING RX path: all reads (RX + registers) run 1-bit while writes/TX
 * stay 4-bit, switched atomically under the SDIO mutex. Default ON = the HW-validated
 * working config (bring-up + WPA2 + DHCP). g_atbm_reads1bit is re-applied on every bring-up
 * so a shell `atbm_reads1bit 0` (for native-4-bit experiments) survives a re-probe. */
static int g_atbm_reads1bit = 1;
void atbm6162_set_reads1bit(int en) { g_atbm_reads1bit = en ? 1 : 0; sdio_set_reads_1bit(g_atbm_reads1bit); }
void atbm6162_set_noblkcnt(int en) { sdh_set_single_no_blkcnt(en); }

/* Native-4-bit read-phase auto-calibration ("tuning"). On the first 4-bit read that
 * CRC-fails, sweep the io_dly sample-phase taps (with SAMPLING_CLK_SEL=1) and lock the
 * first phase that reads clean 3x in a row -> native 4-bit reads with no 1-bit workaround.
 * Requires reads-1bit OFF and ignore-CRC OFF (so real CRC results surface). */
static int g_rdcal_en = 0;
static int g_rdcal_done = 0;
void atbm6162_set_rdcal(int en) { g_rdcal_en = en ? 1 : 0; g_rdcal_done = 0; }

/* Read-twice-verify for small register reads: with ignore-CRC the read returns data that
 * is correct most of the time but occasionally one-nibble-slipped. Reading twice and
 * accepting only when both agree rejects the random slip (idempotent register reads). */
static int g_rd_verify = 0;
void atbm6162_set_rdverify(int en)
{
    g_rd_verify = en ? 1 : 0;
    sdio_set_ignore_rd_crc(en);   /* verify needs the read to return data despite false CRC */
}

/* 4-bit bus clock (Hz). Lowering it widens the read sample margin — the lever for a
 * marginal short-block DATA_CRC. 96MHz src: 24M=div2, 12M=div4, 6M=div8, 3M=div16 (clean). */
static uint32_t g_atbm_clk_hz = 24000000u;
void atbm6162_set_clk(unsigned hz) { g_atbm_clk_hz = (uint32_t)hz; }

/* ---- Native 4-bit READ stress test — quantify the post-fw read-CRC margin ---------
 * Hammer a stable AHB register (default 0xab0016c, a ROM constant = 0x34800000) N times at
 * the CURRENT bus width / clock / io_dly tap and count DATA_CRC failures + value mismatches.
 * The driver's AHB read is lock-serialised (sbus_ops->lock), so this is safe to call from the
 * shell while the bh/TX threads run. Run AFTER atbm_probe. To measure NATIVE 4-bit reads set
 * reads-1bit OFF first (else every read is switched to clean 1-bit and the test is meaningless).
 * Sweep atbm_clk / atbm_tap / atbm_samp between runs to map the read eye. */
extern struct atbmwifi_common g_hw_prv;
extern int atbm_ahb_read_32(struct atbmwifi_common *hw_priv, atbm_uint32 addr, atbm_uint32 *val);
void atbm6162_rdtest(unsigned n, unsigned addr)
{
    atbm_uint32 v = 0, v0 = 0;
    unsigned i, fail = 0, mism = 0, ok = 0;
    int have_ref = 0;
    if (addr == 0u) addr = 0xab0016cu;
    if (n == 0u)    n = 1000u;
    for (i = 0; i < n; i++) {
        int r = atbm_ahb_read_32(&g_hw_prv, (atbm_uint32)addr, &v);
        if (r < 0) { fail++; }
        else {
            ok++;
            if (!have_ref) { v0 = v; have_ref = 1; }
            else if (v != v0) { mism++; }
        }
        /* Yield often: this runs in the shell task, so without it the bh/idle/watchdog
         * starve and the box looks hung. Progress print also shows it's alive. */
        if ((i & 0x3fu) == 0x3fu) {
            vTaskDelay(1);
            if ((i & 0x1ffu) == 0x1ffu)
                wifi_printk(WIFI_ALWAYS, "atbm rdtest: %u/%u (fail=%u mism=%u)\n", i + 1u, n, fail, mism);
        }
    }
    wifi_printk(WIFI_ALWAYS,
        "atbm rdtest: addr=0x%x n=%u clk=%uMHz reads1bit=%d samp=%d tap(inv=%d clk=%d dat=%d)"
        " -> CRC-fail=%u (%u.%u%%) mismatch=%u ok=%u val=0x%x\n",
        addr, n, (unsigned)(g_atbm_clk_hz / 1000000u), g_atbm_reads1bit, g_atbm_samp,
        g_rd_clk_inv, g_rd_clk_dly, g_rd_dat_dly,
        fail, (fail * 100u) / n, ((fail * 1000u) / n) % 10u, mism, ok, (unsigned)v0);
}

/* Fields of GLB smih_0_io_dly_0 this driver owns: read-capture clock inv/dly + DAT0-3
 * input delays. Everything else (clk_o/cmd delays, DAT4-7) is preserved on every write. */
#define ATBM_IODLY_FIELD_MASK  ((1u << 0) | (3u << 2) | (3u << 16) | (3u << 18) | (3u << 20) | (3u << 22))

/* Live io_dly register with our fields cleared — the base every phase word builds on.
 * NEVER write the register from a zero base: the cmd_*_dly / DAT4-7 fields would be
 * clobbered (all default 0 today, but that is not ours to assume). */
static uint32_t atbm_iodly_base(void)
{
    volatile uint32_t *io_dly = (volatile uint32_t *)(0x30000000u + 0x438u);
    return *io_dly & ~ATBM_IODLY_FIELD_MASK;
}

static uint32_t atbm_iodly_word(uint32_t base, int inv, int clkdly, int datdly)
{
    uint32_t v = base;
    v |= ((uint32_t)(inv & 1) << 0);
    v |= ((uint32_t)(clkdly & 3) << 2);
    v |= ((uint32_t)(datdly & 3) << 16);   /* DAT0 */
    v |= ((uint32_t)(datdly & 3) << 18);   /* DAT1 */
    v |= ((uint32_t)(datdly & 3) << 20);   /* DAT2 */
    v |= ((uint32_t)(datdly & 3) << 22);   /* DAT3 */
    return v;
}

/* Per-direction phase split (native full 4-bit): reads sample at the shifted phase,
 * writes stay on the HW-proven default phase (shifted capture breaks the post-write
 * CRC-status token). Settings persist across re-probe like g_atbm_reads1bit. */
static int g_phasesplit_en = 0;
static uint8_t g_ps_inv = 0, g_ps_clkdly = 0, g_ps_datdly = 0;

static void atbm_sdio_apply_rdphase(void)
{
    volatile uint32_t *io_dly = (volatile uint32_t *)(0x30000000u + 0x438u);
    uint32_t v = atbm_iodly_word(atbm_iodly_base(), g_rd_clk_inv, g_rd_clk_dly, g_rd_dat_dly);
    *io_dly = v;
    /* The taps only affect read capture if the SDH samples on the delayed clock. */
    sdio_set_sampling_clk_sel(g_atbm_samp);
    /* Only log on change — rem-1bit toggles width per transfer and would spam otherwise. */
    {
        static uint32_t s_last = 0xffffffffu;
        static int s_last_samp = -1;
        if (v != s_last || g_atbm_samp != s_last_samp) {
            s_last = v; s_last_samp = g_atbm_samp;
            wifi_printk(WIFI_ALWAYS, "atbm sdio: smih io_dly=0x%x samp_sel=%d (inv=%d clkdly=%d datdly=%d)\n",
                        (unsigned)v, g_atbm_samp, g_rd_clk_inv, g_rd_clk_dly, g_rd_dat_dly);
        }
    }
}

/* Live setter for sweeping: set the taps, then re-run atbm_probe to bring up with them. */
void atbm6162_set_rdphase(int inv, int clkdly, int datdly)
{
    g_rd_clk_inv = (uint8_t)(inv & 1);
    g_rd_clk_dly = (uint8_t)(clkdly & 3);
    g_rd_dat_dly = (uint8_t)(datdly & 3);
    atbm_sdio_apply_rdphase();
}

/* Per-direction phase split. en=1: READ transfers sample at {inv,clkdly,datdly}+samp_sel=1,
 * WRITE transfers at the proven default phase (io_dly base, samp_sel=0), switched inside
 * the SDIO mutex per transfer (hal sdio_set_dir_phase). Forces the reads-1bit workaround
 * OFF — phase split exists to make native 4-bit reads work. en=0: back to the legacy
 * global phase (g_rd_* taps + g_atbm_samp). */
void atbm6162_set_phasesplit(int en, int inv, int clkdly, int datdly)
{
    if (en) {
        uint32_t base = atbm_iodly_base();
        SDIO_Phase_Type rd = { atbm_iodly_word(base, inv, clkdly, datdly), 1 };
        SDIO_Phase_Type wr = { base, 0 };
        g_ps_inv = (uint8_t)(inv & 1);
        g_ps_clkdly = (uint8_t)(clkdly & 3);
        g_ps_datdly = (uint8_t)(datdly & 3);
        g_phasesplit_en = 1;
        if (g_atbm_reads1bit) {
            g_atbm_reads1bit = 0;
            wifi_printk(WIFI_ALWAYS, "atbm sdio: phasesplit forces reads-1bit OFF\n");
        }
        sdio_set_reads_1bit(0);
        sdio_set_dir_phase(&rd, &wr, 1);
        wifi_printk(WIFI_ALWAYS,
            "atbm sdio: phasesplit ON rd(inv=%d clkdly=%d datdly=%d samp=1) wr(default samp=0)\n",
            g_ps_inv, g_ps_clkdly, g_ps_datdly);
    } else {
        g_phasesplit_en = 0;
        sdio_set_dir_phase(NULL, NULL, 0);
        atbm_sdio_apply_rdphase();   /* restore the legacy global phase (incl. samp_sel) */
        wifi_printk(WIFI_ALWAYS, "atbm sdio: phasesplit OFF\n");
    }
}

/* rdcal hook: re-apply the current g_rd_* taps through whichever mechanism is active.
 * Under phase split only the READ slot moves (writes keep the proven default phase),
 * which makes rdcal's read-only lock criterion safe — it can no longer lock a phase
 * that breaks writes. */
static void atbm_sdio_apply_current_rdphase(void)
{
    if (g_phasesplit_en) {
        uint32_t base = atbm_iodly_base();
        SDIO_Phase_Type rd = { atbm_iodly_word(base, g_rd_clk_inv, g_rd_clk_dly, g_rd_dat_dly), 1 };
        SDIO_Phase_Type wr = { base, 0 };
        g_ps_inv = g_rd_clk_inv;
        g_ps_clkdly = g_rd_clk_dly;
        g_ps_datdly = g_rd_dat_dly;
        sdio_set_dir_phase(&rd, &wr, 1);
    } else {
        atbm_sdio_apply_rdphase();
    }
}

/* WIRING-DIAG helper: flip host+card bus width at runtime. sdio_set_bus_width reprograms
 * BOTH the card CCCR_IF and the host dataWidth, so a 1-bit re-read here uses the same
 * proven-good path that full 1-bit WiFi uses — letting us compare a 4-bit read against it. */
void atbm_sdio_force_width(int four_bit)
{
    (void)sdio_set_bus_width(&g_host, four_bit ? SDIO_BUS_WIDTH_4BIT : SDIO_BUS_WIDTH_1BIT);
    (void)sdio_set_clock(&g_host, four_bit ? 24000000u : ATBM_SDIO_CLK_HZ);
    if (four_bit) {
        atbm_sdio_apply_rdphase();
    }
}

/* ==================================================================== */
/* Per-direction phase calibration sweep (atbm_physweep)                 */
/* ==================================================================== */

/* Raw HIF access — the vendor v0 protocol replicated on bare hal_sdio calls so the
 * sweep runs with NO vendor driver state (no threads, no sbus, no probe). Register
 * access = a 2/4-byte CMD53 at SDIO address reg_id<<2 on func1 (SPI_REG_ADDR_TO_SDIO;
 * SDIO_ADDR17BIT_V1(0,0,0,x) == x). AHB access = SRAM_BASE + PFETCH-poll + DPORT,
 * mirroring atbm_indirect_read_unlock / atbm_ahb_write_unlock_v0. */
/* When set, raw reg ops retry like the production wrappers (heals the known transient
 * 1-bit glitch class). MEASUREMENT passes keep it OFF — they must stay single-shot. */
static int g_raw_retry = 0;

static SDIO_Status_Type atbm_raw_reg_read(uint16_t reg_id, void *buf, uint16_t n)
{
    SDIO_Status_Type st;
    int tries = g_raw_retry ? ATBM_SDIO_XFER_RETRIES : 1;
    do {
        st = sdio_read_bytes(&g_host, 1, (uint32_t)reg_id << 2, (uint8_t *)buf, n, false);
    } while (st != Status_SDIO_Success && --tries);
    return st;
}

static SDIO_Status_Type atbm_raw_reg_write(uint16_t reg_id, const void *buf, uint16_t n)
{
    SDIO_Status_Type st;
    int tries = g_raw_retry ? ATBM_SDIO_XFER_RETRIES : 1;
    do {
        st = sdio_write_bytes(&g_host, 1, (uint32_t)reg_id << 2, (uint8_t *)buf, n, false);
    } while (st != Status_SDIO_Success && --tries);
    return st;
}

static int atbm_raw_ahb_read32(uint32_t addr, uint32_t *val)
{
    uint32_t cfg;
    int i;

    if (atbm_raw_reg_write(ATBM_HIFREG_SRAM_BASE_ADDR_REG_ID, &addr, 4) != Status_SDIO_Success)
        return -1;
    if (atbm_raw_reg_read(ATBM_HIFREG_CONFIG_REG_ID, &cfg, 4) != Status_SDIO_Success)
        return -1;
    cfg |= ATBM_HIFREG_CONFIG_AHB_PFETCH_BIT;
    if (atbm_raw_reg_write(ATBM_HIFREG_CONFIG_REG_ID, &cfg, 4) != Status_SDIO_Success)
        return -1;
    for (i = 0; i < 20; i++) {
        if (atbm_raw_reg_read(ATBM_HIFREG_CONFIG_REG_ID, &cfg, 4) != Status_SDIO_Success)
            return -1;
        if (!(cfg & ATBM_HIFREG_CONFIG_AHB_PFETCH_BIT))
            break;
        if (i >= 3) vTaskDelay(1);   /* normally clears immediately; only back off late */
    }
    if (cfg & ATBM_HIFREG_CONFIG_AHB_PFETCH_BIT)
        return -1;
    if (atbm_raw_reg_read(ATBM_HIFREG_AHB_DPORT_REG_ID, val, 4) != Status_SDIO_Success)
        return -1;
    return 0;
}

static int atbm_raw_ahb_write32(uint32_t addr, uint32_t val)
{
    if (atbm_raw_reg_write(ATBM_HIFREG_SRAM_BASE_ADDR_REG_ID, &addr, 4) != Status_SDIO_Success)
        return -1;
    if (atbm_raw_reg_write(ATBM_HIFREG_AHB_DPORT_REG_ID, &val, 4) != Status_SDIO_Success)
        return -1;
    return 0;
}

#define ATBM_PHYSWEEP_DIAG_ADDR   0x0ab0016cu       /* known-value chip register */
#define ATBM_PHYSWEEP_DIAG_VAL    0x34800000u
#define ATBM_PHYSWEEP_SCRATCH     (DOWNLOAD_DTCM_ADDR + 0x100u)  /* DCCM RAM, inert under CPU_RESET */

/* Bring the freshly power-cycled chip to the vendor pre-download state at 1-bit:
 * WUP/RDY wake handshake, CPU_RESET+ACCESS_MODE (WiFi CPU halted, direct AHB on),
 * and the chip-side SDIO read-latch 0x161010dc=0x0b — the phase measurements must
 * see the exact chip output timing the real pre-fw reads see. */
static int atbm_physweep_chip_setup(void)
{
    /* NOTE: all HIF register accesses are 4-byte. The vendor's "16-bit" reg API
     * (atbm_reg_read_16/write_16) reads/writes 32 bits on the wire and truncates
     * in SW — this SDH's ADMA rejects 2-byte transfers outright (err_int=0x0200). */
    uint32_t ctrl = 0;
    uint32_t cfg = 0;
    int i;

    if (atbm_raw_reg_read(ATBM_HIFREG_CONTROL_REG_ID, &ctrl, 4) != Status_SDIO_Success)
        return -1;
    ctrl |= (uint32_t)ATBM_HIFREG_CONT_WUP_BIT;
    if (atbm_raw_reg_write(ATBM_HIFREG_CONTROL_REG_ID, &ctrl, 4) != Status_SDIO_Success)
        return -1;
    for (i = 0; i < 100; i++) {
        if (atbm_raw_reg_read(ATBM_HIFREG_CONTROL_REG_ID, &ctrl, 4) != Status_SDIO_Success)
            return -1;
        if (ctrl & ATBM_HIFREG_CONT_RDY_BIT)
            break;
        vTaskDelay(1);
    }
    if (!(ctrl & ATBM_HIFREG_CONT_RDY_BIT))
        return -2;

    if (atbm_raw_reg_read(ATBM_HIFREG_CONFIG_REG_ID, &cfg, 4) != Status_SDIO_Success)
        return -3;
    cfg |= ATBM_HIFREG_CONFIG_CPU_RESET_BIT | ATBM_HIFREG_CONFIG_ACCESS_MODE_BIT;
    if (atbm_raw_reg_write(ATBM_HIFREG_CONFIG_REG_ID, &cfg, 4) != Status_SDIO_Success)
        return -3;

    if (atbm_raw_ahb_write32(0x161010dcu, 0x0bu) != 0)
        return -4;
    return 0;
}

int atbm6162_physweep(unsigned k, void (*chip_power_cycle)(void))
{
    const uint32_t base0 = atbm_iodly_base();
    volatile uint32_t *io_dly = (volatile uint32_t *)(0x30000000u + 0x438u);
    unsigned c, i;
    int best = -1;
    unsigned best_score = 0;

    if (k == 0) k = 20;
    if (g_host.mutex == NULL) {
        /* Cold start (no atbm_probe yet): one-time full host init here; every sweep
         * iteration after this uses sdio_host_reinit (no RTOS object re-creation). */
        if (sdio_host_init(&g_host, (SD_ID_Type)0) != Status_SDIO_Success) {
            wifi_printk(WIFI_ALWAYS, "atbm physweep: host init failed\n");
            return -1;
        }
    }

    wifi_printk(WIFI_ALWAYS,
        "atbm physweep: %u candidates x %u iters (power-cycle each) — this resets the chip\n",
        33u, k);
    wifi_printk(WIFI_ALWAYS, " #  samp inv clk dat | enum setup | rd_ok rd_match | wr_ok wr_vfy\n");

    for (c = 0; c < 33u; c++) {
        /* candidate 0 = baseline (default phase, samp=0); 1..32 = samp=1 sweep */
        int samp = (c == 0) ? 0 : 1;
        int inv  = (c == 0) ? 0 : (int)(((c - 1u) >> 4) & 1u);
        int ckd  = (c == 0) ? 0 : (int)(((c - 1u) >> 2) & 3u);
        int dtd  = (c == 0) ? 0 : (int)((c - 1u) & 3u);
        unsigned rd_ok = 0, rd_match = 0, wr_ok = 0, wr_vfy = 0;
        int enum_ok = 0, setup_rc = -99;
        uint32_t v;

        if (chip_power_cycle) chip_power_cycle();

        /* Neutral host state: dir-phase off, default global phase, 1-bit. */
        sdio_set_dir_phase(NULL, NULL, 0);
        *io_dly = base0;
        sdio_set_sampling_clk_sel(0);
        sdio_set_reads_1bit(0);
        if (sdio_host_reinit(&g_host) != Status_SDIO_Success) goto record;

        for (i = 0; i < 2 && !enum_ok; i++) {
            enum_ok = (sdio_card_enumerate(&g_host) == Status_SDIO_Success);
        }
        if (!enum_ok) goto record;
        (void)sdio_enable_func(&g_host, 1);
        (void)sdio_set_clock(&g_host, 25000000u);   /* setup runs 1-bit @25M (proven) */

        /* Setup + ground truth run with retry (heal transient 1-bit glitches, like the
         * production wrappers do); the measurement passes below stay single-shot. */
        g_raw_retry = 1;
        setup_rc = atbm_physweep_chip_setup();
        if (setup_rc != 0) { g_raw_retry = 0; goto record; }

        /* 1-bit ground-truth gate — a wedged chip must not be scored as phase data. */
        if (atbm_raw_ahb_read32(ATBM_PHYSWEEP_DIAG_ADDR, &v) != 0 || v != ATBM_PHYSWEEP_DIAG_VAL) {
            setup_rc = -5;
            g_raw_retry = 0;
            goto record;
        }
        g_raw_retry = 0;

        (void)sdio_set_bus_width(&g_host, SDIO_BUS_WIDTH_4BIT);
        (void)sdio_set_clock(&g_host, 24000000u);

        /* READ pass: reads at the candidate phase, the protocol's internal writes
         * (SRAM_BASE/CONFIG) ride the proven default write phase — direction isolation. */
        {
            SDIO_Phase_Type rd = { atbm_iodly_word(base0, inv, ckd, dtd), (uint8_t)samp };
            SDIO_Phase_Type wr = { base0, 0 };
            sdio_set_dir_phase(&rd, &wr, 1);
            for (i = 0; i < k; i++) {
                if (atbm_raw_ahb_read32(ATBM_PHYSWEEP_DIAG_ADDR, &v) == 0) {
                    rd_ok++;
                    if (v == ATBM_PHYSWEEP_DIAG_VAL) rd_match++;
                }
                if ((i & 7u) == 7u) vTaskDelay(1);
            }
        }

        /* WRITE pass: writes at the candidate phase; readback verification goes at
         * 1-bit width (reads-1bit) + default read phase = trustworthy ground truth.
         * wr_ok = host's view, wr_vfy = data actually landed — recorded separately
         * (a mis-sampled CRC-status token makes them diverge). */
        {
            SDIO_Phase_Type rd = { base0, 0 };
            SDIO_Phase_Type wr = { atbm_iodly_word(base0, inv, ckd, dtd), (uint8_t)samp };
            sdio_set_dir_phase(&rd, &wr, 1);
            sdio_set_reads_1bit(1);
            for (i = 0; i < k; i++) {
                uint32_t pat = 0xA5C30000u | i;
                if (atbm_raw_ahb_write32(ATBM_PHYSWEEP_SCRATCH, pat) == 0) {
                    wr_ok++;
                    if (atbm_raw_ahb_read32(ATBM_PHYSWEEP_SCRATCH, &v) == 0 && v == pat) wr_vfy++;
                }
                if ((i & 7u) == 7u) vTaskDelay(1);
            }
            sdio_set_reads_1bit(0);
            sdio_set_dir_phase(NULL, NULL, 0);
        }

record:
        wifi_printk(WIFI_ALWAYS, "%2u   %d    %d   %d   %d  |  %d    %3d  |  %3u   %3u    |  %3u   %3u\n",
                    c, samp, inv, ckd, dtd, enum_ok, setup_rc,
                    rd_ok, rd_match, wr_ok, wr_vfy);
        /* best read phase = full protocol success AND full data match (tiebreak: first
         * found = smallest delay); baseline (c==0) is not a native-4-bit-read candidate */
        if (c > 0 && rd_ok == k && rd_match == k && rd_match > best_score) {
            best_score = rd_match;
            best = (int)c;
        }
        vTaskDelay(1);
    }

    /* Restore defaults; final power-cycle leaves the chip fresh for atbm_probe. */
    sdio_set_dir_phase(NULL, NULL, 0);
    *io_dly = base0;
    sdio_set_sampling_clk_sel(g_atbm_samp);
    sdio_set_reads_1bit(g_atbm_reads1bit);
    if (chip_power_cycle) chip_power_cycle();

    if (best > 0) {
        wifi_printk(WIFI_ALWAYS,
            "atbm physweep: RECOMMEND atbm_phasesplit 1 %d %d %d  (then atbm_probe)\n",
            (int)(((best - 1) >> 4) & 1), (int)(((best - 1) >> 2) & 3), (int)((best - 1) & 3));
    } else {
        wifi_printk(WIFI_ALWAYS,
            "atbm physweep: NO clean native-4-bit read phase — keep reads-1bit "
            "(atbm_reads1bit 1); matrix above is the SDH false-CRC evidence\n");
    }
    return 0;
}

/* ---- Host bring-up / teardown ---------------------------------------------- */
int atbm_sdio_register(struct atbm_sdio_driver *sdio_driver)
{
    SDIO_Status_Type st;

    /* Function descriptors (vendor defaults: func0 enabled, func1 pending). */
    func0.func = SDIO_FUNC_0; func0.en = 1; func0.blocksize = ATBM_SDIO_DEF_BLKSIZE; func0.priv = ATBM_NULL;
    func1.func = SDIO_FUNC_1; func1.en = 0; func1.blocksize = ATBM_SDIO_DEF_BLKSIZE; func1.priv = ATBM_NULL;

    /* NOTE: board SDH pinmux + ATBM power/reset are sequenced by the app/board
     * layer (e.g. apps/test/ipro_atbm_probe) before this runs. */
    st = sdio_host_init(&g_host, (SD_ID_Type)0);
    if (st != Status_SDIO_Success) {
        wifi_printk(WIFI_ALWAYS, "atbm sdio: host_init failed 0x%x\n", st);
        return -1;
    }
    (void)sdio_host_reset(&g_host);

    st = sdio_card_enumerate(&g_host);
    if (st != Status_SDIO_Success) {
        wifi_printk(WIFI_ALWAYS, "atbm sdio: enumerate failed 0x%x\n", st);
        return -1;
    }
    /* Bus width + clock — see ATBM_SDIO_BUS_4BIT note above. Default 1-bit @ 25 MHz
     * is the proven config (full bring-up + WPA2 + DHCP on real HW). 4-bit on the
     * flywire prototype: block transfers work <=6 MHz but short byte-mode reads
     * deterministically fail → needs PCB routing, hence 1-bit default here. */
    /* Host High-Speed sample select (HI_SPEED_EN); g_atbm_hs default 1 = the known-good
     * bring-up config. (Setting HS on the CARD too, to chase native 4-bit reads, was a dead
     * end — no host lever recovers the post-download 4-bit read CRC. The shipping RX path is
     * 1-bit, see reads-1bit below, so only the host-side setting is kept here.) */
    g_host.high_speed = g_atbm_hs ? 1 : 0;
    (void)sdio_set_bus_width(&g_host,
            ATBM_SDIO_BUS_4BIT ? SDIO_BUS_WIDTH_4BIT : SDIO_BUS_WIDTH_1BIT);
    /* 4-bit clock is runtime-tunable (atbm_clk) to widen sample margin; 1-bit fixed. */
    (void)sdio_set_clock(&g_host, ATBM_SDIO_BUS_4BIT ? g_atbm_clk_hz : ATBM_SDIO_CLK_HZ);
#if ATBM_SDIO_BUS_4BIT
    if (g_phasesplit_en) {
        /* re-apply after Ctrl_Init: refresh rd/wr words against the live io_dly base
         * and re-assert reads-1bit-off (survives re-probe like g_atbm_reads1bit does) */
        atbm6162_set_phasesplit(1, g_ps_inv, g_ps_clkdly, g_ps_datdly);
    } else {
        atbm_sdio_apply_rdphase();   /* read-capture phase taps (live-tunable via atbm6162_set_rdphase) */
        sdio_set_reads_1bit(g_atbm_reads1bit);  /* apply shipping RX path (1-bit reads / 4-bit TX) */
    }
#endif

    wifi_printk(WIFI_ALWAYS, "atbm sdio: enumerated %d func(s)\n", g_host.num_funcs);

    /* Hand off to the wlan core probe (mirrors vendor register flow). */
    if (sdio_driver && sdio_driver->probe_func) {
        return sdio_driver->probe_func(&func1, sdio_driver->match_id_table);
    }
    return 0;
}

void atbm_sdio_deregister(struct atbm_sdio_driver *sdio_driver)
{
    if (sdio_driver && sdio_driver->discon_func) {
        sdio_driver->discon_func(&func1);
    }
}

int atbm_sdio_register_init(void)
{
    int ret;
    atbm_memcpy(atmbwifi_driver.name, "atbm6162", sizeof("atbm6162"));
    atmbwifi_driver.match_id_table = atbm_sdio_ids;
    atmbwifi_driver.probe_func     = atbm_sdio_probe;
    atmbwifi_driver.discon_func    = atbm_sdio_disconnect;
    wifi_printk(WIFI_ALWAYS, "atbm_sdio_register_init\n");
    ret = atbm_sdio_register(&atmbwifi_driver);
    return ret;
}

int atbm_sdio_register_deinit(void)
{
    atbm_sdio_deregister(&atmbwifi_driver);
    return 0;
}

/* ---- Host bus lock (recursive at hal level) -------------------------------- */
void atbm_sdio_claim_host(struct atbm_sdio_func *func)   { (void)func; (void)sdio_claim_host(&g_host); }
void atbm_sdio_release_host(struct atbm_sdio_func *func) { (void)func; (void)sdio_release_host(&g_host); }

/* ---- Function enable/disable ----------------------------------------------- */
atbm_int32 atbm_sdio_enable_func(struct atbm_sdio_func *func)
{
    if (sdio_enable_func(&g_host, (uint8_t)func->func) != Status_SDIO_Success) {
        return -1;
    }
    func->en = 1;
    return 0;
}

void atbm_sdio_disable_func(struct atbm_sdio_func *func)
{
    (void)sdio_disable_func(&g_host, (uint8_t)func->func);
    func->en = 0;
}

/* ---- drvdata --------------------------------------------------------------- */
void  atbm_sdio_set_drvdata(struct atbm_sdio_func *func, void *priv) { func->priv = priv; }
void *atbm_sdio_get_drvdata(struct atbm_sdio_func *func)             { return func->priv; }

/* ---- IRQ (polling) --------------------------------------------------------- */
int atbm_sdio_claim_irq(struct atbm_sdio_func *func,
                        void (*irq_handle)(struct atbm_sdio_func *func))
{
    s_irq_func      = func;
    s_vendor_irq_cb = irq_handle;

    if (sdio_claim_irq(&g_host, (uint8_t)func->func, atbm_irq_trampoline, ATBM_NULL)
            != Status_SDIO_Success) {
        return -1;
    }
    (void)sdio_enable_irq(&g_host);

    if (s_irq_task == NULL) {
        s_irq_run = 1;
        if (xTaskCreate(atbm_sdio_irq_task, "atbm_sdio_irq", 512, NULL,
                        configMAX_PRIORITIES - 2, &s_irq_task) != pdPASS) {
            s_irq_run = 0;
            return -1;
        }
    }
    return 0;
}

int atbm_sdio_release_irq(struct atbm_sdio_func *func)
{
    (void)func;
    (void)sdio_disable_irq(&g_host);
    (void)sdio_release_irq(&g_host, (uint8_t)func->func);
    s_irq_run       = 0;   /* task self-deletes on next tick */
    s_vendor_irq_cb = ATBM_NULL;
    s_irq_func      = ATBM_NULL;
    return 0;
}

void atbm_sdio_host_enable_irq(int enable)
{
    if (enable) {
        (void)sdio_enable_irq(&g_host);
    } else {
        (void)sdio_disable_irq(&g_host);
    }
}

/* Board OOB host-wake GPIO (DAT1 in-band IRQ is handled by sdio_poll_irq).
 * No hal_sdio equivalent; no-op until an OOB pin is wired. */
void atbm_sdio_gpioirq_en(struct atbm_sdio_func *func, atbm_uint8 en)
{
    (void)func;
    (void)en;
}

/* Wake the IRQ servicing thread. In the polling model the task already runs at a
 * 1 ms cadence, so this is a no-op (kept for the hal/sdio call sites). */
void atbm_sdio_wakeup_irq_thread(struct atbm_sdio_func *func)
{
    (void)func;
}

/* ---- CMD53 block / byte transfers ------------------------------------------
 * INCREMENTING address (fixed_addr = false): matches Linux sdio_memcpy_fromio
 * semantics the ATBM driver assumes — both HIF register access (4-byte regs read
 * as consecutive bytes) and the AHB/data path copy contiguous memory. (Fixed/FIFO
 * addressing made 4-byte register reads return the same byte 4× → HIF R/W failed.) */
/* Short-remainder (n <= blocksize, 4-aligned) transfer. ATBM's own fh_rtt port
 * issues these as a 1-block block-mode CMD53 (sdio_drv_*(blocks=1, blksize=n)),
 * NEVER byte-mode. The #if-4-bit branch mirrors that exactly (set func block size
 * to n, do a 1-block transfer, restore). HW-tested 2026-06-29: this does NOT fix
 * the 4-bit short-transfer timeout — byte-mode and "1 block of n" produce the same
 * blk_size=n / 8-nibble data phase at the SDH level, and both st=0xd1 on the
 * flywire (confirmed signal-integrity, not byte-vs-block). Kept as the documented
 * vendor-faithful path for a future PCB; the active 1-bit shipping path is the
 * #else byte-mode (proven). Functionally identical at 1-bit either way. */
/* OCRAM bounce buffer for the SDH data path. The host dump on the st=0xd1 DataTimeout
 * shows the failing transfer's ADMA descriptor pointing at PSRAM (0x1e0025e4): the
 * SDH<->PSRAM DMA can't keep the 4-bit data phase fed (4x the 1-bit rate) -> the data
 * engine stalls -> DataTimeout. DMA'ing into fast OCRAM and memcpy'ing out removes the
 * stall. MUST land in OCRAM (.bss) — verified at link time (addr 0x11xxxxxx). */
static uint8_t s_sdio_bounce[512] __attribute__((section(".ocram_bss"), aligned(32)));
static int g_sdio_bounce_en = 0;   /* OFF: bounce corrupts reads; A/B via atbm_bounce */
static int g_sdio_cmd52_en = 0;    /* route small transfers via CMD52 (no data phase) */
static int g_rem_1bit = 0;         /* do small (rem) transfers at 1-bit; bulk stays 4-bit */

void atbm6162_set_bounce(int en) { g_sdio_bounce_en = en ? 1 : 0; }
void atbm6162_set_cmd52(int en)  { g_sdio_cmd52_en  = en ? 1 : 0; }
void atbm6162_set_pio(int en)    { sdio_set_small_pio(en); }
void atbm6162_set_rem1bit(int en){ g_rem_1bit = en ? 1 : 0; }

/* Sweep the read-capture phase taps and lock the first that reads `addr` clean 3x.
 * Runs once (on the first CRC-failing 4-bit read) — finds the native-4-bit sample phase. */
static SDIO_Status_Type atbm_sdio_rdcal_sweep(struct atbm_sdio_func *func,
                                              unsigned int addr, uint8_t *buf, uint16_t n)
{
    SDIO_Status_Type st = Status_SDIO_DataTimeout;
    int inv, cd, dd, k, ok;

    sdio_set_ignore_rd_crc(0);   /* need real CRC results while calibrating */
    g_atbm_samp = 1;             /* taps only take effect on the delayed sampling clock */
    wifi_printk(WIFI_ALWAYS, "atbm rdcal: sweeping read phase (addr=0x%x len=%d)...\n", addr, (int)n);

    for (inv = 0; inv < 2; inv++) {
        for (cd = 0; cd < 4; cd++) {
            for (dd = 0; dd < 4; dd++) {
                g_rd_clk_inv = (uint8_t)inv;
                g_rd_clk_dly = (uint8_t)cd;
                g_rd_dat_dly = (uint8_t)dd;
                atbm_sdio_apply_current_rdphase();
                ok = 1;
                for (k = 0; k < 3; k++) {
                    st = sdio_read_bytes(&g_host, (uint8_t)func->func, addr, buf, n, false);
                    if (st != Status_SDIO_Success) { ok = 0; break; }
                }
                if (ok) {
                    g_rdcal_done = 1;
                    wifi_printk(WIFI_ALWAYS,
                        "atbm rdcal: LOCKED phase inv=%d clkdly=%d datdly=%d -> native 4-bit read OK\n",
                        inv, cd, dd);
                    return Status_SDIO_Success;
                }
            }
        }
    }
    g_rd_clk_inv = 0; g_rd_clk_dly = 0; g_rd_dat_dly = 0;
    atbm_sdio_apply_current_rdphase();
    wifi_printk(WIFI_ALWAYS, "atbm rdcal: NO phase passed (swept 2x4x4 x3) at this operating point\n");
    return st;
}

static SDIO_Status_Type atbm_sdio_rem_xfer_inner(struct atbm_sdio_func *func,
                                           unsigned int addr, uint8_t *buf,
                                           uint16_t n, int write)
{
    /* CMD52 byte-loop path: each byte goes via the command/response (R5) with NO data
     * phase, so it cannot DataTimeout like the short CMD53 transfers do on the 4-bit
     * flywire. Matches the incrementing-address CMD53 byte-mode the rem path uses. */
    if (g_sdio_cmd52_en) {
        SDIO_Status_Type st = Status_SDIO_Success;
        for (uint16_t i = 0; i < n; i++) {
            st = write ? sdio_writeb(&g_host, (uint8_t)func->func, addr + i, buf[i])
                       : sdio_readb(&g_host, (uint8_t)func->func, addr + i, &buf[i]);
            if (st != Status_SDIO_Success) {
                return st;
            }
        }
        return st;
    }

    /* Route small transfers through the OCRAM bounce buffer to keep the SDH data engine
     * fed at the 4-bit rate (the direct-to-PSRAM path DataTimeouts). */
    if (g_sdio_bounce_en && n <= sizeof(s_sdio_bounce)) {
        SDIO_Status_Type st;
        if (write) {
            atbm_memcpy(s_sdio_bounce, buf, n);
            st = sdio_write_bytes(&g_host, (uint8_t)func->func, addr, s_sdio_bounce, n, false);
        } else {
            st = sdio_read_bytes(&g_host, (uint8_t)func->func, addr, s_sdio_bounce, n, false);
            if (st == Status_SDIO_Success) {
                atbm_memcpy(buf, s_sdio_bounce, n);
            }
        }
        return st;
    }
    if (write) {
        return sdio_write_bytes(&g_host, (uint8_t)func->func, addr, buf, n, false);
    }
    /* Read-twice-verify: reject the rare one-nibble slip by requiring two reads to agree. */
    if (g_rd_verify && n <= 8) {
        uint8_t tmp[8];
        int t, i, eq;
        for (t = 0; t < 8; t++) {
            SDIO_Status_Type sa = sdio_read_bytes(&g_host, (uint8_t)func->func, addr, buf, n, false);
            SDIO_Status_Type sb = sdio_read_bytes(&g_host, (uint8_t)func->func, addr, tmp, n, false);
            if (sa == Status_SDIO_Success && sb == Status_SDIO_Success) {
                for (i = 0, eq = 1; i < n; i++) { if (buf[i] != tmp[i]) eq = 0; }
                if (eq) {
                    return Status_SDIO_Success;
                }
            }
        }
        return Status_SDIO_DataTimeout;   /* two reads never agreed */
    }
    {
        SDIO_Status_Type st = sdio_read_bytes(&g_host, (uint8_t)func->func, addr, buf, n, false);
        /* First 4-bit read that CRC-fails -> auto-calibrate the read sample phase, once. */
        if (st != Status_SDIO_Success && g_rdcal_en && !g_rdcal_done && ATBM_SDIO_BUS_4BIT && n <= 512) {
            st = atbm_sdio_rdcal_sweep(func, addr, buf, n);
        }
        /* Was the CRC-failed data actually correct? Re-read at 1-bit and compare. */
        if (st != Status_SDIO_Success && g_crc_diag && n >= 4 && n <= 8) {
            uint8_t saved[8], oneb[8];
            int i, match = 1;
            for (i = 0; i < n; i++) saved[i] = buf[i];
            atbm_sdio_force_width(0);
            (void)sdio_read_bytes(&g_host, (uint8_t)func->func, addr, oneb, n, false);
            atbm_sdio_force_width(1);
            for (i = 0; i < n; i++) { if (saved[i] != oneb[i]) match = 0; }
            wifi_printk(WIFI_ALWAYS,
                "CRC-DIAG addr=0x%x: 4bit-rx=%02x%02x%02x%02x 1bit=%02x%02x%02x%02x match=%d\n",
                addr, saved[0], saved[1], saved[2], saved[3],
                oneb[0], oneb[1], oneb[2], oneb[3], match);
        }
        return st;
    }
}

/* Small/short transfers fail at 4-bit (the firmware changes the chip's HIF read state so
 * the post-download short reads CRC). They are control-path, not throughput. When g_rem_1bit
 * is set, run them at 1-bit (rock-solid for short transfers) and restore 4-bit after, so the
 * bulk block path keeps 4-bit throughput. */
static SDIO_Status_Type atbm_sdio_rem_xfer(struct atbm_sdio_func *func,
                                           unsigned int addr, uint8_t *buf,
                                           uint16_t n, int write)
{
    SDIO_Status_Type st;
    if (!g_rem_1bit || !ATBM_SDIO_BUS_4BIT) {
        return atbm_sdio_rem_xfer_inner(func, addr, buf, n, write);
    }
    atbm_sdio_force_width(0);                      /* small xfer at 1-bit (reliable) */
    st = atbm_sdio_rem_xfer_inner(func, addr, buf, n, write);
    atbm_sdio_force_width(1);                      /* restore 4-bit for bulk */
    return st;
}

int __atbm_sdio_memcpy_fromio(struct atbm_sdio_func *func, void *dst,
                              unsigned int addr, int count)
{
    uint8_t *p = (uint8_t *)dst;
    uint16_t blk = (uint16_t)func->blocksize;
    uint32_t rem = (uint32_t)count;

    if (func->en == 0) {
        return -1;
    }
    if ((((unsigned int)dst) & 3) || (count & 3)) {
        wifi_printk(WIFI_ALWAYS, "atbm sdio: read buf/len not 4-aligned (dst=%p len=%d)\n", dst, count);
    }

    /* RX block reads (the data FIFO) can't be read-twice-verified, and at 4-bit they hit
     * the same false-CRC / one-nibble-slip as register reads. Route them at 1-bit (reliable)
     * while registers stay 4-bit (verified) and TX block writes stay 4-bit. */
    int blk_1bit = (g_rd_verify && ATBM_SDIO_BUS_4BIT);
    if (blk) {
        uint32_t nblocks = rem / blk;
        if (blk_1bit && nblocks) atbm_sdio_force_width(0);
        while (nblocks) {
            uint16_t chunk = (nblocks > SDIO_CMD53_BYTE_BLOCK_COUNT_MAX)
                                 ? SDIO_CMD53_BYTE_BLOCK_COUNT_MAX
                                 : (uint16_t)nblocks;
            int tries = ATBM_SDIO_XFER_RETRIES;
            SDIO_Status_Type st;
            while ((st = sdio_read_blocks(&g_host, (uint8_t)func->func, addr, p, chunk, false))
                       != Status_SDIO_Success && --tries) {
                wifi_printk(WIFI_ALWAYS, "atbm sdio: read_blocks retry st=0x%x f=%d addr=0x%x n=%d\n",
                            (unsigned)st, (int)func->func, addr, (int)chunk);
            }
            if (st != Status_SDIO_Success) {
                if (blk_1bit) atbm_sdio_force_width(1);
                return -1;
            }
            p   += (uint32_t)chunk * blk;
            rem -= (uint32_t)chunk * blk;
            nblocks -= chunk;
        }
        if (blk_1bit) atbm_sdio_force_width(1);
    }
    if (rem) {
        int tries = ATBM_SDIO_XFER_RETRIES;
        SDIO_Status_Type st;
        while ((st = atbm_sdio_rem_xfer(func, addr, p, (uint16_t)rem, 0))
                   != Status_SDIO_Success && --tries) {
            wifi_printk(WIFI_ALWAYS, "atbm sdio: read rem retry st=0x%x f=%d addr=0x%x len=%d\n",
                        (unsigned)st, (int)func->func, addr, (int)rem);
        }
        if (st != Status_SDIO_Success) {
            wifi_printk(WIFI_ALWAYS, "atbm sdio: read rem FAIL st=0x%x f=%d addr=0x%x len=%d\n",
                        (unsigned)st, (int)func->func, addr, (int)rem);
            return -1;
        }
    }
    return 0;
}

int __atbm_sdio_memcpy_toio(struct atbm_sdio_func *func, unsigned int addr,
                            void *dst, int count)
{
    const uint8_t *p = (const uint8_t *)dst; /* vendor naming: 'dst' is the source */
    uint16_t blk = (uint16_t)func->blocksize;
    uint32_t rem = (uint32_t)count;

    if (func->en == 0) {
        return -1;
    }
    if ((((unsigned int)dst) & 3) || (count & 3)) {
        wifi_printk(WIFI_ALWAYS, "atbm sdio: write buf/len not 4-aligned (src=%p len=%d)\n", dst, count);
    }

    if (blk) {
        uint32_t nblocks = rem / blk;
        while (nblocks) {
            uint16_t chunk = (nblocks > SDIO_CMD53_BYTE_BLOCK_COUNT_MAX)
                                 ? SDIO_CMD53_BYTE_BLOCK_COUNT_MAX
                                 : (uint16_t)nblocks;
            int tries = ATBM_SDIO_XFER_RETRIES;
            SDIO_Status_Type st;
            while ((st = sdio_write_blocks(&g_host, (uint8_t)func->func, addr, p, chunk, false))
                       != Status_SDIO_Success && --tries) {
                wifi_printk(WIFI_ALWAYS, "atbm sdio: write_blocks retry st=0x%x f=%d addr=0x%x n=%d\n",
                            (unsigned)st, (int)func->func, addr, (int)chunk);
            }
            if (st != Status_SDIO_Success) {
                return -1;
            }
            p   += (uint32_t)chunk * blk;
            rem -= (uint32_t)chunk * blk;
            nblocks -= chunk;
        }
    }
    if (rem) {
        int tries = ATBM_SDIO_XFER_RETRIES;
        SDIO_Status_Type st;
        while ((st = atbm_sdio_rem_xfer(func, addr, (uint8_t *)p, (uint16_t)rem, 1))
                   != Status_SDIO_Success && --tries) {
            wifi_printk(WIFI_ALWAYS, "atbm sdio: write rem retry st=0x%x f=%d addr=0x%x len=%d\n",
                        (unsigned)st, (int)func->func, addr, (int)rem);
        }
        if (st != Status_SDIO_Success) {
            wifi_printk(WIFI_ALWAYS, "atbm sdio: write rem FAIL st=0x%x f=%d addr=0x%x len=%d\n",
                        (unsigned)st, (int)func->func, addr, (int)rem);
            return -1;
        }
    }
    return 0;
}

/* ---- CMD52 func0 register access ------------------------------------------- */
unsigned char atbm_sdio_f0_readb(struct atbm_sdio_func *func, unsigned int addr, int *retval)
{
    uint8_t v = 0;
    SDIO_Status_Type st;
    (void)func;
    st = sdio_readb(&g_host, SDIO_FUNC_0, addr, &v);
    *retval = (st == Status_SDIO_Success) ? 0 : -1;
    return v;
}

void atbm_sdio_f0_writeb(struct atbm_sdio_func *func, unsigned char regdata,
                         unsigned int addr, int *retval)
{
    SDIO_Status_Type st;
    (void)func;
    st = sdio_writeb(&g_host, SDIO_FUNC_0, addr, regdata);
    *retval = (st == Status_SDIO_Success) ? 0 : -1;
}

/* ---- Block size ------------------------------------------------------------ */
int atbm_sdio_set_blocksize(struct atbm_sdio_func *func, int blocksize)
{
    if (sdio_set_block_size(&g_host, (uint8_t)func->func, (uint16_t)blocksize)
            != Status_SDIO_Success) {
        return -1;
    }
    func->blocksize = blocksize;
    return 0;
}

atbm_uint32 atbm_sdio_alignsize(struct atbm_sdio_func *func, atbm_uint32 size)
{
    atbm_uint32 blk = func->blocksize;
    if (blk == 0 || size == 0) {
        return size;
    }
    return (size % blk == 0) ? size : (size + blk - (size % blk));
}
