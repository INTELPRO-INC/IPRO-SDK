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
 * @file sentino_ai.c
 * @brief AI agent access and NFC report via Sentino MQTT
 *
 * Replaces the old HTTPS-based mindchat API with MQTT-based flow:
 * - agora_agent_device_access: request Agora token + channel
 * - agora_agent_nfc_report: report NFC tag + optionally get RTC info
 *
 * Uses FreeRTOS event group to synchronize async MQTT response.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "event_groups.h"

#include "sentino_ai.h"
#include "app_sentino.h"
#include "sentino_protocol.h"
#include "app_json.h"

#include "app_log.h"

#define LOG_TAG "SENTINO_AI"

/* Event bit for response received */
#define AI_RESP_EVENT_BIT (1 << 0)

/* Response wait timeout */
#define AI_RESP_TIMEOUT_MS 10000

/* Sync state */
static EventGroupHandle_t g_ai_event = NULL;
static sentino_ai_access_t g_pending_access;
static volatile int g_pending_res = -1;

/**
 * @brief Parse AI access response data fields
 */
static int parse_access_response(char *payload, sentino_ai_access_t *access) {
    /* Extract fields from "data" object within payload.
     * json_extract_string is destructive so we work on copies for each field. */

    /* We need to extract multiple fields from the same JSON.
     * Since json_extract_string null-terminates in-place, we'll
     * work carefully: extract deeper fields first from a copy. */
    char *work = pvPortMalloc(strlen(payload) + 1);
    if (!work) return -1;

    /* appId */
    strcpy(work, payload);
    char *val = json_extract_string(work, "appId");
    if (val) {
        strncpy(access->app_id, val, sizeof(access->app_id) - 1);
        access->app_id[sizeof(access->app_id) - 1] = '\0';
    }

    /* rtcToken */
    strcpy(work, payload);
    val = json_extract_string(work, "rtcToken");
    if (val) {
        strncpy(access->token, val, sizeof(access->token) - 1);
        access->token[sizeof(access->token) - 1] = '\0';
    }

    /* channelName */
    strcpy(work, payload);
    val = json_extract_string(work, "channelName");
    if (val) {
        strncpy(access->channel, val, sizeof(access->channel) - 1);
        access->channel[sizeof(access->channel) - 1] = '\0';
    }

    /* uid — numeric field, extract manually */
    strcpy(work, payload);
    const char *uid_key = strstr(work, "\"uid\"");
    if (uid_key) {
        uid_key += 5;
        while (*uid_key == ' ' || *uid_key == ':' || *uid_key == '\t') uid_key++;
        access->uid = (uint32_t)strtoul(uid_key, NULL, 10);
    }

    vPortFree(work);

    /* Validate required fields */
    if (access->app_id[0] == '\0' || access->token[0] == '\0' ||
        access->channel[0] == '\0') {
        IPRO_LOGE(LOG_TAG, "Missing required fields in AI response");
        return -2;
    }

    return 0;
}

/**
 * @brief Response handler — called from app_sentino when report_response arrives
 */
static void ai_response_handler(const char *code, char *payload) {
    IPRO_LOGI(LOG_TAG, "Response: code='%s' payload_len=%u",
              code ? code : "(null)",
              payload ? (unsigned)strlen(payload) : 0);

    /* Log full payload for debugging */
    if (payload) {
        uint16_t plen = (uint16_t)strlen(payload);
        /* Print in chunks to avoid log truncation */
        for (uint16_t off = 0; off < plen; off += 200) {
            uint16_t chunk = (plen - off > 200) ? 200 : (plen - off);
            IPRO_LOGI(LOG_TAG, "  payload[%u]: %.*s", off, chunk, payload + off);
        }
    }

    if (!code) return;

    if (strcmp(code, "agora_agent_device_access") == 0 ||
        strcmp(code, "agora_agent_nfc_report") == 0) {

        /* Parse response code */
        int res = 0;
        sentino_proto_parse_message(payload, NULL, NULL, &res);
        g_pending_res = res;
        IPRO_LOGI(LOG_TAG, "AI response: res=%d", res);

        if (res == 0) {
            int rc = parse_access_response(payload, &g_pending_access);
            IPRO_LOGI(LOG_TAG, "Parse result: %d appId='%.20s' ch='%.20s' uid=%lu",
                      rc, g_pending_access.app_id, g_pending_access.channel,
                      (unsigned long)g_pending_access.uid);
        }

        /* Signal waiting task */
        if (g_ai_event) {
            xEventGroupSetBits(g_ai_event, AI_RESP_EVENT_BIT);
        }
    } else {
        IPRO_LOGI(LOG_TAG, "Ignoring response with code='%s' (not AI)", code);
    }
}

