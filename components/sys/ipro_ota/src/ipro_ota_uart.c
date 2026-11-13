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
 * Transport-agnostic OTA staging core (see ota_common.h).
 */

/*
 * OTA over the console UART: the transfer and the `ota_uart` shell command
 * (protocol in ipro_ota_uart.h). The command borrows the console's receive
 * interrupt for the length of the transfer and gives it back to the shell.
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#include "hal_sys.h"
#include "hal_uart.h"
#include "shell.h"

#include "../include/ipro_ota.h"
#include "../include/ipro_ota_uart.h"

#define RX_RING_SIZE        2048u       /* a power of two, more than one frame */
#define FRAME_WAIT_MS       2000u       /* silence before the board asks for the frame again */
#define MAX_RETRIES         8u          /* asks in a row before it gives up */

extern int  board_get_debug_uart_index(void);
extern void shell_uart_reinit(void);    /* puts the shell's receive callback back */

static uint8_t          *s_ring;
static volatile uint32_t s_head;        /* written by the UART interrupt */
static volatile uint32_t s_tail;        /* read by the command */
static SemaphoreHandle_t s_sem;
static int               s_uart;

static void ota_uart_rx_cb(void *dev, void *args, uint32_t size, uint32_t event)
{
    uint8_t tmp[32];
    int n;

    (void)dev; (void)args; (void)size; (void)event;
    while ((n = uart_read_available(s_uart, tmp, sizeof(tmp))) > 0) {
        for (int i = 0; i < n; i++) {
            uint32_t next = (s_head + 1u) & (RX_RING_SIZE - 1u);
            if (next != s_tail) {       /* full: the frame's CRC will say so */
                s_ring[s_head] = tmp[i];
                s_head = next;
            }
        }
    }
    if (xPortIsInsideInterrupt()) {
        BaseType_t woken = pdFALSE;
        if (xSemaphoreGiveFromISR(s_sem, &woken) == pdPASS) {
            portYIELD_FROM_ISR(woken);
        }
    } else {
        xSemaphoreGive(s_sem);
    }
}

static void reply(uint8_t what, uint16_t seq)
{
    uint8_t out[4] = { OTA_UART_SOF0, what, (uint8_t)seq, (uint8_t)(seq >> 8) };

    uart_write(s_uart, out, sizeof(out));
}

int ota_uart_receive(uint32_t total, int apply)
{
    uint32_t addr = 0, max = 0, received = 0;
    uint16_t expected = 0;
    unsigned retries = 0;
    ota_uart_rx_t *rx = NULL;
    int rc = -1;

    if (ota_get_fw_b(&addr, &max) != 0) {
        return -2;                                  /* no second FW slot */
    }
    if (total <= OTA_HEADER_SIZE || total > max) {
        return -3;                                  /* not a package, or larger than the slot */
    }
    rx = malloc(sizeof(*rx));
    s_ring = malloc(RX_RING_SIZE);
    s_sem = xSemaphoreCreateBinary();
    if (!rx || !s_ring || !s_sem || ota_sink_begin_ex(addr, max, OTA_SINK_ERASE_AS_WRITTEN) != 0) {
        rc = -4;                                    /* out of memory */
        goto done;
    }
    s_uart = board_get_debug_uart_index();
    s_head = s_tail = 0;
    ota_uart_rx_reset(rx);

    printf("==OTA_UART_READY:%u==\r\n", (unsigned)OTA_UART_MAX_PAYLOAD);
    vTaskDelay(pdMS_TO_TICKS(20));                  /* let the line leave before the input changes hands */
    uart_set_callback(s_uart, (void *)ota_uart_rx_cb);

    rc = 0;
    while (received < total && rc == 0) {
        int got = OTA_UART_RX_MORE;

        if (s_head == s_tail && xSemaphoreTake(s_sem, pdMS_TO_TICKS(FRAME_WAIT_MS)) != pdTRUE && s_head == s_tail) {
            if (++retries > MAX_RETRIES) {
                rc = -5;                            /* the host went quiet */
                break;
            }
            ota_uart_rx_reset(rx);
            reply(OTA_UART_NAK, expected);
            continue;
        }
        while (s_tail != s_head && got == OTA_UART_RX_MORE) {
            uint8_t byte = s_ring[s_tail];

            s_tail = (s_tail + 1u) & (RX_RING_SIZE - 1u);
            got = ota_uart_rx_push(rx, byte);
        }
        if (got == OTA_UART_RX_BAD) {
            if (++retries > MAX_RETRIES) {
                rc = -6;                            /* the line is too noisy */
                break;
            }
            reply(OTA_UART_NAK, expected);
        } else if (got == OTA_UART_RX_FRAME) {
            if (rx->seq == expected) {
                if (rx->len > total - received || ota_sink(rx->payload, rx->len) != 0) {
                    rc = ota_overflowed() ? -3 : -7;    /* flash */
                    break;
                }
                received += rx->len;
                expected++;
                retries = 0;
                reply(OTA_UART_ACK, rx->seq);
            } else if ((uint16_t)(expected - rx->seq) <= 2u && rx->seq != expected) {
                reply(OTA_UART_ACK, rx->seq);       /* a frame the board has: its ACK was lost */
            } else {
                reply(OTA_UART_NAK, expected);
            }
        }
    }
    if (rc == 0 && ota_sink_flush() != 0) {
        rc = -7;
    }
    shell_uart_reinit();

    if (rc == 0) {
        int pkg = ota_validate();

        if (pkg != OTA_PKG_OK) {
            rc = -20 + pkg;                         /* -21 .. -25: OTA_PKG_SHORT .. OTA_PKG_BAD_SHA */
        } else if (apply && ota_mark_fwb_active() != 0) {
            rc = -8;                                /* partition table */
        }
    }

done:
    ota_sink_end();
    free(rx);
    free(s_ring);
    s_ring = NULL;
    if (s_sem) {
        vSemaphoreDelete(s_sem);
        s_sem = NULL;
    }
    return rc;
}

#ifdef CONFIG_SHELL
static int cmd_ota_uart(int argc, char **argv)
{
    uint32_t total = (argc >= 2) ? (uint32_t)strtoul(argv[1], NULL, 0) : 0;
    int apply = !(argc >= 3 && strcmp(argv[2], "noapply") == 0);
    int rc;

    if (total == 0) {
        printf("usage: ota_uart <package bytes> [noapply]  (sent by a host tool, see ipro_ota_uart.h)\r\n");
        return -1;
    }
    rc = ota_uart_receive(total, apply);
    if (rc != 0) {
        printf("\r\n==OTA_UART_FAIL:%d==\r\n", rc);
        return rc;
    }
    printf("\r\n==OTA_UART_OK==\r\n");
    if (apply) {
        vTaskDelay(pdMS_TO_TICKS(100));
        hal_reboot();
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_ota_uart, ota_uart, receive an OTA package on the console UART and apply it);
#endif
