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

#pragma once
#include <stddef.h>
#include <stdint.h>
#include "psa/error.h"
typedef int32_t psa_handle_t;
typedef struct psa_invec { const void *base; size_t len; } psa_invec;
typedef struct psa_outvec { void *base; size_t len; } psa_outvec;
#define PSA_IPC_CALL 0
#define PSA_NULL_HANDLE ((psa_handle_t)0)
#define PSA_VERSION_NONE (0u)
uint32_t psa_framework_version(void);
uint32_t psa_version(uint32_t sid);
psa_handle_t psa_connect(uint32_t sid, uint32_t version);
psa_status_t psa_call(psa_handle_t handle, int32_t type,
                      const psa_invec *in_vec, size_t in_len,
                      psa_outvec *out_vec, size_t out_len);
void psa_close(psa_handle_t handle);
