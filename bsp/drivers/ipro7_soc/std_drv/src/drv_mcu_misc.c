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

#include "drv_mcu_misc.h"
#if 0
static uint8_t* gDrvIRQMapGroups = NULL;
static uint8_t gDrvIRQMapGroupsSize = 0;

void DRV_IRQMapConfig(void)
{
    uint32_t addr = 0;
    uint32_t val = 0;
    if (gDrvIRQMapGroups == NULL || gDrvIRQMapGroupsSize == 0)
        return;

    for (int irq_num = 0; irq_num < gDrvIRQMapGroupsSize; irq_num += 8) {
        addr = MCU_MISC_BASE + MCU_MISC_ZSP_IRQ_MAP_0_OFFSET + (irq_num / 8) * 4;
        // assume irq map is 8 align
        val = (gDrvIRQMapGroups[irq_num])
               | (gDrvIRQMapGroups[irq_num+1] << 4)
               | (gDrvIRQMapGroups[irq_num+2] << 8)
               | (gDrvIRQMapGroups[irq_num+3] << 12)
               | (gDrvIRQMapGroups[irq_num+4] << 16)
               | (gDrvIRQMapGroups[irq_num+5] << 20)
               | (gDrvIRQMapGroups[irq_num+6] << 24)
               | (gDrvIRQMapGroups[irq_num+7] << 28);
        writel(val, addr);
        DRV_IRQMapMaskSet(irq_num, 1);
    }
}

void DRV_IRQMapMaskSet(uint8_t irqNum, uint8_t mask)
{
    uint32_t val = 0;

    if (irqNum < 32)
        val = readl(MCU_MISC_BASE + MCU_MISC_ZSP_IRQ_MASK_L_OFFSET);
    else
        val = readl(MCU_MISC_BASE + MCU_MISC_ZSP_IRQ_MASK_H_OFFSET);

    if (mask)
        val |= (1 << (irqNum % 32));
    else
        val &= ~(1 << (irqNum % 32));

    if (irqNum < 32)
        writel(val, MCU_MISC_BASE + MCU_MISC_ZSP_IRQ_MASK_L_OFFSET);
    else    
        writel(val, MCU_MISC_BASE + MCU_MISC_ZSP_IRQ_MASK_H_OFFSET);
}


void DRV_IRQMapInit(uint8_t *table, uint8_t size)
{
    gDrvIRQMapGroups = table;
    gDrvIRQMapGroupsSize = size;
    DRV_IRQMapConfig();
}

int DRV_IRQMapCheckRaised(uint8_t irqNum)
{
    uint32_t addr = MCU_MISC_BASE + MCU_MISC_ST_ZSP_IRQ00_L_OFFSET + gDrvIRQMapGroups[irqNum]*8;
    if (irqNum >= 32) {
        irqNum -= 32;
        addr += 4;
    }
    uint32_t val = readl(addr);
    return ((val & (1 << irqNum)) > 0) ? 1 : 0;
}

void DRV_ClearIRQ(uint8_t irqNum)
{
    uint32_t val = readl(MCU_MISC_BASE + MCU_MISC_ZSP_IRQ_CLEAR_OFFSET);
    writel(1 << gDrvIRQMapGroups[irqNum], MCU_MISC_BASE + MCU_MISC_ZSP_IRQ_CLEAR_OFFSET);
    writel(val, MCU_MISC_BASE + MCU_MISC_ZSP_IRQ_CLEAR_OFFSET);
}
#endif
