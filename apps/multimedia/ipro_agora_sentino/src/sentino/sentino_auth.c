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
 * @file sentino_auth.c
 * @brief HMAC-SHA1 authentication for Sentino MQTT (rlink v2)
 *
 * Computes MQTT client_id, username, password from device triple.
 * Aligned with reference SDK: uses hmacSha1 with fixed ts=0.
 *
 * Per spec:
 *   clientId: "rlink_${uuid}_V2"
 *   username: "${uuid}|signMethod=hmacSha1,ts=0"
 *   password: hex(hmacSha1("uuid=${uuid},ts=0", deviceSecret))
 */

#include <stdio.h>
#include <string.h>
#include "sentino_auth.h"
#include "mbedtls/md.h"

#define HMAC_SHA1_LEN 20  /* SHA-1 output = 20 bytes = 40 hex chars */

int sentino_auth_compute(const sentino_device_triple_t *triple,
                       sentino_mqtt_creds_t *creds) {
    if (!triple || !triple->uuid || !triple->secret || !creds) {
        return -1;
    }

    /* client_id: "rlink_${uuid}_V2" */
    snprintf(creds->client_id, sizeof(creds->client_id),
             "rlink_%s_V2", triple->uuid);

    /* username: "${uuid}|signMethod=hmacSha1,ts=0" */
    snprintf(creds->username, sizeof(creds->username),
             "%s|signMethod=hmacSha1,ts=0", triple->uuid);

    /* password: HMAC-SHA1("uuid=${uuid},ts=0", secret) */
    char content[128];
    int content_len = snprintf(content, sizeof(content),
                               "uuid=%s,ts=0", triple->uuid);
    if (content_len < 0 || (size_t)content_len >= sizeof(content)) {
        return -2;
    }

    /* Compute HMAC-SHA1 */
    uint8_t hmac[HMAC_SHA1_LEN];
    const mbedtls_md_info_t *md_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA1);
    if (!md_info) {
        return -3;
    }

    int ret = mbedtls_md_hmac(md_info,
                              (const uint8_t *)triple->secret,
                              strlen(triple->secret),
                              (const uint8_t *)content,
                              (size_t)content_len,
                              hmac);
    if (ret != 0) {
        return -4;
    }

    /* Convert to 40-char hex string */
    for (int i = 0; i < HMAC_SHA1_LEN; i++) {
        snprintf(&creds->password[i * 2], 3, "%02x", hmac[i]);
    }
    creds->password[HMAC_SHA1_LEN * 2] = '\0';

    return 0;
}
