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

#include "hal_sec_gmac.h"
#include "hal_common.h"
#include "hal_cache_alloc.h"
#include "drv_sec_eng.h"

int sec_gmac_init(void)
{
    Sec_Eng_GMAC_Enable_BE();
    Sec_Eng_GMAC_Enable_Link();
    return 0;
}

int sec_gmac_start(sec_gmac_handle_t *handle, const uint8_t key[16])
{
    if (!handle || !key) {
        return -1;
    }

    /* Get non-cacheable address for link_cfg */
    SEC_Eng_GMAC_Link_Config_Type *nc_link_cfg =
        (SEC_Eng_GMAC_Link_Config_Type *)hal_get_noncacheable_addr(&handle->link_cfg);

    memset(nc_link_cfg, 0, sizeof(SEC_Eng_GMAC_Link_Config_Type));
    memcpy(&nc_link_cfg->gmacKey0, key, 16);

    return 0;
}

int sec_gmac_clear(sec_gmac_handle_t *handle)
{
    if (!handle) {
        return -1;
    }

    /* Get non-cacheable address for link_cfg */
    SEC_Eng_GMAC_Link_Config_Type *nc_link_cfg =
        (SEC_Eng_GMAC_Link_Config_Type *)hal_get_noncacheable_addr(&handle->link_cfg);

    memset(nc_link_cfg->result, 0, 16);

    return 0;
}

int sec_gmac_update(sec_gmac_handle_t *handle, const uint8_t *input, uint32_t input_len)
{
    int ret = 0;
    uint32_t len, rem;

    if (!handle) {
        return -1;
    }

    if (input_len >= 16 * 65536) {
        return -1;
    }

    if (input_len == 0) {
        return 0;
    }

    /* TODO: callers (mbedTLS) pass unaligned buffers; bounce buffer
     * needed to fully fix cache-line corruption risk. */

    L1C_DCACHE_CLEAN_RANGE(input, input_len);

    /* Get non-cacheable address for link_cfg */
    SEC_Eng_GMAC_Link_Config_Type *nc_link_cfg =
        (SEC_Eng_GMAC_Link_Config_Type *)hal_get_noncacheable_addr(&handle->link_cfg);

    /* Get non-cacheable temp buffer */
    uint8_t *nc_tmp_buf = (uint8_t *)hal_get_noncacheable_addr(handle->tmp_buf);

    rem = input_len % 16;
    len = input_len - rem;

    /* Process full 16-byte blocks */
    if (len > 0) {
        ret = Sec_Eng_GMAC_Link_Work((uint32_t)nc_link_cfg, input, len, nc_tmp_buf);
        if (ret != SUCCESS) {
            return -1;
        }
    }

    /* Process remaining partial block (padded with zeros) */
    if (rem > 0) {
        memcpy(nc_tmp_buf, input + len, rem);
        memset(nc_tmp_buf + rem, 0, 16 - rem);
        ret = Sec_Eng_GMAC_Link_Work((uint32_t)nc_link_cfg, nc_tmp_buf, 16, nc_tmp_buf);
        if (ret != SUCCESS) {
            return -1;
        }
    }

    return 0;
}

int sec_gmac_finish(sec_gmac_handle_t *handle, uint8_t result[16])
{
    if (!handle || !result) {
        return -1;
    }

    /* Get non-cacheable address for link_cfg */
    SEC_Eng_GMAC_Link_Config_Type *nc_link_cfg =
        (SEC_Eng_GMAC_Link_Config_Type *)hal_get_noncacheable_addr(&handle->link_cfg);

    memcpy(result, nc_link_cfg->result, 16);

    return 0;
}

int sec_gmac_deinit(sec_gmac_handle_t *handle)
{
    if (!handle) {
        return -1;
    }

    /* Get non-cacheable address for link_cfg */
    SEC_Eng_GMAC_Link_Config_Type *nc_link_cfg =
        (SEC_Eng_GMAC_Link_Config_Type *)hal_get_noncacheable_addr(&handle->link_cfg);

    memset(nc_link_cfg, 0, sizeof(SEC_Eng_GMAC_Link_Config_Type));

    return 0;
}
