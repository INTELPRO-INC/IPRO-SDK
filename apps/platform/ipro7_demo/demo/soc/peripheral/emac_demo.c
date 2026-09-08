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

#include <generated/autoconf.h>

#ifdef CONFIG_COMPONENTS_LWIP_ENABLE
#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "shell.h"
#include <hal_emac.h>
#include <hal_ethernetif.h>
#include <lwip/tcpip.h>

static struct netif emac_netif;

static void regist_netif(void *arg) {
    memset(arg, 0, sizeof(struct netif));

    struct netif *netif = (struct netif *)arg;
    ip4_addr_t ipaddr;
    ip4_addr_t netmask;
    ip4_addr_t gw;

#if 0
    IP4_ADDR(&ipaddr, 192, 168, 10, 123);
    IP4_ADDR(&netmask, 255, 255, 255, 0);
    IP4_ADDR(&gw, 192, 168, 10, 1);
#else
    ip_addr_set_zero(&ipaddr);
    ip_addr_set_zero(&netmask);
    ip_addr_set_zero(&gw);
#endif
    netif_add(netif, &ipaddr, &netmask, &gw, NULL, ethernetif_init, tcpip_input);

    /*  Registers the default network interface */
    netif_set_default(netif);
    ethernet_link_status_updated(netif);
    #if LWIP_NETIF_LINK_CALLBACK
    netif_set_link_callback(netif, ethernet_link_status_updated);
    #endif

    //netifapi_dhcp_start(netif);
}

int cmd_emac_lwip(int argc, char **argv)
{
    tcpip_init(regist_netif, (void *)&emac_netif);
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_emac_lwip, emac_lwip, Emac lwip);

#endif
