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
#include "wifi_mgmr_ext.h"
#include "wifi_mgmr.h"
#include "ipro_fw_api.h"
#include "wl_board_cfg.h"
#include "ipro_event.h"
#include "ipro_log.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "lwip/tcpip.h"
#include "drv_common.h"
#include <string.h>
#include <easyflash.h>
#include "br_wifi_profile.h"
#include "br_wifi_storage.h"
#include "br_matter_wifi.h"
#include "wifi_backhaul.h"
#ifdef CONFIG_SHELL
#include "shell.h"
#endif

extern void interrupt0_handler(void);
IPRO_EVENT_DECLARE_BASE(WIFI_EVENT);
static TaskHandle_t sWifiTask;
static wifi_conf_t sWifiConfig = {.country_code = "CN"};
static StaticSemaphore_t sTcpipReadyStorage;
static SemaphoreHandle_t sTcpipReady;
static struct br_wifi_profile sBootProfile;
static bool sStorageReady, sBootProfilePresent;
static bool sRestoreSubmitting;
static struct br_wifi_boot_status sBootStatus;

/* Written before Wi-Fi/OT tasks start, then immutable until the next boot. */
bool br_wifi_thread_autostart(void) { return sBootProfilePresent; }

int br_wifi_boot_status_get(struct br_wifi_boot_status *status)
{
    if (!status) return -1;
    taskENTER_CRITICAL();
    *status = sBootStatus;
    taskEXIT_CRITICAL();
    return 0;
}

static void restore_complete(void *context, int vif, uint32_t result)
{
    (void)context;
    taskENTER_CRITICAL();
    if (vif == MGMR_VIF_STA && sBootStatus.phase == BR_WIFI_BOOT_RESTORING &&
        !sBootStatus.result_valid) {
        sBootStatus.result = result;
        sBootStatus.result_valid = true;
        if (!sRestoreSubmitting) sBootStatus.phase = BR_WIFI_BOOT_COMPLETE;
    }
    taskEXIT_CRITICAL();
}

static void restore_profile(void)
{
    /* Reserve before submitting: the worker may invoke the callback inline.
     * Duplicate manager-ready events must never replay the boot credentials. */
    taskENTER_CRITICAL();
    if (sBootStatus.phase != BR_WIFI_BOOT_WAIT_MANAGER) {
        taskEXIT_CRITICAL();
        return;
    }
    sBootStatus.manager_ready = true;
    sBootStatus.phase = sBootProfilePresent ? BR_WIFI_BOOT_RESTORING : BR_WIFI_BOOT_COMPLETE;
    sRestoreSubmitting = sBootProfilePresent;
    taskEXIT_CRITICAL();
    if (!sBootProfilePresent) return;
    wifi_mgmr_sta_connect_params_t cfg = {0};
    memcpy(cfg.ssid, sBootProfile.ssid, sBootProfile.ssid_len);
    memcpy(cfg.key, sBootProfile.key, sBootProfile.key_len);
    cfg.ssid_len = sBootProfile.ssid_len;
    cfg.key_len = sBootProfile.key_len;
    cfg.pmf_cfg = sBootProfile.pmf;
    cfg.use_dhcp = 1;
    if (sBootProfile.akm) {
        memcpy(cfg.akm_str, sBootProfile.akm == 1 ? "WPA2" : "WPA3", 5);
        cfg.akm_len = 4;
    }
    int result = wifi_mgmr_sta_connect_managed(&cfg, restore_complete, NULL);
    br_wifi_profile_wipe(&cfg, sizeof(cfg));
    br_wifi_profile_wipe(&sBootProfile, sizeof(sBootProfile));
    taskENTER_CRITICAL();
    sRestoreSubmitting = false;
    sBootStatus.submit_error = result;
    if (result != 0) sBootStatus.phase = BR_WIFI_BOOT_FAILED;
    else if (sBootStatus.result_valid) sBootStatus.phase = BR_WIFI_BOOT_COMPLETE;
    taskEXIT_CRITICAL();
    IPRO_LOGI("Backhaul", "Stored STA connect request result=%d", result);
}

static void tcpip_ready(void *context)
{
    (void)context;
    xSemaphoreGive(sTcpipReady);
}

static void wifi_event(const ipro_event_t *event, void *context)
{
    (void)context;
    switch (event->event_id) {
    case CODE_WIFI_ON_INIT_DONE:
        wifi_mgmr_init(&sWifiConfig);
        break;
    case CODE_WIFI_ON_MGMR_DONE:
        taskENTER_CRITICAL();
        bool waiting = sBootStatus.phase == BR_WIFI_BOOT_WAIT_MANAGER;
        taskEXIT_CRITICAL();
        if (!waiting) break;
        /* This application enables the STA owner here. Association is observed
         * separately; a failed join/disconnect does not disable that policy. */
        if (br_matter_wifi_set_enabled(true) != 0)
            IPRO_LOGE("Backhaul", "Matter Wi-Fi snapshot policy unavailable");
        wifi_mgmr_sta_autoconnect_enable();
        restore_profile();
        IPRO_LOGI("Backhaul", "Wi-Fi ready; boot profile=%u", (unsigned)sBootProfilePresent);
        break;
    case CODE_WIFI_ON_GOT_IP:
        IPRO_LOGI("Backhaul", "Wi-Fi address acquired, free heap=%u", (unsigned)xPortGetFreeHeapSize());
        break;
    case CODE_WIFI_ON_DISCONNECT:
        IPRO_LOGW("Backhaul", "Wi-Fi disconnected");
        break;
    default:
        break;
    }
}

