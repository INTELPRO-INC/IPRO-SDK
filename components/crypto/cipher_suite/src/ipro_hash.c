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

#include "ipro_hash.h"

#ifndef IPRO_CRYPT_HARDWARE
#include "mbedtls/sha1.h"
#include "mbedtls/md.h"
#include "mbedtls/sha256.h"
#include "mbedtls/sha512.h"
uint8_t ipro_hash_get_type(uint8_t type)
{
    switch(type)
    {
        case IPRO_HASH_TYPE_SHA1:
            return MBEDTLS_MD_SHA1;
        case IPRO_HASH_TYPE_SHA224:
            return MBEDTLS_MD_SHA224;
        case IPRO_HASH_TYPE_SHA256:
            return MBEDTLS_MD_SHA256;
        case IPRO_HASH_TYPE_SHA384:
            return MBEDTLS_MD_SHA384;
        case IPRO_HASH_TYPE_SHA512:
            return MBEDTLS_MD_SHA512;
    }
    return MBEDTLS_MD_NONE;
}
#endif
static SEC_Eng_SHA256_Ctx shaCtx;
int32_t ipro_hash_init(ipro_hash_handle_t *hash_handle,uint8_t type)
{
    int32_t ret = IPRO_HASH_OK;


    switch(type)
    {
        case IPRO_HASH_TYPE_SHA1 :
        	ret=IPRO_HASH_ERROR;
        	ipro_hash_printe("unsupported type\r\n");
            break;
        case IPRO_HASH_TYPE_SHA224:
            Sec_Eng_SHA256_Init(&shaCtx,SEC_ENG_SHA_ID0,SEC_ENG_SHA224,(uint32_t *)hash_handle->hash_ctx.sha_buf,
                                                                (uint32_t *)hash_handle->sha_padding);
        case IPRO_HASH_TYPE_SHA256 :
            Sec_Eng_SHA256_Init(&shaCtx,SEC_ENG_SHA_ID0,SEC_ENG_SHA256,(uint32_t *)hash_handle->hash_ctx.sha_buf,
                                                                (uint32_t *)hash_handle->sha_padding);
            break;
        case IPRO_HASH_TYPE_SHA384:
        case IPRO_HASH_TYPE_SHA512:
        	ret=IPRO_HASH_ERROR;
        	ipro_hash_printe("unsupported type\r\n");
            break;
        default:
            ipro_hash_printe("unsupported type\r\n");
            ret=IPRO_HASH_ERROR;
            break;
            
    }
    if(ret==IPRO_HASH_OK){
        hash_handle->type=type;
    }
    return ret;
}

int32_t ipro_hash_start(ipro_hash_handle_t *hash_handle)
{
    int32_t ret = IPRO_HASH_OK;


  
    switch(hash_handle->type)
    {
        case IPRO_HASH_TYPE_SHA1:
            break;
        case IPRO_HASH_TYPE_SHA224:
        	 Sec_Eng_SHA_Start(SEC_ENG_SHA_ID0);
            break;
        case IPRO_HASH_TYPE_SHA256:
        	 Sec_Eng_SHA_Start(SEC_ENG_SHA_ID0);
            break;
        case IPRO_HASH_TYPE_SHA384:
            break;
        case IPRO_HASH_TYPE_SHA512:
            break;
        default:
            ipro_hash_printe("unsupported type\r\n");
            return IPRO_HASH_ERROR;
            
    }
    if(ret!=0){
    	ipro_hash_printe("hash start fail\r\n");
        ret=IPRO_HASH_ERROR;
    }
    return ret;
}

int32_t ipro_hash_update(ipro_hash_handle_t *hash_handle,const uint8_t *in,uint32_t len)
{
    int32_t ret = IPRO_HASH_OK;

    switch(hash_handle->type)
    {
        case IPRO_HASH_TYPE_SHA1:
            break;
	    case IPRO_HASH_TYPE_SHA224:
	    	Sec_Eng_SHA256_Update(&shaCtx,SEC_ENG_SHA_ID0,in, len);
	    	break;
        case IPRO_HASH_TYPE_SHA256:
        	Sec_Eng_SHA256_Update(&shaCtx,SEC_ENG_SHA_ID0,in, len);
            break;
        case IPRO_HASH_TYPE_SHA384:
            break;
        case IPRO_HASH_TYPE_SHA512:
            break;
        default:
            ipro_hash_printe("unsupported type\r\n");
            return IPRO_HASH_ERROR;
            
    }
    if(ret!=0){
    	ipro_hash_printe("hash update fail\r\n");
        ret=IPRO_HASH_ERROR;
    }

    return ret;
}

int32_t ipro_hash_finish(ipro_hash_handle_t *hash_handle,uint8_t *out)
{
    int32_t ret = IPRO_HASH_OK;

    switch(hash_handle->type)
    {
        case IPRO_HASH_TYPE_SHA1:
            break;
        case IPRO_HASH_TYPE_SHA224:
        	Sec_Eng_SHA256_Finish(&shaCtx,SEC_ENG_SHA_ID0,out);
        	break;
        case IPRO_HASH_TYPE_SHA256:
        	Sec_Eng_SHA256_Finish(&shaCtx,SEC_ENG_SHA_ID0,out);
            break;
        case IPRO_HASH_TYPE_SHA384:
            break;
        case IPRO_HASH_TYPE_SHA512:
            break;
        default:
            ipro_hash_printe("unsupported type\r\n");
            return IPRO_HASH_ERROR;
            
    }
    if(ret!=0){
    	ipro_hash_printe("hash finish fail\r\n");
        ret=IPRO_HASH_ERROR;
    }

    return ret;
}

int32_t ipro_hash_deinit(ipro_hash_handle_t *hash_handle)
{
    switch(hash_handle->type)
    {
        case IPRO_HASH_TYPE_SHA1:
            break;
        case IPRO_HASH_TYPE_SHA224:
        	break;
        case IPRO_HASH_TYPE_SHA256:
            break;
        case IPRO_HASH_TYPE_SHA384:
            break;
        case IPRO_HASH_TYPE_SHA512:
            break;
        default:
            ipro_hash_printe("unsupported type\r\n");
            return IPRO_HASH_ERROR;
            
    }
    memset(hash_handle,0,sizeof(ipro_hash_handle_t));

    return IPRO_HASH_OK;
}
