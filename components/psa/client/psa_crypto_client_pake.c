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
 * NSPE PSA Crypto client: PAKE (PSA Crypto API 1.2). The SPE runs it
 * on TF-PSA-Crypto; other libraries have no PAKE in the client.
 */
#include <string.h>

#include "psa_crypto_client_priv.h"

#if PSA_IPRO_API_1_2
psa_status_t psa_pake_setup(psa_pake_operation_t *operation,
                            mbedtls_svc_key_id_t password_key,
                            const psa_pake_cipher_suite_t *cipher_suite)
{
    struct psa_crypto_req r = psa_client_crypto_req(password_key, 0,
                                                    PSA_CLIENT_H(operation));
    struct psa_crypto_pake_suite w;
    psa_invec d = { &w, sizeof(w) };
    struct psa_crypto_rsp rsp;
    psa_status_t st;

    w.alg = psa_pake_cs_get_algorithm(cipher_suite);
    w.primitive = psa_pake_cs_get_primitive(cipher_suite);
    w.key_confirmation = psa_pake_cs_get_key_confirmation(cipher_suite);
    st = psa_client_crypto_call(CRYPTO_FN_PAKE_SETUP, &r, &d, 1, NULL, 0,
                                NULL, &rsp);
    if (st == PSA_SUCCESS) {
        PSA_CLIENT_H(operation) = rsp.op_handle;
    }
    return st;
}

psa_status_t psa_pake_set_role(psa_pake_operation_t *operation,
                               psa_pake_role_t role)
{
    struct psa_crypto_req r = psa_client_crypto_req(0, 0,
                                                    PSA_CLIENT_H(operation));

    r.aux = role;
    return psa_client_crypto_call(CRYPTO_FN_PAKE_SET_ROLE, &r, NULL, 0, NULL,
                                  0, NULL, NULL);
}

psa_status_t psa_pake_set_user(psa_pake_operation_t *operation,
                               const uint8_t *user_id, size_t user_id_len)
{
    return psa_client_crypto_op_in(CRYPTO_FN_PAKE_SET_USER,
                                   PSA_CLIENT_H(operation), user_id,
                                   user_id_len);
}

psa_status_t psa_pake_set_peer(psa_pake_operation_t *operation,
                               const uint8_t *peer_id, size_t peer_id_len)
{
    return psa_client_crypto_op_in(CRYPTO_FN_PAKE_SET_PEER,
                                   PSA_CLIENT_H(operation), peer_id,
                                   peer_id_len);
}

psa_status_t psa_pake_set_context(psa_pake_operation_t *operation,
                                  const uint8_t *context, size_t context_len)
{
    return psa_client_crypto_op_in(CRYPTO_FN_PAKE_SET_CONTEXT,
                                   PSA_CLIENT_H(operation), context,
                                   context_len);
}

psa_status_t psa_pake_output(psa_pake_operation_t *operation,
                             psa_pake_step_t step, uint8_t *output,
                             size_t output_size, size_t *output_length)
{
    struct psa_crypto_req r = psa_client_crypto_req(0, 0,
                                                    PSA_CLIENT_H(operation));

    r.step = step;
    return psa_client_crypto_call(CRYPTO_FN_PAKE_OUTPUT, &r, NULL, 0, output,
                                  output_size, output_length, NULL);
}

psa_status_t psa_pake_input(psa_pake_operation_t *operation,
                            psa_pake_step_t step, const uint8_t *input,
                            size_t input_length)
{
    struct psa_crypto_req r = psa_client_crypto_req(0, 0,
                                                    PSA_CLIENT_H(operation));
    psa_invec d = { input, input_length };

    r.step = step;
    return psa_client_crypto_call(CRYPTO_FN_PAKE_INPUT, &r, &d, 1, NULL, 0,
                                  NULL, NULL);
}

psa_status_t psa_pake_get_shared_key(psa_pake_operation_t *operation,
                                     const psa_key_attributes_t *attributes,
                                     mbedtls_svc_key_id_t *key)
{
    psa_client_handle_t h = PSA_CLIENT_H(operation);
    struct psa_crypto_req r = psa_client_crypto_req(0, 0, h);
    struct psa_crypto_rsp rsp;
    psa_status_t st;

    psa_crypto_attr_pack(&r.attr, attributes);
    st = psa_client_crypto_end_req(CRYPTO_FN_PAKE_GET_SHARED_KEY, &h, &r,
                                   NULL, 0, NULL, 0, NULL, &rsp);
    PSA_CLIENT_H(operation) = h;
    *key = rsp.key_id;
    return st;
}

psa_status_t psa_pake_abort(psa_pake_operation_t *operation)
{
    psa_client_handle_t h = PSA_CLIENT_H(operation);
    psa_status_t st = psa_client_crypto_end(CRYPTO_FN_PAKE_ABORT, &h, NULL,
                                            0, NULL, 0, NULL);

    PSA_CLIENT_H(operation) = h;
    return st;
}
#endif
