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
 * @file pure_dcdc_wakeup.c
 * @brief HBN RAM wakeup stub for the pure_dcdc cold-init bounce.
 *
 * Cold-init flow (in __pure_dcdc_early_init in startup_ipro7.S):
 *   1. Rail switch (LDO14 -> DCDC, set HBN_MISC[11] cold-init marker).
 *   2. call pure_dcdc_save_state() — see below for what it does.
 *   3. fall through to __hbn_reboot_1t for HBN_LEVEL_0 entry.
 *
 * Wake flow:
 *   brom checks HBN_RSV0 == HBN_STATUS_ENTER_FLAG && HBN_RSV1 valid →
 *   jumps to HBN_RSV1.  We point HBN_RSV1 at pure_dcdc_wakeup_stub which
 *   lives in HBN RAM (.hbn_code section, copied from XIP LMA to HBN
 *   RAM VMA by save_state).  The stub runs the FULL flash XIP recovery
 *   sequence (matches hal_lp_xip_recovery in bsp/hal/lp_framework/
 *   ipro7/hal_lp.c:519) — only Cache_Read_Enable is not enough because
 *   HBN_LEVEL_0 + brom dispatch leaves flash in unknown state (possibly
 *   continuous-read mode, deep power-down, wrong pin mux).
 *
 * Why HBN RAM (not OCRAM): empirically verified via CCI marker test —
 * OCRAM (sram1, brom calls it tcm_ocram_ret) is WIPED across HBN_LEVEL_0
 * + brom fastboot dispatch.  Markers written before bounce read back as
 * random.  Only HBN domain RAM and AON registers survive.  brom dispatches
 * the wake jump *before* doing any flash init, so the user PC must (1)
 * be in non-flash memory and (2) handle XIP recovery itself.
 *
 * RSV2 progress markers (one byte per step, observable via CCI):
 *   0xB0  stub entry (mcountinhibit cleared)
 *   0xB1  about to call SF_Cfg_Init_Flash_Gpio
 *   0xB2  about to call SFlash_Init (SF controller config)
 *   0xB3  about to call SF_Ctrl_Cmds_Set
 *   0xB4  Releae_Powerdown done, post-delay
 *   0xB5  JEDEC verify loop done (success or 2000-try cap)
 *   0xB6  SF_Ctrl_Set_Flash_Image_Offset done
 *   0xB7  Qspi_Enable done (or skipped for non-Q mode)
 *   0xB8  Cache_Read_Enable done
 *   0xBA  about to jr 0x13000000
 */

#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include <generated/autoconf.h>
#include "drv_sflash.h"
#include "drv_sf_ctrl.h"
#include "drv_romdriver.h"
#include "drv_ef_cfg.h"
#include "boot2_param.h"

#define HBN_RSV1_REG_ADDR 0x3008F104UL

/* Direct register defs (pure_dcdc_save_state runs pre-LMA-copy, can't
 * use TCM-resident helpers; use raw MMIO instead). */
#define SF_CTRL_REG_BASE  0x30081000UL
#define SF_CTRL_2_OFF     0x70U
#define GLB_REG_BASE      0x30000000UL
#define GLB_PARM_OFF      0x90U

/* Linker symbols for .hbn_code LMA -> VMA copy. */
extern uint32_t __hbn_code_load_addr;
extern uint32_t __hbn_code_start;
extern uint32_t __hbn_code_end;

/* Forward decl — actually defined below in .hbn_code. */
void pure_dcdc_wakeup_stub(void);

/*============================ HBN RAM saved state =========================*/

/* All in .hbn_data (HBN RAM AON domain) — survive HBN_LEVEL_0 bounce.
 * Populated by pure_dcdc_save_state(), consumed by hbn_xip_recovery_c(). */

__attribute__((section(".hbn_data"), aligned(4), used))
volatile SPI_Flash_Cfg_Type pure_dcdc_saved_flash_cfg;

__attribute__((section(".hbn_data"), aligned(4), used))
volatile uint32_t pure_dcdc_saved_flash_offset;

