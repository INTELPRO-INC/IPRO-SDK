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

#ifndef SENTINO_AUTH_H
#define SENTINO_AUTH_H

#include <stdint.h>
#include <stddef.h>

/**
 * @brief Device triple for Sentino MQTT authentication
 *
 * Per rlink v2 spec:
 *   uuid   -- device identifier (e.g. "rn014ebe0C20E122")
 *   secret -- device secret (32-char hex string)
 *   pid    -- product ID / model string
 */
typedef struct {
    const char *uuid;
    const char *secret;
    const char *pid;
} sentino_device_triple_t;

/**
 * @brief MQTT connection credentials (computed from triple)
 *
 * Per rlink v2 spec (reference SDK uses hmacSha1 + ts=0):
 *   client_id: "rlink_${uuid}_V2"
 *   username:  "${uuid}|signMethod=hmacSha1,ts=0"
 *   password:  hex(hmacSha1("uuid=${uuid},ts=0", secret))  -- 40 hex chars
 */
typedef struct {
    char client_id[64];
    char username[128];
    char password[48];  /* 40 hex chars + null (SHA1=20 bytes) */
} sentino_mqtt_creds_t;

/**
 * @brief Compute MQTT credentials from device triple using HMAC-SHA1
 *
 * Uses fixed ts=0 per reference SDK convention.
 *
 * @param triple Device triple (uuid, secret, pid)
 * @param creds  Output credentials
 * @return 0 on success, negative on failure
 */
int sentino_auth_compute(const sentino_device_triple_t *triple,
                       sentino_mqtt_creds_t *creds);

#endif /* SENTINO_AUTH_H */
