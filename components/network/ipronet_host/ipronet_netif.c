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
 * ipronet_netif.c — lwIP netif "iproeth0" over the ipronet SDIO-WiFi bridge.
 *
 * The SDIO transport (enum/setup/CMD53 r/w + RX worker + ipronet_sdio_send_pdu)
 * lives in ipronet_sdio.c, the control plane (connect / IP events) in
 * ipronet_ctrl.c; this file is the thin lwIP glue:
 *   - TX: netif->linkoutput flattens the pbuf and calls ipronet_sdio_eth_tx()
 *         (DATA/STA_TO_WIFI_TX through the aggregation path).
 *   - RX: ipronet_netif_input() wraps a received eth frame in a pbuf and hands
 *         it to the stack via netif->input (== tcpip_input, NO_SYS=0).
 * The host netif MAC MUST equal the IPRO6S STA MAC or the AP drops
 * src-mismatched frames; ipronet_netif_set_mac() installs the one learned by
 * the GET_MAC handshake.
 */

#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <FreeRTOS.h>
#include <task.h>

#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "lwip/ip4_addr.h"
#include "lwip/ip_addr.h"
#include "netif/etharp.h"

#include "ipronet_priv.h"

/* iproeth0 link-layer address == the IPRO6S STA MAC. The data plane breaks if
 * these differ (the AP drops src-mismatched frames). Default = the historic
 * hardcoded STA MAC; ipronet_netif_set_mac() overwrites it with the real (random
 * per device boot) STA MAC once the GET_MAC handshake learns it. */
static uint8_t s_iproeth_mac[6] = {0x12, 0x23, 0x34, 0x00, 0x45, 0x56};

/* RX→lwIP drop instrumentation (PM13c 2026-05-29 UDP-RX dig): the two silent
 * drop sites in ipronet_netif_input. pool_drop = PBUF_POOL exhausted; mbox_drop =
 * tcpip_input mbox full (tcpip thread couldn't keep up). Surfaced through
 * ipronet_diag_rx_stats to localize UDP-RX loss between rx_task and the lwIP
 * sink. */
static volatile uint32_t s_pool_drop = 0;
static volatile uint32_t s_mbox_drop = 0;

static struct netif s_iproeth;
static bool s_up = false;

/* lwIP TX: flatten the (possibly chained) pbuf and push to the SDIO TX framer. */
static err_t iproeth_linkoutput(struct netif *netif, struct pbuf *p)
{
    static uint8_t txbuf[1600] __attribute__((aligned(4)));   /* 1500 MTU + eth hdr */
    (void)netif;

    if (p->tot_len > sizeof(txbuf)) {
        return ERR_MEM;
    }
    pbuf_copy_partial(p, txbuf, p->tot_len, 0);

    return (ipronet_sdio_eth_tx(txbuf, p->tot_len) == 0) ? ERR_OK : ERR_IF;
}

/* netif_add init callback. */
static err_t iproeth_netif_init(struct netif *netif)
{
    netif->name[0] = 'i';
    netif->name[1] = 'p';
    netif->output = etharp_output;          /* IPv4 -> ARP -> linkoutput */
    netif->linkoutput = iproeth_linkoutput;
    netif->mtu = 1500;
    netif->hwaddr_len = ETHARP_HWADDR_LEN;
    memcpy(netif->hwaddr, s_iproeth_mac, 6);
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;
    return ERR_OK;
}

/* Bring up tcpip + iproeth0 (idempotent). Brings the netif up with a
 * placeholder/static IP; the real IP is set later by ipronet_netif_set_ip()
 * when the device pushes IPRONET_CMD_STA_IP_UPDATE_IND after a connect. */
void ipronet_netif_up(void)
{
    static bool tcpip_started = false;
    ip4_addr_t ip, nm, gw;

    if (s_up) {
        return;
    }
    if (!tcpip_started) {
        tcpip_init(NULL, NULL);
        tcpip_started = true;
    }

    /* Demo default; the device's DHCP result replaces it through
     * ipronet_netif_set_ip() once IP_UPDATE_IND arrives. */
    IP4_ADDR(&ip, 192, 168, 1, 109);
    IP4_ADDR(&nm, 255, 255, 255, 0);
    IP4_ADDR(&gw, 192, 168, 1, 1);

    netif_add(&s_iproeth, &ip, &nm, &gw, NULL, iproeth_netif_init, tcpip_input);
    netif_set_default(&s_iproeth);
    netif_set_up(&s_iproeth);
    s_up = true;
}

