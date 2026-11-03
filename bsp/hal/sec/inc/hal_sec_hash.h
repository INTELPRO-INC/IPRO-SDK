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

#ifndef __HAL_SEC_HASH__H__
#define __HAL_SEC_HASH__H__

#ifdef __cplusplus
extern "C"{
#endif

#include "hal_common.h"
#include "board_config.h"
#include "drv_sec_eng.h"

/**
 * @brief Security engine operation mode (redefined here to avoid header deps)
 */
#ifndef SEC_MODE_DEFINED
#define SEC_MODE_DEFINED
typedef enum {
    SEC_HASH_MODE_DIRECT = 0,   /**< Direct register access mode */
    SEC_HASH_MODE_LINK          /**< Link mode - config in RAM, DMA based */
} sec_hash_mode_t;
#endif

/**
 * @brief Hash algorithm type
 */
enum sec_hash_type {
    SEC_HASH_SHA1,
    SEC_HASH_SHA224,
    SEC_HASH_SHA256,
    SEC_HASH_SHA384,
    SEC_HASH_SHA512,
    SEC_HASH_UNKNOWN
};

/**
 * @brief Hash handle structure supporting both direct and link modes
 *
 * For link mode, the link_cfg must be placed in non-cached memory region.
 * Buffer sizes accommodate SHA-512 (128 bytes) which is the largest.
 */
typedef struct {
    sec_hash_mode_t mode;           /**< Operation mode: direct or link */
    uint8_t type;                   /**< Hash type (sec_hash_type) */
    uint64_t shaBuf[16];            /**< Data not processed but in this temp buffer (128 bytes for SHA-512) */
    uint64_t shaPadding[16];        /**< Padding data (128 bytes for SHA-512) */

    union {
        /* Direct mode context - SHA-256 family (SHA-1/224/256) */
        SEC_Eng_SHA256_Ctx sha256_ctx;
        /* Direct mode context - SHA-512 family (SHA-384/512) */
        SEC_Eng_SHA512_Ctx sha512_ctx;

        /* Link mode context */
        struct {
            union {
                SEC_Eng_SHA256_Link_Ctx sha256_link_ctx;
                SEC_Eng_SHA512_Link_Ctx sha512_link_ctx;
            };
            uint32_t guard0[8];                     /**< Cache line guard */
            SEC_Eng_SHA_Link_Config_Type link_cfg;  /**< Link configuration */
            uint32_t guard1[8];                     /**< Cache line guard */
        } link;
    };
} sec_hash_handle_t;

/**
 * @brief Initialize hash handle (default: direct mode)
 *
 * @param handle Hash handle pointer
 * @param type Hash algorithm type
 * @return 0 on success, negative on error
 */
int sec_hash_init(sec_hash_handle_t *handle, uint8_t type);

/**
 * @brief Initialize hash handle with specified mode
 *
 * @param handle Hash handle pointer
 * @param mode Operation mode: SEC_HASH_MODE_DIRECT or SEC_HASH_MODE_LINK
 * @param type Hash algorithm type
 * @return 0 on success, negative on error
 */
int sec_hash_init_ex(sec_hash_handle_t *handle, sec_hash_mode_t mode, uint8_t type);

/**
 * @brief Deinitialize hash handle
 *
 * @param handle Hash handle pointer
 * @return 0 on success, negative on error
 */
int sec_hash_deinit(sec_hash_handle_t *handle);

/**
 * @brief Update hash with data
 *
 * @param handle Hash handle pointer
 * @param buffer Input data buffer (must be non-cached for link mode)
 * @param size Data size in bytes
 * @return 0 on success, negative on error
 */
int sec_hash_update(sec_hash_handle_t *handle, const void *buffer, uint32_t size);

/**
 * @brief Finish hash computation and get result
 *
 * @param handle Hash handle pointer
 * @param buffer Output hash buffer (must be non-cached for link mode)
 * @return Hash length on success, negative on error
 */
int sec_hash_finish(sec_hash_handle_t *handle, void *buffer);

#ifdef __cplusplus
}
#endif
#endif
