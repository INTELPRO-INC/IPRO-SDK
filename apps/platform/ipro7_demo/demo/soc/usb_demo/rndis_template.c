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
 * @file rndis_template.c
 * @brief USB RNDIS + lwIP integration for ipro7
 *
 * RNDIS USB device with lwIP TCP/IP stack. IPRO7 obtains IP via DHCP from
 * Windows ICS (192.168.137.x), enabling ping/iperf/socket communication
 * and internet access through Windows.
 */

#include <string.h>
#include "ipro_vendor_ids.h"
#include <generated/autoconf.h>
#include "shell.h"
#include "usbd_core.h"
#include "usbd_rndis.h"
#include "rndis_protocol.h"
#include "usbd_event_common.h"
#include <compiler/common.h>

#include <FreeRTOS.h>
#include "task.h"
#include "semphr.h"

#include "lwip/tcpip.h"
#include "lwip/netif.h"
#include "lwip/etharp.h"
#include "lwip/dhcp.h"
#include "netif/etharp.h"

/* ------------------------------------------------------------------ */
/* USB descriptor constants                                           */
/* ------------------------------------------------------------------ */
#define CDC_RNDIS_INT_EP 0x83
#define CDC_RNDIS_OUT_EP 0x02
#define CDC_RNDIS_IN_EP  0x81

#define USBD_VID           IPRO_USB_VID
#define USBD_PID           0x0701
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

#define CDC_RNDIS_MPS 64

#define USB_CONFIG_SIZE (9 + CDC_RNDIS_DESCRIPTOR_LEN)

static const uint8_t rndis_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xEF, 0x02, 0x01, USBD_VID, USBD_PID, 0x0100, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    CDC_RNDIS_DESCRIPTOR_INIT(0x00, CDC_RNDIS_INT_EP, CDC_RNDIS_OUT_EP, CDC_RNDIS_IN_EP, CDC_RNDIS_MPS, 0x00),
    /* String 0: Language ID */
    USB_LANGID_INIT(USBD_LANGID_STRING),
    /* String 1: Manufacturer */
    0x14,
    USB_DESCRIPTOR_TYPE_STRING,
    'C', 0x00, 'h', 0x00, 'e', 0x00, 'r', 0x00,
    'r', 0x00, 'y', 0x00, 'U', 0x00, 'S', 0x00,
    'B', 0x00,
    /* String 2: Product */
    0x1C,
    USB_DESCRIPTOR_TYPE_STRING,
    'R', 0x00, 'N', 0x00, 'D', 0x00, 'I', 0x00,
    'S', 0x00, ' ', 0x00, 'D', 0x00, 'E', 0x00,
    'M', 0x00, 'O', 0x00, ' ', 0x00, ' ', 0x00,
    ' ', 0x00,
    /* String 3: Serial Number */
    0x16,
    USB_DESCRIPTOR_TYPE_STRING,
    '2', 0x00, '0', 0x00, '2', 0x00, '5', 0x00,
    '0', 0x00, '1', 0x00, '0', 0x00, '1', 0x00,
    '0', 0x00, '0', 0x00,
    /* Terminator */
    0x00
};

/* ------------------------------------------------------------------ */
/* Network configuration                                              */
/* ------------------------------------------------------------------ */
#define RNDIS_IFNAME0 'r'
#define RNDIS_IFNAME1 'n'

#define RNDIS_RX_TASK_STACK_SIZE 1024
#define RNDIS_RX_TASK_PRIO       16

/* ------------------------------------------------------------------ */
/* Static data                                                        */
/* ------------------------------------------------------------------ */
/* bit 0 = 0 (unicast), bit 1 = 1 (locally administered) */
static uint8_t rndis_mac[6] = { 0x02, 0x02, 0x02, 0x03, 0x00, 0x00 };

static struct netif rndis_netif;
static SemaphoreHandle_t rndis_rx_sem;
static StackType_t rndis_rx_stack[RNDIS_RX_TASK_STACK_SIZE];
static StaticTask_t rndis_rx_handle;

static volatile bool g_rndis_connected;
static volatile bool g_rndis_link_notified;
static volatile bool g_rndis_disconnect_pending;

static struct usbd_interface intf0;
static struct usbd_interface intf1;

/* ------------------------------------------------------------------ */
/* lwIP netif driver                                                  */
/* ------------------------------------------------------------------ */

/**
 * TX path: lwIP calls this to send an Ethernet frame via USB RNDIS.
 */
static err_t rndis_linkoutput(struct netif *netif, struct pbuf *p)
{
    (void)netif;

    int ret = usbd_rndis_eth_tx(p);
    if (ret == 0)
        return ERR_OK;
    if (ret == -USB_ERR_BUSY)
        return ERR_WOULDBLOCK;
    return ERR_IF;
}

/**
 * RX task: blocks on semaphore, retrieves pbuf from CherryUSB, feeds lwIP.
 *
 * Link-up notification is deferred here from usbd_rndis_data_recv_done (ISR)
 * because netif_set_link_up and usbd_rndis_set_connect are not ISR-safe.
 */
