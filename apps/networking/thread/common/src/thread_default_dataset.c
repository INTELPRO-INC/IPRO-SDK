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
 * @file thread_default_dataset.c
 * @brief See thread_default_dataset.h header for behaviour + Kconfig keys.
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <generated/autoconf.h>

#include <openthread/instance.h>
#include <openthread/dataset.h>
#include <openthread/thread.h>
#include <openthread/error.h>

#include "thread_default_dataset.h"
#include "ipro_log.h"

#define LOG_TAG "THREAD_DSET"

/* ---- Defaults (override via app .config) ----------------------------- */

#ifndef CONFIG_THREAD_DEFAULT_NETWORK_NAME
#define CONFIG_THREAD_DEFAULT_NETWORK_NAME       "IPRO-Thread"
#endif
#ifndef CONFIG_THREAD_DEFAULT_CHANNEL
#define CONFIG_THREAD_DEFAULT_CHANNEL            15
#endif
#ifndef CONFIG_THREAD_DEFAULT_PAN_ID
#define CONFIG_THREAD_DEFAULT_PAN_ID             0x1234
#endif
/* 8 bytes / 16 hex chars */
#ifndef CONFIG_THREAD_DEFAULT_EXT_PAN_ID
#define CONFIG_THREAD_DEFAULT_EXT_PAN_ID         "dead00beef00cafe"
#endif
/* 16 bytes / 32 hex chars */
#ifndef CONFIG_THREAD_DEFAULT_NETWORK_KEY
#define CONFIG_THREAD_DEFAULT_NETWORK_KEY        "00112233445566778899aabbccddeeff"
#endif
/* 8 bytes / 16 hex chars (mesh-local /64 prefix) */
#ifndef CONFIG_THREAD_DEFAULT_MESH_LOCAL_PREFIX
#define CONFIG_THREAD_DEFAULT_MESH_LOCAL_PREFIX  "fd11111122000000"
#endif
/* Commissioner passphrase — the PSKc is derived from this + network name +
 * ext pan id. A fixed value makes every node compute the same PSKc, which the
 * MeshCoP commissioner needs to build the JoinerEntrust. */
#ifndef CONFIG_THREAD_DEFAULT_COMMISSIONER_PASSPHRASE
#define CONFIG_THREAD_DEFAULT_COMMISSIONER_PASSPHRASE "IPRO-THREAD"
#endif

/* ---- Hex decode helper --------------------------------------------- */