/* Take iproeth0 down and remove it (ipronet_stop). tcpip_init has to run once
 * per boot, so tcpip_started stays true and the next ipronet_netif_up() only
 * re-adds the netif. */
void ipronet_netif_down(void)
{
    if (!s_up) return;
    LOCK_TCPIP_CORE();
    netif_set_down(&s_iproeth);
    netif_remove(&s_iproeth);
    UNLOCK_TCPIP_CORE();
    s_up = false;   /* tcpip_started stays true: tcpip_init must run once per boot */
}

bool ipronet_netif_is_up(void)
{
    return s_up;
}

/* Set the iproeth0 IPv4 config dynamically from the device's
 * IPRONET_CMD_STA_IP_UPDATE_IND event (host-driven connect flow). The device
 * runs its own DHCP and pushes ip/mask/gw as octets a.b.c.d. Called from the
 * SDIO RX worker task; guard the lwIP core like the rest of the netif glue. */
void ipronet_netif_set_ip(const uint8_t ip4[4], const uint8_t mask4[4], const uint8_t gw4[4])
{
    ip4_addr_t ip, nm, gw;

    if (!s_up) {
        ipronet_netif_up();              /* ensure netif exists */
    }

    IP4_ADDR(&ip, ip4[0], ip4[1], ip4[2], ip4[3]);
    IP4_ADDR(&nm, mask4[0], mask4[1], mask4[2], mask4[3]);
    IP4_ADDR(&gw, gw4[0], gw4[1], gw4[2], gw4[3]);

    LOCK_TCPIP_CORE();
    netif_set_addr(&s_iproeth, &ip, &nm, &gw);
    netif_set_up(&s_iproeth);
    UNLOCK_TCPIP_CORE();
}

/* Set iproeth0's MAC to the STA MAC learned from the device GET_MAC handshake.
 * Stores into the module MAC (used by iproeth_netif_init); if the netif is
 * already up, updates netif->hwaddr live under the tcpip core lock. Called from
 * the SDIO RX worker task (ipronet_ctrl_handle GET_MAC case). */
void ipronet_netif_set_mac(const uint8_t mac[6])
{
    memcpy(s_iproeth_mac, mac, 6);
    if (s_up) {
        LOCK_TCPIP_CORE();
        memcpy(s_iproeth.hwaddr, mac, 6);
        UNLOCK_TCPIP_CORE();
    }
}

/* RX: a STA_FROM_WIFI_RX eth frame just came off the SDIO upload path. Runs in
 * the RX worker task (not ISR), so pbuf_alloc + netif->input are safe here. */
void ipronet_netif_input(const uint8_t *eth, uint16_t len)
{
    struct pbuf *p;

    if (!s_up || len == 0) {
        return;
    }
    p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
    if (p == NULL) {
        s_pool_drop++;
        return;                              /* drop on pool exhaustion */
    }
    if (pbuf_take(p, eth, len) != ERR_OK) {
        pbuf_free(p);
        return;
    }
    if (s_iproeth.input(p, &s_iproeth) != ERR_OK) {
        s_mbox_drop++;
        pbuf_free(p);
    }
}

uint32_t ipronet_netif_pool_drops(void) { return s_pool_drop; }
uint32_t ipronet_netif_mbox_drops(void) { return s_mbox_drop; }
int ipronet_diag_netif_up(void) { ipronet_netif_up(); return IPRONET_OK; }

/* Dotted-quad of the address iproeth0 currently holds (the demo default until
 * the device's DHCP result arrives). Returns a pointer to a static buffer. */
const char *ipronet_diag_netif_ip_str(void)
{
    static char buf[16];

    ip4addr_ntoa_r(netif_ip4_addr(&s_iproeth), buf, sizeof(buf));
    return buf;
}
