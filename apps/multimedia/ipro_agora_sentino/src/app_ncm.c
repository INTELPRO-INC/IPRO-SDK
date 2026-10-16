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
 * @file app_ncm.c
 * @brief USB CDC NCM network interface for Agora audio app
 *
 * EXACT COPY of ncm_template.c from ipro7_demo (which works).
 * Only differences: VID/PID, device class for macOS, #include "app_ncm.h".
 */

#include <string.h>
#include "ipro_vendor_ids.h"
#include <generated/autoconf.h>
#include "shell.h"
#include "usbd_core.h"
#include "usbd_cdc_ncm.h"
#include "usbd_event_common.h"
#include <compiler/common.h>

#include <FreeRTOS.h>
#include "task.h"
#include "semphr.h"
#include "timers.h"

#include "lwip/tcpip.h"
#include "lwip/netif.h"
#include "lwip/etharp.h"
#include "lwip/dhcp.h"
#include "lwip/dns.h"
#include "netif/etharp.h"

#include "app_ncm.h"

/* ------------------------------------------------------------------ */
/* USB descriptor constants                                           */
/* ------------------------------------------------------------------ */
#define CDC_NCM_INT_EP 0x83
#define CDC_NCM_OUT_EP 0x02
#define CDC_NCM_IN_EP  0x81

#define USBD_VID           IPRO_USB_VID  /* Linux Foundation */
#define USBD_PID           0x0138  /* NCM Gadget */
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

#define CDC_NCM_MPS 64

#define CDC_NCM_FUNC_DESC_LEN 85
#define USB_CONFIG_SIZE (9 + CDC_NCM_FUNC_DESC_LEN)

static const uint8_t ncm_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0x02, 0x0D, 0x00, USBD_VID, USBD_PID, 0x0100, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    /* --- IAD (8 bytes) --- */
    0x08, USB_DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION,
    0x00, 0x02,
    USB_DEVICE_CLASS_CDC, CDC_NETWORK_CONTROL_MODEL, CDC_COMMON_PROTOCOL_NONE, 0x00,
    /* --- Communication Interface: intf 0, alt 0, 1 EP --- */
    0x09, USB_DESCRIPTOR_TYPE_INTERFACE,
    0x00, 0x00, 0x01,
    USB_DEVICE_CLASS_CDC, CDC_NETWORK_CONTROL_MODEL, CDC_COMMON_PROTOCOL_NONE, 0x04,
    /* CDC Header */
    0x05, CDC_CS_INTERFACE, CDC_FUNC_DESC_HEADER, WBVAL(CDC_V1_10),
    /* CDC Union */
    0x05, CDC_CS_INTERFACE, CDC_FUNC_DESC_UNION, 0x00, 0x01,
    /* CDC Ethernet Networking */
    0x0D, CDC_CS_INTERFACE, CDC_FUNC_DESC_ETHERNET_NETWORKING,
    0x04,
    DBVAL_BE(0x00000000),
    WBVAL(CONFIG_CDC_NCM_ETH_MAX_SEGSZE),
    WBVAL(0x0000),
    0x00,
    /* CDC NCM Functional Descriptor */
    0x06, CDC_CS_INTERFACE, CDC_FUNC_DESC_NCM, 0x00, 0x01, 0x00,
    /* Interrupt IN endpoint */
    0x07, USB_DESCRIPTOR_TYPE_ENDPOINT,
    CDC_NCM_INT_EP, 0x03, WBVAL(16), 0x10,
    /* --- Data Interface: intf 1, alt 0, 0 endpoints (inactive) --- */
    0x09, USB_DESCRIPTOR_TYPE_INTERFACE,
    0x01, 0x00, 0x00,
    CDC_DATA_INTERFACE_CLASS, 0x00, 0x01,
    0x00,
    /* --- Data Interface: intf 1, alt 1, 2 endpoints (active) --- */
    0x09, USB_DESCRIPTOR_TYPE_INTERFACE,
    0x01, 0x01, 0x02,
    CDC_DATA_INTERFACE_CLASS, 0x00, 0x01,
    0x00,
    /* Bulk OUT endpoint */
    0x07, USB_DESCRIPTOR_TYPE_ENDPOINT,
    CDC_NCM_OUT_EP, 0x02, WBVAL(CDC_NCM_MPS), 0x00,
    /* Bulk IN endpoint */
    0x07, USB_DESCRIPTOR_TYPE_ENDPOINT,
    CDC_NCM_IN_EP, 0x02, WBVAL(CDC_NCM_MPS), 0x00,
    /* String 0: Language ID */
    USB_LANGID_INIT(USBD_LANGID_STRING),
    /* String 1: Manufacturer */
    0x14, USB_DESCRIPTOR_TYPE_STRING,
    'C', 0x00, 'h', 0x00, 'e', 0x00, 'r', 0x00,
    'r', 0x00, 'y', 0x00, 'U', 0x00, 'S', 0x00,
    'B', 0x00,
    /* String 2: Product */
    0x1A, USB_DESCRIPTOR_TYPE_STRING,
    'C', 0x00, 'D', 0x00, 'C', 0x00, ' ', 0x00,
    'N', 0x00, 'C', 0x00, 'M', 0x00, ' ', 0x00,
    'D', 0x00, 'E', 0x00, 'M', 0x00, 'O', 0x00,
    /* String 3: Serial Number */
    0x16, USB_DESCRIPTOR_TYPE_STRING,
    '2', 0x00, '0', 0x00, '2', 0x00, '5', 0x00,
    '0', 0x00, '1', 0x00, '0', 0x00, '1', 0x00,
    '0', 0x00, '0', 0x00,
    /* String 4: MAC address "C07415A13D7E" */
    0x1A, USB_DESCRIPTOR_TYPE_STRING,
    'C', 0x00, '0', 0x00, '7', 0x00, '4', 0x00,
    '1', 0x00, '5', 0x00, 'A', 0x00, '1', 0x00,
    '3', 0x00, 'D', 0x00, '7', 0x00, 'E', 0x00,
    /* Terminator */
    0x00
};

