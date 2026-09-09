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

#include <assert.h>
#include "mac_types.h"
#include "fhost.h"
#include "fhost_api.h"

#include "net_al.h"
#include "net_al_ext.h"
#include "platform_al.h"
#include "wifi_mgmr_ext.h"
#include "wifi_mgmr.h"

#include "lwip/netifapi.h"
#include "lwip/api.h"
#include "lwip/dns.h"
#include "lwip/prot/dhcp.h"
#include "dhcp_server.h"

#ifdef CONFIG_WIFI_GATEWAY
#include <lwip/lwip_napt.h>
#endif

#ifdef CFG_IPV6
#include <lwip/ethip6.h>
#endif

#include "lwip/tcpip.h"
#include "lwip/etharp.h"
#include "lwip/priv/tcp_priv.h"

#ifdef NET_AL_NO_IP
int net_al_ext_set_vif_ip(int fvif_idx, struct net_al_ext_ip_addr_cfg *cfg)
{
    return -1;
}

int net_al_ext_get_vif_ip(int fvif_idx, struct net_al_ext_ip_addr_cfg *cfg)
{
    return -1;
}
void net_al_ext_dhcp_connect(void)
{
}
void net_al_ext_dhcp_disconnect(void)
{
}
void net_al_ext_sta_reconnect_arm(void)
{
}
void net_al_ext_sta_reconnect_reset(void)
{
}
void net_al_ext_sta_reconnect_suppress(void)
{
}
int net_dhcpd_start(net_al_if_t net_if, int start, int limit)
{
    return -1
}
#else

static void net_if_disable_arp_for_us(net_al_if_t net_if)
{
    struct addr_ext ext;
    inet_if_t *nif = (inet_if_t *)net_if;

    netif_get_addr_ext(nif, &ext);
    ext.arp_for_us_disable = 1;
    netif_set_addr_ext(nif, &ext);
}

static void net_if_enable_arp_for_us(net_al_if_t net_if)
{
    struct addr_ext ext;
    inet_if_t *nif = (inet_if_t *)net_if;

    netif_get_addr_ext(nif, &ext);
    ext.arp_for_us_disable = 0;
    netif_set_addr_ext(nif, &ext);
}

static void net_if_set_default(net_al_if_t net_if)
{
    inet_if_t *nif = (inet_if_t *)net_if;

    netifapi_netif_set_default(nif);
}

static void net_if_set_ip(net_al_if_t net_if, uint32_t ip, uint32_t mask, uint32_t gw)
{
    inet_if_t *nif = (inet_if_t *)net_if;

    if (!net_if) {
        return;
    }

    netifapi_netif_set_addr(nif, (const ip4_addr_t *)&ip, (const ip4_addr_t *)&mask,
                   (const ip4_addr_t *)&gw);
}

int net_if_get_ip(net_al_if_t net_if, uint32_t *ip, uint32_t *mask, uint32_t *gw)
{
    inet_if_t *nif = (inet_if_t *)net_if;

    if (!nif) {
        return -1;
    }

    if (ip)
        *ip = netif_ip4_addr(nif)->addr;
    if (mask)
        *mask = netif_ip4_netmask(nif)->addr;
    if (gw)
        *gw = netif_ip4_gw(nif)->addr;

    return 0;
}

static uint32_t stop_dhcpc;
static int net_dhcp_start(net_al_if_t net_if)
{
    #if LWIP_IPV4 && LWIP_DHCP
    if (netifapi_dhcp_start(net_if) ==  ERR_OK)
        return 0;
    #endif //LWIP_IPV4 && LWIP_DHCP
    return -1;
}

static void net_dhcp_stop(net_al_if_t net_if)
{
    #if LWIP_IPV4 && LWIP_DHCP
    netifapi_dhcp_stop(net_if);
    #endif //LWIP_IPV4 && LWIP_DHCP
}

static int net_dhcp_release(net_al_if_t net_if)
{
    #if LWIP_IPV4 && LWIP_DHCP
    if (netifapi_dhcp_release(net_if) ==  ERR_OK)
        return 0;
    #endif //LWIP_IPV4 && LWIP_DHCP
    return -1;
}

