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
 * @file ncm_template.c
 * @brief USB CDC NCM + lwIP integration for ipro7
 *
 * CDC NCM USB device with lwIP TCP/IP stack. Windows 11 natively supports
 * NCM via the inbox UsbNcm.sys driver (RNDIS is deprecated on Win11).
 * IPRO7 obtains IP via DHCP from Windows ICS (192.168.137.x).
 */

#include <string.h>
#include "ipro_vendor_ids.h"
#include <generated/autoconf.h>
#include "shell.h"
#include "hal_dma.h"
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

/* ------------------------------------------------------------------ */
/* USB descriptor constants                                           */
/* ------------------------------------------------------------------ */
#define CDC_NCM_INT_EP 0x84
#define CDC_NCM_OUT_EP 0x03
#define CDC_NCM_IN_EP  0x81

#define USBD_VID           IPRO_USB_VID
#define USBD_PID           0x0701
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

#define CDC_NCM_MPS 64

/*
 * NCM descriptor with proper alternate settings for data interface.
 * Windows UsbNcm.sys requires: data intf alt 0 (0 EP) + alt 1 (2 bulk EP).
 * CDC_NCM_DESCRIPTOR_INIT only has alt 0 with endpoints, so we build manually.
 *
 * Layout: IAD(8) + CommIntf(9) + CDC_Header(5) + CDC_Union(5) +
 *         CDC_ETH(13) + CDC_NCM(6) + INT_EP(7) +
 *         DataIntf_alt0(9,0EP) + DataIntf_alt1(9,2EP) + OUT_EP(7) + IN_EP(7)
 * Total function descriptor = 85 bytes
 */
#define CDC_NCM_FUNC_DESC_LEN 85
#define USB_CONFIG_SIZE (9 + CDC_NCM_FUNC_DESC_LEN)

static const uint8_t ncm_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0x02, 0x0D, 0x00, USBD_VID, USBD_PID, 0x0100, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    /* --- IAD (8 bytes) --- */
    0x08, USB_DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION,
    0x00, /* bFirstInterface */
    0x02, /* bInterfaceCount */
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
    0x04, /* iMACAddress = string index 4 */
    DBVAL_BE(0x00000000), /* bmEthernetStatistics */
    WBVAL(CONFIG_CDC_NCM_ETH_MAX_SEGSZE), /* wMaxSegmentSize */
    WBVAL(0x0000), /* wNumberMCFilters */
    0x00, /* bNumberPowerFilters */
    /* CDC NCM Functional Descriptor: bmNetworkCapabilities=0x00 (minimal) */
    0x06, CDC_CS_INTERFACE, CDC_FUNC_DESC_NCM, 0x00, 0x01, 0x00,
    /* Interrupt IN endpoint */
    0x07, USB_DESCRIPTOR_TYPE_ENDPOINT,
    CDC_NCM_INT_EP, 0x03, WBVAL(16), 0x10,
    /* --- Data Interface: intf 1, alt 0, 0 endpoints (inactive) --- */
    0x09, USB_DESCRIPTOR_TYPE_INTERFACE,
    0x01, 0x00, 0x00, /* bInterfaceNumber=1, bAlternateSetting=0, bNumEndpoints=0 */
    CDC_DATA_INTERFACE_CLASS, 0x00, 0x01, /* bInterfaceProtocol=NTB */
    0x00,
    /* --- Data Interface: intf 1, alt 1, 2 endpoints (active) --- */
    0x09, USB_DESCRIPTOR_TYPE_INTERFACE,
    0x01, 0x01, 0x02, /* bInterfaceNumber=1, bAlternateSetting=1, bNumEndpoints=2 */
    CDC_DATA_INTERFACE_CLASS, 0x00, 0x01, /* bInterfaceProtocol=NTB */
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
    0x14,
    USB_DESCRIPTOR_TYPE_STRING,
    'C', 0x00, 'h', 0x00, 'e', 0x00, 'r', 0x00,
    'r', 0x00, 'y', 0x00, 'U', 0x00, 'S', 0x00,
    'B', 0x00,
    /* String 2: Product */
    0x1A,
    USB_DESCRIPTOR_TYPE_STRING,
    'C', 0x00, 'D', 0x00, 'C', 0x00, ' ', 0x00,
    'N', 0x00, 'C', 0x00, 'M', 0x00, ' ', 0x00,
    'D', 0x00, 'E', 0x00, 'M', 0x00, 'O', 0x00,
    /* String 3: Serial Number */
    0x16,
    USB_DESCRIPTOR_TYPE_STRING,
    '2', 0x00, '0', 0x00, '2', 0x00, '5', 0x00,
    '0', 0x00, '1', 0x00, '0', 0x00, '1', 0x00,
    '0', 0x00, '0', 0x00,
    /* String 4: MAC address "C07415A13D7E" (OUI C0:74:15, host adapter) */
    0x1A,
    USB_DESCRIPTOR_TYPE_STRING,
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

