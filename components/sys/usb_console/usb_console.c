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
 * USB CDC-ACM debug console -- see usb_console.h.
 *
 * One CDC-ACM whose IN endpoint carries the firmware printf/log stream and
 * whose OUT endpoint feeds the FinSH shell. Distilled from the proven
 * apps/turnkey/common/usb_console.c (ring + drain task) and the bench-tested
 * apps/platform/common/ota/ota_usb.c (static descriptor + CDC bring-up).
 *
 * Output: debug.c funnels printf/puts/putchar -> usb_console_write() -> a
 * drop-oldest ring (never blocks) -> console_tx_task -> CDC IN endpoint.
 * Input:  CDC OUT cb -> shell_exe_cmd() (when CONFIG_SHELL).
 *
 * Mirror mode: this does NOT replace the UART path -- debug.c still writes the
 * UART too. The UART already expands LF->CRLF per byte; this module does the
 * same expansion on its ring so a USB terminal sees identical line endings.
 *
 * The single usbd_event_handler dispatcher is provided via the register-shim
 * (usb_console_event.h): this module registers a callback rather than owning
 * the handler symbol, so it composes with an app's existing usb_demo / OTA CDC.
 */
#include <generated/autoconf.h>

#if defined(CONFIG_DEBUG_CONSOLE_USB)

#include "usb_console.h"
#include "usb_console_event.h"

#include "usbd_core.h"
#include "usbd_cdc.h"

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* ---- USB identity (overridable per app via -D in CMakeLists) ---- */
#ifndef USB_CONSOLE_VID
#define USB_CONSOLE_VID 0x3896
#endif
#ifndef USB_CONSOLE_PID
#define USB_CONSOLE_PID 0x06A1          /* distinct from the OTA CDC (0x06A0) */
#endif

/* ---- Endpoints ----
 * 獨立裝置模式:interfaces 0/1,EP 0x81/0x02/0x83。
 * attach 模式(掛進 app 自己的複合裝置):app 的第一組 ACM 佔掉了
 * 0x81/0x02/0x83,console 用第二組 0x84/0x05/0x86(interfaces 2/3)。 */
#ifdef CONFIG_DEBUG_CONSOLE_USB_ATTACH
#define CDC_IN_EP        0x84
#define CDC_OUT_EP       0x05
#define CDC_INT_EP       0x86
#else
#define CDC_IN_EP        0x81
#define CDC_OUT_EP       0x02
#define CDC_INT_EP       0x83
#endif

#define USB_CONFIG_SIZE  (9 + CDC_ACM_DESCRIPTOR_LEN)

#ifdef CONFIG_USB_HS
#define CDC_MAX_MPS      512
#else
#define CDC_MAX_MPS      64
#endif

/* TX ring -- text only. Drops oldest on overflow so the firmware never blocks
 * when the host has the console port closed. Power-of-two for masked wrap. */
#define CONSOLE_RING_BYTES (4u * 1024u)
#define TX_CHUNK_BYTES     512u

_Static_assert((CONSOLE_RING_BYTES & (CONSOLE_RING_BYTES - 1)) == 0,
               "CONSOLE_RING_BYTES must be power of 2");

/* ---- Static device/config/CDC descriptor (ASCII strings inline, like ota_usb).
 * String byte0 = 2 + 2*nchars. ---- */
static const uint8_t s_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xEF, 0x02, 0x01,
                               USB_CONSOLE_VID, USB_CONSOLE_PID, 0x0100, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x02, 0x01,
                               USB_CONFIG_BUS_POWERED, 100),
    CDC_ACM_DESCRIPTOR_INIT(0x00, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP,
                            CDC_MAX_MPS, 0x02),
    USB_LANGID_INIT(1033),
    0x0a, USB_DESCRIPTOR_TYPE_STRING,                  /* iManufacturer: "IPRO" */
    'I', 0, 'P', 0, 'R', 0, 'O', 0,
    0x1a, USB_DESCRIPTOR_TYPE_STRING,                  /* iProduct: "IPRO Console" */
    'I', 0, 'P', 0, 'R', 0, 'O', 0, ' ', 0, 'C', 0, 'o', 0, 'n', 0,
    's', 0, 'o', 0, 'l', 0, 'e', 0,
    0x10, USB_DESCRIPTOR_TYPE_STRING,                  /* iSerial: "CON0001" */
    'C', 0, 'O', 0, 'N', 0, '0', 0, '0', 0, '0', 0, '1', 0,
