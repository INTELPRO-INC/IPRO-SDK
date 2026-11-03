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
 * SPE ITS service: unpacks the psa_ipro/its_rpc.h vectors, calls
 * its_store in ITS_NS_NSPE and packs the SPM out contract.
 */
#include <string.h>

#include "its_lfs.h"
#include "its_store.h"
#include "kdf.h"
#include "mbedtls/platform_util.h"
#include "psa/crypto.h"
#include "psa_ipro/its_rpc.h"
#include "svc_its.h"

_Static_assert(sizeof(struct psa_storage_info_t) == 12,
               "PSA Storage 1.0 psa_storage_info_t");
_Static_assert(PSA_STORAGE_FLAG_WRITE_ONCE == ITS_FLAG_WRITE_ONCE,
               "WRITE_ONCE bit");
_Static_assert(ITS_MAX_OBJECT == ITS_STORE_MAX, "object size limit");

/* one call: the decoded req, its vectors and the store results */
struct svc_its_ctx {
    struct its_req r;
    const psa_invec *in;
    psa_outvec *out;
    struct its_info info;
    uint32_t n;
};

#define Z_OFFSET 0x1u
#define Z_FLAGS  0x2u
#define Z_SIZE   0x4u

/* per id: vector counts, req fields that must be 0, store call */
struct svc_its_fn {
    uint8_t n_in;
    uint8_t n_out;
    uint8_t zero;
    psa_status_t (*run)(struct svc_its_ctx *c);
};

static psa_status_t svc_its_set(struct svc_its_ctx *c)
{
    return its_store_set(ITS_NS_NSPE, c->r.uid, c->in[1].base, c->r.size,
                         c->r.flags);
}

static psa_status_t svc_its_get(struct svc_its_ctx *c)
{
    return its_store_get(ITS_NS_NSPE, c->r.uid, c->r.offset,
                         c->out[1].base, c->r.size, &c->n, &c->info);
}

static psa_status_t svc_its_info(struct svc_its_ctx *c)
{
    return its_store_info(ITS_NS_NSPE, c->r.uid, &c->info);
}

static psa_status_t svc_its_remove(struct svc_its_ctx *c)
{
    return its_store_remove(ITS_NS_NSPE, c->r.uid);
}

static const struct svc_its_fn svc_its_fns[ITS_FN_REMOVE + 1] = {
    [ITS_FN_SET]      = { 2, 0, Z_OFFSET, svc_its_set },
    [ITS_FN_GET]      = { 1, 2, Z_FLAGS, svc_its_get },
    [ITS_FN_GET_INFO] = { 1, 1, Z_OFFSET | Z_FLAGS | Z_SIZE, svc_its_info },
    [ITS_FN_REMOVE]   = { 1, 0, Z_OFFSET | Z_FLAGS | Z_SIZE, svc_its_remove },
};

static psa_status_t svc_its_unpack(const struct svc_its_fn *f,
                                   size_t in_len, size_t out_len,
                                   struct svc_its_ctx *c)
{
    const struct its_req *r = &c->r;

    if (in_len != f->n_in || out_len != f->n_out ||
        c->in[0].len != sizeof(*r) ||
        (out_len != 0 && c->out[0].len != sizeof(struct its_rsp))) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    memcpy(&c->r, c->in[0].base, sizeof(c->r));
    if (r->reserved != 0 || ((f->zero & Z_OFFSET) && r->offset != 0) ||
        ((f->zero & Z_FLAGS) && r->flags != 0) ||
        ((f->zero & Z_SIZE) && r->size != 0) ||
        (in_len == 2 && c->in[1].len != r->size) ||
        (out_len == 2 && c->out[1].len != r->size)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    return PSA_SUCCESS;
}

psa_status_t svc_its_call(int32_t type, const psa_invec *in,
                          size_t in_len, psa_outvec *out,
                          size_t out_len)
{
    struct svc_its_ctx c = { .in = in, .out = out };
    const struct svc_its_fn *f = NULL;
    psa_status_t st = PSA_ERROR_NOT_SUPPORTED;

    if (type >= ITS_FN_SET && type <= ITS_FN_REMOVE) {
        f = &svc_its_fns[type];
        st = svc_its_unpack(f, in_len, out_len, &c);
    }
    if (st == PSA_SUCCESS) {
        st = f->run(&c);
    }
    for (size_t i = 0; i < out_len; i++) {
        out[i].len = 0;
    }
    if (st == PSA_SUCCESS && out_len != 0) {
        struct its_rsp rsp = { c.info.size, c.info.size, c.info.flags };

        memcpy(out[0].base, &rsp, sizeof(rsp));
        out[0].len = sizeof(rsp);
    }
    if (st == PSA_SUCCESS && out_len == 2) {
        out[1].len = c.n;
    }
    return st;
}

psa_status_t svc_its_init(void)
{
    uint8_t key[ITS_KEY_SIZE];
    uint8_t rnd[ITS_RAND_SIZE];
    const struct its_backend *be;
    psa_status_t st = PSA_ERROR_STORAGE_FAILURE;

    its_store_deinit();
    /* no HUK: the PSM is not mounted, so nothing is formatted */
    if (kdf_derive(KDF_LABEL_ITS, key, sizeof(key)) != 0) {
        return PSA_ERROR_HARDWARE_FAILURE;
    }
    be = its_lfs_mount();
    if (be != NULL) {
        st = psa_generate_random(rnd, sizeof(rnd));
    }
    if (st == PSA_SUCCESS) {
        st = its_store_init(be, key, rnd);
    }
    mbedtls_platform_zeroize(key, sizeof(key));
    mbedtls_platform_zeroize(rnd, sizeof(rnd));
    return st;
}

void svc_its_client_reset(void)
{
}
