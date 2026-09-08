/*
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/* Please check the TODO items */

#include "riscv_encoding.h"

/* If BOOT_HARTID is not defined, default value is 0 */
    .equ BOOT_HARTID,    0

.macro DECLARE_INT_HANDLER  INT_HDL_NAME
#if defined(__riscv_xlen) && (__riscv_xlen == 32)
    .word \INT_HDL_NAME
#else
    .dword \INT_HDL_NAME
#endif
.endm

    .section .text.init
    .globl _start
    .type _start, @function

/**
 * Reset Handler called on controller reset
 */
_start:
    /* ===== Startup Stage 1 ===== */
    /* Disable Global Interrupt */
    csrc CSR_MSTATUS, MSTATUS_MIE

    /* If SMP_CPU_CNT is not defined,
     * assume that only 1 core is allowed to run,
     * the core hartid is defined via BOOT_HARTID.
     * other harts if run to here, just do wfi in __amp_wait
     */

    /* take bit 0-7 for hart id in a local cluster */
    csrr a0, CSR_MHARTID
    andi a0, a0, 0xFF
    /* BOOT_HARTID is configurable in Makefile via BOOT_HARTID variable */
    li a1, BOOT_HARTID
    bne a0, a1, __amp_wait

    /* Initialize GP and TP and jump table base when zcmt enabled */
    .option push
    .option norelax
    la gp, __global_pointer$
    la tp, __tls_base
#if defined(__riscv_zcmt)
    la t0, __jvt_base$
    csrw CSR_JVT, t0
#endif
    .option pop

    /* Set correct sp for current cpu */
    la sp, _sp

    /*
     * Set the the NMI base mnvec to share
     * with mtvec by setting CSR_MMISC_CTL
     * bit 9 NMI_CAUSE_FFF to 1
     */
    li t0, MMISC_CTL_NMI_CAUSE_FFF
    csrs CSR_MMISC_CTL, t0

    /*
     * Enable Zc feature when compiled zcmp & zcmt
     */
    li t0, MMISC_CTL_ZC
#if defined(__riscv_zcmp) || defined(__riscv_zcmt)
    csrs CSR_MMISC_CTL, t0
#else
    csrc CSR_MMISC_CTL, t0
#endif

    /*
     * Set Exception Entry MTVEC to early_exc_entry
     * Due to settings above, Exception and NMI
     * will share common entry.
     * This early_exc_entry is only used during early
     * boot stage before main
     * Set default interrupt mode to CLINT interrupt mode
     */
    la t0, early_exc_entry
    csrw CSR_MTVEC, t0

    /* unlock opi, set 0x30000070[1] = 0*/
    lui a5,0x30000
    lw  a4,112(a5)
    and a4,a4,-3
    sw  a4,112(a5)

#ifdef CONFIG_AI_SUPPORT
    /* set 0x3000008c[8] = 1 (required for NPU) */
    lw  a4,140(a5)
    ori a4,a4,0x100
    sw  a4,140(a5)
#endif

    /* ===== Startup Stage 2 ===== */

    /* Enable FPU and Vector Unit if f/d/v exist in march */
#if defined(__riscv_flen) && __riscv_flen > 0
    /* Enable FPU, and set state to initial */
    li t0, MSTATUS_FS
    csrc mstatus, t0
    li t0, MSTATUS_FS_INITIAL
    csrs mstatus, t0
#endif

    /* Enable mcycle and minstret counter */
    csrci CSR_MCOUNTINHIBIT, 0x5

    .size _start, . - _start

    /*
     * PDS31 Warm Resume is handled by HBN callback — bootrom jumps
     * to HBN RAM firmware (hal_lp_wakeup_entry) when HBN_MODE is set,
     * completely skipping boot2 and this startup code.
     * No check needed here — normal cold boot path continues below.
     */

