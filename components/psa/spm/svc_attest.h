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
 * SPE Initial Attestation service (PSA_HANDLE_ATTEST): PSA IoT
 * profile 1 tokens over the svc_platform identity and IAK.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#include "psa/client.h"

/* Reset hook: does nothing; tokens keep no client state */
void svc_attest_client_reset(void);

/* spm_service_fn of PSA_HANDLE_ATTEST, psa_ipro/attest_rpc.h ids */
psa_status_t svc_attest_call(int32_t type, const psa_invec *in,
                             size_t in_len, psa_outvec *out,
                             size_t out_len);
