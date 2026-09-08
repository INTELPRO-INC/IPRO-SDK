/*
 * Copyright (c) 2025 IPRO
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @brief Linker script for the IPRO7 processor
 */

#include <zephyr/devicetree.h>

#define ROM_BASE DT_REG_ADDR(DT_CHOSEN(zephyr_flash))
#define ROM_SIZE DT_REG_SIZE(DT_CHOSEN(zephyr_flash))

#include <zephyr/arch/riscv/common/linker.ld>
