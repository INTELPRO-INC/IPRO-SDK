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
 * NSPE client of the PSA_HANDLE_FWU service (fwu_rpc.h): stage an
 * OTA package in FW-B, mark it pending for boot2, read the state.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#include "psa/error.h"
#include "psa_ipro/fwu_rpc.h"

/*
 * len bytes at offset of FW-B. offset 0 starts a new package, any
 * other offset must equal the bytes written so far; at most
 * FWU_WRITE_MAX bytes per call.
 */
psa_status_t psa_fwu_write(uint32_t offset, const void *data, size_t len);

/*
 * Checks the package header of the len bytes written and sets the FW
 * entry pending (activeindex 1) in the inactive partition table.
 */
psa_status_t psa_fwu_install(uint32_t len);

psa_status_t psa_fwu_query(struct fwu_status *status);
