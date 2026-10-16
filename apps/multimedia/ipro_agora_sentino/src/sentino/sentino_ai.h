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

#ifndef SENTINO_AI_H
#define SENTINO_AI_H

#include <stdint.h>

/**
 * @brief Agora RTC access info returned by cloud
 */
typedef struct {
    char app_id[64];
    char token[256];
    char channel[64];
    uint32_t uid;
} sentino_ai_access_t;

/**
 * @brief Request Agora AI agent access via Sentino MQTT
 *
 * Sends agora_agent_device_access report (ack=1), waits for cloud response
 * containing appId, rtcToken, channelName, uid.
 *
 * @param access Output: Agora access info (filled on success)
 * @return 0 on success, negative on failure or timeout
 */
int sentino_ai_request_access(sentino_ai_access_t *access);

/**
 * @brief Report NFC tag scan and request AI access
 *
 * Sends agora_agent_nfc_report with nfcIdentifier, waits for response
 * with RTC info.
 *
 * @param nfc_id     NFC tag identifier string
 * @param only_report 1 = report only (no RTC response), 0 = report + get RTC info
 * @param access     Output: Agora access info (filled when only_report=0)
 * @return 0 on success, negative on failure or timeout
 */
int sentino_ai_nfc_report(const char *nfc_id, int only_report,
                        sentino_ai_access_t *access);

/**
 * @brief Initialize sentino_ai module — register response handler
 *
 * Must be called after app_sentino_init().
 */
void sentino_ai_init(void);

#endif /* SENTINO_AI_H */