static int net_dhcp_address_obtained(net_al_if_t net_if)
{
    #if LWIP_IPV4 && LWIP_DHCP
    if (dhcp_supplied_address(net_if))
        return 0;
    #endif //LWIP_IPV4 && LWIP_DHCP
    return -1;
}

static int net_set_dns(uint32_t dns_server)
{
    #if LWIP_DNS
    ip_addr_t ip;
    ip_addr_set_ip4_u32_val(ip, dns_server);
    dns_setserver(0, &ip);
    return 0;
    #else
    return -1;
    #endif
}

static int net_get_dns(uint32_t *dns_server)
{
    #if LWIP_DNS
    const ip_addr_t *ip;

    if (dns_server == NULL)
        return -1;

    ip = dns_getserver(0);
    *dns_server = ip_addr_get_ip4_u32(ip);
    return 0;
    #else
    return -1;
    #endif
}

static int fhost_dhcp_stop(net_al_if_t net_if)
{
    // Release DHCP lease
    if (!net_dhcp_address_obtained(net_if))
    {
        if (net_dhcp_release(net_if))
        {
            printf("Failed to release DHCP");
            return -1;
        }

        printf("IP released");
    }

    // Stop DHCP
    net_dhcp_stop(net_if);

    return 0;
}

static int fhost_dhcp_start(net_al_if_t net_if, uint32_t to_ms)
{
    uint32_t start_ms;

    // Run DHCP client
    if (net_dhcp_start(net_if))
    {
        printf("Failed to start DHCP");
        return DHCPC_START_FAILED;
    }

    start_ms = rtos_now(false);
    stop_dhcpc = 0;
    while ((net_dhcp_address_obtained(net_if)) &&
          (rtos_now(false) - start_ms < to_ms) &&
          (!stop_dhcpc))
    {
        rtos_task_suspend(100);
    }

    if (stop_dhcpc) {
        fhost_dhcp_stop(net_if);
        return DHCPC_START_ABORT;
    }

    if (net_dhcp_address_obtained(net_if))
    {
        printf("DHCP start timeout");
        fhost_dhcp_stop(net_if);
        return DHCPC_START_TIMEOUT;
    }

    return 0;
}

int net_al_ext_set_vif_ip(int fvif_idx, struct net_al_ext_ip_addr_cfg *cfg)
{
    int ret = 0;
    net_al_if_t net_if;

    net_if = fhost_to_net_if(fvif_idx);
    assert(net_if);

    if (MGMR_VIF_AP == fvif_idx)
    {
        switch (cfg->mode)
        {
        case IP_ADDR_NONE:
            // clear current IP address
            net_if_set_ip(net_if, 0, 0, 0);
            return 0;
        case IP_ADDR_STATIC_IPV4:
            // To be safe
            net_if_set_ip(net_if, cfg->ipv4.addr, cfg->ipv4.mask, cfg->ipv4.gw);
            if (cfg->ipv4.dns)
                net_set_dns(cfg->ipv4.dns);
            else
                net_get_dns(&cfg->ipv4.dns);
            break;
        default:
            return VIF_MODE_ERR;
        }
    }
    else if (MGMR_VIF_STA == fvif_idx)
    {
        switch (cfg->mode)
        {
        case IP_ADDR_NONE:
            // clear current IP address
            fhost_dhcp_stop(net_if);
            wifiMgmr.wlan_sta.dhcp_started = 0;
            net_if_set_ip(net_if, 0, 0, 0);
            stop_dhcpc = 1;
            return 0;
        case IP_ADDR_STATIC_IPV4:
            // To be safe
            net_if_enable_arp_for_us(net_if);
            fhost_dhcp_stop(net_if);
            wifiMgmr.wlan_sta.dhcp_started = 0;
            net_if_set_ip(net_if, cfg->ipv4.addr, cfg->ipv4.mask, cfg->ipv4.gw);
            if (cfg->ipv4.dns)
                net_set_dns(cfg->ipv4.dns);
            else
                net_get_dns(&cfg->ipv4.dns);
            break;
        case IP_ADDR_DHCP_CLIENT:
            if (0 == wifiMgmr.wlan_sta.dhcp_started)
            {
                ret = fhost_dhcp_start(net_if, cfg->dhcp.to_ms);
                if (ret)
                {
                    return ret;
                }
                else
                {
                    wifiMgmr.wlan_sta.dhcp_started = 1;
                }
            }
            net_if_get_ip(net_if, &(cfg->ipv4.addr), &(cfg->ipv4.mask), &(cfg->ipv4.gw));
            net_get_dns(&cfg->ipv4.dns);
            printf("{FVIF-%d} ip=I4 gw=I4", fvif_idx);
            break;
        default:
            return VIF_MODE_ERR;
        }
    }
    else
    {
        printf("wrong fvif_idx: %d", fvif_idx);
        assert(0);
    }

    if (cfg->default_output)
        net_if_set_default(net_if);

    return VIF_SET_SUCCESS;
}

