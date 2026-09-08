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
 * early_print.h - Early boot UART output before platform UART is ready
 *
 * Chip-specific: only IPRO7 has a real implementation.
 * Other chips get empty stubs so callers compile unconditionally.
 *
 * IPRO7 implementation:
 *   Bootrom leaves UART1 configured (115200 baud) on GPIO 17.
 *   Remaps UART1 TX to GPIO 20 (board debug UART pin) and writes
 *   directly to the TX FIFO register — no driver dependencies.
 *
 * Usage:
 *   early_print_init();
 *   early_puts("[EP] stage\n");
 *   early_flush();
 */

#ifndef __EARLY_PRINT_H__
#define __EARLY_PRINT_H__

#include <stdint.h>
#include <generated/autoconf.h>

#if defined(CONFIG_IPRO7)

/* ---- IPRO7 register addresses ---- */
#define EP_GLB_BASE             0x30000000UL
#define EP_UART1_BASE           0x3000C000UL

/* GPIO 20 config register (CFGCTL10, lower 16 bits) */
#define EP_GPIO_CFGCTL10        (EP_GLB_BASE + 0x128)

/* UART signal routing: SIG_SEL_1 holds signals 8-11 */
#define EP_UART_SIG_SEL_1       (EP_GLB_BASE + 0xCC)

/* UART1 TX FIFO and status */
#define EP_UART1_STATUS         (EP_UART1_BASE + 0x30)
#define EP_UART1_FIFO_CFG1      (EP_UART1_BASE + 0x84)
#define EP_UART1_TX_FIFO        (EP_UART1_BASE + 0x88)

/* Bit definitions */
#define EP_UTX_BUS_BUSY         (1U << 0)
#define EP_TX_FIFO_CNT_MSK     0x1FU   /* bits [4:0] = available slots */

/* GPIO 20 config: FUNC_SEL=7(UART), SMT=1, PU=1, MODE=0(AF) */
#define EP_GPIO20_CFG_VAL       ((7U << 8) | (1U << 4) | (1U << 1))
#define EP_GPIO20_CFG_CLR       0x0000FFFFU

/* UART1_TXD function value for signal routing */
#define EP_UART1_TXD_FUN        6U
#define EP_SIG8_MSK             0xFU

#define EP_REG32(addr)  (*(volatile uint32_t *)(addr))

static inline void early_print_init(void)
{
    uint32_t val;

    /* Set GPIO 20 pinmux to UART function */
    val = EP_REG32(EP_GPIO_CFGCTL10);
    val &= ~EP_GPIO20_CFG_CLR;
    val |= EP_GPIO20_CFG_VAL;
    EP_REG32(EP_GPIO_CFGCTL10) = val;

    /* Route UART_SIG_8 (GPIO 20) to UART1_TXD */
    val = EP_REG32(EP_UART_SIG_SEL_1);
    val &= ~EP_SIG8_MSK;
    val |= EP_UART1_TXD_FUN;
    EP_REG32(EP_UART_SIG_SEL_1) = val;
}

static inline void early_putc(char c)
{
    while ((EP_REG32(EP_UART1_FIFO_CFG1) & EP_TX_FIFO_CNT_MSK) == 0)
        ;
    EP_REG32(EP_UART1_TX_FIFO) = (uint32_t)(unsigned char)c;
}

static inline void early_puts(const char *s)
{
    while (*s) {
        if (*s == '\n')
            early_putc('\r');
        early_putc(*s++);
    }
}

static inline void early_flush(void)
{
    while (EP_REG32(EP_UART1_STATUS) & EP_UTX_BUS_BUSY)
        ;
}

static inline void early_print_hex4(uint8_t nibble)
{
    nibble &= 0xF;
    early_putc(nibble < 10 ? '0' + nibble : 'a' + nibble - 10);
}

static inline void early_print_hex32(uint32_t val)
{
    early_puts("0x");
    for (int i = 28; i >= 0; i -= 4)
        early_print_hex4((uint8_t)(val >> i));
}

#else /* !CONFIG_IPRO7 */

/* Empty stubs for non-IPRO7 chips */
static inline void early_print_init(void) {}
static inline void early_putc(char c) { (void)c; }
static inline void early_puts(const char *s) { (void)s; }
static inline void early_flush(void) {}
static inline void early_print_hex4(uint8_t nibble) { (void)nibble; }
static inline void early_print_hex32(uint32_t val) { (void)val; }

#endif /* CONFIG_IPRO7 */

#endif /* __EARLY_PRINT_H__ */
