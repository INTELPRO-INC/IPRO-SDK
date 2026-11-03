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
 * PSA Initial Attestation API 1.0. The NSPE client implements it over
 * psa_ipro/attest_rpc.h; the token is a COSE_Sign1 PSA IoT profile 1
 * token signed by the IAK (ES256).
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#include "psa/error.h"

#define PSA_INITIAL_ATTEST_API_VERSION_MAJOR 1
#define PSA_INITIAL_ATTEST_API_VERSION_MINOR 0

#define PSA_INITIAL_ATTEST_CHALLENGE_SIZE_32 (32u)
#define PSA_INITIAL_ATTEST_CHALLENGE_SIZE_48 (48u)
#define PSA_INITIAL_ATTEST_CHALLENGE_SIZE_64 (64u)

#define PSA_INITIAL_ATTEST_MAX_TOKEN_SIZE (0x400u)

psa_status_t psa_initial_attest_get_token(const uint8_t *auth_challenge,
                                          size_t challenge_size,
                                          uint8_t *token_buf,
                                          size_t token_buf_size,
                                          size_t *token_size);

psa_status_t psa_initial_attest_get_token_size(size_t challenge_size,
                                               size_t *token_size);
