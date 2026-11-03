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
 * PSA client calls from the NSPE. Every iovec is range checked and
 * bounced through SPE RAM: services never see a NSPE pointer, and
 * the bounce buffers are wiped after each call.
 */
#include <string.h>

#include "huk.h"
#include "spm_log.h"
#include "spm_services.h"
#include "svc_attest.h"
#include "svc_crypto.h"
#include "svc_fwu.h"
#include "svc_its.h"
#include "svc_platform.h"

#define SPM_MAX_SERVICES 5u
#define SPM_MAX_RESET_HOOKS 8u
/* raw bytes per direction; each slice starts 8-byte aligned */
#define SPM_BOUNCE_LIMIT 4096u
#define SPM_BOUNCE_ALIGN 8u
#define SPM_BOUNCE_PAD   ((PSA_MAX_IOVEC - 1u) * (SPM_BOUNCE_ALIGN - 1u))
#define SPM_BOUNCE_SIZE  (SPM_BOUNCE_LIMIT + SPM_BOUNCE_PAD)

struct spm_service {
    psa_handle_t handle;
    uint32_t version;
    spm_service_fn fn;
};

/* psa_invec / psa_outvec as laid out in NSPE memory (RV32) */
struct spm_nvec {
    uint32_t base;
    uint32_t len;
};

static struct spm_service spm_services[SPM_MAX_SERVICES];
static spm_reset_fn spm_reset_hooks[SPM_MAX_RESET_HOOKS];
/* cache-line aligned: out[0] at offset 0 can take SEC_ENG writes */
static uint8_t spm_bounce_in[SPM_BOUNCE_SIZE] __attribute__((aligned(32)));
static uint8_t spm_bounce_out[SPM_BOUNCE_SIZE] __attribute__((aligned(32)));

static struct spm_service *spm_find(psa_handle_t handle)
{
    for (uint32_t i = 0; i < SPM_MAX_SERVICES; i++) {
        struct spm_service *s = &spm_services[i];

        if (s->fn != NULL && s->handle == handle) {
            return s;
        }
    }
    return NULL;
}

static struct spm_service *spm_free_slot(void)
{
    for (uint32_t i = 0; i < SPM_MAX_SERVICES; i++) {
        if (spm_services[i].fn == NULL) {
            return &spm_services[i];
        }
    }
    return NULL;
}

int spm_register_service(psa_handle_t handle, uint32_t version,
                         spm_service_fn fn)
{
    struct spm_service *s;

    if (fn == NULL) {
        return -1;
    }
    s = spm_find(handle);
    if (s == NULL) {
        s = spm_free_slot();
    }
    if (s == NULL) {
        return -1;
    }
    s->handle = handle;
    s->version = version;
    s->fn = fn;
    return 0;
}

int spm_register_client_reset(spm_reset_fn fn)
{
    for (uint32_t i = 0; fn != NULL && i < SPM_MAX_RESET_HOOKS; i++) {
        if (spm_reset_hooks[i] == NULL) {
            spm_reset_hooks[i] = fn;
            return 0;
        }
    }
    return -1;
}

void spm_client_reset(void)
{
    for (uint32_t i = 0; i < SPM_MAX_RESET_HOOKS; i++) {
        if (spm_reset_hooks[i] != NULL) {
            spm_reset_hooks[i]();
        }
    }
}

uint32_t spm_psa_version(uint32_t sid)
{
    const struct spm_service *s = spm_find((psa_handle_t)sid);

    return s != NULL ? s->version : PSA_VERSION_NONE;
}

/* Fetch n NSPE iovecs; with write the array must be writable too. */
static int spm_fetch_vecs(struct spm_nvec *v, uint32_t addr, uint32_t n,
                          int write)
{
    uint32_t bytes = n * (uint32_t)sizeof(*v);

    if (n == 0) {
        return 0;
    }
    if (!spm_nspe_range_ok(addr, bytes, write)) {
        return -1;
    }
    return spm_copy_in(v, addr, bytes);
}

/*
 * Check each vector and assign its offset in a bounce buffer. The
 * raw lens may total SPM_BOUNCE_LIMIT; *used ends past the last one.
 */
static psa_status_t spm_layout(const struct spm_nvec *v, uint32_t n,
                               int write, uint32_t *off, uint32_t *used)
{
    uint32_t total = 0;
    uint32_t pos = 0;

    for (uint32_t i = 0; i < n; i++) {
        int null_empty = v[i].base == 0 && v[i].len == 0;

        if (!null_empty && !spm_nspe_range_ok(v[i].base, v[i].len, write)) {
            return PSA_ERROR_INVALID_ARGUMENT;
        }
        if (v[i].len > SPM_BOUNCE_LIMIT - total) {
            return PSA_ERROR_INSUFFICIENT_MEMORY;
        }
        total += v[i].len;
        pos = (pos + SPM_BOUNCE_ALIGN - 1u) & ~(SPM_BOUNCE_ALIGN - 1u);
        off[i] = pos;
        pos += v[i].len;
    }
    *used = pos;
    return PSA_SUCCESS;
}

