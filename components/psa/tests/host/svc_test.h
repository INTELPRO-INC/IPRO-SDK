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
 * Harness of the svc_crypto host test: SPM-like vector copies, the
 * out contract check, RPC helpers and the shared check counters.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#include "psa_ipro/abi.h"
#include "psa_ipro/crypto_rpc.h"

#define SLOTS PSA_CRYPTO_OP_SLOTS
#define ECC_P_PAIR PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1)

struct buf {
    const void *p;
    size_t n;
};

extern const uint8_t abc_sha256[32];
extern const uint8_t aes_key[16];
extern const uint8_t aes_pt[16];
extern const uint8_t aes_ct[16];

/* check.h counters live in svc_test.c; PSA_TEST_VERBOSE adds detail */
void t_check(int cond, const char *what);
int t_summary(void);
void t_info(const char *fmt, ...);
int t_contract_bad(void);
int t_calls(void);

/* svc_crypto_call on copies; out[i].n in: capacity, out: len */
psa_status_t t_raw(int32_t fn, const struct buf *in, size_t in_len,
                   struct buf *out, size_t out_len);

/*
 * req + n data inputs, rsp, and when olen != NULL an output of *olen
 * bytes (*olen = bytes written). rsp is zeroed on failure.
 */
psa_status_t t_rpc(int32_t fn, const struct psa_crypto_req *req,
                   const struct buf *data, size_t n, void *o,
                   size_t *olen, struct psa_crypto_rsp *rsp);

struct psa_crypto_req t_req(uint32_t key, uint32_t alg, uint32_t h);
psa_status_t t_op0(int32_t fn, uint32_t h);
psa_status_t t_op1(int32_t fn, uint32_t h);
uint32_t t_hash_setup(void);
size_t t_free_slots(void);
uint32_t t_import(psa_key_type_t type, size_t bits, psa_key_usage_t usage,
                  psa_algorithm_t alg, const void *key, size_t len);
uint32_t t_generate(psa_key_type_t type, size_t bits,
                    psa_key_usage_t usage, psa_algorithm_t alg);
psa_status_t t_destroy(uint32_t key);

/* known-answer cases per family, test_svc_crypto_family.c */
void test_hash(void);
void test_cipher(void);
void test_mac(void);
void test_aead(void);
void test_ecc(void);
void test_kd(void);
/* API 1.2 cases (TF-PSA-Crypto), test_svc_crypto_api12.c */
void test_api12(void);

/* psa_set_key_id on the wire attributes: volatile becomes persistent */
static inline void t_attr_id(struct psa_crypto_attr *w, uint32_t id)
{
    w->id = id;
    if (PSA_KEY_LIFETIME_IS_VOLATILE(w->lifetime)) {
        w->lifetime = PSA_KEY_LIFETIME_PERSISTENT;
    }
}
