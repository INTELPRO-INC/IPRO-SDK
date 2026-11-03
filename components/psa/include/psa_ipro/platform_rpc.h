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
 * PSA_HANDLE_PLATFORM RPC of svc_platform.c and the NSPE client:
 * psa_call type = platform_fn, no in vector and one out vector of
 * exactly the size below.
 */
#pragma once

#include <stdint.h>

#include "psa_ipro/abi.h"

enum platform_fn {
    PLATFORM_FN_GET_UID = 1,            /* 8 bytes, eFuse chip id */
    PLATFORM_FN_GET_LIFECYCLE,          /* u32, PSA_LIFECYCLE_* */
    PLATFORM_FN_GET_IAK_PUB,            /* 65 bytes, SEC1 point */
    PLATFORM_FN_GET_NSPE_MEASUREMENT,   /* 32 bytes, SHA-256 */
    PLATFORM_FN_GET_STATS,              /* struct psa_nspe_ctrl */
};

#define PLATFORM_UID_SIZE         8u
#define PLATFORM_IAK_PUB_SIZE     65u
#define PLATFORM_MEASUREMENT_SIZE 32u

/* PSA security lifecycle states (PSA Security Model 1.0) */
#define PSA_LIFECYCLE_UNKNOWN              0x0000u
#define PSA_LIFECYCLE_ASSEMBLY_AND_TEST    0x1000u
#define PSA_LIFECYCLE_PSA_ROT_PROVISIONING 0x2000u
#define PSA_LIFECYCLE_SECURED              0x3000u
#define PSA_LIFECYCLE_DECOMMISSIONED       0x6000u
