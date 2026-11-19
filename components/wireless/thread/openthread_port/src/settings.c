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

/**
 * @file settings.c
 * @brief OpenThread Settings (NVS) platform layer for IPRO7
 * 
 * This file provides non-volatile storage for OpenThread network credentials
 * and configuration data using EasyFlash for persistent storage.
 */

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <openthread/platform/settings.h>
#include <ef_def.h>
#include <easyflash.h>
#include "ipro_log.h"

//=============================================================================
// Settings Storage Configuration
//=============================================================================

#define OT_MAX_KEY_LEN              20
#define OT_MAX_ENTRY_NUM            1024
#define OT_MAX_ENTRY_BITMAP         ((OT_MAX_ENTRY_NUM + 31) / 32)

//=============================================================================
// Platform Settings Implementation
//=============================================================================

void otPlatSettingsInit(otInstance *aInstance, const uint16_t *aSensitiveKeys, uint16_t aSensitiveKeysLength)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aSensitiveKeys);
    OT_UNUSED_VARIABLE(aSensitiveKeysLength);
    
    easyflash_init();
}

void otPlatSettingsDeinit(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
}

void otPlatSettingsSetCriticalKeys(otInstance *aInstance, const uint16_t *aKeys, uint16_t aKeysLength)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aKeys);
    OT_UNUSED_VARIABLE(aKeysLength);
}

otError otPlatSettingsGet(otInstance *aInstance, uint16_t aKey, int aIndex, 
                          uint8_t *aValue, uint16_t *aValueLength)
{
    OT_UNUSED_VARIABLE(aInstance);
    
    char            key[OT_MAX_KEY_LEN];
    uint32_t        bitmapArray[OT_MAX_ENTRY_BITMAP];
    size_t          copied = 0, total = 0;
    const size_t    capacity = (aValue && aValueLength) ? *aValueLength : 0;
    otError         ret = OT_ERROR_FAILED;
    EfErrCode       error;

    if (aValueLength) *aValueLength = 0;
    if (aValue && !aValueLength) return OT_ERROR_INVALID_ARGS;
    if (aValue && capacity) memset(aValue, 0, capacity);
    if (aIndex < 0 || aIndex >= OT_MAX_ENTRY_NUM) return OT_ERROR_NOT_FOUND;

    /* Existing ENV port uses a recursive mutex. Keep bitmap and value under
     * one outer lock; checked lookups bypass legacy caches and never repair
     * interrupted records. A missing value behind a set bit is corruption,
     * not permission to erase the bit or return a fresh-network default. */
    ef_port_env_lock();
    do {
        snprintf(key, sizeof(key), "ot-bm-%x", aKey);
        error = ef_env_get_checked(key, 0, bitmapArray, sizeof(bitmapArray), &copied, &total);
        if (error == EF_ENV_NOT_FOUND) { ret = OT_ERROR_NOT_FOUND; break; }
        if (error != EF_NO_ERR || copied != sizeof(bitmapArray) || total != sizeof(bitmapArray)) break;
        const unsigned i = (unsigned)aIndex >> 5;
        const unsigned j = (unsigned)aIndex & 0x1f;
        if (!(bitmapArray[i] & (UINT32_C(1) << j))) { ret = OT_ERROR_NOT_FOUND; break; }

        snprintf(key, sizeof(key), "ot-%x-%x", aKey, (unsigned)aIndex);
        error = ef_env_get_checked(key, 0, aValue, capacity, &copied, &total);
        if (error != EF_NO_ERR || total > UINT16_MAX) break;
        if (aValueLength) *aValueLength = (uint16_t)total;
        ret = OT_ERROR_NONE; // Short reads intentionally report the full length.
    } while (0);
    ef_port_env_unlock();
    if (ret != OT_ERROR_NONE && aValue) {
        volatile uint8_t *p = aValue;
        for (size_t n = 0; n < capacity; ++n) p[n] = 0;
    }
    return ret;
}

static otError otPlatSettingsSet_raw(otInstance *aInstance, uint16_t aKey, const uint8_t *aValue, 
                                     uint16_t aValueLength, int index, char *key, uint32_t *bitmapArray) 
{
    OT_UNUSED_VARIABLE(aInstance);
    
    if ((!aValue && aValueLength) || index < 0 || index >= OT_MAX_ENTRY_NUM)
        return OT_ERROR_INVALID_ARGS;
    EfErrCode   ret;
    uint32_t i = index >> 5;
    uint32_t j = index & 0x1f;

    bitmapArray[i] = bitmapArray[i] | (UINT32_C(1) << j);

    ef_port_env_lock();
    do {
        sprintf(key, "ot-%x-%x", aKey, index);
        /* Read-back checked writes detect silent/partial program failures.
         * Value and bitmap are separate records, NOT an atomic transaction;
         * any error leaves persistence uncertain and must reach the caller. */
        ret = ef_env_set_checked(key, aValue, aValueLength);

        if (EF_NO_ERR == ret) {
            sprintf(key, "ot-bm-%x", aKey);
            ret = ef_env_set_checked(key, bitmapArray, sizeof(uint32_t) * OT_MAX_ENTRY_BITMAP);
        }
    } while (0);
    ef_port_env_unlock();

    return (EF_NO_ERR == ret) ? OT_ERROR_NONE : OT_ERROR_FAILED;
}

