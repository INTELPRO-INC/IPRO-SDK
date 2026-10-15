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
 * ipronet_host.c - lifecycle of the ipronet SDIO-WiFi host: start/stop and
 * device power. The transport lives in ipronet_sdio.c, the control plane in
 * ipronet_ctrl.c, the lwIP glue in ipronet_netif.c.
 */
#include <stdio.h>
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#include "ipronet_priv.h"

#define IPRONET_PWR_OFF_MS      300U    /* rail discharge */
#define IPRONET_BOOT_WINDOW_MS  8000U   /* power-on: enumerate retried this long */
#define IPRONET_ENUM_RETRY_MS   500U    /* pause between enumerate attempts */
#define IPRONET_RECOVER_WINDOW_MS 5000U /* re-attach enumerate window */
#define IPRONET_RECOVER_TRIES   3U      /* re-attach attempts per request */
#define IPRONET_RECOVER_GAP_MS  1000U   /* pause after a re-attach attempt */
#define IPRONET_RECOVER_PRIO    (configMAX_PRIORITIES - 3)

static volatile bool s_busy;            /* an OTA owns the link */
static TaskHandle_t s_recover_task;
static volatile uint32_t s_recovers, s_recover_fails;

void ipronet_config_default(ipronet_config_t *cfg)
{
    memset(cfg, 0, sizeof(*cfg));
    cfg->pwr_en_gpio    = 15;        /* EVB: SD 3.3 V switch */
    cfg->pwr_active_low = 1;         /* 0 = rail ON */
    cfg->bus_hz         = 25000000U;
    cfg->auto_reconnect = 1;
    cfg->tx_pace_kbps   = 16000U;
}

/* "Ready" is what the control plane needs, read from the transport rather
 * than from a flag: F1 set up, rx_task draining the doorbell (replies arrive
 * through it) and the device MAC learned. ipronet_start() establishes exactly
 * that; the bench reaches the same state one ipro6s_* step at a time. */
bool ipronet_is_ready(void)
{
    return ipronet_sdio_is_setup() && ipronet_diag_rx_task_running() &&
           ipronet_ctrl_mac_learned();
}

/* The rail pin is whatever was registered last: ipronet_start() registers
 * cfg->pwr_en_gpio (-1 = none -> IPRONET_ERR_STATE here), the bench may
 * re-register it through ipronet_sdio_power_configure() at any time. */
int ipronet_device_power(int on)
{
    if (!on) {
        ipronet_ctrl_reset_session();   /* a device reboot means a new MAC and a dead session */
    }
    return ipronet_sdio_power(on);
}

/* Everything back to idle, in dependency order: the transport first, so
 * rx_task can no longer dispatch an event (an IP_UPDATE_IND landing after the
 * netif is gone would re-create it through ipronet_netif_set_ip()), then the
 * netif, then the control-plane state. Every step is a no-op when already
 * idle, so this doubles as the "start from a known state" prologue. */
static void rollback(void)
{
    ipronet_sdio_teardown();
    ipronet_netif_down();
    ipronet_ctrl_reset_session();
}

/* Enumerate; while the device does not answer and `window` ticks since t0
 * have not passed, reset the controller to ID mode and try again. */
static int enum_within(TickType_t t0, TickType_t window)
{
    int rc;
    while ((rc = ipronet_sdio_enum()) == IPRONET_ERR_NO_DEVICE &&
           (xTaskGetTickCount() - t0) < window) {
        ipronet_sdio_teardown();
        vTaskDelay(pdMS_TO_TICKS(IPRONET_ENUM_RETRY_MS));
    }
    return rc;
}

/* SDH init through netif up; any failure rolls back to idle. */
static int bring_up(uint32_t bus_hz, TickType_t t_on, TickType_t window,
                    uint32_t *enum_ms)
{
    int rc;

    if ((rc = ipronet_sdio_init()) != IPRONET_OK)             { rollback(); return rc; }
    if ((rc = enum_within(t_on, window)) != IPRONET_OK)       { rollback(); return rc; }
    if (enum_ms != NULL) {
        *enum_ms = (uint32_t)((xTaskGetTickCount() - t_on) * portTICK_PERIOD_MS);
    }
    if ((rc = ipronet_sdio_setup(bus_hz)) != IPRONET_OK)      { rollback(); return rc; }
    /* rx_task MUST be running before HELLO/GET_MAC: the GET_MAC reply is an
     * async CMD upload that only the doorbell rx_task drains. */
    if ((rc = ipronet_sdio_rx_start()) != IPRONET_OK)         { rollback(); return rc; }
    /* HELLO engages the DUT bridge (device sets host_present, unblocks upload)
     * so subsequent WiFi RX frames route through SDIO. */
    if (ipronet_ctrl_send_hello() != IPRONET_OK) {
        printf("ipronet: HELLO failed - device bridge may not engage\r\n");
    }
    /* GET_MAC handshake — learn the device STA MAC (random per boot) into
     * iproeth0. The MAC arrives via the doorbell RX path, not a scratch register
     * (SMID has none — 0x60/0x70 alias the doorbell). */
    if ((rc = ipronet_ctrl_get_mac()) != IPRONET_OK)          { rollback(); return rc; }
    /* Zero-copy opt-in, same place the linux host sends it (probe, zc=1 by
     * default). ZCEN3: the device grants TX+RX stride packing or the copy path
     * stays. */
    (void)ipronet_ctrl_zcen();
    ipronet_netif_up();
    return IPRONET_OK;
}

