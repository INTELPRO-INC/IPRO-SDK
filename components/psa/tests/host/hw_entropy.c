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
 * mbedtls_platform_get_entropy (MBEDTLS_PSA_DRIVER_GET_ENTROPY) of the
 * TF-PSA-Crypto host tests over the test's mbedtls_hardware_poll.
 */
#include "hw_poll.h"
#include "mbedtls/platform.h"

int mbedtls_platform_get_entropy(psa_driver_get_entropy_flags_t flags,
                                 size_t *estimate_bits,
                                 unsigned char *output, size_t output_size)
{
    size_t olen = 0;

    if (flags != 0) {
        return PSA_ERROR_NOT_SUPPORTED;
    }
    if (mbedtls_hardware_poll(NULL, output, output_size, &olen) != 0 ||
        olen != output_size) {
        return PSA_ERROR_INSUFFICIENT_ENTROPY;
    }
    *estimate_bits = 8u * output_size;
    return 0;
}
