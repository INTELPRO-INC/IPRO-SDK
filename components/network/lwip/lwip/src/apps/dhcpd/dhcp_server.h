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

#ifndef DHCPV4_SERVER_H__
#define DHCPV4_SERVER_H__

#include <stdio.h>
#include <lwip/ip.h>

#ifdef __cplusplus
extern "C" {
#endif

/* start, limit is range of dhcpd ip pool, -1 mean use default value */
void dhcpd_start(struct netif *netif, int start, int limit);
void dhcpd_stop(const char *netif_name);
void dhcpd_stop_with_netif(const struct netif *netif);

/* Whether OFFER/ACK replies carry the Router (opt 3) + DNS (opt 6) options
 * pointing at this device (default: 1, classic SoftAP behaviour).
 * Pass 0 for LINK-LOCAL-ONLY mode: clients get an address + mask but keep
 * their existing default route and DNS — required when the device is a
 * sidecar link (e.g. USB CDC-ECM) on a host that also has Wi-Fi/LAN
 * internet, otherwise the host routes everything into us and goes dark. */
void dhcpd_announce_router_dns(int enable);

void dhcpd_clear_dns_server(void* netif);
err_t dhcpd_add_dns_server(void* netif, const ip_addr_t *dnsserver);
err_t dhcpd_remove_dns_server(void* netif, const ip_addr_t *dnsserver);

#ifdef __cplusplus
}
#endif

#endif

