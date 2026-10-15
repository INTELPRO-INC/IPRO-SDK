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

/* ipronet_ota_cli.c - shell command ipro6s_ota: HTTP fetch + OTA. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include "shell.h"
#include "ipronet_host.h"
#include "ipronet_ota.h"
#include "ota_http_fetch.h"

#define OTA_TASK_STACK_WORDS  2048
#define OTA_TASK_PRIO         (tskIDLE_PRIORITY + 3)

typedef struct {
    const uint8_t *buf;
    uint32_t len;
    uint32_t off;
    uint32_t abort_at;   /* test hook: reads past it fail; 0 = off */
} mem_src_t;

static char s_url[200];
static uint32_t s_abort_at;
static volatile int s_running;

static int mem_read(void *ctx, uint8_t *dst, uint32_t n)
{
    mem_src_t *m = ctx;

    if (m->abort_at != 0 && m->off + n > m->abort_at) {
        return -1;
    }
    if (n > m->len - m->off) {
        n = m->len - m->off;
    }
    memcpy(dst, m->buf + m->off, n);
    m->off += n;
    return (int)n;
}

static uint32_t ms_since(TickType_t t0)
{
    return (uint32_t)((xTaskGetTickCount() - t0) * portTICK_PERIOD_MS);
}

static const char *dev_err_name(uint16_t e)
{
    static const char *const names[] = {
        "STATE", "NO_SLOT", "HEADER", "TOO_BIG", "CKSUM", "GAP", "SIZE", "MISC",
    };

    static const char *const b2[] = {
        "B2_MAGIC", "B2_HEADER", "B2_SHA", "B2_XZ", "B2_TOO_BIG", "B2_NOT_FW",
        "B2_SECURE", "B2_RSVD", "B2_READ",
    };

    if (e >= IPRONET_OTA_E_STATE && e <= IPRONET_OTA_E_MISC) {
        return names[e - IPRONET_OTA_E_STATE];
    }
    if (e > IPRONET_OTA_E_BOOT2 && e <= IPRONET_OTA_E_BOOT2_READ) {
        return b2[e - IPRONET_OTA_E_BOOT2 - 1];
    }
    return (e != 0) ? "RNM_STATUS" : "-";
}

static void print_result(int rc, uint32_t total, uint32_t fetch,
                         const ipronet_ota_report_t *r)
{
    static const char *const ph[IPRONET_OTA_T_MAX] = {
        "xfer", "commit", "reboot", "bringup", "assoc", "ip",
    };

    if (rc == IPRONET_OK) {
        printf("[ota] RESULT OK");
    } else {
        printf("[ota] RESULT ERR rc=%d dev=%u(%s) phase=%s", rc,
               (unsigned)r->dev_err, dev_err_name(r->dev_err),
               (rc == IPRONET_ERR_OTA_PKG) ? "pre" : ph[r->phase]);
    }
    printf(" total=%lu fetch=%lu", (unsigned long)total, (unsigned long)fetch);
    for (int i = 0; i < IPRONET_OTA_T_MAX; i++) {
        printf(" %s=%lu", ph[i], (unsigned long)r->ms[i]);
    }
    printf(" bytes=%lu retries=%lu rail=%u inst=%u reconn=%u\r\n",
           (unsigned long)r->bytes, (unsigned long)r->retries,
           (unsigned)r->power_cycled, (unsigned)r->installed,
           (unsigned)r->reconnected);
}

static void ota_task(void *arg)
{
    TickType_t t0 = xTaskGetTickCount();
    ipronet_ota_report_t rep;
    ipronet_ota_opts_t opts;
    ipronet_ota_src_t src;
    mem_src_t m;
    uint8_t *buf = NULL;
    uint32_t len = 0;
    uint32_t fetch_ms;
    int rc;

    (void)arg;
    if (!ipronet_is_ready()) {
        printf("[ota] RESULT ERR rc=%d phase=ready\r\n", IPRONET_ERR_STATE);
        goto done;
    }
    rc = ota_http_fetch(s_url, &buf, &len);
    fetch_ms = ms_since(t0);
    if (rc != 0) {
        printf("[ota] RESULT ERR rc=%d phase=fetch\r\n", rc);
        goto done;
    }
    printf("[ota] fetch %lu B in %lu ms\r\n", (unsigned long)len,
           (unsigned long)fetch_ms);
    m.buf = buf;
    m.len = len;
    m.off = 0;
    m.abort_at = s_abort_at;
    src.size = len;
    src.read = mem_read;
    src.ctx = &m;
    ipronet_ota_opts_default(&opts);
    rc = ipronet_ota_update(&src, &opts, &rep);
    print_result(rc, ms_since(t0), fetch_ms, &rep);
done:
    if (buf != NULL) {
        vPortFree(buf);
    }
    s_running = 0;
    vTaskDelete(NULL);
}

static int cmd_ipro6s_ota(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: ipro6s_ota <http://host[:port]/path> [abort_at=<bytes>]\r\n");
        return -1;
    }
    if (s_running) {
        printf("[ota] busy\r\n");
        return -1;
    }
    strncpy(s_url, argv[1], sizeof(s_url) - 1);
    s_url[sizeof(s_url) - 1] = '\0';
    s_abort_at = 0;
    if (argc >= 3 && strncmp(argv[2], "abort_at=", 9) == 0) {
        s_abort_at = (uint32_t)strtoul(argv[2] + 9, NULL, 0);
    }
    s_running = 1;
    if (xTaskCreate(ota_task, "ipro6s_ota", OTA_TASK_STACK_WORDS, NULL,
                    OTA_TASK_PRIO, NULL) != pdPASS) {
        s_running = 0;
        printf("[ota] RESULT ERR rc=%d phase=task\r\n", IPRONET_ERR_NOMEM);
        return -1;
    }
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_ipro6s_ota, ipro6s_ota,
                       "IPRO6S OTA: ipro6s_ota <http url> [abort_at=<bytes>]");
