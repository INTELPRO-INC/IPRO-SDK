/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * hwinfo for IPRO7: device id = the 8-byte eFuse chip id, reset cause from
 * the SDK's boot-time detection (hal_rst_reason_detect() runs in
 * soc_early_init_hook(); it reads the WDT status, the PDS wake flag and the
 * reason a previous sys_reboot()/exception left in HBN RAM).
 */

#include <zephyr/drivers/hwinfo.h>
#include <zephyr/kernel.h>
#include <string.h>
#include <soc.h>

#include "ipro_soc_regs.h"
#include "hal_sys.h"
#include "drv_ef_ctrl.h"

#define IPRO_CHIP_ID_LEN 8

ssize_t z_impl_hwinfo_get_device_id(uint8_t *buffer, size_t length)
{
	uint8_t id[IPRO_CHIP_ID_LEN];

	if (EF_Ctrl_Read_Chip_ID(id) != 0) {
		return -EIO;
	}
	if (length > sizeof(id)) {
		length = sizeof(id);
	}
	memcpy(buffer, id, length);
	return (ssize_t)length;
}

static uint32_t ipro_reset_cause(hal_rst_reason_t reason)
{
	switch (reason) {
	case HAL_RST_REASON_POWER_ON:
		return RESET_POR;
	case HAL_RST_REASON_WDT:
	case HAL_RST_REASON_AON_WDT:
		return RESET_WATCHDOG;
	case HAL_RST_REASON_EXCEPTION:
		return RESET_CPU_LOCKUP;
	case HAL_RST_REASON_SOFTWARE:
		return RESET_SOFTWARE;
	case HAL_RST_REASON_LP_WAKEUP:
		return RESET_LOW_POWER_WAKE;
	default:
		return 0;
	}
}

int z_impl_hwinfo_get_reset_cause(uint32_t *cause)
{
	*cause = ipro_reset_cause(hal_rst_reason_get());
	return 0;
}

int z_impl_hwinfo_clear_reset_cause(void)
{
	/* The detected value lives in RAM for this boot; the HBN copy that
	 * seeds the next detection is what a caller wants cleared. */
	hal_rst_reason_set(HAL_RST_REASON_UNKNOWN);
	return 0;
}

int z_impl_hwinfo_get_supported_reset_cause(uint32_t *supported)
{
	*supported = RESET_POR | RESET_WATCHDOG | RESET_CPU_LOCKUP | RESET_SOFTWARE |
		     RESET_LOW_POWER_WAKE;
	return 0;
}
