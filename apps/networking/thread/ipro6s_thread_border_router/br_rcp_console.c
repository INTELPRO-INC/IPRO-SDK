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
 * RCP UART console pass-through (bench tool).
 *
 * `rcpcon <line...>` takes over UART1 (the Spinel RCP link) and sends one text
 * line to the device on the other end, e.g. a radio board running an OpenThread CLI
 * image instead of the RCP. Everything that device prints is forwarded to this
 * console as `[uart1] ...` lines by a background task, including asynchronous
 * output such as ping replies.
 *
 * Taking over UART1 detaches the Spinel driver: only use it when the attached
 * device is not an RCP (the OT stack has no radio then anyway). Reboot the BR
 * to give UART1 back to Spinel.
 *
 * `rcpcon -pins <tx> <rx>` moves UART1 to other pads (e.g. a second radio on
 * GPIO0/1); `rcpcon -spinel` sends a Spinel NCP_VERSION query and switches
 * the forwarder to hex, which checks a UART RCP's wiring in both directions.
 * `rcpcon -baud <rate>` changes the UART1 rate (a peer whose clock runs off);
 * `rcpcon -break <ms>` holds TX low (a break, or a power cut for a peer that
 * is only fed through its RX pin).
 */
#include <generated/autoconf.h>

#ifdef CONFIG_SHELL

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"
#include "drv_glb.h"
#include "drv_gpio.h"
#include "hal_uart.h"
#include "shell.h"
#include "ipro_log.h"
#include "ot_rcp_health.h"
#include "br_rcp_recovery.h"
#include "br_rcp_select.h"

#define TAG "RCPCON"
#define RCPCON_UART 1
#define RCPCON_RING_SIZE 4096u
#define RCPCON_LINE_MAX 200u
#define RCPCON_TASK_STACK 768u
#define RCPCON_TASK_PRIO 3
#define RCPCON_POLL_MS 5u
#define RCPCON_FLUSH_MS 200u

static uint8_t s_ring[RCPCON_RING_SIZE];
static volatile uint16_t s_head;
static volatile uint16_t s_tail;
static volatile uint32_t s_dropped;
static TaskHandle_t s_task;
/* UART1 pins and baud rate in use (the selected RCP link until `rcpcon -pins`
 * moves them) and hex-dump mode. */
static uint8_t s_tx_pin;
static uint8_t s_rx_pin;
static uint32_t s_baud;
static bool s_link_set;
static volatile bool s_hex;

static void rcpcon_link_init(void)
{
    if (!s_link_set) {
        br_rcp_active_link(&s_tx_pin, &s_rx_pin, &s_baud);
        s_link_set = true;
    }
}

static void rcpcon_rx_isr(void *dev, void *arg, uint32_t size, uint32_t event)
{
    uint8_t data[64];
    int count;

    (void)dev;
    (void)arg;
    (void)size;
    (void)event;

    count = uart_read(RCPCON_UART, data, sizeof(data));
    for (int i = 0; i < count; i++) {
        uint16_t next = (uint16_t)((s_head + 1u) % RCPCON_RING_SIZE);
        if (next == s_tail) {
            s_dropped += (uint32_t)(count - i);
            break;
        }
        s_ring[s_head] = data[i];
        s_head = next;
    }
}

static int ring_pop(uint8_t *byte)
{
    if (s_tail == s_head) {
        return 0;
    }
    *byte = s_ring[s_tail];
    s_tail = (uint16_t)((s_tail + 1u) % RCPCON_RING_SIZE);
    return 1;
}

static void emit_line(char *line, uint16_t *len)
{
    line[*len] = '\0';
    printf("[uart1] %s\r\n", line);
    *len = 0;
}

