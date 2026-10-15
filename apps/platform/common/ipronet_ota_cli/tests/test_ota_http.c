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

#include <stdio.h>
#include <string.h>

#include "ota_http_fetch.h"

static int g_fail;

#define CHECK(c)                                                    \
    do {                                                            \
        if (!(c)) {                                                 \
            printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #c);     \
            g_fail++;                                               \
        }                                                           \
    } while (0)

static void test_url(void)
{
    ota_http_url_t u;

    CHECK(ota_http_parse_url("http://192.0.2.10:8000/A.xz.ota", &u) == 0);
    CHECK(strcmp(u.host, "192.0.2.10") == 0);
    CHECK(u.port == 8000);
    CHECK(strcmp(u.path, "/A.xz.ota") == 0);
    CHECK(ota_http_parse_url("http://peer/x", &u) == 0);
    CHECK(u.port == 80);
    CHECK(ota_http_parse_url("http://peer", &u) == 0);
    CHECK(strcmp(u.path, "/") == 0);
    CHECK(ota_http_parse_url("https://peer/x", &u) == -1);
    CHECK(ota_http_parse_url("http://:80/x", &u) == -1);
    CHECK(ota_http_parse_url("http://peer:99999/x", &u) == -1);
    CHECK(ota_http_parse_url("http://peer:80x/x", &u) == -1);
}

static void test_head(void)
{
    const char *ok = "HTTP/1.0 200 OK\r\nServer: SimpleHTTP\r\n"
                     "Content-Length: 557720\r\n\r\nXZ";
    const char *lower = "HTTP/1.1 200 OK\r\ncontent-length: 12\r\n\r\n";
    const char *nf = "HTTP/1.0 404 File not found\r\nContent-Length: 3\r\n\r\n";
    const char *nolen = "HTTP/1.0 200 OK\r\nServer: x\r\n\r\n";
    const char *chunked = "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n"
                          "Content-Length: 5\r\n\r\n";
    int status;
    uint32_t clen;
    uint32_t hl;

    CHECK(ota_http_parse_head(ok, strlen(ok), &status, &clen, &hl) == 0);
    CHECK(status == 200);
    CHECK(clen == 557720);
    CHECK(hl == strlen(ok) - 2);
    CHECK(ota_http_parse_head(lower, strlen(lower), &status, &clen, &hl) == 0);
    CHECK(clen == 12);
    CHECK(ota_http_parse_head(nf, strlen(nf), &status, &clen, &hl) == 0);
    CHECK(status == 404);
    CHECK(ota_http_parse_head(ok, 20, &status, &clen, &hl) == 1);
    CHECK(ota_http_parse_head(nolen, strlen(nolen), &status, &clen, &hl) == -1);
    CHECK(ota_http_parse_head(chunked, strlen(chunked), &status, &clen, &hl) == -1);
    CHECK(ota_http_parse_head("FTP/1.0 200 OK\r\n\r\n", 18, &status, &clen, &hl) == -1);
}

int main(void)
{
    test_url();
    test_head();
    printf("%s (%d failures)\n", g_fail ? "FAILED" : "PASSED", g_fail);
    return g_fail != 0;
}
