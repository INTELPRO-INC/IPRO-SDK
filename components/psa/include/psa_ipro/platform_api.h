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
 * NSPE client of the PSA_HANDLE_PLATFORM service (platform_rpc.h):
 * device identity, lifecycle, IAK public key, NSPE measurement and
 * the vIRQ control block as the SPE reads it.
 */
#pragma once

#include <stdint.h>

#include "psa/error.h"
#include "psa_ipro/abi.h"
#include "psa_ipro/platform_rpc.h"

/* eFuse chip id */
psa_status_t psa_platform_get_uid(uint8_t out[PLATFORM_UID_SIZE]);

/* PSA_LIFECYCLE_* */
psa_status_t psa_platform_get_lifecycle(uint32_t *lifecycle);

/* IAK public key, SEC1 uncompressed P-256 point */
psa_status_t psa_platform_get_iak_pub(uint8_t out[PLATFORM_IAK_PUB_SIZE]);

/* SHA-256 of the NSPE image the SPE measured at launch */
psa_status_t psa_platform_get_nspe_measurement(
    uint8_t out[PLATFORM_MEASUREMENT_SIZE]);

/* struct psa_nspe_ctrl: vIRQ mask, pending bits, fault and counters */
psa_status_t psa_platform_get_stats(struct psa_nspe_ctrl *ctrl);