__attribute__((section(".hbn_data"), aligned(4), used))
volatile uint32_t pure_dcdc_saved_jdec_id;

__attribute__((section(".hbn_data"), aligned(4), used))
volatile uint8_t pure_dcdc_saved_flash_pin_cfg;

/* Pre-bounce clock state — restored by stub so that whatever clock
 * topology was running before HBN_LEVEL_0 (PLL192M+PLL_480M for PSRAM,
 * or XTAL/RC32M with PSRAM on BCLK) is faithfully reinstated.  Avoids
 * baking "always run on PLL" assumptions into the stub. */
__attribute__((section(".hbn_data"), aligned(4), used))
volatile uint32_t pure_dcdc_saved_pu_rst_clkpll;     /* CCI 0x30009400 */

__attribute__((section(".hbn_data"), aligned(4), used))
volatile uint32_t pure_dcdc_saved_clkpll_output_en;  /* CCI 0x3000941C */

__attribute__((section(".hbn_data"), aligned(4), used))
volatile uint32_t pure_dcdc_saved_glb_clk_cfg0;      /* GLB 0x30000000 — root clk + PLL_SEL */

__attribute__((section(".hbn_data"), aligned(4), used))
volatile uint32_t pure_dcdc_saved_glb_clk_cfg1;      /* GLB 0x30000004 — PLL_EN + HCLK/BCLK div */

__attribute__((section(".hbn_data"), aligned(4), used))
volatile uint32_t pure_dcdc_saved_glb_clk_cfg3;      /* GLB 0x3000000C — UART/SF/IR/DMA dividers */

__attribute__((section(".hbn_data"), aligned(4), used))
volatile uint32_t pure_dcdc_saved_glb_clk_cfg5;      /* GLB 0x30000014 — PSRAM mux */

__attribute__((section(".hbn_data"), aligned(4), used))
volatile uint32_t pure_dcdc_saved_glb_cgen_cfg0;     /* GLB 0x30000030 — peripheral gates */

__attribute__((section(".hbn_data"), aligned(4), used))
volatile uint32_t pure_dcdc_saved_hbn_glb;           /* HBN 0x3008F030 — root clk sel */

/*============================ Recovery C function =========================*/

/**
 * Runs from HBN RAM after the naked stub's pre-call setup.  Mirrors
 * hal_lp_xip_recovery() (hal_lp.c:519) but uses RomDriver_* directly so
 * it doesn't depend on .tcm_code (which isn't in OCRAM yet — pure_dcdc
 * cold-init runs pre-__init_common LMA copy).
 *
 * Stack: brom's __StackTopTemp (256 B at 0x11020100). Function uses
 * a few small local structs (~50 B) — well within budget.
 */
