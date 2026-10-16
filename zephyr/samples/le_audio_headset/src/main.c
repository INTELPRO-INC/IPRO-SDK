/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * LE Audio unicast headset: advertises as a CAP acceptor (CAS + CSIP set
 * member of size 1, ASCS unicast announcement, Earbud appearance, RSI), lets
 * a phone connect, pair (bond kept in NVS) and stream LC3 to the speaker;
 * VCP volume renderer drives the output gain.
 */
#include <zephyr/kernel.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gap.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/audio/audio.h>
#include <zephyr/bluetooth/audio/cap.h>
#include <zephyr/bluetooth/audio/csip.h>
#include <zephyr/bluetooth/audio/vcp.h>
#include <zephyr/settings/settings.h>
#include <zephyr/logging/log.h>
#include "audio.h"
#include "pcm_out.h"

LOG_MODULE_REGISTER(headset, CONFIG_LOG_DEFAULT_LEVEL);

int bap_server_init(void);

#define DEVICE_NAME     CONFIG_HEADSET_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
	/* ASCS unicast announcement: general, no metadata */
	BT_DATA_BYTES(BT_DATA_SVC_DATA16, BT_UUID_16_ENCODE(BT_UUID_ASCS_VAL),
		      BT_AUDIO_UNICAST_ANNOUNCEMENT_GENERAL, 0x00),
};

/* Scan response: appearance, service list, and the CSIS RSI added at start. */
static const struct bt_data sd_static[] = {
	BT_DATA_BYTES(BT_DATA_GAP_APPEARANCE, BT_BYTES_LIST_LE16(CONFIG_BT_DEVICE_APPEARANCE)),
	BT_DATA_BYTES(BT_DATA_UUID16_ALL, BT_UUID_16_ENCODE(BT_UUID_CAS_VAL),
		      BT_UUID_16_ENCODE(BT_UUID_ASCS_VAL), BT_UUID_16_ENCODE(BT_UUID_PACS_VAL),
		      BT_UUID_16_ENCODE(BT_UUID_VCS_VAL)),
};

static struct bt_csip_set_member_svc_inst *csip_inst;
static uint8_t rsi[BT_CSIP_RSI_SIZE];
static struct bt_conn *current_conn;
static struct k_work_delayable adv_work;

static int advertising_start(void)
{
	struct bt_data sd[ARRAY_SIZE(sd_static) + 1];
	size_t sd_len = 0;
	int err;

	if (current_conn != NULL) {
		return 0;
	}
	for (size_t i = 0; i < ARRAY_SIZE(sd_static); i++) {
		sd[sd_len++] = sd_static[i];
	}
	if (csip_inst != NULL && bt_csip_set_member_generate_rsi(csip_inst, rsi) == 0) {
		sd[sd_len++] = (struct bt_data)BT_DATA(BT_DATA_CSIS_RSI, rsi, sizeof(rsi));
	}
	err = bt_le_adv_start(BT_LE_ADV_CONN_FAST_2, ad, ARRAY_SIZE(ad), sd, sd_len);
	if (err != 0 && err != -EALREADY) {
		LOG_ERR("advertising start: %d", err);
		return err;
	}
	LOG_INF("advertising as \"%s\"", DEVICE_NAME);
	return 0;
}

static void adv_work_fn(struct k_work *work)
{
	ARG_UNUSED(work);
	(void)advertising_start();
}

static void connected(struct bt_conn *conn, uint8_t err)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
	if (err != 0) {
		LOG_ERR("connection to %s failed (0x%02x)", addr, err);
		(void)k_work_reschedule(&adv_work, K_MSEC(500));
		return;
	}
	if (current_conn != NULL) {
		(void)bt_conn_disconnect(conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
		return;
	}
	current_conn = bt_conn_ref(conn);
	LOG_INF("connected: %s", addr);
	err = bt_conn_set_security(conn, BT_SECURITY_L2);
	if (err != 0) {
		LOG_WRN("set security: %d", err);
	}
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
	LOG_INF("disconnected (0x%02x)", reason);
	audio_stop();
	if (current_conn == conn) {
		bt_conn_unref(current_conn);
		current_conn = NULL;
	}
	(void)k_work_reschedule(&adv_work, K_MSEC(500));
}

