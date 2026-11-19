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

#include "br_matter_runtime.h"
#include "wifi_backhaul.h"
#include "br_wifi_provision.h"
#include "br_wifi_storage.h"
#include <ipro_matter_br.h>
#include <ipro_matter_br_thread.h>
#include <ipro_matter_wifi.h>
#include <wifi_mgmr_ext.h>
#include <ipro_log.h>
#include <string.h>

static enum br_matter_runtime_status sStatus = BR_MATTER_RUNTIME_WAITING;
static uint32_t sError, sLease;
static bool sSealed;

static bool release_pause(void)
{
    if (!sLease) return true;
    if (wifi_mgmr_sta_connect_resume(sLease) != 0) return false;
    sLease = 0;
    return true;
}

static bool network_ready(void)
{
    ipro_wifi_connectivity_snapshot wifi;
    uint8_t mac[6];
    ipro_matter_br_thread_state thread;
    return ipro_wifi_connectivity_read(&wifi) == 0 && wifi.revision &&
        wifi.ready && wifi.enabled && wifi.associated && wifi.ipv6_assigned &&
        ipro_wifi_get_station_mac(mac) == 0 &&
        ipro_matter_br_thread_read(&thread) == OT_ERROR_NONE;
}

static void process_profile(void)
{
    struct br_wifi_profile profile, verified;
    const int pending = br_wifi_provision_take(&profile);
    if (!pending) return;
    if (pending < 0) {
        br_wifi_provision_finish(false);
        IPRO_LOGE("MatterBR", "BR_WIFI_INPUT_REJECTED");
        return;
    }
    bool present = false;
    const int saved = br_wifi_storage_save(&profile, sizeof(profile));
    const int checked = saved == 0 ? br_wifi_storage_load(&verified, &present) : -1;
    const bool matches = saved == 0 && checked == 0 && present &&
        memcmp(&profile, &verified, sizeof(profile)) == 0;
    br_wifi_profile_wipe(&profile, sizeof(profile));
    br_wifi_profile_wipe(&verified, sizeof(verified));
    /* A failed flash operation may still have changed storage. Require reboot
     * even then, so boot recovery/validation decides what can be restored. */
    br_wifi_provision_finish(true);
    sError = matches ? 0 : UINT32_MAX;
    sStatus = BR_MATTER_RUNTIME_REBOOT_REQUIRED;
    if (matches) IPRO_LOGI("MatterBR", "BR_WIFI_SAVED_REBOOT_REQUIRED");
    else IPRO_LOGE("MatterBR", "BR_WIFI_STORAGE_UNCERTAIN_REBOOT_REQUIRED");
}

enum br_matter_runtime_status br_matter_runtime_step(uint32_t *error)
{
    if (!error) return BR_MATTER_RUNTIME_FAILED;
    *error = sError;
    if (sStatus != BR_MATTER_RUNTIME_WAITING) {
        /* Reboot-required can follow a write while CONNECT cleanup was still
         * paused. Keep retrying lease release without issuing another write. */
        if (sStatus == BR_MATTER_RUNTIME_REBOOT_REQUIRED && !release_pause())
            return BR_MATTER_RUNTIME_WAITING;
        return sStatus;
    }
    if (!sSealed) {
        struct br_wifi_boot_status boot;
        if (br_wifi_boot_status_get(&boot) != 0) return sStatus;
        if (!boot.manager_ready) {
            if (boot.phase == BR_WIFI_BOOT_FAILED) {
                sError = (uint32_t)boot.submit_error;
                sStatus = BR_MATTER_RUNTIME_FAILED;
                *error = sError;
            }
            return sStatus;
        }
        if (boot.phase != BR_WIFI_BOOT_COMPLETE && boot.phase != BR_WIFI_BOOT_FAILED)
            return sStatus;
        br_wifi_provision_open();
        process_profile();
        *error = sError;
        if (sStatus == BR_MATTER_RUNTIME_REBOOT_REQUIRED)
            return release_pause() ? sStatus : BR_MATTER_RUNTIME_WAITING;
        if (!network_ready()) {
            (void)release_pause();
            return sStatus;
        }
        /* Concrete firmware ownership: shell configuration writers are denied
         * from boot; the boot restore has completed; the local mailbox is the
         * only additional external writer. Pause drains accepted CONNECT work
         * including native cleanup, not DHCP or normal supplicant maintenance.
         * No terminal close/fence: Matter must retain connect/rollback access. */
        const int paused = wifi_mgmr_sta_connect_pause(&sLease);
        if (paused > 0) return sStatus;
        if (paused < 0) {
            if (!release_pause()) return sStatus;
            sError = UINT32_MAX;
            sStatus = BR_MATTER_RUNTIME_FAILED;
            *error = sError;
            return sStatus;
        }
        if (br_wifi_provision_seal() != 0) {
            (void)release_pause();
            return sStatus;
        }
        sSealed = true;
    }
    if (!release_pause()) return BR_MATTER_RUNTIME_WAITING;
    /* Normal current-network maintenance may resume. No independent external
     * configuration writer can enter now. Matter Init claims profile storage;
     * never pre-open its transaction here. All SDK network locks are released. */
    sError = ipro_matter_br_start();
    sStatus = sError ? BR_MATTER_RUNTIME_FAILED : BR_MATTER_RUNTIME_STARTED;
    *error = sError;
    return sStatus;
}
