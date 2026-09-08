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

#ifndef _IPRO_HASH_H
#define _IPRO_HASH_H

#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include "ipro_hash_port.h"
#include "platform_common.h"


#define ipro_hash_printf     printf
#define ipro_hash_printe     printf
#define ipro_hash_printd     printf
#define ipro_hash_printw     printf

#define IPRO_HASH_OK             0
#define IPRO_HASH_ERROR          -1

#define IPRO_HASH_TYPE_SHA1           0
#define IPRO_HASH_TYPE_SHA224       1
#define IPRO_HASH_TYPE_SHA256       2
#define IPRO_HASH_TYPE_SHA384       3
#define IPRO_HASH_TYPE_SHA512       4
   
typedef struct tag_ipro_hash_handle_t
{
    ipro_hash_ctx_t hash_ctx;
#ifdef IPRO_CRYPT_HARDWARE
    uint32_t sha_padding[64/4];			    //for sha finish compute, must located at OCRAM
#endif
    uint8_t type;
}ipro_hash_handle_t;

int32_t ipro_hash_init(ipro_hash_handle_t *hash_handle,uint8_t type);

uint8_t ipro_hash_get_type(uint8_t type);

int32_t ipro_hash_start(ipro_hash_handle_t *hash_handle);

int32_t ipro_hash_update(ipro_hash_handle_t *hash_handle,const uint8_t *in,uint32_t len);

int32_t ipro_hash_finish(ipro_hash_handle_t *hash_handle,uint8_t *out);

int32_t ipro_hash_deinit(ipro_hash_handle_t *hash_handle);

#endif
