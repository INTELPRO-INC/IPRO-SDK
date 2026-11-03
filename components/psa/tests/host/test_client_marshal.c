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
 * NSPE PSA client marshalling: the crypto, ITS, attestation and
 * platform clients against a psa_call stub that records every call
 * and answers under the SPM out contract.
 */
#include <string.h>

#include "client_stub.h"
#include "psa_ipro/crypto_lib.h"
#include "psa_crypto_client_priv.h"
#include "psa/initial_attestation.h"
#include "psa/internal_trusted_storage.h"
#include "psa_ipro/attest_rpc.h"
#include "psa_ipro/its_rpc.h"
#include "psa_ipro/memmap.h"
#include "psa_ipro/fwu_api.h"
#include "psa_ipro/platform_api.h"

static const uint8_t abc[] = "abc";
static uint8_t big[LIMIT + 16];

static void test_hash_compute(void)
{
    uint8_t out[32];
    size_t len = 99;
    psa_status_t st;
    struct psa_crypto_req q;

    reply(PSA_SUCCESS, NULL, 0, 32);
    st = psa_hash_compute(PSA_ALG_SHA_256, abc, 3, out, sizeof(out), &len);
    q = req_of(&g_log[0]);
    t_check(st == PSA_SUCCESS && g_n == 1, "hash_compute: one psa_call");
    t_check(g_log[0].handle == PSA_HANDLE_CRYPTO &&
            g_log[0].type == CRYPTO_FN_HASH_COMPUTE,
            "hash_compute: PSA_HANDLE_CRYPTO, CRYPTO_FN_HASH_COMPUTE");
    t_check(g_log[0].in_len == 2 && g_log[0].in[0].len == REQ,
            "hash_compute: in_vec[0] is the req");
    t_check(q.alg == PSA_ALG_SHA_256 && q.key_id == 0 && q.op_handle == 0 &&
            q.step == 0 && q.aux == 0 && q.aux2 == 0 &&
            all_zero(&q.attr, sizeof(q.attr)),
            "hash_compute: req.alg = SHA-256, every other field 0");
    t_check(g_log[0].in[1].base == abc && g_log[0].in[1].len == 3,
            "hash_compute: in_vec[1] = {\"abc\", 3}");
    t_check(g_log[0].out_len == 2 && g_log[0].out[0].len == RSP &&
            g_log[0].out[1].base == out && g_log[0].out[1].len == 32,
            "hash_compute: out_vec[0] = rsp, out_vec[1] = {out, 32}");
    t_check(len == 32, "hash_compute: hash_length = out_vec[1] len");

    reply(PSA_ERROR_INVALID_ARGUMENT, NULL, 0, 0);
    g_keep = 1;
    len = 99;
    st = psa_hash_compute(PSA_ALG_SHA_256, abc, 3, out, sizeof(out), &len);
    t_check(st == PSA_ERROR_INVALID_ARGUMENT && len == 0,
            "hash_compute: SPM refusal -> status, hash_length 0");
}

static psa_key_attributes_t aes_attr(void)
{
    psa_key_attributes_t a = PSA_KEY_ATTRIBUTES_INIT;

    psa_set_key_type(&a, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&a, 128);
    psa_set_key_usage_flags(&a, PSA_KEY_USAGE_ENCRYPT);
    psa_set_key_algorithm(&a, PSA_ALG_CBC_NO_PADDING);
    psa_set_key_id(&a, 0x10u);
    return a;
}