void sentino_ai_init(void) {
    if (!g_ai_event) {
        g_ai_event = xEventGroupCreate();
    }
    app_sentino_set_response_handler(ai_response_handler);
    IPRO_LOGI(LOG_TAG, "AI module initialized");
}

int sentino_ai_request_access(sentino_ai_access_t *access) {
    if (!access || !g_ai_event) return -1;
    if (!app_sentino_is_connected()) {
        IPRO_LOGE(LOG_TAG, "MQTT not connected");
        return -2;
    }

    /* Clear pending state */
    memset(&g_pending_access, 0, sizeof(g_pending_access));
    g_pending_res = -1;
    xEventGroupClearBits(g_ai_event, AI_RESP_EVENT_BIT);

    /* Send agora_agent_device_access report */
    IPRO_LOGI(LOG_TAG, "Requesting AI agent access...");
    int ret = app_sentino_report("agora_agent_device_access", NULL, 1);
    if (ret != 0) {
        IPRO_LOGE(LOG_TAG, "Failed to send device_access report: %d", ret);
        return ret;
    }

    /* Wait for response */
    EventBits_t bits = xEventGroupWaitBits(g_ai_event, AI_RESP_EVENT_BIT,
                                            pdTRUE, pdFALSE,
                                            pdMS_TO_TICKS(AI_RESP_TIMEOUT_MS));
    if (!(bits & AI_RESP_EVENT_BIT)) {
        IPRO_LOGE(LOG_TAG, "AI access response timeout (%d ms)", AI_RESP_TIMEOUT_MS);
        return -3;
    }

    if (g_pending_res != 0) {
        IPRO_LOGE(LOG_TAG, "AI access denied: res=%d", g_pending_res);
        return -4;
    }

    /* Copy result */
    memcpy(access, &g_pending_access, sizeof(*access));

    IPRO_LOGI(LOG_TAG, "AI access granted: ch='%s' uid=%lu",
              access->channel, (unsigned long)access->uid);
    return 0;
}

int sentino_ai_nfc_report(const char *nfc_id, int only_report,
                        sentino_ai_access_t *access) {
    if (!nfc_id) return -1;
    if (!only_report && !access) return -1;
    if (!app_sentino_is_connected()) return -2;

    /* Clear pending state */
    if (!only_report) {
        memset(&g_pending_access, 0, sizeof(g_pending_access));
        g_pending_res = -1;
        xEventGroupClearBits(g_ai_event, AI_RESP_EVENT_BIT);
    }

    /* Build data JSON */
    char data[128];
    snprintf(data, sizeof(data),
             "\"nfcIdentifier\":\"%s\",\"onlyReport\":%d",
             nfc_id, only_report);

    IPRO_LOGI(LOG_TAG, "NFC report: id='%s' onlyReport=%d", nfc_id, only_report);
    int ret = app_sentino_report("agora_agent_nfc_report", data,
                               only_report ? 0 : 1);
    if (ret != 0) {
        IPRO_LOGE(LOG_TAG, "Failed to send nfc_report: %d", ret);
        return ret;
    }

    if (only_report) {
        return 0; /* no response expected */
    }

    /* Wait for response with RTC info */
    EventBits_t bits = xEventGroupWaitBits(g_ai_event, AI_RESP_EVENT_BIT,
                                            pdTRUE, pdFALSE,
                                            pdMS_TO_TICKS(AI_RESP_TIMEOUT_MS));
    if (!(bits & AI_RESP_EVENT_BIT)) {
        IPRO_LOGE(LOG_TAG, "NFC access response timeout");
        return -3;
    }

    if (g_pending_res != 0) {
        IPRO_LOGE(LOG_TAG, "NFC access denied: res=%d", g_pending_res);
        return -4;
    }

    memcpy(access, &g_pending_access, sizeof(*access));

    IPRO_LOGI(LOG_TAG, "NFC access granted: ch='%s' uid=%lu",
              access->channel, (unsigned long)access->uid);
    return 0;
}
