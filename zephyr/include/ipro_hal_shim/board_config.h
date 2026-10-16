/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * board_config.h as seen by the SDK HAL when it is compiled for Zephyr.
 *
 * In the FreeRTOS SDK this header selects one of the bsp/board/ipro7/<board>_board_config.h
 * files (pin mux tables, clock macros, UART routing) from CONFIG_EVB & co.
 * Under Zephyr that information comes from the device tree, so only the
 * peripheral default descriptors (UART0_CONFIG, SPI0_CONFIG, ...) that the
 * HAL device tables are initialised from are pulled in.
 */

#ifndef IPRO_ZEPHYR_BOARD_CONFIG_SHIM_H_
#define IPRO_ZEPHYR_BOARD_CONFIG_SHIM_H_

#include "../../../bsp/board/ipro7/peripheral_config.h"

#endif /* IPRO_ZEPHYR_BOARD_CONFIG_SHIM_H_ */
