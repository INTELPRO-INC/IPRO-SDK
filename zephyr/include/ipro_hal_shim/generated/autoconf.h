/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * The SDK includes <generated/autoconf.h> (Linux-kernel style). Zephyr
 * already force-includes its own autoconf.h, which carries the CONFIG_IPRO7
 * / CONFIG_USE_PSRAM / ... symbols mirrored by zephyr/Kconfig.hal.
 */
#ifndef IPRO_ZEPHYR_AUTOCONF_SHIM_H_
#define IPRO_ZEPHYR_AUTOCONF_SHIM_H_
#include <zephyr/autoconf.h>
#endif
