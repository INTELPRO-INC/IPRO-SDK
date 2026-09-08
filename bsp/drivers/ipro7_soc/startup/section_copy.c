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
 * section_copy.c - LMA->VMA copy + BSS clear + PSRAM hand-off for
 *                  __init_common.
 *
 * Replaces the inline CPU memcpy loops that used to live in startup.S.
 * Architectural benefits:
 *   - One place to manage section list (linker symbols)
 *   - Skips empty sections (VMA == LMA) cleanly
 *   - Single cache-coherency point at end (WBINVAL all + fence.i)
 *
 * Note: an earlier revision experimented with DMA-driven copies (see
 * git history of dma_section_copy.c).  Multi-channel parallel runs hung
 * in classic boot mode for reasons not root-caused, while single-channel
 * gave no measurable speed-up over CPU memcpy.  All DMA scaffolding has
 * been removed.
 *
 * Constraints:
 *   - Runs from XIP flash (.text.init), pre-LMA-copy, pre-bss-clear
 *   - No driver calls (driver .text not yet copied to OCRAM)
 *   - Stack only (no .data/.bss access)
 *
 * Sections handled (lengths are NOT guaranteed 4-byte aligned -
 * copy_section/clear_section handle the tail explicitly):
 *   .tcmcode      _tcmtext_lma     -> _tcmtext
 *   .tcmdata      _tcmdata_lma     -> _tcmdata
 *   .em_tcm_ret   _em_tcm_ret_lma  -> _em_tcm_ret    (~14 KB)
 *   .data_ocram   _data_ocram_lma  -> _data_ocram_vma
 *   bss_ocram     -                -> _bss_ocram_*
 *   .data         _data_lma        -> _data          (post-PSRAM)
 *   .text_psram   _text_psram_lma  -> _text_psram_vma
 *   .data_psram   _data_psram_lma  -> _data_psram_vma
 *   bss_psram     -                -> _bss_psram_*
 *   .bss          -                -> __bss_start.._end
 */

#include <stdint.h>
#include <generated/autoconf.h>
#include "system_soc.h"  /* L1C_DCACHE_CLEAN -> MFlushDCache (CCM_DC_WB_ALL) */

#define REG32(a)  (*(volatile uint32_t *)(uintptr_t)(a))

/* Linker symbols. Declared as char[] on purpose, NOT uint32_t[].
 *
 * A uint32_t[] declaration asserts 4-byte alignment, and GCC believes it:
 * -fipa-cp propagates that alignment from these call sites into
 * copy_section(), concludes (vma_end - vma) & 3 is always zero, and deletes
 * the tail handling below as dead code. The assertion is false - _tcmtext_end
 * lands wherever the last function in .tcmcode ends, and RISC-V compressed
 * instructions are 2 bytes wide, so a 4n+2 end address is routine. char[]
 * carries no alignment promise, which is the truth about a linker symbol. */
extern char _tcmtext_lma[], _tcmtext[], _tcmtext_end[];
extern char _tcmdata_lma[], _tcmdata[], _tcmdata_end[];
extern char _em_tcm_ret_lma[], _em_tcm_ret[], __em_tcm_ret_end[];
extern char _data_ocram_lma[], _data_ocram_vma[], _edata_ocram[];
extern char _bss_ocram_start[], _bss_ocram_end[];
/* Weak — only defined when CONFIG_OCRAM_NORET_SPLIT linker carves out
 * the second retention region in the last 16 KB of OCRAM. */
extern char _bss_ocram_high_start[] __attribute__((weak));
extern char _bss_ocram_high_end[]   __attribute__((weak));
extern char _data_lma[], _data[], _edata[];
extern char __bss_start[], _end[];
extern char _bss_psram_start[], _bss_psram_end[];
#if defined(CONFIG_USE_PSRAM)
extern char _text_psram_lma[], _text_psram_vma[], _etext_psram[];
extern char _data_psram_lma[], _data_psram_vma[], _edata_psram[];
extern uint32_t psram_early_hw_init(void);

/* DQS window measured by psram_early_hw_init on THIS chip, packed
 * [7:4]=left [3:0]=right — the same encoding hal_psram_x8_calibration
 * produces and hal_lp_psram_restore consumes. 0 = not measured (boot2
 * hand-off path), in which case the efuse trim is the only source.
 *
 * Without this, the LP wake path would restore PSRAM using the raw efuse
 * value; on parts with an unprogrammed psram_trim that is 0xffff, which
 * decodes to c_val = 15 — the far end of the delay table — and PSRAM comes
 * back mistimed, hanging the first post-wake PSRAM access.
 *
 * Lives in .ocram_bss, which Phase A already cleared, so it is safe to
 * write here and it survives PDS retention. */
__attribute__((section(".ocram_bss")))
volatile uint16_t g_psram_early_dqs_cfg;

/* PSRAM ready magic in GLB_RSV3 (matches startup.S existing check) */
#define GLB_RSV3          0x3000007CUL
#define PSRAM_READY_MAGIC 0x50535249U   /* "IPSR" */
#endif

/* Copy [lma, lma + (vma_end - vma)) to [vma, vma_end).  Skip if VMA==LMA
 * or if the destination range is empty. */
