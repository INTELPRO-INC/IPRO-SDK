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
 * @file usb_console.c
 * @brief Single-CDC-ACM USB console — see usb_console.h.
 *
 * Distilled from a dual-CDC streamer, keeping only the console half: one CDC-ACM whose IN endpoint
 * carries printf/log output and whose OUT endpoint feeds the SDK shell.
 *
 * Output path: __wrap_printf/puts/putchar -> console ring (drop-oldest on
 * overflow, never blocks) -> console_tx_task -> CDC IN endpoint.
 * Input path: CDC OUT cb -> shell_exe_cmd (when CONFIG_SHELL).
 *
 * The 1200-baud bootloader touch is handled by components/boot/uboot_trigger
 * (it owns usbd_cdc_acm_set_line_coding / set_dtr) — not here.
 *
 * Attach timing: usb_console_init() DETACHES the device (USB off, D+/D-
 * floating) and the console task attaches it once the scheduler runs.
 *  - The host must see a real detach first. After a warm reset (reboot, boot2
 *    BOOT) the cable stays attached and the board pinmux has just parked D-
 *    with a pull-up (ipro7_arduino: JTAG TDO on GPIO9), a phantom low-speed
 *    attach; a full-speed attach straight over it was never enumerated, so a
 *    battery-powered tag lost its console until the cable was re-plugged.
 *  - Apps call init from main(), and from the first critical section until
 *    vTaskStartScheduler() the USB IRQ is masked: a device attached there
 *    cannot answer the host for the rest of the pre-scheduler bring-up.
 * If the host still has not configured us ATTACH_TIMEOUT_MS after an attach,
 * the task detaches and attaches again (bounded by ATTACH_RETRIES).
 */

#include <generated/autoconf.h>

#if defined(CONFIG_COMPONENTS_CHERRYUSB_ENABLE) && defined(CONFIG_CHERRYUSB_DEVICE_CDC)

#include "usb_console.h"
#include "device_serial.h"
#include "ipro_log.h"

#include "usbd_core.h"
#include "usbd_cdc.h"
#include "usb_dc.h"
#include "hal_mtimer.h"
#include "system_soc.h"     /* CPU_Interrupt_Disable/Enable, USB_IRQn */

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define LOG_TAG "USB_CON"

/* ---- USB identity (overridable per app via -D in CMakeLists) ---- */
#ifndef USB_CONSOLE_VID
#define USB_CONSOLE_VID 0x1209
#endif
#ifndef USB_CONSOLE_PID
#define USB_CONSOLE_PID 0x7010
#endif
#ifndef USB_CONSOLE_PRODUCT
#define USB_CONSOLE_PRODUCT "IPRO USB Console"
#endif
#ifndef USB_CONSOLE_SERIAL
#define USB_CONSOLE_SERIAL "IPRO-0001"
#endif

/* ---- Endpoints (single CDC-ACM, interfaces 0/1) ---- */
#define CDC_IN_EP        0x81
#define CDC_OUT_EP       0x02
#define CDC_INT_EP       0x83

#define USBD_MAX_POWER   100
#define USBD_LANGID      1033
#define USB_CONFIG_SIZE  (9 + CDC_ACM_DESCRIPTOR_LEN)

#ifdef CONFIG_USB_HS
#define CDC_MAX_MPS      512
#else
#define CDC_MAX_MPS      64
#endif

/* Console TX ring — text only. Drops oldest bytes on overflow so the
 * firmware never blocks when the host has the console port closed. */
#define CONSOLE_RING_BYTES (4u * 1024u)
#define TX_CHUNK_BYTES     512u

/* Not configured this long after attaching -> detach and attach again, at
 * most ATTACH_RETRIES times, and only before the first configuration (later
 * unconfigured spells are host resets / unplugs). A dumb charger never
 * configures; it just sees the retries and then a device left attached. */
#define ATTACH_TIMEOUT_MS  4000u
#define ATTACH_RETRIES     2u
#define DETACH_MS          300u

/* ---- String-descriptor builders (ASCII macro -> UTF-16LE at compile
 * time is awkward; build these descriptors once at init instead). ---- */
#define STR_DESC_MAX 64

static volatile bool s_configured;

