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
 * @file network_emac.c
 * @brief EMAC Ethernet network module — network_ops_t implementation
 *
 * Wraps the existing EMAC + DHCP stack into the network_ops_t
 * interface. Extracted from app_network.c.
 */

#include "network_emac.h"

#include <generated/autoconf.h>

#ifndef CONFIG_NETWORK_USE_LTE  /* EMAC is the fallback when LTE is not selected */

#include <stdbool.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

#include <lwip/netif.h>
#include <lwip/tcpip.h>
#include <lwip/dhcp.h>
#include <hal_ethernetif.h>

#include "app_log.h"

#define LOG_TAG "NET_EMAC"

static struct netif emac_netif;

/* ------------------------------------------------------------------ */
/* Internal helpers                                                    */
/* ------------------------------------------------------------------ */

static void regist_netif(void *arg)
{
    struct netif *netif = (struct netif *)arg;
    ip4_addr_t ipaddr, netmask, gw;

    memset(netif, 0, sizeof(struct netif));
    ip_addr_set_zero(&ipaddr);
    ip_addr_set_zero(&netmask);
    ip_addr_set_zero(&gw);

    netif_add(netif, &ipaddr, &netmask, &gw, NULL, ethernetif_init,
              tcpip_input);
    netif_set_default(netif);
    ethernet_link_status_updated(netif);

#if LWIP_NETIF_LINK_CALLBACK
    netif_set_link_callback(netif, ethernet_link_status_updated);
#endif
}

/* ------------------------------------------------------------------ */
/* network_ops_t implementation                                       */
/* ------------------------------------------------------------------ */

static app_err_t net_emac_init(void)
{
    IPRO_LOGI(LOG_TAG, "Initializing Ethernet and lwIP...");
    tcpip_init(regist_netif, (void *)&emac_netif);
    IPRO_LOGI(LOG_TAG, "Initialization complete");
    return APP_OK;
}

static app_err_t emac_connect(void)
{
    /* DHCP starts in tcpip_init callback — no-op here */
    return APP_OK;
}

static app_err_t emac_disconnect(void)
{
    netif_set_down(&emac_netif);
    return APP_OK;
}

static int emac_is_connected(void)
{
    if (!netif_is_up(&emac_netif)) return 0;
    const ip4_addr_t *ip = netif_ip4_addr(&emac_netif);
    return !ip4_addr_isany(ip);
}

static app_err_t emac_wait_ready(int timeout_s)
{
    IPRO_LOGI(LOG_TAG, "Waiting for DHCP (timeout: %ds)...", timeout_s);

    while (timeout_s > 0) {
        if (netif_is_up(&emac_netif)) {
            const ip4_addr_t *ip = netif_ip4_addr(&emac_netif);
            if (!ip4_addr_isany(ip)) {
                char ip_str[16], gw_str[16];
                snprintf(ip_str, sizeof(ip_str), "%s",
                         ip4addr_ntoa(netif_ip4_addr(&emac_netif)));
                snprintf(gw_str, sizeof(gw_str), "%s",
                         ip4addr_ntoa(netif_ip4_gw(&emac_netif)));
                IPRO_LOGI(LOG_TAG, "IP: %s  GW: %s", ip_str, gw_str);
                return APP_OK;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
        timeout_s--;
    }

    IPRO_LOGE(LOG_TAG, "DHCP timeout!");
    return APP_ERR_TIMEOUT;
}

static app_err_t emac_reconnect(void)
{
    netif_set_down(&emac_netif);
    vTaskDelay(pdMS_TO_TICKS(1000));
    netif_set_up(&emac_netif);
    return APP_OK;
}

const network_ops_t network_emac_ops = {
    .name         = "emac",
    .init         = net_emac_init,
    .connect      = emac_connect,
    .disconnect   = emac_disconnect,
    .is_connected = emac_is_connected,
    .wait_ready   = emac_wait_ready,
    .reconnect    = emac_reconnect,
};

#endif /* !CONFIG_NETWORK_USE_LTE */
