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
 * IPRO6 USB device throughput test class (vendor, bInterfaceClass=0xFF).
 *
 * Measures the max sustained throughput of the ipro6 DUT acting as a USB 2.0
 * High-Speed device against a Windows WinUSB host, for BULK and ISOCHRONOUS
 * transfer, both directions:
 *   device-IN  (DUT -> host) : the DUT keeps an IN  transfer armed (usbd_ep_start_write)
 *   device-OUT (host -> DUT) : the DUT keeps an OUT transfer armed (usbd_ep_start_read)
 *
 * Only 4 data FIFOs exist and a 1024-byte isoc endpoint consumes a FIFO pair,
 * so BULK and ISOC cannot coexist. The two modes are swapped by re-enumeration
 * ("usb_tp init bulk" / "usb_tp init isoc"), each registering its own descriptor.
 *
 * The DCD is single-outstanding-per-endpoint (no queue), so the only zero-gap
 * way to saturate the pipe is to resubmit from inside the completion callback
 * (ISR context) -- never busy-wait from a task. Counters are updated in the cb.
 *
 * The host drives a run via the vendor control requests below; the "usb_tp"
 * shell command offers the same control manually. Host program: the matching
 * tools/usb_dl_tests/winusb_tput.c -- keep the VID/PID, endpoints and the
 * vendor request layout below in sync with it.
 */
#include <generated/autoconf.h>
#include "ipro_vendor_ids.h"
#include "shell.h"
#include "usbd_core.h"
#include "usbd_event_common.h"
#include "FreeRTOS.h"
#include "task.h"

/*!< endpoint address (must match winusb_tput.c) */
#define TP_IN_EP  0x81
#define TP_OUT_EP 0x02

#define USBD_VID           IPRO_USB_VID
#define USBD_PID           0x06C0
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

/* HS packet sizes. Bulk is capped at 512 by USB 2.0; isoc uses a 1024-byte
 * block. TP_ISOC_MULT selects high-bandwidth isoc (additional transactions per
 * microframe): 0 = 1x (Tier 1, no DCD patch), 1 = 2x, 2 = 3x (Tier 2, needs the
 * drv_usb_cherryusb.c blockType patch + PHY support). */
#define TP_BULK_MPS  512
#define TP_ISOC_MPS  1024
/* 0 = Tier-1 (1024 B/microframe, single transaction): the working default.
 * 2 = Tier-2 high-bandwidth (3072 B/microframe): the device configures, but
 *     Windows/WinUSB does NOT expose a usable interface for the 3x endpoint on
 *     this bench (tested 2026-06-25), so Tier-2 is a dead end here. */
#define TP_ISOC_MULT 0

/* wMaxPacketSize field for the isoc endpoints, with the mult bits (12:11). */
#define TP_ISOC_WMPS (TP_ISOC_MPS | ((TP_ISOC_MULT) << 11))

/* One transfer's worth of data. 49152 = 96*512 = 48*1024: a clean multiple of
 * both the bulk and isoc packet sizes, inside the vDMA 17-bit length cap (max
 * 131071), and small enough that two of them (IN + OUT) fit in ram_tcm. The
 * host streams full 512-multiple transfers, so a device-OUT transfer never ends
 * on a short packet regardless of how the two sides' transfer sizes line up. */
#define TP_BURST 49152

/* vendor bRequest codes (must match winusb_tput.c) */
#define TP_REQ_START     0x01 /* host->dev, wValue bit0=IN, bit1=OUT, zero counters + arm */
#define TP_REQ_STOP      0x02 /* host->dev, stop streaming */
#define TP_REQ_GET_STATS 0x03 /* dev->host, 24-byte packed stats */

/* config descriptor total length: config(9) + interface(9) + 2*endpoint(7) */
#define TP_CONFIG_SIZE (9 + 9 + 7 + 7)

/* The shared string tail (langid + 3 strings + optional HS qualifier). The two
 * mode descriptors differ only in their endpoint blocks. */
#define TP_STRING_DESCRIPTORS                                                                    \
    USB_LANGID_INIT(USBD_LANGID_STRING),                                                         \
    0x0a, USB_DESCRIPTOR_TYPE_STRING, 'I', 0x00, 'P', 0x00, 'R', 0x00, 'O', 0x00, /* string1 */  \
    0x12, USB_DESCRIPTOR_TYPE_STRING, 'I', 0x00, 'P', 0x00, 'R', 0x00, 'O', 0x00, '6', 0x00,     \
    ' ', 0x00, 'T', 0x00, 'P', 0x00,                                             /* string2 */   \
    0x0a, USB_DESCRIPTOR_TYPE_STRING, '0', 0x00, '0', 0x00, '0', 0x00, '1', 0x00  /* string3 */

