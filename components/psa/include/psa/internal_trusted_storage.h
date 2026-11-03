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
 * PSA Internal Trusted Storage API 1.0. In the SPE the mbedtls key
 * store calls it (its_mbedtls_adapter.c, KEY namespace); the NSPE
 * client implements it over psa_ipro/its_rpc.h.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#include "psa/error.h"

#define PSA_ITS_API_VERSION_MAJOR 1
#define PSA_ITS_API_VERSION_MINOR 0

typedef uint64_t psa_storage_uid_t;
typedef uint32_t psa_storage_create_flags_t;

struct psa_storage_info_t {
    uint32_t capacity;
    uint32_t size;
    psa_storage_create_flags_t flags;
};

#define PSA_STORAGE_FLAG_NONE                 0u
#define PSA_STORAGE_FLAG_WRITE_ONCE           (1u << 0)
#define PSA_STORAGE_FLAG_NO_CONFIDENTIALITY   (1u << 1)
#define PSA_STORAGE_FLAG_NO_REPLAY_PROTECTION (1u << 2)

#define PSA_STORAGE_SUPPORT_SET_EXTENDED      (1u << 0)

/* Largest object of this ITS, bytes */
#define PSA_ITS_MAX_OBJECT 2048u

psa_status_t psa_its_set(psa_storage_uid_t uid, uint32_t data_length,
                         const void *p_data,
                         psa_storage_create_flags_t create_flags);
psa_status_t psa_its_get(psa_storage_uid_t uid, uint32_t data_offset,
                         uint32_t data_length, void *p_data,
                         size_t *p_data_length);
psa_status_t psa_its_get_info(psa_storage_uid_t uid,
                              struct psa_storage_info_t *p_info);
psa_status_t psa_its_remove(psa_storage_uid_t uid);

/* PSA_STORAGE_SUPPORT_* bits of this ITS: 0 (no set_extended) */
uint32_t psa_its_get_support(void);
