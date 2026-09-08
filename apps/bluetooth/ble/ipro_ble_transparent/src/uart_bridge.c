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
 * @file uart_bridge.c
 * @brief UART/CDC bridge: AT command mode + data transparent mode
 *
 * Transport is selected at compile time:
 *   CONFIG_AT_TRANSPORT_UART (default) - uses UART1
 *   CONFIG_AT_TRANSPORT_CDC           - uses USB CDC ACM
 */

#include "uart_bridge.h"
#include "at_command.h"
#include "ble_transparent.h"
#include "app_config.h"
#include "ipro_log.h"

#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#ifdef CONFIG_AT_TRANSPORT_UART
#include "hal_uart.h"
#endif

#ifdef CONFIG_AT_TRANSPORT_CDC
#include "usbd_core.h"
#include "usbd_cdc.h"
#endif

#define LOG_TAG "UART_BRG"

/* Ring buffer */
typedef struct {
    uint8_t buf[UART_RX_RING_SIZE];
    volatile uint16_t head;
    volatile uint16_t tail;
} ring_buf_t;

static struct {
    bridge_mode_t mode;
    uint32_t baudrate;
    ring_buf_t rx_ring;
    uint8_t cmd_buf[AT_CMD_MAX_LEN];
    uint16_t cmd_len;

    /* Data mode state */
    uint8_t ble_tx_buf[BLE_TX_BUF_SIZE];
    uint16_t ble_tx_len;
    TickType_t last_rx_tick;

    /* +++ escape detection */
    uint8_t escape_count;
    TickType_t escape_start_tick;
} g_bridge = {
    .mode = BRIDGE_MODE_COMMAND,
    .baudrate = DEFAULT_UART_BAUDRATE,
};

static TaskHandle_t g_uart_task_handle;

/* ============================================================
 * Ring buffer helpers
 * ============================================================ */

static inline bool ring_empty(const ring_buf_t *r)
{
    return r->head == r->tail;
}

static inline bool ring_full(const ring_buf_t *r)
{
    return ((r->head + 1) % UART_RX_RING_SIZE) == r->tail;
}

static inline void ring_put(ring_buf_t *r, uint8_t byte)
{
    if (!ring_full(r)) {
        r->buf[r->head] = byte;
        r->head = (r->head + 1) % UART_RX_RING_SIZE;
    }
}

static inline int ring_get(ring_buf_t *r, uint8_t *byte)
{
    if (ring_empty(r)) return -1;
    *byte = r->buf[r->tail];
    r->tail = (r->tail + 1) % UART_RX_RING_SIZE;
    return 0;
}

/* ============================================================
 * Transport abstraction: write to host (UART TX or CDC IN)
 * ============================================================ */

static int transport_write(const void *data, uint32_t len);

#ifdef CONFIG_AT_TRANSPORT_UART

/* --- UART transport --- */

