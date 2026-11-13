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

/* Host stand-in for bsp/hal/sec/inc/hal_sec_hash.h: SHA-256 in software. */
#ifndef HAL_SEC_HASH_H
#define HAL_SEC_HASH_H
#include <stdint.h>
typedef enum { SEC_HASH_MODE_DIRECT = 0 } sec_hash_mode_t;
enum { SEC_HASH_SHA256 = 1 };
typedef struct {
    uint32_t state[8];
    uint64_t bits;
    uint8_t  block[64];
    uint32_t used;
} sec_hash_handle_t;
int sec_hash_init_ex(sec_hash_handle_t *handle, sec_hash_mode_t mode, uint8_t type);
int sec_hash_deinit(sec_hash_handle_t *handle);
int sec_hash_update(sec_hash_handle_t *handle, const void *buffer, uint32_t size);
int sec_hash_finish(sec_hash_handle_t *handle, void *buffer);
#endif
