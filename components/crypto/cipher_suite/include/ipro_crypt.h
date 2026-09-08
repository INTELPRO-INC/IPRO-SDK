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

#ifndef _IPRO_CRYPT_H
#define _IPRO_CRYPT_H

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ipro_crypt_port.h"
#include "platform_common.h"


#define ipro_crypt_printf     		printf
#define ipro_crypt_printe		 	printf
#define ipro_crypt_printd    	    printf
#define ipro_crypt_printw     		printf


#define IPRO_CRYPT_OK             0
#define IPRO_CRYPT_ERROR          -1

#define IPRO_CRYPT_TYPE_AES_CBC       			0
#define IPRO_CRYPT_TYPE_AES_CTR       			1
#define IPRO_CRYPT_TYPE_CHACHA    				2
#define IPRO_CRYPT_TYPE_AES_CCM       			3
#define IPRO_CRYPT_TYPE_AES_GCM       			4
#define IPRO_CRYPT_TYPE_CHACHAPOLY1305       	5

typedef struct tag_ipro_crypt_handle_t
{
#ifndef IPRO_CRYPT_HARDWARE
    ipro_crypt_ctx_t crypt_ctx;
#endif
    ipro_crypt_cfg_t crypt_cfg;
}ipro_crypt_handle_t;

int32_t ipro_crypt_init(ipro_crypt_handle_t *crypt_handle,uint8_t type);

int32_t ipro_crypt_setkey(ipro_crypt_handle_t *aes_handle,const uint8_t *key,
		uint8_t keytype,uint8_t key_len,const uint8_t *nonce,uint8_t nonce_len,uint8_t dir);

int32_t ipro_crypt_setadd(ipro_crypt_handle_t *aes_handle,const uint8_t *add,
		uint8_t len,uint8_t dir);

int32_t ipro_crypt_update(ipro_crypt_handle_t *aes_handle,const uint8_t *in,
		uint32_t len,uint8_t *out);

int32_t ipro_crypt_encrypt(ipro_crypt_handle_t *aes_handle,const uint8_t *in,
		uint32_t len,size_t offset,uint8_t *out);

int32_t ipro_crypt_encrypt_tag(ipro_crypt_handle_t *aes_handle,const uint8_t *in,
		uint32_t in_len,const uint8_t *add,uint32_t add_len,size_t offset,
		uint8_t *out,uint8_t *tag,uint8_t tag_len);

int32_t ipro_crypt_auth_decrypt(ipro_crypt_handle_t *aes_handle,const uint8_t *in,
		uint32_t in_len,const uint8_t *add,uint32_t add_len,size_t offset,
		uint8_t *out,const uint8_t *tag,uint8_t tag_len);

int32_t ipro_crypt_decrypt(ipro_crypt_handle_t *aes_handle,const uint8_t *in,
		uint32_t len,size_t offset,uint8_t *out);

int32_t ipro_crypt_finish(ipro_crypt_handle_t *aes_handle,uint8_t *tag,uint32_t len);

int32_t ipro_crypt_deinit(ipro_crypt_handle_t *aes_handle);
#endif