#ifdef CONFIG_SUPPORT_PURE_DCDC
/* ===== Pure-DCDC early init =====
 *
 * Topology switch from LDO14-driven VDD14 to DCDC-driven VDD14, then
 * a 1T HBN reboot to settle the rail (matches the C bring-up that
 * previously lived in board_power_init).
 *
 * Marker: HBN_MISC[11] (vdd14_bond_option_enb_aon). Persists across
 * every reset in the AON domain; only POR clears it. After the cold-
 * init sequence sets the bit, the 1T HBN bounce wake re-enters _start
 * and __pure_dcdc_early_init takes the fall-through (warm) path.
 *
 * Constraints: runs from XIP flash BEFORE __init_common's LMA->VMA
 * copy. No SRAM/TCM access, no function calls -- only register MMIO.
 * (HBN_Mode_Enter etc. are ATTR_TCM_SECTION so they aren't reachable
 * from here; the HBN entry sequence is inlined below.)
 *
 * Sequence on cold POR:
 *   1. pu_dcdc_aon = 1, pu_ldo14_aon = 1   (HBN_CTL[23], [15])
 *   2. vdd14_bond_option_enb_aon = 1       (HBN_MISC[11])  -- marker
 *   3. pu_dcdc_aon = 1                     (re-assert, matches C)
 *   4. poll dcdc_rdy_aon                   (AON+0x814[31])
 *   5. pu_ldo14_aon = 0                    (LDO14 off, DCDC owns rail)
 *   6. 1T HBN reboot                       (no-return; chip wakes via
 *                                            RTC compare, cold boots)
 *
 * On wake, _start re-runs and HBN_MISC[11]==1 → fall through. The HBN
 * bounce uses hw_pu_pd_en=1 (same as commit 9f6dc5c7762e's
 * hal_lp_enter_fastboot) so the wake circuit can power the core back
 * up with minimal latency.
 *
 * DCDC voutSel keeps its POR default (0x0B = 1.4V); board_power_init
 * still re-programs it later -- vout setting is fine in that stage.
 */
.type __pure_dcdc_early_init, @function
__pure_dcdc_early_init:
    /* a5 = HBN/AON base = 0x3008F000.
     * Note: RV32I 12-bit immediates only cover -2048..2047, so 0x800
     * and 0x814 must be loaded into a register before AND/OR/LW. */
    li   a5, 0x3008F000
    li   a4, 0x800                  /* mask for bit 11 + bond-option set */

    /* Skip if already initialised (HBN_MISC[11] == 1) */
    lw   t0, 0x2C(a5)               /* HBN_MISC */
    and  t1, t0, a4
    bnez t1, __pure_dcdc_done

    /* Apply RC32K efuse trim BEFORE the DCDC switch. RC32K drives the
     * 32.768 kHz domain (RTC compare, HBN tick, AON WDT). Untrimmed
     * RC32K can be off by 10-20%, skewing the HBN-1T bounce duration
     * and AON WDT timeout margin.  Uses ROM API + inline MMIO so it
     * doesn't depend on TCM-resident drivers (LMA copy hasn't run yet). */
    call pure_dcdc_apply_rc32k_trim

    /* Reload a5/a4 — caller-saved by RV ABI, clobbered by the C call. */
    li   a5, 0x3008F000
    li   a4, 0x800

    /* HBN_CTL |= (1<<23) | (1<<15) -- pu_dcdc_aon + pu_ldo14_aon */
    lw   t0, 0x00(a5)
    li   t1, 0x00808000
    or   t0, t0, t1
    sw   t0, 0x00(a5)

    /* HBN_MISC |= (1<<11) -- bond option enable (= our cold-init marker) */
    lw   t0, 0x2C(a5)
    or   t0, t0, a4
    sw   t0, 0x2C(a5)

    /* HBN_CTL |= (1<<23) -- re-assert pu_dcdc_aon (matches C version) */
    lw   t0, 0x00(a5)
    li   t1, 0x00800000
    or   t0, t0, t1
    sw   t0, 0x00(a5)

    /* Poll AON_DCDC18_TOP_0[31] (dcdc_rdy_aon).
     * Compute target = a5 + 0x800 so the lw offset (0x14) fits in imm12.
     * Bit 31 set → loaded word is negative as signed → exit loop. */
    add  t2, a5, a4                 /* t2 = AON_BASE + 0x800 */
