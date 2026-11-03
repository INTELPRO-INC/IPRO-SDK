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
 * CONFIG_PSA_TZC_ISOLATION: TZC_SEC set and locked by the SPE. CPU =
 * ID0 in M and U; DMA, USB, SDHA, EMAC, M154 = ID1, kept off SPE RAM,
 * flash and the slaves outside spm_periph (SEC_ENG and SF_CTRL too).
 */
#pragma once

#include <stdint.h>

/* TZC_SEC register: policy value, lock bits, tamper value */
struct spm_tzc_reg {
    uint16_t off;
    uint16_t lock_off;  /* == off: lock bits are in this register */
    uint32_t val;
    uint32_t lock;
    uint32_t open;      /* write after locking; must not take */
};

/* Entries of the policy table; reg NULL when i >= count */
uint32_t spm_tzc_count(void);
const struct spm_tzc_reg *spm_tzc_at(uint32_t i);

/*
 * Claim SEC_ENG group0, write every value, then the locks, read all
 * back, write the open values and read back again. Any failed claim
 * or mismatch: spm_fatal().
 */
void spm_tzc_apply(void);
