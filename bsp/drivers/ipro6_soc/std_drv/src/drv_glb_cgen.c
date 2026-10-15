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

#include "drv_glb.h"

/* In SDK- and ROM-driver images alike. On ROM builds they replace the
 * weak drv_romapi.c wrappers whose ROM code uses another part's bits;
 * the anchor below force-links this file there. */
int __attribute__((used)) __drv_glb_cgen_anchor = 1;

static int GLB_Get_And_Clr_First_Set_From_U64(uint64_t *val, uint32_t *bit)
{
    if (!*val) {
        return ERROR;
    }

    for (uint8_t i = 0; i < 64; i++) {
        if ((*val) & ((uint64_t)1 << i)) {
            *bit = i;
            (*val) &= ~((uint64_t)1 << i);
            break;
        }
    }

    return SUCCESS;
}

/*
 * Peripheral bus-clock gating, per the IPRO6S GLB register document.
 *
 * Every module gate on this part lives in CGEN_CFG0 (0x580) bits 0..26;
 * cgen_cfg1 (0x584) and cgen_cfg2 (0x588) are RESERVED here -- glb_reg.h
 * already declares them as 32 reserved bits. The generic per-IP switch this
 * replaces came from a part with a different layout: it routed most IPs into
 * cfg1/cfg2, where the write did nothing, and the few it did put in cfg0 used
 * the wrong bit order -- "CPU" landed on ocram, "SEC" on gpip, "DMA_0" on
 * sdbg and "CCI" on tzc. A caller asking to gate one module gated another.
 *
 * A table indexed by the IP enum replaces the switch because the mapping IS
 * the data: it can be checked against the document line by line, and an IP
 * this part does not have is simply absent rather than a case that silently
 * writes a reserved register.
 *
 * Stored as bit+1 so that 0 means "not on this part" and the table needs no
 * parallel validity array.
 */
static const uint8_t glb_cgen0_bit1[] = {
    [GLB_AHB_CLOCK_IP_OCRAM]          =  0 + 1,
    [GLB_AHB_CLOCK_IP_WRAM]           =  1 + 1,
    [GLB_AHB_CLOCK_IP_GPIP]           =  2 + 1,
    [GLB_AHB_CLOCK_IP_SDBG]           =  3 + 1,
    [GLB_AHB_CLOCK_IP_TZC]            =  4 + 1,
    [GLB_AHB_CLOCK_IP_CCI]            =  5 + 1,
    [GLB_AHB_CLOCK_IP_UART0]          =  6 + 1,
    [GLB_AHB_CLOCK_IP_UART1]          =  7 + 1,
    [GLB_AHB_CLOCK_IP_EF_CTRL]        =  8 + 1,
    [GLB_AHB_CLOCK_IP_SPI]            =  9 + 1,
    [GLB_AHB_CLOCK_IP_I2C]            = 10 + 1,
    [GLB_AHB_CLOCK_IP_I2C1]           = 11 + 1,
    [GLB_AHB_CLOCK_IP_PWM]            = 12 + 1,
    [GLB_AHB_CLOCK_IP_TIMER]          = 13 + 1,
    [GLB_AHB_CLOCK_IP_CHECKSUM]       = 14 + 1,
    [GLB_AHB_CLOCK_IP_I2S]            = 15 + 1,
    [GLB_AHB_CLOCK_IP_USB11]          = 16 + 1,
    [GLB_AHB_CLOCK_IP_SEC]            = 17 + 1,
    [GLB_AHB_CLOCK_IP_DMA_0]          = 18 + 1,
    [GLB_AHB_CLOCK_IP_SF_CTRL]        = 19 + 1,
    [GLB_AHB_CLOCK_IP_EMAC]           = 20 + 1,
    [GLB_AHB_CLOCK_IP_USB20_SDU]      = 21 + 1,
    [GLB_AHB_CLOCK_IP_BT_BLE_NORMAL]  = 22 + 1,
    [GLB_AHB_CLOCK_IP_RF_TOP]         = 23 + 1,
    [GLB_AHB_CLOCK_IP_MM_MISC]        = 24 + 1,
    [GLB_AHB_CLOCK_IP_DBI]            = 25 + 1,
    [GLB_AHB_CLOCK_IP_SPI2]           = 26 + 1,
};

static int glb_per_clock_set(uint64_t ips, uint8_t ungate)
{
    uint32_t cfg0 = readl(GLB_BASE + GLB_CGEN_CFG0_OFFSET);
    uint32_t bitfield = 0;

    while (ips) {
        if (SUCCESS != GLB_Get_And_Clr_First_Set_From_U64(&ips, &bitfield)) {
            break;              /* never spin: the helper owns the clear */
        }
        if (bitfield >= (sizeof(glb_cgen0_bit1) / sizeof(glb_cgen0_bit1[0]))) {
            continue;
        }
        if (glb_cgen0_bit1[bitfield] == 0) {
            continue;           /* module not present on this part */
        }
        if (ungate) {
            cfg0 |= (uint32_t)1 << (glb_cgen0_bit1[bitfield] - 1u);
        } else {
            cfg0 &= ~((uint32_t)1 << (glb_cgen0_bit1[bitfield] - 1u));
        }
    }
    writel(cfg0, GLB_BASE + GLB_CGEN_CFG0_OFFSET);

    return SUCCESS;
}

int GLB_PER_Clock_Gate(uint64_t ips)
{
    return glb_per_clock_set(ips, 0);
}

int GLB_PER_Clock_UnGate(uint64_t ips)
{
    return glb_per_clock_set(ips, 1);
}

/*
 * Gate every module clock that the platform does not need to run.
 *
 * The kept set is what the SoC itself runs on -- OCRAM/WRAM (code and data),
 * TZC (bus permissions), CCI (the register bridge), the debug UART, EF_CTRL
 * (efuse, read during RF/MAC init), SF_CTRL (XIP flash: gating it stops the
 * next instruction fetch) and RF_TOP (the WiFi RF front end). Everything else
 * is gated here and ungated again by its own module init, so a module that is
 * never initialised never gets a clock.
 *
 * Measured 2026-09-16 on the awake full firmware: 50.2 -> 39.1 mA
 * unassociated and 74.4 -> 66.1 mA associated, with scan, association, ping
 * and TCP throughput unchanged. Note this must run on every full-firmware
 * wake, not only at cold boot: a PDS wake restores CGEN_CFG0 to its reset
 * value (all modules clocked). board_init() is on both paths.
 */
int GLB_PER_Clock_Gate_All(void)
{
    static const uint8_t keep_ip[] = {
        GLB_AHB_CLOCK_IP_OCRAM, GLB_AHB_CLOCK_IP_WRAM, GLB_AHB_CLOCK_IP_TZC,
        GLB_AHB_CLOCK_IP_CCI, GLB_AHB_CLOCK_IP_UART0, GLB_AHB_CLOCK_IP_EF_CTRL,
        GLB_AHB_CLOCK_IP_SF_CTRL, GLB_AHB_CLOCK_IP_RF_TOP,
    };
    uint32_t keep = 0;
    uint32_t i;

    for (i = 0; i < sizeof(keep_ip) / sizeof(keep_ip[0]); i++) {
        keep |= (uint32_t)1 << (glb_cgen0_bit1[keep_ip[i]] - 1u);
    }
    writel(keep, GLB_BASE + GLB_CGEN_CFG0_OFFSET);

    return SUCCESS;
}
