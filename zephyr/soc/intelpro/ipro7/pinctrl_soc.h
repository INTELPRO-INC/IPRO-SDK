/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * pinctrl_soc.h for the IPRO7: one 32-bit word per pin.
 *
 *   [21:0]  IPRO7_PINMUX_RAW() from the "pinmux" property
 *   [23:22] pull   (0 up, 1 down, 2 none — GPIO_PULL_* encoding)
 *   [25:24] drive strength
 *   [26]    Schmitt trigger
 *   [27]    input enable  (GPIO_MODE_INPUT override)
 *   [28]    output enable (GPIO_MODE_OUTPUT override)
 *   [29]    output-high initial level
 */

#ifndef ZEPHYR_SOC_INTELPRO_IPRO7_PINCTRL_SOC_H_
#define ZEPHYR_SOC_INTELPRO_IPRO7_PINCTRL_SOC_H_

#include <zephyr/devicetree.h>
#include <zephyr/types.h>
#include <zephyr/dt-bindings/pinctrl/ipro-pinctrl.h>
#include <zephyr/dt-bindings/pinctrl/ipro7-pinctrl.h>

#ifdef __cplusplus
extern "C" {
#endif



typedef uint32_t pinctrl_soc_pin_t;

#define Z_PINCTRL_IPRO_PULL(node_id)                                                              \
	(DT_PROP(node_id, bias_pull_up) ? IPRO_PULL_UP                                            \
	 : DT_PROP(node_id, bias_pull_down) ? IPRO_PULL_DOWN                                       \
					    : IPRO_PULL_NONE)

#define Z_PINCTRL_STATE_PIN_INIT(node_id, prop, idx)                                              \
	(DT_PROP_BY_IDX(node_id, prop, idx) |                                                      \
	 (Z_PINCTRL_IPRO_PULL(node_id) << IPRO_PINCFG_PULL_POS) |                                \
	 ((DT_PROP(node_id, drive_strength) & IPRO_PINCFG_DRIVE_MASK) << IPRO_PINCFG_DRIVE_POS) |  \
	 (DT_PROP(node_id, input_schmitt_enable) << IPRO_PINCFG_SMT_POS) |                        \
	 (DT_PROP(node_id, input_enable) << IPRO_PINCFG_INPUT_POS) |                              \
	 (DT_PROP(node_id, output_enable) << IPRO_PINCFG_OUTPUT_POS) |                            \
	 (DT_PROP(node_id, output_high) << IPRO_PINCFG_OUTHI_POS)),

#define Z_PINCTRL_STATE_PINS_INIT(node_id, prop)                                                  \
	{                                                                                          \
		DT_FOREACH_CHILD_VARGS(DT_PHANDLE(node_id, prop), DT_FOREACH_PROP_ELEM, pinmux,    \
				       Z_PINCTRL_STATE_PIN_INIT)                                   \
	}

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_SOC_INTELPRO_IPRO7_PINCTRL_SOC_H_ */
