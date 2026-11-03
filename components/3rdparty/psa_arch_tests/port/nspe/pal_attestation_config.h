/** @file
 * Copyright (c) 2019-2025, Arm Limited or its affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
**/

/* Attestation suite settings, after tgt_dev_apis_tfm_an521/nspe/
 * pal_attestation_config.h; the IAK public key comes from the SPE
 * at run time (pal_attest_pk.c), not from a table here.
 */

#ifndef _PAL_ATTESTATION_CONFIG_H_
#define _PAL_ATTESTATION_CONFIG_H_

#define COSE_ALGORITHM_ES256             -7
#define COSE_ALG_SHA256_PROPRIETARY      -72000

#define USEFUL_BUF_MAKE_STACK_UB UsefulBuf_MAKE_STACK_UB

#define COSE_SIG_CONTEXT_STRING_SIGNATURE1 "Signature1"

/* Bytes reserved for the encoded COSE protected headers */
#define T_COSE_SIGN1_MAX_PROT_HEADER (1+1+5+9)

/* Bytes of the first, non-payload part of the CBOR encoded TBS */
#define T_COSE_SIZE_OF_TBS \
    1 + /* For opening the array */ \
    sizeof(COSE_SIG_CONTEXT_STRING_SIGNATURE1) + /* "Signature1" */ \
    2 + /* Overhead for encoding string */ \
    T_COSE_SIGN1_MAX_PROT_HEADER + /* entire protected headers */ \
    3 * (/* 3 NULL bstrs for fields not used */ \
        1 /* size of a NULL bstr */  \
    )
#define NULL_USEFUL_BUF_C  NULLUsefulBufC

#define ATTEST_PUBLIC_KEY_SLOT                  4
#define ECC_CURVE_SECP256R1_PULBIC_KEY_LENGTH   (1 + 2 * PSA_BITS_TO_BYTES(256))

#ifdef INITIAL_ATTESTATION
#include <stddef.h>
#include <stdint.h>
#include "psa/crypto.h"

/* IAK public key (SEC1 uncompressed P-256) read from the SPE */
int32_t tfm_initial_attest_get_public_key(uint8_t *public_key_buff,
                                          size_t public_key_buf_size,
                                          size_t *public_key_len,
                                          psa_ecc_family_t *elliptic_family_type);
#endif

#endif /* _PAL_ATTESTATION_CONFIG_H_ */
