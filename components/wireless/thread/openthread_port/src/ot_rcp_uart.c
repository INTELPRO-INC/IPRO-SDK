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

/* Dedicated UART1 transport for OpenThread Spinel/HDLC RCP mode. */

#include <stdbool.h>
#include <stdint.h>

#include "drv_glb.h"
#include "drv_gpio.h"
#include "hal_uart.h"
#include "openthread/ncp.h"
#include "openthread/platform/misc.h"
#include "openthread_port.h"
#include "utils/uart.h"
#include "ipro_log.h"
#include "ipro7.h"
#ifdef CONFIG_THREAD_RCP_DEVICE_UART_DIAGNOSTICS
#include "drv_uart.h"
#include "rcp_uart_diagnostics.h"
#ifdef CONFIG_SHELL
#include "shell.h"
#endif
static RcpUartDiagnostics s_diag;
/* RX FIFO overruns (bytes lost before the ISR ran) and the most bytes the
 * ISR found waiting: the FIFO holds 16. */
static volatile uint32_t s_hw_overflow;
static volatile uint32_t s_fifo_fill_max;
static bool s_hw_overflow_seen;
#endif

#define RCP_RX_RING_SIZE 2048u

/* ECLIC level of the Spinel UART RX interrupt: above the 802.15.4 MAC
 * interrupt (level 3). That one runs for up to ~1.2 ms while it prepares an
 * Enh-ACK during a reception (hal_rx_read_head waits for the header bytes
 * on air), and the UART RX FIFO holds 16 bytes - 160 us at 1 Mbaud. At the
 * default level 1 the FIFO overran, the host's frame failed its FCS and was
 * dropped, and the host timed out on its transmit ("radio tx timeout", RCP
 * recovery; bench 2026-09-29: 25 in 3 min of CSL traffic, one per bad FCS).
 * The MAC's timed register sequences run with interrupts off, so a UART
 * interrupt only delays its waits. Clamped to the levels the ECLIC has. */
#ifndef RCP_UART_IRQ_LEVEL
#define RCP_UART_IRQ_LEVEL 4u
#endif
static uint8_t s_irq_level;

static uint8_t s_rx_ring[RCP_RX_RING_SIZE];
static volatile uint16_t s_rx_head;
static volatile uint16_t s_rx_tail;
static bool s_tx_done_pending;

static void route_uart1_pin(uint8_t gpio, int is_tx)
{
    GLB_GPIO_Cfg_Type cfg = {
        .gpioPin = gpio,
        .gpioFun = GPIO_FUN_UART,
        .gpioMode = is_tx ? GPIO_MODE_OUTPUT : GPIO_MODE_INPUT,
        .pullType = GPIO_PULL_UP,
        .drive = 0,
        .smtCtrl = 1,
    };

    GLB_GPIO_Init(&cfg);
    GLB_UART_Fun_Sel((GLB_UART_SIG_Type)(gpio % 12u),
                     is_tx ? GLB_UART_SIG_FUN_UART1_TXD
                           : GLB_UART_SIG_FUN_UART1_RXD);
}

