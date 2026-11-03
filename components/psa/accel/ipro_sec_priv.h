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

/* ipro_sec driver internals. */
#ifndef IPRO_SEC_PRIV_H
#define IPRO_SEC_PRIV_H

#include "ipro_sec_driver.h"

/* PSA status of a HAL v2 return value */
psa_status_t ipro_sec_status(int ret);

/*
 * AES key of attributes for the HAL: PSA_SUCCESS when the key is an
 * AES key of 128, 192 or 256 bits, else PSA_ERROR_NOT_SUPPORTED.
 */
psa_status_t ipro_sec_aes_key(const psa_key_attributes_t *attributes,
                              size_t key_buffer_size);

#endif