#define NCM_RX_TASK_STACK_SIZE 1024
#define NCM_RX_TASK_PRIO       28

/* ------------------------------------------------------------------ */
/* Static data                                                        */
/* ------------------------------------------------------------------ */
/* Device MAC must differ from iMACAddress (host adapter MAC C0:74:15:A1:3D:7E).
 * Using same MAC causes NDIS to drop frames as loopback. */
static uint8_t ncm_mac[6] = { 0xC0, 0x74, 0x15, 0xB2, 0x4E, 0x8F };

static struct netif ncm_netif;
static SemaphoreHandle_t ncm_rx_sem;
static StackType_t ncm_rx_stack[NCM_RX_TASK_STACK_SIZE];
static StaticTask_t ncm_rx_handle;

static volatile bool g_ncm_connected;
static volatile bool g_ncm_link_notified;
static volatile bool g_ncm_disconnect_pending;
static volatile bool g_ncm_data_intf_ready;

static struct usbd_interface intf0;
static struct usbd_interface intf1;

static TimerHandle_t ncm_tx_flush_timer;

/* --- NCM DMA RX (CH1 <- EP2 OUT) ---
 * P2M DMA with hardware handshake: USB EP FIFO asserts dma_rx_req
 * when data is ready, DMA reads one byte per request.  This replaces
 * the CPU byte-by-byte FIFO read in the ISR that suffered 10% per-
 * packet errors (busy + rxz_still) due to EP_DONE-to-FIFO timing.
 *
 * TODO: EP2 DMA P2M causes NTH sig BAD errors on standalone NCM;
 * EP3 (miniclaw composite) works fine.  Disabled until root cause
 * is found — may be EP2-specific HW issue or DMA request conflict. */
#if 0
static dma_device_t ncm_rx_dma_dev = {
    .id = DMA0_ID,
    .ch = DMA0_CH1_INDEX,
    .transfer_mode = DMA_LLI_ONCE_MODE,
    .direction = DMA_PERIPH_TO_MEMORY,
    .src_req = DMA_REQUEST_USB_EP2,
    .dst_req = DMA_REQUEST_NONE,
    .src_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
    .dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
    .src_burst_size = DMA_BURST_INCR1,
    .dst_burst_size = DMA_BURST_INCR1,
    .src_width = DMA_TRANSFER_WIDTH_8BIT,
    .dst_width = DMA_TRANSFER_WIDTH_8BIT,
    .lli_cfg = NULL,
    .callback = NULL,
};

static void ncm_rx_dma_init(void)
{
    dma_setConfig(&ncm_rx_dma_dev);
    dma_init(DMA0_ID, DMA0_CH1_INDEX);
    usbd_ep_setup_dma_rx(CDC_NCM_OUT_EP, DMA0_CH1_INDEX);
}
#endif

/* ------------------------------------------------------------------ */
/* lwIP netif driver                                                  */
/* ------------------------------------------------------------------ */

static void ncm_tx_flush_callback(TimerHandle_t timer)
{
    taskENTER_CRITICAL();
    int ret = usbd_cdc_ncm_tx_flush();
    taskEXIT_CRITICAL();

    /* If USB IN was busy, the ACK stays in the aggregation buffer.
     * Restart the one-shot timer to retry — without this, window
     * update ACKs can be stuck indefinitely. */
    if (ret == -USB_ERR_BUSY)
        xTimerReset(timer, 0);
}

