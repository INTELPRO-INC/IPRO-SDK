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

/* ipro_sec driver: init and SHA-1/224/256/384/512 on the SEC_ENG. */
#include <string.h>

#include "psa/crypto.h"
#include "hal_sec_common_v2.h"
#include "ipro_sec_driver.h"
#include "ipro_sec_priv.h"

psa_status_t ipro_sec_status(int ret)
{
    switch (ret) {
    case HAL_SEC_V2_OK:
        return PSA_SUCCESS;
    case HAL_SEC_V2_ERR_PARAM:
        return PSA_ERROR_INVALID_ARGUMENT;
    case HAL_SEC_V2_ERR_VERIFY:
        return PSA_ERROR_INVALID_SIGNATURE;
    default:
        return PSA_ERROR_HARDWARE_FAILURE;
    }
}

psa_status_t ipro_sec_init(void)
{
    return ipro_sec_status(hal_sec_v2_init());
}

/* HAL type of alg, HAL_SEC_HASH_V2_TYPES when not accelerated */
static unsigned int ipro_sec_hash_type(psa_algorithm_t alg)
{
    switch (alg) {
#if defined(PSA_WANT_ALG_SHA_1)
    case PSA_ALG_SHA_1:
        return HAL_SEC_HASH_V2_SHA1;
#endif
#if defined(PSA_WANT_ALG_SHA_224)
    case PSA_ALG_SHA_224:
        return HAL_SEC_HASH_V2_SHA224;
#endif
#if defined(PSA_WANT_ALG_SHA_256)
    case PSA_ALG_SHA_256:
        return HAL_SEC_HASH_V2_SHA256;
#endif
#if defined(PSA_WANT_ALG_SHA_384)
    case PSA_ALG_SHA_384:
        return HAL_SEC_HASH_V2_SHA384;
#endif
#if defined(PSA_WANT_ALG_SHA_512)
    case PSA_ALG_SHA_512:
        return HAL_SEC_HASH_V2_SHA512;
#endif
    default:
        return HAL_SEC_HASH_V2_TYPES;
    }
}

psa_status_t ipro_sec_hash_setup(ipro_sec_hash_operation_t *operation,
                                 psa_algorithm_t alg)
{
    unsigned int type = ipro_sec_hash_type(alg);

    if (type == HAL_SEC_HASH_V2_TYPES) {
        return PSA_ERROR_NOT_SUPPORTED;
    }
    return ipro_sec_status(hal_sec_hash_v2_setup(operation, type));
}

psa_status_t ipro_sec_hash_clone(const ipro_sec_hash_operation_t *source,
                                 ipro_sec_hash_operation_t *target)
{
    memcpy(target, source, sizeof(*target));
    return PSA_SUCCESS;
}

psa_status_t ipro_sec_hash_update(ipro_sec_hash_operation_t *operation,
                                  const uint8_t *input, size_t input_length)
{
    return ipro_sec_status(
        hal_sec_hash_v2_update(operation, input, input_length));
}

psa_status_t ipro_sec_hash_finish(ipro_sec_hash_operation_t *operation,
                                  uint8_t *hash, size_t hash_size,
                                  size_t *hash_length)
{
    size_t n = hal_sec_hash_v2_digest_len(operation->type);

    *hash_length = 0;
    if (hash_size < n) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }
    return ipro_sec_status(
        hal_sec_hash_v2_finish(operation, hash, hash_size, hash_length));
}

psa_status_t ipro_sec_hash_abort(ipro_sec_hash_operation_t *operation)
{
    hal_sec_hash_v2_abort(operation);
    return PSA_SUCCESS;
}

psa_status_t ipro_sec_hash_compute(psa_algorithm_t alg,
                                   const uint8_t *input, size_t input_length,
                                   uint8_t *hash, size_t hash_size,
                                   size_t *hash_length)
{
    ipro_sec_hash_operation_t op;
    psa_status_t st = ipro_sec_hash_setup(&op, alg);

    if (st == PSA_SUCCESS) {
        st = ipro_sec_hash_update(&op, input, input_length);
    }
    if (st == PSA_SUCCESS) {
        st = ipro_sec_hash_finish(&op, hash, hash_size, hash_length);
    }
    ipro_sec_hash_abort(&op);
    return st;
}
