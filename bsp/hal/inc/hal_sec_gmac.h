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

#ifndef __HAL_SEC_GMAC__H__
#define __HAL_SEC_GMAC__H__

#ifdef __cplusplus
extern "C"{
#endif

#include "hal_common.h"
#include "drv_sec_eng.h"

/**
 * @brief GMAC handle structure
 *
 * GMAC uses link mode only. The link_cfg must be in non-cached memory region.
 * Use hal_get_noncacheable_addr() to get non-cached address when operating.
 */
typedef struct {
    uint32_t guard0[8];                     /**< Cache line guard */
    SEC_Eng_GMAC_Link_Config_Type link_cfg; /**< Link configuration */
    uint32_t guard1[8];                     /**< Cache line guard */
    uint8_t tmp_buf[16];                    /**< Temp buffer for partial blocks */
} sec_gmac_handle_t;

/**
 * @brief Initialize GMAC hardware
 *
 * Must be called once before using GMAC functions.
 *
 * @return 0 on success, negative on error
 */
int sec_gmac_init(void);

/**
 * @brief Initialize GMAC handle with key
 *
 * @param handle GMAC handle pointer
 * @param key 16-byte key for GHASH
 * @return 0 on success, negative on error
 */
int sec_gmac_start(sec_gmac_handle_t *handle, const uint8_t key[16]);

/**
 * @brief Clear GMAC result (reset for new operation with same key)
 *
 * @param handle GMAC handle pointer
 * @return 0 on success, negative on error
 */
int sec_gmac_clear(sec_gmac_handle_t *handle);

/**
 * @brief Update GMAC with data
 *
 * Input length does not need to be multiple of 16.
 * Partial blocks are padded with zeros.
 *
 * @param handle GMAC handle pointer
 * @param input Input data buffer
 * @param input_len Input data length
 * @return 0 on success, negative on error
 */
int sec_gmac_update(sec_gmac_handle_t *handle, const uint8_t *input, uint32_t input_len);

/**
 * @brief Finish GMAC and get result
 *
 * @param handle GMAC handle pointer
 * @param result 16-byte output buffer for GMAC result
 * @return 0 on success, negative on error
 */
int sec_gmac_finish(sec_gmac_handle_t *handle, uint8_t result[16]);

/**
 * @brief Deinitialize GMAC handle
 *
 * @param handle GMAC handle pointer
 * @return 0 on success, negative on error
 */
int sec_gmac_deinit(sec_gmac_handle_t *handle);

#ifdef __cplusplus
}
#endif
#endif
