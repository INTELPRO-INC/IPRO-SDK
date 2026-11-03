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
 * SPE ITS service (PSA_HANDLE_ITS): psa_ipro/its_rpc.h calls on the
 * its_store ITS_NS_NSPE namespace.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#include "psa/client.h"

/*
 * ITS key = kdf_derive("ITS", 16) (failure: HARDWARE_FAILURE, PSM not
 * mounted), littlefs mount, 4 random bytes, its_store_init. Until it
 * succeeds every call: STORAGE_FAILURE.
 */
psa_status_t svc_its_init(void);

/* Reset hook: does nothing; ITS objects stay */
void svc_its_client_reset(void);

/* spm_service_fn of PSA_HANDLE_ITS */
psa_status_t svc_its_call(int32_t type, const psa_invec *in,
                          size_t in_len, psa_outvec *out,
                          size_t out_len);