static err_t ncm_linkoutput(struct netif *netif, struct pbuf *p)
{
    (void)netif;
    int ret;

    taskENTER_CRITICAL();
    ret = usbd_cdc_ncm_eth_tx(p);
    taskEXIT_CRITICAL();

    if (ret == 0) {
        if (ncm_tx_flush_timer)
            xTimerReset(ncm_tx_flush_timer, 0);
        return ERR_OK;
    }
    if (ret == -USB_ERR_BUSY)
        return ERR_WOULDBLOCK;
    return ERR_IF;
}

static void ncm_rx_task(void *arg)
{
    struct netif *nif = (struct netif *)arg;
    struct pbuf *p;
    bool dhcp_started = false;
    bool ip_printed = false;
    int dhcp_wait = 0;
    int arp_retry = 0;
    ip4_addr_t static_gw;

    IP4_ADDR(&static_gw, 192, 168, 137, 1);

    for (;;) {
        xSemaphoreTake(ncm_rx_sem, pdMS_TO_TICKS(2));

        /* Send link-up notification as soon as host configures the device.
         * NCM host (Windows UsbNcm.sys) waits for NETWORK_CONNECTION
         * notification before sending any data, so we must not gate this
         * on receiving RX packets. */
        if (g_ncm_data_intf_ready && !g_ncm_link_notified) {
            int ret = usbd_cdc_ncm_send_connect_status(true);
            if (ret < 0) {
                /* INT IN EP not ready — retry next iteration */
                continue;
            }

            g_ncm_link_notified = true;
            g_ncm_data_intf_ready = false;

            if (dhcp_started) {
                dhcp_stop(nif);
                netif_set_link_down(nif);
            }

            netif_set_link_up(nif);
            vTaskDelay(pdMS_TO_TICKS(100));
            dhcp_start(nif);
            dhcp_started = true;
            ip_printed = false;
            arp_retry = 0;
            printf("[NCM] link up, DHCP started\r\n");
        }

        /* Drain all available RX datagrams */
        while ((p = usbd_cdc_ncm_eth_rx()) != NULL) {
            if (nif->input(p, nif) != ERR_OK)
                pbuf_free(p);
        }

        /* Flush pending TX (e.g. TCP ACKs) immediately after processing
         * RX data.  Without this, ACKs sit in the aggregation buffer
         * until the 1 ms flush timer fires, adding up to 1 ms per
         * round-trip and starving TCP throughput on FS USB. */
        taskENTER_CRITICAL();
        usbd_cdc_ncm_tx_flush();
        taskEXIT_CRITICAL();

        /* Recover stale TX: if EP IN is halted by host the bulk_in
         * callback never fires, blocking all TX indefinitely. */
        usbd_cdc_ncm_tx_recover_stale(50);

        /* RX stale recovery: detect "lost" state where EP_RDY=0,
         * both slots free, not stalled.  Check every 500 iterations
         * (~1s) to avoid overhead.  Re-arm the endpoint to resume
         * receiving data. */
        {
            extern uint8_t usbd_cdc_ncm_get_rx_stalled(void);
            extern void usbd_cdc_ncm_recover_stale_rx(void);
            static uint32_t rx_watchdog_cnt;
            static uint32_t rx_watchdog_hit;
            if (++rx_watchdog_cnt >= 500) {
                rx_watchdog_cnt = 0;
                uint8_t stalled = usbd_cdc_ncm_get_rx_stalled();
                int ep_rdy = usbd_ep_is_ready(CDC_NCM_OUT_EP);
                if (g_ncm_link_notified && !stalled && ep_rdy == 0) {
                    rx_watchdog_hit++;
                    printf("[WD] EP OUT lost! stall=%u rdy=%d hit=%lu\r\n",
                           stalled, ep_rdy, rx_watchdog_hit);
                    usbd_cdc_ncm_recover_stale_rx();
                }
            }
        }

        if (dhcp_started && !ip_printed && !ip4_addr_isany_val(*netif_ip4_addr(nif))) {
            printf("[NCM] DHCP got IP: %s\r\n", ip4addr_ntoa(netif_ip4_addr(nif)));
            printf("[NCM]         GW: %s\r\n", ip4addr_ntoa(netif_ip4_gw(nif)));
            ip_printed = true;
        }

        /* Static IP fallback if DHCP doesn't respond within ~5 seconds */
        if (dhcp_started && !ip_printed) {
            dhcp_wait++;
            if (dhcp_wait > 2500) { /* 2500 * 2ms = 5s */
                ip4_addr_t ip, mask;
                IP4_ADDR(&ip, 192, 168, 137, 2);
                IP4_ADDR(&mask, 255, 255, 255, 0);
                dhcp_stop(nif);
                netif_set_addr(nif, &ip, &mask, &static_gw);
                ip_addr_t dns1, dns2;
                IP_ADDR4(&dns1, 192, 168, 137, 1);
                IP_ADDR4(&dns2, 8, 8, 8, 8);
                dns_setserver(0, &dns1);
                dns_setserver(1, &dns2);
                printf("[NCM] DHCP timeout, static: 192.168.137.2\r\n");
                arp_retry = 1;
                ip_printed = true;
                dhcp_started = false;
            }
        }

        /* Non-blocking ARP retry: send every 500th loop iteration (1s)
         * for up to 5 attempts. RX keeps running between retries. */
        if (arp_retry > 0) {
            if ((arp_retry % 500) == 1) { /* fire at iter 1, 501, 1001, ... */
                struct eth_addr *eth_ret;
                const ip4_addr_t *ip_ret;

                if (etharp_find_addr(nif, &static_gw,
                                     &eth_ret, &ip_ret) >= 0) {
                    printf("[NCM] gateway ARP resolved\r\n");
                    arp_retry = 0;
                } else {
                    etharp_gratuitous(nif);
                    etharp_request(nif, &static_gw);
                }
            }
            if (arp_retry > 2500) /* 2500 * 2ms = 5s total */
                arp_retry = 0;
            else if (arp_retry > 0)
                arp_retry++;
        }

        if (g_ncm_disconnect_pending) {
            g_ncm_disconnect_pending = false;
            if (dhcp_started) {
                dhcp_stop(nif);
                dhcp_started = false;
            }
            netif_set_link_down(nif);
            ip_printed = false;
            arp_retry = 0;
            printf("[NCM] link down\r\n");
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

    printf("[NCM] netif up, waiting for link + DHCP...\r\n");
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

void usbd_cdc_ncm_set_interface_done(void)
{
    g_ncm_data_intf_ready = true;
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
        g_ncm_data_intf_ready = false;
        g_ncm_disconnect_pending = true;
        break;
    default:
        break;
    }
}

/* ------------------------------------------------------------------ */
/* Shell commands                                                     */
/* ------------------------------------------------------------------ */
static int cmd_ncm_init(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    usbd_deinitialize();
    usbd_desc_register(ncm_descriptor);
    usbd_add_interface(usbd_cdc_ncm_init_intf(&intf0, CDC_NCM_INT_EP, CDC_NCM_OUT_EP, CDC_NCM_IN_EP));
    usbd_add_interface(usbd_cdc_ncm_init_intf(&intf1, CDC_NCM_INT_EP, CDC_NCM_OUT_EP, CDC_NCM_IN_EP));
    usbd_event_register_callback(ncm_event_handler);
    usbd_initialize();

    /* DMA for NCM EP OUT RX — must be after usbd_initialize().
     * TODO: EP2 DMA P2M causes NTH sig BAD errors; disabled until
     * root cause is found.  EP3 (miniclaw composite) works fine. */
    /* ncm_rx_dma_init(); */

    ncm_tx_flush_timer = xTimerCreate("ncm_flush", pdMS_TO_TICKS(1),
                                       pdFALSE, NULL,
                                       ncm_tx_flush_callback);

    tcpip_init(ncm_tcpip_init_done, &ncm_netif);

    printf("[NCM] initialized with lwIP, waiting for host...\r\n");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_ncm_init, usbd_ncm_init, USBD CDC NCM + lwIP Init);
