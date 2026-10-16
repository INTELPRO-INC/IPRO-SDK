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
 * @file cloud_sentino.c
 * @brief Sentino MQTT cloud module — cloud_ops_t adapter
 *
 * Wraps the existing sentino/ sub-modules (app_sentino, sentino_ai)
 * into the cloud_ops_t interface.
 */

#include "cloud_sentino.h"

#include <string.h>
#include "FreeRTOS.h"
#include "task.h"

#include "app_sentino.h"
#include "sentino_ai.h"
#include "sentino_lifecycle.h"
#include "app_log.h"

#define LOG_TAG "CLOUD_SNT"

/* ------------------------------------------------------------------ */
/* cloud_ops_t implementation                                         */
/* ------------------------------------------------------------------ */

static app_err_t sentino_init(void)
{
    int ret = app_sentino_init();
    if (ret < 0) return APP_ERR_MQTT;
    sentino_ai_init();
    return APP_OK;
}

static app_err_t sentino_deinit(void)
{
    app_sentino_deinit();
    return APP_OK;
}

static int sentino_is_connected(void)
{
    return app_sentino_is_connected();
}

static app_err_t sentino_wait_connected(int timeout_s)
{
    IPRO_LOGI(LOG_TAG, "Waiting for MQTT connection (timeout: %ds)...", timeout_s);
    for (int i = 0; i < timeout_s; i++) {
        if (app_sentino_is_connected()) {
            IPRO_LOGI(LOG_TAG, "MQTT connected");
            return APP_OK;
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    IPRO_LOGE(LOG_TAG, "MQTT connection timeout");
    return APP_ERR_TIMEOUT;
}

static app_err_t sentino_publish(const char *code, const char *data_json,
                                  int ack)
{
    int ret = app_sentino_report(code, data_json, ack);
    return (ret == 0) ? APP_OK : APP_ERR_MQTT;
}

static void sentino_set_issue_handler(cloud_msg_handler_t h)
{
    /* cloud_msg_handler_t and sentino_msg_handler_t have the same signature */
    app_sentino_set_issue_handler(h);
}

static app_err_t sentino_issue_resp(const char *payload)
{
    int ret = app_sentino_issue_response(payload);
    return (ret == 0) ? APP_OK : APP_ERR_MQTT;
}

static void sentino_report_info(void)
{
    sentino_lifecycle_info_report();
}

static void sentino_time_sync_fn(void)
{
    sentino_lifecycle_time_sync();
}

static void sentino_get_device_info_fn(const char **pid, const char **uuid)
{
    app_sentino_get_device_info(pid, uuid);
}

static app_err_t sentino_request_rtc_access(cloud_rtc_access_t *out)
{
    if (!out) return APP_ERR_INVALID;

    sentino_ai_access_t ai_access;
    int ret = sentino_ai_request_access(&ai_access);
    if (ret < 0) return APP_ERR_MQTT;

    memcpy(out->app_id,  ai_access.app_id,  sizeof(out->app_id));
    memcpy(out->token,   ai_access.token,   sizeof(out->token));
    memcpy(out->channel, ai_access.channel, sizeof(out->channel));
    out->uid = ai_access.uid;

    return APP_OK;
}

const cloud_ops_t cloud_sentino_ops = {
    .name              = "sentino",
    .init              = sentino_init,
    .deinit            = sentino_deinit,
    .is_connected      = sentino_is_connected,
    .wait_connected    = sentino_wait_connected,
    .publish           = sentino_publish,
    .set_issue_handler  = sentino_set_issue_handler,
    .issue_response     = sentino_issue_resp,
    .request_rtc_access = sentino_request_rtc_access,
    .report_info        = sentino_report_info,
    .time_sync          = sentino_time_sync_fn,
    .get_device_info    = sentino_get_device_info_fn,
};
