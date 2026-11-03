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
 * Harness of the NSPE client marshalling test: records each psa_call
 * of the clients and answers it like spm_psa_call: rsp and lens on
 * success, lens 0 on a service failure.
 */
#include <string.h>

#include "check.h"
#include "client_stub.h"

struct call g_log[LOG_MAX];
size_t g_n;
int g_keep;
static psa_status_t g_st;
static uint8_t g_rsp[SNAP];
static size_t g_out1;

void reply(psa_status_t st, const void *rsp, size_t n, size_t out1)
{
    memset(g_rsp, 0, sizeof(g_rsp));
    if (rsp != NULL) {
        memcpy(g_rsp, rsp, n);
    }
    g_st = st;
    g_out1 = out1;
    g_keep = 0;
    g_n = 0;
}

psa_status_t psa_call(psa_handle_t handle, int32_t type,
                      const psa_invec *in_vec, size_t in_len,
                      psa_outvec *out_vec, size_t out_len)
{
    struct call *c = &g_log[g_n < LOG_MAX ? g_n : LOG_MAX - 1u];

    g_n++;
    memset(c, 0, sizeof(*c));
    c->handle = handle;
    c->type = type;
    c->in_len = in_len;
    c->out_len = out_len;
    for (size_t i = 0; i < in_len && i < PSA_MAX_IOVEC; i++) {
        c->in[i] = in_vec[i];
    }
    for (size_t i = 0; i < out_len && i < PSA_MAX_IOVEC; i++) {
        c->out[i] = out_vec[i];
    }
    if (in_len != 0 && in_vec[0].len <= SNAP) {
        memcpy(c->req, in_vec[0].base, in_vec[0].len);
    }
    for (size_t i = 0; i < out_len; i++) {
        size_t n = out_vec[i].len;

        if (g_st != PSA_SUCCESS) {
            out_vec[i].len = g_keep ? n : 0;
        } else if (i == 0) {
            memcpy(out_vec[0].base, g_rsp, n < SNAP ? n : SNAP);
        } else {
            out_vec[i].len = g_out1 < n ? g_out1 : n;
        }
    }
    return g_st;
}

struct psa_crypto_req req_of(const struct call *c)
{
    struct psa_crypto_req q;

    memcpy(&q, c->req, sizeof(q));
    return q;
}

int all_zero(const void *p, size_t n)
{
    const uint8_t *b = p;

    for (size_t i = 0; i < n; i++) {
        if (b[i] != 0) {
            return 0;
        }
    }
    return 1;
}

void t_check(int cond, const char *what)
{
    check(cond, what);
}

int t_summary(void)
{
    return check_summary();
}
