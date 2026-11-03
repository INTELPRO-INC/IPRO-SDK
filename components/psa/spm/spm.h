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
 * SPM interface for SPE services and the SPE main: NSPE memory
 * checks, copies across the boundary, service registration and
 * the NSPE launch.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#include "psa/client.h"
#include "psa_ipro/abi.h"
#include "psa_ipro/memmap.h"

/*
 * 1 when [base, base + len) lies entirely in one NSPE region: flash
 * (read only), OCRAM, OCRAM2 or PSRAM. len 0 is ok if base is in one.
 */
int spm_nspe_range_ok(uint32_t base, uint32_t len, int write);

/* 1 when [entry, entry + 4) lies in NSPE flash or NSPE OCRAM */
int spm_nspe_entry_ok(uint32_t entry);

/* Range check, then memcpy. 0 on success, -1 when refused. */
int spm_copy_in(void *dst, uint32_t nspe_src, uint32_t len);
int spm_copy_out(uint32_t nspe_dst, const void *src, uint32_t len);

/*
 * in/out are SPE bounce buffers. Each out len holds the capacity on
 * entry; the service sets it to the bytes written (0 on failure).
 * Data is copied out only on PSA_SUCCESS; len, clamped, always.
 */
typedef psa_status_t (*spm_service_fn)(int32_t type,
                                       const psa_invec *in, size_t in_len,
                                       psa_outvec *out, size_t out_len);

/* Add or replace the service of handle; -1 when full or fn NULL */
int spm_register_service(psa_handle_t handle, uint32_t version,
                         spm_service_fn fn);

/* Drops the state a service keeps for the NSPE (handles, keys) */
typedef void (*spm_reset_fn)(void);

/* Add a hook run before every NSPE re-launch; -1 when full or NULL */
int spm_register_client_reset(spm_reset_fn fn);

/* Run the client reset hooks in registration order */
void spm_client_reset(void);

void spm_launch_nspe(void) __attribute__((noreturn));
