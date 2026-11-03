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
 * NSPE client of the PSA_HANDLE_FWU service (psa_ipro/fwu_rpc.h).
 * The SPE writes the flash; the NSPE only passes the bytes.
 */
#include "psa_client_priv.h"
#include "psa_ipro/fwu_api.h"

/* Over FWU_WRITE_MAX bytes: INVALID_ARGUMENT, no call */
psa_status_t psa_fwu_write(uint32_t offset, const void *data, size_t len)
{
    struct fwu_req r = { offset, (uint32_t)len };
    psa_invec in[2] = { { &r, sizeof(r) }, { data, len } };

    if (len > FWU_WRITE_MAX) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    return psa_client_call(PSA_HANDLE_FWU, FWU_FN_WRITE, in, 2, NULL, 0);
}

psa_status_t psa_fwu_install(uint32_t len)
{
    struct fwu_req r = { 0, len };
    psa_invec in = { &r, sizeof(r) };

    return psa_client_call(PSA_HANDLE_FWU, FWU_FN_INSTALL, &in, 1, NULL, 0);
}

psa_status_t psa_fwu_query(struct fwu_status *status)
{
    psa_outvec out = { status, sizeof(*status) };

    return psa_client_call(PSA_HANDLE_FWU, FWU_FN_QUERY, NULL, 0, &out, 1);
}