int net_al_ext_get_vif_ip(int fvif_idx, struct net_al_ext_ip_addr_cfg *cfg)
{
    net_al_if_t net_if;

    if (fvif_idx >= CFG_VIF_MAX)
        return -1;

    net_if = fhost_to_net_if(fvif_idx);
    if (!net_if)
        return -1;

    if (!net_dhcp_address_obtained(net_if))
        cfg->mode = IP_ADDR_DHCP_CLIENT;
    else
        cfg->mode = IP_ADDR_STATIC_IPV4;

    cfg->default_output = false;

    net_if_get_ip(net_if, &(cfg->ipv4.addr), &(cfg->ipv4.mask), &(cfg->ipv4.gw));
    net_get_dns(&(cfg->ipv4.dns));

    return 0;
}

static int show_ip(uint8_t fhost_vif_idx)
{
    char fmt[] = "[%d] %s: MAC=%02x:%02x:%02x:%02x:%02x:%02x ip=%d.%d.%d.%d/%d %s%s\n";
    char ifname[NET_AL_MAX_IFNAME];
    char *state, *connected;
    struct net_al_ext_ip_addr_cfg ip_cfg;
    const uint8_t *mac_addr;
    struct fhost_vif_status vif_status;

    fhost_get_vif_status(fhost_vif_idx, &vif_status);
    if (fhost_vif_is_up(fhost_vif_idx))
    {
        state = "UP";
        if (vif_status.chan.prim20_freq != 0)
            connected = ",CONNECTED";
        else
            connected = "";
    }
    else
    {
        state = "DOWN";
        connected = "";
    }
    mac_addr = net_if_get_mac_addr(fhost_to_net_if(fhost_vif_idx));

    if (fhost_vif_name(fhost_vif_idx, ifname, NET_AL_MAX_IFNAME) < 0)
        return -1;


    if (net_al_ext_get_vif_ip(fhost_vif_idx, &ip_cfg))
        return -1;

    printf(fmt, fhost_vif_idx, ifname,
                mac_addr[0], mac_addr[1], mac_addr[2],
                mac_addr[3], mac_addr[4], mac_addr[5],
                ip_cfg.ipv4.addr & 0xff, (ip_cfg.ipv4.addr >> 8) & 0xff,
                (ip_cfg.ipv4.addr >> 16) & 0xff, (ip_cfg.ipv4.addr >> 24) & 0xff,
                32 - __builtin_clz(ip_cfg.ipv4.mask), state, connected);
    return 0;
}

