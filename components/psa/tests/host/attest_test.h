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
 * Harness of the svc_platform and svc_attest host tests: eFuse,
 * flash, HUK, KDF, NSPE and spm_fatal stand-ins, and service calls
 * on heap copies checked against the SPM out contract.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#include "psa/client.h"
#include "psa_ipro/abi.h"
#include "spm_nspe.h"

#define NSPE_VERSION 0x01020003u

typedef psa_status_t (*svc_fn)(int32_t, const psa_invec *, size_t,
                               psa_outvec *, size_t);

struct iv {
    const void *p;
    size_t n;
};

struct ov {
    void *p;
    size_t n;
};

/* stand-in controls; g_bad_seeds bit i: seed i is 0xFF.. (>= n) */
extern uint8_t g_spe[3000];
extern uint32_t g_lc_raw;
extern int g_dev;
extern int g_kdf_fail;
extern unsigned g_bad_seeds;
extern char g_labels[64];
extern struct psa_nspe_ctrl g_ctrl;
extern int g_bound;
extern int g_fatal;
extern int g_contract_bad;

/* the valid P-256 scalar kdf_derive gives for label */
void t_seed_of(const char *label, uint8_t out[32]);

/* fn on heap copies; out[i].n: capacity in, len out */
psa_status_t t_raw(svc_fn fn, int32_t type, const struct iv *in,
                   size_t in_len, struct ov *out, size_t out_len);

/* svc_platform_call of type, one out vector of cap bytes */
psa_status_t t_plat(int32_t type, void *buf, size_t cap);

/* svc_platform_init; g_labels lists the KDF labels it asked for */
psa_status_t t_init(void);