static void test_import_key(void)
{
    static const uint8_t key[16] = { 1, 2, 3 };
    psa_key_attributes_t a = aes_attr();
    struct psa_crypto_rsp rsp = { 0, 0x4001u, 0, 0 };
    mbedtls_svc_key_id_t id = 7;
    struct psa_crypto_req q;
    psa_status_t st;

    reply(PSA_SUCCESS, &rsp, sizeof(rsp), 0);
    st = psa_import_key(&a, key, sizeof(key), &id);
    q = req_of(&g_log[0]);
    t_check(st == PSA_SUCCESS && g_n == 1 &&
            g_log[0].handle == PSA_HANDLE_CRYPTO &&
            g_log[0].type == CRYPTO_FN_KEY_IMPORT,
            "import_key: PSA_HANDLE_CRYPTO, CRYPTO_FN_KEY_IMPORT");
    t_check(q.attr.type == PSA_KEY_TYPE_AES && q.attr.bits == 128 &&
            q.attr.usage == PSA_KEY_USAGE_ENCRYPT &&
            q.attr.alg == PSA_ALG_CBC_NO_PADDING && q.attr.id == 0x10u &&
            q.attr.lifetime == PSA_KEY_LIFETIME_PERSISTENT &&
            q.attr.alg2 == 0 && q.key_id == 0 &&
            q.alg == 0 && q.op_handle == 0 && q.aux2 == 0,
            "import_key: attributes copied into req.attr");
    t_check(g_log[0].in_len == 2 && g_log[0].in[1].base == key &&
            g_log[0].in[1].len == 16 && g_log[0].out_len == 1,
            "import_key: in_vec[1] = key data, out_vec = [rsp]");
    t_check(id == 0x4001u, "import_key: *key = rsp.key_id");

    reply(PSA_ERROR_NOT_SUPPORTED, &rsp, sizeof(rsp), 0);
    id = 7;
    st = psa_import_key(&a, key, sizeof(key), &id);
    t_check(st == PSA_ERROR_NOT_SUPPORTED && id == 0,
            "import_key: failure -> *key = 0");
}

static void test_its(void)
{
    static const uint8_t hello[] = "hello";
    void *top = (void *)(UINTPTR_MAX - 15u);
    uint8_t buf[64];
    struct its_req q;
    struct its_rsp rsp = { 7, 7, PSA_STORAGE_FLAG_WRITE_ONCE };
    struct psa_storage_info_t info;
    size_t n = 99;
    psa_status_t st;

    reply(PSA_SUCCESS, NULL, 0, 0);
    st = psa_its_set(0x1122334455667788ull, 5, hello,
                     PSA_STORAGE_FLAG_WRITE_ONCE);
    memcpy(&q, g_log[0].req, sizeof(q));
    t_check(st == PSA_SUCCESS && g_n == 1 &&
            g_log[0].handle == PSA_HANDLE_ITS && g_log[0].type == ITS_FN_SET,
            "its_set: PSA_HANDLE_ITS, ITS_FN_SET");
    t_check(g_log[0].in_len == 2 && g_log[0].in[0].len == sizeof(q) &&
            q.uid == 0x1122334455667788ull && q.offset == 0 &&
            q.flags == PSA_STORAGE_FLAG_WRITE_ONCE && q.size == 5 &&
            q.reserved == 0, "its_set: its_req uid, flags, size");
    t_check(g_log[0].in[1].base == hello && g_log[0].in[1].len == 5 &&
            g_log[0].out_len == 0, "its_set: in_vec[1] = data, no out");

    reply(PSA_SUCCESS, &rsp, sizeof(rsp), 7);
    st = psa_its_get(9, 2, 5000, buf, &n);
    memcpy(&q, g_log[0].req, sizeof(q));
    t_check(st == PSA_SUCCESS && g_log[0].type == ITS_FN_GET &&
            g_log[0].in_len == 1 && g_log[0].out_len == 2 &&
            g_log[0].out[0].len == sizeof(rsp) && q.uid == 9 &&
            q.offset == 2 && q.flags == 0 && q.size == ITS_MAX_OBJECT &&
            g_log[0].out[1].len == ITS_MAX_OBJECT && n == 7,
            "its_get: size = out len clamped to 2048, *len = out len");

    reply(PSA_SUCCESS, &rsp, sizeof(rsp), 7);
    n = 99;
    st = psa_its_get(9, 0, 17, top, &n);
    t_check(st == PSA_ERROR_INVALID_ARGUMENT && g_n == 0 && n == 0,
            "its_get: span past the address top -> INVALID_ARGUMENT");
    st = psa_its_get(9, 0, 16, top, &n);
    t_check(st == PSA_SUCCESS && g_n == 1 && g_log[0].out[1].base == top &&
            g_log[0].out[1].len == 16 && n == 7,
            "its_get: span ending at the address top is sent");

    reply(PSA_SUCCESS, &rsp, sizeof(rsp), 0);
    st = psa_its_get_info(9, &info);
    memcpy(&q, g_log[0].req, sizeof(q));
    t_check(st == PSA_SUCCESS && g_log[0].type == ITS_FN_GET_INFO &&
            g_log[0].out_len == 1 && q.size == 0 && q.offset == 0 &&
            info.size == 7 && info.capacity == 7 &&
            info.flags == PSA_STORAGE_FLAG_WRITE_ONCE,
            "its_get_info: req uid only, info from rsp");

    reply(PSA_SUCCESS, NULL, 0, 0);
    st = psa_its_remove(9);
    t_check(st == PSA_SUCCESS && g_log[0].type == ITS_FN_REMOVE &&
            g_log[0].in_len == 1 && g_log[0].out_len == 0 &&
            psa_its_get_support() == 0, "its_remove: [req] -> []; support 0");
}