/* Attach bookkeeping (mtimer ms), reported once the host configures us. */
static uint32_t          s_init_ms;     /* usb_console_init(): detached   */
static uint32_t          s_task_ms;     /* console task start (scheduler) */
static uint32_t          s_attach_ms;   /* last attach                    */
static volatile uint32_t s_cfg_ms;      /* first USBD_EVENT_CONFIGURED    */
static uint8_t           s_attempt;     /* attaches so far                */

static void usb_console_attach(void);

static uint8_t s_cdc_rx_buf[CDC_MAX_MPS];

static uint8_t           s_console_ring[CONSOLE_RING_BYTES];
static volatile uint32_t s_console_head;
static volatile uint32_t s_console_tail;
static SemaphoreHandle_t s_console_lock;
static StaticSemaphore_t s_console_lock_buf;
static SemaphoreHandle_t s_console_sig;
static StaticSemaphore_t s_console_sig_buf;

static SemaphoreHandle_t s_tx_done;
static StaticSemaphore_t s_tx_done_buf;

static StackType_t  s_console_stack[1024];
static StaticTask_t s_console_tcb;

/* ---- Descriptor. Built at init so the product/serial strings can come
 * from plain ASCII macros (converted to UTF-16LE here). ---- */
#define DESC_FIXED_LEN  (18 + 9 + CDC_ACM_DESCRIPTOR_LEN + 4 /* langid */)
static uint8_t s_descriptor[DESC_FIXED_LEN + 4 * STR_DESC_MAX];

/* Append a USB string descriptor (ASCII -> UTF-16LE) at *pos; returns new pos. */
static uint32_t desc_put_string(uint8_t *d, uint32_t pos, const char *s)
{
    uint32_t n = (uint32_t)strlen(s);
    if (n > (STR_DESC_MAX - 2) / 2) n = (STR_DESC_MAX - 2) / 2;
    d[pos++] = (uint8_t)(2 + n * 2);
    d[pos++] = USB_DESCRIPTOR_TYPE_STRING;
    for (uint32_t i = 0; i < n; i++) {
        d[pos++] = (uint8_t)s[i];
        d[pos++] = 0x00;
    }
    return pos;
}

static void build_descriptor(void)
{
    const uint8_t dev_hdr[] = {
        USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xEF, 0x02, 0x01,
                                   USB_CONSOLE_VID, USB_CONSOLE_PID, 0x0100, 0x01),
        USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x02, 0x01,
                                   USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
        CDC_ACM_DESCRIPTOR_INIT(0x00, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP,
                                CDC_MAX_MPS, 0x02),
    };
    uint32_t pos = 0;
    memcpy(s_descriptor, dev_hdr, sizeof(dev_hdr));
    pos = sizeof(dev_hdr);

    /* String 0 — LANGID (US English). */
    s_descriptor[pos++] = 0x04;
    s_descriptor[pos++] = USB_DESCRIPTOR_TYPE_STRING;
    s_descriptor[pos++] = (uint8_t)(USBD_LANGID & 0xFF);
    s_descriptor[pos++] = (uint8_t)(USBD_LANGID >> 8);

    /* Per-device UNIQUE serial (hex of the device serial) so multiple boards
     * with identical VID/PID enumerate as distinct ports on one host. */
    uint8_t sn[8];
    char sn_str[17];
    device_serial_get(sn);
    for (int i = 0; i < 8; i++) snprintf(&sn_str[i * 2], 3, "%02x", sn[i]);

    pos = desc_put_string(s_descriptor, pos, "IPRO");                 /* iManufacturer (1) */
    pos = desc_put_string(s_descriptor, pos, USB_CONSOLE_PRODUCT);    /* iProduct (2)      */
    pos = desc_put_string(s_descriptor, pos, sn_str);                 /* iSerial (3)       */

    s_descriptor[pos] = 0x00;   /* descriptor list terminator */
}

/* ---- Console ring primitives ---- */
_Static_assert((CONSOLE_RING_BYTES & (CONSOLE_RING_BYTES - 1)) == 0,
               "CONSOLE_RING_BYTES must be power of 2");

static inline uint32_t console_used(void)
{
    return (s_console_head - s_console_tail) & (CONSOLE_RING_BYTES - 1);
}
static inline uint32_t console_free(void)
{
    return (CONSOLE_RING_BYTES - 1) - console_used();
}

/* Push one byte, dropping the oldest on overflow (keeps newest log visible). */
static inline void console_ring_push(uint8_t b)
{
    if (console_free() == 0) {
        s_console_tail = (s_console_tail + 1) & (CONSOLE_RING_BYTES - 1);
    }
    s_console_ring[s_console_head] = b;
    s_console_head = (s_console_head + 1) & (CONSOLE_RING_BYTES - 1);
}