static int hex_nibble(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

static int hex_decode(const char *hex, uint8_t *out, size_t out_len)
{
    for (size_t i = 0; i < out_len; i++) {
        int hi = hex_nibble(hex[i * 2]);
        int lo = hex_nibble(hex[i * 2 + 1]);
        if (hi < 0 || lo < 0) {
            IPRO_LOGE(LOG_TAG, "bad hex at byte %u: '%c%c'",
                      (unsigned)i, hex[i * 2], hex[i * 2 + 1]);
            return -1;
        }
        out[i] = (uint8_t)((hi << 4) | lo);
    }
    /* require exact length — reject longer strings */
    if (hex[out_len * 2] != '\0') {
        IPRO_LOGE(LOG_TAG, "hex string too long, expected %u bytes",
                  (unsigned)out_len);
        return -1;
    }
    return 0;
}

/* ---- API ----------------------------------------------------------- */

int thread_apply_default_dataset(otInstance *aInstance)
{
    /* Standard Thread persistent behaviour:
     *   - If the device already has a committed Active Dataset in NVDS,
     *     keep it (so MLE re-attach uses the same network key, panid,
     *     channel, key sequence as before the reboot).
     *   - Only on a truly virgin device (first flash, or after an
     *     explicit `ot factoryreset`) do we apply the compiled-in
     *     default dataset.
     *
     * To wipe state explicitly during development use:
     *     ot factoryreset       (CLI — triggers POR reboot)
     */
    if (otDatasetIsCommissioned(aInstance)) {
        IPRO_LOGI(LOG_TAG, "dataset already commissioned, keeping existing");
        return 0;
    }

    IPRO_LOGI(LOG_TAG, "applying compiled-in defaults (first boot)");

    otOperationalDataset ds;
    memset(&ds, 0, sizeof(ds));

    ds.mActiveTimestamp.mSeconds       = 1;
    ds.mActiveTimestamp.mTicks         = 0;
    ds.mActiveTimestamp.mAuthoritative = false;
    ds.mComponents.mIsActiveTimestampPresent = true;

    ds.mChannel = (uint16_t)CONFIG_THREAD_DEFAULT_CHANNEL;
    ds.mComponents.mIsChannelPresent = true;

    ds.mPanId = (uint16_t)CONFIG_THREAD_DEFAULT_PAN_ID;
    ds.mComponents.mIsPanIdPresent = true;

    if (hex_decode(CONFIG_THREAD_DEFAULT_EXT_PAN_ID,
                   ds.mExtendedPanId.m8, sizeof(ds.mExtendedPanId.m8)) != 0) {
        IPRO_LOGE(LOG_TAG, "EXT_PAN_ID '%s' invalid",
                  CONFIG_THREAD_DEFAULT_EXT_PAN_ID);
        return -1;
    }
    ds.mComponents.mIsExtendedPanIdPresent = true;

    if (hex_decode(CONFIG_THREAD_DEFAULT_NETWORK_KEY,
                   ds.mNetworkKey.m8, sizeof(ds.mNetworkKey.m8)) != 0) {
        IPRO_LOGE(LOG_TAG, "NETWORK_KEY invalid (must be 32 hex chars)");
        return -1;
    }
    ds.mComponents.mIsNetworkKeyPresent = true;

    /* Network name — strncpy into the embedded char array. */
    {
        const char *name = CONFIG_THREAD_DEFAULT_NETWORK_NAME;
        size_t      n    = strlen(name);
        if (n >= sizeof(ds.mNetworkName.m8)) {
            n = sizeof(ds.mNetworkName.m8) - 1;
        }
        memcpy(ds.mNetworkName.m8, name, n);
        ds.mNetworkName.m8[n] = '\0';
    }
    ds.mComponents.mIsNetworkNamePresent = true;

    if (hex_decode(CONFIG_THREAD_DEFAULT_MESH_LOCAL_PREFIX,
                   ds.mMeshLocalPrefix.m8, sizeof(ds.mMeshLocalPrefix.m8)) != 0) {
        IPRO_LOGE(LOG_TAG, "MESH_LOCAL_PREFIX invalid");
        return -1;
    }
    ds.mComponents.mIsMeshLocalPrefixPresent = true;

    /* Channel mask, security policy, and PSKc complete the dataset. These three
     * are REQUIRED by the MeshCoP commissioner: PrepareJoinerEntrustMessage()
     * builds the JoinerEntrust (which delivers the network key to a joiner) from
     * the active dataset and returns InvalidState — silently sending nothing —
     * if ChannelMask, SecurityPolicy, OR Pskc is absent. Without them a joiner
     * completes DTLS/EC-JPAKE + JoinerFinalize but then times out in the Entrust
     * state because the network key never arrives. */
    ds.mChannelMask = (uint32_t)0x07FFF800u; /* 2.4 GHz channels 11-26 */
    ds.mComponents.mIsChannelMaskPresent = true;

    ds.mSecurityPolicy.mRotationTime                 = 672;
    ds.mSecurityPolicy.mObtainNetworkKeyEnabled      = true;
    ds.mSecurityPolicy.mNativeCommissioningEnabled   = true;
    ds.mSecurityPolicy.mRoutersEnabled               = true;
    ds.mSecurityPolicy.mExternalCommissioningEnabled = true;
    ds.mComponents.mIsSecurityPolicyPresent = true;

#ifdef CONFIG_OPENTHREAD_FTD
    /* otDatasetGeneratePskc() only exists in the FTD library (OpenThread
     * guards it with OPENTHREAD_FTD). An MTD/SED never builds JoinerEntrust,
     * so it does not need the PSKc in its local dataset. */
    {
        otError pErr = otDatasetGeneratePskc(CONFIG_THREAD_DEFAULT_COMMISSIONER_PASSPHRASE,
                                             &ds.mNetworkName, &ds.mExtendedPanId, &ds.mPskc);
        if (pErr != OT_ERROR_NONE) {
            IPRO_LOGE(LOG_TAG, "otDatasetGeneratePskc failed: %d", pErr);
            return -1;
        }
        ds.mComponents.mIsPskcPresent = true;
    }
#endif

    otError err = otDatasetSetActive(aInstance, &ds);
    if (err != OT_ERROR_NONE) {
        IPRO_LOGE(LOG_TAG, "otDatasetSetActive failed: %d", err);
        return -1;
    }

    IPRO_LOGI(LOG_TAG,
              "applied: name='%s' ch=%u panid=0x%04x",
              CONFIG_THREAD_DEFAULT_NETWORK_NAME,
              (unsigned)CONFIG_THREAD_DEFAULT_CHANNEL,
              (unsigned)CONFIG_THREAD_DEFAULT_PAN_ID);
    return 0;
}
