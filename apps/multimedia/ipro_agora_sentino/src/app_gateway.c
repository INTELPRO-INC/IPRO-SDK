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
 * @file app_gateway.c
 * @brief Gateway orchestrator — manages the startup sequence
 *
 * Extracted from main.c audio_gateway_task(). Executes the phased
 * startup through module ops pointers, tracking state at each phase.
 */

#include "app_gateway.h"
#include "app_audio_route.h"
#include "app_issue.h"
#include "app_lcd.h"

#include <string.h>
#include "FreeRTOS.h"
#include "task.h"

#include <generated/autoconf.h>

#ifdef CONFIG_EASYFLASH4
#include "easyflash.h"
#endif

#ifdef CONFIG_BLUETOOTH_LE_ENABLE
#include "app_ble_prov.h"
#endif

/* sentino headers no longer needed — all access through cloud_ops_t */

#include <lwip/apps/sntp.h>

#include "app_log.h"

#define LOG_TAG "GATEWAY"

#define NETWORK_TIMEOUT_S 30
#define MQTT_TIMEOUT_S    15

/* Module state tracking (one per startup phase) */
#define NUM_PHASES 6
static module_state_t s_phase_state[NUM_PHASES];

static gateway_config_t s_cfg;

/* ------------------------------------------------------------------ */
/* Gateway task — phased startup                                      */
/* ------------------------------------------------------------------ */

