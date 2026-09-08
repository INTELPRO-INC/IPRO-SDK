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

#include "hal_qdec.h"
#include "qdec_reg.h"
#include "drv_qdec.h"
#include "drv_gpio.h"
#include "drv_glb.h"

static qdec_device_t qdec_devices[QDEC_MAX_INDEX] = {
    QDEC0_CONFIG,
    QDEC1_CONFIG,
    QDEC2_CONFIG,
};
static void QDEC0_IRQ(void);
static void QDEC1_IRQ(void);
static void QDEC2_IRQ(void);

qdec_device_t* dqec_getDevice(int index)
{
    return &qdec_devices[index];
}

int qdec_init(int qdec_id, uint16_t caps)
{
    qdec_device_t *qdec_device = &qdec_devices[qdec_id];
    QDEC_CFG_Type qdec_cfg = { 0 };

    qdec_device->caps = caps;

    QDEC_DeInit(qdec_device->id);

    qdec_cfg.sampleCfg.sampleMod = qdec_device->sample_mode;
    qdec_cfg.sampleCfg.samplePeriod = qdec_device->sample_period;
    qdec_cfg.reportCfg.reportMod = qdec_device->report_mode;
    qdec_cfg.reportCfg.reportPeriod = qdec_device->report_period;
    qdec_cfg.ledCfg.ledEn = qdec_device->led_en;
    qdec_cfg.ledCfg.ledSwap = qdec_device->led_swap;
    qdec_cfg.ledCfg.ledPeriod = qdec_device->led_period;
    qdec_cfg.deglitchCfg.deglitchEn = qdec_device->deglitch_en;
    qdec_cfg.deglitchCfg.deglitchStrength = qdec_device->deglitch_strength;
    qdec_cfg.accMod = qdec_device->acc_mode;

    QDEC_Init(qdec_device->id, &qdec_cfg);

    if (caps & COMMON_CAPS_INT_RX) {
        if (qdec_device->id == QDEC0_ID)
            Interrupt_Handler_Register(QDEC0_IRQn, QDEC0_IRQ);

        if (qdec_device->id == QDEC1_ID) {
            Interrupt_Handler_Register(QDEC1_IRQn, QDEC1_IRQ);
            QDEC_SetIntMask(qdec_device->id, QDEC_INT_REPORT, MASK);
            QDEC_SetIntMask(qdec_device->id, QDEC_INT_SAMPLE, MASK);
            QDEC_SetIntMask(qdec_device->id, QDEC_INT_ERROR, MASK);
            QDEC_SetIntMask(qdec_device->id, QDEC_INT_OVERFLOW, MASK);
            // CPU_Interrupt_Enable(QDEC1_IRQn);
        }

        if (qdec_device->id == QDEC2_ID) {
            Interrupt_Handler_Register(QDEC2_IRQn, QDEC2_IRQ);
            QDEC_SetIntMask(qdec_device->id, QDEC_INT_REPORT, MASK);
            QDEC_SetIntMask(qdec_device->id, QDEC_INT_SAMPLE, MASK);
            QDEC_SetIntMask(qdec_device->id, QDEC_INT_ERROR, MASK);
            QDEC_SetIntMask(qdec_device->id, QDEC_INT_OVERFLOW, MASK);
            // CPU_Interrupt_Enable(QDEC2_IRQn);
        }
    }

    return 0;
}

int qdec_close(int    qdec_id)
{
    QDEC_Disable(qdec_id);
    return 0;
}

