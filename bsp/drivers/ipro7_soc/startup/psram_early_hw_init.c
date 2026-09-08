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
 * psram_early_hw_init.c - Register-level PSRAM init for earliest boot
 *
 * Called from startup.S BEFORE section copies. This function:
 *   1. Clears PDS PSRAM pad config
 *   2. Enables PSRAM clock from PLL_480M
 *   3. Initializes PSRAM controller (Winbond X8, 8MB)
 *   4. Runs DQS calibration (16-value sweep with quick R/W test)
 *   5. Returns calibrated DQS config (or 0 on failure)
 *
 * Constraints:
 *   - Runs from flash .text (XIP), uses only stack
 *   - No .data/.bss references (PSRAM not available yet)
 *   - No driver API calls — pure register writes
 *   - No SysTimer dependency — uses NOP-loop delays
 */

#include <stdint.h>
#include "early_print.h"

#if defined(CONFIG_IPRO7) && defined(CONFIG_USE_PSRAM) && defined(CONFIG_LINK_FLASH)

/* ================================================================
 * Register addresses
 * ================================================================ */

#define REG32(a)    (*(volatile uint32_t *)(a))

/* GLB */
#define GLB_BASE        0x30000000UL
#define CLK_CFG5        (GLB_BASE + 0x14)

/* PDS */
#define PDS_BASE        0x3008E000UL
#define PDS_PSRAM_PDPU  (PDS_BASE + 0x74)
#define PDS_PSRAM_IEOE  (PDS_BASE + 0x78)
#define PDS_PSRAM_O     (PDS_BASE + 0x7C)

/* PSRAM controller (PSRAM0) */
#define PSR_BASE        0x3001B000UL
#define PSR_CFG         (PSR_BASE + 0x00)   /* PSRAM_CONFIGURE */
#define PSR_MC1         (PSR_BASE + 0x04)   /* MANUAL_CONTROL */
#define PSR_MC2         (PSR_BASE + 0x0C)   /* MANUAL_CONTROL2 */
#define PSR_WBCFG       (PSR_BASE + 0x10)   /* WINBOND_PSRAM_CONFIGURE */
#define PSR_DQS5        (PSR_BASE + 0x114)  /* ROUGH_DELAY_CTRL5 */

/* PSRAM memory test address */
#define PSRAM_TEST_ADDR 0x1E000000UL

/* Winbond 8MB device ID */
#define WB_8MB_ID       0x0C86

/* ================================================================
 * Bit field helpers
 * ================================================================ */

/* CLK_CFG5 bits */
#define CLK_SEL_POS     26
#define CLK_SEL_MSK     (3U << CLK_SEL_POS)
#define CLK_DIV_EN      (1U << 28)
#define CLK_DIV_POS     29
#define CLK_DIV_MSK     (7U << CLK_DIV_POS)
#define CLK_PLL_480M    2

/* PSR_CFG bits */
#define CFG_VENDOR_POS  0
#define CFG_VENDOR_MSK  (7U << CFG_VENDOR_POS)
#define CFG_REGS_POS    8       /* WB_REG_SEL */
#define CFG_REGS_MSK    (7U << CFG_REGS_POS)
#define CFG_W_PULSE     (1U << 12)
#define CFG_R_PULSE     (1U << 13)
#define CFG_W_DONE      (1U << 14)
#define CFG_R_DONE      (1U << 15)
#define CFG_REQ         (1U << 16)
#define CFG_GNT         (1U << 17)
#define CFG_X16         (1U << 18)
#define CFG_PCK_POS     20
#define CFG_PCK_MSK     (7U << CFG_PCK_POS)

/* PSR_MC2 bits */
#define MC2_AMASK_POS   24
#define MC2_AMASK_MSK   (0xFFU << MC2_AMASK_POS)
#define MC2_DQS_POS     8
#define MC2_DQS_MSK     (0x7FU << MC2_DQS_POS)

/* DQS delay (ROUGH_DELAY_CTRL5, bits [15:0]) */
#define DQS_DELAY_MSK   0xFFFFU