1:  lw   t0, 0x14(t2)               /* @ AON_BASE + 0x814 */
    bgez t0, 1b

    /* HBN_CTL &= ~(1<<15) -- pu_ldo14_aon = 0, DCDC now owns the rail */
    lw   t0, 0x00(a5)
    li   t1, 0x8000
    not  t1, t1                     /* 0xFFFF7FFF */
    and  t0, t0, t1
    sw   t0, 0x00(a5)

    /* Prepare HBN RAM for fastboot wake:
     *   - copy .hbn_code from XIP LMA to HBN RAM VMA (wakeup stub)
     *   - copy flash_cfg from boot2 hand-off (OCRAM) to HBN RAM
     *   - set HBN_RSV1 = wakeup stub address (in HBN RAM) */
    call pure_dcdc_save_state

    /* Re-load a5 = HBN_BASE since the C call may have clobbered it. */
    li   a5, 0x3008F000

    /* Fall through to __hbn_reboot_1t -- does not return. */

/* ===== HBN 1T reboot (no parameter, no return) =====
 *
 * Bounces the chip through HBN for one 32 kHz tick (~30 µs) so the
 * VDD14 rail can settle after the LDO14->DCDC switch, then wakes via
 * the *fastboot* path (bootrom -> user PC, skipping boot2/media_boot).
 *
 * Brom dispatch (ipro_bootrom_hbn_jump_check, called from brom's
 * __init_common BEFORE start_load + main()):
 *   if (HBN_RSV0 == HBN_STATUS_ENTER_FLAG &&
 *       HBN_RSV1 != 0 && HBN_RSV1 != 0xFFFFFFFF) jump to RSV1.
 * RSV1 here points at the pure_dcdc wakeup stub in HBN RAM (set by
 * pure_dcdc_save_state above).  The stub restores PLL/DLL/SF/cache
 * state, then jumps back into XIP _start.
 *
 * Settings:
 *   - RTC compare = current_low + 1   (1T = ~30 µs at 32.768 kHz)
 *   - HBN_LEVEL_0 (core + RTC stay powered)
 *   - HBN_RSV0 = 0x4E424845            (HBN_STATUS_ENTER_FLAG, fastboot
 *                                        gate magic)
 *   - HBN_RSV1 = stub PC               (set by pure_dcdc_save_state)
 *   - LDO11_AON_VOUT_SEL = 6           (HBN_LDO_LEVEL_0P90V — required
 *                                        for wake circuit; default 0xA
 *                                        bricks the chip)
 *
 * Caller must have a5 = HBN_BASE = 0x3008F000.
 * RC32K, F32K_SEL, and HBN_ROOT_CLK_SEL are at their POR defaults
 * (HBN_GLB resetValue 0xA41A0020 has PU_RC32K=1, F32K_SEL=0,
 * HBN_ROOT_CLK_SEL=0), so no HBN_GLB write is needed.
 */
