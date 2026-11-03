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

#include "hal_sec_hash.h"
#include "hal_common.h"
#include "hal_cache_alloc.h"
#include "drv_sec_eng.h"

#define SHA_ID SEC_ENG_SHA_ID0

/**
 * @brief Get driver SHA type from HAL hash type
 */
static SEC_ENG_SHA_Type sec_hash_get_drv_type(uint8_t type)
{
    switch (type) {
        case SEC_HASH_SHA1:
            return SEC_ENG_SHA1;
        case SEC_HASH_SHA224:
            return SEC_ENG_SHA224;
        case SEC_HASH_SHA256:
            return SEC_ENG_SHA256;
        case SEC_HASH_SHA384:
            return SEC_ENG_SHA384;
        case SEC_HASH_SHA512:
            return SEC_ENG_SHA512;
        default:
            return SEC_ENG_SHA256;
    }
}

/**
 * @brief Get hash output length
 */
static int sec_hash_get_output_len(uint8_t type)
{
    switch (type) {
        case SEC_HASH_SHA1:
            return 20;
        case SEC_HASH_SHA224:
            return 28;
        case SEC_HASH_SHA256:
            return 32;
        case SEC_HASH_SHA384:
            return 48;
        case SEC_HASH_SHA512:
            return 64;
        default:
            return -1;
    }
}

int sec_hash_init(sec_hash_handle_t *handle, uint8_t type)
{
    /* Default to link mode for better performance */
    return sec_hash_init_ex(handle, SEC_HASH_MODE_LINK, type);
}

int sec_hash_init_ex(sec_hash_handle_t *handle, sec_hash_mode_t mode, uint8_t type)
{
    if (!handle || type >= SEC_HASH_UNKNOWN) {
        return -1;
    }

    memset(handle, 0, sizeof(sec_hash_handle_t));
    handle->mode = mode;
    handle->type = type;

    /*
     * Flush and discard dirty cache lines for HW-visible buffers.
     *
     * memset above created dirty cache lines at cacheable addresses for
     * the entire handle including shaBuf, shaPadding, and link_cfg.
     * These regions will be written through noncacheable aliases below.
     * Without flushing first, later cache eviction writes stale zeros
     * back to physical memory, overwriting the NC-written HW config.
     *
     * Must use clean+invalidate (not pure invalidate) — pure invalidate
     * may write back dirty lines on this HW, defeating the purpose.
     */
    L1C_DCACHE_CLEAN_INVALID_RANGE(handle->shaBuf, sizeof(handle->shaBuf));
    L1C_DCACHE_CLEAN_INVALID_RANGE(handle->shaPadding, sizeof(handle->shaPadding));
    L1C_DCACHE_CLEAN_INVALID_RANGE(&handle->link.link_cfg,
                                   sizeof(SEC_Eng_SHA_Link_Config_Type));

    /* Get non-cacheable addresses for HW buffers */
    uint64_t *nc_shaBuf = (uint64_t *)hal_get_noncacheable_addr(handle->shaBuf);
    uint64_t *nc_shaPadding = (uint64_t *)hal_get_noncacheable_addr(handle->shaPadding);

    /* Zero HW buffers through noncacheable aliases */
    memset(nc_shaBuf, 0, sizeof(handle->shaBuf));
    memset(nc_shaPadding, 0, sizeof(handle->shaPadding));

    //Sec_Eng_Group0_Request_SHA_Access();

    if (mode == SEC_HASH_MODE_DIRECT) {
        /* Direct mode: disable link mode first, then use driver API */
        Sec_Eng_SHA_Disable_Link(SHA_ID);
        if (type == SEC_HASH_SHA384 || type == SEC_HASH_SHA512) {
            Sec_Eng_SHA512_Init(&handle->sha512_ctx, SHA_ID, sec_hash_get_drv_type(type),
                                nc_shaBuf, nc_shaPadding);
        } else {
            Sec_Eng_SHA256_Init(&handle->sha256_ctx, SHA_ID, sec_hash_get_drv_type(type),
                                (uint32_t *)nc_shaBuf, (uint32_t *)nc_shaPadding);
        }
        Sec_Eng_SHA_Start(SHA_ID);
    } else {
        /* Link mode: initialize link config */
        SEC_Eng_SHA_Link_Config_Type *nc_link_cfg =
            (SEC_Eng_SHA_Link_Config_Type *)hal_get_noncacheable_addr(&handle->link.link_cfg);

        /*
         * Zero link_cfg through noncacheable alias before setting fields.
         * The memset above writes zeros through cache only. Bitfield writes
         * below do read-modify-write from NC, so stale HW state (e.g.,
         * shaHashSel=1 from a previous operation) would leak through.
         */
        memset(nc_link_cfg, 0, sizeof(SEC_Eng_SHA_Link_Config_Type));

        nc_link_cfg->shaMode = sec_hash_get_drv_type(type);
        nc_link_cfg->shaIntSet = 1;
        nc_link_cfg->shaIntClr = 1;

        Sec_Eng_SHA_Enable_Link(SHA_ID);
        if (type == SEC_HASH_SHA384 || type == SEC_HASH_SHA512) {
            Sec_Eng_SHA512_Link_Init(&handle->link.sha512_link_ctx, SHA_ID,
                                     (uint32_t)nc_link_cfg,
                                     nc_shaBuf, nc_shaPadding);
        } else {
            Sec_Eng_SHA256_Link_Init(&handle->link.sha256_link_ctx, SHA_ID,
                                     (uint32_t)nc_link_cfg,
                                     (uint32_t *)nc_shaBuf, (uint32_t *)nc_shaPadding);
        }
    }

    return 0;
}