/*
 * Winbond CR0 register value (at PSR_WBCFG offset 0x10).
 * Must match boot2_psram_init() / hal_psram_init() — final value 0x42002591.
 *   latency       = 1 (6 clocks, 166 MHz)     [3:0]
 *   driveStrength = 1 (35Ω / 115Ω for 8MB)    [6:4]
 *   brustType     = 1 (wrapped)               [7]    (a.k.a. HYBRID_EN bit)
 *   brustLen      = 5 (64 bytes)              [10:8]
 *   fixedLatency  = 0                         [12]
 *   dpd_dis       = 1                         [13]
 *   PASR          = 0                         [20:16]
 *   hybridSleep   = 0                         [24]
 *   linear_dis    = 1                         [25]
 *   IPD           = 0                         [29]
 *   mclk_type     = 1 (single-ended)          [30]
 *   sw_rst        = 0                         [31]
 */
#define WB_CR0_VAL  ((1U << 0) | (1U << 4) | (1U << 7) | (5U << 8) \
                   | (1U << 13) | (1U << 25) | (1U << 30))

/* Winbond register select enum: ID0=0, ID1=1, CR0=2, CR1=3 */
#define WB_REG_ID0  0
#define WB_REG_CR0  2

/* ================================================================
 * DQS delay lookup table (must be const, lives in .rodata/flash)
 * ================================================================ */

static const uint16_t dqs_table[16] = {
    0x8000, 0xC000, 0xE000, 0xF000,
    0xF800, 0xFC00, 0xFE00, 0xFF00,
    0xFF80, 0xFFC0, 0xFFE0, 0xFFF0,
    0xFFF8, 0xFFFC, 0xFFFE, 0xFFFF,
};

/* ================================================================
 * Internal helpers
 * ================================================================ */

static void nop_delay_us(uint32_t us)
{
    /* ~4 cycles per iteration at 32MHz ≈ 8 iters/us (conservative) */
    volatile uint32_t n = us * 8;
    while (n--)
        ;
}

/* Set fields in a register (read-modify-write) */
static inline void reg_rmw(uint32_t addr, uint32_t mask, uint32_t val)
{
    REG32(addr) = (REG32(addr) & ~mask) | val;
}

/* Request PSRAM controller config access */
static void psr_request(void)
{
    reg_rmw(PSR_CFG, CFG_REQ, CFG_REQ);
    for (int i = 0; i < 1000; i++) {
        if (REG32(PSR_CFG) & CFG_GNT)
            return;
    }
}

/* Release PSRAM controller config access */
static void psr_release(void)
{
    reg_rmw(PSR_CFG, CFG_REQ, 0);
}

/* Write a Winbond register (CR0/CR1) via controller */
static void psr_wb_write(uint32_t reg_sel, uint32_t wb_val)
{
    psr_request();

    /* Load Winbond config value */
    REG32(PSR_WBCFG) = wb_val;

    /* Select register */
    reg_rmw(PSR_CFG, CFG_REGS_MSK, reg_sel << CFG_REGS_POS);

    /* Pulse write */
    reg_rmw(PSR_CFG, CFG_W_PULSE, CFG_W_PULSE);

    /* Wait for done */
    for (int i = 0; i < 1000; i++) {
        if (REG32(PSR_CFG) & CFG_W_DONE)
            break;
    }

    psr_release();
}

/* Read a Winbond register (ID0) via controller, return value */
static uint16_t psr_wb_read(uint32_t reg_sel)
{
    psr_request();

    /* Select register */
    reg_rmw(PSR_CFG, CFG_REGS_MSK, reg_sel << CFG_REGS_POS);

    /* Pulse read */
    reg_rmw(PSR_CFG, CFG_R_PULSE, CFG_R_PULSE);

    /* Wait for done */
    for (int i = 0; i < 1000; i++) {
        if (REG32(PSR_CFG) & CFG_R_DONE)
            break;
    }

    uint16_t val = (uint16_t)(REG32(PSR_MC1) >> 16);

    psr_release();
    return val;
}

/*
 * Initialize PSRAM controller with a given DQS delay, write CR0,
 * read ID0, return device ID.
 */
