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


#ifndef __DRV_L1C_H__
#define __DRV_L1C_H__

#include "drv_common.h"
#include "core_compatiable.h"

typedef struct
{
    uint8_t cacheEn;
    uint8_t wayDis;
    uint8_t wa;
    uint8_t wb;
    uint8_t wt;
    uint8_t rsvd[3];
    uint32_t cacheRangeL;
    uint32_t cacheRangeH;
} L1C_CACHE_Cfg_Type;

#define L1C_WAY_DISABLE_NONE       0x00
#define L1C_WAY_DISABLE_ONE        0x01
#define L1C_WAY_DISABLE_TWO        0x03
#define L1C_WAY_DISABLE_ALL        0x03
#define L1C_WAY_DISABLE_NOT_CAHNGE 0xFF

/*
 * L1C Cache API Macros
 * Maps L1C_* functions to CCM operations via csi_* compatibility macros
 */
#ifndef __riscv_32e

#define L1C_ICache_Enable(wayDisable)           ({ csi_icache_enable(); SUCCESS; })
#define L1C_DCache_Enable(wayDisable)           ({ csi_dcache_enable(); SUCCESS; })
#define L1C_ICache_Disable()                    ({ csi_icache_disable(); SUCCESS; })
#define L1C_DCache_Disable()                    ({ csi_dcache_disable(); SUCCESS; })

#define L1C_DCache_Clean_All()                  ({ csi_dcache_clean(); SUCCESS; })
#define L1C_DCache_Clean_Invalid_All()          ({ csi_dcache_clean_invalid(); SUCCESS; })
#define L1C_ICache_Invalid_All()                ({ csi_icache_invalid(); SUCCESS; })
#define L1C_DCache_Invalid_All()                ({ csi_dcache_invalid(); SUCCESS; })

#define L1C_DCache_Clean_By_Addr(addr, len) \
    ({ csi_dcache_clean_range((void *)(addr), (len)); SUCCESS; })
#define L1C_DCache_Clean_Invalid_By_Addr(addr, len) \
    ({ csi_dcache_clean_invalid_range((void *)(addr), (len)); SUCCESS; })
#define L1C_ICache_Invalid_By_Addr(addr, len) \
    ({ csi_icache_invalid(); SUCCESS; })
#define L1C_DCache_Invalid_By_Addr(addr, len) \
    ({ csi_dcache_invalid_range((void *)(addr), (len)); SUCCESS; })

#else /* __riscv_32e - no cache on E cores */

#define L1C_ICache_Enable(wayDisable)           (SUCCESS)
#define L1C_DCache_Enable(wayDisable)           (SUCCESS)
#define L1C_ICache_Disable()                    (SUCCESS)
#define L1C_DCache_Disable()                    (SUCCESS)

#define L1C_DCache_Clean_All()                  (SUCCESS)
#define L1C_DCache_Clean_Invalid_All()          (SUCCESS)
#define L1C_ICache_Invalid_All()                (SUCCESS)
#define L1C_DCache_Invalid_All()                (SUCCESS)

#define L1C_DCache_Clean_By_Addr(addr, len)             (SUCCESS)
#define L1C_DCache_Clean_Invalid_By_Addr(addr, len)     (SUCCESS)
#define L1C_ICache_Invalid_By_Addr(addr, len)           (SUCCESS)
#define L1C_DCache_Invalid_By_Addr(addr, len)           (SUCCESS)

#endif /* __riscv_32e */

/* Address range check - IPRO6 specific */
static inline int L1C_Is_DCache_Range(uintptr_t addr)
{
    return (((addr >> 16) & 0xffff) >= 0x62FC) ? 1 : 0;
}

static inline int L1C_Get_None_Cache_Addr(uintptr_t addr)
{
    return (addr & 0x0FFFFFFF) | 0x20000000;
}

#endif /* __DRV_L1C_H__ */