/* STA auto-reconnect (bench 2026-07-25).
 *
 * There was no reconnect policy anywhere: the app's CODE_WIFI_ON_DISCONNECT
 * handler is a printf, and nothing else re-armed the link. For the SDIO/USB
 * bridge that made a single unsolicited deauth terminal - LAB02 answers a 45M
 * over-offer with reason_code 1, and after it iproeth0 stayed carrier-down
 * until someone typed wifi_sta_connect by hand. A device that presents itself
 * to a host as a NIC has to re-associate on its own.
 *
 * This is the BACKSTOP, not the primary path. The primary is the supplicant's
 * own STA_AUTOCONNECT, which wifi_mgmr_sta_autoconnect_enable() turns on (the
 * bridge app does that at MGMR_DONE; wifi_mgmr_init defaults it OFF). Two
 * drivers racing into the same connect machinery is how the known supplicant
 * connect limbo gets hit, so the first attempt here waits well past the
 * supplicant's own retry and only covers the case where that retry never
 * happens or never completes.
 *
 * Shape: first retry after RECONNECT_DELAY_MIN_MS, then double up to
 * RECONNECT_DELAY_MAX_MS, forever, and reset the backoff once an IP is up.
 * Unbounded on purpose - the link coming back is never "no longer wanted", and
 * the cap keeps a wrong-password loop at one attempt per 30 s.
 *
 * The attempt runs in a transient task, never in the caller's context:
 * wifi_mgmr_sta_connect() blocks on a synchronous CFM, and issuing it from the
 * fhost message handler is the known way into the supplicant connect limbo.
 * Same idiom (and same stored credentials + disable_autoreconnect gate) as the
 * DHCP-failure retry in wifi_sta_dhcpc_start below. */
#define STA_RECONNECT_DELAY_MIN_MS 10000
#define STA_RECONNECT_DELAY_MAX_MS 30000
static uint32_t sta_reconnect_delay_ms = STA_RECONNECT_DELAY_MIN_MS;
static bool sta_reconnect_pending;
/* Set by wifi_sta_disconnect(): the next disconnect indication is one we asked
 * for, so do not fight the caller by re-associating. Cleared when someone asks
 * for a connection again. */
static bool sta_reconnect_suppressed;
/* Bumped by anything that invalidates a sleeping attempt: a link that came up
 * (by any means, including the supplicant's own retry) and an explicit
 * disconnect. A task whose generation is stale must not connect - bench
 * 2026-07-25 caught exactly that, a leftover attempt undoing an operator's
 * wifi_sta_disconnect 8 s later. */
static uint32_t sta_reconnect_gen;

static RTOS_TASK_FCT(sta_reconnect_task)
{
    uint32_t delay = sta_reconnect_delay_ms;
    uint32_t gen = sta_reconnect_gen;

    rtos_task_suspend((int)delay);

    if (gen != sta_reconnect_gen)
    {
        printf("[STA] auto-reconnect stale (gen %lu != %lu), dropping\r\n",
               (unsigned long)gen, (unsigned long)sta_reconnect_gen);
        sta_reconnect_pending = false;
        rtos_task_delete(NULL);
        return;
    }

    /* Re-check the gate: the user may have disabled auto-reconnect while we
     * were sleeping. */
    if (!wifiMgmr.disable_autoreconnect &&
        (wifiMgmr.sta_connect_param.ssid_len != 0))
    {
        printf("[STA] auto-reconnect after %lums\r\n", (unsigned long)delay);
        delay <<= 1;
        sta_reconnect_delay_ms = (delay > STA_RECONNECT_DELAY_MAX_MS) ?
                                 STA_RECONNECT_DELAY_MAX_MS : delay;
        sta_reconnect_pending = false;
        wifi_mgmr_sta_connect(&wifiMgmr.sta_connect_param);
    }
    else
    {
        sta_reconnect_pending = false;
    }

    rtos_task_delete(NULL);
}

void net_al_ext_sta_reconnect_suppress(void)
{
    sta_reconnect_suppressed = true;
    sta_reconnect_gen++;   /* invalidate any attempt already sleeping */
}

void net_al_ext_sta_reconnect_arm(void)
{
    /* One line per disconnect: an arm that silently declines is impossible to
     * tell from a reconnect that failed, and on the bench it cost a whole
     * repro cycle to find out which. */
    printf("[STA] reconnect arm: sup=%d dis=%u pend=%d ssid_len=%u\r\n",
           (int)sta_reconnect_suppressed, wifiMgmr.disable_autoreconnect,
           (int)sta_reconnect_pending, wifiMgmr.sta_connect_param.ssid_len);

    if (sta_reconnect_suppressed)
    {
        /* One-shot: consume it, so a later unsolicited deauth still recovers. */
        sta_reconnect_suppressed = false;
        return;
    }
    if (wifiMgmr.disable_autoreconnect || sta_reconnect_pending)
        return;
    /* Nothing to reconnect to until the STA has been told an SSID once. */
    if (wifiMgmr.sta_connect_param.ssid_len == 0)
        return;

    sta_reconnect_pending = true;
    if (rtos_task_create(sta_reconnect_task, "sta_reconnect", WPA_CONNECTED_TASK,
                         512, NULL, fhost_wpa_priority, NULL))
    {
        /* Out of heap for the task - drop the attempt rather than leaving the
         * gate latched, so the next disconnect event can try again. */
        sta_reconnect_pending = false;
        printf("[STA] auto-reconnect task create failed\r\n");
    }
}

