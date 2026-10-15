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

/* ota_http_parse.c - URL and response-head parsing, no I/O. */
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "ota_http_fetch.h"

int ota_http_parse_url(const char *url, ota_http_url_t *u)
{
    const char *host;
    const char *path;
    const char *colon;
    size_t hl;

    if (strncmp(url, "http://", 7) != 0) {
        return -1;
    }
    host = url + 7;
    path = strchr(host, '/');
    if (path == NULL) {
        path = host + strlen(host);
    }
    colon = memchr(host, ':', (size_t)(path - host));
    u->port = 80;
    if (colon != NULL) {
        char *end;
        unsigned long port = strtoul(colon + 1, &end, 10);

        if (end != path || port == 0 || port > 65535) {
            return -1;
        }
        u->port = (uint16_t)port;
    } else {
        colon = path;
    }
    hl = (size_t)(colon - host);
    if (hl == 0 || hl >= sizeof(u->host) || strlen(path) >= sizeof(u->path)) {
        return -1;
    }
    memcpy(u->host, host, hl);
    u->host[hl] = '\0';
    strcpy(u->path, (*path != '\0') ? path : "/");
    return 0;
}

static int starts_with_ci(const char *s, const char *end, const char *name)
{
    for (; *name != '\0'; s++, name++) {
        if (s >= end || tolower((unsigned char)*s) != *name) {
            return 0;
        }
    }
    return 1;
}

int ota_http_parse_head(const char *buf, uint32_t len, int *status,
                        uint32_t *content_len, uint32_t *head_len)
{
    const char *end = NULL;
    const char *line = buf;
    int have_len = 0;

    for (uint32_t i = 0; i + 3 < len; i++) {
        if (memcmp(buf + i, "\r\n\r\n", 4) == 0) {
            end = buf + i + 2;
            *head_len = i + 4;
            break;
        }
    }
    if (end == NULL) {
        return 1;
    }
    if (end - buf < 12 || memcmp(buf, "HTTP/1.", 7) != 0) {
        return -1;
    }
    *status = atoi(buf + 9);
    while ((line = memchr(line, '\n', (size_t)(end - line))) != NULL) {
        line++;
        if (line >= end) {
            break;
        }
        if (starts_with_ci(line, end, "content-length:")) {
            *content_len = (uint32_t)strtoul(line + 15, NULL, 10);
            have_len = 1;
        } else if (starts_with_ci(line, end, "transfer-encoding:")) {
            return -1;
        }
    }
    return have_len ? 0 : -1;
}