static void rcp_uart_rx_isr(void *dev, void *arg, uint32_t size, uint32_t event)
{
    uint8_t data[64];
    uint32_t count;
    int n;
    int notify = 0;

    (void)dev;
    (void)arg;
    (void)size;
    (void)event;

#ifdef CONFIG_THREAD_RCP_DEVICE_UART_DIAGNOSTICS
    {
        uint32_t fill = UART_GetRxFifoCount((UART_ID_Type)CONFIG_THREAD_RCP_UART_PORT);

        if (fill > s_fifo_fill_max) {
            s_fifo_fill_max = fill;
        }
    }
#endif
    n = uart_read(CONFIG_THREAD_RCP_UART_PORT, data, sizeof(data));
    /* A HAL error (negative) or an impossible length queues nothing: it was
     * taken for a huge count and read past data[]. */
    count = (n > 0 && (uint32_t)n <= sizeof(data)) ? (uint32_t)n : 0u;
#ifdef CONFIG_THREAD_RCP_DEVICE_UART_DIAGNOSTICS
    if (n >= 0 && (uint32_t)n <= sizeof(data)) {
        s_diag.read_bytes += count;
    } else {
        ++s_diag.read_invalid;
    }
#endif
    for (uint32_t i = 0; i < count; i++) {
        uint16_t next = (uint16_t)((s_rx_head + 1u) % RCP_RX_RING_SIZE);
        if (next == s_rx_tail) {
#ifdef CONFIG_THREAD_RCP_DEVICE_UART_DIAGNOSTICS
            s_diag.drop_bytes += count - i;
            ++s_diag.overflow_events;
#endif
            break;
        }
        s_rx_ring[s_rx_head] = data[i];
        s_rx_head = next;
#ifdef CONFIG_THREAD_RCP_DEVICE_UART_DIAGNOSTICS
        ++s_diag.queued_bytes;
#endif
        notify = 1;
    }

#ifdef CONFIG_THREAD_RCP_DEVICE_UART_DIAGNOSTICS
    /* The hardware FIFO overran before this ISR ran: bytes were lost on the
     * wire side. The flag clears only with the FIFO, so it is cleared only
     * while the FIFO is empty (interrupts off: nothing but a byte finishing
     * in those two register accesses can be lost), else on a later
     * interrupt; counted once per overrun. */
    if (UART_GetOverflowStatus((UART_ID_Type)CONFIG_THREAD_RCP_UART_PORT, UART_RX_OVERFLOW) == SET) {
        unsigned long mstatus = __RV_CSR_READ_CLEAR(CSR_MSTATUS, MSTATUS_MIE);

        if (!s_hw_overflow_seen) {
            s_hw_overflow_seen = true;
            ++s_hw_overflow;
        }
        if (UART_GetRxFifoCount((UART_ID_Type)CONFIG_THREAD_RCP_UART_PORT) == 0u) {
            (void)UART_RxFifoClear((UART_ID_Type)CONFIG_THREAD_RCP_UART_PORT);
            s_hw_overflow_seen = false;
        }
        __RV_CSR_WRITE(CSR_MSTATUS, mstatus);
    }
#endif

    if (notify) {
        OT_NOTIFY_ISR(OT_SYSTEM_EVENT_UART_RXD);
    }
}

otError otPlatUartEnable(void)
{
    s_rx_head = 0;
    s_rx_tail = 0;
    route_uart1_pin(CONFIG_THREAD_RCP_UART_TX_PIN, 1);
    route_uart1_pin(CONFIG_THREAD_RCP_UART_RX_PIN, 0);
    uart_set_baudrate(CONFIG_THREAD_RCP_UART_PORT, CONFIG_THREAD_RCP_UART_BAUD);
    uart_init(CONFIG_THREAD_RCP_UART_PORT, 0);
    uart_ioctl(CONFIG_THREAD_RCP_UART_PORT, UART_CTRL_SET_INT,
               (void *)(uintptr_t)(UART_RX_END_IT | UART_RX_FIFO_IT | UART_RTO_IT));
    uart_ioctl(CONFIG_THREAD_RCP_UART_PORT, UART_CTRL_SET_CALLBACK,
               (void *)rcp_uart_rx_isr);
    {
        uint8_t max = (uint8_t)((1u << ECLIC_GetInfoCtlbits()) - 1u);

        s_irq_level = (uint8_t)(RCP_UART_IRQ_LEVEL < max ? RCP_UART_IRQ_LEVEL : max);
        ECLIC_SetLevelIRQ(CONFIG_THREAD_RCP_UART_PORT == 0 ? UART0_IRQn : UART1_IRQn, s_irq_level);
    }
    return OT_ERROR_NONE;
}

otError otPlatUartDisable(void)
{
    uart_ioctl(CONFIG_THREAD_RCP_UART_PORT, UART_CTRL_SET_INT, (void *)0);
    return OT_ERROR_NONE;
}

otError otPlatUartSend(const uint8_t *buf, uint16_t length)
{
    IPRO_LOGD("RCP-UART", "TX %u bytes", length);
    uart_write(CONFIG_THREAD_RCP_UART_PORT, buf, length);
#ifdef CONFIG_THREAD_RCP_DEVICE_UART_DIAGNOSTICS
    s_diag.tx_bytes += length;
    ++s_diag.tx_calls;
#endif
    /* Complete after the NCP send callback returns and commits its buffer
     * accounting. Inline completion reenters NCP with stale TX state. */
    s_tx_done_pending = true;
    OT_NOTIFY(OT_SYSTEM_EVENT_UART_TXD);
    return OT_ERROR_NONE;
}

otError otPlatUartFlush(void)
{
    return OT_ERROR_NONE;
}