void net_al_ext_sta_reconnect_reset(void)
{
    sta_reconnect_delay_ms = STA_RECONNECT_DELAY_MIN_MS;
    /* A link that came up cancels any stale suppression, so the first deauth
     * after it is treated as unsolicited even if the previous disconnect never
     * produced an indication. It also invalidates any attempt still sleeping -
     * whoever brought the link up (us, or the supplicant's own retry) won. */
    sta_reconnect_suppressed = false;
    sta_reconnect_gen++;
}

static int wifi_sta_dhcpc_start(uint8_t fhost_vif_idx)
{
    struct net_al_ext_ip_addr_cfg ip_cfg;
    ip_cfg.mode = IP_ADDR_DHCP_CLIENT;
    ip_cfg.default_output = true;
    ip_cfg.dhcp.to_ms = 15000;
    int ret = 0;

    ret = net_al_ext_set_vif_ip(fhost_vif_idx, &ip_cfg);
    if (ret)
    {
        if (ret != DHCPC_START_ABORT) {
            if (ret == DHCPC_START_TIMEOUT) {
                printf("dhcpc obtain ip failed\n");
            } else {
                printf("dhcpc start error, ret is %d\n", ret);
            }
            if (!wifiMgmr.disable_autoreconnect) {
                wifi_mgmr_sta_connect(&wifiMgmr.sta_connect_param);
            } else {
                wifi_sta_disconnect();
            }
        } else {
            printf("dhcpc start abort\n");
        }
        PLATFORM_HOOK(prevent_sleep, PSM_EVENT_CONNECT, 0);
        return -1;
    } else {
        /* Link is fully up again - forget the backoff so the next outage gets
         * a fast first retry instead of inheriting a 30 s delay. */
        net_al_ext_sta_reconnect_reset();
        platform_post_event(EV_WIFI, CODE_WIFI_ON_GOT_IP, 0);
        show_ip(fhost_vif_idx);
        PLATFORM_HOOK(prevent_sleep, PSM_EVENT_CONNECT, 0);
        if(wifi_mgmr_sta_connect_params_get() & LOW_RATE_CONNECT) {
            wifi_mgmr_rate_config(0xFFFF);
        }
    }
    return 0;
}

static RTOS_TASK_FCT(fhost_wpa_connected_task) {
    wifi_sta_dhcpc_start(MGMR_VIF_STA);

    rtos_task_delete(NULL);
}

static int wifi_sta_dhcpc_stop(uint8_t fhost_vif_idx)
{
    struct net_al_ext_ip_addr_cfg ip_cfg;
    ip_cfg.mode = IP_ADDR_NONE;
    if (net_al_ext_set_vif_ip(fhost_vif_idx, &ip_cfg)) {
        printf("dhcpc stop error\n");
        return -1;
    }
    return 0;
}

int net_al_dhcpd_start(net_al_if_t net_if, int start, int limit)
{
    inet_if_t *nif = (inet_if_t *)net_if;

    netifapi_netif_set_link_up(nif);
    dhcpd_start(net_if, start, limit);

    return 0;
}

int net_al_link_set(net_al_if_t net_if)
{
    inet_if_t *nif = (inet_if_t *)net_if;

    netifapi_netif_set_link_up(nif);

    return 0;
}

