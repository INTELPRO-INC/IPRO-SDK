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
 * mbedtls_hardware_poll (MBEDTLS_ENTROPY_HARDWARE_ALT) of the host
 * tests; mbedtls 3.6 declares it only in a library-private header.
 */
#ifndef HOST_HW_POLL_H
#define HOST_HW_POLL_H

#include <stddef.h>

int mbedtls_hardware_poll(void *data, unsigned char *output, size_t len,
                          size_t *olen);

#endif /* HOST_HW_POLL_H */
