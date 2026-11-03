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
 * SPE PSA Crypto service: unpacks the psa_ipro/crypto_rpc.h vectors,
 * checks key ownership and operation handles, calls the mbedtls PSA
 * core and packs the result under the SPM out contract.
 */
#include <string.h>

#include "mbedtls/platform.h"
#include "mbedtls/platform_util.h"
#include "spm_heap.h"
#include "svc_crypto.h"
#include "svc_crypto_priv.h"

_Static_assert(sizeof(mbedtls_svc_key_id_t) == sizeof(uint32_t),
               "key ids are plain u32 (no owner encoding)");

#define K_IN     0x01u      /* req.key_id must be a client key */
#define K_OUT    0x02u      /* success: rsp.key_id is a client key */
#define K_DEL    0x04u      /* success: req.key_id is gone */
#define A_IN     0x08u      /* req.attr is read into c->attr */
#define OP_NEW   0x10u      /* takes a free slot, rsp.op_handle */
#define OP_USE   0x20u      /* req.op_handle: a slot of the family */
#define OP_FREE  0x40u      /* frees the req.op_handle slot */
#define OP_IDLE  0x80u      /* req.op_handle 0: PSA_SUCCESS, no call */
#define U_STEP   0x100u     /* req.step is read, else it must be 0 */
#define U_AUX    0x200u     /* req.aux is read, else it must be 0 */
#define OP_END   (OP_USE | OP_FREE)
#define OP_ABORT (OP_USE | OP_FREE | OP_IDLE)

struct svc_fn {
    uint8_t family;
    uint8_t n_in;           /* data vectors after the req */
    uint8_t out;            /* 1: out_vec[1] is the output buffer */
    uint16_t flags;
};

