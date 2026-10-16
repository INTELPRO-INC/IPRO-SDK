/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * Facade over the prebuilt IPRO7 BLE controller (arch_main.c + hcitl).
 * Implemented in zephyr/hal_glue/ipro_bt_ctlr_glue.c, the only file that
 * includes the controller headers.
 */

#ifndef IPRO_ZEPHYR_IPRO_BT_CTLR_H_
#define IPRO_ZEPHYR_IPRO_BT_CTLR_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Bring the controller up: RF, clocks, LL reset, controller thread. */
int ipro_bt_ctlr_start(unsigned int task_prio, void (*rx_notify)(void *ctx), void *ctx);

/** Non-blocking H4 stream access (0 = nothing transferred). */
uint32_t ipro_bt_ctlr_hci_write(const uint8_t *data, uint32_t len);
uint32_t ipro_bt_ctlr_hci_read(uint8_t *out, uint32_t max);

#ifdef __cplusplus
}
#endif

#endif /* IPRO_ZEPHYR_IPRO_BT_CTLR_H_ */
