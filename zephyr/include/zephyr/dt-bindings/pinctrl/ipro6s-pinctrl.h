/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * IPRO6S pad function codes (bsp/drivers/ipro6_soc/std_drv/inc/drv_gpio.h)
 * on the chip-neutral pinmux word from ipro-pinctrl.h. 35 pads (0-34).
 */

#ifndef ZEPHYR_INCLUDE_DT_BINDINGS_PINCTRL_IPRO6S_PINCTRL_H_
#define ZEPHYR_INCLUDE_DT_BINDINGS_PINCTRL_IPRO6S_PINCTRL_H_

#include <zephyr/dt-bindings/pinctrl/ipro-pinctrl.h>

#define IPRO6S_FUN_SDH        0
#define IPRO6S_FUN_SPI        1
#define IPRO6S_FUN_FLASH      2
#define IPRO6S_FUN_I2S        3
#define IPRO6S_FUN_PDM        4
#define IPRO6S_FUN_I2C0       5
#define IPRO6S_FUN_I2C1       6
#define IPRO6S_FUN_UART       7
#define IPRO6S_FUN_ETHER_MAC  8
#define IPRO6S_FUN_CAM        9
#define IPRO6S_FUN_ANALOG     10
#define IPRO6S_FUN_GPIO       11
#define IPRO6S_FUN_SDIO       12
#define IPRO6S_FUN_PWM0       16
#define IPRO6S_FUN_DBI_B      22
#define IPRO6S_FUN_DBI_C      23
#define IPRO6S_FUN_DISP_QSPI  24
#define IPRO6S_FUN_AUPWM      25
#define IPRO6S_FUN_JTAG       26
#define IPRO6S_FUN_PEC        27
#define IPRO6S_FUN_CLOCK_OUT  31

#define IPRO6S_PINMUX(pin, fun) IPRO_PINMUX_RAW(pin, fun, IPRO_UART_SIG_NONE, IPRO_MODE_AF)
#define IPRO6S_PINMUX_UART(pin, sig)                                                               \
	IPRO_PINMUX_RAW(pin, IPRO6S_FUN_UART, IPRO_UART_SIG_##sig, IPRO_MODE_AF)
#define IPRO6S_PINMUX_GPIO(pin)                                                                    \
	IPRO_PINMUX_RAW(pin, IPRO6S_FUN_GPIO, IPRO_UART_SIG_NONE, IPRO_MODE_INPUT)
#define IPRO6S_PINMUX_ANALOG(pin)                                                                  \
	IPRO_PINMUX_RAW(pin, IPRO6S_FUN_ANALOG, IPRO_UART_SIG_NONE, IPRO_MODE_ANALOG)

#endif /* ZEPHYR_INCLUDE_DT_BINDINGS_PINCTRL_IPRO6S_PINCTRL_H_ */
