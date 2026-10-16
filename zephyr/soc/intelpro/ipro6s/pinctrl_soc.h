/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * pinctrl_soc_pin_t for IPRO6S: the same 32-bit encoding as IPRO7
 * (<zephyr/dt-bindings/pinctrl/ipro-pinctrl.h>), so the shared pinctrl
 * driver decodes both.
 */

#ifndef ZEPHYR_SOC_INTELPRO_IPRO6S_PINCTRL_SOC_H_
#define ZEPHYR_SOC_INTELPRO_IPRO6S_PINCTRL_SOC_H_

#include <zephyr/devicetree.h>
#include <zephyr/types.h>
#include <zephyr/dt-bindings/pinctrl/ipro-pinctrl.h>
#include <zephyr/dt-bindings/pinctrl/ipro6s-pinctrl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t pinctrl_soc_pin_t;

#define Z_PINCTRL_IPRO_PULL(node_id)                                                               \
	(DT_PROP(node_id, bias_pull_up) ? IPRO_PULL_UP                                             \
	 : DT_PROP(node_id, bias_pull_down) ? IPRO_PULL_DOWN                                       \
					    : IPRO_PULL_NONE)

#define Z_PINCTRL_STATE_PIN_INIT(node_id, prop, idx)                                              \
	(DT_PROP_BY_IDX(node_id, prop, idx) |                                                      \
	 (Z_PINCTRL_IPRO_PULL(node_id) << IPRO_PINCFG_PULL_POS) |                                  \
	 ((DT_PROP(node_id, drive_strength) & IPRO_PINCFG_DRIVE_MASK) << IPRO_PINCFG_DRIVE_POS) |    \
	 (DT_PROP(node_id, input_schmitt_enable) << IPRO_PINCFG_SMT_POS) |                         \
	 (DT_PROP(node_id, input_enable) << IPRO_PINCFG_INPUT_POS) |                               \
	 (DT_PROP(node_id, output_enable) << IPRO_PINCFG_OUTPUT_POS) |                             \
	 (DT_PROP(node_id, output_high) << IPRO_PINCFG_OUTHI_POS)),

#define Z_PINCTRL_STATE_PINS_INIT(node_id, prop)                                                  \
	{                                                                                          \
		DT_FOREACH_CHILD_VARGS(DT_PHANDLE(node_id, prop), DT_FOREACH_PROP_ELEM, pinmux,    \
				       Z_PINCTRL_STATE_PIN_INIT)                                   \
	}

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_SOC_INTELPRO_IPRO6S_PINCTRL_SOC_H_ */
