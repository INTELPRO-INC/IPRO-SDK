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
 * @file network_lte.c
 * @brief LTE PPP network module — network_ops_t implementation
 *
 * Wraps the existing LTE modem + PPP stack into the network_ops_t
 * interface. Extracted from app_network.c.
 */

#include "network_lte.h"

#include <generated/autoconf.h>

#ifdef CONFIG_NETWORK_USE_LTE

#include <stdbool.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

#include <lwip/netif.h>
#include <lwip/tcpip.h>

#include "lte_modem.h"
#include "app_log.h"

#define LOG_TAG "NET_LTE"

#define LTE_PPP_RETRIES          3
#define LTE_PPP_RETRY_MS         3000
#define LTE_TASK_STACK           2048
#define LTE_TASK_PRIO            10

#define PPP_WATCHDOG_INTERVAL_MS 10000  /* check every 10s */
#define PPP_RECONNECT_DELAY_MS   5000

static volatile int g_modem_initialized;
static volatile int g_reconnecting;  /* prevent concurrent reconnect */

/* ------------------------------------------------------------------ */
/* Internal helpers                                                    */
/* ------------------------------------------------------------------ */

static int ppp_do_connect(void)
{
    lte_err_t rc;
    for (int i = 0; i < LTE_PPP_RETRIES; i++) {
        IPRO_LOGI(LOG_TAG, "PPP connect attempt %d/%d...", i + 1, LTE_PPP_RETRIES);
        rc = lte_ppp_connect();
        if (rc == LTE_OK) return 0;
        IPRO_LOGE(LOG_TAG, "PPP connect failed (%d), retry in %ds...",
                  rc, LTE_PPP_RETRY_MS / 1000);
        vTaskDelay(pdMS_TO_TICKS(LTE_PPP_RETRY_MS));
    }
    return -1;
}

static int ppp_is_up(void)
{
    struct netif *nif = lte_ppp_get_netif();
    if (!nif || !netif_is_up(nif)) return 0;
    const ip4_addr_t *ip = netif_ip4_addr(nif);
    return !ip4_addr_isany(ip);
}

/* ------------------------------------------------------------------ */
/* Watchdog task — auto-reconnect on PPP drop                         */
/* ------------------------------------------------------------------ */

static void lte_watchdog_task(void *pvParameters)
{
    (void)pvParameters;

    /* Initial modem init */
    IPRO_LOGI(LOG_TAG, "LTE auto-init...");
    lte_err_t rc = lte_modem_init();
    if (rc != LTE_OK) {
        IPRO_LOGW(LOG_TAG, "LTE auto-init failed (%d) — "
                  "press OPEN then run: lte_init, lte_ppp", rc);
        vTaskDelete(NULL);
        return;
    }
    g_modem_initialized = 1;

    /* Initial PPP connect */
    int init_ok = (ppp_do_connect() == 0);
    int was_up = init_ok ? 1 : 0;

    IPRO_LOGI(LOG_TAG, "PPP watchdog started (initial: %s)",
              was_up ? "UP" : "DOWN");

    /* Watchdog loop: monitor PPP and reconnect on drop */
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(PPP_WATCHDOG_INTERVAL_MS));

        int is_up = ppp_is_up();

        /* Log state transitions */
        if (is_up != was_up) {
            IPRO_LOGI(LOG_TAG, "PPP watchdog: %s -> %s",
                      was_up ? "UP" : "DOWN", is_up ? "UP" : "DOWN");
        }

        if (was_up && !is_up) {
            /* Link dropped — full cleanup + reconnect.
             * Skip if lte_reconnect() is already handling it. */
            if (__atomic_exchange_n(&g_reconnecting, 1, __ATOMIC_SEQ_CST)) {
                IPRO_LOGI(LOG_TAG, "PPP watchdog: reconnect already in progress, skip");
                was_up = 0;
                continue;
            }

            IPRO_LOGW(LOG_TAG, "PPP link dropped! Full modem re-init in 3s...");
            vTaskDelay(pdMS_TO_TICKS(3000));

            /* Full cleanup: PPP disconnect → modem deinit → modem init */
            lte_ppp_disconnect();
            vTaskDelay(pdMS_TO_TICKS(1000));
            lte_modem_deinit();
            vTaskDelay(pdMS_TO_TICKS(1000));

            lte_err_t rc2 = lte_modem_init();
            if (rc2 != LTE_OK) {
                IPRO_LOGE(LOG_TAG, "Modem re-init failed: %d", rc2);
                __atomic_store_n(&g_reconnecting, 0, __ATOMIC_SEQ_CST);
            } else if (ppp_do_connect() == 0) {
                IPRO_LOGI(LOG_TAG, "PPP reconnected successfully");
                __atomic_store_n(&g_reconnecting, 0, __ATOMIC_SEQ_CST);
            } else {
                IPRO_LOGE(LOG_TAG, "PPP reconnect failed");
                __atomic_store_n(&g_reconnecting, 0, __ATOMIC_SEQ_CST);
            }
        } else if (!was_up && !is_up) {
            /* Still down — retry only if not already reconnecting */
            if (!g_reconnecting) {
                IPRO_LOGW(LOG_TAG, "PPP still down, retrying...");
                ppp_do_connect();
            }
        }

        was_up = is_up;
    }
}

