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
 * Harness of the NSPE client marshalling test: a psa_call stub that
 * records each call and answers under the SPM out contract, and the
 * shared check counters.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#include "psa/client.h"
#include "psa/crypto.h"
#include "psa_ipro/abi.h"
#include "psa_ipro/crypto_rpc.h"

#define LOG_MAX 64u
#define SNAP    128u
#define LIMIT   4096u
#define REQ     sizeof(struct psa_crypto_req)
#define RSP     sizeof(struct psa_crypto_rsp)

struct call {
    psa_handle_t handle;
    int32_t type;
    size_t in_len;
    size_t out_len;
    psa_invec in[PSA_MAX_IOVEC];
    psa_outvec out[PSA_MAX_IOVEC];
    uint8_t req[SNAP];          /* in_vec[0] bytes during the call */
};

extern struct call g_log[LOG_MAX];
extern size_t g_n;
/* 1: a failure leaves the out lens as sent (an SPM refusal) */
extern int g_keep;

/* Clears the log; every next call answers st, rsp and out1 bytes */
void reply(psa_status_t st, const void *rsp, size_t n, size_t out1);

struct psa_crypto_req req_of(const struct call *c);
int all_zero(const void *p, size_t n);

/* check.h counters live in client_stub.c */
void t_check(int cond, const char *what);
int t_summary(void);

/* every crypto id in crypto_rpc.h order, test_client_ids.c */
void test_client_ids(void);
