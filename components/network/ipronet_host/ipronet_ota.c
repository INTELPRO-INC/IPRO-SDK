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

/*
 * ipronet_ota.c - IPRO6S firmware update: transfer, device reboot,
 * re-enumeration, bring-up and STA reconnect.
 */
#include <stdio.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include "ipronet_priv.h"
#include "ipronet_ota.h"
#include "ipronet_ota_proto.h"

#define OTA_SETTLE_MS    300U     /* COMMIT ack to 1st enumerate */
#define OTA_FAIL_WIN_MS  10000U   /* no COMMIT: old FW boots ~5 s */
#define OTA_RAIL_OFF_MS  300U
#define OTA_STA_DOWN_MS  500U
#define OTA_RSP_MAX      64U

static uint32_t ms_since(TickType_t t0)
{
    return (uint32_t)((xTaskGetTickCount() - t0) * portTICK_PERIOD_MS);
}

static int ctrl_req(void *ctx, const uint8_t *body, uint16_t len,
                    uint8_t *rsp, uint16_t *rsp_len, uint16_t *status,
                    uint32_t timeout_ms)
{
    uint8_t buf[sizeof(ipronet_ctrl_hdr_t) + OTA_RSP_MAX];
    const ipronet_ctrl_hdr_t *h = (const ipronet_ctrl_hdr_t *)buf;
    uint16_t n = sizeof(buf);
    int rc;

    (void)ctx;
    rc = ipronet_ctrl_request(IPRONET_CMD_OTA, body, len, buf, &n,
                              timeout_ms);
    if (rc != IPRONET_OK) {
        return rc;
    }
    if (n < sizeof(*h)) {
        return IPRONET_ERR_OTA_DEVICE;
    }
    *status = h->status;
    n -= sizeof(*h);
    if (n > *rsp_len) {
        n = *rsp_len;
    }
    memcpy(rsp, buf + sizeof(*h), n);
    *rsp_len = n;
    return IPRONET_OK;
}

static int ctrl_probe(void *ctx)
{
    (void)ctx;
    return ipronet_ctrl_get_mac() == IPRONET_OK;
}

static int read_exact(const ipronet_ota_src_t *src, uint8_t *buf,
                      uint32_t len)
{
    uint32_t got = 0;

    while (got < len) {
        int r = src->read(src->ctx, buf + got, len - got);

        if (r <= 0) {
            return IPRONET_ERR_OTA_SOURCE;
        }
        got += (uint32_t)r;
    }
    return IPRONET_OK;
}

/* Re-enumerate without a power cycle; one rail cycle (D0 parked low,
 * MEDIA strap) if the window passes with no device. */