int sec_hash_deinit(sec_hash_handle_t *handle)
{
    if (!handle) {
        return -1;
    }

    /* Zero HW buffers through both cache and noncacheable aliases */
    memset(handle->shaBuf, 0, sizeof(handle->shaBuf));
    memset(handle->shaPadding, 0, sizeof(handle->shaPadding));
    memset(hal_get_noncacheable_addr(handle->shaBuf), 0, sizeof(handle->shaBuf));
    memset(hal_get_noncacheable_addr(handle->shaPadding), 0, sizeof(handle->shaPadding));

    return 0;
}

int sec_hash_update(sec_hash_handle_t *handle, const void *buffer, uint32_t size)
{
    if (!handle || !buffer || size == 0) {
        return -1;
    }

    /* TODO: callers (mbedTLS) pass unaligned buffers; bounce buffer
     * needed to fully fix cache-line corruption risk. */

    /*
     * Flush input buffer from D-Cache to physical memory.
     * The SEC_ENG DMA reads input data bypassing cache. If the caller wrote
     * data through cached addresses (e.g., TLS handshake buffers in PSRAM),
     * dirty cache lines would cause HW to read stale memory.
     */
    L1C_DCACHE_CLEAN_RANGE(buffer, size);

    /*
     * Flush + invalidate all HW-visible buffers in the handle.
     * The SEC engine DMA reads/writes shaBuf, shaPadding, and link_cfg
     * bypassing CPU cache.  Stale cached copies of these buffers —
     * created by struct copies (clone), memset (init), or cache-line
     * sharing with the input buffer — can be evicted and overwrite
     * the DMA-written state in physical memory.
     */
    if (handle->mode == SEC_HASH_MODE_LINK) {
        L1C_DCACHE_CLEAN_INVALID_RANGE(handle->shaBuf,
                                       sizeof(handle->shaBuf));
        L1C_DCACHE_CLEAN_INVALID_RANGE(handle->shaPadding,
                                       sizeof(handle->shaPadding));
        L1C_DCACHE_CLEAN_INVALID_RANGE(&handle->link.link_cfg,
                                       sizeof(SEC_Eng_SHA_Link_Config_Type));
    }

    if (handle->mode == SEC_HASH_MODE_DIRECT) {
        /* Direct mode: use driver API */
        if (handle->type == SEC_HASH_SHA384 || handle->type == SEC_HASH_SHA512) {
            Sec_Eng_SHA512_Update(&handle->sha512_ctx, SHA_ID, (uint8_t *)buffer, size);
        } else {
            Sec_Eng_SHA256_Update(&handle->sha256_ctx, SHA_ID, (uint8_t *)buffer, size);
        }
    } else {
        /* Link mode: use link API */
        if (handle->type == SEC_HASH_SHA384 || handle->type == SEC_HASH_SHA512) {
            Sec_Eng_SHA512_Link_Update(&handle->link.sha512_link_ctx, SHA_ID, buffer, size);
        } else {
            Sec_Eng_SHA256_Link_Update(&handle->link.sha256_link_ctx, SHA_ID, buffer, size);
        }
    }

    return 0;
}

int sec_hash_finish(sec_hash_handle_t *handle, void *buffer)
{
    int ret;

    if (!handle || !buffer) {
        return -1;
    }

    ret = sec_hash_get_output_len(handle->type);
    if (ret < 0) {
        return ret;
    }

    /* Same cache coherency protection as sec_hash_update —
     * Link_Finish internally calls Link_Update for padding/length,
     * which triggers the SEC engine DMA. */
    if (handle->mode == SEC_HASH_MODE_LINK) {
        L1C_DCACHE_CLEAN_INVALID_RANGE(handle->shaBuf,
                                       sizeof(handle->shaBuf));
        L1C_DCACHE_CLEAN_INVALID_RANGE(handle->shaPadding,
                                       sizeof(handle->shaPadding));
        L1C_DCACHE_CLEAN_INVALID_RANGE(&handle->link.link_cfg,
                                       sizeof(SEC_Eng_SHA_Link_Config_Type));
    }

    if (handle->mode == SEC_HASH_MODE_DIRECT) {
        /* Direct mode: use driver API */
        if (handle->type == SEC_HASH_SHA384 || handle->type == SEC_HASH_SHA512) {
            Sec_Eng_SHA512_Finish(&handle->sha512_ctx, SHA_ID, buffer);
        } else {
            Sec_Eng_SHA256_Finish(&handle->sha256_ctx, SHA_ID, buffer);
        }
    } else {
        /* Link mode: use link API */
        if (handle->type == SEC_HASH_SHA384 || handle->type == SEC_HASH_SHA512) {
            Sec_Eng_SHA512_Link_Finish(&handle->link.sha512_link_ctx, SHA_ID, buffer);
        } else {
            Sec_Eng_SHA256_Link_Finish(&handle->link.sha256_link_ctx, SHA_ID, buffer);
        }
    }

    return ret;
}

/* Legacy API for backward compatibility */
int sec_hash_write(sec_hash_handle_t *handle, uint32_t pos, const void *buffer, uint32_t size)
{
    (void)pos;
    return sec_hash_update(handle, buffer, size);
}

int sec_hash_read(sec_hash_handle_t *handle, uint32_t pos, void *buffer, uint32_t size)
{
    (void)pos;
    (void)size;
    return sec_hash_finish(handle, buffer);
}