static void rcpcon_task(void *arg)
{
    char line[RCPCON_LINE_MAX + 1u];
    uint16_t len = 0;
    TickType_t last_rx = xTaskGetTickCount();

    (void)arg;
    for (;;) {
        uint8_t c;
        int got = 0;

        while (ring_pop(&c)) {
            got = 1;
            if (s_hex) {
                /* Binary peer (Spinel/HDLC): 16 bytes per line as hex. */
                static const char hx[] = "0123456789abcdef";
                if (len + 3u > RCPCON_LINE_MAX) {
                    emit_line(line, &len);
                }
                line[len++] = hx[c >> 4];
                line[len++] = hx[c & 0xfu];
                line[len++] = ' ';
                if (len >= 48u) {
                    emit_line(line, &len);
                }
                continue;
            }
            if (c == '\n') {
                emit_line(line, &len);
            } else if (c != '\r' && c != '\b' && len < RCPCON_LINE_MAX) {
                line[len++] = (char)c;
            }
            if (len == RCPCON_LINE_MAX) {
                emit_line(line, &len);
            }
        }
        if (got) {
            last_rx = xTaskGetTickCount();
        } else if (len && (xTaskGetTickCount() - last_rx) > pdMS_TO_TICKS(RCPCON_FLUSH_MS)) {
            /* Prompt or partial line without a newline. */
            emit_line(line, &len);
        }
        vTaskDelay(pdMS_TO_TICKS(RCPCON_POLL_MS));
    }
}

static void rcpcon_route_pins(void)
{
    uint8_t pins[] = {s_tx_pin, s_rx_pin};

    uart_ioctl(RCPCON_UART, UART_CTRL_CLR_INT,
               (void *)(uintptr_t)(UART_RX_END_IT | UART_RX_FIFO_IT | UART_RTO_IT));
    GLB_GPIO_Func_Init(GPIO_FUN_UART, pins, 2);
    GLB_UART_Fun_Sel_Swap(pins[0] % 8, GLB_UART_SIG_FUN_UART1_TXD);
    GLB_UART_Fun_Sel_Swap(pins[1] % 8, GLB_UART_SIG_FUN_UART1_RXD);
    uart_set_baudrate(RCPCON_UART, s_baud);
    uart_init(RCPCON_UART, 0);
    uart_ioctl(RCPCON_UART, UART_CTRL_SET_CALLBACK, (void *)rcpcon_rx_isr);
    uart_ioctl(RCPCON_UART, UART_CTRL_SET_INT,
               (void *)(uintptr_t)(UART_RX_END_IT | UART_RX_FIFO_IT | UART_RTO_IT));
}

/* Move UART1 to other pins; the previous pins go back to plain GPIO so two
 * pads never share one UART signal slot (slot = pin % 8). */
static void rcpcon_set_pins(uint8_t tx, uint8_t rx)
{
    uint8_t old[] = {s_tx_pin, s_rx_pin};

    GLB_GPIO_Func_Init(GPIO_FUN_GPIO, old, 2);
    s_tx_pin = tx;
    s_rx_pin = rx;
    rcpcon_route_pins();
    IPRO_LOGI(TAG, "UART1 now TX=GPIO%u RX=GPIO%u", tx, rx);
}

static int rcpcon_attach(void)
{
    if (s_task != NULL) {
        return 0;
    }
    /* UART1 is ours from now on: Spinel stops touching it, and the OpenThread
     * failure that follows must not reboot the BR under this session. */
    br_rcp_recovery_hold();
    ot_radio_spinel_release_uart();
    s_head = s_tail = 0;
    s_dropped = 0;
    rcpcon_link_init();
    rcpcon_route_pins();
    if (xTaskCreate(rcpcon_task, "rcpcon", RCPCON_TASK_STACK, NULL, RCPCON_TASK_PRIO, &s_task) != pdPASS) {
        s_task = NULL;
        IPRO_LOGE(TAG, "task create failed");
        return -1;
    }
    IPRO_LOGW(TAG, "UART1 detached from Spinel, %lu baud; reboot to restore", (unsigned long)s_baud);
    return 0;
}

