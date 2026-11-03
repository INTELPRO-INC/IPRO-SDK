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
 * PSA_HANDLE_ATTEST RPC of svc_attest.c and the NSPE client: psa_call
 * type = attest_fn; req and rsp vectors are exactly their struct size.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#include "psa/initial_attestation.h"

/* in vectors -> out vectors */
enum attest_fn {
    ATTEST_FN_GET_TOKEN = 1,    /* [req, challenge] -> [rsp, token] */
    ATTEST_FN_GET_TOKEN_SIZE,   /* [req] -> [rsp] */
};

/* challenge_size 32, 48 or 64; GET_TOKEN: = challenge vector len */
struct attest_req {
    uint32_t challenge_size;
};

/*
 * Token bytes; GET_TOKEN also returns them as the token vector len.
 * A token over the token vector capacity: BUFFER_TOO_SMALL, no data.
 */
struct attest_rsp {
    uint32_t token_size;
};

_Static_assert(sizeof(struct attest_req) == 4, "attest_req size");
_Static_assert(sizeof(struct attest_rsp) == 4, "attest_rsp size");
