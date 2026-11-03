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
 * Label KDF over huk_prf (AES-128-ECB, HUK key). Block i, from 1:
 *   huk_prf(label zero-padded to 12 bytes || i as u32 big-endian)
 * Output = blocks 1, 2, ... truncated to out_len; no [L], no context.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#define KDF_LABEL_MAX 12u

#define KDF_LABEL_ITS      "ITS"
#define KDF_LABEL_IAK      "IAK"
#define KDF_LABEL_IAK2     "IAK2"
#define KDF_LABEL_IAK3     "IAK3"

/*
 * 0 on success. -1 for a label of 0 or more than 12 bytes, a NULL
 * out with out_len > 0, or a PRF failure; out is zeroed on failure.
 */
int kdf_derive(const char *label, uint8_t *out, size_t out_len);