/* ------------------------------------------------------------------ */
/* Network configuration                                              */
/* ------------------------------------------------------------------ */
#define NCM_IFNAME0 'n'
#define NCM_IFNAME1 'c'

#define NCM_RX_TASK_STACK_SIZE 3072
#define NCM_RX_TASK_PRIO       16

/* ------------------------------------------------------------------ */
/* Static data                                                        */
/* ------------------------------------------------------------------ */
static uint8_t ncm_mac[6] = { 0xC0, 0x74, 0x15, 0xB2, 0x4E, 0x8F };

static struct netif ncm_netif;
static SemaphoreHandle_t ncm_rx_sem;
static StackType_t ncm_rx_stack[NCM_RX_TASK_STACK_SIZE];
static StaticTask_t ncm_rx_handle;

static volatile bool g_ncm_connected;
static volatile bool g_ncm_link_notified;
static volatile bool g_ncm_disconnect_pending;
static volatile bool g_ncm_link_is_up;  /* lwIP link is up */

static struct usbd_interface intf0;
static struct usbd_interface intf1;

static TimerHandle_t ncm_tx_flush_timer;

/* ------------------------------------------------------------------ */
/* lwIP netif driver — EXACTLY as ncm_template.c (no LOCK_TCPIP_CORE) */
/* ------------------------------------------------------------------ */

static void ncm_tx_flush_callback(TimerHandle_t timer)
{
    (void)timer;
    taskENTER_CRITICAL();
    usbd_cdc_ncm_tx_flush();
    taskEXIT_CRITICAL();
}

static uint32_t ncm_tx_count, ncm_tx_err_count;

static err_t ncm_linkoutput(struct netif *netif, struct pbuf *p)
{
    (void)netif;
    int ret;

    taskENTER_CRITICAL();
    ret = usbd_cdc_ncm_eth_tx(p);
    taskEXIT_CRITICAL();

    if (ret == 0) {
        ncm_tx_count++;
        if (ncm_tx_count <= 5 || (ncm_tx_count % 50) == 0)
            printf("[NCM-TX] frame %lu len=%u\r\n",
                   (unsigned long)ncm_tx_count, p->tot_len);
        if (ncm_tx_flush_timer)
            xTimerReset(ncm_tx_flush_timer, 0);
        return ERR_OK;
    }
    ncm_tx_err_count++;
    if (ncm_tx_err_count <= 5)
        printf("[NCM-TX] ERROR ret=%d len=%u\r\n", ret, p->tot_len);
    if (ret == -USB_ERR_BUSY)
        return ERR_WOULDBLOCK;
    return ERR_IF;
}

/* ---- RX task ---- */

