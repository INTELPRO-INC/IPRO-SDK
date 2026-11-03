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

#ifndef _HAL_SEC_TRNG_H
#define _HAL_SEC_TRNG_H

#include "platform_common.h"

#ifdef __cplusplus
extern "C" {
#endif

int hal_rand_stream(uint8_t *buf, int len);
int hal_rand(void);
uint32_t hal_sec_get_random_word(void);
int hal_sec_trng_init(void);
uint32_t hal_rosc_measure(uint8_t rosc_idx, uint16_t *t_cnt, uint16_t *g_cnt);

#ifdef __cplusplus
}
#endif

#endif