static void rndis_rx_task(void *arg)
{
    struct netif *nif = (struct netif *)arg;
    struct pbuf *p;
    bool dhcp_started = false;
    bool ip_printed = false;
    int dhcp_wait = 0;

    for (;;) {
        /* Use timeout so we can detect disconnect even without RX data */
        if (xSemaphoreTake(rndis_rx_sem, pdMS_TO_TICKS(200)) == pdTRUE) {
            /* First packet after (re-)enumeration: notify host, start DHCP */
            if (!g_rndis_link_notified) {
                g_rndis_link_notified = true;

                /* Clean up previous DHCP session on re-enumeration */
                if (dhcp_started) {
                    dhcp_stop(nif);
                    netif_set_link_down(nif);
                }

                usbd_rndis_set_connect(true);
                netif_set_link_up(nif);
                dhcp_start(nif);
                dhcp_started = true;
                ip_printed = false;
                printf("[RNDIS] link up, DHCP client started\r\n");
            }

            p = usbd_rndis_eth_rx();
            if (p) {
                if (nif->input(p, nif) != ERR_OK)
                    pbuf_free(p);
            }
        }

        /* Print IP once DHCP assigns it */
        if (dhcp_started && !ip_printed && !ip4_addr_isany_val(*netif_ip4_addr(nif))) {
            printf("[RNDIS] DHCP got IP: %s\r\n", ip4addr_ntoa(netif_ip4_addr(nif)));
            printf("[RNDIS]         GW: %s\r\n", ip4addr_ntoa(netif_ip4_gw(nif)));
            ip_printed = true;
        }

        /* Static IP fallback if DHCP doesn't respond within ~60 seconds */
        if (dhcp_started && !ip_printed) {
            dhcp_wait++;
            if (dhcp_wait > 300) { /* 300 * 200ms = 60s */
                ip4_addr_t ip, mask, gw;
                IP4_ADDR(&ip, 192, 168, 137, 2);
                IP4_ADDR(&mask, 255, 255, 255, 0);
                IP4_ADDR(&gw, 192, 168, 137, 1);
                dhcp_stop(nif);
                netif_set_addr(nif, &ip, &mask, &gw);
                printf("[RNDIS] DHCP timeout, static IP: 192.168.137.2\r\n");
                ip_printed = true;
                dhcp_started = false;
            }
        }

        /* Handle disconnect in task context (set by ISR via flag) */
        if (g_rndis_disconnect_pending) {
            g_rndis_disconnect_pending = false;
            if (dhcp_started) {
                dhcp_stop(nif);
                dhcp_started = false;
            }
            netif_set_link_down(nif);
            ip_printed = false;
            printf("[RNDIS] link down\r\n");
        }
    }
}

/**
 * lwIP netif init callback — sets up MAC, MTU, flags, and starts RX task.
 */
static err_t rndis_netif_init(struct netif *netif)
{
    netif->name[0] = RNDIS_IFNAME0;
    netif->name[1] = RNDIS_IFNAME1;

    netif->hwaddr_len = 6;
    memcpy(netif->hwaddr, rndis_mac, 6);

    netif->mtu = 1500;
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;

    netif->output = etharp_output;
    netif->linkoutput = rndis_linkoutput;

    rndis_rx_sem = xSemaphoreCreateBinary();
    if (rndis_rx_sem == NULL)
        return ERR_MEM;

    xTaskCreateStatic(rndis_rx_task, "rndis_rx", RNDIS_RX_TASK_STACK_SIZE,
                      netif, RNDIS_RX_TASK_PRIO, rndis_rx_stack, &rndis_rx_handle);

    return ERR_OK;
}

/* ------------------------------------------------------------------ */
/* tcpip_init callback — register netif (IP via DHCP after link up)   */
/* ------------------------------------------------------------------ */
static void rndis_tcpip_init_done(void *arg)
{
    struct netif *nif = (struct netif *)arg;
    ip4_addr_t zero;

    ip4_addr_set_zero(&zero);
    netif_add(nif, &zero, &zero, &zero, NULL, rndis_netif_init, tcpip_input);
    netif_set_default(nif);
    netif_set_up(nif);

    printf("[RNDIS] netif up, waiting for link + DHCP...\r\n");
}

/* ------------------------------------------------------------------ */
/* USB event handlers                                                 */
/* ------------------------------------------------------------------ */

/**
 * Override weak usbd_rndis_data_recv_done — wake RX task via semaphore.
 *
 * Called from USB IRQ context — only give semaphore here.
 * Link-up notification is handled in rndis_rx_task (task context).
 */
void usbd_rndis_data_recv_done(uint32_t len)
{
    (void)len;
    BaseType_t woken = pdFALSE;

    if (rndis_rx_sem)
        xSemaphoreGiveFromISR(rndis_rx_sem, &woken);
    portYIELD_FROM_ISR(woken);
}

void usbd_rndis_data_send_done(uint32_t len)
{
    (void)len;
}

static void rndis_event_handler(uint8_t event)
{
    switch (event) {
    case USBD_EVENT_CONFIGURED:
        /*
         * In lwIP mode, the internal rndis_notify_handler already starts
         * bulk OUT read with its own buffer. We just track state here.
         */
        g_rndis_connected = true;
        g_rndis_link_notified = false;
        break;
    case USBD_EVENT_DISCONNECTED:
        g_rndis_connected = false;
        g_rndis_disconnect_pending = true;
        break;
    default:
        break;
    }
}

/* ------------------------------------------------------------------ */
/* Shell commands                                                     */
/* ------------------------------------------------------------------ */
static int cmd_rndis_init(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    usbd_deinitialize();
    usbd_desc_register(rndis_descriptor);
    usbd_add_interface(usbd_rndis_init_intf(&intf0, CDC_RNDIS_OUT_EP, CDC_RNDIS_IN_EP, CDC_RNDIS_INT_EP, rndis_mac));
    usbd_add_interface(usbd_rndis_init_intf(&intf1, CDC_RNDIS_OUT_EP, CDC_RNDIS_IN_EP, CDC_RNDIS_INT_EP, rndis_mac));
    usbd_event_register_callback(rndis_event_handler);
    usbd_initialize();

    tcpip_init(rndis_tcpip_init_done, &rndis_netif);

    printf("[RNDIS] initialized with lwIP, waiting for host...\r\n");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_rndis_init, usbd_rndis_init, USBD RNDIS + lwIP Init);
