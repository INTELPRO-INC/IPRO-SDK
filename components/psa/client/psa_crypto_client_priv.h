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
 * Shared by psa_crypto_client*.c: one psa_ipro/crypto_rpc.h call per
 * API function. Operation structs keep the SPE op handle in the field
 * PSA_CLIENT_H / PSA_CLIENT_KD_H names; 0 = no SPE operation.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#include "psa/client.h"
#include "psa/crypto.h"
#include "psa_ipro/crypto_lib.h"
#include "psa_ipro/abi.h"
#include "psa_ipro/crypto_rpc.h"

/* 2.28: id (alg of a key derivation); 3.6, TF-PSA-Crypto: handle */
#if PSA_IPRO_CLIENT_HANDLE
typedef mbedtls_psa_client_handle_t psa_client_handle_t;
#define PSA_CLIENT_H(op)    ((op)->handle)
#define PSA_CLIENT_KD_H(op) ((op)->handle)
#else
typedef unsigned int psa_client_handle_t;
#define PSA_CLIENT_H(op)    ((op)->id)
#define PSA_CLIENT_KD_H(op) ((op)->alg)
#endif

/* req with key_id, alg and op_handle; every other byte 0 */
struct psa_crypto_req psa_client_crypto_req(uint32_t key, uint32_t alg,
                                            uint32_t h);

/*
 * in [req, d[0..n-1]], out [rsp] or, for an id with an output,
 * [rsp, {o, o_size}]. *o_len and *rsp (each may be NULL) get the
 * output length and rsp on success, 0 on failure.
 */
psa_status_t psa_client_crypto_call(int32_t fn,
                                    const struct psa_crypto_req *req,
                                    const psa_invec *d, size_t n, void *o,
                                    size_t o_size, size_t *o_len,
                                    struct psa_crypto_rsp *rsp);

/* A setup id: *h = the new op handle on success */
psa_status_t psa_client_crypto_setup(int32_t fn, uint32_t key,
                                     uint32_t alg, psa_client_handle_t *h);

/* An id with one data vector and no output on op handle h */
psa_status_t psa_client_crypto_op_in(int32_t fn, uint32_t h, const void *p,
                                     size_t n);

/*
 * An "end" id on *h: *h = 0 once the SPE service ran the call; a
 * refusal by the client or the SPM keeps *h for an abort.
 */
psa_status_t psa_client_crypto_end(int32_t fn, psa_client_handle_t *h,
                                   const psa_invec *d, size_t n, void *o,
                                   size_t o_size, size_t *o_len);

/* psa_client_crypto_end with the caller's req; *rsp may be NULL */
psa_status_t psa_client_crypto_end_req(int32_t fn, psa_client_handle_t *h,
                                       const struct psa_crypto_req *r,
                                       const psa_invec *d, size_t n,
                                       void *o, size_t o_size,
                                       size_t *o_len,
                                       struct psa_crypto_rsp *rsp);