.type __hbn_reboot_1t, @function
__hbn_reboot_1t:
    /* Disable CPU IRQ before reading HBN RTC counter + writing compare.
     * If an ISR fires between latched counter read and compare write,
     * compare becomes stale by ISR duration → in past → HBN entry
     * silent-fails (Mode 1 hang). */
    csrc CSR_MSTATUS, MSTATUS_MIE

    /* (a) RTC compare = current_low + 16.
     *     Latch RTC: HBN_RTC_TIME_H |= (1<<31), then read low. */
    lw   t0, 0x10(a5)               /* HBN_RTC_TIME_H */
    li   t1, 0x80000000             /* RTC_TIME_LATCH bit */
    or   t0, t0, t1
    sw   t0, 0x10(a5)
    lw   t0, 0x0C(a5)               /* HBN_RTC_TIME_L (latched) */
    /* +16 ticks = ~0.488 ms @ 32.768 kHz.
     *
     * Why not +1T?  ~23 instructions (8 HBN-domain MMIO writes ~20µs each
     * cross-domain to 32K clock + 15 reg ops) between this compare write
     * and the final HBN_MODE bit set, totalling ~200µs.  RTC counter
     * advances ~6-7 ticks during that window.  With +1T, compare ends
     * up in the past by HBN_MODE write → HBN entry silently fails
     * (Mode 1 — chip continues past HBN_MODE write, no sleep). */
    li   t1, 0x10
    add  t0, t0, t1
    sw   t0, 0x04(a5)               /* HBN_TIME_L (compare) */
    sw   zero, 0x08(a5)             /* HBN_TIME_H = 0 */

    /* (b) HBN_IRQ_MODE[16] = 1 -- enable HW power up/down sequencer.
     *     Required for the chip to actually enter HBN; without it the
     *     HBN_MODE write below is a no-op and the chip just spins.
     *     Unrelated to brom fastboot dispatch (that's RSV0/RSV1). */
    lw   t0, 0x14(a5)
    li   t1, 0x10000
    or   t0, t0, t1
    sw   t0, 0x14(a5)

    /* (c) HBN_RSV0 = HBN_STATUS_ENTER_FLAG (0x4E424845, "EHBN").
     *     HBN_RSV1 was already written by pure_dcdc_save_state() to
     *     point at the wakeup stub in HBN RAM.  Both regs are in AON
     *     so they survive the bounce.  brom on wake reads RSV0 + RSV1,
     *     dispatches to stub directly (skips boot2). */
    li   t1, 0x100
    add  t2, a5, t1                 /* t2 = HBN_RSV0 (HBN_BASE+0x100) */
    li   t0, 0x4E424845
    sw   t0, 0(t2)

    /* (d) HBN_CTL final-state write — combine all bits in one write to
     *     avoid intermediate in-flight states:
     *       - bit 0  RTC_CTL = 1            (enable RTC counter)
     *       - bit 1  compMode<<1 = 1        (RTC compare mode = 1)
     *       - bit 5  RTC_RESET = 1          (release RTC reset)
     *       - bit 7  HBN_MODE = 1 (LAST)    (do NOT set yet — see below)
     *       - bits[22:19] LDO11_VOUT_SEL = 0xA
     *       - clear bit 9  PWRDN_HBN_CORE   (LEVEL_0)
     *       - clear bit 11 PWRDN_HBN_RTC    (LEVEL_0)
     *       - clear bit 25 PWR_ON_OPTION    (POR-twice for robust)
     *     POR default = 0x00D08000:
     *       - bit 15 PU_LDO14_AON=1 (we cleared above), bit 23 PU_DCDC_AON=1,
     *         LDO11_VOUT_SEL=0xA (matches our setting).
     *     First write WITHOUT HBN_MODE; HBN_MODE is set last after the
     *     IRQ_CLR pulse. */
    lw   t0, 0x00(a5)               /* current HBN_CTL */
    /* Clear LDO11_VOUT_SEL[22:19], PWRDN_CORE[9], PWRDN_RTC[11], PWR_ON_OPT[25] */
    li   t1, 0x02780A00             /* mask: bits 9 + 11 + 19..22 + 25 */
    not  t1, t1
    and  t0, t0, t1
    /* OR in: RTC_CTL[0]=1, compMode[1]=1, RTC_RESET[5]=1,
     *        LDO11_VOUT_SEL=0xA → (0xA<<19) = 0x00500000 */
    li   t1, 0x00500023
    or   t0, t0, t1
    sw   t0, 0x00(a5)

    /* (e) HBN_IRQ_CLR pulse: 0xFFFFFFFF, then 0. */
    li   t0, -1
    sw   t0, 0x1C(a5)
    sw   zero, 0x1C(a5)

    /* === Pre-HBN_MODE additions ====================================
     * 1. GLB+0x1AC: 0xFFF, then 0xFFFFFF
     * 2. HBN_IRQ_MODE[14:4] = 0x7FF (PIN_WAKEUP_MASK)
     * 3. HBN_IRQ_CLR = 0xFFFFFFFF
     */
    /* (1) */
    li   t0, 0x300001AC
    li   t1, 0xFFF
    sw   t1, 0(t0)
    li   t1, 0xFFFFFF
    sw   t1, 0(t0)

    /* (2) HBN_IRQ_MODE[14:4] = 0x7FF */
    lw   t0, 0x14(a5)
    li   t1, 0x7FF0
    or   t0, t0, t1
    sw   t0, 0x14(a5)

    /* (3) HBN_IRQ_CLR = 0xFFFFFFFF */
    li   t0, -1
    sw   t0, 0x1C(a5)

    /* (f) HBN_CTL: set HBN_MODE (bit 7) -- enters HBN; RTC compare
     *     fires after ~30 µs and the chip cold-boots. */
    lw   t0, 0x00(a5)
    ori  t0, t0, 0x80
    sw   t0, 0x00(a5)

    /* Spin from XIP until HBN takes over. Must not reach __init_common. */
