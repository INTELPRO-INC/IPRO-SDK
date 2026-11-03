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
 * Shared by the NSPE PSA client families. The SPE bounces at most
 * PSA_CLIENT_BOUNCE_LIMIT bytes per direction of one psa_call.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#include "psa/client.h"
#include "psa_ipro/abi.h"

#define PSA_CLIENT_BOUNCE_LIMIT 4096u

/* 1 when the len bytes at p do not run past the address space top */
static inline int psa_client_span_ok(const void *p, size_t len)
{
    return len == 0 || len - 1u <= UINTPTR_MAX - (uintptr_t)p;
}

/* cap, clamped to the room left after used bytes of a direction */
static inline size_t psa_client_cap(size_t cap, size_t used)
{
    size_t room = PSA_CLIENT_BOUNCE_LIMIT - used;

    return cap < room ? cap : room;
}

/*
 * psa_call, or PSA_ERROR_INSUFFICIENT_MEMORY without a call when the
 * in or the out vectors total more than PSA_CLIENT_BOUNCE_LIMIT.
 */
static inline psa_status_t psa_client_call(psa_handle_t handle,
                                           int32_t type,
                                           const psa_invec *in,
                                           size_t in_len, psa_outvec *out,
                                           size_t out_len)
{
    size_t in_total = 0;
    size_t out_total = 0;

    for (size_t i = 0; i < in_len; i++) {
        if (in[i].len > PSA_CLIENT_BOUNCE_LIMIT - in_total) {
            return PSA_ERROR_INSUFFICIENT_MEMORY;
        }
        in_total += in[i].len;
    }
    for (size_t i = 0; i < out_len; i++) {
        if (out[i].len > PSA_CLIENT_BOUNCE_LIMIT - out_total) {
            return PSA_ERROR_INSUFFICIENT_MEMORY;
        }
        out_total += out[i].len;
    }
    return psa_call(handle, type, in, in_len, out, out_len);
}