int net_al_gw_service_enable(net_al_if_t net_if)
{
#ifdef CONFIG_WIFI_GATEWAY
    struct net_al_ext_ip_addr_cfg ip_cfg;

#ifndef WIFI_GATEWAY_DNS_SERVER
    ip_addr_t back_dns_server;

    net_al_ext_get_vif_ip(MGMR_VIF_STA, &ip_cfg);
    fhost_printf("Reuse STA DNS sever: %08x\n\r", ip_cfg.ipv4.dns);
    dhcpd_add_dns_server(net_if, (const ip_addr_t *)&ip_cfg.ipv4.dns);

    IP_ADDR4(&back_dns_server, 223, 5, 5, 5);
    fhost_printf("Add backup DNS sever: %08x\n\r", ip_addr_get_ip4_u32(&back_dns_server));
    dhcpd_add_dns_server(net_if, (const ip_addr_t *)&back_dns_server);
#endif

    net_al_ext_get_vif_ip(MGMR_VIF_AP, &ip_cfg);
    LOCK_TCPIP_CORE();
    ip_napt_enable(ip_cfg.ipv4.addr, 1);
    UNLOCK_TCPIP_CORE();
#endif

    return 0;
}

int net_al_gw_service_disable(net_al_if_t net_if)
{
#ifdef CONFIG_WIFI_GATEWAY
    struct net_al_ext_ip_addr_cfg ip_cfg;

#ifndef WIFI_GATEWAY_DNS_SERVER
    dhcpd_clear_dns_server(net_if);
#endif
    net_al_ext_get_vif_ip(MGMR_VIF_AP, &ip_cfg);
    LOCK_TCPIP_CORE();
    ip_napt_enable(ip_cfg.ipv4.addr, 0);
    UNLOCK_TCPIP_CORE();
#endif

    return 0;
}

int net_al_dhcpd_stop(net_al_if_t net_if)
{
    if (!net_if) {
        return -1;
    }

    dhcpd_stop_with_netif(net_if);
    return 0;
}

static ip4_addr_t saved_ip_addr = {IPADDR_ANY};
static ip4_addr_t saved_ip_mask = {IPADDR_ANY};
static ip4_addr_t saved_ip_gw = {IPADDR_ANY};

static void qc_callback(struct netif *net_if)
{
    inet_if_t *nif = (inet_if_t *)net_if;

    printf("[quick_connect] dhcp bind new ip callback\r\n"
            "  IP: %s\r\n", ip4addr_ntoa(netif_ip4_addr(nif)));
    printf("  MK: %s\r\n", ip4addr_ntoa(netif_ip4_netmask(nif)));
    printf("  GW: %s\r\n", ip4addr_ntoa(netif_ip4_gw(nif)));

    ip4_addr_copy(saved_ip_addr, *netif_ip4_addr(nif));
    ip4_addr_copy(saved_ip_mask, *netif_ip4_netmask(nif));
    ip4_addr_copy(saved_ip_gw, *netif_ip4_gw(nif));
#define  EV_WIFI                  0x0002
#define  CODE_WIFI_ON_GOT_IP      7
    platform_post_event(EV_WIFI, CODE_WIFI_ON_GOT_IP, 0);
}

static void net_quick_dhcp_restore(net_al_if_t net_if)
{
    inet_if_t *nif = (inet_if_t *)net_if;
    struct addr_ext ext;

    netifapi_netif_set_default(nif);

    netif_get_addr_ext(nif, &ext);
    ext.dhcp_qc_callback = qc_callback;
    netif_set_addr_ext(nif, &ext);

    if (!ip4_addr_isany_val(saved_ip_addr)) {
        netifapi_netif_set_addr(nif, &saved_ip_addr, &saved_ip_mask, &saved_ip_gw);
        platform_post_event(EV_WIFI, CODE_WIFI_ON_GOT_IP, 0);
        /* already get ip */
        netifapi_dhcp_renew(nif);
    } else {
        /* start dhcp */
        netifapi_dhcp_start(nif);
    }
}

static void net_quick_dhcp_stop(net_al_if_t net_if)
{
    struct addr_ext ext;
    inet_if_t *nif = (inet_if_t *)net_if;
    struct dhcp *dhcp;

    dhcp = netif_dhcp_data(nif);

    netif_get_addr_ext(nif, &ext);
    ext.dhcp_qc_callback = NULL;
    netif_set_addr_ext(nif, &ext);

    if(dhcp) {
        // stop dhcp timer
        dhcp->state = DHCP_STATE_OFF;
        dhcp->request_timeout = 0;
    }
}

