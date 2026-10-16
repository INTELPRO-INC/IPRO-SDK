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

#ifndef SENTINO_PROTOCOL_H
#define SENTINO_PROTOCOL_H

#include <stdint.h>
#include <stddef.h>

/**
 * @brief Build rlink v2 report topic
 *
 * Format: "rlink/v2/${pid}/${uuid}/report"
 */
int sentino_proto_build_report_topic(const char *pid, const char *uuid,
                                   char *buf, size_t buf_len);

/**
 * @brief Build rlink v2 issue response topic
 *
 * Format: "rlink/v2/${pid}/${uuid}/issue_response"
 */
int sentino_proto_build_issue_response_topic(const char *pid, const char *uuid,
                                           char *buf, size_t buf_len);

/**
 * @brief Build rlink v2 event report message
 *
 * Format: {"id":"...","ts":...,"code":"...","data":{...},"ack":N}
 *
 * @param code      Event code (e.g. "reset", "time", "info")
 * @param data_json Inner JSON for "data" field (without braces), or NULL for empty
 * @param ack       0 = no reply needed, 1 = reply expected
 * @param buf       Output buffer
 * @param buf_len   Output buffer size
 * @return Number of bytes written (excluding null), or negative on error
 */
int sentino_proto_build_report(const char *code, const char *data_json,
                             int ack, char *buf, size_t buf_len);

/**
 * @brief Build rlink v2 issue response message
 *
 * Format: {"res":0,"id":"...","ts":...,"code":"...","data":{...}}
 *
 * @param id        Message ID from the issue (must match)
 * @param code      Command code from the issue
 * @param res       Response code (0 = success)
 * @param data_json Inner JSON for "data" field, or NULL for empty
 * @param buf       Output buffer
 * @param buf_len   Output buffer size
 * @return Number of bytes written, or negative on error
 */
int sentino_proto_build_response(const char *id, const char *code,
                               int res, const char *data_json,
                               char *buf, size_t buf_len);

/**
 * @brief Parse rlink v2 message — extract id, code, and data pointer
 *
 * This is a lightweight parser using strstr. The payload buffer may be
 * modified in-place (json_extract_string is destructive).
 *
 * @param payload   Raw JSON payload (will be modified)
 * @param id        Output: message ID (pointer into payload)
 * @param code      Output: event/command code (pointer into payload)
 * @param res       Output: response code (0 if not present)
 * @return 0 on success, negative on parse error
 */
int sentino_proto_parse_message(char *payload,
                              const char **id,
                              const char **code,
                              int *res);

/**
 * @brief Generate a unique message ID string
 *
 * Uses a monotonic counter + timestamp for uniqueness.
 *
 * @param buf     Output buffer (must be >= 20 bytes)
 * @param buf_len Output buffer size
 */
void sentino_proto_gen_id(char *buf, size_t buf_len);

#endif /* SENTINO_PROTOCOL_H */