static void test_attest(void)
{
    static const uint8_t ch[64] = { 0xC0 };
    uint8_t tok[512];
    struct attest_rsp rsp = { 300 };
    struct attest_req q;
    size_t ts = 0;
    psa_status_t st;

    reply(PSA_SUCCESS, &rsp, sizeof(rsp), 300);
    st = psa_initial_attest_get_token(ch, 33, tok, sizeof(tok), &ts);
    t_check(st == PSA_ERROR_INVALID_ARGUMENT && g_n == 0,
            "attest: challenge 33 -> INVALID_ARGUMENT, no call");

    ts = 99;
    st = psa_initial_attest_get_token(ch, 32, tok, 0, &ts);
    t_check(st == PSA_ERROR_INVALID_ARGUMENT && g_n == 0 && ts == 0,
            "attest: token buffer 0 -> INVALID_ARGUMENT, no call");

    st = psa_initial_attest_get_token(ch, 32, tok, 100, &ts);
    memcpy(&q, g_log[0].req, sizeof(q));
    t_check(st == PSA_ERROR_BUFFER_TOO_SMALL && ts == 300 && g_n == 1,
            "attest: buffer 100 < 300 -> BUFFER_TOO_SMALL, *size 300");
    t_check(g_log[0].handle == PSA_HANDLE_ATTEST &&
            g_log[0].type == ATTEST_FN_GET_TOKEN_SIZE &&
            g_log[0].in_len == 1 && g_log[0].out_len == 1 &&
            g_log[0].out[0].len == sizeof(rsp) && q.challenge_size == 32,
            "attest: GET_TOKEN_SIZE [req] -> [rsp] first");

    reply(PSA_SUCCESS, &rsp, sizeof(rsp), 300);
    ts = 0;
    st = psa_initial_attest_get_token(ch, 64, tok, sizeof(tok), &ts);
    memcpy(&q, g_log[1].req, sizeof(q));
    t_check(st == PSA_SUCCESS && g_n == 2 && ts == 300 &&
            g_log[1].type == ATTEST_FN_GET_TOKEN && g_log[1].in_len == 2 &&
            g_log[1].in[1].base == ch && g_log[1].in[1].len == 64 &&
            g_log[1].out[1].base == tok &&
            g_log[1].out[1].len == sizeof(tok) && q.challenge_size == 64,
            "attest: GET_TOKEN [req, challenge] -> [rsp, token]");

    reply(PSA_SUCCESS, &rsp, sizeof(rsp), 0);
    ts = 0;
    st = psa_initial_attest_get_token_size(48, &ts);
    t_check(st == PSA_SUCCESS && ts == 300 && g_n == 1,
            "attest: get_token_size(48) = rsp.token_size");
}