/* ids a library lacks have no entry (PSA_IPRO_* of crypto_lib.h) */
static const struct svc_fn svc_fns[CRYPTO_FN_LAST + 1] = {
    [CRYPTO_FN_INIT]                 = { S_KEY, 0, 0, 0 },
    [CRYPTO_FN_KEY_IMPORT]           = { S_KEY, 1, 0, A_IN | K_OUT },
    [CRYPTO_FN_KEY_DESTROY]          = { S_KEY, 0, 0, K_IN | K_DEL },
    [CRYPTO_FN_KEY_GET_ATTRIBUTES]   = { S_KEY, 0, 1, K_IN },
    [CRYPTO_FN_KEY_EXPORT]           = { S_KEY, 0, 1, K_IN },
    [CRYPTO_FN_KEY_EXPORT_PUBLIC]    = { S_KEY, 0, 1, K_IN },
    [CRYPTO_FN_KEY_GENERATE]         = { S_KEY, 0, 0, A_IN | K_OUT },
    [CRYPTO_FN_KEY_COPY]             = { S_KEY, 0, 0, K_IN | A_IN | K_OUT },
    [CRYPTO_FN_KEY_PURGE]            = { S_KEY, 0, 0, K_IN },
    [CRYPTO_FN_HASH_COMPUTE]         = { S_HASH, 1, 1, 0 },
    [CRYPTO_FN_HASH_COMPARE]         = { S_HASH, 2, 0, 0 },
    [CRYPTO_FN_HASH_SETUP]           = { S_HASH, 0, 0, OP_NEW },
    [CRYPTO_FN_HASH_UPDATE]          = { S_HASH, 1, 0, OP_USE },
    [CRYPTO_FN_HASH_FINISH]          = { S_HASH, 0, 1, OP_END },
    [CRYPTO_FN_HASH_VERIFY]          = { S_HASH, 1, 0, OP_END },
    [CRYPTO_FN_HASH_ABORT]           = { S_HASH, 0, 0, OP_ABORT },
    [CRYPTO_FN_HASH_CLONE]           = { S_HASH, 0, 0, OP_USE | OP_NEW },
    [CRYPTO_FN_MAC_COMPUTE]          = { S_MAC, 1, 1, K_IN },
    [CRYPTO_FN_MAC_VERIFY]           = { S_MAC, 2, 0, K_IN },
    [CRYPTO_FN_MAC_SIGN_SETUP]       = { S_MAC, 0, 0, K_IN | OP_NEW },
    [CRYPTO_FN_MAC_VERIFY_SETUP]     = { S_MAC, 0, 0, K_IN | OP_NEW },
    [CRYPTO_FN_MAC_UPDATE]           = { S_MAC, 1, 0, OP_USE },
    [CRYPTO_FN_MAC_SIGN_FINISH]      = { S_MAC, 0, 1, OP_END },
    [CRYPTO_FN_MAC_VERIFY_FINISH]    = { S_MAC, 1, 0, OP_END },
    [CRYPTO_FN_MAC_ABORT]            = { S_MAC, 0, 0, OP_ABORT },
    [CRYPTO_FN_CIPHER_ENCRYPT]       = { S_CIPHER, 1, 1, K_IN },
    [CRYPTO_FN_CIPHER_DECRYPT]       = { S_CIPHER, 1, 1, K_IN },
    [CRYPTO_FN_CIPHER_ENCRYPT_SETUP] = { S_CIPHER, 0, 0, K_IN | OP_NEW },
    [CRYPTO_FN_CIPHER_DECRYPT_SETUP] = { S_CIPHER, 0, 0, K_IN | OP_NEW },
    [CRYPTO_FN_CIPHER_GENERATE_IV]   = { S_CIPHER, 0, 1, OP_USE },
    [CRYPTO_FN_CIPHER_SET_IV]        = { S_CIPHER, 1, 0, OP_USE },
    [CRYPTO_FN_CIPHER_UPDATE]        = { S_CIPHER, 1, 1, OP_USE },
    [CRYPTO_FN_CIPHER_FINISH]        = { S_CIPHER, 0, 1, OP_END },
    [CRYPTO_FN_CIPHER_ABORT]         = { S_CIPHER, 0, 0, OP_ABORT },
    [CRYPTO_FN_AEAD_ENCRYPT]         = { S_AEAD, 3, 1, K_IN },
    [CRYPTO_FN_AEAD_DECRYPT]         = { S_AEAD, 3, 1, K_IN },
#if PSA_IPRO_AEAD_MULTI
    [CRYPTO_FN_AEAD_ENCRYPT_SETUP]   = { S_AEAD, 0, 0, K_IN | OP_NEW },
    [CRYPTO_FN_AEAD_DECRYPT_SETUP]   = { S_AEAD, 0, 0, K_IN | OP_NEW },
    [CRYPTO_FN_AEAD_GENERATE_NONCE]  = { S_AEAD, 0, 1, OP_USE },
    [CRYPTO_FN_AEAD_SET_NONCE]       = { S_AEAD, 1, 0, OP_USE },
    [CRYPTO_FN_AEAD_SET_LENGTHS]     = { S_AEAD, 1, 0, OP_USE },
    [CRYPTO_FN_AEAD_UPDATE_AD]       = { S_AEAD, 1, 0, OP_USE },
    [CRYPTO_FN_AEAD_UPDATE]          = { S_AEAD, 1, 1, OP_USE },
    [CRYPTO_FN_AEAD_FINISH]          = { S_AEAD, 0, 1, OP_END | U_AUX },
    [CRYPTO_FN_AEAD_VERIFY]          = { S_AEAD, 1, 1, OP_END },
    [CRYPTO_FN_AEAD_ABORT]           = { S_AEAD, 0, 0, OP_ABORT },
#endif
    [CRYPTO_FN_SIGN_HASH]            = { S_SIGN, 1, 1, K_IN },
    [CRYPTO_FN_VERIFY_HASH]          = { S_SIGN, 2, 0, K_IN },
    [CRYPTO_FN_SIGN_MESSAGE]         = { S_SIGN, 1, 1, K_IN },
    [CRYPTO_FN_VERIFY_MESSAGE]       = { S_SIGN, 2, 0, K_IN },
    [CRYPTO_FN_ASYM_ENCRYPT]         = { S_SIGN, 2, 1, K_IN },
    [CRYPTO_FN_ASYM_DECRYPT]         = { S_SIGN, 2, 1, K_IN },
    [CRYPTO_FN_KD_SETUP]             = { S_KD, 0, 0, OP_NEW },
    [CRYPTO_FN_KD_GET_CAPACITY]      = { S_KD, 0, 0, OP_USE },
    [CRYPTO_FN_KD_SET_CAPACITY]      = { S_KD, 0, 0, OP_USE | U_AUX },
    [CRYPTO_FN_KD_INPUT_BYTES]       = { S_KD, 1, 0, OP_USE | U_STEP },
    [CRYPTO_FN_KD_INPUT_KEY]         = { S_KD, 0, 0, K_IN | OP_USE | U_STEP },
    [CRYPTO_FN_KD_KEY_AGREEMENT]     = { S_KD, 1, 0, K_IN | OP_USE | U_STEP },
    [CRYPTO_FN_KD_OUTPUT_BYTES]      = { S_KD, 0, 1, OP_USE },
    [CRYPTO_FN_KD_OUTPUT_KEY]        = { S_KD, 0, 0, A_IN | K_OUT | OP_USE },
    [CRYPTO_FN_KD_ABORT]             = { S_KD, 0, 0, OP_ABORT },
    [CRYPTO_FN_RAW_KEY_AGREEMENT]    = { S_KD, 1, 1, K_IN },
    [CRYPTO_FN_GENERATE_RANDOM]      = { S_RANDOM, 0, 1, 0 },
#if PSA_IPRO_KD_INTEGER
    [CRYPTO_FN_KD_INPUT_INTEGER]     = { S_KD, 1, 0, OP_USE | U_STEP },
#endif
#if PSA_IPRO_API_1_2
    [CRYPTO_FN_KEY_AGREEMENT]        = { S_KD, 1, 0, K_IN | A_IN | K_OUT },
    [CRYPTO_FN_PAKE_SETUP]           = { S_PAKE, 1, 0, K_IN | OP_NEW },
    [CRYPTO_FN_PAKE_SET_ROLE]        = { S_PAKE, 0, 0, OP_USE | U_AUX },
    [CRYPTO_FN_PAKE_SET_USER]        = { S_PAKE, 1, 0, OP_USE },
    [CRYPTO_FN_PAKE_SET_PEER]        = { S_PAKE, 1, 0, OP_USE },
    [CRYPTO_FN_PAKE_SET_CONTEXT]     = { S_PAKE, 1, 0, OP_USE },
    [CRYPTO_FN_PAKE_OUTPUT]          = { S_PAKE, 0, 1, OP_USE | U_STEP },
    [CRYPTO_FN_PAKE_INPUT]           = { S_PAKE, 1, 0, OP_USE | U_STEP },
    [CRYPTO_FN_PAKE_GET_SHARED_KEY]  = { S_PAKE, 0, 0, A_IN | K_OUT | OP_END },
    [CRYPTO_FN_PAKE_ABORT]           = { S_PAKE, 0, 0, OP_ABORT },
#endif
};