#ifdef CONFIG_USB_HS
    0x0a, USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER, 0x00, 0x02, 0x02, 0x02, 0x01, 0x40, 0x01, 0x00,
#endif
    0x00
};

static volatile bool s_configured;

/* USB DMA can only touch non-cacheable memory. */
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX static uint8_t s_tx_buf[TX_CHUNK_BYTES];
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX static uint8_t s_cdc_rx_buf[CDC_MAX_MPS];

/* Ring is plain memory -- it is memcpy'd into the nocache s_tx_buf, never DMA'd. */
static uint8_t           s_ring[CONSOLE_RING_BYTES];
static volatile uint32_t s_head;
static volatile uint32_t s_tail;
static uint8_t           s_last;        /* last raw byte, for LF->CRLF across calls */

static volatile bool     s_ready;       /* init done -> ring may be published to */
static SemaphoreHandle_t s_sig;         /* data-available signal to drain task */
static StaticSemaphore_t s_sig_buf;
static SemaphoreHandle_t s_tx_done;     /* IN-completion signal */
static StaticSemaphore_t s_tx_done_buf;

static StackType_t  s_stack[1024];
static StaticTask_t s_tcb;

/* ---- ring primitives ---- */
static inline uint32_t ring_used(void)
{
    return (s_head - s_tail) & (CONSOLE_RING_BYTES - 1);
}
static inline uint32_t ring_free(void)
{
    return (CONSOLE_RING_BYTES - 1) - ring_used();
}
static inline void ring_push(uint8_t b)
{
    if (ring_free() == 0)                                       /* drop oldest */
        s_tail = (s_tail + 1) & (CONSOLE_RING_BYTES - 1);
    s_ring[s_head] = b;
    s_head = (s_head + 1) & (CONSOLE_RING_BYTES - 1);
}

uint32_t usb_console_write(const void *buf, uint32_t len)
{
    const uint8_t *data = (const uint8_t *)buf;
    if (!data || len == 0) return 0;
    if (!s_ready) return 0;                                    /* pre-init: drop */

    /* Protect s_ring/head/tail with a short interrupt-masking critical section
     * -- correct from BOTH task and ISR (printf-in-ISR), unlike a FreeRTOS mutex
     * which must not be taken from an ISR. The masked section cannot nest. */
    UBaseType_t key = portSET_INTERRUPT_MASK_FROM_ISR();
    uint8_t prev = s_last;
    for (uint32_t i = 0; i < len; i++) {                       /* LF -> CRLF */
        uint8_t b = data[i];
        if (b == '\n' && prev != '\r')
            ring_push('\r');
        ring_push(b);
        prev = b;
    }
    s_last = prev;
    portCLEAR_INTERRUPT_MASK_FROM_ISR(key);

    /* Wake the drain task -- outside the critical section. */
    if (s_sig) {
        if (xPortIsInsideInterrupt()) {
            BaseType_t woken = pdFALSE;
            xSemaphoreGiveFromISR(s_sig, &woken);
            portYIELD_FROM_ISR(woken);
        } else {
            xSemaphoreGive(s_sig);
        }
    }
    return len;
}

/* Copy up to want contiguous bytes out of the ring into dst; returns count. */
static uint32_t ring_pop(uint8_t *dst, uint32_t want)
{
    UBaseType_t key = portSET_INTERRUPT_MASK_FROM_ISR();       /* drain task ctx */
    uint32_t avail  = ring_used();
    uint32_t to_end = CONSOLE_RING_BYTES - s_tail;
    uint32_t take   = avail < want ? avail : want;
    if (take > to_end) take = to_end;                          /* one wrap-safe run */
    memcpy(dst, &s_ring[s_tail], take);
    s_tail = (s_tail + take) & (CONSOLE_RING_BYTES - 1);
    portCLEAR_INTERRUPT_MASK_FROM_ISR(key);
    return take;
}

static void console_tx_task(void *pv)
{
    (void)pv;
    while (1) {
        if (!s_configured) {
            vTaskDelay(pdMS_TO_TICKS(50));
            continue;
        }
        uint32_t n = ring_pop(s_tx_buf, TX_CHUNK_BYTES);
        if (n == 0) {
            xSemaphoreTake(s_sig, pdMS_TO_TICKS(100));
            continue;
        }
        if (usbd_ep_start_write(CDC_IN_EP, s_tx_buf, n) < 0) {
            vTaskDelay(pdMS_TO_TICKS(10));                     /* host port not open */
            continue;
        }
        /* Short timeout so we re-check s_configured and never hard-block. */
        (void)xSemaphoreTake(s_tx_done, pdMS_TO_TICKS(500));
    }
}

