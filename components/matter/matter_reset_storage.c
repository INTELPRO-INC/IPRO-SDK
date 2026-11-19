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

#include "include/ipro_matter_storage.h"

#include <easyflash.h>
#include <string.h>

static const char reset_key[] = "m/reset-ready";
static const unsigned char reset_ready[] = { 0x49, 0x52, 0x01, 0xa5 };

static int read_reset_ready(bool *ready)
{
    unsigned char stored[sizeof(reset_ready)];
    size_t copied = 0, total = 0;
    *ready = false;
    const EfErrCode error = ef_env_get_checked(reset_key, 0, stored, sizeof(stored), &copied, &total);
    if (error == EF_ENV_NOT_FOUND) return EF_NO_ERR;
    if (error != EF_NO_ERR) return error;
    if (copied != sizeof(stored) || total != sizeof(stored) ||
            memcmp(stored, reset_ready, sizeof(stored)) != 0) return EF_ENV_CORRUPT;
    *ready = true;
    return EF_NO_ERR;
}

int ipro_matter_storage_begin_reset(void)
{
    bool ready = false;
    int error = read_reset_ready(&ready);
    if (error != EF_NO_ERR || !ready) return error;
    /* A pre-existing sentinel permits starting reset with no allocation even
     * when storage is full. Its absence is the durable pending state. */
    error = ef_env_del_checked(reset_key);
    if (error != EF_NO_ERR) return error;
    error = read_reset_ready(&ready);
    return error != EF_NO_ERR ? error : (ready ? EF_WRITE_ERR : EF_NO_ERR);
}

int ipro_matter_storage_complete_reset(void)
{
    bool ready = false;
    int error = read_reset_ready(&ready);
    if (error != EF_NO_ERR || ready) return error;

    /* Only reset-owned namespaces. Keep m/c/f/ identity/provisioning,
     * calibration and vendor keys intact. Missing sentinel on first install
     * also requests reset: never silently adopt unversioned runtime state. */
    static const char *const prefixes[] = { "m/k/", "m/c/r/", "m/c/p/", "m/c/c/" };
    for (size_t i = 0; i < sizeof(prefixes) / sizeof(prefixes[0]); ++i) {
        error = ef_env_del_prefix_checked(prefixes[i]);
        if (error != EF_NO_ERR) return error;
    }
    error = ipro_matter_reset_network_storage();
    if (error != EF_NO_ERR) return error;

    /* Publish READY only after all namespaces are verified absent. Deleted
     * records can now be reclaimed to recreate the sentinel in a full store. */
    error = ef_env_set_checked(reset_key, reset_ready, sizeof(reset_ready));
    if (error != EF_NO_ERR) return error;
    error = read_reset_ready(&ready);
    return error != EF_NO_ERR ? error : (ready ? EF_NO_ERR : EF_WRITE_ERR);
}

int ipro_matter_storage_boot_recover(void)
{
    const int error = easyflash_init();
    return error != EF_NO_ERR ? error : ipro_matter_storage_complete_reset();
}