static void uart_irq_callback(void *dev, void *args, uint32_t size, uint32_t event)
{
    if (event == UART_EVENT_RX_FIFO || event == UART_EVENT_RTO) {
        uint8_t *d = (uint8_t *)args;
        for (uint32_t i = 0; i < size; i++) {
            ring_put(&g_bridge.rx_ring, d[i]);
        }
        if (g_uart_task_handle) {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            vTaskNotifyGiveFromISR(g_uart_task_handle, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }
}

static int transport_write(const void *data, uint32_t len)
{
    return uart_write(BRIDGE_UART_ID, data, len);
}

static void transport_hw_init(void)
{
    uart_init(BRIDGE_UART_ID, 0);
    uart_set_baudrate(BRIDGE_UART_ID, g_bridge.baudrate);
    uart_set_callback(BRIDGE_UART_ID, (void *)uart_irq_callback);
    uart_ioctl(BRIDGE_UART_ID, UART_CTRL_SET_INT,
               (void *)(uintptr_t)(UART_RX_FIFO_IT | UART_RTO_IT));
    IPRO_LOGI(LOG_TAG, "Transport: UART%d @ %lu baud", BRIDGE_UART_ID,
              (unsigned long)g_bridge.baudrate);
}

#endif /* CONFIG_AT_TRANSPORT_UART */

#ifdef CONFIG_AT_TRANSPORT_CDC

/* --- CDC transport --- */

#include "usbd_event_common.h"

/* DMA-aligned buffers */
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX static uint8_t cdc_read_buf[2048];
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX static uint8_t cdc_tx_buf[2048];
static volatile bool cdc_tx_busy;
static volatile bool cdc_connected;
static volatile bool cdc_dtr;

static void cdc_event_handler(uint8_t event)
{
    switch (event) {
    case USBD_EVENT_CONFIGURED:
        cdc_connected = true;
        usbd_ep_start_read(CDC_OUT_EP, cdc_read_buf, sizeof(cdc_read_buf));
        break;
    case USBD_EVENT_RESET:
        cdc_connected = false;
        break;
    case USBD_EVENT_DISCONNECTED:
        cdc_connected = false;
        break;
    default:
        break;
    }
}

void usbd_cdc_acm_bulk_out(uint8_t ep, uint32_t nbytes)
{
    for (uint32_t i = 0; i < nbytes; i++) {
        ring_put(&g_bridge.rx_ring, cdc_read_buf[i]);
    }

    if (g_uart_task_handle) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        vTaskNotifyGiveFromISR(g_uart_task_handle, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }

    /* Re-arm read */
    usbd_ep_start_read(CDC_OUT_EP, cdc_read_buf, sizeof(cdc_read_buf));
}

void usbd_cdc_acm_bulk_in(uint8_t ep, uint32_t nbytes)
{
    if ((nbytes % CDC_MAX_MPS) == 0 && nbytes) {
        /* Need ZLP */
        usbd_ep_start_write(CDC_IN_EP, NULL, 0);
    } else {
        cdc_tx_busy = false;
    }
}

void usbd_cdc_acm_set_dtr(uint8_t intf, bool dtr)
{
    (void)intf;
    cdc_dtr = dtr;
}

void usbd_cdc_acm_set_rts(uint8_t intf, bool rts)
{    
    (void)intf;
    (void)rts;
}

static int transport_write(const void *data, uint32_t len)
{
    if (!cdc_connected || len == 0) {
        return -1;
    }

    uint32_t remaining = len;
    const uint8_t *src = (const uint8_t *)data;

    while (remaining > 0) {
        uint32_t chunk = remaining > sizeof(cdc_tx_buf) ? sizeof(cdc_tx_buf) : remaining;
        memcpy(cdc_tx_buf, src, chunk);

        cdc_tx_busy = true;
        usbd_ep_start_write(CDC_IN_EP, cdc_tx_buf, chunk);

        /* Wait for transfer complete */
        uint32_t timeout = 1000;
        while (cdc_tx_busy && --timeout) {
            vTaskDelay(1);
        }
        if (cdc_tx_busy) {
            cdc_tx_busy = false;
            return -1;
        }

        src += chunk;
        remaining -= chunk;
    }

    return 0;
}

/* USB descriptor for CDC ACM */
#define USB_CONFIG_SIZE (9 + CDC_ACM_DESCRIPTOR_LEN)

static const uint8_t cdc_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xEF, 0x02, 0x01, 0xFFFF, 0xFFFF, 0x0100, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x02, 0x01, USB_CONFIG_BUS_POWERED, 100),
    CDC_ACM_DESCRIPTOR_INIT(0x00, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP, CDC_MAX_MPS, 0x02),
    USB_LANGID_INIT(1033),
    /* String 1: Manufacturer */
    0x0E, USB_DESCRIPTOR_TYPE_STRING,
    'I', 0, 'P', 0, 'R', 0, 'O', 0, ' ', 0, ' ', 0,
    /* String 2: Product */
    0x1E, USB_DESCRIPTOR_TYPE_STRING,
    'B', 0, 'L', 0, 'E', 0, ' ', 0, 'T', 0, 'r', 0, 'a', 0, 'n', 0,
    's', 0, 'p', 0, 'a', 0, 'r', 0, 'e', 0, 'n', 0,
    /* String 3: Serial */
    0x12, USB_DESCRIPTOR_TYPE_STRING,
    '2', 0, '0', 0, '2', 0, '6', 0, '0', 0, '2', 0, '0', 0, '8', 0,
    0x00
};

