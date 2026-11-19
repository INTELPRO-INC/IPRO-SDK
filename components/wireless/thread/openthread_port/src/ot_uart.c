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
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <bl_uart.h>
#include <hosal_uart.h>

#include <openthread-core-config.h>
#include <openthread/config.h>

#include <openthread/platform/debug_uart.h>
#include <openthread/platform/logging.h>
#include <openthread_port.h>
#include <utils/uart.h>

extern hosal_uart_dev_t uart_stdio;

void otPlatDebugUart_write_bytes(const uint8_t *pBytes, int nBytes)
{
}

typedef struct _otUart {
    uint16_t    start;
    uint16_t    end;
    uint32_t    recvLen;
    uint8_t     rxbuf[OT_UART_RX_BUFFSIZE];
} otUart_t;

static otUart_t otUart_var;

static int ot_uartRxdCb(void *p_arg);
static int ot_uartTxdCb(void *p_arg)
{
#if !CFG_CPC_ENABLE

    hosal_uart_ioctl(&uart_stdio, HOSAL_UART_TX_TRIGGER_OFF, NULL);
#endif
    return 0;
}

otError otPlatUartEnable(void)
{
#if !CFG_CPC_ENABLE
    hosal_uart_finalize(&uart_stdio);

    hosal_uart_init(&uart_stdio);

    hosal_uart_callback_set(&uart_stdio, HOSAL_UART_RX_CALLBACK, ot_uartRxdCb, NULL);
    hosal_uart_callback_set(&uart_stdio, HOSAL_UART_TX_CALLBACK, ot_uartTxdCb, NULL);
    hosal_uart_ioctl(&uart_stdio, HOSAL_UART_MODE_SET, (void *)HOSAL_UART_MODE_INT);

    memset(&otUart_var, 0, sizeof(otUart_t));

#endif
    return OT_ERROR_NONE;
}

otError otPlatUartDisable(void)
{
#if !CFG_CPC_ENABLE
    hosal_uart_finalize(&uart_stdio);
    memset(&otUart_var, 0, sizeof(otUart_t));
#endif
    return OT_ERROR_NONE;
}

otError otPlatUartSend(const uint8_t *aBuf, uint16_t aBufLength)
{
#if !CFG_CPC_ENABLE
    OT_CLI_UART_OUTPUT_LOCK();

    hosal_uart_send(&uart_stdio, aBuf, aBufLength);
    otPlatUartSendDone();

    OT_CLI_UART_OUTPUT_UNLOCK();
#endif
    return OT_ERROR_NONE;
}

otError otPlatUartFlush(void)
{
#if !CFG_CPC_ENABLE
    hosal_uart_ioctl(&uart_stdio, HOSAL_UART_FLUSH, NULL);
#endif
    return OT_ERROR_NONE;
}

static int ot_uartRxdCb(void *p_arg)
{
#if !CFG_CPC_ENABLE
    uint32_t len = 0;

    if (otUart_var.start >= otUart_var.end) {
        otUart_var.start += hosal_uart_receive(&uart_stdio, otUart_var.rxbuf + otUart_var.start,
            OT_UART_RX_BUFFSIZE - otUart_var.start - 1);
        if (otUart_var.start == OT_UART_RX_BUFFSIZE - 1) {
            otUart_var.start = hosal_uart_receive(&uart_stdio, otUart_var.rxbuf,
                (OT_UART_RX_BUFFSIZE + otUart_var.end - 1) % OT_UART_RX_BUFFSIZE);
        }
    }
    else if (((otUart_var.start + 1) % OT_UART_RX_BUFFSIZE) != otUart_var.end) {
        otUart_var.start += hosal_uart_receive(&uart_stdio, otUart_var.rxbuf,
            otUart_var.end - otUart_var.start - 1);
    }

    if (otUart_var.start != otUart_var.end) {

        len = (otUart_var.start + OT_UART_RX_BUFFSIZE - otUart_var.end) % OT_UART_RX_BUFFSIZE;
        if (otUart_var.recvLen != len) {
            otUart_var.recvLen = len;
            OT_NOTIFY_ISR(OT_SYSTEM_EVENT_UART_RXD);
        }
    }
#endif
    return 0;
}

void ot_uartTask (ot_system_event_t sevent)
{
#ifndef CFG_CPC_ENABLE
    if (!(OT_SYSETM_EVENT_UART_ALL_MASK & sevent)) {
        return;
    }

    if (OT_SYSTEM_EVENT_UART_RXD & sevent) {
        OT_ENTER_CRITICAL();

        if (otUart_var.start != otUart_var.end) {
            if (otUart_var.start > otUart_var.end) {
                otPlatUartReceived(otUart_var.rxbuf + otUart_var.end, otUart_var.start - otUart_var.end);
                otUart_var.end = otUart_var.start;
            }
            else {
                otPlatUartReceived(otUart_var.rxbuf + otUart_var.end, OT_UART_RX_BUFFSIZE - otUart_var.end);
                otUart_var.end = OT_UART_RX_BUFFSIZE - 1;
                if (otUart_var.start) {
                    otPlatUartReceived(otUart_var.rxbuf, otUart_var.start);
                    otUart_var.end = (OT_UART_RX_BUFFSIZE + otUart_var.start - 1) % OT_UART_RX_BUFFSIZE;
                }
            }
        }

        otUart_var.start = otUart_var.end = 0;
        otUart_var.recvLen = 0;
        OT_EXIT_CRITICAL();
    }
#endif
}

void ot_uartLog(const char *fmt, va_list argp)
{
#ifndef CFG_CPC_ENABLE
#if CFG_USB_CDC_ENABLE || ! defined (CONFIG_NCP)
    static char *str;
    static char string[384];
    int ch;

    str = string;
    if (0 < vsprintf(str, fmt, argp)) {
        while ('\0' != (ch = *(str++))) {
            bl_uart_data_send(uart_stdio.config.uart_id, ch);
        }
    }
#endif
#endif
}