int ipronet_start(const ipronet_config_t *cfg, const ipronet_callbacks_t *cb)
{
    ipronet_config_t def;

    if (s_busy) return IPRONET_ERR_STATE;
    if (ipronet_is_ready()) return IPRONET_ERR_STATE;
    if (cfg == NULL) { ipronet_config_default(&def); cfg = &def; }
    if (cfg->bus_hz != 25000000U && cfg->bus_hz != 50000000U) return IPRONET_ERR_PARAM;

    rollback();   /* a partial bring-up (step-by-step, or a failed start) may be behind us */
    ipronet_sdio_power_configure(cfg->pwr_en_gpio, cfg->pwr_active_low);
    ipronet_ctrl_set_callbacks(cb);
    ipronet_ctrl_set_auto_reconnect(cfg->auto_reconnect);
    ipronet_sdio_set_pace(cfg->tx_pace_kbps);

    TickType_t boot_window = 0;
    if (cfg->pwr_en_gpio >= 0) {
        /* A host reset leaves the device's SMID session bound to the old host
         * (enumerate answers 207 forever); a power-cycle is the only cure. */
        ipronet_ctrl_reset_session();
        (void)ipronet_sdio_power(0);
        vTaskDelay(pdMS_TO_TICKS(IPRONET_PWR_OFF_MS));
        (void)ipronet_sdio_power(1);
        boot_window = pdMS_TO_TICKS(IPRONET_BOOT_WINDOW_MS);
    }
    return bring_up(cfg->bus_hz, xTaskGetTickCount(), boot_window, NULL);
}

int ipronet_stop(void)
{
    if (s_busy) return IPRONET_ERR_STATE;
    rollback();
    return IPRONET_OK;
}

bool ipronet_host_busy(void)
{
    return s_busy;
}

void ipronet_host_set_busy(bool on)
{
    s_busy = on;
}

void ipronet_host_teardown(void)
{
    rollback();
}

int ipronet_host_restart(uint32_t window_ms, uint32_t *enum_ms)
{
    uint32_t hz = ipronet_sdio_bus_hz();

    rollback();
    return bring_up(hz, xTaskGetTickCount(), pdMS_TO_TICKS(window_ms), enum_ms);
}

/* Device kept its WiFi session (MAC, IP): keep iproeth0 and the
 * control plane, rebuild only the SMID session. The enumerate's
 * CCCR I/O reset ends the device's old session. */
static int reattach(uint32_t window_ms)
{
    uint32_t hz = ipronet_sdio_bus_hz();
    int rc;

    ipronet_sdio_teardown();
    if ((rc = enum_within(xTaskGetTickCount(), pdMS_TO_TICKS(window_ms))) != IPRONET_OK) return rc;
    if ((rc = ipronet_sdio_setup(hz)) != IPRONET_OK)    return rc;
    if ((rc = ipronet_sdio_rx_start()) != IPRONET_OK)   return rc;
    (void)ipronet_ctrl_send_hello();
    if ((rc = ipronet_ctrl_get_mac()) != IPRONET_OK)    return rc;
    (void)ipronet_ctrl_zcen();
    return IPRONET_OK;
}

/* Re-attach on a transport request; idle like ipronet_stop() after
 * the last failed try. Skipped while an OTA owns the link or the
 * host is not started. */
static void recover_task(void *arg)
{
    (void)arg;
    for (;;) {
        (void)ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (s_busy || !ipronet_sdio_is_setup()) {
            continue;
        }
        s_busy = true;
        TickType_t t0 = xTaskGetTickCount();
        int rc = IPRONET_ERR_NO_DEVICE;
        for (uint32_t i = 0; i < IPRONET_RECOVER_TRIES && rc != IPRONET_OK; i++) {
            rc = reattach(IPRONET_RECOVER_WINDOW_MS);
            if (rc != IPRONET_OK) {
                vTaskDelay(pdMS_TO_TICKS(IPRONET_RECOVER_GAP_MS));
            }
        }
        if (rc == IPRONET_OK) {
            s_recovers++;
        } else {
            s_recover_fails++;
            rollback();
        }
        s_busy = false;
        printf("ipronet: re-attach %s (%d) in %lu ms, ok %lu fail %lu\r\n",
               rc == IPRONET_OK ? "done" : "failed", rc,
               (unsigned long)((xTaskGetTickCount() - t0) * portTICK_PERIOD_MS),
               (unsigned long)s_recovers, (unsigned long)s_recover_fails);
        /* requests the old session raised while it went down */
        vTaskDelay(pdMS_TO_TICKS(IPRONET_RECOVER_GAP_MS));
        (void)ulTaskNotifyTake(pdTRUE, 0);
    }
}

void ipronet_host_recover_request(void)
{
    if (s_recover_task == NULL &&
        xTaskCreate(recover_task, "ipronet_rcv", 1024, NULL,
                    IPRONET_RECOVER_PRIO, &s_recover_task) != pdPASS) {
        s_recover_task = NULL;
        return;
    }
    xTaskNotifyGive(s_recover_task);
}