static struct usbd_endpoint cdc_out_ep_desc = {
    .ep_addr = CDC_OUT_EP,
    .ep_cb = usbd_cdc_acm_bulk_out
};

static struct usbd_endpoint cdc_in_ep_desc = {
    .ep_addr = CDC_IN_EP,
    .ep_cb = usbd_cdc_acm_bulk_in
};

static struct usbd_interface intf0;
static struct usbd_interface intf1;

static void transport_hw_init(void)
{
    usbd_deinitialize();
    usbd_desc_register(cdc_descriptor);
    usbd_add_interface(usbd_cdc_acm_init_intf(&intf0));
    usbd_add_interface(usbd_cdc_acm_init_intf(&intf1));
    usbd_add_endpoint(&cdc_out_ep_desc);
    usbd_add_endpoint(&cdc_in_ep_desc);
    usbd_event_register_callback(cdc_event_handler);
    usbd_initialize();
    IPRO_LOGI(LOG_TAG, "Transport: USB CDC ACM initialized");
}

#endif /* CONFIG_AT_TRANSPORT_CDC */

/* ============================================================
 * BLE RX callback: forward BLE data to host
 * ============================================================ */

static void ble_rx_to_host(const uint8_t *data, uint16_t len)
{
    transport_write(data, len);
}

/* ============================================================
 * Command mode: collect lines → AT parser
 * ============================================================ */

static void process_command_mode(uint8_t byte)
{
    if (byte == '\n') {
        if (g_bridge.cmd_len > 0 && g_bridge.cmd_buf[g_bridge.cmd_len - 1] == '\r') {
            g_bridge.cmd_len--;
        }
        g_bridge.cmd_buf[g_bridge.cmd_len] = '\0';
        if (g_bridge.cmd_len > 0) {
            IPRO_LOGI(LOG_TAG, "AT cmd: '%s'", g_bridge.cmd_buf);
            at_command_process((const char *)g_bridge.cmd_buf);
        }
        g_bridge.cmd_len = 0;
    } else if (byte == '\r') {
        g_bridge.cmd_buf[g_bridge.cmd_len] = '\0';
        if (g_bridge.cmd_len > 0) {
            IPRO_LOGI(LOG_TAG, "AT cmd: '%s'", g_bridge.cmd_buf);
            at_command_process((const char *)g_bridge.cmd_buf);
        }
        g_bridge.cmd_len = 0;
    } else {
        if (g_bridge.cmd_len < AT_CMD_MAX_LEN - 1) {
            g_bridge.cmd_buf[g_bridge.cmd_len++] = byte;
        }
    }
}

/* ============================================================
 * Escape detection (+++ with guard silence)
 * ============================================================ */

static void check_escape(uint8_t byte)
{
    TickType_t now = xTaskGetTickCount();

    if (byte == DATA_MODE_ESCAPE_CHAR) {
        if (g_bridge.escape_count == 0) {
            TickType_t silence = now - g_bridge.last_rx_tick;
            if (silence >= pdMS_TO_TICKS(DATA_MODE_ESCAPE_GUARD_MS)) {
                g_bridge.escape_count = 1;
                g_bridge.escape_start_tick = now;
            }
        } else {
            g_bridge.escape_count++;
        }
    } else {
        /* Non-'+' resets; flush accumulated '+' as data */
        if (g_bridge.escape_count > 0) {
            for (int i = 0; i < g_bridge.escape_count; i++) {
                if (g_bridge.ble_tx_len < BLE_TX_BUF_SIZE) {
                    g_bridge.ble_tx_buf[g_bridge.ble_tx_len++] = DATA_MODE_ESCAPE_CHAR;
                }
            }
            g_bridge.escape_count = 0;
        }
    }
}

/* Flush BLE TX buffer */
static void flush_ble_tx(void)
{
    if (g_bridge.ble_tx_len > 0) {
        if (ble_transparent_get_state() == BLE_STATE_CONNECTED) {
            ble_transparent_send(g_bridge.ble_tx_buf, g_bridge.ble_tx_len);
        }
        g_bridge.ble_tx_len = 0;
    }
}

