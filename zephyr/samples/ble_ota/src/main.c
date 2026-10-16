/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * BLE OTA: advertise the MCUmgr SMP service, let a phone / mcumgr client
 * upload a signed image into slot1 and reset; MCUboot swaps it in (swap-
 * move), and this image confirms itself on its first boot so the
 * bootloader keeps it (otherwise MCUboot reverts on the next reset).
 * A heartbeat prints the running version so an upgrade is visible on the
 * console.
 */
#include <zephyr/kernel.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/mgmt/mcumgr/transport/smp_bt.h>
#include <zephyr/dfu/mcuboot.h>
#include <zephyr/drivers/hwinfo.h>
#include <zephyr/storage/flash_map.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/logging/log.h>
#include <stdlib.h>
#include <string.h>

LOG_MODULE_REGISTER(ble_ota, CONFIG_LOG_DEFAULT_LEVEL);

static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA_BYTES(BT_DATA_UUID128_ALL, SMP_BT_SVC_UUID_VAL),
};

static const struct bt_data sd[] = {
	BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME, sizeof(CONFIG_BT_DEVICE_NAME) - 1),
};

static struct k_work advertise_work;

static void advertise(struct k_work *work)
{
	ARG_UNUSED(work);
	int rc = bt_le_adv_start(BT_LE_ADV_CONN_FAST_1, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));

	if (rc != 0 && rc != -EALREADY) {
		LOG_ERR("advertising: %d", rc);
		return;
	}
	LOG_INF("advertising as \"%s\" (SMP service)", CONFIG_BT_DEVICE_NAME);
}

static void connected(struct bt_conn *conn, uint8_t err)
{
	ARG_UNUSED(conn);
	if (err != 0) {
		LOG_ERR("connection failed: 0x%02x", err);
		k_work_submit(&advertise_work);
		return;
	}
	LOG_INF("connected");
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
	ARG_UNUSED(conn);
	LOG_INF("disconnected (0x%02x)", reason);
}

static void recycled(void)
{
	k_work_submit(&advertise_work);
}

BT_CONN_CB_DEFINE(conn_cbs) = {
	.connected = connected,
	.disconnected = disconnected,
	.recycled = recycled,
};

static void confirm_if_needed(void)
{
	if (boot_is_img_confirmed()) {
		return;
	}
	int rc = boot_write_img_confirmed();

	LOG_INF("first boot of this image: confirmed (%d)", rc);
}

int main(void)
{
	uint32_t cause = 0;
	const struct flash_area *fa;
	int rc;

	(void)hwinfo_get_reset_cause(&cause);
	LOG_INF("IPRO7 BLE OTA, image version %s, reset cause 0x%x",
		CONFIG_MCUBOOT_IMGTOOL_SIGN_VERSION, cause);
	if (flash_area_open(PARTITION_ID(slot0_partition), &fa) == 0) {
		LOG_INF("slot0 at flash 0x%lx (%u KB), slot1 at 0x%lx", (unsigned long)fa->fa_off,
			(unsigned)fa->fa_size / 1024,
			(unsigned long)PARTITION_OFFSET(slot1_partition));
		flash_area_close(fa);
	}
	confirm_if_needed();

	k_work_init(&advertise_work, advertise);
	rc = bt_enable(NULL);
	if (rc != 0) {
		LOG_ERR("bt_enable: %d", rc);
		return 0;
	}
	k_work_submit(&advertise_work);

	while (true) {
		k_sleep(K_SECONDS(60));
		LOG_INF("alive, version %s", CONFIG_MCUBOOT_IMGTOOL_SIGN_VERSION);
	}
	return 0;
}

#ifdef CONFIG_SHELL
#include <zephyr/shell/shell.h>

/* `ota selftest [kb]`: erase the start of slot1 and write/verify kb KB in
 * 512-byte pieces, the way img_mgmt's stream_flash does, while BLE runs. */
static int cmd_ota_selftest(const struct shell *sh, size_t argc, char **argv)
{
	const struct flash_area *fa;
	uint32_t kb = argc > 1 ? strtoul(argv[1], NULL, 0) : 8;
	static uint8_t buf[512], back[512];
	int rc = flash_area_open(PARTITION_ID(slot1_partition), &fa);

	if (rc != 0) {
		shell_print(sh, "slot1 open: %d", rc);
		return rc;
	}
	int64_t t0 = k_uptime_get();

	rc = flash_area_erase(fa, 0, ROUND_UP(kb * 1024, 4096));
	shell_print(sh, "erase %u KB: %d in %lld ms", (unsigned)ROUND_UP(kb, 4), rc,
		    k_uptime_get() - t0);
	t0 = k_uptime_get();
	for (uint32_t off = 0; off < kb * 1024 && rc == 0; off += sizeof(buf)) {
		for (size_t i = 0; i < sizeof(buf); i++) {
			buf[i] = (uint8_t)(off / 512 + i);
		}
		rc = flash_area_write(fa, off, buf, sizeof(buf));
		if (rc == 0) {
			rc = flash_area_read(fa, off, back, sizeof(back));
		}
		if (rc == 0 && memcmp(buf, back, sizeof(buf)) != 0) {
			shell_print(sh, "verify mismatch at %u", off);
			rc = -EIO;
		}
	}
	shell_print(sh, "write+verify %u KB: %d in %lld ms", kb, rc, k_uptime_get() - t0);
	flash_area_close(fa);
	return rc;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_ota,
	SHELL_CMD_ARG(selftest, NULL, "selftest [kb] - erase/write/verify slot1", cmd_ota_selftest, 1, 1),
	SHELL_SUBCMD_SET_END);
SHELL_CMD_REGISTER(ota, &sub_ota, "OTA slot checks", NULL);
#endif
