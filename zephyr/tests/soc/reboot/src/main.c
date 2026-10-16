/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * sys_reboot() + hwinfo: the first boot after flashing records a marker in
 * HBN RAM (.hbn_noinit - OCRAM does not survive a reboot because boot2 runs
 * from it) and reboots; the second boot must see the marker, report
 * RESET_SOFTWARE as the cause and a non-zero eFuse device id. The marker is
 * cleared so a third boot starts the cycle again. Both sys_reboot() types
 * are exercised (on IPRO7 both take the POR-class reset).
 */
#include <zephyr/ztest.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/drivers/hwinfo.h>

#define MARKER_WARM 0x5EB007ED
#define MARKER_COLD 0xC01DB007

static uint32_t reboot_marker __attribute__((section(".hbn_noinit")));

ZTEST(ipro_reboot, test_reboot_and_cause)
{
	uint32_t cause = 0, supported = 0;
	uint8_t id[8] = {0};
	ssize_t n;

	zassert_ok(hwinfo_get_supported_reset_cause(&supported));
	zassert_true(supported & RESET_SOFTWARE);
	zassert_ok(hwinfo_get_reset_cause(&cause));

	n = hwinfo_get_device_id(id, sizeof(id));
	zassert_equal(n, 8, "device id length %d", (int)n);
	zassert_true(id[0] | id[1] | id[2] | id[3] | id[4] | id[5] | id[6] | id[7],
		     "device id all zero");

	if (reboot_marker != MARKER_WARM && reboot_marker != MARKER_COLD) {
		reboot_marker = MARKER_WARM;
		printk("first boot (cause 0x%x): warm reboot\n", cause);
		k_msleep(50); /* let the line leave the UART */
		sys_reboot(SYS_REBOOT_WARM);
		zassert_unreachable("sys_reboot returned");
	}

	printk("boot after %s reset: cause 0x%x id %02x%02x%02x%02x%02x%02x%02x%02x\n",
	       reboot_marker == MARKER_WARM ? "warm" : "cold", cause, id[0], id[1], id[2], id[3],
	       id[4], id[5], id[6], id[7]);
	zassert_equal(cause, RESET_SOFTWARE, "expected RESET_SOFTWARE, got 0x%x", cause);

	if (reboot_marker == MARKER_WARM) {
		reboot_marker = MARKER_COLD;
		printk("cold reboot\n");
		k_msleep(50);
		sys_reboot(SYS_REBOOT_COLD);
		zassert_unreachable("sys_reboot returned");
	}

	reboot_marker = 0;
	zassert_ok(hwinfo_clear_reset_cause());
}

ZTEST_SUITE(ipro_reboot, NULL, NULL, NULL, NULL, NULL);