static void test_limit(void)
{
    uint8_t out[32];
    size_t len;
    psa_status_t st;

    reply(PSA_SUCCESS, NULL, 0, 0);
    st = psa_hash_compute(PSA_ALG_SHA_256, big, LIMIT - REQ, out, 32, &len);
    t_check(st == PSA_SUCCESS && g_n == 1, "limit: req + input = 4096 sent");
    reply(PSA_SUCCESS, NULL, 0, 0);
    st = psa_hash_compute(PSA_ALG_SHA_256, big, LIMIT - REQ + 1, out, 32,
                          &len);
    t_check(st == PSA_ERROR_INSUFFICIENT_MEMORY && g_n == 0,
            "limit: req + input = 4097 -> INSUFFICIENT_MEMORY, no call");
    st = psa_its_set(1, PSA_ITS_MAX_OBJECT + 1u, big, 0);
    t_check(st == PSA_ERROR_INVALID_ARGUMENT && g_n == 0,
            "limit: its_set of 2049 bytes -> INVALID_ARGUMENT, no call");
    st = psa_its_set(1, LIMIT, big, 0);
    t_check(st == PSA_ERROR_INVALID_ARGUMENT && g_n == 0,
            "limit: its_set of 4096 bytes -> INVALID_ARGUMENT, no call");
    reply(PSA_SUCCESS, NULL, 0, 0);
    st = psa_its_set(1, PSA_ITS_MAX_OBJECT, big, 0);
    t_check(st == PSA_SUCCESS && g_n == 1 &&
            g_log[0].in[1].len == PSA_ITS_MAX_OBJECT,
            "limit: its_set of 2048 bytes sent");
    reply(PSA_SUCCESS, NULL, 0, 0);
    st = psa_generate_random(big, LIMIT - RSP + 1);
    t_check(st == PSA_ERROR_INSUFFICIENT_MEMORY && g_n == 0,
            "limit: random of 4096 - rsp + 1 -> INSUFFICIENT_MEMORY");
    st = psa_generate_random(big, LIMIT - RSP);
    t_check(st == PSA_SUCCESS && g_n == 1 &&
            g_log[0].out[1].len == LIMIT - RSP,
            "limit: random of 4096 - rsp sent unclamped");
    reply(PSA_SUCCESS, NULL, 0, 32);
    st = psa_hash_compute(PSA_ALG_SHA_256, abc, 3, big, sizeof(big), &len);
    t_check(st == PSA_SUCCESS && g_log[0].out[1].len == LIMIT - RSP &&
            len == 32, "limit: output capacity clamped to 4096 - rsp");
}

