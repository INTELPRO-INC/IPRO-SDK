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

/* ota_http_fetch.h - HTTP/1.0 fetch of an OTA package. */
#ifndef OTA_HTTP_FETCH_H
#define OTA_HTTP_FETCH_H

#include <stdint.h>

#define OTA_HTTP_MAX_BODY  (1024U * 1024U)

typedef struct {
    char host[64];
    uint16_t port;
    char path[128];
} ota_http_url_t;

/* http://host[:port][/path]; 0 or -1. */
int ota_http_parse_url(const char *url, ota_http_url_t *u);
/* 0: head complete (status, content_len, head_len set); 1: need more;
 * -1: malformed, chunked, or no Content-Length. */
int ota_http_parse_head(const char *buf, uint32_t len, int *status,
                        uint32_t *content_len, uint32_t *head_len);
/* 0 with *buf from pvPortMalloc; -1 url, -2 resolve, -3 io,
 * -4 no memory, -5 status or length. */
int ota_http_fetch(const char *url, uint8_t **buf, uint32_t *len);

#endif /* OTA_HTTP_FETCH_H */