2:  j    2b
    .size __hbn_reboot_1t, . - __hbn_reboot_1t

__pure_dcdc_done:
    .size __pure_dcdc_early_init, . - __pure_dcdc_early_init
#endif /* CONFIG_SUPPORT_PURE_DCDC */

/* ===== Clock boost to PLL 192M before LMA->VMA copy =====
 *
 * Calls the XIP C helper clock_boost_init() (clock_boost.c). The helper
 * either:
 *   - just switches root clock + dividers when boot2 already powered
 *     up PLL/DLL (normal cold POR), or
 *   - performs the full PLL+DLL bring-up first (fastboot HBN wake,
 *     where boot2 was skipped — see commit 9f6dc5c7762e).
 *
 * Result: HCLK = PLL 192M, BCLK = HCLK/2 = 96M for the LMA copy that
 * __init_common is about to run.
 *
 * The C helper is in section .text.init (XIP) so no LMA copy is
 * needed before this call. It uses no globals, no SRAM/TCM data.
 */
    call clock_boost_init

.type __init_common, @function
__init_common:
    /* ===== Startup Stage 3 ===== */

    /*
     * Save boot2 parameters BEFORE LMA to VMA copy
     * Boot2 passes parameters in OCRAM which may be overwritten
     * during the data section copy below.
     */
    call Boot2_Param_Save

    /* All LMA->VMA section copies + BSS clears + (when CONFIG_USE_PSRAM)
     * psram_early_hw_init are done by early_section_copy() in
     * bsp/drivers/ipro7_soc/startup/section_copy.c.  Replaces the
     * sequential CPU memcpy loops that used to live here. */
    call early_section_copy

    .size __init_common, . - __init_common

.globl _start_premain
.type _start_premain, @function
_start_premain:
    /*
     * Call vendor defined SystemInit to
     * initialize the micro-controller system
     * SystemInit will just be called by boot cpu
     */
    call SystemInit

    /*
     * Call C/C++ constructor start up code,
     * __libc_fini is defined in linker script,
     * so register_fini function will be called
     * and will run atexit (__libc_fini_array)
     * to do previous call atexit function
     */
    call __libc_init_array

    .size _start_premain, . - _start_premain

.type __skip_init, @function
__skip_init:

    /* do pre-init steps before main */
    /* _premain_init will be called by each cpu
     * please make sure the implementation of __premain_int
     * considered this
     * it will update mtvec according to eclic present or not
     * mtvec will set to exc_entry
     */
    call _premain_init

    /* BPU cold bringup need time, so enable BPU before enter to main */
    li t0, MMISC_CTL_BPU
    csrs CSR_MMISC_CTL, t0

    // Interrupt is still disabled here
    /* ===== Call SMP Main Function  ===== */
    /* argc = argv = 0 */
    li a0, 0
    li a1, 0

    call main

    /* do post-main steps after main
     * this function will be called by each cpu */
    call _postmain_fini

    .size __skip_init, . - __skip_init

.type __amp_wait, @function
__amp_wait:
1:
    wfi
    j 1b

    .size __amp_wait, . - __amp_wait

/* Early boot exception entry before main */
.align 6
.global early_exc_entry
.type early_exc_entry, @function
early_exc_entry:
    wfi
    j early_exc_entry

    .size early_exc_entry, . - early_exc_entry