static void ncm_rx_task(void *arg)
{
    struct netif *nif = (struct netif *)arg;
    struct pbuf *p;

    for (;;) {
        xSemaphoreTake(ncm_rx_sem, pdMS_TO_TICKS(200));

        /* Step 1: USB CONFIGURED → send notification → wait for alt=1.
         * macOS behavior varies: sometimes sends SET_INTF before notification,
         * sometimes waits for notification first. Handle both cases. */
        if (g_ncm_connected && !g_ncm_link_notified) {
            g_ncm_link_notified = true;
            extern volatile uint8_t ncm_current_alt;

            /* Wait for host EP0 setup sequence to complete (GET_NTB_PARAMETERS,
             * GET_NTB_INPUT_SIZE, etc.).  Sending INT notification during EP0
             * setup confuses macOS's NCM driver and it aborts initialization.
             * If host already sent SET_INTF alt=1 during this time, even better. */
            vTaskDelay(pdMS_TO_TICKS(3000));

            if (ncm_current_alt == 1) {
                printf("[NCM] alt=1 already set\r\n");
            }

            /* Send connect notification (SPEED_CHANGE → NETWORK_CONNECTION) */
            usbd_cdc_ncm_send_connect_status(true);
            printf("[NCM] connect notify sent\r\n");

            /* Wait for host to respond with SET_INTERFACE alt=1 */
            for (int w = 0; w < 50; w++) { /* up to 10s */
                if (ncm_current_alt == 1)
                    break;
                vTaskDelay(pdMS_TO_TICKS(200));
            }

            if (ncm_current_alt != 1) {
                printf("[NCM] WARNING: alt=%u after all retries\r\n",
                       ncm_current_alt);
                g_ncm_link_notified = false;
                vTaskDelay(pdMS_TO_TICKS(5000)); /* back off before retry */
                continue;
            }

            /* Link is up — lwIP can send/receive */
            vTaskDelay(pdMS_TO_TICKS(500));
            LOCK_TCPIP_CORE();
            netif_set_link_up(nif);
            UNLOCK_TCPIP_CORE();
            g_ncm_link_is_up = true;
            printf("[NCM] link UP (alt=1). Use 'ncm_dhcp' or 'ncm_static' to configure IP.\r\n");
        }

        /* Drain all available RX datagrams */
        while ((p = usbd_cdc_ncm_eth_rx()) != NULL) {
            if (nif->input(p, nif) != ERR_OK)
                pbuf_free(p);
        }

        /* Handle disconnect */
        if (g_ncm_disconnect_pending) {
            g_ncm_disconnect_pending = false;
            LOCK_TCPIP_CORE();
            netif_set_link_down(nif);
            UNLOCK_TCPIP_CORE();
            g_ncm_link_is_up = false;
            printf("[NCM] link DOWN\r\n");
        }
    }
}

static err_t ncm_netif_init(struct netif *netif)
{
    netif->name[0] = NCM_IFNAME0;
    netif->name[1] = NCM_IFNAME1;

    netif->hwaddr_len = 6;
    memcpy(netif->hwaddr, ncm_mac, 6);

    netif->mtu = 1500;
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;

    netif->output = etharp_output;
    netif->linkoutput = ncm_linkoutput;

    ncm_rx_sem = xSemaphoreCreateBinary();
    if (ncm_rx_sem == NULL)
        return ERR_MEM;

    xTaskCreateStatic(ncm_rx_task, "ncm_rx", NCM_RX_TASK_STACK_SIZE,
                      netif, NCM_RX_TASK_PRIO, ncm_rx_stack, &ncm_rx_handle);

    return ERR_OK;
}

/* ------------------------------------------------------------------ */
/* USB event handlers                                                 */
/* ------------------------------------------------------------------ */

void usbd_cdc_ncm_data_recv_done(uint8_t *buf, uint32_t len)
{
    (void)buf;
    (void)len;
    BaseType_t woken = pdFALSE;

    if (ncm_rx_sem)
        xSemaphoreGiveFromISR(ncm_rx_sem, &woken);
    portYIELD_FROM_ISR(woken);
}

void usbd_cdc_ncm_data_send_done(void)
{
}

static void ncm_event_handler(uint8_t event)
{
    switch (event) {
    case USBD_EVENT_CONFIGURED:
        g_ncm_connected = true;
        g_ncm_link_notified = false;
        break;
    case USBD_EVENT_DISCONNECTED:
        g_ncm_connected = false;
        g_ncm_disconnect_pending = true;
        break;
    default:
        break;
    }
}

/* ------------------------------------------------------------------ */
/* tcpip_init callback                                                */
/* ------------------------------------------------------------------ */
static void ncm_tcpip_init_done(void *arg)
{
    struct netif *nif = (struct netif *)arg;
    ip4_addr_t zero;

    ip4_addr_set_zero(&zero);
    netif_add(nif, &zero, &zero, &zero, NULL, ncm_netif_init, tcpip_input);
    netif_set_default(nif);
    netif_set_up(nif);

    printf("[NCM] netif up\r\n");
}

