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

#include <generated/autoconf.h>
#include "br_management.h"
#include "br_ble_protocol.h"
#include "br_wifi_profile.h"
#include "br_wifi_storage.h"
#include "br_factory_reset.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include <ipro_matter_wifi.h>
#include <ipro_matter_br_thread.h>
#include <openthread/thread.h>
#include <openthread/dataset_ftd.h>
#include <openthread/ip6.h>
#include "openthread_port.h"
#include <string.h>
static SemaphoreHandle_t sLock;
static bool sStorageUncertain, sWifiSaved;
int br_management_init(void) { sLock = xSemaphoreCreateMutex(); return sLock ? 0 : -1; }
static uint8_t ensure_thread(const uint8_t *input, size_t size)
{
#ifdef CONFIG_MATTER_ENABLE
    (void)input; (void)size;
    return BR_BLE_STATUS_BUSY; /* The local Matter server owns its network transaction. */
#else
    otInstance *instance = otrGetInstanceIfReady();
    if (!instance || !otrTryLock()) return BR_BLE_STATUS_BUSY;
    otError error = OT_ERROR_NONE;
    otOperationalDataset dataset = {0};
    otOperationalDatasetTlvs tlvs = {0};
    bool commissioned = otDatasetIsCommissioned(instance);
    if (size && commissioned) error = OT_ERROR_ALREADY;
    else if (!commissioned) {
        error = otDatasetGetPendingTlvs(instance, &tlvs);
        if (error == OT_ERROR_NOT_FOUND) {
            if (!size) error = otDatasetCreateNewNetwork(instance, &dataset);
            else if (size <= sizeof(tlvs.mTlvs)) {
                tlvs.mLength = size; memcpy(tlvs.mTlvs, input, size);
                error = otDatasetParseTlvs(&tlvs, &dataset);
                if (error == OT_ERROR_NONE && (!dataset.mComponents.mIsActiveTimestampPresent ||
                    !dataset.mComponents.mIsChannelPresent || !dataset.mComponents.mIsExtendedPanIdPresent ||
                    !dataset.mComponents.mIsNetworkKeyPresent || !dataset.mComponents.mIsNetworkNamePresent ||
                    !dataset.mComponents.mIsMeshLocalPrefixPresent || !dataset.mComponents.mIsPanIdPresent ||
                    !dataset.mComponents.mIsSecurityPolicyPresent || dataset.mChannel < 11 || dataset.mChannel > 26))
                    error = OT_ERROR_INVALID_ARGS;
            } else error = OT_ERROR_INVALID_ARGS;
            if (error == OT_ERROR_NONE) error = otDatasetSetActive(instance, &dataset);
        } else if (error == OT_ERROR_NONE) error = OT_ERROR_BUSY;
    }
    if (error == OT_ERROR_NONE && !otIp6IsEnabled(instance))
        error = otIp6SetEnabled(instance, true);
    /* MLE Start() detaches even an enabled node. Preserve an existing attachment
     * (or an in-progress attachment) when this idempotent operation is repeated. */
    if (error == OT_ERROR_NONE && otThreadGetDeviceRole(instance) == OT_DEVICE_ROLE_DISABLED)
        error = otThreadSetEnabled(instance, true);
    br_wifi_profile_wipe(&dataset, sizeof(dataset)); br_wifi_profile_wipe(&tlvs, sizeof(tlvs));
    otrUnlock();
    return error == OT_ERROR_NONE ? BR_BLE_STATUS_OK : error == OT_ERROR_BUSY ? BR_BLE_STATUS_BUSY : BR_BLE_STATUS_INVALID;
#endif
}