#ifdef CONFIG_USB_HS
#define TP_QUALIFIER_DESCRIPTOR \
    ,                           \
    0x0a, USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER, 0x00, 0x02, 0xff, 0x00, 0x00, 0x40, 0x01, 0x00
#else
#define TP_QUALIFIER_DESCRIPTOR
#endif

/* BULK mode descriptor: two 512-byte bulk endpoints. */
static const uint8_t tp_descriptor_bulk[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xff, 0x00, 0x00, USBD_VID, USBD_PID, 0x0100, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(TP_CONFIG_SIZE, 0x01, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    USB_INTERFACE_DESCRIPTOR_INIT(0x00, 0x00, 0x02, 0xff, 0x00, 0x00, 0x00),
    USB_ENDPOINT_DESCRIPTOR_INIT(TP_IN_EP, USB_ENDPOINT_TYPE_BULK, TP_BULK_MPS, 0x00),
    USB_ENDPOINT_DESCRIPTOR_INIT(TP_OUT_EP, USB_ENDPOINT_TYPE_BULK, TP_BULK_MPS, 0x00),
    TP_STRING_DESCRIPTORS
    TP_QUALIFIER_DESCRIPTOR,
    0x00
};

/* ISOC mode descriptor: two isoc endpoints, one transaction set per microframe. */
static const uint8_t tp_descriptor_isoc[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xff, 0x00, 0x00, USBD_VID, USBD_PID, 0x0100, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(TP_CONFIG_SIZE, 0x01, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    USB_INTERFACE_DESCRIPTOR_INIT(0x00, 0x00, 0x02, 0xff, 0x00, 0x00, 0x00),
    USB_ENDPOINT_DESCRIPTOR_INIT(TP_IN_EP, USB_ENDPOINT_TYPE_ISOCHRONOUS, TP_ISOC_WMPS, 0x01),
    USB_ENDPOINT_DESCRIPTOR_INIT(TP_OUT_EP, USB_ENDPOINT_TYPE_ISOCHRONOUS, TP_ISOC_WMPS, 0x01),
    TP_STRING_DESCRIPTORS
    TP_QUALIFIER_DESCRIPTOR,
    0x00
};

/* vDMA can only touch DMA-coherent (non-cacheable), aligned memory. Separate IN
 * and OUT buffers: in "both" mode device-IN (ep1/F0) and device-OUT (ep2/F1) are
 * two independent, concurrent vDMA channels, so they must not alias one region.
 * Payload content is irrelevant to throughput; the split just keeps two live DMA
 * engines off the same memory. */
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX static uint8_t tp_in_buf[TP_BURST];
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX static uint8_t tp_out_buf[TP_BURST];
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX static uint8_t tp_stats_buf[24];

static volatile bool     tp_in_run;
static volatile bool     tp_out_run;
static volatile bool     tp_configured;
static volatile uint64_t tp_tx_bytes;
static volatile uint64_t tp_rx_bytes;
static volatile uint32_t tp_start_tick;
static uint8_t           tp_mode; /* 0 = bulk, 1 = isoc */

/* IN completion (ISR): account the bytes, then immediately re-arm to keep the
 * pipe full. Single-outstanding DCD => this cb is the only zero-gap restart. */
static void tp_in_cb(uint8_t ep, uint32_t nbytes)
{
    (void)ep;
    tp_tx_bytes += nbytes;
    if (tp_in_run) {
        usbd_ep_start_write(TP_IN_EP, tp_in_buf, TP_BURST);
    }
}

/* OUT completion (ISR): account the bytes, then re-arm the next read. */
static void tp_out_cb(uint8_t ep, uint32_t nbytes)
{
    (void)ep;
    tp_rx_bytes += nbytes;
    if (tp_out_run) {
        usbd_ep_start_read(TP_OUT_EP, tp_out_buf, TP_BURST);
    }
}

/* Arm streaming. Callable from task ctx (shell) or ISR ctx (TP_REQ_START), so
 * no taskENTER_CRITICAL here. Reject a start while already running: that is the
 * only way a transfer could already be in flight, so once we know nothing is
 * running, no completion ISR can race the initial arm below. */
static void tp_start(bool in_en, bool out_en)
{
    /* Reset counters + window only on a fresh start (nothing running yet); a
     * second call that adds the other direction (the host "both" mode arms IN
     * and OUT with separate START requests) keeps the running counters. Arming a
     * direction only when it is not already running keeps the single-outstanding
     * DCD from double-arming: nothing is in flight for a not-yet-running
     * direction, so no completion ISR can race the arm below. */
    if (!tp_in_run && !tp_out_run) {
        tp_tx_bytes = 0;
        tp_rx_bytes = 0;
        tp_start_tick = xTaskGetTickCount();
    }

    if (in_en && !tp_in_run) {
        tp_in_run = true;
        usbd_ep_start_write(TP_IN_EP, tp_in_buf, TP_BURST);
    }
    if (out_en && !tp_out_run) {
        tp_out_run = true;
        usbd_ep_start_read(TP_OUT_EP, tp_out_buf, TP_BURST);
    }
}

static void tp_stop(void)
{
    tp_in_run = false;
    tp_out_run = false;
}

/* Print "X.XX MB/s (Y Mbit/s)" for a byte count over the elapsed window using
 * integer math (no float printf in this firmware). */
static void tp_print_rate(const char *tag, uint64_t bytes, uint32_t ms)
{
    uint64_t bps;
    uint32_t mb_int, mb_frac, mbit;

    if (ms == 0) {
        ms = 1;
    }
    bps = bytes * 1000ULL / ms;            /* bytes per second */
    mb_int = (uint32_t)(bps / 1000000ULL);
    mb_frac = (uint32_t)((bps / 10000ULL) % 100ULL);
    mbit = (uint32_t)(bps * 8ULL / 1000000ULL);

    printf("  %s: %llu bytes in %lu ms = %lu.%02lu MB/s (%lu Mbit/s)\r\n",
           tag, (unsigned long long)bytes, (unsigned long)ms,
           (unsigned long)mb_int, (unsigned long)mb_frac, (unsigned long)mbit);
}

static void tp_print_stats(void)
{
    uint64_t tx, rx;
    uint32_t st, ms;

    taskENTER_CRITICAL();
    tx = tp_tx_bytes;
    rx = tp_rx_bytes;
    st = tp_start_tick;
    taskEXIT_CRITICAL();

    ms = (xTaskGetTickCount() - st) * portTICK_PERIOD_MS;
    printf("usb_tp stats (mode=%s, speed=%s):\r\n",
           tp_mode ? "isoc" : "bulk",
           usbd_get_port_speed(0) == USB_SPEED_HIGH ? "HIGH" : "NOT-HIGH");
    if (tx || tp_in_run) {
        tp_print_rate("device-IN  (DUT->host)", tx, ms);
    }
    if (rx || tp_out_run) {
        tp_print_rate("device-OUT (host->DUT)", rx, ms);
    }
}

/* Vendor control requests. Reached for any vendor-type setup packet; we only
 * claim our three bRequest codes and return -1 for anything else. */
static int tp_vendor_handler(struct usb_setup_packet *setup, uint8_t **data, uint32_t *len)
{
    switch (setup->bRequest) {
        case TP_REQ_START:
            tp_start((setup->wValue & 0x1) != 0, (setup->wValue & 0x2) != 0);
            *len = 0;
            return 0;

        case TP_REQ_STOP:
            tp_stop();
            *len = 0;
            return 0;

        case TP_REQ_GET_STATS: {
            uint32_t elapsed = (xTaskGetTickCount() - tp_start_tick) * portTICK_PERIOD_MS;
            uint32_t mode = tp_mode;

            /* packed LE: u64 tx, u64 rx, u32 elapsed_ms, u32 mode (24 bytes).
             * tp_stats_buf is non-cacheable: the control-IN data is sent by the
             * vDMA over the control FIFO. This handler runs in the USB ISR, the
             * same ISR that updates the counters, so the 64-bit reads below are
             * serialized with the writers and cannot tear. */
            memcpy(&tp_stats_buf[0], (const void *)&tp_tx_bytes, 8);
            memcpy(&tp_stats_buf[8], (const void *)&tp_rx_bytes, 8);
            memcpy(&tp_stats_buf[16], &elapsed, 4);
            memcpy(&tp_stats_buf[20], &mode, 4);
            *data = tp_stats_buf;
            *len = sizeof(tp_stats_buf);
            return 0;
        }

        default:
            return -1;
    }
}

static void tp_event_handler(uint8_t event)
{
    switch (event) {
        case USBD_EVENT_RESET:
            tp_in_run = false;
            tp_out_run = false;
            tp_configured = false;
            break;
        case USBD_EVENT_CONFIGURED:
            tp_configured = true;
            break;
        default:
            break;
    }
}

static struct usbd_interface tp_intf;
static struct usbd_endpoint tp_in_ep = { .ep_addr = TP_IN_EP, .ep_cb = tp_in_cb };
static struct usbd_endpoint tp_out_ep = { .ep_addr = TP_OUT_EP, .ep_cb = tp_out_cb };

static void tp_register(const uint8_t *desc)
{
    /* Fill the IN pattern once; a fixed pattern lets one buffer feed every IN. */
    for (uint32_t i = 0; i < TP_BURST; i++) {
        tp_in_buf[i] = (uint8_t)i;
    }

    tp_in_run = false;
    tp_out_run = false;
    tp_configured = false;

    usbd_deinitialize();
    memset(&tp_intf, 0, sizeof(tp_intf));
    tp_intf.vendor_handler = tp_vendor_handler;
    usbd_desc_register(desc);
    usbd_add_interface(&tp_intf);
    usbd_add_endpoint(&tp_in_ep);
    usbd_add_endpoint(&tp_out_ep);
    usbd_event_register_callback(tp_event_handler);
    usbd_initialize();
}

/* Wait (bounded) for the host to (re)enumerate + configure us. */
static bool tp_wait_configured(uint32_t timeout_ms)
{
    TickType_t deadline = xTaskGetTickCount() + pdMS_TO_TICKS(timeout_ms);

    while (!tp_configured) {
        if ((int32_t)(xTaskGetTickCount() - deadline) >= 0) {
            return false;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    return true;
}

static void tp_usage(void)
{
    printf("Usage:\r\n");
    printf("  usb_tp init <bulk|isoc>      re-enumerate in the chosen mode, gate on HS\r\n");
    printf("  usb_tp start <in|out|both> [secs]  arm streaming (secs: auto-stop + report)\r\n");
    printf("  usb_tp stop                  stop streaming\r\n");
    printf("  usb_tp stat                  print current counters\r\n");
}

int cmd_usb_tp(int argc, char **argv)
{
    if (argc < 2) {
        tp_usage();
        return 0;
    }

    if (strcmp(argv[1], "init") == 0) {
        uint8_t speed;

        if (argc < 3) {
            tp_usage();
            return 0;
        }
        if (strcmp(argv[2], "bulk") == 0) {
            tp_mode = 0;
            tp_register(tp_descriptor_bulk);
        } else if (strcmp(argv[2], "isoc") == 0) {
            tp_mode = 1;
            tp_register(tp_descriptor_isoc);
        } else {
            tp_usage();
            return 0;
        }

        printf("usb_tp: %s mode, waiting for host enumeration...\r\n", argv[2]);
        bool configured = tp_wait_configured(5000);

        /* The bus speed is negotiated at USB reset (HS chirp), before any
         * SET_CONFIGURATION, so it is valid even if the host has not bound a
         * driver yet (Windows leaves a bare vendor device unconfigured until
         * WinUSB is bound via Zadig). Report it regardless of config state. */
        speed = usbd_get_port_speed(0);
        printf("usb_tp: port speed = %s, configured = %s\r\n",
               speed == USB_SPEED_HIGH ? "HIGH (480Mbps)" :
               speed == USB_SPEED_FULL ? "FULL (12Mbps)" : "LOW",
               configured ? "yes" : "no (bind WinUSB via Zadig to run transfers)");
        if (speed != USB_SPEED_HIGH) {
            printf("usb_tp: *** NOT High-Speed - throughput numbers are INVALID ***\r\n");
            printf("usb_tp: check CONFIG_USB_HS=y, WIFIPLL up, and a direct HS port\r\n");
        }
        return 0;
    }

    if (strcmp(argv[1], "start") == 0) {
        bool in_en, out_en;
        int secs = 0;

        if (argc < 3) {
            tp_usage();
            return 0;
        }
        if (strcmp(argv[2], "in") == 0) {
            in_en = true;
            out_en = false;
        } else if (strcmp(argv[2], "out") == 0) {
            in_en = false;
            out_en = true;
        } else if (strcmp(argv[2], "both") == 0) {
            in_en = true;
            out_en = true;
        } else {
            tp_usage();
            return 0;
        }

        if (argc >= 4) {
            secs = atoi(argv[3]);
        }

        printf("usb_tp: start %s%s\r\n", in_en ? "IN " : "", out_en ? "OUT" : "");
        tp_start(in_en, out_en);

        if (secs > 0) {
            vTaskDelay(pdMS_TO_TICKS((uint32_t)secs * 1000U));
            tp_stop();
            tp_print_stats();
        }
        return 0;
    }

    if (strcmp(argv[1], "stop") == 0) {
        tp_stop();
        tp_print_stats();
        return 0;
    }

    if (strcmp(argv[1], "stat") == 0) {
        tp_print_stats();
        return 0;
    }

    tp_usage();
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_usb_tp, usb_tp, USB throughput test: init/start/stop/stat);
