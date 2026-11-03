/*
 * Copyright (c) 2026 IntelPro Technology Co., Ltd.
 * All rights reserved.
 *
 * This software and associated documentation files (the "Software") are
 * proprietary and confidential to IntelPro Technology Co., Ltd..
 *
 * Access to and use of this Software is limited exclusively to authorized 
 * customers and partners of IntelPro Technology Co., Ltd. who have entered into a 
 * valid license or non-disclosure agreement.
 *
 * Unauthorized copying, modification, distribution, or reverse engineering 
 * of this Software, via any medium, is strictly prohibited without 
 * prior written permission from IntelPro Technology Co., Ltd..
 *
 * SPDX-License-Identifier: Proprietary
 */

/*
 * Hardware unique key: eFuse AES key slot HUK_EFUSE_SLOT. huk_prf is
 * its only user; the key bytes never leave huk.c.
 */
#pragma once

#include <stdint.h>

/* eFuse key slot of the HUK: 4 (XIP AES-128 parts) or 2 (plaintext) */
#ifdef CONFIG_PSA_HUK_EFUSE_SLOT
#define HUK_EFUSE_SLOT CONFIG_PSA_HUK_EFUSE_SLOT
#else
#define HUK_EFUSE_SLOT 4
#endif
#if HUK_EFUSE_SLOT != 2 && HUK_EFUSE_SLOT != 4
#error "CONFIG_PSA_HUK_EFUSE_SLOT must be 2 or 4"
#endif

/*
 * Picks the HUK source: SEC_ENG key, the slot per call, or the dev HUK
 * (blank key only; a locked or failed engine gives no HUK). 0 when
 * huk_prf is usable, else -1.
 */
int huk_init(void);

/*
 * out = AES-128-ECB(HUK, in); -1 (out zeroed) without a source or
 * after huk_lock (logs "[SPE] HUK locked: PRF refused" once).
 */
int huk_prf(const uint8_t in[16], uint8_t out[16]);

/*
 * Ends HUK use; with CONFIG_PSA_HUK_LOCK and tzc_sboot_done 0 at init
 * sets it to 0xF, then KEY_SEL must give the zero-key block. 0 locked,
 * 1 field left as found, -1 route open; huk_prf, huk_init refuse after.
 */
int huk_lock(void);

/* 1 when huk_prf runs on the built-in development HUK */
int huk_is_dev(void);
