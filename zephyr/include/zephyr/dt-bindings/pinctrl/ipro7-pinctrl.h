/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * IPRO7 pinmux encoding for device tree.
 *
 *   [5:0]   pad number (0-41)
 *   [12:8]  GLB alternate function (GPIO_FUN_* in bsp/.../drv_gpio.h)
 *   [19:16] UART signal function for the pad's slot, IPRO7_UART_SIG_NONE
 *           for non-UART pads
 *   [21:20] pad mode: 0 input, 1 output, 2 alternate function, 3 analog
 */

#ifndef ZEPHYR_INCLUDE_DT_BINDINGS_PINCTRL_IPRO7_PINCTRL_H_
#define ZEPHYR_INCLUDE_DT_BINDINGS_PINCTRL_IPRO7_PINCTRL_H_

#include <zephyr/dt-bindings/pinctrl/ipro-pinctrl.h>

#define IPRO7_PIN_POS      0
#define IPRO7_PIN_MASK     0x3F
#define IPRO7_FUN_POS      8
#define IPRO7_FUN_MASK     0x1F
#define IPRO7_UARTSIG_POS  16
#define IPRO7_UARTSIG_MASK 0xF
#define IPRO7_MODE_POS     20
#define IPRO7_MODE_MASK    0x3

/* GLB alternate functions (GLB_GPIO_FUNC_Type) */
#define IPRO7_FUN_CLK_OUT        0
#define IPRO7_FUN_BT_COEXIST     1
#define IPRO7_FUN_FLASH_PSRAM    2
#define IPRO7_FUN_I2S            3
#define IPRO7_FUN_SPI0           4
#define IPRO7_FUN_PDM            5
#define IPRO7_FUN_I2C0           6
#define IPRO7_FUN_UART           7
#define IPRO7_FUN_PWM            8
#define IPRO7_FUN_CAM            9
#define IPRO7_FUN_ANALOG         10
#define IPRO7_FUN_GPIO           11
#define IPRO7_FUN_RF_TEST        12
#define IPRO7_FUN_SCAN           13
#define IPRO7_FUN_MCU_JTAG       14
#define IPRO7_FUN_DEBUG          15
#define IPRO7_FUN_EXTERNAL_PA    16
#define IPRO7_FUN_USB_TRANCEIVER 17
#define IPRO7_FUN_USB_CONTROLLER 18
#define IPRO7_FUN_EMAC           19
#define IPRO7_FUN_QDEC           20
#define IPRO7_FUN_KEY_SCAN_ROW   21
#define IPRO7_FUN_KEY_SCAN_COL   22
#define IPRO7_FUN_CAM_MISC       23
#define IPRO7_FUN_PIO            24
#define IPRO7_FUN_I2C1           25
#define IPRO7_FUN_SPI1           26
#define IPRO7_FUN_SDC            27

/* Pad modes (GPIO_MODE_*) */
#define IPRO7_MODE_INPUT  0
#define IPRO7_MODE_OUTPUT 1
#define IPRO7_MODE_AF     2
#define IPRO7_MODE_ANALOG 3

/* UART signal functions (GLB_UART_SIG_FUN_Type) */
#define IPRO7_UART_SIG_UART0_RTS 0
#define IPRO7_UART_SIG_UART0_CTS 1
#define IPRO7_UART_SIG_UART0_TXD 2
#define IPRO7_UART_SIG_UART0_RXD 3
#define IPRO7_UART_SIG_UART1_RTS 4
#define IPRO7_UART_SIG_UART1_CTS 5
#define IPRO7_UART_SIG_UART1_TXD 6
#define IPRO7_UART_SIG_UART1_RXD 7
#define IPRO7_UART_SIG_NONE      0xF

#define IPRO7_PINMUX_RAW(pin, fun, sig, mode)                                                   \
	((((pin) & IPRO7_PIN_MASK) << IPRO7_PIN_POS) | (((fun) & IPRO7_FUN_MASK) << IPRO7_FUN_POS) |  \
	 (((sig) & IPRO7_UARTSIG_MASK) << IPRO7_UARTSIG_POS) |                                     \
	 (((mode) & IPRO7_MODE_MASK) << IPRO7_MODE_POS))

/** Generic alternate-function pad: IPRO7_PINMUX(20, IPRO7_FUN_SPI0) */
#define IPRO7_PINMUX(pin, fun) IPRO7_PINMUX_RAW(pin, fun, IPRO7_UART_SIG_NONE, IPRO7_MODE_AF)

/** UART pad: IPRO7_PINMUX_UART(20, UART0_TXD) */
#define IPRO7_PINMUX_UART(pin, sig)                                                             \
	IPRO7_PINMUX_RAW(pin, IPRO7_FUN_UART, IPRO7_UART_SIG_##sig, IPRO7_MODE_AF)

/** Plain GPIO pad handed to the gpio driver (function 11, input by default) */
#define IPRO7_PINMUX_GPIO(pin) IPRO7_PINMUX_RAW(pin, IPRO7_FUN_GPIO, IPRO7_UART_SIG_NONE, IPRO7_MODE_INPUT)

/** Analog pad (ADC/DAC) */
#define IPRO7_PINMUX_ANALOG(pin)                                                                \
	IPRO7_PINMUX_RAW(pin, IPRO7_FUN_ANALOG, IPRO7_UART_SIG_NONE, IPRO7_MODE_ANALOG)

#define IPRO7_PINMUX_PIN(mux)     (((mux) >> IPRO7_PIN_POS) & IPRO7_PIN_MASK)
#define IPRO7_PINMUX_FUN(mux)     (((mux) >> IPRO7_FUN_POS) & IPRO7_FUN_MASK)
#define IPRO7_PINMUX_UARTSIG(mux) (((mux) >> IPRO7_UARTSIG_POS) & IPRO7_UARTSIG_MASK)
#define IPRO7_PINMUX_MODE(mux)    (((mux) >> IPRO7_MODE_POS) & IPRO7_MODE_MASK)

#endif /* ZEPHYR_INCLUDE_DT_BINDINGS_PINCTRL_IPRO7_PINCTRL_H_ */
