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
 * SEC engine common bring-up.
 *
 * Central place to initialise the SEC_ENG sub-blocks the platform needs.
 * Today that is just the TRNG; SHA / AES / PKA / etc. init can be added here
 * as those blocks gain init requirements, so callers only ever invoke
 * hal_sec_common_init() once.
 */
#include "hal_sec_common.h"
#include "hal_sec_trng.h"

int hal_sec_common_init(void)
{
    /* TRNG feeds wpa_supplicant os_get_random (WPA2 SNonce, SAE commit
     * scalar). Without it every secure connect dies in the handshake with
     * "Failed to get random data". */
    return hal_sec_trng_init();
}
