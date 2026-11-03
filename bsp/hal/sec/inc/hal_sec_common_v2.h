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
 * SEC_ENG HAL v2. Contexts hold no pointers or engine state: each call
 * programs the block from the context and saves the state back.
 */
#ifndef HAL_SEC_COMMON_V2_H
#define HAL_SEC_COMMON_V2_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define HAL_SEC_V2_OK          0
#define HAL_SEC_V2_ERR_PARAM   (-1)
#define HAL_SEC_V2_ERR_TIMEOUT (-2)
#define HAL_SEC_V2_ERR_HW      (-3)
#define HAL_SEC_V2_ERR_VERIFY  (-4)
#define HAL_SEC_V2_ERR_ACCESS  (-5)

enum hal_sec_v2_block {
    HAL_SEC_V2_SHA,
    HAL_SEC_V2_AES,
    HAL_SEC_V2_TRNG,
    HAL_SEC_V2_PKA,
    HAL_SEC_V2_CDET,
    HAL_SEC_V2_GMAC,
    HAL_SEC_V2_BLOCKS
};

/* Engine lock hooks; without them every lock call succeeds. */
struct hal_sec_v2_lock_ops {
    int (*lock)(void *arg, unsigned int block);
    void (*unlock)(void *arg, unsigned int block);
    void *arg;
};

/* Clock on, engine endianness, TRNG on. */
int hal_sec_v2_init(void);

/*
 * Claim group0 (ID0) on every block; ID1 then reads 0xDEADBEEF and
 * cannot trigger or release. A SEC_ENG or chip reset frees it.
 */
int hal_sec_v2_claim(void);

/* 1 when every block is owned by group0 only. */
int hal_sec_v2_claimed(void);

/* SEC_ENG soft reset, then re-claim if claimed before. */
int hal_sec_v2_recover(void);

void hal_sec_v2_set_lock_ops(const struct hal_sec_v2_lock_ops *ops);

void hal_sec_v2_zeroize(void *p, size_t n);

#ifdef __cplusplus
}
#endif

#endif
