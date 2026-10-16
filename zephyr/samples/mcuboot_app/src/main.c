/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * Smallest application that proves the boot chain boot2 -> MCUboot ->
 * image-0: prints where it runs from, the image slot it was linked for and
 * the reset cause, then idles.
 */
#include <zephyr/kernel.h>
#include <zephyr/drivers/hwinfo.h>
#include <zephyr/storage/flash_map.h>
#include <zephyr/dfu/mcuboot.h>
#include <zephyr/sys/reboot.h>

/* The image version is what imgtool signed (sysbuild: SB_CONFIG_... or the
 * app's CONFIG_MCUBOOT_IMGTOOL_SIGN_VERSION). An upgrade image (version
 * 2.x) that was swapped in as a "test" confirms itself and reboots so the
 * bootloader keeps it; otherwise MCUboot would revert on the next boot. */
int main(void)
{
	uint32_t cause = 0;
	const struct flash_area *fa;
	bool confirmed = boot_is_img_confirmed();

	(void)hwinfo_get_reset_cause(&cause);
	printk("ipro mcuboot_app: booted by MCUboot, version %s, main at %p, reset cause 0x%x, %s\n",
	       CONFIG_MCUBOOT_IMGTOOL_SIGN_VERSION, main, cause,
	       confirmed ? "confirmed" : "test (not confirmed)");
	if (flash_area_open(PARTITION_ID(slot0_partition), &fa) == 0) {
		printk("slot0: flash offset 0x%lx size 0x%x (xip 0x%lx)\n", (unsigned long)fa->fa_off,
		       (unsigned)fa->fa_size, (unsigned long)(CONFIG_FLASH_BASE_ADDRESS + fa->fa_off));
		flash_area_close(fa);
	}
	if (!confirmed) {
		int rc = boot_write_img_confirmed();

		printk("confirming this image: %d, rebooting\n", rc);
		k_sleep(K_MSEC(200));
		sys_reboot(SYS_REBOOT_WARM);
	}
	while (1) {
		k_sleep(K_SECONDS(5));
		printk("mcuboot_app alive\n");
	}
	return 0;
}
