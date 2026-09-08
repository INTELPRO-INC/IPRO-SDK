/**
 * @file ble_main.c
 * @brief LE Audio Main Entry (Phase 2)
 *
 * Integrates BAP, VCP, and CAP modules for complete LE Audio functionality.
 *
 * Copyright (C) IPRO SDK 2026
 */

#include "ipro_log.h"
#include "bap_server.h"
#include "vcp_server.h"
#include "cap_acceptor.h"

#define LOG_TAG "BLE_MAIN"

/**
 * @brief Initialize all LE Audio profiles
 */
int le_audio_init(void)
{
	int err;

	IPRO_LOGI(LOG_TAG, "Initializing LE Audio services");

	/* Initialize CAP first (registers CAS and CSIP) */
	err = cap_acceptor_init();
	if (err != 0) {
		IPRO_LOGE(LOG_TAG, "CAP init failed: %d", err);
		return err;
	}

	/* Initialize BAP Unicast Server (registers ASCS and PACS) */
	err = bap_server_init();
	if (err != 0) {
		IPRO_LOGE(LOG_TAG, "BAP init failed: %d", err);
		return err;
	}

	/* Initialize VCP Volume Renderer (registers VCS) */
	err = vcp_server_init();
	if (err != 0) {
		IPRO_LOGE(LOG_TAG, "VCP init failed: %d", err);
		return err;
	}

	IPRO_LOGI(LOG_TAG, "LE Audio services initialized successfully");
	return 0;
}