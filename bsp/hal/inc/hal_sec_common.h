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

#pragma once

#include <stdint.h>
#include <stddef.h>

#include <FreeRTOS.h>
#include <task.h>

/*
 * SEC engine bring-up entry point (implemented in hal_sec_common.c).
 * Initialises the SEC_ENG sub-blocks the platform needs (TRNG today).
 */
int hal_sec_common_init(void);

typedef enum {
    HAL_SHA1,
    HAL_SHA224,
    HAL_SHA256,
    HAL_MD5,
    HAL_SHA384,
    HAL_SHA512,
} hal_sha_type_t;

/*
 * SHA1/SHA224/SHA256
 */
/* copied SEC_Eng_SHA256_Ctx from stddrv */
typedef struct {
    uint32_t total[2];
    uint32_t  *shaBuf;
    uint32_t  *shaPadding;
    uint32_t linkAddr;
} hal_SEC_Eng_SHA256_Link_Ctx;

// used for both SHA1/SHA224/SHA256 and SHA512
typedef struct {
    uint32_t shaCfgWord;
    uint32_t shaSrcAddr;                     /*!< Message source address */
    uint32_t result[16];                      /*!< Result of SHA */
} __attribute__ ((aligned(4))) hal_SEC_Eng_SHA_Link_Config_Type;

typedef struct hal_sha_ctx {
    uint32_t guard0_[7];
    hal_sha_type_t type;
    hal_SEC_Eng_SHA256_Link_Ctx ctx;
    hal_SEC_Eng_SHA_Link_Config_Type link_cfg;
    uint32_t tmp[16];
    uint32_t pad[16];
    uint32_t guard1_[7];
} hal_sha_ctx_t;


/*
 * SHA384/512
 */
// copied SEC_Eng_SHA512_Link_Ctx from stddrv */
typedef struct
{
    uint64_t total[2];    /*!< Number of bytes processed */
    uint64_t *shaBuf;     /*!< Data not processed but in this temp buffer */
    uint64_t *shaPadding; /*!< Padding data */
    uint32_t linkAddr;    /*!< Link configure address */
} hal_SEC_Eng_SHA512_Link_Ctx;

typedef struct hal_sha512_ctx {
    uint32_t guard0_[7];
    hal_sha_type_t type;
    hal_SEC_Eng_SHA512_Link_Ctx ctx;
    hal_SEC_Eng_SHA_Link_Config_Type link_cfg;
    uint64_t tmp[16];
    uint64_t pad[16];
    uint32_t guard1_[7];
} hal_sha512_ctx_t;

static inline unsigned long hal_sec_enter_critical()
{
    taskENTER_CRITICAL();
    return 0;
}

static inline void hal_sec_exit_critical(unsigned long prev_level)
{
    taskEXIT_CRITICAL();
}
