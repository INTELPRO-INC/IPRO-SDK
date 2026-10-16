/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * _sp for the prebuilt BLE controller.
 *
 * The archives are built from the controller verifier's configuration, which
 * enables a bench wake-trace diagnostic (CONFIG_HAL_LP_WAKE_MARKER): the
 * controller loop compares the stack pointer with _sp, the top of the ISR
 * stack in the FreeRTOS SDK linker script. Zephyr has no such symbol and the
 * link fails without it. Give it the same meaning here; the controller runs
 * in a Zephyr thread, so the diagnostic never fires.
 */
PROVIDE(_sp = z_interrupt_stacks + CONFIG_ISR_STACK_SIZE);
