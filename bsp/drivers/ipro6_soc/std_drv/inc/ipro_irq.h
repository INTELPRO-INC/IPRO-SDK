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

#ifndef __IPRO_IRQ_H__
#define __IPRO_IRQ_H__

/*
 * IPRO IRQ shim for ported SDIO-device drivers on the Nuclei ECLIC core.
 *
 * Maps the legacy irq-attach API onto the IPRO SDK's
 * Interrupt_Handler_Register / CPU_Interrupt_{Enable,Disable}, and provides
 * MIE save/restore critical sections + arch_memset.
 *
 * IMPORTANT: handlers registered via ipro_irq_attach() MUST be zero-arg
 * `void (*)(void)` -- the registration DROPS the legacy (int irq, void *arg)
 * parameters (ECLIC dispatch passes none). A driver that needs a context
 * pointer must recover it from a file-static, NOT from a handler argument.
 */

/* drv_common.h pulls system_soc.h (Interrupt_Handler_Register / CPU_Interrupt_*
 * macros), misc.h (arch_memset), and the Nuclei core CSR helpers. */
#include "drv_common.h"

/* arg is intentionally dropped: the ECLIC handler is void(*)(void). */
#define ipro_irq_attach(irq, handler, arg) \
    Interrupt_Handler_Register((IRQn_Type)(irq), (void (*)(void))(handler))
#define ipro_irq_enable(irq)   CPU_Interrupt_Enable((uint32_t)(irq))
#define ipro_irq_disable(irq)  CPU_Interrupt_Disable((uint32_t)(irq))

/* Short critical section: atomically clear MIE, returning the prior mstatus. */
static inline uintptr_t ipro_irq_save(void)
{
    return (uintptr_t)__RV_CSR_READ_CLEAR(CSR_MSTATUS, MSTATUS_MIE);
}

static inline void ipro_irq_restore(uintptr_t flags)
{
    __RV_CSR_WRITE(CSR_MSTATUS, (unsigned long)flags);
}

#endif /* __IPRO_IRQ_H__ */