#ifdef CFG_IPV6
static void net_al_create_ip6_linklocal_address(void)
{
    LOCK_TCPIP_CORE();
    netif_create_ip6_linklocal_address(fhost_to_net_if(MGMR_VIF_STA), 1);
    netif_set_ip6_autoconfig_enabled(((struct netif *)fhost_env.vif[MGMR_VIF_STA].net_if), true);
    UNLOCK_TCPIP_CORE();
}
#endif

void net_al_ext_dhcp_connect(void)
{
    net_al_if_t net_if = fhost_to_net_if(MGMR_VIF_STA);

    printf("DHCP_CONN: use_dhcp=%d qc=%d net_if=%p\r\n",
           wifiMgmr.sta_connect_param.use_dhcp,
           wifiMgmr.sta_connect_param.quick_connect,
           net_if);

#if !LWIP_TCPIP_CORE_LOCKING
    #error To do add netif msg call
#endif
    if (wifiMgmr.sta_connect_param.use_dhcp) {
        net_if_disable_arp_for_us(net_if);
        if (wifiMgmr.sta_connect_param.quick_connect) {
            net_quick_dhcp_restore(net_if);
        } else {
            printf("start dhcping ... \r\n");
            rtos_task_create(fhost_wpa_connected_task, "fhost_wpa_connected_task",
                             WPA_CONNECTED_TASK, 512, NULL, fhost_wpa_priority, NULL);
        }
    }

#ifdef CFG_IPV6
    net_al_create_ip6_linklocal_address();
#endif
}

void net_al_ext_dhcp_disconnect(void)
{
    if (wifiMgmr.sta_connect_param.use_dhcp) {
        if (wifiMgmr.sta_connect_param.quick_connect) {
            net_al_if_t netif = fhost_to_net_if(MGMR_VIF_STA);
            net_quick_dhcp_stop(netif);
        } else {
            wifi_sta_dhcpc_stop(MGMR_VIF_STA);
        }
    }
}

/* Emit a gratuitous ARP on the STA interface so peers refresh their ARP cache
 * entry for us. In DTIM power-save the lp_fw blob wakes the host only for our
 * unicast TIM bit and sleeps through DTIM group delivery, so a peer whose ARP
 * entry has aged out cannot re-resolve us via broadcast ARP (~42% loss,
 * HW-measured 2026-07-23) — it then declares us unreachable and tears the flow
 * down. Announcing ourselves keeps every peer's entry warm so they never need
 * to broadcast-ARP us. The tickless keepalive drives this, so it adds no extra
 * wake. Post-DHCP arp_for_us_disable is 0 (dhcp.c clears it on bind), so the
 * send is not the inbound path gated by that flag. Returns 0 on success, -1 if
 * the STA netif is not up with a configured address. */
/* Does the STA have a usable IPv4 route right now?
 *
 * Split out of net_al_sta_arp_announce's own precondition so callers can ask
 * BEFORE spending anything. A connect() attempted without a route fails with
 * ENETUNREACH, but not for free: lwIP still builds a SYN (tcp_enqueue_flags) and
 * an ARP request for the gateway (etharp_request), and on this platform both
 * strand in the TX path and pin their heap blocks permanently. Measured
 * 2026-07-30: those two are 18-58% and 7-21% of the heap at the point the DUT
 * dies, accumulated almost entirely inside reconnect episodes where every attempt
 * is already doomed. So the caller that is about to retry a dead link should ask
 * first and skip the attempt rather than pay for a guaranteed failure.
 *
 * @return 1 when the STA netif is up with a configured address, 0 otherwise.
 */
/*
 * Dump every TCP pcb lwIP is holding.
 *
 * Two questions at once. A keepalive that send()s fine yet never reaches the
 * peer -- no FIN, no RST, four attempts, and the queued pbuf still at ref 1, so
 * it was never even handed to net_if_output -- means tcp_output() itself
 * declined to send. snd_wnd/cwnd/unacked/unsent/nrtx say which reason it was.
 * And counting the pcbs says whether the sockets we close() are ever reaped:
 * their queued data is the per-cycle heap leak.
 */
#define TCP_DUMP_MAX 8

