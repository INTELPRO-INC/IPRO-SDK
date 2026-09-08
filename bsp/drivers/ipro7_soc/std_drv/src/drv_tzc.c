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

#include <drv_tzc.h>

void TZC_Set_SbootDone(uint8_t val)
{
    uint32_t tmpVal;

    tmpVal=readl(TZC_SEC_BASE + TZC_SEC_TZC_ROM_TZSRG_CTRL_OFFSET);
    tmpVal=reg_set_bits(tmpVal,TZC_SEC_TZC_SBOOT_DONE, 0xf);
    writel(tmpVal, TZC_SEC_BASE + TZC_SEC_TZC_ROM_TZSRG_CTRL_OFFSET);
}