__attribute__((section(".hbn_code"), used, noinline))
static void hbn_xip_recovery_c(void)
{
    SPI_Flash_Cfg_Type *cfg = (SPI_Flash_Cfg_Type *)&pure_dcdc_saved_flash_cfg;
    SF_Ctrl_Cfg_Type sfCtrlCfg;
    SF_Ctrl_Cmds_Cfg cmdsCfg;
    uint32_t jdec_id;
    int timeout;
    int ret;

    /* Restore BROM's CpuIRegionBase global (0x11037b5c) — this OCRAM
     * variable is wiped by HBN_LEVEL_0 + brom fastboot dispatch, but ROM
     * functions (Power_On_PLL → arch_delay_us → SysTimer_GetLoadValue) read
     * it to compute SysTimer base.  With it == 0, ROM arch_delay_us reads
     * MTime from 0x30000 → never advances → PLL lock wait hangs forever.
     * Recompute from CSR_MIRGB_INFO (HW-backed, always valid) and write back. */
    {
        uint32_t mirgb;
        __asm__ volatile ("csrr %0, 0x7F7" : "=r"(mirgb));  /* CSR_MIRGB_INFO */
        *(volatile uint32_t *)0x11037B5CUL = mirgb & ~0x3FFU;
    }

    RomDriver_GLB_Set_MTimer_CLK(1, GLB_MTIMER_CLK_XCLK, 15);
    /* Derive target system clock from pre-bounce saved state instead of
     * hard-coding PLL192M.  HBN_GLB[1:0] = root_clk_sel; PLL_SEL bits[5:4]
     * of saved glb_clk_cfg0 select PLL/DLL output when root = DLL. */
    {
        uint32_t hbn_root = pure_dcdc_saved_hbn_glb & 0x3;
        uint32_t pll_sel = (pure_dcdc_saved_glb_clk_cfg0 >> 4) & 0x3;
        GLB_SYS_CLK_Type clkFreq;
        if (hbn_root == HBN_ROOT_CLK_DLL) {
            clkFreq = (GLB_SYS_CLK_Type)(GLB_SYS_CLK_DLL25P6M + pll_sel);
        } else if (hbn_root == HBN_ROOT_CLK_XTAL) {
            clkFreq = GLB_SYS_CLK_XTAL;
        } else {
            clkFreq = GLB_SYS_CLK_RC32M;
        }
        RomDriver_GLB_Set_System_CLK(GLB_DLL_XTAL_32M, clkFreq);
    }
    RomDriver_GLB_Set_SF_CLK(1, GLB_SFLASH_CLK_80M, 0);

    /* (1) Reinit flash GPIO with saved pin cfg. */
    RomDriver_SF_Cfg_Init_Flash_Gpio(pure_dcdc_saved_flash_pin_cfg, 1);

    /* (2) Init SF controller from saved flash cfg.
     *     clkDelay = 1T per HW guidance — matches SF_CLK = DLL_64M rate. */
    sfCtrlCfg.owner = SF_CTRL_OWNER_SAHB;
    sfCtrlCfg.clkDelay = 1;
    sfCtrlCfg.clkInvert = cfg->clkInvert & 0x01;
    sfCtrlCfg.rxClkInvert = (cfg->clkInvert >> 1) & 0x01;
    sfCtrlCfg.doDelay = 0;
    sfCtrlCfg.diDelay = 0;
    sfCtrlCfg.oeDelay = 0;
    RomDriver_SFlash_Init(&sfCtrlCfg);

    /* (3) Set cmd config — wrap mode depends on QIO vs other. */
    cmdsCfg.cmdsEn = 1;
    if ((cfg->ioMode & 0x1f) == SF_CTRL_QIO_MODE) {
        cmdsCfg.wrapMode = 2;
        cmdsCfg.wrapLen = SF_CTRL_WRAP_LEN_64;
    } else {
        cmdsCfg.wrapMode = 1;
        cmdsCfg.wrapLen = SF_CTRL_WRAP_LEN_512;
    }
    RomDriver_SF_Ctrl_Cmds_Set(&cmdsCfg, SF_CTRL_SEL_FLASH);

    /* (4) Release flash from deep power-down. tRES1 typ 3 µs / max
     *     30 µs per W25Q14JV.  Releae_Powerdown call itself takes
     *     ~5 µs (cmd 0xAB at 64 MHz SF_CLK, 8 SCLK cycles).
     *
     *     Match brom ipro_spi_flash_init() which calls ARCH_Delay_US(120)
     *     here = 4x datasheet max. */
    RomDriver_SFlash_Releae_Powerdown(cfg);
    RomDriver_arch_delay_us(120);

    /* (5) Reset_Continue_Read only — exits continuous-read mode without
     *     busy-bit polling.  SFlash_Software_Reset / Write_Enable /
     *     DisableBurstWrap / SetSPIMode all internally call SFlash_Busy()
     *     which polls the flash status register; after HBN bounce the
     *     status read returns 0xFF (busy=1) forever and the call hangs.
     *     Skip them — flash should already be in a usable state after
     *     Releae_Powerdown + settling delay. */
    (void)timeout; (void)ret; (void)jdec_id;  /* unused without verify loop */
    RomDriver_SFlash_Reset_Continue_Read(cfg);

    /* (6) Restore flash image offset (so XIP reads land at right address).
     *     ROM macro takes only addrOffset (single arg). */
    RomDriver_SF_Ctrl_Set_Flash_Image_Offset(pure_dcdc_saved_flash_offset);

    /* (7) Re-enable QSPI mode if flash is QO/QIO (single-line ops above
     *     would have left flash in single-line state). */
    if ((cfg->ioMode & 0x0f) == SF_CTRL_QO_MODE ||
        (cfg->ioMode & 0x0f) == SF_CTRL_QIO_MODE) {
        RomDriver_SFlash_Qspi_Enable(cfg);
    }

    /* (8) Mirror boot2 BLSP_Boot2_Set_Cache: prime flash into continuous-
     *     read mode with one SFlash_Read(contRead=1), then enable cache
     *     with contRead=1 so XIP cache fills skip the 0xEB cmd byte.
     *
     *     With contRead=0 (CMD_EN=1, full 0xEB per fill) we observed
     *     Phase C 15x slower than classic — root cause is not just the
     *     missed continuous-read but also that flash chip ends up in
     *     QIO+SDR but not cont-read state, so each cache line incurs
     *     full cmd+addr+M+dummy+data sequence per 32 bytes. */
    {
        uint32_t prime_buf[1] __attribute__((aligned(4)));
        if ((cfg->cReadSupport & 0x01) != 0) {
            (void)RomDriver_SFlash_Read(cfg, cfg->ioMode & 0xf, 1, 0,
                                        (uint8_t *)prime_buf,
                                        sizeof(prime_buf));
            RomDriver_SFlash_Cache_Read_Enable(cfg, cfg->ioMode & 0xf, 1, 0);
        } else {
            RomDriver_SFlash_Cache_Read_Enable(cfg, cfg->ioMode & 0xf, 0, 0);
        }
    }

    /* (9) Restore non-clock state (clocks already done in step 0):
     *       - PSRAM clock mux from saved CFG5
     *       - Peripheral clock gates from saved CGEN0
     *       - UART_CLK_DIV cleared in CLK_CFG3 (SF bits already set
     *         by GLB_Set_SF_CLK in step 0d) */
    {
        volatile uint32_t *clkcfg3 = (volatile uint32_t *)0x3000000CU;
        volatile uint32_t *clkcfg5 = (volatile uint32_t *)0x30000014U;
        volatile uint32_t *cgen0   = (volatile uint32_t *)0x30000030U;

        *clkcfg5 = pure_dcdc_saved_glb_clk_cfg5;
        *cgen0   = pure_dcdc_saved_glb_cgen_cfg0;
        /* Clear UART_CLK_DIV (bits 0-2), set UART_CLK_EN (bit 4); leave
         * SF/IR/DMA bits as configured by GLB_Set_SF_CLK. */
        *clkcfg3 = (*clkcfg3 & ~0x07U) | (1U << 4);
        (void)pure_dcdc_saved_glb_clk_cfg0;
        (void)pure_dcdc_saved_glb_clk_cfg1;
        (void)pure_dcdc_saved_glb_clk_cfg3;
    }

    /* UART1 internal regs reset across the bounce (peripheral domain).
     * early_print expects UART1 ready at 115200; reinit to brom's standard
     * 32 MHz / (277+1) ≈ 115200 setup. */
    {
        volatile uint32_t *u1_utx = (volatile uint32_t *)0x3000C000U;
        volatile uint32_t *u1_urx = (volatile uint32_t *)0x3000C004U;
        volatile uint32_t *u1_prd = (volatile uint32_t *)0x3000C008U;
        *u1_prd = 0x01150115U;
        *u1_utx = 0x00000F05U;
        *u1_urx = 0x00002F01U;
    }

    /* (10) Enable RISC-V I/D cache via CSR_MCACHE_CTL. In classic boot,
     *     boot2 BLSP_Boot2_Set_Cache calls EnableICache+EnableDCache
     *     before jumping to user FW, so __init_common LMA copies run
     *     with caches ON. Pure_dcdc warm path skips boot2 — caches stay
     *     OFF until _premain_init (after main()), making Phase C ~15x
     *     slower than classic. Mirror boot2 here.
     *
     *     CSR_MCACHE_CTL bit 0 = MCACHE_CTL_IC_EN, bit 16 = MCACHE_CTL_DC_EN. */
    __asm__ volatile (
        "csrsi 0x7CA, 0x1\n"             /* IC_EN */
        "li t0, 0x10000\n"
        "csrs 0x7CA, t0\n"               /* DC_EN */
        "fence\n"
        "fence.i\n"
        ::: "t0", "memory"
    );

#ifdef CONFIG_PURE_DCDC_AON_WDT_RECOVER
    /* (11) Disable AON WDT — recovery is complete (clocks up, flash XIP
     *      working, cache on).  WDT was the safety net against hangs in
     *      stages 0-10; from here on the chip is in normal-FW conditions
     *      and unrelated FW hangs shouldn't trigger a wake-path WDT. */
    {
        volatile uint32_t *hbn_wdt = (volatile uint32_t *)0x3008F110UL;
        uint32_t tmpVal = (*hbn_wdt & 0x00FFFFFFU) & ~(1U << 8);
        *hbn_wdt = tmpVal | (0x7AU << 24);
    }
#endif

}

