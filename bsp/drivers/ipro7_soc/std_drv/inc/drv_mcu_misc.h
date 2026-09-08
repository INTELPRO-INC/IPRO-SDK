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

#ifndef __DRV_MCU_MISC_H__
#define __DRV_MCU_MISC_H__

#include "mcu_misc_reg.h"
#include "drv_common.h"

enum {
    ZSP_IRQ_USR_0 = 0,
    ZSP_IRQ_USR_1,
    ZSP_IRQ_USR_2,
    ZSP_IRQ_USR_3,
    ZSP_IRQ_USR_4,
    ZSP_IRQ_USR_7,
    ZSP_IRQ_USR_8,
    ZSP_IRQ_USR_9,
    ZSP_IRQ_USR_10,
    ZSP_IRQ_USR_11,
    ZSP_IRQ_USR_12,
    //ZSP_IRQ_USR_13, // USR_13 is used by freertos as yield
    ZSP_IRQ_USR_MAX = ZSP_IRQ_USR_12,
};

void DRV_IRQMapConfig(void);
void DRV_IRQMapMaskSet(uint8_t irqNum, uint8_t mask);
void DRV_IRQMapInit(uint8_t *table, uint8_t size);
void DRV_ClearIRQ(uint8_t irqNum);
int DRV_IRQMapCheckRaised(uint8_t irqNum);

#endif /* __DRV_MCU_MISC_H__ */
