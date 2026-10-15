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
 * OTA package signature block — see ota_sig.h.
 */
#include "ota_sig.h"

#include <string.h>

#include "mbedtls/bignum.h"
#include "mbedtls/ecdsa.h"
#include "mbedtls/ecp.h"
#include "mbedtls/sha256.h"

static uint32_t rd32(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

int ota_sig_parse(const uint8_t hdr[512], ota_sig_info_t *out)
{
    if (memcmp(&hdr[OTA_SIG_OFFSET], OTA_SIG_MAGIC, 4) != 0) return OTA_SIG_ABSENT;
    out->product_id   = rd32(&hdr[100]);
    out->hw_id        = rd32(&hdr[104]);
    out->fw_version   = rd32(&hdr[108]);
    out->unpacked_len = rd32(&hdr[112]);
    out->key_id       = rd32(&hdr[116]);
    return OTA_SIG_OK;
}

static const ota_pubkey_t *find_key(uint32_t key_id, const ota_pubkey_t *keys,
                                    size_t nkeys)
{
    for (size_t i = 0; i < nkeys; i++) {
        if (keys[i].key_id == key_id) return &keys[i];
    }
    return NULL;
}

int ota_sig_check_policy(const uint8_t hdr[512], uint32_t product_id,
                         uint32_t hw_id, uint32_t cur_version,
                         uint32_t exec_slot_len,
                         const ota_pubkey_t *keys, size_t nkeys)
{
    ota_sig_info_t inf;
    if (ota_sig_parse(hdr, &inf) != OTA_SIG_OK) return OTA_SIG_ABSENT;
    if (inf.product_id != product_id) return OTA_SIG_BAD_PRODUCT;
    if (inf.hw_id != 0 && inf.hw_id != hw_id) return OTA_SIG_BAD_HW;
    if (inf.fw_version < cur_version) return OTA_SIG_ROLLBACK;
    if (inf.unpacked_len == 0 || inf.unpacked_len > exec_slot_len) return OTA_SIG_TOO_BIG;
    if (find_key(inf.key_id, keys, nkeys) == NULL) return OTA_SIG_UNKNOWN_KEY;
    return OTA_SIG_OK;
}

int ota_sig_verify(const uint8_t hdr[512], const ota_pubkey_t *keys, size_t nkeys)
{
    ota_sig_info_t inf;
    if (ota_sig_parse(hdr, &inf) != OTA_SIG_OK) return OTA_SIG_ABSENT;
    const ota_pubkey_t *key = find_key(inf.key_id, keys, nkeys);
    if (key == NULL) return OTA_SIG_UNKNOWN_KEY;

    uint8_t hash[32];
    if (mbedtls_sha256_ret(hdr, OTA_SIG_SIGNED_LEN, hash, 0) != 0) return OTA_SIG_BAD_SIG;

    mbedtls_ecp_group grp;
    mbedtls_ecp_point q;
    mbedtls_mpi r, s;
    mbedtls_ecp_group_init(&grp);
    mbedtls_ecp_point_init(&q);
    mbedtls_mpi_init(&r);
    mbedtls_mpi_init(&s);

    int rc = OTA_SIG_BAD_SIG;
    if (mbedtls_ecp_group_load(&grp, MBEDTLS_ECP_DP_SECP256R1) == 0 &&
        mbedtls_ecp_point_read_binary(&grp, &q, key->pub, sizeof(key->pub)) == 0 &&
        mbedtls_ecp_check_pubkey(&grp, &q) == 0 &&
        mbedtls_mpi_read_binary(&r, &hdr[OTA_SIG_RS_OFFSET], 32) == 0 &&
        mbedtls_mpi_read_binary(&s, &hdr[OTA_SIG_RS_OFFSET + 32], 32) == 0 &&
        mbedtls_ecdsa_verify(&grp, hash, sizeof(hash), &q, &r, &s) == 0) {
        rc = OTA_SIG_OK;
    }

    mbedtls_mpi_free(&s);
    mbedtls_mpi_free(&r);
    mbedtls_ecp_point_free(&q);
    mbedtls_ecp_group_free(&grp);
    return rc;
}
