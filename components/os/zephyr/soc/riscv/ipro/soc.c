/* Copyright (c) 2025 IPRO
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/init.h>

/**
 * @brief Perform basic hardware initialization
 */
static int ipro7_init(void)
{
	return 0;
}

SYS_INIT(ipro7_init, PRE_KERNEL_1, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);