static void test_ops(void)
{
    psa_hash_operation_t h = PSA_HASH_OPERATION_INIT;
    psa_hash_operation_t h2 = PSA_HASH_OPERATION_INIT;
    psa_cipher_operation_t c = PSA_CIPHER_OPERATION_INIT;
    psa_aead_operation_t ae = PSA_AEAD_OPERATION_INIT;
    psa_key_derivation_operation_t kd = PSA_KEY_DERIVATION_OPERATION_INIT;
    struct psa_crypto_rsp rsp = { 3, 0, 0, 0 };
    uint8_t out[16];
    size_t len = 5;
    size_t cap = 0;
    psa_status_t st;

    reply(PSA_SUCCESS, &rsp, sizeof(rsp), 0);
    st = psa_hash_setup(&h, PSA_ALG_SHA_256);
    t_check(st == PSA_SUCCESS && PSA_CLIENT_H(&h) == 3 &&
            req_of(&g_log[0]).op_handle == 0,
            "hash_setup: h=0 sent, op handle = rsp.op_handle");
    st = psa_hash_setup(&h, PSA_ALG_SHA_256);
    t_check(req_of(&g_log[1]).op_handle == 3,
            "hash_setup on an active op sends its handle (SPE: BAD_STATE)");
    reply(PSA_SUCCESS, &rsp, sizeof(rsp), 0);
    PSA_CLIENT_H(&h2) = 1;
    st = psa_hash_clone(&h, &h2);
    t_check(st == PSA_ERROR_BAD_STATE && g_n == 0 && PSA_CLIENT_H(&h2) == 1,
            "hash_clone to an active target -> BAD_STATE, no call");
    reply(PSA_ERROR_BAD_STATE, NULL, 0, 0);
    st = psa_hash_finish(&h, out, sizeof(out), &len);
    t_check(st == PSA_ERROR_BAD_STATE && PSA_CLIENT_H(&h) == 0 && len == 0 &&
            req_of(&g_log[0]).op_handle == 3,
            "hash_finish failed in the service: op handle 0");
    PSA_CLIENT_H(&h) = 4;
    reply(PSA_ERROR_INVALID_ARGUMENT, NULL, 0, 0);
    g_keep = 1;
    st = psa_hash_finish(&h, out, sizeof(out), &len);
    t_check(st == PSA_ERROR_INVALID_ARGUMENT && PSA_CLIENT_H(&h) == 4 && g_n == 1,
            "hash_finish refused by the SPM: op handle kept for an abort");
    st = psa_hash_verify(&h, big, LIMIT - REQ + 1);
    t_check(st == PSA_ERROR_INSUFFICIENT_MEMORY && PSA_CLIENT_H(&h) == 4 && g_n == 1,
            "hash_verify refused by the client: op handle kept, no call");
    reply(PSA_SUCCESS, NULL, 0, 0);
    st = psa_hash_abort(&h);
    t_check(st == PSA_SUCCESS && PSA_CLIENT_H(&h) == 0 &&
            req_of(&g_log[0]).op_handle == 4,
            "hash_abort after a refusal sends the kept handle");
    reply(PSA_ERROR_INVALID_ARGUMENT, NULL, 0, 0);
    PSA_CLIENT_H(&c) = 2;
    st = psa_cipher_finish(&c, out, sizeof(out), &len);
    t_check(st == PSA_ERROR_INVALID_ARGUMENT && PSA_CLIENT_H(&c) == 0,
            "cipher_finish INVALID_ARGUMENT from the service: op handle 0");

    rsp.op_handle = 6;
    rsp.length = 1000;
    reply(PSA_SUCCESS, &rsp, sizeof(rsp), 0);
    psa_key_derivation_setup(&kd, PSA_ALG_HKDF(PSA_ALG_SHA_256));
    psa_key_derivation_set_capacity(&kd, 42);
    psa_key_derivation_get_capacity(&kd, &cap);
    t_check(req_of(&g_log[1]).op_handle == 6 && req_of(&g_log[1]).aux == 42 &&
            g_log[1].type == CRYPTO_FN_KD_SET_CAPACITY && cap == 1000 &&
            g_log[2].out_len == 1,
            "kd: handle kept, capacity in aux, rsp.length = capacity");

#if PSA_IPRO_AEAD_MULTI
    rsp.op_handle = 7;
    reply(PSA_SUCCESS, &rsp, sizeof(rsp), 0);
    t_check(psa_aead_encrypt_setup(&ae, 1, PSA_ALG_GCM) == PSA_SUCCESS &&
            g_log[0].type == CRYPTO_FN_AEAD_ENCRYPT_SETUP &&
            req_of(&g_log[0]).key_id == 1 &&
            req_of(&g_log[0]).alg == PSA_ALG_GCM &&
            req_of(&g_log[0]).op_handle == 0 && PSA_CLIENT_H(&ae) == 7,
            "aead_encrypt_setup: key, alg, h=0 sent, op handle kept");
    reply(PSA_SUCCESS, NULL, 0, 0);
    t_check(psa_aead_abort(&ae) == PSA_SUCCESS && g_n == 1 &&
            g_log[0].type == CRYPTO_FN_AEAD_ABORT &&
            req_of(&g_log[0]).op_handle == 7 && PSA_CLIENT_H(&ae) == 0,
            "aead_abort sends the handle, op handle 0");
    reply(PSA_SUCCESS, NULL, 0, 0);
#else
    reply(PSA_SUCCESS, NULL, 0, 0);
    len = 5;
    t_check(psa_aead_encrypt_setup(&ae, 1, PSA_ALG_GCM) ==
            PSA_ERROR_NOT_SUPPORTED &&
            psa_aead_update(&ae, abc, 3, out, sizeof(out), &len) ==
            PSA_ERROR_NOT_SUPPORTED && len == 0 &&
            psa_aead_abort(&ae) == PSA_SUCCESS && g_n == 0,
            "aead multipart: NOT_SUPPORTED (abort SUCCESS), no call");
#endif
    t_check(psa_destroy_key(0) == PSA_SUCCESS && g_n == 0,
            "destroy_key(0): SUCCESS, no call");
}

static void test_platform(void)
{
    uint8_t uid[PLATFORM_UID_SIZE];
    struct psa_nspe_ctrl ctrl;
    psa_status_t st;

    reply(PSA_SUCCESS, NULL, 0, 0);
    st = psa_platform_get_uid(uid);
    t_check(st == PSA_SUCCESS && g_log[0].handle == PSA_HANDLE_PLATFORM &&
            g_log[0].type == PLATFORM_FN_GET_UID && g_log[0].in_len == 0 &&
            g_log[0].out_len == 1 && g_log[0].out[0].base == uid &&
            g_log[0].out[0].len == PLATFORM_UID_SIZE,
            "platform get_uid: no in, out [{uid, 8}]");
    st = psa_platform_get_stats(&ctrl);
    t_check(st == PSA_SUCCESS && g_log[1].type == PLATFORM_FN_GET_STATS &&
            g_log[1].out[0].len == sizeof(ctrl),
            "platform get_stats: out [{ctrl, 64}]");
}

