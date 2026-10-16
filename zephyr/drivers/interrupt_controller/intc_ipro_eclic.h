/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * Nuclei ECLIC memory map (NMSIS core_feature_eclic.h) and CSRs.
 */

#ifndef ZEPHYR_DRIVERS_INTERRUPT_CONTROLLER_INTC_IPRO_ECLIC_H_
#define ZEPHYR_DRIVERS_INTERRUPT_CONTROLLER_INTC_IPRO_ECLIC_H_

#define CSR_MTVT   0x307
#define CSR_MNXTI  0x345

#ifndef _ASMLANGUAGE

/* Global registers */
#define ECLIC_CFG_OFS        0x0000 /* 8-bit:  [4:1] nlbits, [6:5] nmbits */
#define ECLIC_INFO_OFS       0x0004 /* 32-bit: [12:0] numint, [20:13] version, [24:21] intctlbits */
#define ECLIC_MTH_OFS        0x000B /* 8-bit:  machine interrupt level threshold */

/* Per-interrupt 4-byte control words at 0x1000 + 4*irq */
#define ECLIC_INT_OFS(irq)   (0x1000 + 4 * (irq))
#define ECLIC_INTIP_OFS(irq)   (ECLIC_INT_OFS(irq) + 0) /* pending */
#define ECLIC_INTIE_OFS(irq)   (ECLIC_INT_OFS(irq) + 1) /* enable */
#define ECLIC_INTATTR_OFS(irq) (ECLIC_INT_OFS(irq) + 2) /* [0] shv, [2:1] trig, [7:6] mode */
#define ECLIC_INTCTL_OFS(irq)  (ECLIC_INT_OFS(irq) + 3) /* level / priority */

#define ECLIC_CFG_NLBITS_POS   1
#define ECLIC_CFG_NLBITS_MASK  (0xF << ECLIC_CFG_NLBITS_POS)
#define ECLIC_INFO_INTCTLBITS_POS 21
#define ECLIC_INFO_INTCTLBITS_MASK (0xF << ECLIC_INFO_INTCTLBITS_POS)
#define ECLIC_INFO_NUMINT_MASK 0x1FFF

#define ECLIC_INTATTR_SHV      (1 << 0)
#define ECLIC_INTATTR_TRIG_POS 1
#define ECLIC_INTATTR_MODE_M   (3 << 6)

#endif /* !_ASMLANGUAGE */

#endif /* ZEPHYR_DRIVERS_INTERRUPT_CONTROLLER_INTC_IPRO_ECLIC_H_ */
