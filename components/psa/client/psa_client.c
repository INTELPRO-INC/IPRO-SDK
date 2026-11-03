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
 * NSPE PSA client API (FF-M 1.1, stateless handles) over the SPE
 * ecall ABI: a7 = PSA_ECALL_*, a0..a5 = arguments, a0 = result.
 */
#include "psa/client.h"
#include "psa_ipro/ecall.h"

uint32_t psa_framework_version(void)
{
    return (uint32_t)nspe_ecall(PSA_ECALL_FRAMEWORK_VERSION,
                                0, 0, 0, 0, 0, 0);
}

uint32_t psa_version(uint32_t sid)
{
    return (uint32_t)nspe_ecall(PSA_ECALL_VERSION, sid, 0, 0, 0, 0, 0);
}

psa_handle_t psa_connect(uint32_t sid, uint32_t version)
{
    return (psa_handle_t)nspe_ecall(PSA_ECALL_CONNECT, sid, version,
                                    0, 0, 0, 0);
}

psa_status_t psa_call(psa_handle_t handle, int32_t type,
                      const psa_invec *in_vec, size_t in_len,
                      psa_outvec *out_vec, size_t out_len)
{
    return (psa_status_t)nspe_ecall(PSA_ECALL_CALL, (uint32_t)handle,
                                    (uint32_t)type,
                                    (uint32_t)(uintptr_t)in_vec,
                                    (uint32_t)in_len,
                                    (uint32_t)(uintptr_t)out_vec,
                                    (uint32_t)out_len);
}

void psa_close(psa_handle_t handle)
{
    (void)nspe_ecall(PSA_ECALL_CLOSE, (uint32_t)handle, 0, 0, 0, 0, 0);
}