int qdec_ioctl(int qdec_id, int cmd, void *args)
{
    qdec_device_t *qdec_device = &qdec_devices[qdec_id];

    switch (cmd) {
        case QDEC_CTRL_SET_INT: {
            uint32_t offset = __builtin_ctz((uint32_t)args);
            while (offset < 5) {
                if ((uint32_t)args & (1 << offset)) {
                    QDEC_SetIntMask(qdec_id, offset, UNMASK);
                }
                offset++;
            }
            if (qdec_id == QDEC0_ID) {
                CPU_Interrupt_Enable(QDEC0_IRQn);
            } else if (qdec_id == QDEC1_ID) {
                CPU_Interrupt_Enable(QDEC1_IRQn);
            } else if (qdec_id == QDEC2_ID) {
                CPU_Interrupt_Enable(QDEC2_IRQn);
            }
            break;
        }

        case QDEC_CTRL_CLR_INT: {
            uint32_t offset = __builtin_ctz((uint32_t)args);
            while (offset < 5) {
                if ((uint32_t)args & (1 << offset)) {
                    QDEC_SetIntMask(qdec_id, offset, MASK);
                }
                offset++;
            }
            if (qdec_id == QDEC0_ID) {
                CPU_Interrupt_Disable(QDEC0_IRQn);
            } else if (qdec_id == QDEC1_ID) {
                CPU_Interrupt_Disable(QDEC1_IRQn);
            } else if (qdec_id == QDEC2_ID) {
                CPU_Interrupt_Disable(QDEC2_IRQn);
            }
            break;
        }

        case QDEC_CTRL_RESUME:
            QDEC_Enable(qdec_id);
            break;

        case QDEC_CTRL_SUSPEND:
            QDEC_Disable(qdec_id);
            break;

        case QDEC_CTRL_GET_SAMPLE_VAL:
            return QDEC_Get_Sample_Val(qdec_id);

        case QDEC_CTRL_GET_SAMPLE_DIR:
            return QDEC_Get_Sample_Direction(qdec_id);

        case QDEC_CTRL_GET_ERROR_CNT:
            return QDEC_Get_Err_Cnt(qdec_id);

        case QDEC_CTRL_SET_CALLBACK:
            qdec_device->callback = args;
            break;

        default:
            break;
    }

    return 0;
}

void qdec_isr(int qdec_id)
{
    qdec_device_t *qdec_device = &qdec_devices[qdec_id];

    /* qdec report intterupt */
    if (QDEC_GetIntMask(qdec_id, QDEC_INT_REPORT)) {
        if (QDEC_Get_Int_Status(qdec_id, QDEC_INT_REPORT)) {
            QDEC_Clr_Int_Status(qdec_id, QDEC_INT_REPORT);
            if (qdec_device->callback)
                qdec_device->callback(qdec_device, NULL, 0, QDEC_REPORT_EVENT);
        }
    }

    /* qdec sample intterupt */
    if (QDEC_GetIntMask(qdec_id, QDEC_INT_SAMPLE)) {
        if (QDEC_Get_Int_Status(qdec_id, QDEC_INT_SAMPLE)) {
            QDEC_Clr_Int_Status(qdec_id, QDEC_INT_SAMPLE);
            if (qdec_device->callback)
                qdec_device->callback(qdec_device, NULL, 0, QDEC_SAMPLE_EVENT);
        }
    }

    /* qdec error intterupt */
    if (QDEC_GetIntMask(qdec_id, QDEC_INT_ERROR)) {
        if (QDEC_Get_Int_Status(qdec_id, QDEC_INT_ERROR)) {
            QDEC_Clr_Int_Status(qdec_id, QDEC_INT_ERROR);
            if (qdec_device->callback)
                qdec_device->callback(qdec_device, NULL, 0, QDEC_ERROR_EVENT);
        }
    }

    /* qdec overflow intterupt */
    if (QDEC_GetIntMask(qdec_id, QDEC_INT_OVERFLOW)) {
        if (QDEC_Get_Int_Status(qdec_id, QDEC_INT_OVERFLOW)) {
            QDEC_Clr_Int_Status(qdec_id, QDEC_INT_OVERFLOW);
            if (qdec_device->callback)
                qdec_device->callback(qdec_device, NULL, 0, QDEC_OVERFLOW_EVENT);
        }
    }

}

static void QDEC0_IRQ(void)
{
    qdec_isr(QDEC0_INDEX);
}
static void QDEC1_IRQ(void)
{
    qdec_isr(QDEC1_INDEX);
}
static void QDEC2_IRQ(void)
{
    qdec_isr(QDEC2_INDEX);
}