/* ------------------------------------------------------------------ */
/* Public init API                                                    */
/* ------------------------------------------------------------------ */
void app_ncm_init(void)
{
    usbd_deinitialize();
    usbd_desc_register(ncm_descriptor);
    usbd_add_interface(usbd_cdc_ncm_init_intf(&intf0, CDC_NCM_INT_EP, CDC_NCM_OUT_EP, CDC_NCM_IN_EP));
    usbd_add_interface(usbd_cdc_ncm_init_intf(&intf1, CDC_NCM_INT_EP, CDC_NCM_OUT_EP, CDC_NCM_IN_EP));
    usbd_event_register_callback(ncm_event_handler);
    /* Report 100 Mbps so macOS can match to a known Ethernet medium type.
     * Default 5 Mbps doesn't match any IOKit medium → link stays inactive. */
    uint32_t speed[2] = { 100000000u, 100000000u }; /* DL, UL in bits/s */
    usbd_cdc_ncm_set_connect_speed(speed);
    usbd_initialize();

    ncm_tx_flush_timer = xTimerCreate("ncm_flush", pdMS_TO_TICKS(1),
                                       pdFALSE, NULL,
                                       ncm_tx_flush_callback);

    tcpip_init(ncm_tcpip_init_done, &ncm_netif);

    printf("[NCM] initialized with lwIP, waiting for host...\r\n");
}

/* ------------------------------------------------------------------ */
/* Public API                                                         */
/* ------------------------------------------------------------------ */

struct netif *app_ncm_get_netif(void)
{
    return &ncm_netif;
}

/* ------------------------------------------------------------------ */
/* Shell commands for manual network control                          */
/* ------------------------------------------------------------------ */

static void cmd_ncm_status(int argc, char **argv)
{
    (void)argc; (void)argv;
    extern volatile uint32_t ncm_diag_set_intf;
    extern volatile uint8_t  ncm_current_alt;
    struct netif *nif = &ncm_netif;

    printf("NCM status:\r\n");
    printf("  USB connected: %s\r\n", g_ncm_connected ? "yes" : "no");
    printf("  Link notified: %s\r\n", g_ncm_link_notified ? "yes" : "no");
    printf("  Link up:       %s\r\n", g_ncm_link_is_up ? "yes" : "no");
    printf("  Alt setting:   %u\r\n", ncm_current_alt);
    printf("  SET_INTF cnt:  %lu\r\n", (unsigned long)ncm_diag_set_intf);
    printf("  IP:  %s\r\n", ip4addr_ntoa(netif_ip4_addr(nif)));
    printf("  GW:  %s\r\n", ip4addr_ntoa(netif_ip4_gw(nif)));
    printf("  DNS: %s\r\n", ip4addr_ntoa(dns_getserver(0)));
}
SHELL_CMD_EXPORT_ALIAS(cmd_ncm_status, ncm_status, Show NCM network status);

static void cmd_ncm_dhcp(int argc, char **argv)
{
    (void)argc; (void)argv;
    struct netif *nif = &ncm_netif;

    if (!g_ncm_link_is_up) {
        printf("ERROR: NCM link not up\r\n");
        return;
    }

    LOCK_TCPIP_CORE();
    dhcp_release_and_stop(nif);
    /* Clear any static IP */
    ip4_addr_t zero;
    ip4_addr_set_zero(&zero);
    netif_set_addr(nif, &zero, &zero, &zero);
    dhcp_start(nif);
    UNLOCK_TCPIP_CORE();
    printf("[NCM] DHCP client started\r\n");
}
SHELL_CMD_EXPORT_ALIAS(cmd_ncm_dhcp, ncm_dhcp, Start DHCP client);

static void cmd_ncm_static(int argc, char **argv)
{
    (void)argc; (void)argv;
    struct netif *nif = &ncm_netif;
    ip4_addr_t ip, mask, gw;
    ip_addr_t dns_addr;

    if (!g_ncm_link_is_up) {
        printf("ERROR: NCM link not up\r\n");
        return;
    }

    IP4_ADDR(&ip, 192, 168, 137, 2);
    IP4_ADDR(&mask, 255, 255, 255, 0);
    IP4_ADDR(&gw, 192, 168, 137, 1);

    LOCK_TCPIP_CORE();
    dhcp_release_and_stop(nif);
    netif_set_addr(nif, &ip, &mask, &gw);
    IP_ADDR4(&dns_addr, 8, 8, 8, 8);
    dns_setserver(0, &dns_addr);
    IP_ADDR4(&dns_addr, 1, 1, 1, 1);
    dns_setserver(1, &dns_addr);
    UNLOCK_TCPIP_CORE();

    printf("[NCM] Static IP: 192.168.137.2 GW: 192.168.137.1 DNS: 8.8.8.8\r\n");
}
SHELL_CMD_EXPORT_ALIAS(cmd_ncm_static, ncm_static, Set static IP 192.168.137.2);