static void security_changed(struct bt_conn *conn, bt_security_t level, enum bt_security_err err)
{
	ARG_UNUSED(conn);
	LOG_INF("security level %d (err %d)", level, err);
}

BT_CONN_CB_DEFINE(conn_cbs) = {
	.connected = connected,
	.disconnected = disconnected,
	.security_changed = security_changed,
};

static void pairing_complete(struct bt_conn *conn, bool bonded)
{
	ARG_UNUSED(conn);
	LOG_INF("pairing complete, bonded %d", bonded);
}

static void pairing_failed(struct bt_conn *conn, enum bt_security_err reason)
{
	ARG_UNUSED(conn);
	LOG_WRN("pairing failed: %d (forget the device on the phone and retry)", reason);
}

static struct bt_conn_auth_info_cb auth_info_cbs = {
	.pairing_complete = pairing_complete,
	.pairing_failed = pairing_failed,
};

/* ---- VCP volume renderer ------------------------------------------------ */

static void vcp_state(struct bt_conn *conn, int err, uint8_t volume, uint8_t mute)
{
	ARG_UNUSED(conn);
	if (err != 0) {
		return;
	}
	LOG_INF("volume %u mute %u", volume, mute);
	audio_set_volume(volume, mute != 0);
}

static void vcp_flags(struct bt_conn *conn, int err, uint8_t flags)
{
	ARG_UNUSED(conn); ARG_UNUSED(err); ARG_UNUSED(flags);
}

static struct bt_vcp_vol_rend_cb vcp_cbs = {.state = vcp_state, .flags = vcp_flags};

static int vcp_init(void)
{
	struct bt_vcp_vol_rend_register_param param = {
		.step = 16, .mute = BT_VCP_STATE_UNMUTED, .volume = 200, .cb = &vcp_cbs,
	};
	int err = bt_vcp_vol_rend_register(&param);

	if (err == 0) {
		audio_set_volume(param.volume, false);
	}
	return err;
}

/* ---- CAP acceptor (CAS + CSIP set member) -------------------------------- */

static int cap_init(void)
{
	static const uint8_t sirk[BT_CSIP_SIRK_SIZE] = {0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc,
							 0xde, 0xf0, 0x11, 0x22, 0x33, 0x44,
							 0x55, 0x66, 0x77, 0x88};
	struct bt_csip_set_member_register_param csip = {
		.set_size = 1, .rank = 1, .lockable = true,
	};

	memcpy(csip.sirk, sirk, sizeof(sirk));
	return bt_cap_acceptor_register(&csip, &csip_inst);
}

int main(void)
{
	int err;

	LOG_INF("LE Audio unicast headset (IPRO7AI EVK)");
	err = bt_enable(NULL);
	if (err != 0) {
		LOG_ERR("bt_enable: %d", err);
		return 0;
	}
	if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
		err = settings_load();
		LOG_INF("settings load: %d", err);
	}
	(void)bt_conn_auth_info_cb_register(&auth_info_cbs);
	err = cap_init();
	LOG_INF("CAP acceptor: %d", err);
	err = bap_server_init();
	if (err != 0) {
		return 0;
	}
	err = vcp_init();
	LOG_INF("VCP renderer: %d", err);
	(void)pcm_out_init();
	k_work_init_delayable(&adv_work, adv_work_fn);
	(void)advertising_start();
	return 0;
}

#ifdef CONFIG_SHELL
#include <zephyr/shell/shell.h>

static int cmd_hs_unpair(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc); ARG_UNUSED(argv);
	int err = bt_unpair(BT_ID_DEFAULT, NULL);

	shell_print(sh, "unpair all: %d", err);
	return 0;
}

static int cmd_hs_disconnect(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc); ARG_UNUSED(argv);
	if (current_conn == NULL) {
		shell_print(sh, "not connected");
		return 0;
	}
	shell_print(sh, "disconnect: %d",
		    bt_conn_disconnect(current_conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN));
	return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_hs,
	SHELL_CMD(unpair, NULL, "forget all bonds", cmd_hs_unpair),
	SHELL_CMD(disconnect, NULL, "drop the current link", cmd_hs_disconnect),
	SHELL_SUBCMD_SET_END);
SHELL_CMD_REGISTER(hs, &sub_hs, "LE Audio headset", NULL);
#endif
