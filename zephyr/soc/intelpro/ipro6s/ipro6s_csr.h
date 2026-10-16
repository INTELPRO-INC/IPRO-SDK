/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * Nuclei N300 vendor CSRs used by the IPRO6S Zephyr port. Values are taken
 * from bsp/drivers/ipro6s_soc/risc-v/Core/Include/riscv_encoding.h; this
 * header exists so assembly and Zephyr C code do not have to pull in the
 * whole NMSIS tree.
 */

#ifndef ZEPHYR_SOC_INTELPRO_IPRO6S_CSR_H_
#define ZEPHYR_SOC_INTELPRO_IPRO6S_CSR_H_

/* Nuclei ECLIC CSRs (IPRO6S_ prefix: the SDK's riscv_encoding.h defines the
 * bare names with a different spelling, which would trip -Wredefined). */
#define IPRO6S_CSR_MTVT            0x307
#define IPRO6S_CSR_MNXTI           0x345
#define IPRO6S_CSR_MINTSTATUS      0x346
#define IPRO6S_CSR_MTVT2           0x7EC

/* Nuclei core control CSRs */
#define IPRO6S_CSR_MCOUNTINHIBIT   0x320
#define IPRO6S_CSR_MILM_CTL        0x7C0
#define IPRO6S_CSR_MDLM_CTL        0x7C1
#define IPRO6S_CSR_MSUBM           0x7C4
#define IPRO6S_CSR_MSTACK_CTRL     0x7C6
#define IPRO6S_CSR_MSTACK_BOUND    0x7C7
#define IPRO6S_CSR_MSTACK_BASE     0x7C8
#define IPRO6S_CSR_MDCAUSE         0x7C9
#define IPRO6S_CSR_MCACHE_CTL      0x7CA
#define IPRO6S_CSR_MMISC_CTL       0x7D0
#define IPRO6S_CSR_MIRGB_INFO      0x7F7
#define IPRO6S_CSR_MCFG_INFO       0xFC2

/* MMISC_CTL bits */
#define IPRO6S_MMISC_CTL_BPU           (1 << 3)
#define IPRO6S_MMISC_CTL_MISALIGN      (1 << 6)
#define IPRO6S_MMISC_CTL_ZC            (1 << 7)
#define IPRO6S_MMISC_CTL_NMI_CAUSE_FFF (1 << 9)

/* MCACHE_CTL bits */
#define IPRO6S_MCACHE_CTL_IC_EN        (1 << 0)
#define IPRO6S_MCACHE_CTL_DC_EN        (1 << 16)

/* MCFG_INFO bits */
#define IPRO6S_MCFG_INFO_CLIC          (1 << 2)
#define IPRO6S_MCFG_INFO_ICACHE        (1 << 9)
#define IPRO6S_MCFG_INFO_DCACHE        (1 << 10)

/* mstatus.FS */
#define IPRO6S_MSTATUS_FS_INITIAL      0x00002000

/*
 * IREGION: the Nuclei "internal region" holding the ECLIC and the private
 * systimer. Its base is read from MIRGB_INFO[31:10] at run time; the SoC
 * never publishes it as a constant.
 */
#define IPRO6S_IREGION_ECLIC_OFS  0x20000
#define IPRO6S_IREGION_TIMER_OFS  0x30000

#endif /* ZEPHYR_SOC_INTELPRO_IPRO6S_CSR_H_ */
