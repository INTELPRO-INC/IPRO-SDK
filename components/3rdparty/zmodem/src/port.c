/*
 * IPRO SDK serial port for the Zmodem component.
 *
 * The library leaves only two hooks unimplemented (zm_recv / zm_send); this
 * file backs them with hal_uart. The target UART and the receive idle-timeout
 * are runtime-settable (zm_port_set_uart / zm_port_set_timeout_ms) so a caller
 * can drive zmodem over any UART at any baud without editing this file.
 *
 * No printf in the data path: the wire must stay byte-clean. RX is polled in
 * bulk straight from the HW FIFO (uart_read_available -> UART_ReceiveData), so
 * no RX interrupt / driver ring buffer needs to be armed on the chosen UART.
 */
#include <stdint.h>
#include <stdbool.h>
#include <FreeRTOS.h>
#include <task.h>
#include "zmodem.h"
#include "hal_uart.h"

#ifndef UART1_INDEX
#define UART1_INDEX 1
#endif

/* Bulk-drain the RX FIFO once, then dole out one byte per zm_recv() call. */
static int      s_uart_id    = UART1_INDEX;
static uint32_t s_timeout_ms = 30000;
static uint8_t  s_rx[256];
static uint16_t s_rx_len;
static uint16_t s_rx_pos;

void zm_port_set_uart(int uart_id)
{
    s_uart_id = uart_id;
    s_rx_len = 0;
    s_rx_pos = 0;
}

void zm_port_set_timeout_ms(uint32_t ms)
{
    s_timeout_ms = ms;
}

ZRESULT zm_recv(void)
{
    if (s_rx_pos >= s_rx_len) {
        TickType_t deadline = xTaskGetTickCount() + pdMS_TO_TICKS(s_timeout_ms);
        int n;

        while ((n = uart_read_available(s_uart_id, s_rx, sizeof(s_rx))) <= 0) {
            if ((int32_t)(xTaskGetTickCount() - deadline) >= 0)
                return CLOSED;          /* idle timeout -> abort transfer */
            taskYIELD();
        }
        s_rx_len = (uint16_t)n;
        s_rx_pos = 0;
    }

    return s_rx[s_rx_pos++];
}

ZRESULT zm_send(uint8_t chr)
{
    uart_write(s_uart_id, &chr, 1);
    return OK;
}