static int wait_device(uint32_t window_ms, ipronet_ota_report_t *rep)
{
    TickType_t t0 = xTaskGetTickCount();
    TickType_t t_try;
    uint32_t t_enum = 0;
    uint32_t up;
    uint8_t mac[6];
    int rc;

    rep->phase = IPRONET_OTA_T_REBOOT;
    ipronet_host_teardown();
    vTaskDelay(pdMS_TO_TICKS(OTA_SETTLE_MS));
    t_try = xTaskGetTickCount();
    rc = ipronet_host_restart(window_ms, &t_enum);
    if (rc == IPRONET_ERR_NO_DEVICE && ipronet_device_power(0) == IPRONET_OK) {
        printf("[ota] no device after %lu ms, rail cycled\r\n",
               (unsigned long)ms_since(t0));
        vTaskDelay(pdMS_TO_TICKS(OTA_RAIL_OFF_MS));
        (void)ipronet_device_power(1);
        rep->power_cycled = 1;
        t_try = xTaskGetTickCount();
        rc = ipronet_host_restart(window_ms, &t_enum);
    }
    if (rc != IPRONET_OK) {
        printf("[ota] device not back: %d\r\n", rc);
        return (rc == IPRONET_ERR_NO_DEVICE) ? IPRONET_ERR_OTA_REBOOT : rc;
    }
    up = ms_since(t_try) - t_enum;
    rep->ms[IPRONET_OTA_T_BRINGUP] = up;
    rep->ms[IPRONET_OTA_T_REBOOT] = ms_since(t0) - up;
    rep->phase = IPRONET_OTA_T_BRINGUP;
    (void)ipronet_get_mac(mac);
    printf("[ota] device back on sdio after %lu ms, bring-up %lu ms "
           "(mac %02x:%02x:%02x:%02x:%02x:%02x)\r\n",
           (unsigned long)rep->ms[IPRONET_OTA_T_REBOOT], (unsigned long)up,
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return IPRONET_OK;
}

/* boot2 clears the FW entry's activeIndex once FW-B is installed. */
static int check_installed(ipronet_ota_report_t *rep)
{
    ipronet_ota_report_t scratch;
    uint32_t pending = 1;

    memset(&scratch, 0, sizeof(scratch));
    if (ipronet_ota_query_pending(ctrl_req, NULL, &pending, &scratch) !=
        IPRONET_OK) {
        rep->installed = 2;
        printf("[ota] install state unknown (no OTA query on new FW)\r\n");
        return IPRONET_OK;
    }
    /* [7:0] activeIndex, [15:8] boot2's reason for not installing */
    if (pending != 0) {
        uint32_t reason = (pending >> 8) & 0xFFU;

        rep->installed = 0;
        if (reason != 0) {
            rep->dev_err = (uint16_t)(IPRONET_OTA_E_BOOT2 + reason);
            printf("[ota] boot2 refused the package, reason %lu\r\n",
                   (unsigned long)reason);
        } else {
            printf("[ota] boot2 did not install the package\r\n");
        }
        return IPRONET_ERR_OTA_APPLY;
    }
    rep->installed = 1;
    return IPRONET_OK;
}

static void reconnect(const char *ssid, const char *psk,
                      const ipronet_ota_opts_t *opts,
                      ipronet_ota_report_t *rep)
{
    TickType_t t0 = xTaskGetTickCount();
    ipronet_ip_info_t ip;

    rep->phase = IPRONET_OTA_T_ASSOC;
    if (ipronet_ctrl_sta_connect(ssid, psk) != IPRONET_OK) {
        return;
    }
    while (ms_since(t0) < opts->ip_timeout_ms) {
        if (rep->ms[IPRONET_OTA_T_ASSOC] == 0 && ipronet_is_connected()) {
            rep->ms[IPRONET_OTA_T_ASSOC] = ms_since(t0);
            rep->phase = IPRONET_OTA_T_IP;
        }
        if (ipronet_has_ip()) {
            rep->ms[IPRONET_OTA_T_IP] = ms_since(t0);
            rep->reconnected = 1;
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
    if (rep->reconnected && rep->ms[IPRONET_OTA_T_ASSOC] == 0) {
        rep->ms[IPRONET_OTA_T_ASSOC] = rep->ms[IPRONET_OTA_T_IP];
    }
    if (rep->reconnected && ipronet_get_ip(&ip) == IPRONET_OK) {
        printf("[ota] assoc %lu ms, ip %u.%u.%u.%u after %lu ms\r\n",
               (unsigned long)rep->ms[IPRONET_OTA_T_ASSOC],
               ip.ip4[0], ip.ip4[1], ip.ip4[2], ip.ip4[3],
               (unsigned long)rep->ms[IPRONET_OTA_T_IP]);
    } else {
        printf("[ota] no ip within %lu ms\r\n",
               (unsigned long)opts->ip_timeout_ms);
    }
}

void ipronet_ota_opts_default(ipronet_ota_opts_t *o)
{
    memset(o, 0, sizeof(*o));
    o->reboot_window_ms = 60000U;
    o->ip_timeout_ms = 30000U;
}

int ipronet_ota_update(const ipronet_ota_src_t *src,
                       const ipronet_ota_opts_t *opts,
                       ipronet_ota_report_t *rep)
{
    ipronet_ota_opts_t def;
    uint8_t hdr[IPRONET_OTA_HDR_SIZE];
    char ssid[33];
    char psk[65];
    bool armed;
    uint8_t fail_phase;
    TickType_t t;
    int rc;

    if (src == NULL || src->read == NULL || rep == NULL) {
        return IPRONET_ERR_PARAM;
    }
    memset(rep, 0, sizeof(*rep));
    if (opts == NULL) {
        ipronet_ota_opts_default(&def);
        opts = &def;
    }
    if (!ipronet_is_ready() || ipronet_host_busy()) {
        return IPRONET_ERR_STATE;
    }
    rc = read_exact(src, hdr, sizeof(hdr));
    if (rc == IPRONET_OK) {
        rc = ipronet_ota_check_header(hdr, src->size);
    }
    if (rc != IPRONET_OK) {
        return rc;
    }

    ipronet_host_set_busy(true);
    armed = ipronet_ctrl_get_creds(ssid, psk);
    printf("[ota] pkg %lu B ok%s%s%s\r\n", (unsigned long)src->size,
           armed ? ", sta down (reconnect: " : "", armed ? ssid : "",
           armed ? ")" : "");
    if (armed) {
        (void)ipronet_ctrl_sta_disconnect();
        vTaskDelay(pdMS_TO_TICKS(OTA_STA_DOWN_MS));
    }

    rep->phase = IPRONET_OTA_T_XFER;
    t = xTaskGetTickCount();
    rc = ipronet_ota_xfer(src, hdr, ctrl_req, NULL, opts, rep);
    rep->ms[IPRONET_OTA_T_XFER] = ms_since(t);
    if (rc == IPRONET_OK) {
        printf("[ota] xfer %lu B, %lu chunks, %lu retries, %lu ms\r\n",
               (unsigned long)rep->bytes, (unsigned long)rep->chunks,
               (unsigned long)rep->retries,
               (unsigned long)rep->ms[IPRONET_OTA_T_XFER]);
        rep->phase = IPRONET_OTA_T_COMMIT;
        t = xTaskGetTickCount();
        rc = ipronet_ota_commit(ctrl_req, ctrl_probe, NULL, rep);
        rep->ms[IPRONET_OTA_T_COMMIT] = ms_since(t);
    }

    fail_phase = rep->phase;
    if (rc == IPRONET_OK || rc == IPRONET_OTA_COMMIT_ASSUMED) {
        printf("[ota] commit %s %lu ms\r\n",
               (rc == IPRONET_OK) ? "ok" : "unacked",
               (unsigned long)rep->ms[IPRONET_OTA_T_COMMIT]);
        rc = wait_device(opts->reboot_window_ms, rep);
        if (rc == IPRONET_OK) {
            rc = check_installed(rep);
        }
        fail_phase = rep->phase;
    } else {
        printf("[ota] failed %d dev %u\r\n", rc, (unsigned)rep->dev_err);
        /* Silent device: bring the old FW back. */
        if (rc != IPRONET_ERR_OTA_DEVICE && rc != IPRONET_ERR_OTA_SOURCE &&
            !ctrl_probe(NULL)) {
            (void)wait_device(OTA_FAIL_WIN_MS, rep);
        }
    }
    if (armed && ipronet_is_ready()) {
        reconnect(ssid, psk, opts, rep);
    }
    /* On failure report the phase that failed, not the recovery. */
    if (rc != IPRONET_OK) {
        rep->phase = fail_phase;
    }
    ipronet_host_set_busy(false);
    return rc;
}
