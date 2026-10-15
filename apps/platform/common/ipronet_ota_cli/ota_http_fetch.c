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

/* ota_http_fetch.c - HTTP/1.0 GET of a whole body into the heap. */
#include <stdio.h>
#include <string.h>

#include <FreeRTOS.h>

#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "lwip/ip4_addr.h"

#include "ota_http_fetch.h"

#define HTTP_HEAD_MAX  1024U
#define HTTP_RCV_S     5

static int resolve(const char *host, struct sockaddr_in *sa)
{
    ip4_addr_t ip;

    if (ip4addr_aton(host, &ip)) {
        sa->sin_addr.s_addr = ip4_addr_get_u32(&ip);
        return 0;
    }
#if LWIP_DNS
    struct addrinfo *ai = NULL;

    if (lwip_getaddrinfo(host, NULL, NULL, &ai) == 0 && ai != NULL) {
        sa->sin_addr = ((struct sockaddr_in *)ai->ai_addr)->sin_addr;
        lwip_freeaddrinfo(ai);
        return 0;
    }
#endif
    return -1;
}

int ota_http_fetch(const char *url, uint8_t **out, uint32_t *out_len)
{
    ota_http_url_t u;
    struct sockaddr_in sa;
    struct timeval tv = { HTTP_RCV_S, 0 };
    char req[256];
    char head[HTTP_HEAD_MAX];
    uint8_t *buf = NULL;
    uint32_t hl = 0;
    uint32_t clen = 0;
    uint32_t head_len = 0;
    uint32_t got;
    int status = 0;
    int s;
    int n;
    int rc = -3;

    if (ota_http_parse_url(url, &u) != 0) {
        return -1;
    }
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = lwip_htons(u.port);
    if (resolve(u.host, &sa) != 0) {
        return -2;
    }
    s = lwip_socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        return -3;
    }
    lwip_setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    if (lwip_connect(s, (struct sockaddr *)&sa, sizeof(sa)) != 0) {
        goto out;
    }
    n = snprintf(req, sizeof(req),
                 "GET %s HTTP/1.0\r\nHost: %s\r\nConnection: close\r\n\r\n",
                 u.path, u.host);
    if (n <= 0 || n >= (int)sizeof(req) || lwip_send(s, req, n, 0) != n) {
        goto out;
    }
    for (;;) {
        int r = lwip_recv(s, head + hl, sizeof(head) - hl, 0);
        int ph;

        if (r <= 0) {
            goto out;
        }
        hl += (uint32_t)r;
        ph = ota_http_parse_head(head, hl, &status, &clen, &head_len);
        if (ph == 0) {
            break;
        }
        if (ph < 0 || hl >= sizeof(head)) {
            rc = -5;
            goto out;
        }
    }
    if (status != 200 || clen == 0 || clen > OTA_HTTP_MAX_BODY) {
        rc = -5;
        goto out;
    }
    buf = pvPortMalloc(clen);
    if (buf == NULL) {
        rc = -4;
        goto out;
    }
    got = hl - head_len;
    if (got > clen) {
        got = clen;
    }
    memcpy(buf, head + head_len, got);
    while (got < clen) {
        int r = lwip_recv(s, buf + got, clen - got, 0);

        if (r <= 0) {
            goto out;
        }
        got += (uint32_t)r;
    }
    *out = buf;
    *out_len = clen;
    buf = NULL;
    rc = 0;
out:
    if (buf != NULL) {
        vPortFree(buf);
    }
    lwip_close(s);
    return rc;
}