static uint16_t psram_ctrl_init_dqs(uint16_t dqs_delay)
{
    uint32_t v;

    /* 150us power-up delay */
    nop_delay_us(150);

    /* DQS delay */
    v = REG32(PSR_DQS5);
    v = (v & ~DQS_DELAY_MSK) | dqs_delay;
    REG32(PSR_DQS5) = v;

    /* Controller config: vendor=Winbond(1), X8 mode, PCK_S_DIV=1 */
    v = REG32(PSR_CFG);
    v = (v & ~CFG_VENDOR_MSK) | (1U << CFG_VENDOR_POS);
    v = (v & ~CFG_PCK_MSK) | (1U << CFG_PCK_POS);
    v &= ~CFG_X16;
    REG32(PSR_CFG) = v;

    /* Address mask = 0x07 (8MB), DQS release = 0x1f */
    v = REG32(PSR_MC2);
    v = (v & ~MC2_AMASK_MSK) | (0x07U << MC2_AMASK_POS);
    v = (v & ~MC2_DQS_MSK) | (0x1FU << MC2_DQS_POS);
    REG32(PSR_MC2) = v;

    /* Write Winbond CR0 */
    psr_wb_write(WB_REG_CR0, WB_CR0_VAL);

    /* Read Winbond ID0 */
    return psr_wb_read(WB_REG_ID0);
}

/* Quick R/W test: 4-word pattern at PSRAM base address */
static int psram_quick_rw_test(void)
{
    volatile uint32_t *p = (volatile uint32_t *)PSRAM_TEST_ADDR;

    p[0] = 0xAA55AA55;
    p[1] = 0x33CC33CC;
    p[2] = 0x55AA55AA;
    p[3] = 0xCC33CC33;

    __asm volatile("fence" ::: "memory");

    if (p[0] != 0xAA55AA55) return 0;
    if (p[1] != 0x33CC33CC) return 0;
    if (p[2] != 0x55AA55AA) return 0;
    if (p[3] != 0xCC33CC33) return 0;
    return 1;
}

/* ================================================================
 * Public API — called from startup.S
 * ================================================================ */

/*
 * psram_early_hw_init
 *
 * Returns: DQS config (bits [7:4]=left, [3:0]=right) on success
 *          0 on failure
 *
 * After successful return, PSRAM at 0x1E000000 is accessible and
 * section copies can proceed.
 */
