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
 * mbedtls_hardware_poll (MBEDTLS_ENTROPY_HARDWARE_ALT) over the
 * SEC_ENG TRNG (HAL v1), with interrupts off during the read; with
 * CONFIG_PSA_SEC_ENG_ACCEL the ipro_sec driver provides it.
 */
#include <stddef.h>
#include <stdint.h>

#include "mbedtls/build_info.h"
#include "mbedtls/entropy.h"

#include "hal_sec_trng.h"
#include "system_soc.h"

#if defined(MBEDTLS_ENTROPY_HARDWARE_ALT) && \
    !defined(CONFIG_PSA_SEC_ENG_ACCEL)
int mbedtls_hardware_poll(void *data, unsigned char *output, size_t len,
                          size_t *olen)
{
    uintptr_t flag;

    (void)data;
    flag = __irq_save();
    hal_rand_stream(output, len);
    __irq_restore(flag);
    *olen = len;
    return 0;
}
#endif