otError otPlatSettingsSet(otInstance *aInstance, uint16_t aKey, 
                          const uint8_t *aValue, uint16_t aValueLength)
{
    char            key[OT_MAX_KEY_LEN];
    uint32_t        bitmapArray[OT_MAX_ENTRY_BITMAP];

    memset(bitmapArray, 0, OT_MAX_ENTRY_BITMAP * 4);
    return otPlatSettingsSet_raw(aInstance, aKey, aValue, aValueLength, 0, key, bitmapArray);
}

otError otPlatSettingsAdd(otInstance *aInstance, uint16_t aKey,
                          const uint8_t *aValue, uint16_t aValueLength)
{
    char        key[OT_MAX_KEY_LEN];
    uint32_t    bitmapArray[OT_MAX_ENTRY_BITMAP];
    size_t      len = 0, index = 0, i, j, entryNum;
    otError     ret = OT_ERROR_NONE;

    ef_port_env_lock();

    do {
        sprintf(key, "ot-bm-%x", aKey);
        ef_get_env_blob(key, bitmapArray, OT_MAX_ENTRY_BITMAP * 4, (size_t *)&len);
        if (len != OT_MAX_ENTRY_BITMAP * 4) {
            memset(bitmapArray, 0, OT_MAX_ENTRY_BITMAP * 4);
            ret = otPlatSettingsSet_raw(aInstance, aKey, aValue, aValueLength, 0, key, bitmapArray);
        }
        else {
            // Length value read from flash should be aligned with 4 bytes
            entryNum = len * 8;
            for (index = 0; index < entryNum; index++) {
                i = index >> 5;
                j = index & 0x1f;
                if ((bitmapArray[i] & (1 << j)) == 0) {
                    // Found empty index
                    break;
                }
            }

            if (index == entryNum) {
                ret = OT_ERROR_NO_BUFS;
                break;
            }

            ret = otPlatSettingsSet_raw(aInstance, aKey, aValue, aValueLength, index, key, bitmapArray);
        }
    } while (0);

    ef_port_env_unlock();

    return ret;
}

otError otPlatSettingsDelete(otInstance *aInstance, uint16_t aKey, int aIndex)
{
    OT_UNUSED_VARIABLE(aInstance);
    
    char        key[OT_MAX_KEY_LEN];
    uint32_t    bitmapArray[OT_MAX_ENTRY_BITMAP];
    size_t      i, j, len;
    otError     ret = OT_ERROR_NONE;

    ef_port_env_lock();

    sprintf(key, "ot-bm-%x", aKey);
    do {
        if (aIndex < 0) {
            /* Dataset rollback needs a verified result. Hide the bitmap first,
             * then remove only this key's values (including interrupted-write
             * orphans). A failure remains retryable and must reach the caller.
             * This is logical deletion, not a secure physical flash erase. */
            EfErrCode error = ef_env_del_checked(key);
            if (error == EF_ENV_NOT_FOUND) error = EF_NO_ERR;
            if (error == EF_NO_ERR) {
                snprintf(key, sizeof(key), "ot-%x-", aKey);
                error = ef_env_del_prefix_checked(key);
            }
            ret = error == EF_NO_ERR ? OT_ERROR_NONE : OT_ERROR_FAILED;
            break;
        }

        ef_get_env_blob(key, bitmapArray, OT_MAX_ENTRY_BITMAP * 4, (size_t *)&len);
        if (len != OT_MAX_ENTRY_BITMAP * 4) {
            ret = OT_ERROR_NOT_FOUND;
            break;
        }

        i = aIndex >> 5;
        j = aIndex & 0x1f;
        if (!(bitmapArray[i] & (1 << j))) {
            ret = OT_ERROR_NOT_FOUND;
            break;
        }

        bitmapArray[i] = bitmapArray[i] & (~(1 << j));

        sprintf(key, "ot-bm-%x", aKey);
        ef_set_env_blob(key, bitmapArray, OT_MAX_ENTRY_BITMAP * 4);

        sprintf(key, "ot-%x-%x", aKey, aIndex);
        ef_del_env(key);

    } while (0);

    ef_port_env_unlock();

    return ret;
}

void otPlatSettingsWipe(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    /* Delete only OT-owned records. The shared store also contains Matter
     * factory data and device identity; never format it or save/restore a
     * partial whitelist. This is logical deletion, not secure erasure.
     * OT serializes settings users; a failed wipe can be partially complete.
     * Coordinators requiring a result must use the checked storage API. */
    const EfErrCode error = ef_env_del_prefix_checked("ot-");
    if (error != EF_NO_ERR) {
        IPRO_LOGE("OT-Settings", "OT settings wipe failed: %d", (int)error);
        return;
    }
    IPRO_LOGI("OT-Settings", "OT settings wiped (other namespaces preserved)");
}
