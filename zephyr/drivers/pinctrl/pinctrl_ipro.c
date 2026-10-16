/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * pinctrl adapter: each pinctrl_soc_pin_t word becomes one GLB_GPIO_Init()
 * call (exactly what BOARD_PINMUX_INIT() does in the SDK board files) and,
 * for UART pads, one GLB_UART_Fun_Sel() for the pad's signal slot
 * (pad % 12, e.g. GPIO20 -> GLB_UART_SIG_8). Shared by IPRO7 and IPRO6S;
 * the pinmux word is chip-neutral (dt-bindings/pinctrl/ipro-pinctrl.h) and
 * the function codes come from the chip's header.
 */

#include <zephyr/drivers/pinctrl.h>
#include <zephyr/sys/util.h>
#include <zephyr/dt-bindings/pinctrl/ipro-pinctrl.h>
#include <soc.h>

#include "drv_glb.h"
#include "drv_gpio.h"
#ifdef CONFIG_SOC_SERIES_IPRO6S
#include "drv_glb_gpio.h"
#endif

/* GLB_UART_CFG1/2: 12 four-bit UART signal slots on both chips. */
#define IPRO_UART_SIG_SLOTS 12

static int ipro_pinctrl_apply(pinctrl_soc_pin_t pin)
{
	uint8_t pad = IPRO_PINMUX_PIN(pin);
	uint8_t uart_sig = IPRO_PINMUX_UARTSIG(pin);
	GLB_GPIO_Cfg_Type cfg = {
		.gpioPin = pad,
		.gpioFun = IPRO_PINMUX_FUN(pin),
		.gpioMode = IPRO_PINMUX_MODE(pin),
		.pullType = (pin >> IPRO_PINCFG_PULL_POS) & IPRO_PINCFG_PULL_MASK,
		.drive = (pin >> IPRO_PINCFG_DRIVE_POS) & IPRO_PINCFG_DRIVE_MASK,
		.smtCtrl = (pin >> IPRO_PINCFG_SMT_POS) & 1,
	};

	if (pad >= GLB_GPIO_PIN_MAX) {
		return -EINVAL;
	}

	if (pin & BIT(IPRO_PINCFG_OUTPUT_POS)) {
		cfg.gpioMode = GPIO_MODE_OUTPUT;
	} else if (pin & BIT(IPRO_PINCFG_INPUT_POS)) {
		cfg.gpioMode = GPIO_MODE_INPUT;
	}

	if (GLB_GPIO_Init(&cfg) != 0) {
		return -EIO;
	}

	if (cfg.gpioMode == GPIO_MODE_OUTPUT) {
		GLB_GPIO_Write(pad, (pin >> IPRO_PINCFG_OUTHI_POS) & 1);
#ifdef CONFIG_SOC_SERIES_IPRO6S
		GLB_GPIO_Output_Enable(pad);
#else
		GLB_GPIO_OUTPUT_Enable(pad);
#endif
	}

	if (uart_sig != IPRO_UART_SIG_NONE) {
#ifdef CONFIG_SOC_SERIES_IPRO6S
		/* _Swap keeps every UART function on exactly one slot (the ROM
		 * leaves its own console mapping behind), as board.c does. */
		if (GLB_UART_Fun_Sel_Swap(pad % IPRO_UART_SIG_SLOTS, uart_sig) != 0) {
			return -EIO;
		}
#else
		if (GLB_UART_Fun_Sel((GLB_UART_SIG_Type)(pad % IPRO_UART_SIG_SLOTS),
				     (GLB_UART_SIG_FUN_Type)uart_sig) != 0) {
			return -EIO;
		}
#endif
	}

	return 0;
}

int pinctrl_configure_pins(const pinctrl_soc_pin_t *pins, uint8_t pin_cnt, uintptr_t reg)
{
	ARG_UNUSED(reg);

	for (uint8_t i = 0; i < pin_cnt; i++) {
		int ret = ipro_pinctrl_apply(pins[i]);

		if (ret != 0) {
			return ret;
		}
	}
	return 0;
}
