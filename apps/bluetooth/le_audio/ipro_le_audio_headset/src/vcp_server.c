/**
 * @file vcp_server.c
 * @brief VCP Volume Renderer Implementation (Phase 2)
 *
 * Implements Volume Control Profile (VCP) server functionality for
 * remote volume and mute control.
 *
 * Copyright (C) IPRO SDK 2026
 */

#include <zephyr/bluetooth/audio/vcp.h>

#include "ipro_log.h"
#include "app_audio.h"
#include "app_dac.h"

#define LOG_TAG "VCP_SERVER"

/* Current volume state */
static uint8_t current_volume = 128; /* Mid-level (0-255) */
static uint8_t current_mute = 0;     /* 0 = unmuted, 1 = muted */

/**
 * @brief Volume state callback - called when volume or mute changes
 */
static void vcp_vol_rend_state_cb(struct bt_conn *conn, int err, uint8_t volume, uint8_t mute)
{
	(void)conn;

	if (err != 0) {
		IPRO_LOGE(LOG_TAG, "Volume state error: %d", err);
		return;
	}

	IPRO_LOGI(LOG_TAG, "Volume state changed: volume=%u mute=%u", volume, mute);

	current_volume = volume;
	current_mute = mute;

	/* Apply volume to DAC */
#if defined(CONFIG_BSP_USING_DAC) && !defined(CONFIG_AUDIO_OUTPUT_UAC)
	if (mute) {
		app_dac_set_volume(0);
	} else {
		/* VCP volume: 0-255, DAC volume: 0-100 */
		uint8_t dac_volume = (uint8_t)((uint16_t)volume * 100 / 255);
		app_dac_set_volume(dac_volume);
	}
#else
	IPRO_LOGI(LOG_TAG, "Audio output disabled/UAC: volume state stored only");
#endif
}

/**
 * @brief Flags callback - called when VCP flags change
 */
static void vcp_vol_rend_flags_cb(struct bt_conn *conn, int err, uint8_t flags)
{
	(void)conn;

	if (err != 0) {
		IPRO_LOGE(LOG_TAG, "Flags error: %d", err);
		return;
	}

	IPRO_LOGI(LOG_TAG, "Flags changed: 0x%02x", flags);
}

/* VCP Volume Renderer callbacks */
static struct bt_vcp_vol_rend_cb vol_rend_cb = {
	.state = vcp_vol_rend_state_cb,
	.flags = vcp_vol_rend_flags_cb,
};

/**
 * @brief Initialize VCP Volume Renderer
 */
int vcp_server_init(void)
{
	int err;
	struct bt_vcp_vol_rend_register_param param = {
		.cb = &vol_rend_cb,
		.mute = current_mute,
		.volume = current_volume,
		.step = 16, /* Step size for volume up/down */
	};

	IPRO_LOGI(LOG_TAG, "Initializing VCP Volume Renderer");

	err = bt_vcp_vol_rend_register(&param);
	if (err != 0) {
		IPRO_LOGE(LOG_TAG, "Failed to register VCP (err %d)", err);
		return err;
	}

	IPRO_LOGI(LOG_TAG, "VCP Volume Renderer initialized (volume=%u, mute=%u)",
		  current_volume, current_mute);
	return 0;
}

/**
 * @brief Get current volume level
 */
uint8_t vcp_server_get_volume(void)
{
	return current_volume;
}

/**
 * @brief Get current mute state
 */
bool vcp_server_is_muted(void)
{
	return current_mute != 0;
}
