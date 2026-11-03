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
 * SPE platform service (PSA_HANDLE_PLATFORM) and the identity of this
 * boot it measures at init: UID, lifecycle, implementation id, SPE
 * hash, boot seed and the IAK the attestation token is signed with.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#include "psa/client.h"

struct svc_platform_id {
    uint8_t uid[8];
    uint32_t lifecycle;             /* PSA_LIFECYCLE_* */
    uint8_t impl_id[32];            /* SHA-256 of eFuse PK hash */
    uint8_t boot_seed[32];
    uint8_t spe_hash[32];           /* SHA-256 of the SPE image */
    uint8_t iak_pub[65];
    uint8_t instance_id[33];        /* 0x01 || SHA-256(iak_pub) */
    char signer_hex[65];            /* eFuse PK hash, hex */
};

/*
 * UID, lifecycle, PK and SPE image hashes, boot seed, a new IAK
 * (spm_fatal when no seed gives a valid scalar). Until it succeeds
 * there is no identity and no IAK.
 */
psa_status_t svc_platform_init(void);

/* Identity of this boot; NULL until svc_platform_init succeeded */
const struct svc_platform_id *svc_platform_id(void);

/* ES256 with the IAK: sig = r || s of the SHA-256 hash */
psa_status_t svc_platform_sign(const uint8_t hash[32], uint8_t sig[64]);

/* Reset hook: does nothing; the identity is per boot */
void svc_platform_client_reset(void);

/* spm_service_fn of PSA_HANDLE_PLATFORM, psa_ipro/platform_rpc.h */
psa_status_t svc_platform_call(int32_t type, const psa_invec *in,
                               size_t in_len, psa_outvec *out,
                               size_t out_len);