/* Data mode: accumulate → BLE */
static void process_data_mode(uint8_t byte)
{
    g_bridge.last_rx_tick = xTaskGetTickCount();
    check_escape(byte);

    if (g_bridge.escape_count > 0) {
        return;
    }

    if (g_bridge.ble_tx_len < BLE_TX_BUF_SIZE) {
        g_bridge.ble_tx_buf[g_bridge.ble_tx_len++] = byte;
    }

    uint16_t mtu = ble_transparent_get_mtu();
    uint16_t max_payload = (mtu > 3) ? (mtu - 3) : 20;
    if (g_bridge.ble_tx_len >= max_payload) {
        flush_ble_tx();
    }
}

/* ============================================================
 * Bridge task
 * ============================================================ */

static void bridge_task(void *pvParameters)
{
    (void)pvParameters;
    IPRO_LOGI(LOG_TAG, "Bridge task started");

    for (;;) {
        ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(DATA_MODE_PACK_TIMEOUT_MS));

        uint8_t byte;
        while (ring_get(&g_bridge.rx_ring, &byte) == 0) {
            if (g_bridge.mode == BRIDGE_MODE_COMMAND) {
                process_command_mode(byte);
            } else {
                process_data_mode(byte);
            }
        }

        /* Data mode: check escape completion & timeout flush */
        if (g_bridge.mode == BRIDGE_MODE_DATA) {
            TickType_t now = xTaskGetTickCount();

            if (g_bridge.escape_count >= DATA_MODE_ESCAPE_COUNT) {
                TickType_t since = now - g_bridge.escape_start_tick;
                if (since >= pdMS_TO_TICKS(DATA_MODE_ESCAPE_GUARD_MS)) {
                    IPRO_LOGI(LOG_TAG, "Escape: switching to command mode");
                    g_bridge.escape_count = 0;
                    g_bridge.mode = BRIDGE_MODE_COMMAND;
                    transport_write("OK\r\n", 4);
                    continue;
                }
            }

            if (g_bridge.ble_tx_len > 0) {
                TickType_t idle = now - g_bridge.last_rx_tick;
                if (idle >= pdMS_TO_TICKS(DATA_MODE_PACK_TIMEOUT_MS)) {
                    flush_ble_tx();
                }
            }
        }
    }
}

/* ============================================================
 * Public API
 * ============================================================ */

int uart_bridge_init(void)
{
    transport_hw_init();
    ble_transparent_set_rx_cb(ble_rx_to_host);

    static StackType_t task_stack[1024];
    static StaticTask_t task_tcb;
    g_uart_task_handle = xTaskCreateStatic(
        bridge_task, "bridge", 1024, NULL,
        configMAX_PRIORITIES - 3, task_stack, &task_tcb);

    IPRO_LOGI(LOG_TAG, "Bridge initialized (%s)", uart_bridge_get_transport_name());
    return 0;
}

int uart_bridge_send(const uint8_t *data, uint16_t len)
{
    return transport_write(data, len);
}

int uart_bridge_send_str(const char *str)
{
    return transport_write(str, strlen(str));
}

bridge_mode_t uart_bridge_get_mode(void)
{
    return g_bridge.mode;
}

int uart_bridge_set_mode(bridge_mode_t mode)
{
    if (mode == g_bridge.mode) return 0;
    g_bridge.mode = mode;
    g_bridge.cmd_len = 0;
    g_bridge.ble_tx_len = 0;
    g_bridge.escape_count = 0;
    g_bridge.last_rx_tick = xTaskGetTickCount();
    IPRO_LOGI(LOG_TAG, "Mode: %s", mode == BRIDGE_MODE_DATA ? "DATA" : "COMMAND");
    return 0;
}

int uart_bridge_set_baudrate(uint32_t baudrate)
{
#ifdef CONFIG_AT_TRANSPORT_UART
    g_bridge.baudrate = baudrate;
    uart_set_baudrate(BRIDGE_UART_ID, baudrate);
    IPRO_LOGI(LOG_TAG, "Baudrate: %lu", (unsigned long)baudrate);
#else
    (void)baudrate;
    /* CDC baudrate is controlled by host */
#endif
    return 0;
}

uint32_t uart_bridge_get_baudrate(void)
{
    return g_bridge.baudrate;
}

const char *uart_bridge_get_transport_name(void)
{
#ifdef CONFIG_AT_TRANSPORT_CDC
    return "USB CDC";
#else
    return "UART";
#endif
}
