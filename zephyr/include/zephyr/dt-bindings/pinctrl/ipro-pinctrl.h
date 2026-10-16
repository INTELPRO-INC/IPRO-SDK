/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * Chip-neutral pinmux word shared by the IPRO7 and IPRO6S ports:
 *   [5:0]   pad number       [12:8] GLB function code (chip specific)
 *   [19:16] UART signal      [21:20] pad mode
 *   [23:22] pull  [25:24] drive  [26] schmitt  [27] input  [28] output  [29] output-high
 * The upper bits are filled in by pinctrl_soc.h from the pin properties.
 */

#ifndef ZEPHYR_INCLUDE_DT_BINDINGS_PINCTRL_IPRO_PINCTRL_H_
#define ZEPHYR_INCLUDE_DT_BINDINGS_PINCTRL_IPRO_PINCTRL_H_

#define IPRO_PIN_POS      0
#define IPRO_PIN_MASK     0x3F
#define IPRO_FUN_POS      8
#define IPRO_FUN_MASK     0x1F
#define IPRO_UARTSIG_POS  16
#define IPRO_UARTSIG_MASK 0xF
#define IPRO_MODE_POS     20
#define IPRO_MODE_MASK    0x3

#define IPRO_PINCFG_PULL_POS   22
#define IPRO_PINCFG_PULL_MASK  0x3
#define IPRO_PINCFG_DRIVE_POS  24
#define IPRO_PINCFG_DRIVE_MASK 0x3
#define IPRO_PINCFG_SMT_POS    26
#define IPRO_PINCFG_INPUT_POS  27
#define IPRO_PINCFG_OUTPUT_POS 28
#define IPRO_PINCFG_OUTHI_POS  29

/* Pull encoding = the SDK's GPIO_PULL_* values on both chips. */
#define IPRO_PULL_UP   0
#define IPRO_PULL_DOWN 1
#define IPRO_PULL_NONE 2

#define IPRO_MODE_INPUT  0
#define IPRO_MODE_OUTPUT 1
#define IPRO_MODE_AF     2
#define IPRO_MODE_ANALOG 3

/* GLB_UART_SIG_FUN_* (identical on IPRO7 and IPRO6). */
#define IPRO_UART_SIG_UART0_RTS 0
#define IPRO_UART_SIG_UART0_CTS 1
#define IPRO_UART_SIG_UART0_TXD 2
#define IPRO_UART_SIG_UART0_RXD 3
#define IPRO_UART_SIG_UART1_RTS 4
#define IPRO_UART_SIG_UART1_CTS 5
#define IPRO_UART_SIG_UART1_TXD 6
#define IPRO_UART_SIG_UART1_RXD 7
#define IPRO_UART_SIG_NONE      0xF

#define IPRO_PINMUX_RAW(pin, fun, sig, mode)                                                       \
	((((pin) & IPRO_PIN_MASK) << IPRO_PIN_POS) | (((fun) & IPRO_FUN_MASK) << IPRO_FUN_POS) |      \
	 (((sig) & IPRO_UARTSIG_MASK) << IPRO_UARTSIG_POS) |                                        \
	 (((mode) & IPRO_MODE_MASK) << IPRO_MODE_POS))

#define IPRO_PINMUX_PIN(mux)     (((mux) >> IPRO_PIN_POS) & IPRO_PIN_MASK)
#define IPRO_PINMUX_FUN(mux)     (((mux) >> IPRO_FUN_POS) & IPRO_FUN_MASK)
#define IPRO_PINMUX_UARTSIG(mux) (((mux) >> IPRO_UARTSIG_POS) & IPRO_UARTSIG_MASK)
#define IPRO_PINMUX_MODE(mux)    (((mux) >> IPRO_MODE_POS) & IPRO_MODE_MASK)

#endif /* ZEPHYR_INCLUDE_DT_BINDINGS_PINCTRL_IPRO_PINCTRL_H_ */
