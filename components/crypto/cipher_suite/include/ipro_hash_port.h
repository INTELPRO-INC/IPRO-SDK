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

#ifndef _IPRO_HASH_PORT_H
#define _IPRO_HASH_PORT_H

#ifndef IPRO_CRYPT_HARDWARE
#include "mbedtls/sha1.h"
#include "mbedtls/md.h"
#include "mbedtls/sha256.h"
#include "mbedtls/sha512.h"
#else
#include "drv_sec_eng.h"
#endif

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

typedef union tag_ipro_hash_ctx_t
{
#ifndef IPRO_CRYPT_HARDWARE
    mbedtls_sha1_context ctx_sha1;
    mbedtls_sha256_context ctx_sha256;
    mbedtls_sha512_context ctx_sha512;
#else
    uint32_t sha_buf[64/4];					//for sha finish compute, must located at OCRAM
#endif
}ipro_hash_ctx_t;


#endif
