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

#include <stdarg.h>
#include "ipro_vendor_ids.h"
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "semphr.h"

#include "usbd_core.h"
#include "usbd_cdc.h"
#include "usbd_event_common.h"

#include <compiler/common.h>

#include "app_log.h"
#include "app_usb_cdc.h"

#define LOG_TAG "USB_CDC"

/* Endpoint addresses */
#define CDC_IN_EP  0x81
#define CDC_OUT_EP 0x02
#define CDC_INT_EP 0x83

#define USBD_VID           IPRO_USB_VID /* Linux Foundation */
#define USBD_PID           0x0104 /* Multifunction Composite Gadget */
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

#define USB_CONFIG_SIZE (9 + CDC_ACM_DESCRIPTOR_LEN)

#ifdef CONFIG_USB_HS
#define CDC_MAX_MPS 512
#else
#define CDC_MAX_MPS 64
#endif

/* TX ring buffer size */
#define TX_BUF_SIZE 2048

/* USB descriptor */
static const uint8_t cdc_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xEF, 0x02, 0x01, USBD_VID, USBD_PID, 0x0100, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    CDC_ACM_DESCRIPTOR_INIT(0x00, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP, CDC_MAX_MPS, 0x02),
    /* string0: Language ID */
    USB_LANGID_INIT(USBD_LANGID_STRING),
    /* string1: Manufacturer */
    0x0C,
    USB_DESCRIPTOR_TYPE_STRING,
    'I', 0x00,
    'P', 0x00,
    'R', 0x00,
    'O', 0x00,
    '7', 0x00,
    /* string2: Product — appears as device name on macOS */
    0x1E,                       /* bLength = 2 + 14*2 = 30 */
    USB_DESCRIPTOR_TYPE_STRING,
    'I', 0x00,
    'P', 0x00,
    'R', 0x00,
    'O', 0x00,
    '7', 0x00,
    ' ', 0x00,
    'A', 0x00,
    'I', 0x00,
    ' ', 0x00,
    'V', 0x00,
    'o', 0x00,
    'i', 0x00,
    'c', 0x00,
    'e', 0x00,
    /* string3: Serial number */
    0x12,
    USB_DESCRIPTOR_TYPE_STRING,
    '2', 0x00,
    '0', 0x00,
    '2', 0x00,
    '6', 0x00,
    '0', 0x00,
    '3', 0x00,
    '0', 0x00,
    '2', 0x00,
    0x00
};

/* DMA-aligned buffers in PSRAM */
static uint8_t s_rx_buf[CDC_MAX_MPS] ATTR_EALIGN(32) ATTR_PSRAM_BSS_SECTION;
static uint8_t s_tx_dma_buf[CDC_MAX_MPS] ATTR_EALIGN(32) ATTR_PSRAM_BSS_SECTION;

/* TX ring buffer in PSRAM */
static uint8_t s_tx_ring[TX_BUF_SIZE] ATTR_PSRAM_BSS_SECTION;
static volatile uint32_t s_tx_head;  /* write position */
static volatile uint32_t s_tx_tail;  /* read position */

static volatile bool s_tx_busy;
static volatile bool s_dtr_enabled;
static volatile bool s_configured;

static SemaphoreHandle_t s_tx_mutex;

/* Forward declarations */
static void cdc_flush_tx(void);

/* ---- USB event / endpoint callbacks ---- */

static void cdc_event_handler(uint8_t event)
{
    switch (event) {
    case USBD_EVENT_CONFIGURED:
        s_configured = true;
        usbd_ep_start_read(CDC_OUT_EP, s_rx_buf, sizeof(s_rx_buf));
        IPRO_LOGI(LOG_TAG, "USB configured");
        break;
    case USBD_EVENT_RESET:
    case USBD_EVENT_DISCONNECTED:
        s_configured = false;
        s_dtr_enabled = false;
        s_tx_busy = false;
        break;
    default:
        break;
    }
}

void usbd_cdc_acm_bulk_out(uint8_t ep, uint32_t nbytes)
{
    /* Discard received data — re-arm RX */
    usbd_ep_start_read(CDC_OUT_EP, s_rx_buf, sizeof(s_rx_buf));
}

void usbd_cdc_acm_bulk_in(uint8_t ep, uint32_t nbytes)
{
    if ((nbytes % CDC_MAX_MPS) == 0 && nbytes) {
        /* Send ZLP */
        usbd_ep_start_write(CDC_IN_EP, NULL, 0);
    } else {
        s_tx_busy = false;
        /* Try to send more data from ring buffer */
        cdc_flush_tx();
    }
}