static struct svc_op svc_ops[PSA_CRYPTO_OP_SLOTS];

/* volatile keys of the client; 0 = empty entry */
static mbedtls_svc_key_id_t svc_keys[MBEDTLS_PSA_KEY_SLOT_COUNT];

static int svc_key_user_range(mbedtls_svc_key_id_t id)
{
    return id >= PSA_KEY_ID_USER_MIN && id <= PSA_KEY_ID_USER_MAX;
}

static int svc_key_is_client(mbedtls_svc_key_id_t id)
{
    if (svc_key_user_range(id)) {
        return 1;
    }
    for (size_t i = 0; id != 0 && i < MBEDTLS_PSA_KEY_SLOT_COUNT; i++) {
        if (svc_keys[i] == id) {
            return 1;
        }
    }
    return 0;
}

static int svc_key_adopt(mbedtls_svc_key_id_t id)
{
    if (svc_key_user_range(id)) {
        return 0;
    }
    for (size_t i = 0; i < MBEDTLS_PSA_KEY_SLOT_COUNT; i++) {
        if (svc_keys[i] == 0) {
            svc_keys[i] = id;
            return 0;
        }
    }
    return -1;
}

/* Clear id from the table once mbedtls no longer holds the key */
static void svc_key_drop_if_gone(mbedtls_svc_key_id_t id)
{
    psa_key_attributes_t a = PSA_KEY_ATTRIBUTES_INIT;
    psa_status_t st;

    for (size_t i = 0; id != 0 && i < MBEDTLS_PSA_KEY_SLOT_COUNT; i++) {
        if (svc_keys[i] == id) {
            st = psa_get_key_attributes(id, &a);
            psa_reset_key_attributes(&a);
            if (st != PSA_SUCCESS) {
                svc_keys[i] = 0;
            }
        }
    }
}

