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
 * ATBM6162 bring-up entry — thin wrapper over the vendor register flow.
 * IPRO7 port — Phase 0. See .claude/plans/atbm6162-integration.md.
 */
#include "atbm6162.h"
#include "atbm_hal.h"   /* declares atbm_sdio_register_init() via atbm_os_sdio.h */

extern struct atbmwifi_common g_hw_prv;

int atbm6162_bringup(void)
{
    /* Use the vendor's proper top-level init — NOT atbm_sdio_register_init() directly.
     * atbm_wifi_hw_init -> atbm_sdio_module_init -> atbm_init_firmware + atbm_sdio_init
     * (the latter POPULATES atbm_sdio_sbus_ops.lock/.unlock/.memcpy_* BEFORE calling
     *  atbm_sdio_register_init -> register -> probe -> Atbmwifi_halEntry).
     * Calling register_init directly left sbus_ops empty → wild sbus_ops->lock() call. */
    int rc = (int)atbm_wifi_hw_init();
    if (rc != 0) {
        return rc;
    }
    /* The vendor init returns 0 unconditionally — fw-download / WSM-handshake failures
     * inside Atbmwifi_halEntry are swallowed. The real success criterion is the WSM
     * startup indication having arrived (same flag halEntry's own wait loop checks). */
    if (!g_hw_prv.wsm_caps.firmwareReady) {
        return -1;
    }
    return 0;
}