static void gateway_task(void *param)
{
    (void)param;
    app_err_t err;

    /* ========== Phase 0: Persistent storage ========== */
#ifdef CONFIG_EASYFLASH4
    {
        EfErrCode ef_ret = easyflash_init();
        IPRO_LOGI(LOG_TAG, "EasyFlash init: %d", ef_ret);
    }
#endif

    /* ========== LCD display init (before other phases for visual feedback) ========== */
    {
        app_err_t lcd_err = app_lcd_init();
        if (lcd_err != APP_OK) {
            IPRO_LOGW(LOG_TAG, "LCD init failed (%d) — continuing without display", lcd_err);
        }
    }

    /* ========== Phase 1: BLE provisioning ========== */
    s_phase_state[0] = MODULE_STATE_INIT;
#ifdef CONFIG_BLUETOOTH_LE_ENABLE
    {
        const char *prov_pid, *prov_uuid;
        s_cfg.cloud->get_device_info(&prov_pid, &prov_uuid);
        app_ble_prov_init(prov_pid, prov_uuid, NULL);

        if (!app_ble_prov_is_bound()) {
            IPRO_LOGI(LOG_TAG, "Device not bound -- starting BLE provisioning");
            app_ble_prov_start();

            while (!app_ble_prov_is_bound()) {
                vTaskDelay(pdMS_TO_TICKS(1000));
            }

            IPRO_LOGI(LOG_TAG, "Sending BLE bind confirmation to phone");
            app_ble_prov_send_framed("{\"ble\":\"bind\"}");

            /* Wait for BLE notify to be transmitted over the air.
             * bt_gatt_notify() queues data but actual TX depends on the
             * connection interval (7.5ms~4s). 2s covers most CI values
             * and gives the phone time to process + ACK. */
            vTaskDelay(pdMS_TO_TICKS(2000));

            app_ble_prov_stop();
            /* Give BLE stack time to finish pending operations before
             * tearing down the controller. */
            vTaskDelay(pdMS_TO_TICKS(500));
            IPRO_LOGI(LOG_TAG, "Disabling BLE to free memory for Agora...");
            app_ble_prov_deinit();
            vTaskDelay(pdMS_TO_TICKS(500));
            IPRO_LOGI(LOG_TAG, "BLE disabled, continuing to network");
        } else {
            IPRO_LOGI(LOG_TAG, "Device bound (uid=%s aid=%s)",
                      app_ble_prov_get_user_id(),
                      app_ble_prov_get_asset_id());
        }
    }
#endif
    s_phase_state[0] = MODULE_STATE_RUNNING;

    /* ========== Phase 2: Network ========== */
    s_phase_state[1] = MODULE_STATE_INIT;
    IPRO_LOGI(LOG_TAG, "Starting network...");
    s_cfg.network->connect();

    /* Init audio hardware in parallel with network connect.
     * Audio HW failure is fatal — retry with backoff rather than
     * falling through to the health loop (network/cloud not yet up). */
    s_phase_state[3] = MODULE_STATE_INIT;
    for (int audio_retry = 0; ; audio_retry++) {
        err = s_cfg.audio_out->init();
        if (err == APP_OK) {
            err = s_cfg.audio_in->init();
        }
        if (err == APP_OK) break;

        s_phase_state[3] = MODULE_STATE_ERROR;
        int delay_s = (audio_retry < 3) ? 10 : 30;
        IPRO_LOGE(LOG_TAG, "Audio init failed (attempt %d), retry in %ds...",
                  audio_retry + 1, delay_s);
        vTaskDelay(pdMS_TO_TICKS(delay_s * 1000));
    }
    s_phase_state[3] = MODULE_STATE_RUNNING;

    /* Wait for network — PPP watchdog handles reconnect in background,
     * just keep polling wait_ready until link comes up */
    while (1) {
        err = s_cfg.network->wait_ready(NETWORK_TIMEOUT_S);
        if (err == APP_OK) break;
        IPRO_LOGW(LOG_TAG, "Network not ready after %ds, waiting for PPP watchdog...",
                  NETWORK_TIMEOUT_S);
    }
    s_phase_state[1] = MODULE_STATE_RUNNING;

    /* Start NTP */
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_setservername(1, "time.google.com");
    sntp_setservername(2, "time.cloudflare.com");
    sntp_init();
    IPRO_LOGI(LOG_TAG, "SNTP started");

    /* ========== Phase 3: Cloud (MQTT) ========== */
    s_phase_state[2] = MODULE_STATE_INIT;
    err = s_cfg.cloud->init();
    if (err != APP_OK) {
        IPRO_LOGE(LOG_TAG, "Cloud init failed");
        s_phase_state[2] = MODULE_STATE_ERROR;
        goto idle;
    }

    /* Set up issue dispatcher */
    {
        app_issue_cfg_t issue_cfg = {
            .audio_out = s_cfg.audio_out,
            .rtc       = s_cfg.rtc,
            .cloud     = s_cfg.cloud,
        };
        app_issue_init(&issue_cfg);
        s_cfg.cloud->set_issue_handler(app_issue_get_handler());
    }

    /* Wait for MQTT — sentino task retries internally, just keep polling */
    while (1) {
        err = s_cfg.cloud->wait_connected(MQTT_TIMEOUT_S);
        if (err == APP_OK) break;
        IPRO_LOGW(LOG_TAG, "MQTT not connected after %ds, waiting...", MQTT_TIMEOUT_S);
    }
    s_phase_state[2] = MODULE_STATE_RUNNING;

    /* Report device info + bind */
    s_cfg.cloud->report_info();
    s_cfg.cloud->time_sync();

    {
#ifdef CONFIG_BLUETOOTH_LE_ENABLE
        const char *uid = app_ble_prov_get_user_id();
        const char *aid = app_ble_prov_get_asset_id();
#else
        const char *uid = "";
        const char *aid = "";
#endif
        char bind_data[256];
        snprintf(bind_data, sizeof(bind_data),
                 "\"userId\":\"%s\",\"assetId\":\"%s\",\"version\":\"1.0.0\"",
                 uid ? uid : "", aid ? aid : "");
        IPRO_LOGI(LOG_TAG, "Sending bind report: %s", bind_data);
        s_cfg.cloud->publish("bind", bind_data, 1);
        vTaskDelay(pdMS_TO_TICKS(3000));
    }

    /* ========== Phase 4: RTC join ========== */
    s_phase_state[4] = MODULE_STATE_INIT;

    cloud_rtc_access_t rtc_access;
    err = s_cfg.cloud->request_rtc_access(&rtc_access);
    if (err != APP_OK) {
        IPRO_LOGE(LOG_TAG, "Failed to get RTC access");
        s_phase_state[4] = MODULE_STATE_ERROR;
        goto idle;
    }

    s_cfg.audio_out->start();

    {
        rtc_join_params_t join_params = {
            .app_id  = rtc_access.app_id,
            .token   = rtc_access.token,
            .channel = rtc_access.channel,
            .uid     = rtc_access.uid,
        };
        err = s_cfg.rtc->join(&join_params);
        if (err != APP_OK) {
            IPRO_LOGE(LOG_TAG, "RTC join failed");
            s_phase_state[4] = MODULE_STATE_ERROR;
            goto idle;
        }
    }

    /* Wait for channel join */
    for (int i = 0; i < 10; i++) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        if (s_cfg.rtc->is_connected())
            break;
    }
    s_phase_state[4] = MODULE_STATE_RUNNING;

    /* ========== Phase 5: Wire audio route + start capture ========== */
    s_phase_state[5] = MODULE_STATE_INIT;
    {
        audio_route_cfg_t route_cfg = {
            .input  = s_cfg.audio_in,
            .output = s_cfg.audio_out,
            .rtc    = s_cfg.rtc,
        };
        audio_route_init(&route_cfg);
    }

    s_cfg.audio_in->start();
    s_phase_state[5] = MODULE_STATE_RUNNING;

    IPRO_LOGI(LOG_TAG, "Channel joined, remote_users=%lu",
              (unsigned long)s_cfg.rtc->get_remote_user_count());
    IPRO_LOGI(LOG_TAG, "Audio gateway running — health monitor active");