void usbd_cdc_acm_set_dtr(uint8_t intf, bool dtr)
{
    s_dtr_enabled = dtr;
    if (dtr) {
        IPRO_LOGI(LOG_TAG, "Host opened port");
        /* Flush any pending data */
        cdc_flush_tx();
    } else {
        IPRO_LOGI(LOG_TAG, "Host closed port");
    }
}

/* ---- TX ring buffer helpers ---- */

static uint32_t ring_used(void)
{
    uint32_t h = s_tx_head, t = s_tx_tail;
    return (h >= t) ? (h - t) : (TX_BUF_SIZE - t + h);
}

static void cdc_flush_tx(void)
{
    if (s_tx_busy || !s_dtr_enabled || !s_configured)
        return;

    uint32_t used = ring_used();
    if (used == 0)
        return;

    /* Copy contiguous chunk from ring to DMA buffer */
    uint32_t tail = s_tx_tail;
    uint32_t chunk = used;
    if (chunk > CDC_MAX_MPS)
        chunk = CDC_MAX_MPS;
    /* Don't wrap around in one copy */
    uint32_t to_end = TX_BUF_SIZE - tail;
    if (chunk > to_end)
        chunk = to_end;

    memcpy(s_tx_dma_buf, &s_tx_ring[tail], chunk);
    s_tx_tail = (tail + chunk) % TX_BUF_SIZE;

    /* Flush cache for DMA */
    L1C_DCACHE_CLEAN_RANGE(s_tx_dma_buf, chunk);

    s_tx_busy = true;
    usbd_ep_start_write(CDC_IN_EP, s_tx_dma_buf, chunk);
}

/* ---- Public API ---- */

static struct usbd_endpoint s_cdc_out_ep = {
    .ep_addr = CDC_OUT_EP,
    .ep_cb = usbd_cdc_acm_bulk_out
};

static struct usbd_endpoint s_cdc_in_ep = {
    .ep_addr = CDC_IN_EP,
    .ep_cb = usbd_cdc_acm_bulk_in
};

static struct usbd_interface s_intf0;
static struct usbd_interface s_intf1;

void app_usb_cdc_init(void)
{
    s_tx_mutex = xSemaphoreCreateMutex();
    configASSERT(s_tx_mutex);

    s_tx_head = 0;
    s_tx_tail = 0;
    s_tx_busy = false;
    s_dtr_enabled = false;
    s_configured = false;

    usbd_desc_register(cdc_descriptor);
    usbd_add_interface(usbd_cdc_acm_init_intf(&s_intf0));
    usbd_add_interface(usbd_cdc_acm_init_intf(&s_intf1));
    usbd_add_endpoint(&s_cdc_out_ep);
    usbd_add_endpoint(&s_cdc_in_ep);
    usbd_event_register_callback(cdc_event_handler);
    usbd_initialize();

    IPRO_LOGI(LOG_TAG, "USB CDC ACM initialized (VID=%04X PID=%04X)", USBD_VID, USBD_PID);
}

int app_usb_cdc_write(const char *data, uint32_t len)
{
    if (!data || len == 0)
        return 0;
    if (!s_configured)
        return -1;

    xSemaphoreTake(s_tx_mutex, portMAX_DELAY);

    /* Calculate available space in ring buffer */
    uint32_t used = ring_used();
    uint32_t avail = TX_BUF_SIZE - 1 - used;
    if (len > avail)
        len = avail;

    /* Copy to ring buffer */
    uint32_t head = s_tx_head;
    for (uint32_t i = 0; i < len; i++) {
        s_tx_ring[head] = (uint8_t)data[i];
        head = (head + 1) % TX_BUF_SIZE;
    }
    s_tx_head = head;

    /* Kick TX if idle */
    cdc_flush_tx();

    xSemaphoreGive(s_tx_mutex);

    return (int)len;
}

int app_usb_cdc_printf(const char *fmt, ...)
{
    char buf[256];
    va_list args;

    va_start(args, fmt);
    int n = vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    if (n <= 0)
        return n;
    if ((uint32_t)n >= sizeof(buf))
        n = sizeof(buf) - 1;

    return app_usb_cdc_write(buf, (uint32_t)n);
}
