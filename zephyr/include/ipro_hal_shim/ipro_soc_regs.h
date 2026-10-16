/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * The SDK's register/IRQ map header for the SoC being built (ipro7.h or
 * ipro6.h). Shared Zephyr drivers include this instead of naming the chip.
 */

#ifndef IPRO_HAL_SHIM_IPRO_SOC_REGS_H_
#define IPRO_HAL_SHIM_IPRO_SOC_REGS_H_

#if defined(CONFIG_SOC_SERIES_IPRO6S)
#include "ipro6.h"
#elif defined(CONFIG_SOC_SERIES_IPRO7)
#include "ipro7.h"
#else
#error "unsupported INTELPRO SoC series"
#endif

#endif /* IPRO_HAL_SHIM_IPRO_SOC_REGS_H_ */