uint32_t psram_early_hw_init(void)
{
    int dqs_min = 16, dqs_max = -1;

    /* Early print init — needed for fail-path messages.
     * Success path only emits one final dqs-window summary at the end
     * (each early_flush() blocks ~1.3 ms on UART TX drain @115200,
     * so verbose progress prints used to cost ~7 ms here). */
    early_print_init();

    /* 1. Clear PDS PSRAM pad overrides */
    REG32(PDS_PSRAM_PDPU) = 0;
    REG32(PDS_PSRAM_IEOE) = 0;
    REG32(PDS_PSRAM_O) = 0;

    /* 1b. Init GPIO 50-61 (PSRAM pads): IE=1, SMT=1, no pull */
    /* CFGCTL25..CFGCTL30, each holds 2 GPIOs, 16 bits each */
    /* Per-pin value: IE(bit0)=1, SMT(bit1)=1 = 0x0003 */
    for (uint32_t off = 0x164; off <= 0x178; off += 4)
        REG32(EP_GLB_BASE + off) = 0x00030003;

    /* 2. Enable PSRAM clock: PLL_480M, div=0
     *    Caller is responsible for ensuring PLL_480M tap is actually
     *    running.  Normal boot: brom's ipro_bootrom_clk_init powers PLL
     *    on + enables taps.  pure_dcdc fastboot wake: pure_dcdc_save_state
     *    snapshots pre-bounce PLL/clock state and the wakeup stub restores
     *    it (see bsp/drivers/ipro7_soc/startup/pure_dcdc_wakeup.c).  This
     *    preserves the "use whatever clock was active pre-bounce" semantic
     *    — works whether pre-bounce was PLL-based or XTAL-based. */
    {
        uint32_t v = REG32(CLK_CFG5);
        v &= ~CLK_DIV_EN;          /* disable first */
        REG32(CLK_CFG5) = v;

        v = REG32(CLK_CFG5);
        v = (v & ~CLK_SEL_MSK) | (CLK_PLL_480M << CLK_SEL_POS);
        v &= ~CLK_DIV_MSK;         /* div = 0 */
        REG32(CLK_CFG5) = v;

        v = REG32(CLK_CFG5);
        v |= CLK_DIV_EN;           /* enable */
        REG32(CLK_CFG5) = v;
    }

    /* 3. Read efuse psram_trim — direct register read, no driver API */
    {
        /*
         * psram_trim location in efuse (from drv_ef_cfg.c trim_lit[]):
         *   en_addr     = 0x58*8 + 29 = bit 733  → word 0x5C[29]
         *   parity_addr = 0x58*8 + 28 = bit 732  → word 0x5C[28]
         *   value_addr  = 0x58*8 + 20 = bit 724  → word 0x5C[20..27]
         *   value_len   = 8
         *
         * All in efuse shadow register at EF_DATA_BASE + 0x5C
         */
        #define EF_DATA_BASE_   0x30008000UL
        #define EF_PSRAM_REG    (EF_DATA_BASE_ + 0x58)

        /* Efuse shadow registers are auto-loaded on reset — read directly */
        uint32_t ef_word = REG32(EF_PSRAM_REG);
        uint8_t  ef_en     = (ef_word >> 29) & 1;
        uint8_t  ef_parity = (ef_word >> 28) & 1;
        uint32_t ef_value  = (ef_word >> 20) & 0xFF;

        /* Calculate expected parity (popcount & 1) */
        uint8_t parity_calc = 0;
        for (int b = 0; b < 8; b++)
            parity_calc ^= (ef_value >> b) & 1;

        if (ef_en && ef_parity == parity_calc) {
            /* Efuse DQS config valid — use it directly (fast path) */
            int c_val = (((ef_value >> 4) & 0xF) + (ef_value & 0xF)) >> 1;

            if (psram_ctrl_init_dqs(dqs_table[c_val]) != WB_8MB_ID) {
                early_puts("[EP] FAIL: efuse_id\n");
                early_flush();
                return 0;
            }
            if (!psram_quick_rw_test()) {
                early_puts("[EP] FAIL: efuse_rw\n");
                early_flush();
                return 0;
            }

            /* Success: no UART output on hot path — that 15-char message
             * + early_flush() blocks ~1.3 ms on UART TX FIFO drain @115200,
             * dominating PhaseC.  Visibility for the dqs window value is
             * already provided by main() via printf later if needed. */
            return ef_value & 0xFF;
        }
    }

    /* 4. No efuse — full DQS calibration sweep */

    /* Initial probe with default safe DQS */
    if (psram_ctrl_init_dqs(0xFFC0) != WB_8MB_ID) {
        early_puts("[EP] FAIL: probe_id\n");
        early_flush();
        return 0;
    }

    for (int i = 0; i < 16; i++) {
        uint16_t id = psram_ctrl_init_dqs(dqs_table[i]);
        if (id == WB_8MB_ID && psram_quick_rw_test()) {
            if (i < dqs_min) dqs_min = i;
            if (i > dqs_max) dqs_max = i;
        }
    }

    /* 5. Validate window */
    int window = dqs_max - dqs_min;
    if (dqs_max < 0 || window < 4 || window > 15) {
        early_puts("[EP] FAIL: win=");
        early_print_hex4(dqs_min);
        early_putc('-');
        early_print_hex4(dqs_max);
        early_puts("\n");
        early_flush();
        return 0;
    }

    /* 6. Re-init with center value */
    int c_val = (dqs_min + dqs_max) >> 1;
    if (psram_ctrl_init_dqs(dqs_table[c_val]) != WB_8MB_ID) {
        early_puts("[EP] FAIL: center_id\n");
        early_flush();
        return 0;
    }

    if (!psram_quick_rw_test()) {
        early_puts("[EP] FAIL: center_rw\n");
        early_flush();
        return 0;
    }

    /* Success: emit single dqs-window summary */
    early_puts("[EP] win=");
    early_print_hex4(dqs_min);
    early_putc('-');
    early_print_hex4(dqs_max);
    early_puts("\n");
    early_flush();

    /* Return packed dqs_cfg: [7:4]=left, [3:0]=right */
    return ((dqs_min << 4) | dqs_max) & 0xFF;
}

#endif /* CONFIG_IPRO7 && CONFIG_USE_PSRAM && CONFIG_LINK_FLASH */
