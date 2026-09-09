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
 * OTA staging over USB CDC.
 *
 * `ota_usb` is the USB sibling of `ota_zmodem`: same app-level OTA flow (stream
 * the package to FW-B via ota_common, SHA256-validate, flip activeIndex,
 * reboot -> boot2 decompresses FW-B -> FW-A); only the transport differs.
 *
 * USB has its own pins, so there is NO console/UART handoff -- the shell stays
 * live on UART0 and prints progress during the transfer. Bulk-OUT gives free
 * hardware flow control: the OUT callback only flags the chunk, the command
 * task sinks it to flash and *then* re-arms the read, so the host is NAK'd while
 * the DUT writes flash -- no ACK protocol, no framing, no throughput tuning.
 *
 * Length is self-describing: the OTA header (first 512 B) carries payload_len,
 * so total = 512 + payload_len; the receiver stops there. On completion it
 * sends a status line back on CDC IN so the host knows pass/fail before reboot.
 */
#include <generated/autoconf.h>
#include "ipro_vendor_ids.h"

#if defined(CONFIG_COMPONENTS_CHERRYUSB_ENABLE)

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"
#include "shell.h"
#include "hal_flash.h"
#include "hal_sys.h"
#include "usbd_core.h"
#include "usbd_cdc.h"
#include "ota_usb_event.h"   /* usbd_event_register_callback (app provides the dispatcher) */
#include "ota_common.h"

#define CDC_IN_EP   0x81
#define CDC_OUT_EP  0x02
#define CDC_INT_EP  0x83

#define USBD_VID    IPRO_USB_VID
#define USBD_PID    0x06A0          /* distinct PID so the host can find the OTA CDC */

#ifdef CONFIG_USB_HS
#define CDC_MAX_MPS 512
#else
#define CDC_MAX_MPS 64
#endif

/* bulk-OUT DMA buffer (must be USB nocache). Larger = fewer OUT transfers =
 * less per-transfer host-stack latency (the receive rate is round-trip bound,
 * not bandwidth bound, since each completed read costs one host turnaround). */
#define OTA_USB_BUF         16384
#define OTA_USB_TIMEOUT_MS  30000

#define USB_CONFIG_SIZE (9 + CDC_ACM_DESCRIPTOR_LEN)

static const uint8_t ota_cdc_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xEF, 0x02, 0x01, USBD_VID, USBD_PID, 0x0100, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x02, 0x01, USB_CONFIG_BUS_POWERED, 100),
    CDC_ACM_DESCRIPTOR_INIT(0x00, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP, CDC_MAX_MPS, 0x02),
    USB_LANGID_INIT(1033),
    0x14, USB_DESCRIPTOR_TYPE_STRING,
    'C', 0, 'h', 0, 'e', 0, 'r', 0, 'r', 0, 'y', 0, 'U', 0, 'S', 0, 'B', 0,
    0x18, USB_DESCRIPTOR_TYPE_STRING,
    'I', 0, 'P', 0, 'R', 0, 'O', 0, '6', 0, ' ', 0, 'O', 0, 'T', 0, 'A', 0, ' ', 0, 'U', 0, 'S', 0,
    0x0a, USB_DESCRIPTOR_TYPE_STRING, 'o', 0, 't', 0, 'a', 0, 'u', 0,
#ifdef CONFIG_USB_HS
    0x0a, USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER, 0x00, 0x02, 0x02, 0x02, 0x01, 0x40, 0x01, 0x00,
#endif
    0x00
};

/* USB DMA can only touch non-cacheable memory. */
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX static uint8_t s_usb_buf[OTA_USB_BUF];
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX static uint8_t s_usb_tx[64];

static volatile bool     s_configured;
static volatile bool     s_rx_ready;   /* an OUT transfer completed, awaiting sink */
static volatile uint32_t s_rx_len;
static volatile bool     s_tx_busy;

/* OUT completion (ISR): flag the chunk; the command task sinks it and re-arms
 * the read -- leaving no read armed is what NAKs the host while we flash. */
static void ota_usb_out_cb(uint8_t ep, uint32_t nbytes)
{
    (void)ep;
    s_rx_len   = nbytes;
    s_rx_ready = true;
}

static void ota_usb_in_cb(uint8_t ep, uint32_t nbytes)
{
    (void)ep;
    (void)nbytes;
    s_tx_busy = false;
}

static void ota_usb_event(uint8_t event)
{
    if (event == USBD_EVENT_CONFIGURED) {
        s_configured = true;
        usbd_ep_start_read(CDC_OUT_EP, s_usb_buf, OTA_USB_BUF);
    }
}

static struct usbd_endpoint ota_out_ep = { .ep_addr = CDC_OUT_EP, .ep_cb = ota_usb_out_cb };
static struct usbd_endpoint ota_in_ep  = { .ep_addr = CDC_IN_EP,  .ep_cb = ota_usb_in_cb };
static struct usbd_interface ota_intf0;
static struct usbd_interface ota_intf1;

static void ota_usb_cdc_up(void)
{
    s_configured = false;
    s_rx_ready   = false;
    s_tx_busy    = false;

    usbd_deinitialize();
    usbd_desc_register(ota_cdc_descriptor);
    usbd_add_interface(usbd_cdc_acm_init_intf(&ota_intf0));
    usbd_add_interface(usbd_cdc_acm_init_intf(&ota_intf1));
    usbd_add_endpoint(&ota_out_ep);
    usbd_add_endpoint(&ota_in_ep);
    usbd_event_register_callback(ota_usb_event);
    usbd_initialize();
}