void ot_uartTask(ot_system_event_t events)
{
    if (s_tx_done_pending) {
        s_tx_done_pending = false;
#ifdef CONFIG_THREAD_RCP_DEVICE_UART_DIAGNOSTICS
        ++s_diag.tx_done;
#endif
        otPlatUartSendDone();
    }
    /* bytes in the ring are handled even without the event bit */
    if ((events & OT_SYSTEM_EVENT_UART_RXD) == 0u && s_rx_tail == s_rx_head) {
        return;
    }

    while (s_rx_tail != s_rx_head) {
        uint16_t end = s_rx_head;
        uint16_t length = (end > s_rx_tail)
                              ? (uint16_t)(end - s_rx_tail)
                              : (uint16_t)(RCP_RX_RING_SIZE - s_rx_tail);
        IPRO_LOGD("RCP-UART", "RX %u bytes", length);
#ifdef CONFIG_THREAD_RCP_DEVICE_UART_DIAGNOSTICS
        s_diag.decode_bytes += length;
#endif
        otPlatUartReceived(&s_rx_ring[s_rx_tail], length);
        s_rx_tail = (uint16_t)((s_rx_tail + length) % RCP_RX_RING_SIZE);
    }
}

void otPlatUartReceived(const uint8_t *buf, uint16_t length)
{
    otNcpHdlcReceive(buf, length);
}

void otPlatUartSendDone(void)
{
    otNcpHdlcSendDone();
}

static int rcp_hdlc_send(const uint8_t *buf, uint16_t length)
{
    (void)otPlatUartSend(buf, length);
    return length;
}

void otRcpUartInit(otInstance *instance)
{
    (void)otPlatUartEnable();
    otNcpHdlcInit(instance, rcp_hdlc_send);
}

otPlatResetReason otPlatGetResetReason(otInstance *instance)
{
    (void)instance;
    return OT_PLAT_RESET_REASON_POWER_ON;
}

void otPlatWakeHost(void)
{
}

#if defined(CONFIG_THREAD_RCP_DEVICE_UART_DIAGNOSTICS) && defined(CONFIG_SHELL)
static void cmd_rcp_uart_stats(int argc, char **argv)
{
    RcpUartDiagnostics snapshot;
    otNcpHdlcDiagnosticCounters hdlc;
    bool hdlc_ready;
    (void)argv;
    if (argc != 1) {
        IPRO_LOGW("RCP-UART", "usage: rcp_uart_stats");
        return;
    }
    if (otrGetInstanceIfReady() == NULL) {
        IPRO_LOGW("RCP-UART", "uart_v1 not-ready");
        return;
    }
    if (!otrTryLock()) {
        IPRO_LOGW("RCP-UART", "uart_v1 busy");
        return;
    }
    hdlc_ready = otNcpHdlcGetDiagnosticCounters(&hdlc);
    OT_ENTER_CRITICAL();
    snapshot = s_diag;
    OT_EXIT_CRITICAL();
    otrUnlock();
    IPRO_LOGI("RCP-UART",
              "uart_v1 read=%lu invalid=%lu queued=%lu drop=%lu overflow=%lu "
              "decode=%lu tx_bytes=%lu tx_calls=%lu tx_done=%lu",
              (unsigned long)snapshot.read_bytes, (unsigned long)snapshot.read_invalid,
              (unsigned long)snapshot.queued_bytes, (unsigned long)snapshot.drop_bytes,
              (unsigned long)snapshot.overflow_events, (unsigned long)snapshot.decode_bytes,
              (unsigned long)snapshot.tx_bytes, (unsigned long)snapshot.tx_calls,
              (unsigned long)snapshot.tx_done);
    if (hdlc_ready) {
        IPRO_LOGI("RCP-UART", "hdlc_v1 valid=%lu bad_fcs=%lu short=%lu no_buf=%lu",
                  (unsigned long)hdlc.mValid, (unsigned long)hdlc.mBadFcs,
                  (unsigned long)hdlc.mShort, (unsigned long)hdlc.mNoBuffer);
    } else {
        IPRO_LOGW("RCP-UART", "hdlc_v1 not-ready");
    }
    IPRO_LOGI("RCP-UART", "uart_hw_v1 overflow=%lu fifo_fill_max=%lu irq_level=%u",
              (unsigned long)s_hw_overflow, (unsigned long)s_fifo_fill_max, s_irq_level);
}
SHELL_CMD_EXPORT_ALIAS(cmd_rcp_uart_stats, rcp_uart_stats, Read RCP UART diagnostic counters);
#endif
