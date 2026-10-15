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

/**
 * @file evk.c
 * @brief evk_begin(). See evk.h.
 */

#include "evk.h"

#include "ipro_log.h"

#define LOG_TAG "EVK"

static bool s_ready;

int evk_begin(void)
{
    int rc = 0;

    if (s_ready) {
        return 0;
    }

#if EVK_HAS_CAMERA
    /* Park the camera before anything else. Its RESETB defaults to released,
     * and a sensor left streaming loads the 1V8 LDO that also feeds the
     * BOOT strap - see evk_cam_stop(). */
    evk_cam_stop();
#endif
#if EVK_HAS_AUDIO
    /* Amplifier off until something plays: no idle hiss, no pop at boot. */
    evk_spk_enable(0);
#endif

    if (st7789p3_init(ST7789P3_SEQ_MINIMAL, 1) != ST7789P3_OK) {
        IPRO_LOGE(LOG_TAG, "panel init failed - continuing headless");
        rc = -1;
    }

    IPRO_LOGI(LOG_TAG, "%s: display %s, camera %s, audio %s, sd %s",
              EVK_BOARD_NAME, rc == 0 ? "up" : "FAILED",
              EVK_HAS_CAMERA ? "yes" : "no", EVK_HAS_AUDIO ? "yes" : "no",
#if EVK_HAS_SDCARD
              evk_sd_present() ? "card seated" : "no card");
#else
              "no");
#endif
    s_ready = true;
    return rc;
}

bool evk_ready(void)
{
    return s_ready;
}
