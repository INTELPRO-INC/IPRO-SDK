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

#ifndef IPRO6_WRAM_SPLIT_H
#define IPRO6_WRAM_SPLIT_H

#include <stdint.h>

/* IPRO6S SRAM_CFG3 selects consecutive 16 KiB banks from the top of WRAM.
 * Its encoding differs from the unused GLB_EM_Type enum copied from IPRO7:
 * the existing IPRO6 GLB_Set_EM_Sel driver uses 0x3/0xf for 32/64 KiB. */
static inline uint32_t ipro6_wram_em_mask(uint32_t usable_bytes)
{
    if (usable_bytes < 96u * 1024u || usable_bytes > 160u * 1024u ||
        usable_bytes % (16u * 1024u) != 0u)
        return UINT32_MAX;
    return (1u << ((160u * 1024u - usable_bytes) / (16u * 1024u))) - 1u;
}

#endif
