/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * FreeRTOS portmacro.h surface for the SDK MAC software under Zephyr: its
 * GLOBAL_INT_DISABLE/RESTORE no longer touch the FreeRTOS nesting counter
 * (the increments are commented out in plf/refip ll.h), so these are no-ops.
 */
#ifndef IPRO_FREERTOS_SHIM_PORTMACRO_H
#define IPRO_FREERTOS_SHIM_PORTMACRO_H

#define portINCREMENT_CRITICAL_NESTING_COUNT() do { } while (0)
#define portDECREMENT_CRITICAL_NESTING_COUNT() do { } while (0)

#endif