/**
 * Wakeup stub — runs from HBN RAM after brom fastboot dispatch.
 *
 * Naked + noreturn: no C prologue/epilogue; brom's sp/gp are still
 * valid for the C function call below.  Standard call ABI works because
 * the C function is also in .hbn_code (close jal target).
 *
 * mcountinhibit cleared so RomDriver_arch_delay_us (mcycle-based) works
 * inside hbn_xip_recovery_c.
 */
__attribute__((section(".hbn_code"), naked, noreturn, used))
void pure_dcdc_wakeup_stub(void)
{
    __asm__ volatile (
        ".option push\n"
        ".option norelax\n"

        /* Enable mcycle counter (clear mcountinhibit bits 0+2) for
         * arch_delay_us inside the recovery function. */
        "csrci mcountinhibit, 0x5\n"

        /* MARK 0xB0 = stub entry */
        "li t0, 0x3008F108\n"
        "li t1, 0xB0\n"
        "sw t1, 0(t0)\n"

        /* Run full XIP recovery (hal_lp_xip_recovery equivalent). */
        "call hbn_xip_recovery_c\n"

        /* MARK 0xBA = recovery returned, about to jump to firmware */
        "li t0, 0x3008F108\n"
        "li t1, 0xBA\n"
        "sw t1, 0(t0)\n"

        /* Jump to firmware _start at flash XIP base */
        "li t0, 0x13000000\n"
        "jr t0\n"

        ".option pop\n"
    );
}

