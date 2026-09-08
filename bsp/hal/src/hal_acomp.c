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

#include "hal_acomp.h"
#include "drv_acomp.h"

void acomp_init(uint8_t idx, acomp_cfg_t *device)
{
    AON_ACOMP_CFG_Type acomp_cfg;

    acomp_cfg.biasProg = AON_ACOMP_BIAS_POWER_MODE1;
    acomp_cfg.hysteresisNegVolt = device->neg_hysteresis_vol;
    acomp_cfg.muxEn = 1;
    acomp_cfg.posChanSel = device->pos_ch;
    acomp_cfg.negChanSel = device->neg_ch;
    acomp_cfg.levelFactor = AON_ACOMP_LEVEL_FACTOR_1;
    acomp_cfg.biasProg = AON_ACOMP_BIAS_POWER_MODE1;
    acomp_cfg.hysteresisPosVolt = device->pos_hysteresis_vol;
    acomp_cfg.hysteresisNegVolt = device->neg_hysteresis_vol;

    AON_ACOMP_Init(idx, &acomp_cfg);
}

void acomp_enable(uint8_t idx)
{
    AON_ACOMP_Enable(idx);
}

void acomp_disable(uint8_t idx)
{
    AON_ACOMP_Disable(idx);
}

void acomp_interrupt_mask(uint8_t idx, uint8_t edge_type)
{
    AON_ACOMP_Interrupt_Mask(idx, edge_type);
}

void acomp_interrupt_unmask(uint8_t idx, uint8_t edge_type)
{
    AON_ACOMP_Interrupt_UnMask(idx, edge_type);
}

int acomp_get_result(uint8_t idx)
{
    return AON_ACOMP_Get_Result(idx);
}
