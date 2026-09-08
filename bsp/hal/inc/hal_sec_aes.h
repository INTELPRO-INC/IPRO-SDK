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

#ifndef __HAL_SEC_AES__H__
#define __HAL_SEC_AES__H__

#ifdef __cplusplus
extern "C"{
#endif

#include "hal_common.h"
#include "drv_sec_eng.h"

/**
 * @brief Security engine operation mode
 */
typedef enum {
    SEC_MODE_DIRECT = 0,    /**< Direct register access mode */
    SEC_MODE_LINK           /**< Link mode - config in RAM, DMA based */
} sec_mode_t;

/**
 * @brief AES encryption type
 */
typedef enum {
    SEC_AES_CBC,
    SEC_AES_CTR,
    SEC_AES_ECB
} sec_aes_type;

/**
 * @brief AES key length type
 */
typedef enum {
    SEC_AES_KEY_128,
    SEC_AES_KEY_256,
    SEC_AES_KEY_192
} sec_aes_key_type;

/**
 * @brief AES direction type
 */
typedef enum {
    SEC_AES_DIR_ENCRYPT,
    SEC_AES_DIR_DECRYPT
} sec_aes_dir_type;

/**
 * @brief AES handle structure supporting both direct and link modes
 *
 * For link mode, the link_cfg must be placed in non-cached memory region.
 * Use hal_get_noncacheable_addr() to get non-cached address when operating.
 *
 * Layout (struct must be 64-byte aligned):
 *   offset  0: mode/aes_type/key_type/dir  (16 bytes, CPU-only)
 *   offset 16: guard0                       (48 bytes padding)
 *   offset 64: link_cfg                     (own cache line, NC-accessed)
 *
 * guard0 ensures link_cfg starts on a separate 64-byte cache line,
 * so cacheable writes to mode/type/dir never share a line with link_cfg.
 */
typedef struct sec_aes_handle_t {
    sec_mode_t mode;            /**< Operation mode: direct or link */
    sec_aes_type aes_type;      /**< AES mode: CBC/CTR/ECB */
    sec_aes_key_type key_type;  /**< Key length: 128/192/256 bits */
    sec_aes_dir_type dir;       /**< Direction: encrypt/decrypt */

    union {
        /* Direct mode context */
        SEC_Eng_AES_Ctx direct_ctx;

        /* Link mode context */
        struct {
            uint32_t guard0[12];                    /**< Pad to cache line boundary */
            SEC_Eng_AES_Link_Config_Type link_cfg;  /**< Link configuration (NC-accessed) */
            uint32_t guard1[8];                     /**< Trailing guard */
        } link;
    };
} sec_aes_handle_t;

/**
 * @brief Initialize AES handle with specified mode
 *
 * @param handle AES handle pointer
 * @param mode Operation mode: SEC_MODE_DIRECT or SEC_MODE_LINK
 * @param aes_type AES type: CBC/CTR/ECB
 * @param key_type Key type: 128/192/256 bits
 * @return 0 on success, negative on error
 */
int sec_aes_init(sec_aes_handle_t *handle, sec_aes_type aes_type, sec_aes_key_type key_type);

/**
 * @brief Initialize AES handle with specified mode (extended version)
 *
 * @param handle AES handle pointer
 * @param mode Operation mode: SEC_MODE_DIRECT or SEC_MODE_LINK
 * @param aes_type AES type: CBC/CTR/ECB
 * @param key_type Key type: 128/192/256 bits
 * @return 0 on success, negative on error
 */
int sec_aes_init_ex(sec_aes_handle_t *handle, sec_mode_t mode, sec_aes_type aes_type, sec_aes_key_type key_type);

/**
 * @brief Set AES key and IV
 *
 * @param handle AES handle pointer
 * @param key Key buffer (NULL for hardware key)
 * @param key_len Key length in bytes (0 for hardware key)
 * @param nonce IV/nonce buffer
 * @param dir Direction: SEC_AES_DIR_ENCRYPT or SEC_AES_DIR_DECRYPT
 * @return 0 on success, negative on error
 */
int sec_aes_setkey(sec_aes_handle_t *handle, const uint8_t *key, uint8_t key_len, const uint8_t *nonce, uint8_t dir);

/**
 * @brief AES encryption
 *
 * @param handle AES handle pointer
 * @param in Input data buffer (must be non-cached for link mode)
 * @param len Data length (must be multiple of 16)
 * @param offset Offset (reserved)
 * @param out Output data buffer (must be non-cached for link mode)
 * @return 0 on success, negative on error
 */
int sec_aes_encrypt(sec_aes_handle_t *handle, const uint8_t *in, uint32_t len, size_t offset, uint8_t *out);

/**
 * @brief AES decryption
 *
 * @param handle AES handle pointer
 * @param in Input data buffer (must be non-cached for link mode)
 * @param len Data length (must be multiple of 16)
 * @param offset Offset (reserved)
 * @param out Output data buffer (must be non-cached for link mode)
 * @return 0 on success, negative on error
 */
int sec_aes_decrypt(sec_aes_handle_t *handle, const uint8_t *in, uint32_t len, size_t offset, uint8_t *out);

/**
 * @brief Deinitialize AES handle
 *
 * @param handle AES handle pointer
 * @return 0 on success, negative on error
 */
int sec_aes_deinit(sec_aes_handle_t *handle);

#ifdef __cplusplus
}
#endif
#endif
