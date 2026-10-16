/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * Flash facade over the SDK HAL (hal_flash.c / hal_boot2_ipro7.c). Pure SDK
 * side: hal_flash.h's flash_read/write/erase share their names with Zephyr's
 * flash API.
 */

#include <ipro_zephyr/ipro_hal_glue.h>

#include "hal_flash.h"
#include "hal_boot2.h"
#ifdef CONFIG_SOC_SERIES_IPRO6S
#include <string.h>
#include "ipro6s_flash_default_cfg.h"
#endif

int ipro_hal_flash_init(void)
{
	/* Partition table + flash geometry from the boot2 hand-off, then
	 * flash_init() to refresh the g_flash_cfg the XIP helpers use. */
	int ret = hal_boot2_init();

	if (ret != 0) {
		/* No boot2 (BootROM-direct image, e.g. the IPRO6S border-router
		 * layout): hal_boot2_init() skipped flash_init(); detect the part
		 * by JEDEC id instead. */
#ifdef CONFIG_SOC_SERIES_IPRO6S
		/*
		 * flash_init()'s detection issues the JEDEC-ID read with whatever
		 * g_flash_cfg holds - all zeros on a cold start, which wedges the
		 * SF controller (observed on the EVB). Seed it with a generic
		 * SPI NOR configuration first; the real one is then picked from
		 * the ROM's table by id.
		 */
		uint8_t *cfg_addr = NULL;
		uint32_t cfg_len = 0;

		if (flash_get_cfg(&cfg_addr, &cfg_len) == 0 && cfg_addr != NULL &&
		    cfg_len == sizeof(ipro6s_flash_cfg_default)) {
			memcpy(cfg_addr, &ipro6s_flash_cfg_default, cfg_len);
		}
#endif
		return flash_init() == 0 ? 0 : ret;
	}
	return 0;
}

uint32_t ipro_hal_flash_size(void)
{
	return flash_get_size();
}

int ipro_hal_flash_read(uint32_t addr, void *buf, size_t len)
{
	return flash_read(addr, buf, len);
}

int ipro_hal_flash_write(uint32_t addr, const void *buf, size_t len)
{
	return flash_write(addr, (uint8_t *)buf, len);
}

int ipro_hal_flash_erase(uint32_t addr, size_t len)
{
	/* hal_flash: len is a byte count, erases [addr, addr + len - 1]. */
	return flash_erase(addr, len);
}
