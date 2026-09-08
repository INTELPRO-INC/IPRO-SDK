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

#ifndef _IPRO_AES_PORT_H
#define _IPRO_AES_PORT_H

#ifndef IPRO_CRYPT_HARDWARE
#include "mbedtls/aes.h"
#include "mbedtls/ccm.h"
#include "mbedtls/gcm.h"
#include "chacha.h"
#include "chacha20_poly1305.h"
#else
#include "drv_sec_eng.h"
#endif

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define IPRO_CRYPT_KEY_SIZE_MAX   		32
#define IPRO_CRYPT_NONCE_SIZE_MAX       16
#define IPRO_CRYPT_BLK_SIZE       		16

#ifndef IPRO_CRYPT_HARDWARE
typedef union tag_ipro_crypt_ctx_t
{
    mbedtls_aes_context ctx_aes;
    mbedtls_ccm_context ctx_ccm;
    mbedtls_gcm_context ctx_gcm;
    ChaCha              ctx_chacha;
    ChaChaPoly1305      ctx_chachapoly1305;
}ipro_crypt_ctx_t;
#endif

typedef struct tag_ipro_crypt_cfg_t
{
#ifndef IPRO_CRYPT_HARDWARE
	uint8_t key[IPRO_CRYPT_KEY_SIZE_MAX];
	//for general cipher,iv is 16 bytes, but for GCM vector test, there is a 60 bytes test
	uint8_t iv_nonce[IPRO_CRYPT_NONCE_SIZE_MAX*4];
	uint8_t stream_block[IPRO_CRYPT_BLK_SIZE];
	uint8_t key_len;
	uint8_t nonce_len;
	uint8_t dir;
#else
#endif
    uint8_t type;
}ipro_crypt_cfg_t;



#ifndef IPRO_CRYPT_HARDWARE
#define IPRO_CRYPT_DIR_ENCRYPT    MBEDTLS_AES_ENCRYPT
#define IPRO_CRYPT_DIR_DECRYPT    MBEDTLS_AES_DECRYPT
#else
#define IPRO_CRYPT_DIR_ENCRYPT    1
#define IPRO_CRYPT_DIR_DECRYPT    2
#endif

#endif