static void test_fwu(void)
{
    static const uint8_t data[FWU_WRITE_MAX + 1u];
    struct fwu_req r;
    struct fwu_status q;
    psa_status_t st;

    reply(PSA_SUCCESS, NULL, 0, 0);
    st = psa_fwu_write(0x800, data, FWU_WRITE_MAX);
    memcpy(&r, g_log[0].req, sizeof(r));
    t_check(st == PSA_SUCCESS && g_n == 1 &&
            g_log[0].handle == PSA_HANDLE_FWU &&
            g_log[0].type == FWU_FN_WRITE && g_log[0].in_len == 2 &&
            g_log[0].in[0].len == sizeof(r) && r.offset == 0x800 &&
            r.len == FWU_WRITE_MAX && g_log[0].in[1].base == data &&
            g_log[0].in[1].len == FWU_WRITE_MAX && g_log[0].out_len == 0,
            "fwu write: in [{req 8: offset, len}, {data, len}], no out");
    reply(PSA_SUCCESS, NULL, 0, 0);
    t_check(psa_fwu_write(0, data, FWU_WRITE_MAX + 1u) ==
            PSA_ERROR_INVALID_ARGUMENT && g_n == 0,
            "fwu write over FWU_WRITE_MAX: INVALID_ARGUMENT, no call");
    st = psa_fwu_install(318440u);
    memcpy(&r, g_log[0].req, sizeof(r));
    t_check(st == PSA_SUCCESS && g_log[0].type == FWU_FN_INSTALL &&
            g_log[0].in_len == 1 && g_log[0].in[0].len == sizeof(r) &&
            r.offset == 0 && r.len == 318440u && g_log[0].out_len == 0,
            "fwu install: in [{req: 0, len}], no out");
    reply(PSA_SUCCESS, NULL, 0, 0);
    st = psa_fwu_query(&q);
    t_check(st == PSA_SUCCESS && g_log[0].type == FWU_FN_QUERY &&
            g_log[0].in_len == 0 && g_log[0].out_len == 1 &&
            g_log[0].out[0].base == &q &&
            g_log[0].out[0].len == sizeof(q),
            "fwu query: no in, out [{status, 40}]");
}

/* probe_cli.c badptr: hash_compute of 32 bytes at PSA_SPE_RAM_BASE */
static void test_badptr_request(void)
{
    const uint8_t *spe = (const uint8_t *)(uintptr_t)PSA_SPE_RAM_BASE;
    uint8_t hash[32];
    uint8_t normal[SNAP];
    size_t len = 7;
    psa_status_t st;

    reply(PSA_SUCCESS, NULL, 0, 32);
    st = psa_hash_compute(PSA_ALG_SHA_256, abc, 3, hash, sizeof(hash), &len);
    memcpy(normal, g_log[0].req, sizeof(normal));
    reply(PSA_ERROR_INVALID_ARGUMENT, NULL, 0, 0);
    g_keep = 1;
    st = psa_hash_compute(PSA_ALG_SHA_256, spe, 32, hash, sizeof(hash), &len);
    t_check(st == PSA_ERROR_INVALID_ARGUMENT && len == 0 && g_n == 1 &&
            g_log[0].handle == PSA_HANDLE_CRYPTO &&
            g_log[0].type == CRYPTO_FN_HASH_COMPUTE &&
            g_log[0].in_len == 2 && g_log[0].in[0].len == REQ &&
            memcmp(g_log[0].req, normal, REQ) == 0,
            "badptr: req bytes = a normal SHA-256 hash_compute's");
    t_check(g_log[0].in[1].base == spe && g_log[0].in[1].len == 32 &&
            g_log[0].out_len == 2 && g_log[0].out[0].len == RSP &&
            g_log[0].out[1].base == hash && g_log[0].out[1].len == 32,
            "badptr: in_vec[1] = {0x11010400, 32}, out {rsp, {hash, 32}}");
}

int main(void)
{
    test_hash_compute();
    test_import_key();
    test_its();
    test_attest();
    test_limit();
    test_ops();
    test_platform();
    test_fwu();
    test_badptr_request();
    test_client_ids();
    return t_summary();
}
