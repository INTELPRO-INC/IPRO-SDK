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
 * svc_crypto PSA Crypto API 1.2 cases (TF-PSA-Crypto SPE):
 * psa_key_agreement and a J-PAKE exchange between two SPE operations.
 */
#include <string.h>

#include "psa_ipro/crypto_lib.h"
#include "svc_test.h"

#if PSA_IPRO_API_1_2
/* a volatile id the client never made */
#define NOT_CLIENT (PSA_KEY_ID_VENDOR_MAX - 1u)

static void key_agreement(void)
{
    uint32_t a = t_generate(ECC_P_PAIR, 256, PSA_KEY_USAGE_DERIVE,
                            PSA_ALG_ECDH);
    uint32_t b = t_generate(ECC_P_PAIR, 256, PSA_KEY_USAGE_DERIVE,
                            PSA_ALG_ECDH);
    struct psa_crypto_req r = t_req(b, 0, 0);
    struct psa_crypto_rsp rsp;
    uint8_t pb[65];
    uint8_t z[32];
    uint8_t zk[32];
    size_t n = sizeof(pb);
    struct buf peer = { pb, 0 };

    t_rpc(CRYPTO_FN_KEY_EXPORT_PUBLIC, &r, NULL, 0, pb, &n, NULL);
    peer.n = n;
    r = t_req(a, PSA_ALG_ECDH, 0);
    n = sizeof(z);
    t_rpc(CRYPTO_FN_RAW_KEY_AGREEMENT, &r, &peer, 1, z, &n, NULL);
    r.attr.type = PSA_KEY_TYPE_DERIVE;
    r.attr.usage = PSA_KEY_USAGE_EXPORT;
    t_check(t_rpc(CRYPTO_FN_KEY_AGREEMENT, &r, &peer, 1, NULL, NULL,
                  &rsp) == PSA_SUCCESS && rsp.key_id != 0,
            "KEY_AGREEMENT: ECDH P-256 into a DERIVE key");
    r = t_req(rsp.key_id, 0, 0);
    n = sizeof(zk);
    t_check(t_rpc(CRYPTO_FN_KEY_EXPORT, &r, NULL, 0, zk, &n, NULL) ==
            PSA_SUCCESS && n == 32 && memcmp(z, zk, 32) == 0 &&
            t_destroy(rsp.key_id) == PSA_SUCCESS,
            "KEY_AGREEMENT key = RAW_KEY_AGREEMENT secret, client's");
    r = t_req(NOT_CLIENT, PSA_ALG_ECDH, 0);
    r.attr.type = PSA_KEY_TYPE_DERIVE;
    t_check(t_rpc(CRYPTO_FN_KEY_AGREEMENT, &r, &peer, 1, NULL, NULL,
                  NULL) == PSA_ERROR_INVALID_HANDLE,
            "KEY_AGREEMENT: not a client key, INVALID_HANDLE");
    t_destroy(a);
    t_destroy(b);
}

static uint32_t jpake_setup(uint32_t pw, const char *user, const char *peer)
{
    struct psa_crypto_pake_suite cs = {
        PSA_ALG_JPAKE(PSA_ALG_SHA_256),
        PSA_PAKE_PRIMITIVE(PSA_PAKE_PRIMITIVE_TYPE_ECC,
                           PSA_ECC_FAMILY_SECP_R1, 256),
        PSA_PAKE_UNCONFIRMED_KEY,
    };
    struct buf d = { &cs, sizeof(cs) };
    struct psa_crypto_req r = t_req(pw, 0, 0);
    struct psa_crypto_rsp rsp;
    int ok;

    ok = t_rpc(CRYPTO_FN_PAKE_SETUP, &r, &d, 1, NULL, NULL, &rsp) ==
         PSA_SUCCESS;
    r = t_req(0, 0, rsp.op_handle);
    r.aux = PSA_PAKE_ROLE_NONE;
    ok &= t_rpc(CRYPTO_FN_PAKE_SET_ROLE, &r, NULL, 0, NULL, NULL, NULL) ==
          PSA_SUCCESS;
    r.aux = 0;
    d.p = user;
    d.n = strlen(user);
    ok &= t_rpc(CRYPTO_FN_PAKE_SET_USER, &r, &d, 1, NULL, NULL, NULL) ==
          PSA_SUCCESS;
    d.p = peer;
    d.n = strlen(peer);
    ok &= t_rpc(CRYPTO_FN_PAKE_SET_PEER, &r, &d, 1, NULL, NULL, NULL) ==
          PSA_SUCCESS;
    return ok ? rsp.op_handle : 0;
}

/* OUTPUT of step on from, INPUT of it on to */
static int jpake_send(uint32_t from, uint32_t to, uint32_t step)
{
    struct psa_crypto_req r = t_req(0, 0, from);
    uint8_t m[PSA_PAKE_OUTPUT_MAX_SIZE];
    size_t n = sizeof(m);
    struct buf d = { m, 0 };

    r.step = step;
    if (t_rpc(CRYPTO_FN_PAKE_OUTPUT, &r, NULL, 0, m, &n, NULL) !=
        PSA_SUCCESS) {
        return 0;
    }
    d.n = n;
    r.op_handle = to;
    return t_rpc(CRYPTO_FN_PAKE_INPUT, &r, &d, 1, NULL, NULL, NULL) ==
           PSA_SUCCESS;
}

