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

#ifndef HAL_SEC_P256_H
#define HAL_SEC_P256_H

#include <stdint.h>

/* Internal P-256 field backend for a protected scalar-multiplication caller.
 * These are NOT standalone ECDH/signing APIs. The caller owns the PKA mutex,
 * validates the curve/points, and retains scalar blinding and randomization.
 * Coordinates are aligned, 32-byte big-endian integers in [0,p), Jacobian
 * x=X/Z^2, y=Y/Z^3. Q in add_mixed is affine and nonzero. Aliasing is allowed.
 * No private scalar enters this interface; coordinates may still be sensitive.
 */
typedef struct {
    uint32_t x[8], y[8], z[8];
} sec_p256_point_t;

void sec_p256_field_init(void);
void sec_p256_field_free(void);
void sec_p256_double(sec_p256_point_t *r, const sec_p256_point_t *p);
void sec_p256_add_mixed(sec_p256_point_t *r, const sec_p256_point_t *p,
                        const sec_p256_point_t *q);

#endif
