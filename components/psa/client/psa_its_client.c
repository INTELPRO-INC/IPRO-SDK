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
 * NSPE PSA Internal Trusted Storage client over psa_ipro/its_rpc.h
 * (PSA_HANDLE_ITS). The SPE stores the objects encrypted under a
 * HUK-derived key in the NSPE namespace.
 */
#include <string.h>

#include "psa_client_priv.h"
#include "psa_ipro/its_rpc.h"

static struct its_req its_req_of(psa_storage_uid_t uid)
{
    struct its_req r;

    memset(&r, 0, sizeof(r));
    r.uid = uid;
    return r;
}

/* Over PSA_ITS_MAX_OBJECT bytes: INVALID_ARGUMENT, no call */
psa_status_t psa_its_set(psa_storage_uid_t uid, uint32_t data_length,
                         const void *p_data,
                         psa_storage_create_flags_t create_flags)
{
    struct its_req r = its_req_of(uid);
    psa_invec in[2] = { { &r, sizeof(r) }, { p_data, data_length } };

    if (data_length > PSA_ITS_MAX_OBJECT) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    r.flags = create_flags;
    r.size = data_length;
    return psa_client_call(PSA_HANDLE_ITS, ITS_FN_SET, in, 2, NULL, 0);
}

/*
 * data_length over PSA_ITS_MAX_OBJECT reads PSA_ITS_MAX_OBJECT; a
 * p_data span past the address space top: INVALID_ARGUMENT, no call
 */
psa_status_t psa_its_get(psa_storage_uid_t uid, uint32_t data_offset,
                         uint32_t data_length, void *p_data,
                         size_t *p_data_length)
{
    struct its_req r = its_req_of(uid);
    struct its_rsp rsp;
    uint32_t n = data_length < PSA_ITS_MAX_OBJECT ? data_length
                                                  : PSA_ITS_MAX_OBJECT;
    psa_invec in = { &r, sizeof(r) };
    psa_outvec out[2] = { { &rsp, sizeof(rsp) }, { p_data, n } };
    psa_status_t st;

    if (!psa_client_span_ok(p_data, data_length)) {
        *p_data_length = 0;
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    r.offset = data_offset;
    r.size = n;
    st = psa_client_call(PSA_HANDLE_ITS, ITS_FN_GET, &in, 1, out, 2);
    *p_data_length = st == PSA_SUCCESS ? out[1].len : 0;
    return st;
}

psa_status_t psa_its_get_info(psa_storage_uid_t uid,
                              struct psa_storage_info_t *p_info)
{
    struct its_req r = its_req_of(uid);
    struct its_rsp rsp;
    psa_invec in = { &r, sizeof(r) };
    psa_outvec out = { &rsp, sizeof(rsp) };
    psa_status_t st;

    memset(&rsp, 0, sizeof(rsp));
    st = psa_client_call(PSA_HANDLE_ITS, ITS_FN_GET_INFO, &in, 1, &out, 1);
    if (st != PSA_SUCCESS) {
        memset(&rsp, 0, sizeof(rsp));
    }
    p_info->capacity = rsp.capacity;
    p_info->size = rsp.size;
    p_info->flags = rsp.flags;
    return st;
}

psa_status_t psa_its_remove(psa_storage_uid_t uid)
{
    struct its_req r = its_req_of(uid);
    psa_invec in = { &r, sizeof(r) };

    return psa_client_call(PSA_HANDLE_ITS, ITS_FN_REMOVE, &in, 1, NULL, 0);
}

uint32_t psa_its_get_support(void)
{
    return 0;
}
