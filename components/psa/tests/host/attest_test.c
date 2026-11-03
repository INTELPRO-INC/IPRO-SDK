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
 * svc_platform / svc_attest host harness, see attest_test.h.
 */
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>

#include "attest_test.h"
#include "hw_poll.h"
#include "svc_platform.h"

uint8_t spm_nspe_measurement[32];
uint32_t spm_nspe_version = NSPE_VERSION;
uint8_t g_spe[3000];
uint32_t g_lc_raw;
int g_dev;
int g_kdf_fail;
unsigned g_bad_seeds;
char g_labels[64];
struct psa_nspe_ctrl g_ctrl;
int g_bound;
int g_fatal;
int g_contract_bad;
static jmp_buf g_fatal_jmp;
static uint64_t g_rng = 0x9E3779B97F4A7C15ull;

int plat_hw_uid(uint8_t uid[8])
{
    for (int i = 0; i < 8; i++) {
        uid[i] = (uint8_t)(0xC0 + i);
    }
    return -1;
}

uint32_t plat_hw_lifecycle(void)
{
    return g_lc_raw;
}

void plat_hw_pk_hash(uint32_t pk[8])
{
    for (uint32_t i = 0; i < 8; i++) {
        pk[i] = 0x11111111u * (i + 1u);
    }
}

const uint8_t *plat_hw_spe_image(size_t *len)
{
    *len = sizeof(g_spe);
    return g_spe;
}

int huk_is_dev(void)
{
    return g_dev;
}

/* valid P-256 scalar per label: 0x01 then label-dependent bytes */
void t_seed_of(const char *label, uint8_t out[32])
{
    out[0] = 0x01;
    for (size_t i = 1; i < 32; i++) {
        out[i] = (uint8_t)(label[strlen(label) - 1] * 7 + i);
    }
}

int kdf_derive(const char *label, uint8_t *out, size_t out_len)
{
    size_t idx = strcmp(label, "IAK") == 0 ? 0 :
                 strcmp(label, "IAK2") == 0 ? 1 : 2;

    strncat(g_labels, label, sizeof(g_labels) - strlen(g_labels) - 2);
    strcat(g_labels, " ");
    memset(out, 0, out_len);
    if (g_kdf_fail || out_len != 32) {
        return -1;
    }
    if (g_bad_seeds & (1u << idx)) {
        memset(out, 0xFF, out_len);
    } else {
        t_seed_of(label, out);
    }
    return 0;
}

void spm_fatal(void)
{
    g_fatal++;
    longjmp(g_fatal_jmp, 1);
}

struct psa_nspe_ctrl *spm_virq_ctrl(void)
{
    return g_bound ? &g_ctrl : NULL;
}

int mbedtls_hardware_poll(void *data, unsigned char *out, size_t len,
                          size_t *olen)
{
    (void)data;
    for (size_t i = 0; i < len; i++) {
        g_rng ^= g_rng << 13;
        g_rng ^= g_rng >> 7;
        g_rng ^= g_rng << 17;
        out[i] = (uint8_t)g_rng;
    }
    *olen = len;
    return 0;
}

/* fn on heap copies; out[i].n: capacity in, len out; contract */
psa_status_t t_raw(svc_fn fn, int32_t type, const struct iv *in,
                      size_t in_len, struct ov *out, size_t out_len)
{
    psa_invec iv[PSA_MAX_IOVEC];
    psa_outvec ov[PSA_MAX_IOVEC];
    psa_status_t st;

    for (size_t i = 0; i < in_len; i++) {
        void *p = malloc(in[i].n ? in[i].n : 1);

        memcpy(p, in[i].p, in[i].n);
        iv[i].base = p;
        iv[i].len = in[i].n;
    }
    for (size_t i = 0; i < out_len; i++) {
        ov[i].base = calloc(1, out[i].n ? out[i].n : 1);
        ov[i].len = out[i].n;
    }
    st = fn(type, iv, in_len, ov, out_len);
    for (size_t i = 0; i < out_len; i++) {
        if (ov[i].len > out[i].n || (st != PSA_SUCCESS && ov[i].len)) {
            g_contract_bad++;
        }
        if (st == PSA_SUCCESS && out[i].p != NULL) {
            memcpy(out[i].p, ov[i].base, ov[i].len);
        }
        out[i].n = ov[i].len;
        free(ov[i].base);
    }
    for (size_t i = 0; i < in_len; i++) {
        free((void *)iv[i].base);
    }
    return st;
}

psa_status_t t_plat(int32_t type, void *buf, size_t cap)
{
    struct ov out[1] = { { buf, cap } };

    return t_raw(svc_platform_call, type, NULL, 0, out, 1);
}

psa_status_t t_init(void)
{
    psa_status_t st = PSA_ERROR_GENERIC_ERROR;

    g_labels[0] = '\0';
    if (setjmp(g_fatal_jmp) == 0) {
        st = svc_platform_init();
    }
    return st;
}
