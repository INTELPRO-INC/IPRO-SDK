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
 * IPRO6 SMID (SDIO 3.0 device) low-level port.
 *
 * Replaces the legacy SDU bitmap-mailbox sdio_port.c for IPRO6. Provides:
 *   - the shared SMID device handle (CSR base + ECLIC IRQ),
 *   - HW clock / controller-reset / pinmux helpers, and
 *   - the scratch-register accessors mapped onto the SMID custom-register
 *     indirect window.
 *
 * Scratch contract (host <-> device): the legacy SDU exposed a directly-mapped
 * scratch region (BL_FUNC_SCRATCH_BASE = SDU_BASE + 0x160). SMID has no such
 * region; the equivalent is the custom-register indirect RAM (address @0x48 /
 * data @0x4C), driven by ipro6_sdio3_custom_reg_{read,write}(). We place the
 * scratch bytes in the global block 0 (custom-reg offset 0x00-0x3F) which the
 * driver's per-function queue layout (func1 block @0x40) never touches. Scratch
 * offset N maps 1:1 to custom-reg offset N. The (patched) host reads/writes the
 * same offsets through the indirect window.
 */

#include <stdint.h>

#include "drv_glb.h"
#include "drv_gpio.h"
#include "drv_glb_gpio.h"
#include "ipro6_sdio3.h"

#include "sdio_port.h"
#include "sdio_port_ipro6.h"

/* SMID device handle: CSR base + ECLIC IRQ. Shared with trcver_sdio_ipro6.c. */
static ipro6_smid_dev_t g_smid_dev = {
    .reg_base = SMID_BASE,
    .irq_num  = SMID_IRQn,
};

ipro6_smid_dev_t *sdio_smid_dev(void)
{
    return &g_smid_dev;
}

/* SMID controller reset — same AHB MCU SW-reset bit as the ipro6le SDU. */
void sdio_glb_reset(void)
{
    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_SDU);
}

/* SDIO-device pinmux: CLK/CMD/DAT0-3 on GPIO10-15 @ AF=GPIO_FUN_SDIO, plus
 * un-gate the SDU/SMID AHB clock. Same pad wiring as the ipro6le SDIO path. */
void sdio_gpio_init(void)
{
    GLB_GPIO_Cfg_Type gpio_cfg;

    gpio_cfg.drive    = 1;
    gpio_cfg.smtCtrl  = 1;
    gpio_cfg.gpioMode = GPIO_MODE_AF;
    gpio_cfg.pullType = GPIO_PULL_UP;

    for (int i = 10; i < 16; i++) {
        gpio_cfg.gpioPin = i;
        gpio_cfg.gpioFun = GPIO_FUN_SDIO;
        GLB_GPIO_Init(&gpio_cfg);
    }
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_USB20_SDU);
}

/* ----- scratch registers via the SMID custom-register indirect window ----- */

/* Base of the scratch block inside the custom-register window. Block 0
 * (0x00-0x3F) is free: the driver only writes function blocks (func1 @0x40). */
#define SMID_SCRATCH_BASE   (0x00)

uint8_t sdu_read_s_reg(uint32_t offset)
{
    uint8_t v = 0;
    ipro6_sdio3_custom_reg_read(&g_smid_dev,
                                (uint16_t)(SMID_SCRATCH_BASE + offset), &v, 1);
    return v;
}

void sdu_write_s_reg(uint32_t offset, uint8_t value)
{
    ipro6_sdio3_custom_reg_write(&g_smid_dev,
                                 (uint16_t)(SMID_SCRATCH_BASE + offset), &value, 1);
}