static psa_status_t svc_op_get(uint32_t handle, uint32_t kind,
                               struct svc_op **op)
{
    if (handle == 0) {
        return PSA_ERROR_BAD_STATE;
    }
    if (handle > PSA_CRYPTO_OP_SLOTS || svc_ops[handle - 1u].kind != kind) {
        return PSA_ERROR_INVALID_HANDLE;
    }
    *op = &svc_ops[handle - 1u];
    return PSA_SUCCESS;
}

static psa_status_t svc_op_new(uint32_t kind, struct svc_op **op,
                               uint32_t *handle)
{
    for (uint32_t i = 0; i < PSA_CRYPTO_OP_SLOTS; i++) {
        if (svc_ops[i].kind == S_NONE) {
            memset(&svc_ops[i], 0, sizeof(svc_ops[i]));
            svc_ops[i].kind = kind;
            *op = &svc_ops[i];
            *handle = i + 1u;
            return PSA_SUCCESS;
        }
    }
    return PSA_ERROR_INSUFFICIENT_MEMORY;
}

/* Abort releases what the operation holds on the heap, then wipe */
static void svc_op_free(struct svc_op *op)
{
    switch (op->kind) {
    case S_HASH:
        psa_hash_abort(&op->u.hash);
        break;
    case S_MAC:
        psa_mac_abort(&op->u.mac);
        break;
    case S_CIPHER:
        psa_cipher_abort(&op->u.cipher);
        break;
    case S_KD:
        psa_key_derivation_abort(&op->u.kd);
        break;
#if PSA_IPRO_AEAD_MULTI
    case S_AEAD:
        psa_aead_abort(&op->u.aead);
        break;
#endif
#if PSA_IPRO_API_1_2
    case S_PAKE:
        psa_pake_abort(&op->u.pake);
        break;
#endif
    default:
        break;
    }
    mbedtls_platform_zeroize(op, sizeof(*op));
}

/*
 * Vector counts and the req/rsp sizes must match the id exactly;
 * aux2, and step / aux on ids that do not read them, must be 0.
 */
