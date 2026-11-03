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
 * ipro_sec driver: entropy for the CTR_DRBG from the SEC_ENG TRNG
 * through HAL v2, as mbedtls_hardware_poll (3.6,
 * MBEDTLS_ENTROPY_HARDWARE_ALT) or mbedtls_platform_get_entropy
 * (TF-PSA, MBEDTLS_PSA_DRIVER_GET_ENTROPY); full entropy per byte.
 */
#include <stddef.h>
#include <stdint.h>

#include "psa/crypto.h"
#include "hal_sec_common_v2.h"
#include "hal_sec_trng_v2.h"

#if defined(MBEDTLS_ENTROPY_HARDWARE_ALT)
#include "mbedtls/entropy.h"

int mbedtls_hardware_poll(void *data, unsigned char *output, size_t len,
                          size_t *olen)
{
    (void)data;
    *olen = 0;
    if (hal_sec_trng_v2_read(output, len) != HAL_SEC_V2_OK) {
        hal_sec_v2_zeroize(output, len);
        return MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
    }
    *olen = len;
    return 0;
}
#endif

#if defined(MBEDTLS_PSA_DRIVER_GET_ENTROPY)
#include "mbedtls/platform.h"

int mbedtls_platform_get_entropy(psa_driver_get_entropy_flags_t flags,
                                 size_t *estimate_bits,
                                 unsigned char *output, size_t output_size)
{
    *estimate_bits = 0;
    if (flags != 0) {
        return PSA_ERROR_NOT_SUPPORTED;
    }
    if (hal_sec_trng_v2_read(output, output_size) != HAL_SEC_V2_OK) {
        hal_sec_v2_zeroize(output, output_size);
        return PSA_ERROR_INSUFFICIENT_ENTROPY;
    }
    *estimate_bits = 8u * output_size;
    return 0;
}
#endif