static int boot_failed(int error)
{
    br_wifi_profile_wipe(&sBootProfile, sizeof(sBootProfile));
    taskENTER_CRITICAL();
    sBootStatus.phase = BR_WIFI_BOOT_FAILED;
    sBootStatus.submit_error = error;
    taskEXIT_CRITICAL();
    return error;
}

int br_wifi_start(void)
{
    /* Complete the non-thread-safe first initialization before either stack
     * starts. OT settings' later easyflash_init() is then idempotent. */
    if (easyflash_init() != EF_NO_ERR) return boot_failed(-1);
    sStorageReady = true;
    const int profile_error = br_wifi_storage_load(&sBootProfile, &sBootProfilePresent);
    if (profile_error != EF_NO_ERR) return boot_failed(profile_error);
    taskENTER_CRITICAL();
    sBootStatus.profile_present = sBootProfilePresent;
    sBootStatus.phase = BR_WIFI_BOOT_WAIT_MANAGER;
    taskEXIT_CRITICAL();
    if (!sBootProfilePresent) br_wifi_profile_wipe(&sBootProfile, sizeof(sBootProfile));
    sTcpipReady = xSemaphoreCreateBinaryStatic(&sTcpipReadyStorage);
    tcpip_init(tcpip_ready, NULL);
    if (xSemaphoreTake(sTcpipReady, pdMS_TO_TICKS(5000)) != pdTRUE) return boot_failed(-1);
    if (br_matter_wifi_init() != 0) return boot_failed(-1);
    if (ipro_event_loop_create_default() != IPRO_OK) return boot_failed(-1);
    if (ipro_event_handler_register(WIFI_EVENT, IPRO_EVENT_ANY_ID, wifi_event, NULL) != IPRO_OK) return boot_failed(-1);
    Interrupt_Handler_Register(WIFI_IRQn, interrupt0_handler);
    return xTaskCreate(wifi_main, "wifi_fw", 1536, NULL, 30, &sWifiTask) == pdPASS ? 0 : boot_failed(-1);
}

#ifdef CONFIG_SHELL
static void cmd_wifi_save(int argc, char **argv)
{
    (void)argv;
    if (argc != 1 || !sStorageReady || !wifi_mgmr_sta_state_get()) {
        IPRO_LOGW("Backhaul", "Save requires initialized storage and a connected STA");
        return;
    }
    wifi_mgmr_sta_connect_params_t cfg;
    struct br_wifi_profile profile = {.magic = {'B','W','0','1'}};
    /* Single-core snapshot only; never hold IRQ/console/OT locks over flash. */
    taskENTER_CRITICAL();
    cfg = wifiMgmr.sta_connect_param;
    taskEXIT_CRITICAL();
    bool supported = cfg.use_dhcp == 1 && cfg.bssid_str[0] == 0 && cfg.wpa_flags == 0;
    if (cfg.akm_len) {
        if (cfg.akm_len == 4 && memcmp(cfg.akm_str, "WPA2", 4) == 0) profile.akm = 1;
        else if (cfg.akm_len == 4 && memcmp(cfg.akm_str, "WPA3", 4) == 0) profile.akm = 2;
        else supported = false;
    }
    profile.ssid_len = cfg.ssid_len;
    profile.key_len = cfg.key_len;
    profile.pmf = cfg.pmf_cfg;
    if (profile.ssid_len <= sizeof(profile.ssid) && profile.key_len <= sizeof(profile.key)) {
        memcpy(profile.ssid, cfg.ssid, profile.ssid_len);
        memcpy(profile.key, cfg.key, profile.key_len);
    }
    br_wifi_profile_wipe(&cfg, sizeof(cfg));
    int result = -1;
    if (supported && br_wifi_profile_valid(&profile, sizeof(profile)))
        result = br_wifi_storage_save(&profile, sizeof(profile));
    br_wifi_profile_wipe(&profile, sizeof(profile));
    IPRO_LOGI("Backhaul", "Save result=%d (0=stored); profile storage is not encrypted", result);
}
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_save, br_wifi_save, Save connected personal STA for next boot);

static void cmd_wifi_clear(int argc, char **argv)
{
    (void)argv;
    if (argc != 1 || !sStorageReady) return;
    int result = br_wifi_storage_clear();
    IPRO_LOGI("Backhaul", "Clear boot profile result=%d; current connection unchanged", result);
}
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_clear, br_wifi_clear, Clear only BR boot profile);
#endif
