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
 * PSA_HANDLE_ITS RPC of svc_its.c and the NSPE client: psa_call type
 * = its_fn; req and rsp vectors are exactly their struct size.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#include "psa/internal_trusted_storage.h"

#define ITS_MAX_OBJECT PSA_ITS_MAX_OBJECT

enum its_fn {
    ITS_FN_SET = 1,         /* in [req, data], out [] */
    ITS_FN_GET,             /* in [req], out [rsp, data] */
    ITS_FN_GET_INFO,        /* in [req], out [rsp] */
    ITS_FN_REMOVE,          /* in [req], out [] */
};

/*
 * SET: size = data length, flags = create flags. GET: offset; size =
 * data_length = data out vector length. Other fields are 0.
 */
struct its_req {
    uint64_t uid;
    uint32_t offset;
    uint32_t flags;
    uint32_t size;
    uint32_t reserved;
};

/*
 * The object, capacity == size. GET returns its byte count as the
 * data out vector length.
 */
struct its_rsp {
    uint32_t size;
    uint32_t capacity;
    uint32_t flags;
};

_Static_assert(sizeof(struct its_req) == 24, "its_req size");
_Static_assert(offsetof(struct its_req, offset) == 8, "its_req.offset");
_Static_assert(offsetof(struct its_req, size) == 16, "its_req.size");
_Static_assert(sizeof(struct its_rsp) == 12, "its_rsp size");
