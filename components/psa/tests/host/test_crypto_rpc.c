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
 * Layout of the crypto RPC structs. Built once with the client
 * mbedtls config (-DRPC_CLIENT) and once with the SPE config
 * (-DRPC_SERVER); the Makefile compares the two "layout" lines.
 */
#include <stddef.h>

#include "check.h"
#include "psa_ipro/crypto_rpc.h"

#if defined(MBEDTLS_PSA_CRYPTO_C)
#define MODE_SERVER 1
#define MODE_CLIENT 0
#elif defined(MBEDTLS_PSA_CRYPTO_CLIENT)
#define MODE_SERVER 0
#define MODE_CLIENT 1
#else
#define MODE_SERVER 0
#define MODE_CLIENT 0
#endif

int main(void)
{
    printf("layout req=%zu attr_off=%zu attr=%zu rsp=%zu\n",
           sizeof(struct psa_crypto_req),
           offsetof(struct psa_crypto_req, attr),
           sizeof(psa_key_attributes_t), sizeof(struct psa_crypto_rsp));

#if defined(RPC_CLIENT)
    check(MODE_CLIENT, "client mode: CLIENT on, CRYPTO_C off");
#else
    check(MODE_SERVER, "server mode: MBEDTLS_PSA_CRYPTO_C on");
#endif
    check(offsetof(struct psa_crypto_req, attr) == 24u,
          "attr follows the six u32 fields");
    check(sizeof(struct psa_crypto_attr) == 28u &&
          sizeof(struct psa_crypto_req) == 52u,
          "wire attr 28 bytes, req 52 bytes");
    check(sizeof(mbedtls_svc_key_id_t) == sizeof(uint32_t),
          "key ids are plain u32 (no owner encoding)");
    check(CRYPTO_FN_INIT == 1 && CRYPTO_FN_KEY_PURGE == 9 &&
          CRYPTO_FN_HASH_CLONE == 17 && CRYPTO_FN_MAC_ABORT == 25 &&
          CRYPTO_FN_CIPHER_ABORT == 34 && CRYPTO_FN_AEAD_ABORT == 46 &&
          CRYPTO_FN_ASYM_DECRYPT == 52 && CRYPTO_FN_KD_ABORT == 61 &&
          CRYPTO_FN_GENERATE_RANDOM == 63 &&
          CRYPTO_FN_KD_INPUT_INTEGER == 64 &&
          CRYPTO_FN_KEY_AGREEMENT == 65 && CRYPTO_FN_PAKE_SETUP == 66 &&
          CRYPTO_FN_PAKE_ABORT == 74 && CRYPTO_FN_LAST == 74,
          "function ids 1..74");
    check(sizeof(struct psa_crypto_pake_suite) == 12u,
          "wire PAKE suite 12 bytes");
    check(PSA_CRYPTO_OP_SLOTS == 8u, "8 operation slots");
    return check_summary();
}
