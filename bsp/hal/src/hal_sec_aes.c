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

#include "hal_sec_aes.h"
#include "hal_common.h"
#include "hal_cache_alloc.h"
#include "drv_sec_eng.h"

#define AES_ID SEC_ENG_AES_ID0

/**
 * @brief Get driver key type from HAL key type
 */
static SEC_ENG_AES_Key_Type sec_aes_get_drv_key_type(sec_aes_key_type key_type)
{
    switch (key_type) {
        case SEC_AES_KEY_128:
            return SEC_ENG_AES_KEY_128BITS;
        case SEC_AES_KEY_192:
            return SEC_ENG_AES_KEY_192BITS;
        case SEC_AES_KEY_256:
            return SEC_ENG_AES_KEY_256BITS;
        default:
            return SEC_ENG_AES_KEY_128BITS;
    }
}

/**
 * @brief Get driver AES type from HAL AES type
 */
static SEC_ENG_AES_Type sec_aes_get_drv_aes_type(sec_aes_type aes_type)
{
    switch (aes_type) {
        case SEC_AES_CBC:
            return SEC_ENG_AES_CBC;
        case SEC_AES_CTR:
            return SEC_ENG_AES_CTR;
        case SEC_AES_ECB:
            return SEC_ENG_AES_ECB;
        default:
            return SEC_ENG_AES_ECB;
    }
}

int sec_aes_init(sec_aes_handle_t *handle, sec_aes_type aes_type, sec_aes_key_type key_type)
{
    /* Default to link mode for better performance */
    return sec_aes_init_ex(handle, SEC_MODE_LINK, aes_type, key_type);
}

int sec_aes_init_ex(sec_aes_handle_t *handle, sec_mode_t mode, sec_aes_type aes_type, sec_aes_key_type key_type)
{
    if (!handle) {
        return -1;
    }

    handle->mode = mode;
    handle->aes_type = aes_type;
    handle->key_type = key_type;

    //Sec_Eng_Group0_Request_AES_Access();

    if (mode == SEC_MODE_DIRECT) {
        /* Direct mode: disable link mode first */
        Sec_Eng_AES_Disable_Link(AES_ID);
    } else {
        /* Link mode: initialize link config with non-cacheable address */
        SEC_Eng_AES_Link_Config_Type *nc_link_cfg =
            (SEC_Eng_AES_Link_Config_Type *)hal_get_noncacheable_addr(&handle->link.link_cfg);

        memset(nc_link_cfg, 0, sizeof(SEC_Eng_AES_Link_Config_Type));
        nc_link_cfg->aesIntClr = 1;
        nc_link_cfg->aesBlockMode = sec_aes_get_drv_aes_type(aes_type);
        nc_link_cfg->aesMode = sec_aes_get_drv_key_type(key_type);
    }

    return 0;
}

int sec_aes_setkey(sec_aes_handle_t *handle, const uint8_t *key, uint8_t key_len, const uint8_t *nonce, uint8_t dir)
{
    if (!handle) {
        return -1;
    }

    handle->dir = (sec_aes_dir_type)dir;

    if (handle->mode == SEC_MODE_DIRECT) {
        /* Direct mode: use driver API */
        SEC_ENG_AES_Key_Type drv_key_type = sec_aes_get_drv_key_type(handle->key_type);
        SEC_ENG_AES_Type drv_aes_type = sec_aes_get_drv_aes_type(handle->aes_type);
        SEC_ENG_AES_EnDec_Type drv_dir = (dir == SEC_AES_DIR_ENCRYPT) ?
                                          SEC_ENG_AES_ENCRYPTION : SEC_ENG_AES_DECRYPTION;

        Sec_Eng_AES_Enable_BE(AES_ID);
        Sec_Eng_AES_Init(&handle->direct_ctx, AES_ID, drv_aes_type, drv_key_type, drv_dir);

        /* Set key and IV */
        if (key_len == 0) {
            /* Hardware key */
            Sec_Eng_AES_Set_Key_IV_BE(AES_ID, SEC_ENG_AES_KEY_HW, key, nonce);
        } else {
            Sec_Eng_AES_Set_Key_IV_BE(AES_ID, SEC_ENG_AES_KEY_SW, key, nonce);
        }
    } else {
        /* Link mode: configure link structure with non-cacheable address */
        SEC_Eng_AES_Link_Config_Type *nc_link_cfg =
            (SEC_Eng_AES_Link_Config_Type *)hal_get_noncacheable_addr(&handle->link.link_cfg);

        /* Set key type and block mode */
        nc_link_cfg->aesMode = sec_aes_get_drv_key_type(handle->key_type);
        nc_link_cfg->aesBlockMode = sec_aes_get_drv_aes_type(handle->aes_type);
        nc_link_cfg->aesDecKeySel = SEC_ENG_AES_USE_NEW;

        /* Set direction */
        nc_link_cfg->aesDecEn = (dir == SEC_AES_DIR_ENCRYPT) ?
                                SEC_ENG_AES_ENCRYPTION : SEC_ENG_AES_DECRYPTION;

        /* Copy key */
        if (key && key_len > 0) {
            nc_link_cfg->aesHwKeyEn = 0;
            memcpy(&nc_link_cfg->aesKey0, key, key_len);
        } else {
            /* Hardware key */
            nc_link_cfg->aesHwKeyEn = 1;
        }

        /* Copy IV/nonce if provided */
        if (nonce) {
            memcpy(&nc_link_cfg->aesIV0, nonce, 16);
            nc_link_cfg->aesIVSel = SEC_ENG_AES_USE_NEW;
        }
    }

    return 0;
}