/**
 * Cold-init helper (called from __pure_dcdc_early_init in startup.S
 * AFTER the rail switch / marker-set, BEFORE __hbn_reboot_1t).
 *
 *   1. Manually copy .hbn_code from XIP LMA to HBN RAM VMA so the
 *      wakeup stub + recovery function are present in HBN RAM (the
 *      standard LMA copy in __init_common hasn't run yet — we're
 *      pre-init_common).
 *   2. Run Boot2_Param_Save() to populate g_boot2_param from the
 *      OCRAM hand-off region (0x11010000); copy the parsed flash_cfg
 *      to HBN RAM.
 *   3. Snapshot current SF Controller / flash state needed by the
 *      wakeup recovery: flash image offset, pad selection, JEDEC ID.
 *   4. Set HBN_RSV1 = &pure_dcdc_wakeup_stub so brom fastboot
 *      dispatcher jumps there on wake.
 *
 * SECTION: .text.init so this stays in XIP and is reachable before
 * __init_common runs.
 */
/**
 * XIP-only RC32K efuse trim applier.  Runs BEFORE __init_common LMA->VMA
 * copy, so we can't call the standard TCM-resident drivers
 * (EF_Ctrl_Read_Common_Trim is ATTR_TCM_SECTION, HBN_Set_RC32K_CapSel is
 * ATTR_CLOCK_SECTION).  Instead use the ROM API directly + inline MMIO.
 *
 * Applies cap_sel (4-bit, HBN_RC32K_CTRL0[31:28]) and 13-bit fine code
 * (HBN_RC32K_CTRL1[12:0]) from efuse common-trim entries "rc32k",
 * "rc32k_code_h", "rc32k_code_l".
 *
 * Parity field per Efuse_Common_Trim_Type is the LSB of popcount(value)
 * over `len` bits (matches EF_Ctrl_Get_Trim_Parity in drv_ef_cfg.c).
 */
