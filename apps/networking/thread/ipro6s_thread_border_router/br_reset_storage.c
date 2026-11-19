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

#include <ipro_matter_storage.h>
#include <easyflash.h>
#include "br_wifi_storage.h"

/* Boot recovery runs before profile loading and either network worker starts.
 * Runtime reset must first stop/drain CHIP and acquire the combined BR fence.
 * Checked deletion verifies absence; do not replace it with legacy void wipe
 * or the ordinary Wi-Fi clear API, which is closed by the reset fence. */
int ipro_matter_reset_network_storage(void)
{
    const int error = ef_env_del_prefix_checked("ot-");
    if (error != EF_NO_ERR) return error;
    const int wifi_error = ef_env_del_checked(BR_WIFI_KEY);
    /* A prior interrupted reset (or first install) may already have no key. */
    return wifi_error == EF_ENV_NOT_FOUND ? EF_NO_ERR : wifi_error;
}