/* ------------------------------------------------------------------ */
/* network_ops_t implementation                                       */
/* ------------------------------------------------------------------ */

static app_err_t lte_init(void)
{
    tcpip_init(NULL, NULL);
    return APP_OK;
}

static app_err_t lte_connect(void)
{
    xTaskCreate(lte_watchdog_task, "lte_wd", LTE_TASK_STACK,
                NULL, LTE_TASK_PRIO, NULL);
    return APP_OK;
}

static app_err_t lte_disconnect(void)
{
    if (!g_modem_initialized) return APP_ERR_INIT;
    lte_ppp_disconnect();
    return APP_OK;
}

static int lte_is_connected(void)
{
    return ppp_is_up();
}

static app_err_t lte_wait_ready(int timeout_s)
{
    IPRO_LOGI(LOG_TAG, "Waiting for PPP link (timeout: %ds)...", timeout_s);
    while (timeout_s > 0) {
        struct netif *nif = lte_ppp_get_netif();
        if (nif && netif_is_up(nif)) {
            const ip4_addr_t *ip = netif_ip4_addr(nif);
            if (!ip4_addr_isany(ip)) {
                IPRO_LOGI(LOG_TAG, "IP: %s  GW: %s",
                          ip4addr_ntoa(netif_ip4_addr(nif)),
                          ip4addr_ntoa(netif_ip4_gw(nif)));
                return APP_OK;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
        timeout_s--;
    }
    IPRO_LOGE(LOG_TAG, "PPP timeout!");
    return APP_ERR_TIMEOUT;
}

static app_err_t lte_reconnect(void)
{
    if (!g_modem_initialized) return APP_ERR_INIT;

    /* Prevent concurrent reconnect with watchdog */
    if (__atomic_exchange_n(&g_reconnecting, 1, __ATOMIC_SEQ_CST)) {
        IPRO_LOGW(LOG_TAG, "Reconnect already in progress");
        return APP_ERR_NETWORK;
    }

    IPRO_LOGI(LOG_TAG, "Manual PPP reconnect...");
    lte_ppp_disconnect();
    vTaskDelay(pdMS_TO_TICKS(2000));
    app_err_t ret = (ppp_do_connect() == 0) ? APP_OK : APP_ERR_NETWORK;

    __atomic_store_n(&g_reconnecting, 0, __ATOMIC_SEQ_CST);
    return ret;
}

const network_ops_t network_lte_ops = {
    .name         = "lte",
    .init         = lte_init,
    .connect      = lte_connect,
    .disconnect   = lte_disconnect,
    .is_connected = lte_is_connected,
    .wait_ready   = lte_wait_ready,
    .reconnect    = lte_reconnect,
};

#endif /* CONFIG_NETWORK_USE_LTE */