static void copy_section(void *lma, void *vma, void *vma_end)
{
    uintptr_t s = (uintptr_t)lma;
    uintptr_t d = (uintptr_t)vma;
    uintptr_t e = (uintptr_t)vma_end;
    if (s == d || d >= e)
        return;
    volatile uint32_t *src = (volatile uint32_t *)s;
    volatile uint32_t *dst = (volatile uint32_t *)d;
    uint32_t len   = (uint32_t)(e - d);
    uint32_t words = len >> 2;
    for (uint32_t i = 0; i < words; i++)
        dst[i] = src[i];

    /* Tail. The section list above is documented as 4-byte aligned, but that
     * is a property of whatever happens to land last, not something the linker
     * scripts enforce: .tcmcode ends wherever the final function ends, and a
     * compressed RISC-V instruction is 2 bytes. A length of 4n+2 used to drop
     * the last halfword, which for .tcmcode means the trailing `ret` of the
     * last function is never copied - the CPU then executes uninitialised
     * OCRAM and takes an Illegal Instruction at an address that disassembles,
     * in the ELF, to a perfectly valid `ret`. */
    if (len & 3u) {
        volatile uint8_t *bsrc = (volatile uint8_t *)s;
        volatile uint8_t *bdst = (volatile uint8_t *)d;
        for (uint32_t i = words << 2; i < len; i++)
            bdst[i] = bsrc[i];
    }
}

/* Zero-fill [vma, vma_end). */
static void clear_section(void *vma, void *vma_end)
{
    uintptr_t d = (uintptr_t)vma;
    uintptr_t e = (uintptr_t)vma_end;
    if (d >= e)
        return;
    volatile uint32_t *dst = (volatile uint32_t *)d;
    uint32_t len   = (uint32_t)(e - d);
    uint32_t words = len >> 2;
    for (uint32_t i = 0; i < words; i++)
        dst[i] = 0;

    /* Same truncation as copy_section(): a bss whose length is not a multiple
     * of 4 would leave its last bytes holding whatever was in RAM. */
    if (len & 3u) {
        volatile uint8_t *bdst = (volatile uint8_t *)d;
        for (uint32_t i = words << 2; i < len; i++)
            bdst[i] = 0u;
    }
}

/* Public entry: replaces __init_common LMA copy + bss clear loops. */
void early_section_copy(void)
{
    /* Phase A: OCRAM section copies + bss clear. */
    copy_section(_tcmtext_lma,    _tcmtext,         _tcmtext_end);
    copy_section(_tcmdata_lma,    _tcmdata,         _tcmdata_end);
    copy_section(_em_tcm_ret_lma, _em_tcm_ret,      __em_tcm_ret_end);
    copy_section(_data_ocram_lma, _data_ocram_vma,  _edata_ocram);
    clear_section(                _bss_ocram_start, _bss_ocram_end);
    if (_bss_ocram_high_start && _bss_ocram_high_end) {
        clear_section(_bss_ocram_high_start, _bss_ocram_high_end);
    }

#if defined(CONFIG_USE_PSRAM)
    /* Phase B: PSRAM HW init.  Skip if boot2 already set the magic
     * (classic boot path); otherwise run psram_early_hw_init. */
    int psram_inited;
    if (REG32(GLB_RSV3) == PSRAM_READY_MAGIC) {
        REG32(GLB_RSV3) = 0;          /* consume magic */
        psram_inited = 1;
    } else {
        uint32_t dqs = psram_early_hw_init();
        g_psram_early_dqs_cfg = (uint16_t)(dqs & 0xFF);
        psram_inited = (dqs != 0);
    }
    if (!psram_inited) {
        for (;;) {} /* match __psram_not_ready: spin forever */
    }

    /* Phase C: PSRAM section copies + bss clears. */
    copy_section(_data_lma,       _data,            _edata);
    copy_section(_text_psram_lma, _text_psram_vma,  _etext_psram);
    copy_section(_data_psram_lma, _data_psram_vma,  _edata_psram);
    clear_section(                _bss_psram_start, _bss_psram_end);
    if ((uintptr_t)__bss_start != (uintptr_t)_bss_psram_start ||
        (uintptr_t)_end        != (uintptr_t)_bss_psram_end) {
        clear_section(__bss_start, _end);
    }
#else
    /* No PSRAM: copy .data to OCRAM, clear .bss and .bss_psram */
    copy_section(_data_lma, _data, _edata);
    clear_section(__bss_start, _end);
    clear_section(_bss_psram_start, _bss_psram_end);
#endif

    /* Section copies done.
     * (1) Flush D-cache so dirty lines from CPU memcpy land in physical
     *     memory; required for I-cache fetches of newly-loaded
     *     .text_psram / .tcmcode to see the right bytes.  WB_ALL keeps
     *     lines valid (vs WBINVAL) so subsequent CPU reads still hit.
     * (2) fence.i so subsequent instruction fetches reload I-cache
     *     from the freshly-flushed memory. */
    L1C_DCACHE_CLEAN();
    __asm__ volatile ("fence" ::: "memory");
    __asm__ volatile ("fence.i" ::: "memory");
}
