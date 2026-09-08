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

#include "hal_keyscan.h"
#include "kys_reg.h"
#include "drv_glb.h"
#include "drv_kys.h"

static void KeyScan_IRQ(void);

static keyscan_device_t keyscan_devices[KEYSCAN_MAX_INDEX] = {
    KEYSCAN_CONFIG
};

keyscan_device_t* keyscan_getDevice(int index)
{
    return &keyscan_devices[index];
}

int keyscan_init(int kys_id, uint16_t casp)
{
    keyscan_device_t *keyscan_device = &keyscan_devices[kys_id];

    KYS_CFG_Type kysCfg;

    kysCfg.col = keyscan_device->col_num;
    kysCfg.row = keyscan_device->row_num;
    kysCfg.idleDuration = 0;
    kysCfg.ghostEn = 0;
    kysCfg.deglitchEn = (keyscan_device->deglitch_count > 0);
    kysCfg.deglitchCnt = keyscan_device->deglitch_count;

    KYS_Init(&kysCfg);

    return 0;
}

int keyscan_control(int kys_id, int cmd, void *args)
{
    keyscan_device_t *keyscan_device = &keyscan_devices[kys_id];

    switch (cmd) {
        case KYS_CTRL_SET_INT:
#ifdef BSP_USING_KEYSCAN
            Interrupt_Handler_Register(KYS_IRQn, KeyScan_IRQ);
#endif
            KYS_IntMask(0);
            CPU_Interrupt_Enable(KYS_IRQn);
            break;
        case KYS_CTRL_CLR_INT:
            Interrupt_Handler_Register(KYS_IRQn, NULL);
            KYS_IntMask(1);
            CPU_Interrupt_Disable(KYS_IRQn);
            break;
        case KYS_CTRL_GET_INT:
            return KYS_GetIntStatus();
        case KYS_CTRL_RESUME:
            KYS_Enable();
            break;
        case KYS_CTRL_SUSPEND:
            KYS_Disable();
            break;
        case KYS_CTRL_KEYSCAN_GET_KEYCODE: {
            uint32_t *key_code = (uint32_t *)args;
            *key_code = KYS_GetKeycode(KYS_KEYCODE_ALL, NULL, NULL);
            KYS_IntClear();
            break;
        }
        case KYS_CTRL_SET_CALLBACK:
            keyscan_device->callback = args;
            break;
        default:
            break;
    }

    return 0;
}

static void KeyScan_IRQ(void)
{
    if (keyscan_devices[KEYSCAN_INDEX].callback) {
        keyscan_devices[KEYSCAN_INDEX].callback(&keyscan_devices[KEYSCAN_INDEX], (void *)(uintptr_t)KYS_GetKeycode(KYS_KEYCODE_ALL, NULL, NULL), 0, KEYSCAN_EVENT_TRIG);
    }

    KYS_IntClear();
}