size_t br_management_execute(uint8_t opcode, const uint8_t *input, size_t size, uint8_t *response, bool *reboot)
{
    *reboot = false;
    if (!sLock || xSemaphoreTake(sLock, 0) != pdTRUE) { response[0] = BR_BLE_STATUS_BUSY; return 1; }
    response[0] = BR_BLE_STATUS_OK;
    size_t length = 1;
    *reboot = false;
    /* A committed factory reset is terminal: refuse everything until the reboot. */
    if (br_factory_reset_pending()) { response[0] = BR_BLE_STATUS_BUSY; xSemaphoreGive(sLock); return length; }
    /* A failed boot erase is retried on the next boot and would discard new
     * settings: report storage as uncertain and refuse writes. REBOOT and
     * FACTORY_RESET stay available to retry it. */
    const bool reset_incomplete = br_factory_reset_incomplete();
    switch (opcode) {
    case BR_BLE_STATUS: {
        if (size) { response[0] = BR_BLE_STATUS_INVALID; break; }
        ipro_wifi_connectivity_snapshot wifi = {0};
        response[1] = ipro_wifi_connectivity_read(&wifi) == 0;
        response[2] = wifi.associated; response[3] = wifi.ipv4_assigned;
        response[4] = wifi.ipv6_assigned; response[5] = sWifiSaved;
        otInstance *instance = otrGetInstanceIfReady();
        if (instance && otrTryLock()) {
            response[6] = otThreadGetDeviceRole(instance);
            response[7] = otDatasetIsCommissioned(instance);
            otrUnlock();
        }
        response[8] = sStorageUncertain || reset_incomplete; length = 9;
        break;
    }
    case BR_BLE_WIFI_CONFIGURE: {
        struct br_wifi_profile profile = {0}, verified = {0};
        bool present = false;
        if (size != sizeof(profile)) { response[0] = BR_BLE_STATUS_INVALID; break; }
        memcpy(&profile, input, sizeof(profile));
        if (!br_wifi_profile_valid(&profile, sizeof(profile))) response[0] = BR_BLE_STATUS_INVALID;
        else if (sStorageUncertain || reset_incomplete) response[0] = BR_BLE_STATUS_STORAGE_ERROR;
        else {
            int result = br_wifi_storage_save(&profile, sizeof(profile));
            if (result == -1) response[0] = BR_BLE_STATUS_BUSY;
            else if (result || br_wifi_storage_load(&verified, &present) || !present || memcmp(&profile, &verified, sizeof(profile))) {
                sStorageUncertain = true; response[0] = BR_BLE_STATUS_STORAGE_ERROR;
            } else { sWifiSaved = true; response[1] = 1; length = 2; }
        }
        br_wifi_profile_wipe(&profile, sizeof(profile)); br_wifi_profile_wipe(&verified, sizeof(verified));
        break;
    }
    case BR_BLE_THREAD_DATASET_READ: {
        otOperationalDatasetTlvs dataset = {0};
        otError error = size ? OT_ERROR_INVALID_ARGS : ipro_matter_br_thread_dataset_read(false, &dataset);
        if (error == OT_ERROR_NONE) { memcpy(response + 1, dataset.mTlvs, dataset.mLength); length += dataset.mLength; }
        else response[0] = error == OT_ERROR_BUSY ? BR_BLE_STATUS_BUSY : BR_BLE_STATUS_NOT_READY;
        br_wifi_profile_wipe(&dataset, sizeof(dataset));
        break;
    }
    case BR_BLE_THREAD_DATASET_WRITE:
        response[0] = reset_incomplete ? BR_BLE_STATUS_STORAGE_ERROR : ensure_thread(input, size);
        break;
    case BR_BLE_REBOOT:
        if (size) response[0] = BR_BLE_STATUS_INVALID;
        else *reboot = true;
        break;
    case BR_BLE_FACTORY_RESET:
#ifdef CONFIG_MATTER_ENABLE
        response[0] = BR_BLE_STATUS_UNSUPPORTED; /* The Matter BR owns its reset flow. */
#else
        /* Also the way out of sStorageUncertain, so that state does not block it. */
        if (size) response[0] = BR_BLE_STATUS_INVALID;
        else if (br_factory_reset_request() != 0) response[0] = BR_BLE_STATUS_STORAGE_ERROR;
        else *reboot = true;
#endif
        break;
    default: response[0] = BR_BLE_STATUS_UNSUPPORTED; break;
    }
    xSemaphoreGive(sLock);
    return length;
}