/**
 * @brief Internal AES crypt function for both modes
 */
static int sec_aes_crypt(sec_aes_handle_t *handle, const uint8_t *in, uint32_t len, uint8_t *out)
{
    if (!handle || !in || !out || (len % 16 != 0)) {
        return -1;
    }

    /* TODO: callers (mbedTLS) pass unaligned buffers; bounce buffer
     * needed to fully fix cache-line corruption risk. */

    /* Flush input to memory for DMA; discard dirty output cache lines
     * so they cannot overwrite the DMA result on later eviction. */
    L1C_DCACHE_CLEAN_RANGE(in, len);
    L1C_DCACHE_CLEAN_INVALID_RANGE(out, len);

    if (handle->mode == SEC_MODE_DIRECT) {
        /* Direct mode: use driver crypt */
        if (SUCCESS != Sec_Eng_AES_Crypt(&handle->direct_ctx, AES_ID, in, len, out)) {
            return -1;
        }
    } else {
        /* Link mode: use link work */
        SEC_Eng_AES_Link_Config_Type *nc_link_cfg =
            (SEC_Eng_AES_Link_Config_Type *)hal_get_noncacheable_addr(&handle->link.link_cfg);

        Sec_Eng_AES_Enable_BE(AES_ID);
        Sec_Eng_AES_Enable_Link(AES_ID);

        if (SUCCESS != Sec_Eng_AES_Link_Work(AES_ID, (uint32_t)nc_link_cfg,
                                              in, len, out)) {
            Sec_Eng_AES_Disable_Link(AES_ID);
            return -1;
        }

        Sec_Eng_AES_Disable_Link(AES_ID);

        /* After link operation, mark IV as reuse for subsequent blocks */
        nc_link_cfg->aesIVSel = SEC_ENG_AES_USE_NEW;
        nc_link_cfg->aesDecKeySel = SEC_ENG_AES_USE_NEW;
    }

    /* Invalidate output buffer cache after DMA writes */
    L1C_DCACHE_CLEAN_INVALID_RANGE(out, len);

    return 0;
}

int sec_aes_encrypt(sec_aes_handle_t *handle, const uint8_t *in, uint32_t len, size_t offset, uint8_t *out)
{
    (void)offset;
    return sec_aes_crypt(handle, in, len, out);
}

int sec_aes_decrypt(sec_aes_handle_t *handle, const uint8_t *in, uint32_t len, size_t offset, uint8_t *out)
{
    (void)offset;
    return sec_aes_crypt(handle, in, len, out);
}

int sec_aes_deinit(sec_aes_handle_t *handle)
{
    if (!handle) {
        return -1;
    }

    if (handle->mode == SEC_MODE_DIRECT) {
        Sec_Eng_AES_Finish(AES_ID);
    }

    memset(handle, 0, sizeof(sec_aes_handle_t));

    return 0;
}