psa_status_t spm_psa_call(psa_handle_t handle, int32_t type,
                          uint32_t in_vec, uint32_t in_len,
                          uint32_t out_vec, uint32_t out_len)
{
    const struct spm_service *s = spm_find(handle);
    struct spm_nvec nin[PSA_MAX_IOVEC];
    struct spm_nvec nout[PSA_MAX_IOVEC];
    uint32_t in_off[PSA_MAX_IOVEC];
    uint32_t out_off[PSA_MAX_IOVEC];
    psa_invec in[PSA_MAX_IOVEC];
    psa_outvec out[PSA_MAX_IOVEC];
    uint32_t in_used = 0;
    uint32_t out_used = 0;
    psa_status_t status;

    if (s == NULL) {
        return PSA_ERROR_INVALID_HANDLE;
    }
    if (in_len > PSA_MAX_IOVEC || out_len > PSA_MAX_IOVEC ||
        spm_fetch_vecs(nin, in_vec, in_len, 0) != 0 ||
        spm_fetch_vecs(nout, out_vec, out_len, 1) != 0) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    status = spm_layout(nin, in_len, 0, in_off, &in_used);
    if (status == PSA_SUCCESS) {
        status = spm_layout(nout, out_len, 1, out_off, &out_used);
    }
    if (status != PSA_SUCCESS) {
        return status;
    }

    for (uint32_t i = 0; i < in_len; i++) {
        in[i].base = &spm_bounce_in[in_off[i]];
        in[i].len = nin[i].len;
        if (nin[i].len != 0) {
            spm_copy_in(&spm_bounce_in[in_off[i]], nin[i].base, nin[i].len);
        }
    }
    memset(spm_bounce_out, 0, out_used);
    for (uint32_t i = 0; i < out_len; i++) {
        out[i].base = &spm_bounce_out[out_off[i]];
        out[i].len = nout[i].len;
    }

    status = s->fn(type, in, in_len, out, out_len);

    for (uint32_t i = 0; i < out_len; i++) {
        uint32_t n = out[i].len < nout[i].len ? (uint32_t)out[i].len
                                              : nout[i].len;

        if (status == PSA_SUCCESS && n != 0) {
            spm_copy_out(nout[i].base, &spm_bounce_out[out_off[i]], n);
        }
        nout[i].len = n;
    }
    if (out_len != 0) {
        spm_copy_out(out_vec, nout, out_len * (uint32_t)sizeof(nout[0]));
    }
    memset(spm_bounce_in, 0, in_used);
    memset(spm_bounce_out, 0, out_used);
    return status;
}

void spm_services_init(void)
{
    static const struct {
        psa_handle_t handle;
        spm_service_fn fn;
        spm_reset_fn reset;
    } services[] = {
        { PSA_HANDLE_CRYPTO, svc_crypto_call, svc_crypto_client_reset },
        { PSA_HANDLE_ITS, svc_its_call, svc_its_client_reset },
        { PSA_HANDLE_ATTEST, svc_attest_call, svc_attest_client_reset },
        { PSA_HANDLE_PLATFORM, svc_platform_call,
          svc_platform_client_reset },
        { PSA_HANDLE_FWU, svc_fwu_call, svc_fwu_client_reset },
    };
    psa_status_t st = svc_crypto_init();

    if (st != PSA_SUCCESS) {
        SPM_LOG("PSA Crypto init failed: %d", (int)st);
        spm_fatal();
    }
    if (huk_init() != 0) {
        SPM_LOG("no HUK source");
    }
    st = svc_its_init();
    if (st != PSA_SUCCESS) {
        SPM_LOG("ITS unavailable: %d", (int)st);
    }
    st = svc_platform_init();
    if (st != PSA_SUCCESS) {
        SPM_LOG("attestation unavailable: %d", (int)st);
    }
    if (huk_lock() < 0) {
        spm_fatal();
    }
    svc_fwu_init();
    for (uint32_t i = 0; i < sizeof(services) / sizeof(services[0]); i++) {
        spm_register_service(services[i].handle, 1u, services[i].fn);
        if (services[i].reset != NULL &&
            spm_register_client_reset(services[i].reset) != 0) {
            SPM_LOG("client reset hook table full");
            spm_fatal();
        }
    }
}
