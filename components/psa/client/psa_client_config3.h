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
 * mbedtls 3.6 config of the NSPE PSA Crypto client (CONFIG_MBEDTLS_V3):
 * API types, macros and the client-mode operation structs; no mbedtls
 * code is built or linked.
 */
#ifndef PSA_CLIENT_CONFIG3_H
#define PSA_CLIENT_CONFIG3_H

#define MBEDTLS_PSA_CRYPTO_CLIENT

/* What the SPE serves: PSA_WANT_* and the size macros follow */
#define MBEDTLS_PSA_CRYPTO_CONFIG
#define MBEDTLS_PSA_CRYPTO_CONFIG_FILE "psa_ipro/crypto_features3.h"

#endif /* PSA_CLIENT_CONFIG3_H */
