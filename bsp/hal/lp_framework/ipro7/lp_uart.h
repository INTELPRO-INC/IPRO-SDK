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
 * @file lp_uart.h
 * @brief Lightweight UART for LP firmware
 *
 * Minimal UART driver for LP firmware debug output.
 * Does not link full driver to keep code size small.
 */

#ifndef __LP_UART_H__
#define __LP_UART_H__

#include <stdint.h>

/*============================ UART API ===================================*/

/**
 * @brief Initialize UART for LP firmware (minimal init, assumes clock is ready)
 * @param uart_id UART ID (0 or 1)
 * @param baudrate Baud rate (e.g., 2000000)
 */
void lp_uart_init(int uart_id, uint32_t baudrate);

/**
 * @brief Send a single character
 * @param uart_id UART ID (0 or 1)
 * @param c Character to send
 */
void lp_uart_putc(int uart_id, char c);

/**
 * @brief Send a string
 * @param uart_id UART ID (0 or 1)
 * @param str String to send
 */
void lp_uart_puts(int uart_id, const char *str);

/**
 * @brief Send a hex value (for debug)
 * @param uart_id UART ID (0 or 1)
 * @param val Value to print in hex
 */
void lp_uart_puthex(int uart_id, uint32_t val);

/**
 * @brief Send a decimal value
 * @param uart_id UART ID (0 or 1)
 * @param val Value to print in decimal
 */
void lp_uart_putdec(int uart_id, uint32_t val);

/*============================ LP PRINT API ===============================*/

/**
 * LP Print API - uses UART0 by default for convenience.
 * These are lightweight alternatives to printf for LP firmware.
 */

/** @brief Print a string */
#define lp_print(str)           lp_uart_puts(0, str)

/** @brief Print a string with newline */
#define lp_println(str)         do { lp_uart_puts(0, str); lp_uart_puts(0, "\r\n"); } while(0)

/** @brief Print hex value with prefix "0x" */
#define lp_print_hex(val)       lp_uart_puthex(0, val)

/** @brief Print decimal value */
#define lp_print_dec(val)       lp_uart_putdec(0, val)

/** @brief Print a character */
#define lp_print_char(c)        lp_uart_putc(0, c)

/** @brief Print newline */
#define lp_print_nl()           lp_uart_puts(0, "\r\n")

/** @brief Print labeled hex: "label: 0x12345678\r\n" */
#define lp_print_reg(label, val) do { \
    lp_uart_puts(0, label); \
    lp_uart_puts(0, ": "); \
    lp_uart_puthex(0, val); \
    lp_uart_puts(0, "\r\n"); \
} while(0)

#endif /* __LP_UART_H__ */