static psa_status_t svc_unpack(const struct svc_fn *f, const psa_invec *in,
                               size_t in_len, psa_outvec *out,
                               size_t out_len, struct svc_call *c)
{
    if (f == NULL) {
        return PSA_ERROR_NOT_SUPPORTED;
    }
    if (in_len != 1u + f->n_in || out_len != 1u + f->out ||
        in[0].len != sizeof(c->req) || out[0].len != sizeof(c->rsp)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    memcpy(&c->req, in[0].base, sizeof(c->req));
    if (c->req.aux2 != 0 ||
        (!(f->flags & U_AUX) && c->req.aux != 0) ||
        (!(f->flags & U_STEP) && c->req.step != 0) ||
        c->req.step > UINT16_MAX) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    for (size_t i = 1; i < in_len; i++) {
        c->in[i - 1u] = in[i].base;
        c->in_len[i - 1u] = in[i].len;
    }
    if (f->out) {
        c->out = out[1].base;
        c->out_cap = out[1].len;
    }
    return PSA_SUCCESS;
}

/* Checks and slot lookup first; a new slot is taken last */
static psa_status_t svc_prepare(const struct svc_fn *f, struct svc_call *c)
{
    psa_status_t st;

    if (f->flags & A_IN) {
        psa_crypto_attr_unpack(&c->attr, &c->req.attr);
    }
    if ((f->flags & K_IN) && !svc_key_is_client(c->req.key_id)) {
        return PSA_ERROR_INVALID_HANDLE;
    }
    if (f->flags & OP_USE) {
        if (c->req.op_handle == 0 && (f->flags & OP_IDLE)) {
            c->idle = 1;
            return PSA_SUCCESS;
        }
        st = svc_op_get(c->req.op_handle, f->family, &c->op);
        if (st != PSA_SUCCESS) {
            return st;
        }
    } else if ((f->flags & OP_NEW) && c->req.op_handle != 0) {
        return PSA_ERROR_BAD_STATE;
    }
    if (f->flags & OP_NEW) {
        return svc_op_new(f->family, &c->new_op, &c->rsp.op_handle);
    }
    return PSA_SUCCESS;
}

static psa_status_t svc_run(const struct svc_fn *f, int32_t fn,
                            struct svc_call *c)
{
    psa_status_t st = svc_prepare(f, c);

    if (st != PSA_SUCCESS || c->idle) {
        return st;
    }
    st = svc_family_call(f->family, fn, c);
    if (st == PSA_SUCCESS && f->out && c->rsp.length > c->out_cap) {
        st = PSA_ERROR_CORRUPTION_DETECTED;
    }
    if ((f->flags & OP_NEW) && st != PSA_SUCCESS) {
        svc_op_free(c->new_op);
        c->rsp.op_handle = 0;
    }
    if (f->flags & OP_FREE) {
        svc_op_free(c->op);
    }
    if (st == PSA_SUCCESS && (f->flags & K_OUT) &&
        svc_key_adopt(c->rsp.key_id) != 0) {
        psa_destroy_key(c->rsp.key_id);
        st = PSA_ERROR_INSUFFICIENT_MEMORY;
    }
    if (f->flags & K_DEL) {
        svc_key_drop_if_gone(c->req.key_id);
    }
    return st;
}

psa_status_t svc_crypto_call(int32_t type, const psa_invec *in,
                             size_t in_len, psa_outvec *out,
                             size_t out_len)
{
    const struct svc_fn *f = NULL;
    struct svc_call c;
    psa_status_t st;

    if (type >= CRYPTO_FN_INIT && type <= CRYPTO_FN_LAST &&
        svc_fns[type].family != S_NONE) {
        f = &svc_fns[type];
    }
    memset(&c, 0, sizeof(c));
    st = svc_unpack(f, in, in_len, out, out_len, &c);
    if (st == PSA_SUCCESS) {
        st = svc_run(f, type, &c);
    }
    for (size_t i = 0; i < out_len; i++) {
        out[i].len = 0;
    }
    if (st == PSA_SUCCESS) {
        memcpy(out[0].base, &c.rsp, sizeof(c.rsp));
        out[0].len = sizeof(c.rsp);
        if (f->out) {
            out[1].len = c.rsp.length;
        }
    }
    mbedtls_platform_zeroize(&c, sizeof(c));
    return st;
}

void svc_crypto_client_reset(void)
{
    for (size_t i = 0; i < PSA_CRYPTO_OP_SLOTS; i++) {
        svc_op_free(&svc_ops[i]);
    }
    for (size_t i = 0; i < MBEDTLS_PSA_KEY_SLOT_COUNT; i++) {
        if (svc_keys[i] != 0) {
            psa_destroy_key(svc_keys[i]);
            svc_keys[i] = 0;
        }
    }
}

psa_status_t svc_crypto_init(void)
{
    if (mbedtls_platform_set_calloc_free(spm_calloc, spm_free) != 0) {
        return PSA_ERROR_GENERIC_ERROR;
    }
    return psa_crypto_init();
}
