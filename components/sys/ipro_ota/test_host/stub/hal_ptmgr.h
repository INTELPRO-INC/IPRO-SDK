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

/* Host stand-in for bsp/hal/inc/hal_ptmgr.h: the fields ipro_ota reads. */
#ifndef HAL_PTMGR_H
#define HAL_PTMGR_H
#include <stdint.h>
enum { PT_ENTRY_FW_CPU0 = 0 };
typedef struct {
    uint8_t  activeIndex;
    uint32_t Address[2];
    uint32_t maxLen[2];
} PtTable_Entry_Config;
#endif
