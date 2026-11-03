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
 * NSPE client of the PSA_HANDLE_PLATFORM service: no in vector and
 * one out vector of exactly the size of the item.
 */
#include "psa_client_priv.h"
#include "psa_ipro/platform_api.h"

static psa_status_t platform_get(int32_t fn, void *out, size_t size)
{
    psa_outvec o = { out, size };

    return psa_client_call(PSA_HANDLE_PLATFORM, fn, NULL, 0, &o, 1);
}

psa_status_t psa_platform_get_uid(uint8_t out[PLATFORM_UID_SIZE])
{
    return platform_get(PLATFORM_FN_GET_UID, out, PLATFORM_UID_SIZE);
}

psa_status_t psa_platform_get_lifecycle(uint32_t *lifecycle)
{
    return platform_get(PLATFORM_FN_GET_LIFECYCLE, lifecycle,
                        sizeof(*lifecycle));
}

psa_status_t psa_platform_get_iak_pub(uint8_t out[PLATFORM_IAK_PUB_SIZE])
{
    return platform_get(PLATFORM_FN_GET_IAK_PUB, out, PLATFORM_IAK_PUB_SIZE);
}

psa_status_t psa_platform_get_nspe_measurement(
    uint8_t out[PLATFORM_MEASUREMENT_SIZE])
{
    return platform_get(PLATFORM_FN_GET_NSPE_MEASUREMENT, out,
                        PLATFORM_MEASUREMENT_SIZE);
}

psa_status_t psa_platform_get_stats(struct psa_nspe_ctrl *ctrl)
{
    return platform_get(PLATFORM_FN_GET_STATS, ctrl, sizeof(*ctrl));
}