struct tcp_dump_row {
    u8_t  state;
    u8_t  flags;
    u8_t  nrtx;
    u8_t  persist;
    u16_t snd_wnd;
    u16_t cwnd;
    u16_t snd_buf;
    u16_t qlen;
    s16_t rtime;
    s16_t rto;
    u8_t  unacked;
    u8_t  unsent;
    u8_t  uref;         /* ref of the unacked head pbuf                     */
    u8_t  ubusy;        /* == tcp_output_segment_busy(): blocks retransmit  */
};

void net_al_tcp_dump(const char *tag)
{
    struct tcp_dump_row row[TCP_DUMP_MAX];
    struct tcp_pcb *pcb;
    int n = 0, tw = 0, i;

    /* Snapshot under the core lock, print outside it. The list mutates from the
     * tcpip thread, so an unlocked walk can follow a freed next pointer -- but
     * printing 8 rows over a 115200 UART inside the lock would stall the whole
     * stack for ~100 ms and perturb the very stall being measured. */
    LOCK_TCPIP_CORE();
    for (pcb = tcp_active_pcbs; pcb != NULL; pcb = pcb->next) {
        if (n < TCP_DUMP_MAX) {
            row[n].state   = (u8_t)pcb->state;
            row[n].flags   = (u8_t)pcb->flags;
            row[n].nrtx    = pcb->nrtx;
            row[n].persist = pcb->persist_backoff;
            row[n].snd_wnd = (u16_t)pcb->snd_wnd;
            row[n].cwnd    = (u16_t)pcb->cwnd;
            row[n].snd_buf = pcb->snd_buf;
            row[n].qlen    = pcb->snd_queuelen;
            row[n].rtime   = pcb->rtime;
            row[n].rto     = pcb->rto;
            row[n].unacked = (pcb->unacked != NULL);
            row[n].unsent  = (pcb->unsent != NULL);
            /* tcp_rexmit_rto_prepare() gives up on the WHOLE retransmit when
             * any unacked segment still carries a driver reference, and this
             * driver takes that reference in net_if_output() and drops it only
             * on TX confirm. A frame destroyed at PDS power-down never gets its
             * confirm, so the ref sticks and retransmission stops forever. */
            row[n].uref  = pcb->unacked ? pcb->unacked->p->ref : 0;
            row[n].ubusy = pcb->unacked ? (pcb->unacked->p->ref > 1) : 0;
        }
        n++;
    }
    for (pcb = tcp_tw_pcbs; pcb != NULL; pcb = pcb->next) {
        tw++;
    }
    UNLOCK_TCPIP_CORE();

    for (i = 0; i < n && i < TCP_DUMP_MAX; i++) {
        printf("[TCPDBG] %s pcb%d st=%u flags=0x%x snd_wnd=%u cwnd=%u "
               "snd_buf=%u qlen=%u unacked=%u unsent=%u nrtx=%u rtime=%d "
               "rto=%d persist=%u uref=%u ubusy=%u\r\n",
               tag, i, row[i].state, row[i].flags, row[i].snd_wnd,
               row[i].cwnd, row[i].snd_buf, row[i].qlen, row[i].unacked,
               row[i].unsent, row[i].nrtx, row[i].rtime, row[i].rto,
               row[i].persist, row[i].uref, row[i].ubusy);
    }
    printf("[TCPDBG] %s active=%d time_wait=%d\r\n", tag, n, tw);
}

int net_al_sta_has_route(void)
{
    struct netif *nif = (struct netif *)fhost_to_net_if(MGMR_VIF_STA);

    return !(nif == NULL || !netif_is_up(nif) ||
             ip4_addr_isany_val(*netif_ip4_addr(nif)));
}

int net_al_sta_arp_announce(void)
{
    struct netif *nif = (struct netif *)fhost_to_net_if(MGMR_VIF_STA);

    if (nif == NULL || !netif_is_up(nif) ||
        ip4_addr_isany_val(*netif_ip4_addr(nif))) {
        return -1;
    }
    LOCK_TCPIP_CORE();
    etharp_gratuitous(nif);
    UNLOCK_TCPIP_CORE();
    return 0;
}

#endif // NET_AL_NO_IP