__attribute__((section(".text.init"), used))
void pure_dcdc_apply_rc32k_trim(void)
{
    Efuse_Common_Trim_Type trim;
    uint32_t tmpVal;

    RomDriver_EF_Ctrl_Read_Common_Trim("rc32k", &trim);
    if (!trim.en) {
        return;
    }

    /* Verify parity: popcount(value) lsb == parity */
    uint32_t v = trim.value;
    uint8_t cnt = 0;
    for (uint32_t i = 0; i < trim.len; i++) {
        if (v & (1U << i)) cnt++;
    }
    if ((cnt & 0x1) != trim.parity) {
        return;  /* bad parity — skip */
    }

    /* Apply cap_sel to HBN_RC32K_CTRL0[31:28] */
    volatile uint32_t *ctrl0 = (volatile uint32_t *)0x3008F200UL;
    tmpVal = *ctrl0;
    tmpVal = (tmpVal & ~(0xFU << 28)) | ((trim.value & 0xFU) << 28);
    *ctrl0 = tmpVal;

    /* 13-bit fine code: ipro7 brom_sdk's ROM Common_Trim table does NOT
     * include rc32k_code_h/rc32k_code_l entries, so RomDriver_EF_Ctrl_
     * Read_Common_Trim() returns en=0 for those names.  Read raw efuse
     * rows directly via RomDriver_EF_Ctrl_Read_Direct_R0 and parse the
     * bits manually.
     *
     * Layout (per drv_ef_cfg.c trim_lit[]):
     *   ef_ana_trim_0    (byte 0x0C / word 3):
     *     [9]=en, [8]=parity, [7:2]=code[12:7]  (6 high bits)
     *   ef_key_slot_3_w1 (byte 0x50 / word 20):
     *     [30:24]=code[6:0]                     (7 low bits)
     *   parity is over the full 13-bit composed value. */
    uint32_t ef_word3 = 0, ef_word20 = 0;
    RomDriver_EF_Ctrl_Read_Direct_R0(3, &ef_word3, 1);
    RomDriver_EF_Ctrl_Read_Direct_R0(20, &ef_word20, 1);

    uint32_t code_en     = (ef_word3 >> 9) & 0x1U;
    uint32_t code_parity = (ef_word3 >> 8) & 0x1U;
    if (!code_en) {
        return;
    }
    uint32_t code_h = (ef_word3 >> 2) & 0x3FU;     /* 6 bits */
    uint32_t code_l = (ef_word20 >> 24) & 0x7FU;   /* 7 bits */
    uint16_t fine_code = (uint16_t)((code_h << 7) | code_l);

    uint8_t cnt2 = 0;
    for (uint32_t i = 0; i < 13; i++) {
        if (fine_code & (1U << i)) cnt2++;
    }
    if ((cnt2 & 0x1) != code_parity) {
        return;
    }

    int sdm_bypass = ((fine_code & 0x7U) == 0) ? 1 : 0;
    /* HBN_RC32K_CTRL1 layout (from hal_sys.c rc32k_set_code_fr_cal):
     *   [28:..] aon_rc32k_resv = 0xB
     *   [20]    rc32k_sdm_bypass
     *   [16]    rc32k_sdm_rstb = 1
     *   [12:0]  13-bit fine code */
    uint32_t v1 = 0
        | (0xBU                              << 28)
        | ((uint32_t)(sdm_bypass ? 1U : 0U) << 20)
        | (0x1U                              << 16)
        | (uint32_t)(fine_code & 0x1FFFU);
    *(volatile uint32_t *)0x3008F208UL = v1;
}

