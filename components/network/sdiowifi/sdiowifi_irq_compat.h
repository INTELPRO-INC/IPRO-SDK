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
 * Compat shim. dev_net_sdio (trcver_sdio.c) calls sdiowifi_irq_attach + sdiowifi_irq_enable
 * with handler signature `void (*)(int irq, void *arg)`. ipro_sdk exposes
 * Interrupt_Handler_Register(IRQn_Type, void (*)(void)) + CPU_Interrupt_Enable.
 *
 * The two SDIO handlers (sdu_interrupt_entry / sdu_soft_reset_interrupt_entry)
 * never reference the int/void* args, so casting to the no-arg signature is safe.
 * Same trick used by drv_usb_cherryusb.c (see comment header in that file).
 */
#ifndef SDIOWIFI_COMPAT_IRQ_H
#define SDIOWIFI_COMPAT_IRQ_H

#include "drv_common.h"

extern void Interrupt_Handler_Register(IRQn_Type irq, void (*interruptFun)(void));
extern void CPU_Interrupt_Enable(uint32_t irq_num);
extern void CPU_Interrupt_Disable(uint32_t irq_num);

#define sdiowifi_irq_attach(irq, handler, arg) \
    Interrupt_Handler_Register((IRQn_Type)(irq), (void (*)(void))(handler))
#define sdiowifi_irq_enable(irq)  CPU_Interrupt_Enable((uint32_t)(irq))
#define sdiowifi_irq_disable(irq) CPU_Interrupt_Disable((uint32_t)(irq))

#endif