idle:
    /* ========== Health monitoring loop ========== */
    {
        int net_was_up = (s_phase_state[1] == MODULE_STATE_RUNNING);
        int cloud_was_up = (s_phase_state[2] == MODULE_STATE_RUNNING);
        int rtc_was_up = (s_phase_state[4] == MODULE_STATE_RUNNING);

        /* Initial LCD status from current state */
        app_lcd_set_network_status(net_was_up);
        app_lcd_set_cloud_status(rtc_was_up);

        while (1) {
            vTaskDelay(pdMS_TO_TICKS(5000));

            /* Check network */
            int net_up = s_cfg.network->is_connected();
            if (net_was_up && !net_up) {
                IPRO_LOGW(LOG_TAG, "Health: network DOWN");
                s_phase_state[1] = MODULE_STATE_ERROR;
                app_lcd_set_network_status(false);
                s_cfg.network->reconnect();
            } else if (!net_was_up && net_up) {
                IPRO_LOGI(LOG_TAG, "Health: network recovered");
                s_phase_state[1] = MODULE_STATE_RUNNING;
                app_lcd_set_network_status(true);
            }
            net_was_up = net_up;

            /* Check cloud (only if network is up) */
            if (net_up) {
                int cloud_up = s_cfg.cloud->is_connected();
                if (cloud_was_up && !cloud_up) {
                    /* The cloud module reconnects on its own (sentino_mqtt
                     * task: disconnect, free, new client, connect). Tearing it
                     * down from here as well raced that task: deinit()
                     * vTaskDelete()s it wherever it is, possibly between
                     * freeing the old client and clearing the pointer, or
                     * holding the tcpip core lock. A client freed twice or
                     * with its pcb still open then shares memory with the new
                     * one - Reef 2026-10-08: "status=257" followed by
                     * "mqtt_tcp_recv_cb: client->conn == pcb". One owner. */
                    IPRO_LOGW(LOG_TAG, "Health: cloud DOWN — cloud module reconnects");
                    s_phase_state[2] = MODULE_STATE_ERROR;
                    app_lcd_set_network_status(false);
                } else if (!cloud_was_up && cloud_up) {
                    IPRO_LOGI(LOG_TAG, "Health: cloud recovered");
                    s_phase_state[2] = MODULE_STATE_RUNNING;
                    app_lcd_set_network_status(true);
                }
                cloud_was_up = cloud_up;
            }

            /* Check RTC (only if network is up) */
            if (net_up) {
                int rtc_up = s_cfg.rtc->is_connected();
                if (rtc_was_up && !rtc_up) {
                    IPRO_LOGW(LOG_TAG, "Health: RTC disconnected — requesting new token");
                    s_phase_state[4] = MODULE_STATE_ERROR;
                    app_lcd_set_cloud_status(false);
                    /* Request fresh credentials and rejoin */
                    cloud_rtc_access_t new_access;
                    if (s_cfg.cloud->is_connected() &&
                        s_cfg.cloud->request_rtc_access(&new_access) == APP_OK) {
                        rtc_join_params_t params = {
                            .app_id  = new_access.app_id,
                            .token   = new_access.token,
                            .channel = new_access.channel,
                            .uid     = new_access.uid,
                        };
                        if (s_cfg.rtc->rejoin(&params) == APP_OK) {
                            IPRO_LOGI(LOG_TAG, "Health: RTC rejoin initiated");
                        }
                    }
                } else if (!rtc_was_up && rtc_up) {
                    IPRO_LOGI(LOG_TAG, "Health: RTC recovered");
                    s_phase_state[4] = MODULE_STATE_RUNNING;
                    app_lcd_set_cloud_status(true);
                }
                rtc_was_up = rtc_up;
            }
        }
    }

    /* Unreachable, but satisfy compiler */
    while (1) { vTaskDelay(pdMS_TO_TICKS(1000)); }
}

/* ------------------------------------------------------------------ */
/* Public API                                                         */
/* ------------------------------------------------------------------ */

app_err_t app_gateway_start(const gateway_config_t *cfg)
{
    if (!cfg || !cfg->network || !cfg->audio_in || !cfg->audio_out ||
        !cfg->rtc || !cfg->cloud)
        return APP_ERR_INVALID;

    s_cfg = *cfg;
    memset(s_phase_state, 0, sizeof(s_phase_state));

    BaseType_t ret = xTaskCreate(gateway_task, "agora_gw", 24 * 1024, NULL,
                                 tskIDLE_PRIORITY + 5, NULL);
    return (ret == pdPASS) ? APP_OK : APP_ERR_NOMEM;
}

module_state_t app_gateway_get_state(int phase)
{
    if (phase < 0 || phase >= NUM_PHASES)
        return MODULE_STATE_IDLE;
    return s_phase_state[phase];
}