/* one J-PAKE round: 2 (round 1) or 1 (round 2) key shares each way */
static int jpake_round(uint32_t u, uint32_t p, int shares)
{
    static const uint32_t steps[3] = { PSA_PAKE_STEP_KEY_SHARE,
                                       PSA_PAKE_STEP_ZK_PUBLIC,
                                       PSA_PAKE_STEP_ZK_PROOF };
    int ok = 1;

    for (int dir = 0; dir < 2; dir++) {
        for (int i = 0; i < 3 * shares; i++) {
            ok &= dir == 0 ? jpake_send(u, p, steps[i % 3])
                           : jpake_send(p, u, steps[i % 3]);
        }
    }
    return ok;
}

/* GET_SHARED_KEY of h, then the TLS 1.2 PMS of that key in pms */
static int jpake_pms(uint32_t h, uint8_t pms[32])
{
    struct psa_crypto_req r = t_req(0, 0, h);
    struct psa_crypto_rsp rsp;
    size_t n = 32;
    uint32_t key;
    int ok;

    r.attr.type = PSA_KEY_TYPE_DERIVE;
    r.attr.usage = PSA_KEY_USAGE_DERIVE;
    r.attr.alg = PSA_ALG_TLS12_ECJPAKE_TO_PMS;
    ok = t_rpc(CRYPTO_FN_PAKE_GET_SHARED_KEY, &r, NULL, 0, NULL, NULL,
               &rsp) == PSA_SUCCESS && rsp.key_id != 0;
    key = rsp.key_id;
    r = t_req(0, PSA_ALG_TLS12_ECJPAKE_TO_PMS, 0);
    t_rpc(CRYPTO_FN_KD_SETUP, &r, NULL, 0, NULL, NULL, &rsp);
    r = t_req(key, 0, rsp.op_handle);
    r.step = PSA_KEY_DERIVATION_INPUT_SECRET;
    ok &= t_rpc(CRYPTO_FN_KD_INPUT_KEY, &r, NULL, 0, NULL, NULL, NULL) ==
          PSA_SUCCESS;
    r = t_req(0, 0, rsp.op_handle);
    ok &= t_rpc(CRYPTO_FN_KD_OUTPUT_BYTES, &r, NULL, 0, pms, &n, NULL) ==
          PSA_SUCCESS && n == 32;
    t_op0(CRYPTO_FN_KD_ABORT, rsp.op_handle);
    ok &= t_destroy(key) == PSA_SUCCESS;
    return ok;
}

static void jpake(void)
{
    static const uint8_t pw_data[] = "abcdefghijklmnop";
    uint32_t pw = t_import(PSA_KEY_TYPE_PASSWORD, 128, PSA_KEY_USAGE_DERIVE,
                           PSA_ALG_JPAKE(PSA_ALG_SHA_256), pw_data, 16);
    struct psa_crypto_req r = t_req(0, 0, 0);
    uint8_t s1[32];
    uint8_t s2[32];
    uint32_t u;
    uint32_t p;

    u = jpake_setup(pw, "client", "server");
    p = jpake_setup(pw, "server", "client");
    t_check(u != 0 && p != 0 && u != p, "J-PAKE: two ops set up");
    r.op_handle = u;
    r.aux = 0x100;
    t_check(t_rpc(CRYPTO_FN_PAKE_SET_ROLE, &r, NULL, 0, NULL, NULL,
                  NULL) == PSA_ERROR_INVALID_ARGUMENT,
            "PAKE_SET_ROLE role 0x100: INVALID_ARGUMENT");
    t_check(jpake_round(u, p, 2), "J-PAKE round 1, both ways");
    t_check(jpake_round(u, p, 1), "J-PAKE round 2, both ways");
    t_check(jpake_pms(u, s1) && jpake_pms(p, s2) &&
            memcmp(s1, s2, 32) == 0,
            "J-PAKE: both sides derive the same PMS");
    t_check(t_free_slots() == SLOTS,
            "PAKE_GET_SHARED_KEY ended both ops");
    t_check(t_op0(CRYPTO_FN_PAKE_ABORT, 0) == PSA_SUCCESS,
            "PAKE_ABORT handle 0: SUCCESS");
    t_check(jpake_setup(NOT_CLIENT, "a", "b") == 0 && t_free_slots() == SLOTS,
            "PAKE_SETUP: not a client key, no slot taken");
    u = jpake_setup(pw, "client", "server");
    t_check(t_op0(CRYPTO_FN_PAKE_ABORT, u) == PSA_SUCCESS &&
            t_free_slots() == SLOTS, "PAKE_ABORT frees the op");
    t_destroy(pw);
}
#endif

void test_api12(void)
{
#if PSA_IPRO_API_1_2
    key_agreement();
    jpake();
#endif
}