/* ---- USB endpoint callbacks ---- */
static void cdc_in_cb(uint8_t ep, uint32_t nbytes)
{
    (void)ep; (void)nbytes;
    BaseType_t woken = pdFALSE;
    xSemaphoreGiveFromISR(s_tx_done, &woken);
    portYIELD_FROM_ISR(woken);
}

#ifdef CONFIG_SHELL
extern void shell_exe_cmd(uint8_t *cmd, uint16_t len);
#endif

static void cdc_out_cb(uint8_t ep, uint32_t nbytes)
{
#ifdef CONFIG_SHELL
    if (nbytes > 0)
        shell_exe_cmd(s_cdc_rx_buf, (uint16_t)nbytes);
#else
    (void)nbytes;                                             /* no shell -> drop input */
#endif
    usbd_ep_start_read(ep, s_cdc_rx_buf, sizeof(s_cdc_rx_buf));
}

static void usb_console_event_cb(uint8_t event)
{
    switch (event) {
    case USBD_EVENT_CONFIGURED:
        s_configured = true;
        usbd_ep_start_read(CDC_OUT_EP, s_cdc_rx_buf, sizeof(s_cdc_rx_buf));
        break;
    case USBD_EVENT_RESET:
        s_configured = false;
        break;
    default:
        break;
    }
}

bool usb_console_is_connected(void) { return s_configured; }

/* ---- Init ---- */
static struct usbd_interface s_intf0, s_intf1;
static struct usbd_endpoint s_ep_out = { .ep_addr = CDC_OUT_EP, .ep_cb = cdc_out_cb };
static struct usbd_endpoint s_ep_in  = { .ep_addr = CDC_IN_EP,  .ep_cb = cdc_in_cb  };

static int console_core_init(void)
{
    s_sig     = xSemaphoreCreateBinaryStatic(&s_sig_buf);
    s_tx_done = xSemaphoreCreateBinaryStatic(&s_tx_done_buf);
    if (!s_sig || !s_tx_done)
        return -1;

    xTaskCreateStatic(console_tx_task, "usb_con",
                      sizeof(s_stack) / sizeof(StackType_t),
                      NULL, 2, s_stack, &s_tcb);

    s_ready = true;   /* ring + signal up -> usb_console_write may publish now */
    return 0;
}

int usb_console_init(void)
{
    if (console_core_init() != 0)
        return -1;
    usbd_desc_register(s_descriptor);
    usbd_add_interface(usbd_cdc_acm_init_intf(&s_intf0));
    usbd_add_interface(usbd_cdc_acm_init_intf(&s_intf1));
    usbd_add_endpoint(&s_ep_out);
    usbd_add_endpoint(&s_ep_in);
    usbd_event_register_callback(usb_console_event_cb);
    usbd_initialize();
    return 0;
}

#ifdef CONFIG_DEBUG_CONSOLE_USB_ATTACH
/* 掛進 app 已經擁有的 USB 裝置:只加自己的 interfaces/endpoints,
 * descriptor 與 usbd_initialize() 由 app 負責(合成的描述元裡要含第二組
 * ACM,interface 編號接在 app 的後面)。事件由 app 的 dispatcher 轉呼
 * usb_console_notify_event()。 */
int usb_console_attach(void)
{
    if (console_core_init() != 0)
        return -1;
    usbd_add_interface(usbd_cdc_acm_init_intf(&s_intf0));
    usbd_add_interface(usbd_cdc_acm_init_intf(&s_intf1));
    usbd_add_endpoint(&s_ep_out);
    usbd_add_endpoint(&s_ep_in);
    return 0;
}

void usb_console_notify_event(uint8_t event)
{
    usb_console_event_cb(event);
}
#endif /* CONFIG_DEBUG_CONSOLE_USB_ATTACH */

#else  /* CONFIG_DEBUG_CONSOLE_USB unset -- no-op stubs so callers compile cleanly. */

#include "usb_console.h"
int      usb_console_init(void)                       { return 0; }
uint32_t usb_console_write(const void *b, uint32_t l) { (void)b; (void)l; return 0; }
bool     usb_console_is_connected(void)               { return false; }

#endif /* CONFIG_DEBUG_CONSOLE_USB */
