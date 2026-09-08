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
 * @file cdc_ecm_template.c
 * @brief USB CDC ECM (Ethernet Control Model) demo for ipro7
 *
 * Minimal CDC ECM device that enumerates as a USB Ethernet adapter.
 * Received packets are hex-dumped; a shell command sends a test packet.
 */

#include <generated/autoconf.h>
#include "ipro_vendor_ids.h"
#include "shell.h"
#include "usbd_core.h"
#include "usbd_cdc_ecm.h"
#include "usbd_event_common.h"
#include <compiler/common.h>

#define CDC_ECM_INT_EP 0x83
#define CDC_ECM_OUT_EP 0x02
#define CDC_ECM_IN_EP  0x81

#define USBD_VID           IPRO_USB_VID
#define USBD_PID           0x0701
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

#ifdef CONFIG_USB_HS
#define CDC_ECM_MPS 512
#else
#define CDC_ECM_MPS 64
#endif

#define USB_CONFIG_SIZE (9 + CDC_ECM_DESCRIPTOR_LEN)

/*!< global descriptor */
static const uint8_t cdc_ecm_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0x02, 0x06, 0x00, USBD_VID, USBD_PID, 0x0100, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    CDC_ECM_DESCRIPTOR_INIT(0x00, CDC_ECM_INT_EP, CDC_ECM_OUT_EP, CDC_ECM_IN_EP, CDC_ECM_MPS,
                            0x00000000, CONFIG_CDC_ECM_ETH_MAX_SEGSZE, 0x0000, 0x00, 0x03),
    /* String 0: Language ID */
    USB_LANGID_INIT(USBD_LANGID_STRING),
    /* String 1: Manufacturer */
    0x1C,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'I', 0x00,                  /* wcChar0 */
    'N', 0x00,                  /* wcChar1 */
    'T', 0x00,                  /* wcChar2 */
    'E', 0x00,                  /* wcChar3 */
    'L', 0x00,                  /* wcChar4 */
    'P', 0x00,                  /* wcChar5 */
    'R', 0x00,                  /* wcChar6 */
    'O', 0x00,                  /* wcChar7 */
    ' ', 0x00,                  /* wcChar8 */
    'I', 0x00,                  /* wcChar9 */
    'N', 0x00,                  /* wcChar10 */
    'C', 0x00,                  /* wcChar11 */
    '.', 0x00,                  /* wcChar12 */
    /* String 2: Product */
    0x1E,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'C', 0x00, 'D', 0x00, 'C', 0x00, ' ', 0x00,
    'E', 0x00, 'C', 0x00, 'M', 0x00, ' ', 0x00,
    'D', 0x00, 'E', 0x00, 'M', 0x00, 'O', 0x00,
    ' ', 0x00, ' ', 0x00,
    /* String 3: MAC address "010202030000" */
    0x1A,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    '0', 0x00, '1', 0x00, '0', 0x00, '2', 0x00,
    '0', 0x00, '2', 0x00, '0', 0x00, '3', 0x00,
    '0', 0x00, '0', 0x00, '0', 0x00, '0', 0x00,
    /* Terminator */
    0x00
};

static volatile bool g_ecm_connected = false;

static void ecm_event_handler(uint8_t event)
{
    switch (event) {
        case USBD_EVENT_CONFIGURED:
            g_ecm_connected = false;
            break;
        case USBD_EVENT_DISCONNECTED:
            g_ecm_connected = false;
            break;
        default:
            break;
    }
}

void usbd_cdc_ecm_packet_recv_done(uint8_t *buf, uint32_t len)
{
    printf("[ECM] rx %lu bytes:", (unsigned long)len);
    for (uint32_t i = 0; i < len && i < 32; i++)
        printf(" %02x", buf[i]);
    if (len > 32)
        printf(" ...");
    printf("\r\n");

    usbd_cdc_ecm_start_read_next_packet();
}

void usbd_cdc_ecm_packet_send_done(void)
{
    /* tx complete */
}

static struct usbd_interface intf0;
static struct usbd_interface intf1;

static int cmd_cdc_ecm_init(int argc, char **argv)
{
    usbd_deinitialize();
    usbd_desc_register(cdc_ecm_descriptor);
    usbd_add_interface(usbd_cdc_ecm_init_intf(&intf0, CDC_ECM_INT_EP, CDC_ECM_OUT_EP, CDC_ECM_IN_EP));
    usbd_add_interface(usbd_cdc_ecm_init_intf(&intf1, CDC_ECM_INT_EP, CDC_ECM_OUT_EP, CDC_ECM_IN_EP));
    usbd_event_register_callback(ecm_event_handler);
    usbd_initialize();

    /* notify host: link up */
    usbd_cdc_ecm_send_connect_status(true);
    g_ecm_connected = true;
    printf("[ECM] CDC ECM initialized\r\n");

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_cdc_ecm_init, usbd_ecm_init, USBD CDC ECM Init);

static int cmd_cdc_ecm_send(int argc, char **argv)
{
    if (!g_ecm_connected) {
        printf("[ECM] not initialized\r\n");
        return 0;
    }

    /* build a minimal test Ethernet frame (14-byte header + 46-byte payload) */
    uint8_t *tx_buf = usbd_cdc_ecm_get_tx_buffer();
    /* dst MAC: broadcast */
    memset(tx_buf, 0xFF, 6);
    /* src MAC */
    tx_buf[6]  = 0x01; tx_buf[7]  = 0x02; tx_buf[8]  = 0x02;
    tx_buf[9]  = 0x03; tx_buf[10] = 0x00; tx_buf[11] = 0x00;
    /* EtherType: 0x0800 (IPv4) */
    tx_buf[12] = 0x08; tx_buf[13] = 0x00;
    /* payload: pattern fill */
    for (int i = 14; i < 60; i++)
        tx_buf[i] = (uint8_t)(i & 0xFF);

    int ret = usbd_cdc_ecm_send_packet(tx_buf, 60);
    printf("[ECM] send %s (%d)\r\n", ret == 0 ? "ok" : "fail", ret);

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_cdc_ecm_send, usbd_ecm_send, USBD CDC ECM Send Test Packet);
