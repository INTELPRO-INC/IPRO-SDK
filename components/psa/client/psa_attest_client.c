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
 * NSPE PSA Initial Attestation client over psa_ipro/attest_rpc.h
 * (PSA_HANDLE_ATTEST): the SPE builds and signs the token.
 */
#include "psa_client_priv.h"
#include "psa_ipro/attest_rpc.h"

static int attest_challenge_ok(size_t n)
{
    return n == PSA_INITIAL_ATTEST_CHALLENGE_SIZE_32 ||
           n == PSA_INITIAL_ATTEST_CHALLENGE_SIZE_48 ||
           n == PSA_INITIAL_ATTEST_CHALLENGE_SIZE_64;
}

psa_status_t psa_initial_attest_get_token_size(size_t challenge_size,
                                               size_t *token_size)
{
    struct attest_req r = { (uint32_t)challenge_size };
    struct attest_rsp rsp = { 0 };
    psa_invec in = { &r, sizeof(r) };
    psa_outvec out = { &rsp, sizeof(rsp) };
    psa_status_t st = PSA_ERROR_INVALID_ARGUMENT;

    if (attest_challenge_ok(challenge_size)) {
        st = psa_client_call(PSA_HANDLE_ATTEST, ATTEST_FN_GET_TOKEN_SIZE,
                             &in, 1, &out, 1);
    }
    *token_size = st == PSA_SUCCESS ? rsp.token_size : 0;
    return st;
}

/*
 * GET_TOKEN_SIZE first: a smaller buffer gives BUFFER_TOO_SMALL with
 * *token_size = size needed; a 0-byte one INVALID_ARGUMENT, no call
 */
psa_status_t psa_initial_attest_get_token(const uint8_t *auth_challenge,
                                          size_t challenge_size,
                                          uint8_t *token_buf,
                                          size_t token_buf_size,
                                          size_t *token_size)
{
    struct attest_req r = { (uint32_t)challenge_size };
    struct attest_rsp rsp = { 0 };
    psa_invec in[2] = { { &r, sizeof(r) },
                        { auth_challenge, challenge_size } };
    psa_outvec out[2] = { { &rsp, sizeof(rsp) }, { token_buf, 0 } };
    size_t need = 0;
    psa_status_t st;

    if (token_buf_size == 0) {
        *token_size = 0;
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    st = psa_initial_attest_get_token_size(challenge_size, &need);
    if (st == PSA_SUCCESS && token_buf_size < need) {
        *token_size = need;
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }
    if (st == PSA_SUCCESS) {
        out[1].len = psa_client_cap(token_buf_size, sizeof(rsp));
        st = psa_client_call(PSA_HANDLE_ATTEST, ATTEST_FN_GET_TOKEN, in, 2,
                             out, 2);
    }
    *token_size = st == PSA_SUCCESS ? out[1].len : 0;
    return st;
}
