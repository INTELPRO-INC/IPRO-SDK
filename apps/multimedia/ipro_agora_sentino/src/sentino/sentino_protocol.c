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
 * @file sentino_protocol.c
 * @brief Sentino rlink v2 JSON envelope builder/parser
 *
 * Message formats per mqtt_ai.md spec:
 *
 * Report (device -> cloud):
 *   Topic: rlink/v2/${pid}/${uuid}/report
 *   {"id":"xxx","ts":123,"code":"xxx","data":{...},"ack":0|1}
 *
 * Report response (cloud -> device):
 *   Topic: rlink/v2/${pid}/${uuid}/report_response
 *   {"res":0,"msg":"success","id":"xxx","ts":123,"code":"xxx","data":{...}}
 *
 * Issue (cloud -> device):
 *   Topic: rlink/v2/${pid}/${uuid}/issue
 *   {"id":"xxx","ts":123,"code":"xxx","data":{...}}
 *
 * Issue response (device -> cloud):
 *   Topic: rlink/v2/${pid}/${uuid}/issue_response
 *   {"res":0,"id":"xxx","ts":123,"code":"xxx","data":{...}}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "sentino_protocol.h"
#include "app_json.h"

/* Monotonic counter for message ID generation */
static uint32_t g_msg_seq = 0;

int sentino_proto_build_report_topic(const char *pid, const char *uuid,
                                   char *buf, size_t buf_len) {
    if (!pid || !uuid || !buf) return -1;
    return snprintf(buf, buf_len, "rlink/v2/%s/%s/report", pid, uuid);
}

int sentino_proto_build_issue_response_topic(const char *pid, const char *uuid,
                                           char *buf, size_t buf_len) {
    if (!pid || !uuid || !buf) return -1;
    return snprintf(buf, buf_len, "rlink/v2/%s/%s/issue_response", pid, uuid);
}

int sentino_proto_build_report(const char *code, const char *data_json,
                             int ack, char *buf, size_t buf_len) {
    if (!code || !buf || buf_len == 0) return -1;

    char id[20];
    sentino_proto_gen_id(id, sizeof(id));

    uint32_t ts = (uint32_t)time(NULL);

    const char *data = data_json ? data_json : "";
    int n = snprintf(buf, buf_len,
                     "{\"id\":\"%s\",\"ts\":%lu,\"code\":\"%s\","
                     "\"data\":{%s},\"ack\":%d}",
                     id, (unsigned long)ts, code, data, ack);

    if (n < 0 || (size_t)n >= buf_len) return -2;
    return n;
}

int sentino_proto_build_response(const char *id, const char *code,
                               int res, const char *data_json,
                               char *buf, size_t buf_len) {
    if (!id || !code || !buf || buf_len == 0) return -1;

    uint32_t ts = (uint32_t)time(NULL);

    const char *data = data_json ? data_json : "";
    int n = snprintf(buf, buf_len,
                     "{\"res\":%d,\"id\":\"%s\",\"ts\":%lu,"
                     "\"code\":\"%s\",\"data\":{%s}}",
                     res, id, (unsigned long)ts, code, data);

    if (n < 0 || (size_t)n >= buf_len) return -2;
    return n;
}

int sentino_proto_parse_message(char *payload,
                              const char **id,
                              const char **code,
                              int *res) {
    if (!payload) return -1;

    /* Make a copy for second extraction (json_extract_string is destructive) */
    /* We parse id first, then code from the original since they are at
     * different positions and the destructive null-termination is localized */

    if (id) {
        *id = json_extract_string(payload, "id");
    }

    if (code) {
        *code = json_extract_string(payload, "code");
    }

    if (res) {
        /* Look for "res": integer value */
        const char *p = strstr(payload, "\"res\"");
        if (p) {
            p += 5; /* skip "res" */
            while (*p == ' ' || *p == ':' || *p == '\t') p++;
            *res = (int)strtol(p, NULL, 10);
        } else {
            *res = 0;
        }
    }

    return 0;
}

void sentino_proto_gen_id(char *buf, size_t buf_len) {
    uint32_t ts = (uint32_t)time(NULL);
    uint32_t seq = __atomic_add_fetch(&g_msg_seq, 1, __ATOMIC_RELAXED);
    snprintf(buf, buf_len, "%lu%04lu",
             (unsigned long)ts, (unsigned long)(seq % 10000));
}
