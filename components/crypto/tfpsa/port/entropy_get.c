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
 * mbedtls_platform_get_entropy (MBEDTLS_PSA_DRIVER_GET_ENTROPY) over
 * the SEC_ENG TRNG (HAL v1), with interrupts off during the read; full
 * entropy per output byte. With CONFIG_PSA_SEC_ENG_ACCEL the ipro_sec
 * driver provides it.
 */
#include <stddef.h>
#include <stdint.h>

#include "tf-psa-crypto/build_info.h"
#include "mbedtls/platform.h"

#include "hal_sec_trng.h"
#include "system_soc.h"

#if defined(MBEDTLS_PSA_DRIVER_GET_ENTROPY) && \
    !defined(CONFIG_PSA_SEC_ENG_ACCEL)
int mbedtls_platform_get_entropy(psa_driver_get_entropy_flags_t flags,
                                 size_t *estimate_bits,
                                 unsigned char *output, size_t output_size)
{
    uintptr_t irq;

    if (flags != 0) {
        return PSA_ERROR_NOT_SUPPORTED;
    }
    irq = __irq_save();
    hal_rand_stream(output, output_size);
    __irq_restore(irq);
    *estimate_bits = 8u * output_size;
    return 0;
}
#endif