/* Last byte emitted, tracked across calls so a bare '\n' at the very start of
 * the next write still gets its '\r'. */
static uint8_t s_console_last;

static uint32_t console_puts(const uint8_t *data, uint32_t len)
{
    if (!data || len == 0) return 0;
    if (s_console_lock == NULL) return 0;   /* pre-init — drop */

    BaseType_t in_isr = xPortIsInsideInterrupt();
    if (in_isr) {
        BaseType_t woken = pdFALSE;
        if (xSemaphoreTakeFromISR(s_console_lock, &woken) != pdTRUE) return 0;
    } else {
        xSemaphoreTake(s_console_lock, portMAX_DELAY);
    }

    /* LF -> CRLF: OpenThread (and other) logs terminate lines with a bare
     * '\n'; a raw serial terminal then steps right each line (staircase).
     * Inject '\r' before any '\n' not already preceded by one. */
    uint8_t prev = s_console_last;
    for (uint32_t i = 0; i < len; i++) {
        uint8_t b = data[i];
        if (b == '\n' && prev != '\r') {
            console_ring_push('\r');
        }
        console_ring_push(b);
        prev = b;
    }
    s_console_last = prev;

    if (in_isr) {
        BaseType_t woken = pdFALSE;
        xSemaphoreGiveFromISR(s_console_lock, &woken);
        if (s_console_sig) xSemaphoreGiveFromISR(s_console_sig, &woken);
        portYIELD_FROM_ISR(woken);
    } else {
        xSemaphoreGive(s_console_lock);
        if (s_console_sig) xSemaphoreGive(s_console_sig);
    }
    return len;
}

static uint32_t console_peek(uint32_t want, uint32_t *out_len)
{
    xSemaphoreTake(s_console_lock, portMAX_DELAY);
    uint32_t avail  = console_used();
    uint32_t to_end = CONSOLE_RING_BYTES - s_console_tail;
    uint32_t take   = avail < want ? avail : want;
    if (take > to_end) take = to_end;
    *out_len = take;
    uint32_t off = s_console_tail;
    xSemaphoreGive(s_console_lock);
    return off;
}

static void console_advance(uint32_t n)
{
    xSemaphoreTake(s_console_lock, portMAX_DELAY);
    s_console_tail = (s_console_tail + n) & (CONSOLE_RING_BYTES - 1);
    xSemaphoreGive(s_console_lock);
}

/* DMA-safe TX buffer (OCRAM, aligned — must not live in PSRAM). */
static uint8_t s_tx_buf[TX_CHUNK_BYTES] __attribute__((aligned(32)));

/* cherryusb's usbd_ep_start_read/write take no lock and IPRO7 drives every
 * endpoint through one VDMA engine: this task's IN write racing the OUT
 * callback's re-arm in the USB ISR interleaves the two descriptors, and under
 * sustained traffic the endpoint wedges or DMAs stray memory (tag OTA over the
 * console stalled and hung the tag, 2026-10-20; same race as ipro7_cmsis_dap).
 * This task is the only task-context caller, so masking USB_IRQn is enough;
 * the ISR side cannot run while it is masked. */
static int ep_start_write_guarded(uint8_t ep, uint8_t *buf, uint32_t len)
{
    CPU_Interrupt_Disable(USB_IRQn);
    int rc = usbd_ep_start_write(ep, buf, len);
    CPU_Interrupt_Enable(USB_IRQn);
    return rc;
}

