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
 * Label KDF with huk_prf, single-block AES-128-ECB, as the PRF.
 */
#include <string.h>

#include "huk.h"
#include "kdf.h"
#include "mbedtls/platform_util.h"

int kdf_derive(const char *label, uint8_t *out, size_t out_len)
{
    uint8_t in[16] = { 0 };
    uint8_t blk[16];
    size_t llen = label != NULL ? strlen(label) : 0;
    int ret = 0;

    if (out == NULL && out_len != 0) {
        return -1;
    }
    if (llen == 0 || llen > KDF_LABEL_MAX) {
        mbedtls_platform_zeroize(out, out_len);
        return -1;
    }
    memcpy(in, label, llen);
    for (size_t pos = 0; ret == 0 && pos < out_len; pos += 16) {
        uint32_t i = (uint32_t)(pos / 16u) + 1u;
        size_t n = out_len - pos < 16 ? out_len - pos : 16;

        in[12] = (uint8_t)(i >> 24);
        in[13] = (uint8_t)(i >> 16);
        in[14] = (uint8_t)(i >> 8);
        in[15] = (uint8_t)i;
        ret = huk_prf(in, blk);
        if (ret == 0) {
            memcpy(out + pos, blk, n);
        }
    }
    mbedtls_platform_zeroize(blk, sizeof(blk));
    if (ret != 0) {
        mbedtls_platform_zeroize(out, out_len);
        return -1;
    }
    return 0;
}
