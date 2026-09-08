/**
 * @file cap_acceptor.c
 * @brief CAP Acceptor Implementation (Phase 2)
 *
 * Implements Common Audio Profile (CAP) Acceptor role with
 * Coordinated Set Identification Profile (CSIP) Set Member.
 *
 * Copyright (C) IPRO SDK 2026
 */

#include <zephyr/bluetooth/audio/cap.h>
#include <zephyr/bluetooth/audio/csip.h>

#include "cap_acceptor.h"
#include "ipro_log.h"
#include <string.h>

#define LOG_TAG "CAP_ACCEPTOR"

#if defined(CONFIG_BT_CAP_ACCEPTOR_SET_MEMBER)
/* CSIP Set Member instance */
static struct bt_csip_set_member_svc_inst *csip_svc_inst;

/* SIRK (Set Identity Resolving Key) - Example value, should be provisioned */
static uint8_t sirk_value[BT_CSIP_SIRK_SIZE] = {
	0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0,
	0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88
};
#endif

/**
 * @brief Initialize CAP Acceptor
 */
int cap_acceptor_init(void)
{
#if defined(CONFIG_BT_CAP_ACCEPTOR_SET_MEMBER)
	int err;
	struct bt_csip_set_member_register_param csip_param = {
		.set_size = 1,  /* Single device set for now */
		.rank = 1,      /* Primary device */
		.lockable = true,
		.sirk = { 0 },
		.cb = NULL,     /* No CSIP callbacks needed for Phase 2 */
	};

	IPRO_LOGI(LOG_TAG, "Initializing CAP Acceptor");

	/* Copy SIRK to parameter */
	memcpy(csip_param.sirk, sirk_value, sizeof(sirk_value));

	/* Register CAP Acceptor with CSIP */
	err = bt_cap_acceptor_register(&csip_param, &csip_svc_inst);
	if (err != 0) {
		IPRO_LOGE(LOG_TAG, "Failed to register CAP Acceptor (err %d)", err);
		return err;
	}

	IPRO_LOGI(LOG_TAG, "CAP Acceptor initialized successfully");
	return 0;
#else
	IPRO_LOGI(LOG_TAG, "CAP Acceptor CAS-only mode (no CSIP set member)");
	return 0;
#endif
}

bool cap_acceptor_generate_rsi(uint8_t rsi[6])
{
#if defined(CONFIG_BT_CAP_ACCEPTOR_SET_MEMBER)
	if (csip_svc_inst == NULL) {
		return false;
	}

	return bt_csip_set_member_generate_rsi(csip_svc_inst, rsi) == 0;
#else
	(void)rsi;
	return false;
#endif
}
