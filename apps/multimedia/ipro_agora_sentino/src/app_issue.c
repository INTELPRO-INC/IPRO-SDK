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
 * @file app_issue.c
 * @brief Issue dispatcher — handles cloud commands
 *
 * Extracted from main.c on_sentino_issue(). Dispatches incoming cloud
 * commands (property_set/volume, agora_join, reset, etc.) through
 * module ops pointers instead of direct function calls.
 */

#include "app_issue.h"

#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

#include "app_json.h"
#include "sentino_lifecycle.h"  /* for sentino_lifecycle_reset() — device-level */
#include "app_log.h"

#define LOG_TAG "ISSUE"

static const audio_output_ops_t *s_audio_out;
static const rtc_ops_t          *s_rtc;
static const cloud_ops_t        *s_cloud;

/* ------------------------------------------------------------------ */
/* Issue handler                                                      */
/* ------------------------------------------------------------------ */

static void on_issue(const char *code, char *payload)
{
    IPRO_LOGI(LOG_TAG, "Issue: code='%s'", code ? code : "(null)");

    if (code && strcmp(code, "property_set") == 0) {
        int vol_changed = 0;
        int platform_vol = -1;

        const char *vol_key = strstr(payload, "\"volume_set\"");
        if (vol_key) {
            const char *colon = strchr(vol_key + 12, ':');
            if (colon) {
                platform_vol = atoi(colon + 1);
                int dac_vol = platform_vol * 10;
                if (dac_vol > 100) dac_vol = 100;
                if (dac_vol < 0) dac_vol = 0;
                IPRO_LOGI(LOG_TAG, "Volume set: platform=%d dac=%d", platform_vol, dac_vol);
                s_audio_out->set_volume(dac_vol);
                vol_changed = 1;
            }
        }

        if (!vol_changed) {
            vol_key = strstr(payload, "\"volume\"");
            if (vol_key && !strstr(vol_key, "\"volume_set\"")) {
                const char *colon = strchr(vol_key + 8, ':');
                if (colon) {
                    platform_vol = atoi(colon + 1);
                    int dac_vol = platform_vol * 10;
                    if (dac_vol > 100) dac_vol = 100;
                    if (dac_vol < 0) dac_vol = 0;
                    IPRO_LOGI(LOG_TAG, "Volume set: platform=%d dac=%d", platform_vol, dac_vol);
                    s_audio_out->set_volume(dac_vol);
                    vol_changed = 1;
                }
            }
        }

        if (vol_changed) {
            int cur_vol = s_audio_out->get_volume() / 10;
            char report_data[128];
            snprintf(report_data, sizeof(report_data),
                     "\"properties\":{\"volume_set\":%d}", cur_vol);
            s_cloud->publish("property_report", report_data, 0);
            IPRO_LOGI(LOG_TAG, "Property report sent: volume_set=%d", cur_vol);
        }

    } else if (code && strcmp(code, "ping") == 0) {
        IPRO_LOGI(LOG_TAG, "Ping received");

    } else if (code && strcmp(code, "reset") == 0) {
        IPRO_LOGW(LOG_TAG, "Reset command received!");
        /* Send issue_response before resetting */
        s_cloud->issue_response(payload);
        vTaskDelay(pdMS_TO_TICKS(500));
        sentino_lifecycle_reset();
        return; /* never reached */

    } else if (code && strcmp(code, "get_bind_code") == 0) {
        IPRO_LOGI(LOG_TAG, "get_bind_code — returning stub");

    } else if (code && strcmp(code, "get_device_bind_status") == 0) {
        IPRO_LOGI(LOG_TAG, "get_device_bind_status — returning bound=1");

    } else if (code && strcmp(code, "agora_join") == 0) {
        IPRO_LOGI(LOG_TAG, "agora_join: switching channel");

        char work[512];
        uint16_t plen = (uint16_t)strlen(payload);
        uint16_t clen = (plen < sizeof(work) - 1) ? plen : sizeof(work) - 1;

        static char new_app_id[64];
        static char new_token[256];
        static char new_channel[64];
        uint32_t new_uid = 0;

        memcpy(work, payload, clen); work[clen] = '\0';
        char *val = json_extract_string(work, "appId");
        if (val) strncpy(new_app_id, val, sizeof(new_app_id) - 1);

        memcpy(work, payload, clen); work[clen] = '\0';
        val = json_extract_string(work, "rtcToken");
        if (val) strncpy(new_token, val, sizeof(new_token) - 1);

        memcpy(work, payload, clen); work[clen] = '\0';
        val = json_extract_string(work, "channelName");
        if (val) strncpy(new_channel, val, sizeof(new_channel) - 1);

        memcpy(work, payload, clen); work[clen] = '\0';
        const char *uid_key = strstr(work, "\"uid\"");
        if (uid_key) {
            uid_key += 5;
            while (*uid_key == ' ' || *uid_key == ':' || *uid_key == '\t') uid_key++;
            new_uid = (uint32_t)strtoul(uid_key, NULL, 10);
        }

        IPRO_LOGI(LOG_TAG, "agora_join: ch='%s' uid=%lu",
                  new_channel, (unsigned long)new_uid);

        if (new_channel[0] && new_token[0]) {
            rtc_join_params_t params = {
                .app_id  = new_app_id,
                .token   = new_token,
                .channel = new_channel,
                .uid     = new_uid,
            };
            s_rtc->rejoin(&params);
        } else {
            IPRO_LOGW(LOG_TAG, "agora_join: missing channel or token");
        }

    } else if (code && strcmp(code, "agora_event") == 0) {
        IPRO_LOGI(LOG_TAG, "agora_event received");
    }

    /* Acknowledge the issue through the cloud ops interface */
    s_cloud->issue_response(payload);
}

/* ------------------------------------------------------------------ */
/* Public API                                                         */
/* ------------------------------------------------------------------ */

app_err_t app_issue_init(const app_issue_cfg_t *cfg)
{
    if (!cfg || !cfg->audio_out || !cfg->rtc || !cfg->cloud)
        return APP_ERR_INVALID;

    s_audio_out = cfg->audio_out;
    s_rtc       = cfg->rtc;
    s_cloud     = cfg->cloud;

    IPRO_LOGI(LOG_TAG, "Issue dispatcher initialized");
    return APP_OK;
}

cloud_msg_handler_t app_issue_get_handler(void)
{
    return on_issue;
}