static void cmd_rcpcon(int argc, char **argv)
{
    if (argc < 2) {
        IPRO_LOGI(TAG, "usage: rcpcon <line...> | -stat | -pins <tx> <rx> | -baud <rate> | -break <ms> | -hex on|off | -spinel");
        return;
    }
    if (strcmp(argv[1], "-stat") == 0) {
        rcpcon_link_init();
        IPRO_LOGI(TAG, "attached=%d dropped=%lu TX=GPIO%u RX=GPIO%u baud=%lu hex=%d", s_task != NULL,
                  (unsigned long)s_dropped, s_tx_pin, s_rx_pin, (unsigned long)s_baud, (int)s_hex);
        return;
    }
    if (rcpcon_attach() != 0) {
        return;
    }
    if (strcmp(argv[1], "-pins") == 0 && argc >= 4) {
        char *e1, *e2;
        long tx = strtol(argv[2], &e1, 10), rx = strtol(argv[3], &e2, 10);

        /* two GPIOs on different UART signal slots (slot = pin % 8) */
        if (*argv[2] == '\0' || *e1 != '\0' || *argv[3] == '\0' || *e2 != '\0' || tx < 0 || rx < 0 ||
            tx >= GLB_GPIO_PIN_MAX || rx >= GLB_GPIO_PIN_MAX || (tx % 8) == (rx % 8)) {
            IPRO_LOGE(TAG, "pins: two GPIOs 0..%d on different UART slots (pin %% 8)", GLB_GPIO_PIN_MAX - 1);
            return;
        }
        rcpcon_set_pins((uint8_t)tx, (uint8_t)rx);
        return;
    }
    if (strcmp(argv[1], "-baud") == 0 && argc >= 3) {
        char *end;
        unsigned long baud = strtoul(argv[2], &end, 10);

        /* e.g. to read a peer whose clock (and so its UART) runs off */
        if (*argv[2] == '\0' || *end != '\0' || baud < 1200u || baud > 4000000u) {
            IPRO_LOGE(TAG, "baud: 1200..4000000");
            return;
        }
        s_baud = (uint32_t)baud;
        rcpcon_route_pins();
        IPRO_LOGI(TAG, "UART1 now %lu baud", baud);
        return;
    }
    if (strcmp(argv[1], "-break") == 0 && argc >= 3) {
        char *end;
        unsigned long ms = strtoul(argv[2], &end, 10);
        GLB_GPIO_Cfg_Type cfg = {
            .gpioPin = s_tx_pin,
            .gpioFun = GPIO_FUN_GPIO,
            .gpioMode = GPIO_MODE_OUTPUT,
            .pullType = GPIO_PULL_NONE,
            .drive = 0,
            .smtCtrl = 1,
        };

        /* Hold TX low: a UART break, and a power cut for a peer that is only
         * fed through its RX pin. */
        if (*argv[2] == '\0' || *end != '\0' || ms == 0u || ms > 10000u) {
            IPRO_LOGE(TAG, "break: 1..10000 ms");
            return;
        }
        GLB_GPIO_Cfg_Type in = cfg;
        uint32_t rx_before, rx_during;

        in.gpioPin = s_rx_pin;
        in.gpioMode = GPIO_MODE_INPUT;
        GLB_GPIO_Init(&in);
        rx_before = GLB_GPIO_Read(s_rx_pin);
        GLB_GPIO_Init(&cfg);
        GLB_GPIO_Write(s_tx_pin, 0);
        vTaskDelay(pdMS_TO_TICKS(ms));
        /* A powered peer keeps its TX (our RX) high through a break. */
        rx_during = GLB_GPIO_Read(s_rx_pin);
        rcpcon_route_pins();
        IPRO_LOGI(TAG, "TX=GPIO%u held low %lu ms; RX=GPIO%u level before %lu, at end %lu", s_tx_pin, ms,
                  s_rx_pin, (unsigned long)rx_before, (unsigned long)rx_during);
        return;
    }
    if (strcmp(argv[1], "-hex") == 0 && argc >= 3) {
        s_hex = (strcmp(argv[2], "on") == 0 || strcmp(argv[2], "1") == 0);
        return;
    }
    if (strcmp(argv[1], "-spinel") == 0) {
        /* HDLC-lite: PROP_VALUE_GET(NCP_VERSION), TID 1, FCS 0x805e. */
        static const uint8_t q[] = {0x7e, 0x81, 0x02, 0x02, 0x5e, 0x80, 0x7e};
        s_hex = true;
        uart_write(RCPCON_UART, q, sizeof(q));
        IPRO_LOGI(TAG, "sent Spinel NCP_VERSION query on TX=GPIO%u", s_tx_pin);
        return;
    }
    for (int i = 1; i < argc; i++) {
        if (i > 1) {
            uart_write(RCPCON_UART, (const uint8_t *)" ", 1);
        }
        uart_write(RCPCON_UART, (const uint8_t *)argv[i], strlen(argv[i]));
    }
    uart_write(RCPCON_UART, (const uint8_t *)"\r", 1);
}
SHELL_CMD_EXPORT_ALIAS(cmd_rcpcon, rcpcon, Send a line to the UART1 device and forward its output);

#endif /* CONFIG_SHELL */
