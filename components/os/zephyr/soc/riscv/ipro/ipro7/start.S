/*
 * Copyright (c) 2025 IPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * IPRO7 SOC-specific startup code for Zephyr
 * This replicates the critical early initialization from FreeRTOS SDK startup_ipro7.S
 */

#include <zephyr/toolchain.h>
#include <zephyr/linker/sections.h>

/* =========== IPRO7 CSR Definitions =========== */
/* These must match the FreeRTOS SDK definitions */
#define CSR_MMISC_CTL       0x7D0
#define CSR_MCACHE_CTL      0x7CA
#define CSR_MCFG_INFO       0xFC2
#define CSR_MIRGB_INFO      0x7F7
#define CSR_MSTATUS         0x300
#define CSR_MTVEC           0x305
#define CSR_MHARTID         0xF14
#define CSR_MIE             0x304
#define CSR_MCOUNTINHIBIT   0x320

/* mmisc_ctl bits */
#define MMISC_CTL_NMI_CAUSE_FFF  0x200  /* NMI base mnvec share with mtvec */
#define MMISC_CTL_ZC             0x80   /* Zc feature enable */
#define MMISC_CTL_BPU            0x8    /* BPU enable */

/* mstatus bits */
#define MSTATUS_MIE              0x8    /* Machine Interrupt Enable */
#define MSTATUS_FS_MASK          0x6000 /* FPU State mask */
#define MSTATUS_FS_INITIAL       0x2000 /* FPU State Initial */

/* mcfg_info bits */
#define MCFG_INFO_ICACHE         0x200  /* I-Cache present */
#define MCFG_INFO_DCACHE         0x400  /* D-Cache present */

/* mcache_ctl bits */
#define MCACHE_CTL_IC_EN         0x1    /* I-Cache enable */
#define MCACHE_CTL_DC_EN         0x10000 /* D-Cache enable */

/* Boot hart ID - which CPU core to boot */
#define BOOT_HARTID              0

/* exports */
GTEXT(__reset)
GTEXT(__initialize)
GTEXT(early_exc_entry)

/* imports */
GTEXT(_PrepC)
GTEXT(z_interrupt_stacks)

/*
 * Reset vector - entry point at 0x13000000
 * This is placed in .reset section by linker script
 */
SECTION_FUNC(reset, __reset)
    /* Jump to initialization code */
    j __initialize

/*
 * IPRO7 SOC Initialization
 * This replicates the FreeRTOS SDK startup sequence
 */
SECTION_FUNC(TEXT, __initialize)
    /* ===== Stage 1: Disable interrupts and check hart ID ===== */
    
    /* Disable global interrupts immediately */
    csrc CSR_MSTATUS, MSTATUS_MIE
    
    /* Check hart ID - only boot hart 0 */
    csrr a0, CSR_MHARTID
    andi a0, a0, 0xFF
    li a1, BOOT_HARTID
    bne a0, a1, __amp_wait
    
    /* ===== Stage 2: Set up global pointer and stack ===== */
    .option push
    .option norelax
    la gp, __global_pointer$
    .option pop
    
    /* Set stack pointer to interrupt stack */
    la sp, z_interrupt_stacks
    li t0, CONFIG_ISR_STACK_SIZE
    add sp, sp, t0
    
    /* ===== Stage 3: IPRO7-specific CSR initialization ===== */
    
    /*
     * Set the NMI base mnvec to share with mtvec
     * by setting CSR_MMISC_CTL bit 9 (NMI_CAUSE_FFF) to 1
     */
    li t0, MMISC_CTL_NMI_CAUSE_FFF
    csrs CSR_MMISC_CTL, t0
    
    /*
     * Disable Zc feature (not using zcmp/zcmt)
     * Clear bit 7 of mmisc_ctl
     */
    li t0, MMISC_CTL_ZC
    csrc CSR_MMISC_CTL, t0
    
    /*
     * Set Exception Entry MTVEC to early_exc_entry
     * This is critical for catching early exceptions during boot
     */
    la t0, early_exc_entry
    csrw CSR_MTVEC, t0
    
    /*
     * Unlock OPI - set 0x30000070[1] = 0
     * This is required for proper memory access
     */
    lui a5, 0x30000
    lw a4, 112(a5)
    andi a4, a4, -3
    sw a4, 112(a5)
    
    /* ===== Stage 4: FPU initialization ===== */
#ifdef CONFIG_FPU
    /* Enable FPU and set state to initial */
    li t0, MSTATUS_FS_MASK
    csrc mstatus, t0
    li t0, MSTATUS_FS_INITIAL
    csrs mstatus, t0
    
    /* Clear FPU CSR */
    fscsr zero
#endif
    
    /* Enable mcycle and minstret counter */
    li t0, 0x5
    csrc CSR_MCOUNTINHIBIT, t0
    
    /* ===== Stage 5: Enable caches ===== */
    /* 
     * Enable I-Cache if present (check mcfg_info bit 9)
     * This matches _premain_init behavior
     */
    csrr a5, CSR_MCFG_INFO
    andi a4, a5, MCFG_INFO_ICACHE
    beqz a4, skip_icache
    li t0, MCACHE_CTL_IC_EN
    csrs CSR_MCACHE_CTL, t0
skip_icache:
    
    /* Memory barrier after cache enable */
    fence
    fence.i
    
    /* ===== Stage 6: Enable BPU (Branch Prediction Unit) ===== */
    li t0, MMISC_CTL_BPU
    csrs CSR_MMISC_CTL, t0
    
    /* ===== Stage 7: Jump to Zephyr C initialization ===== */
    /*
     * _PrepC will:
     * - Zero BSS section
     * - Copy data section
     * - Call z_cstart (kernel entry point)
     */
    call _PrepC

/*
 * AMP wait - other harts wait here
 */
__amp_wait:
1:
    wfi
    j 1b

/*
 * Early exception entry - catch exceptions during boot
 * Just loops forever on exception (easier to debug with JTAG)
 */
.align 6
.global early_exc_entry
.type early_exc_entry, @function
early_exc_entry:
    wfi
    j early_exc_entry
    .size early_exc_entry, . - early_exc_entry
