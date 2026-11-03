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
 * PSA client ecalls: service table, iovec bounce and dispatch.
 */
#pragma once

#include <stdint.h>

#include "spm.h"

/*
 * PSA Crypto init (spm_fatal on failure), HUK, ITS and platform init
 * (logged on failure), huk_lock (spm_fatal when the route stays
 * open), FWU init, the service table and its client reset hooks.
 */
void spm_services_init(void);

/* PSA_ECALL_CALL with NSPE addresses of the iovec arrays */
psa_status_t spm_psa_call(psa_handle_t handle, int32_t type,
                          uint32_t in_vec, uint32_t in_len,
                          uint32_t out_vec, uint32_t out_len);

/* PSA_ECALL_VERSION: registered version, else PSA_VERSION_NONE */
uint32_t spm_psa_version(uint32_t sid);
