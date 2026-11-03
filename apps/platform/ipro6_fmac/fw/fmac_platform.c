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

/* Board hooks the PHY/RF driver takes from rwnx_platform.c */
#include <stdint.h>

#include "drv_aon.h"

void aon_capcode_set(uint8_t capIn, uint8_t capOut)
{
    AON_Set_Xtal_CapCode(capIn, capOut);
}

void aon_capcode_get(uint8_t *capcode_in, uint8_t *capcode_out)
{
    uint32_t tmpVal = readl(AON_BASE + AON_XTAL_CFG_OFFSET);

    *capcode_in = reg_get_bits(tmpVal, AON_XTAL_CAPCODE_IN_AON);
    *capcode_out = reg_get_bits(tmpVal, AON_XTAL_CAPCODE_OUT_AON);
}
