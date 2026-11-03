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
 * SPE psa_its_* of the mbedtls PSA key store on its_store ITS_NS_KEY.
 * Without MBEDTLS_PSA_ITS_FILE_C psa_crypto_storage.c calls them
 * through psa/internal_trusted_storage.h, the header used here.
 */
#include "its_store.h"
#include "psa/crypto.h"
#include "psa/internal_trusted_storage.h"

#if defined(MBEDTLS_PSA_ITS_FILE_C)
#error "MBEDTLS_PSA_ITS_FILE_C: mbedtls would use psa_crypto_its.h"
#endif

_Static_assert(sizeof(struct psa_storage_info_t) == 12,
               "PSA Storage 1.0 psa_storage_info_t");
_Static_assert(PSA_STORAGE_FLAG_WRITE_ONCE == ITS_FLAG_WRITE_ONCE,
               "WRITE_ONCE bit");

psa_status_t psa_its_set(psa_storage_uid_t uid, uint32_t data_length,
                         const void *p_data,
                         psa_storage_create_flags_t create_flags)
{
    return its_store_set(ITS_NS_KEY, uid, p_data, data_length,
                         create_flags);
}

psa_status_t psa_its_get(psa_storage_uid_t uid, uint32_t data_offset,
                         uint32_t data_length, void *p_data,
                         size_t *p_data_length)
{
    uint32_t n = 0;
    psa_status_t st = its_store_get(ITS_NS_KEY, uid, data_offset, p_data,
                                    data_length, &n, NULL);

    if (p_data_length != NULL) {
        *p_data_length = n;
    }
    return st;
}

psa_status_t psa_its_get_info(psa_storage_uid_t uid,
                              struct psa_storage_info_t *p_info)
{
    struct its_info info = { 0, 0 };
    psa_status_t st;

    if (p_info == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    st = its_store_info(ITS_NS_KEY, uid, &info);
    p_info->capacity = info.size;
    p_info->size = info.size;
    p_info->flags = info.flags;
    return st;
}

psa_status_t psa_its_remove(psa_storage_uid_t uid)
{
    return its_store_remove(ITS_NS_KEY, uid);
}

uint32_t psa_its_get_support(void)
{
    return 0;
}