static void console_tx_task(void *pv)
{
    (void)pv;
    bool reported = false;

    /* The scheduler is up, so the USB IRQ can serve the host: attach now. */
    s_task_ms = (uint32_t)mtimer_get_time_ms();
    usb_console_attach();

    while (1) {
        if (!s_configured) {
            if (s_cfg_ms == 0 && s_attempt <= ATTACH_RETRIES &&
                (uint32_t)mtimer_get_time_ms() - s_attach_ms >= ATTACH_TIMEOUT_MS) {
                usbd_deinitialize();   /* pull-up off: the host sees a detach */
                vTaskDelay(pdMS_TO_TICKS(DETACH_MS));
                usb_console_attach();
            }
            vTaskDelay(pdMS_TO_TICKS(50));
            continue;
        }
        if (!reported) {
            reported = true;
            IPRO_LOGI(LOG_TAG, "configured on attach %u, %lu ms after it "
                      "(scheduler up %lu ms after init, %lu bus resets)",
                      s_attempt, (unsigned long)(s_cfg_ms - s_attach_ms),
                      (unsigned long)(s_task_ms - s_init_ms),
                      (unsigned long)g_usb_irq_stats.reset);
        }
        uint32_t chunk_len = 0;
        uint32_t off = console_peek(TX_CHUNK_BYTES, &chunk_len);
        if (chunk_len == 0) {
            xSemaphoreTake(s_console_sig, pdMS_TO_TICKS(100));
            continue;
        }
        memcpy(s_tx_buf, &s_console_ring[off], chunk_len);
        console_advance(chunk_len);

        if (ep_start_write_guarded(CDC_IN_EP, s_tx_buf, chunk_len) < 0) {
            vTaskDelay(pdMS_TO_TICKS(10));   /* host hasn't opened the port */
            continue;
        }
        /* Short timeout so we re-check s_configured and never hard-block. */
        (void)xSemaphoreTake(s_tx_done, pdMS_TO_TICKS(500));
    }
}

/* ---- newlib printf / puts / putchar wrap -> console ring ---- */
#define PRINTF_WRAP_BUF 256

int __wrap_printf(const char *fmt, ...)
{
    char buf[PRINTF_WRAP_BUF];
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    if (n <= 0) return n;
    if ((size_t)n > sizeof(buf)) n = (int)sizeof(buf);
    console_puts((const uint8_t *)buf, (uint32_t)n);
    return n;
}

int __wrap_putchar(int c)
{
    uint8_t b = (uint8_t)c;
    console_puts(&b, 1);
    return c;
}

int __wrap_puts(const char *s)
{
    if (!s) return -1;
    size_t len = strlen(s);
    if (len) console_puts((const uint8_t *)s, (uint32_t)len);
    console_puts((const uint8_t *)"\r\n", 2);   /* puts() appends newline */
    return (int)len + 2;
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
    if (nbytes > 0) {
        shell_exe_cmd(s_cdc_rx_buf, (uint16_t)nbytes);
    }
#else
    (void)nbytes;   /* no shell — input is dropped */
#endif
    usbd_ep_start_read(ep, s_cdc_rx_buf, sizeof(s_cdc_rx_buf));
}

void usbd_event_handler(uint8_t event)
{
    switch (event) {
    case USBD_EVENT_CONFIGURED:
        if (s_cfg_ms == 0) s_cfg_ms = (uint32_t)mtimer_get_time_ms();
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

/* Register the composite and turn the pull-up on. usbd_deinitialize() clears
 * the interface table, so every attach registers again. Task context only. */
static void usb_console_attach(void)
{
    usbd_desc_register(s_descriptor);
    usbd_add_interface(usbd_cdc_acm_init_intf(&s_intf0));
    usbd_add_interface(usbd_cdc_acm_init_intf(&s_intf1));
    usbd_add_endpoint(&s_ep_out);
    usbd_add_endpoint(&s_ep_in);
    s_attempt++;
    s_attach_ms = (uint32_t)mtimer_get_time_ms();
    usbd_initialize();
}

int usb_console_init(void)
{
    s_console_lock = xSemaphoreCreateMutexStatic(&s_console_lock_buf);
    s_console_sig  = xSemaphoreCreateBinaryStatic(&s_console_sig_buf);
    s_tx_done      = xSemaphoreCreateBinaryStatic(&s_tx_done_buf);
    if (!s_console_lock || !s_console_sig || !s_tx_done) return -1;

    /* Detach whatever the bootloader left attached (USB_EN + pull-up) and
     * stay detached until console_tx_task attaches -- see the file header. */
    usb_dc_deinit();
    s_init_ms = (uint32_t)mtimer_get_time_ms();
    build_descriptor();

    xTaskCreateStatic(console_tx_task, "usb_con",
                      sizeof(s_console_stack) / sizeof(StackType_t),
                      NULL, 2, s_console_stack, &s_console_tcb);

    return 0;
}

#else  /* USB CDC not enabled — provide a no-op so callers compile cleanly. */

#include "usb_console.h"
int  usb_console_init(void)         { return 0; }
bool usb_console_is_connected(void) { return false; }

#endif