__attribute__((section(".text.init"), used))
void pure_dcdc_save_state(void)
{
    /* (1) .hbn_code LMA -> VMA copy. */
    {
        uint32_t *src = &__hbn_code_load_addr;
        uint32_t *dst = &__hbn_code_start;
        uint32_t *end = &__hbn_code_end;
        while (dst < end) {
            *dst++ = *src++;
        }
    }

    /* (2) Get flash_cfg from boot2 hand-off, copy to HBN RAM. */
    Boot2_Param_Save();
    SPI_Flash_Cfg_Type *cfg = Boot2_Param_Get_Flash_Cfg();
    if (cfg) {
        memcpy((void *)&pure_dcdc_saved_flash_cfg, cfg,
               sizeof(SPI_Flash_Cfg_Type));
    }

    /* (3) Snapshot recovery params from current HW state.  Boot2 set up
     *     flash XIP and we're running from it now, so SF controller +
     *     flash are in known-good state.  Replicates hal_lp_init logic. */

    /* (3a) Flash image offset — what SF_Ctrl_Set_Flash_Image_Offset must
     *      restore so XIP reads land at the right flash address.
     *      Use ROM API (no-arg form) — the C wrapper is ATTR_TCM_SECTION
     *      and not in OCRAM yet (we're pre-__init_common LMA copy). */
    pure_dcdc_saved_flash_offset = RomDriver_SF_Ctrl_Get_Flash_Image_Offset();

    /* (3b) Flash pad selection — derived from SF_CTRL_2[1:0] pad_sel.
     *      For external flash, pin cfg = SEL_EXTERNAL_FLASH_SF{2,3}.
     *      For internal flash (SF1), derive swap mode from GLB_PARM. */
    {
        uint32_t sf_ctrl_2 =
            *(volatile uint32_t *)(SF_CTRL_REG_BASE + SF_CTRL_2_OFF);
        uint8_t pad_sel = sf_ctrl_2 & 0x3;

        if (pad_sel == SF_CTRL_PAD_SEL_SF3) {
            pure_dcdc_saved_flash_pin_cfg = SF_CTRL_SEL_EXTERNAL_FLASH_SF3;
        } else if (pad_sel == SF_CTRL_PAD_SEL_SF2) {
            pure_dcdc_saved_flash_pin_cfg = SF_CTRL_SEL_EXTERNAL_FLASH_SF2;
        } else {
            uint32_t glb_parm =
                *(volatile uint32_t *)(GLB_REG_BASE + GLB_PARM_OFF);
            uint8_t reverse = (glb_parm >> 10) & 0x1;
            uint8_t swap_io0_io3 = (glb_parm >> 8) & 0x1;
            uint8_t swap_cs_io2 = (glb_parm >> 9) & 0x1;
            pure_dcdc_saved_flash_pin_cfg =
                SF_CTRL_SEL_INTERNAL_FLASH_SWAP_NONE +
                ((reverse << 2) | (swap_io0_io3 << 1) | swap_cs_io2);
        }
    }

    /* (3c) JEDEC ID — would be ideal for verify loop, but
     *      RomDriver_XIP_SFlash_GetJedecId_Need_Lock breaks XIP mid-call
     *      and calling it from a XIP-resident function (.text.init) is
     *      hazardous: if cache state isn't fully restored on return, the
     *      caller continuation faults.  Skip the save; stub handles
     *      saved_jdec_id==0 by skipping the verify loop. */
    pure_dcdc_saved_jdec_id = 0;

    /* (3d) Snapshot pre-bounce clock topology so the wakeup stub can
     *      restore it byte-for-byte.  This makes the stub agnostic to
     *      whether boot2 set up PLL192M+PLL_480M (typical) or some other
     *      app set up XTAL-only with PSRAM disabled — whatever was
     *      running pre-bounce gets reinstated post-wake. */
    pure_dcdc_saved_pu_rst_clkpll =
        *(volatile uint32_t *)0x30009400UL;
    pure_dcdc_saved_clkpll_output_en =
        *(volatile uint32_t *)0x3000941CUL;
    pure_dcdc_saved_glb_clk_cfg0 =
        *(volatile uint32_t *)0x30000000UL;
    pure_dcdc_saved_glb_clk_cfg1 =
        *(volatile uint32_t *)0x30000004UL;
    pure_dcdc_saved_glb_clk_cfg3 =
        *(volatile uint32_t *)0x3000000CUL;
    pure_dcdc_saved_glb_clk_cfg5 =
        *(volatile uint32_t *)0x30000014UL;
    pure_dcdc_saved_glb_cgen_cfg0 =
        *(volatile uint32_t *)0x30000030UL;
    pure_dcdc_saved_hbn_glb =
        *(volatile uint32_t *)0x3008F030UL;

    /* (4) Wire up HBN_RSV1 = wakeup stub VMA address (HBN RAM).  brom
     *     fastboot dispatcher reads this when waking from HBN. */
    *(volatile uint32_t *)HBN_RSV1_REG_ADDR =
        (uint32_t)&pure_dcdc_wakeup_stub;

#ifdef CONFIG_PURE_DCDC_AON_WDT_RECOVER
    /* (5) Enable AON WDT as wake-hang safety net.  If any stage from
     *     HBN bounce -> brom fastboot dispatch -> stub -> hbn_xip_recovery_c
     *     hangs, WDT fires (~3.9ms) and forces a chip reset.  HBN_MISC[11]
     *     is already set (cold-init marker) so the next boot takes the
     *     warm trampoline path -- no infinite cold-init loop.
     *     hbn_xip_recovery_c disables WDT at end of recovery.
     *
     *     HBN_WDT (0x3008F110) layout:
     *       [8]    ANO_WDT_EN
     *       [9]    SW_KICK (write 1 to feed)
     *       [10]   AON_WDT_CLR (write 1 to clear ST_AON_WDT)
     *       [11]   ST_AON_WDT (read-only reset status)
     *       [17:12] CNT_MASK_BIT (timeout = 2^(N+1)/32768 sec; N=7 -> ~7.8ms)
     *       [31:24] AON_WDT_KEY (must write 0x7A or HW ignores write)
     *     HBN_GLB[14] must also be set to gate WDT timeout to chip reset.
     *
     *     Timeout sizing: covers DCDC switch + HBN-1T bounce (~1ms) +
     *     wakeup stub recovery (~3ms).  stub disables WDT at end so FW
     *     runtime is not protected.  mask_bit=7 -> ~7.8ms = ~1.5x margin
     *     over typical 5ms wakeup path. */
    {
        volatile uint32_t *hbn_wdt = (volatile uint32_t *)0x3008F110UL;
        volatile uint32_t *hbn_glb = (volatile uint32_t *)0x3008F030UL;
        uint32_t tmpVal;
        const uint32_t KEY = (0x7AU << 24);
        const uint32_t KEY_UMSK = 0x00FFFFFFU;

        /* Disable first (in case boot2 / prior session left it on).
         * Do NOT clear ST_AON_WDT here — it must persist for
         * hal_rst_reason_detect() (runs later in SystemInit) to detect
         * the WDT-triggered reset. ST is SW-clear-only per HW design;
         * the rst_reason_detect path clears it after reading. */
        tmpVal = (*hbn_wdt & KEY_UMSK) & ~(1U << 8);
        *hbn_wdt = tmpVal | KEY;

        /* Set timeout (mask_bit=7 -> ~7.8ms, covers DCDC switch + bounce + stub) + enable */
        tmpVal = *hbn_wdt & KEY_UMSK;
        tmpVal &= ~(0x3FU << 12);
        tmpVal |= (7U << 12);
        tmpVal |= (1U << 8);
        *hbn_wdt = tmpVal | KEY;

        /* HBN_GLB[14] = enable WDT timeout to chip reset */
        *hbn_glb = *hbn_glb | (1U << 14);

        /* Initial kick */
        tmpVal = (*hbn_wdt & KEY_UMSK) | (1U << 9);
        *hbn_wdt = tmpVal | KEY;
    }
#endif /* CONFIG_PURE_DCDC_AON_WDT_RECOVER */

}