/* Send a short status line back to the host; bounded wait for the IN to drain. */
static void ota_usb_send_status(const char *s)
{
    uint32_t n = strlen(s);
    if (n > sizeof(s_usb_tx))
        n = sizeof(s_usb_tx);
    memcpy(s_usb_tx, s, n);

    s_tx_busy = true;
    usbd_ep_start_write(CDC_IN_EP, s_usb_tx, n);

    TickType_t deadline = xTaskGetTickCount() + pdMS_TO_TICKS(1000);
    while (s_tx_busy) {
        if ((int32_t)(xTaskGetTickCount() - deadline) >= 0)
            break;
        taskYIELD();
    }
}

int cmd_ota_usb(int argc, char **argv)
{
    uint32_t fw_b_addr = 0, fw_b_max = 0, total = 0;
    char status[40];
    bool reboot = false;
    bool usb_up = false;
    int rc = -1;

    /* `ota_usb noapply`: receive + validate only, no activeIndex flip / reboot. */
    bool noapply = (argc >= 2 && strcmp(argv[1], "noapply") == 0);

    if (ota_get_fw_b(&fw_b_addr, &fw_b_max) != 0) {
        printf("ota_usb: no valid partition table / FW entry\r\n");
        return -1;
    }

    printf("ota_usb: erasing FW-B @ 0x%lx (max 0x%lx)...\r\n",
           (unsigned long)fw_b_addr, (unsigned long)fw_b_max);
    if (flash_erase(fw_b_addr, fw_b_max) != SUCCESS) {
        printf("ota_usb: FW-B erase failed\r\n");
        return -1;
    }

    if (ota_sink_begin(fw_b_addr, fw_b_max) != 0) {
        printf("ota_usb: out of memory\r\n");
        goto cleanup;
    }

    ota_usb_cdc_up();
    usb_up = true;
    printf("ota_usb: USB CDC up -- send the .ota with tools/ota_usb_send.py now "
           "(%ds timeout)...\r\n", OTA_USB_TIMEOUT_MS / 1000);

    /* Receive until the self-describing total (512 + payload_len) is reached. */
    {
        TickType_t deadline = xTaskGetTickCount() + pdMS_TO_TICKS(OTA_USB_TIMEOUT_MS);
        rc = -2;  /* timeout unless we complete */
        while (total == 0 || ota_recv_len() < total) {
            if (s_rx_ready) {
                uint32_t n = s_rx_len;
                s_rx_ready = false;
                if (ota_sink(s_usb_buf, n) != 0) {
                    rc = -4;
                    break;
                }
                if (total == 0)
                    total = ota_total_len();   /* header seen -> know the size */
                if (total == 0 || ota_recv_len() < total) {
                    uint32_t want = OTA_USB_BUF;
                    if (total) {
                        uint32_t left = total - ota_recv_len();
                        if (left < want)        /* size last read exactly -> no ZLP needed */
                            want = left;
                    }
                    usbd_ep_start_read(CDC_OUT_EP, s_usb_buf, want);
                }
                deadline = xTaskGetTickCount() + pdMS_TO_TICKS(OTA_USB_TIMEOUT_MS);
            } else {
                if ((int32_t)(xTaskGetTickCount() - deadline) >= 0)
                    break;       /* idle timeout -> rc stays -2 */
                taskYIELD();
            }
        }
        if (total != 0 && ota_recv_len() >= total)
            rc = ota_sink_flush();
    }

    if (rc == 0)
        rc = ota_validate();

    /* Tell the host pass/fail, then drop USB before we reboot. */
    if (rc == 0) {
        ota_usb_send_status("OTA OK\r\n");
    } else {
        snprintf(status, sizeof(status), "OTA FAIL rc=%d\r\n", rc);
        ota_usb_send_status(status);
    }

    /* Let the host drain the status line before the CDC port vanishes
     * (usbd_deinitialize + reboot would otherwise tear it down too fast). */
    vTaskDelay(pdMS_TO_TICKS(500));

    usbd_deinitialize();
    usb_up = false;

    if (rc != 0) {
        printf("ota_usb: transfer/validate failed (rc=%d%s) -- FW-A untouched\r\n",
               rc, ota_overflowed() ? ", package > FW-B slot" : "");
        goto cleanup;
    }

    printf("ota_usb: received %lu bytes, package OK (SHA verified).\r\n",
           (unsigned long)ota_recv_len());

    if (noapply) {
        printf("ota_usb: noapply -- FW-B staged, activeIndex NOT flipped\r\n");
        goto cleanup;
    }

    if (ota_mark_fwb_active() != 0) {
        printf("ota_usb: partition activeIndex update failed\r\n");
        rc = -1;
        goto cleanup;
    }
    reboot = true;

cleanup:
    if (usb_up)
        usbd_deinitialize();
    ota_sink_end();

    if (reboot) {
        printf("ota_usb: rebooting to apply OTA...\r\n");
        hal_reboot();
    }
    return rc;
}
SHELL_CMD_EXPORT_ALIAS(cmd_ota_usb, ota_usb,
                       receive OTA package via USB CDC and apply);

#endif /* CONFIG_COMPONENTS_CHERRYUSB_ENABLE */
