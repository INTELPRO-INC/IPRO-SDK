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

#include "ipro_crypt.h"

#ifndef IPRO_CRYPT_HARDWARE
int32_t ipro_crypt_init_do(ipro_crypt_handle_t *crypt_handle,uint8_t type)
{
    int32_t ret = IPRO_CRYPT_OK;

    switch(type)
	{
		case IPRO_CRYPT_TYPE_AES_CBC:
		case IPRO_CRYPT_TYPE_AES_CTR:
			mbedtls_aes_init((mbedtls_aes_context*)&crypt_handle->crypt_ctx);
			break;
		case IPRO_CRYPT_TYPE_CHACHA:
			memset((ChaCha*)&crypt_handle->crypt_ctx,0,sizeof(ChaCha));
			break;
		case IPRO_CRYPT_TYPE_AES_CCM:
			mbedtls_ccm_init((mbedtls_ccm_context *)&crypt_handle->crypt_ctx);
			break;
		case IPRO_CRYPT_TYPE_AES_GCM:
			mbedtls_gcm_init((mbedtls_gcm_context *)&crypt_handle->crypt_ctx);
			break;
		case IPRO_CRYPT_TYPE_CHACHAPOLY1305:
			memset((ChaChaPoly1305*)&crypt_handle->crypt_ctx,0,sizeof(ChaChaPoly1305));
			break;
		default:
			ipro_crypt_printe("unsupported type\r\n");
			ret=IPRO_CRYPT_ERROR;
			break;
	}
    return ret;
}

int32_t ipro_crypt_setkey_do(ipro_crypt_handle_t *crypt_handle,const uint8_t *key,uint8_t key_len,
                        const uint8_t *nonce,uint8_t nonce_len,uint8_t dir)
{
    int32_t ret = IPRO_CRYPT_OK;

	memset(crypt_handle->crypt_cfg.key,0,sizeof(crypt_handle->crypt_cfg.key));
	memset(crypt_handle->crypt_cfg.iv_nonce,0,sizeof(crypt_handle->crypt_cfg.iv_nonce));
	memset(crypt_handle->crypt_cfg.stream_block,0,sizeof(crypt_handle->crypt_cfg.stream_block));

	memcpy(crypt_handle->crypt_cfg.key,key,key_len);
	memcpy(crypt_handle->crypt_cfg.iv_nonce,nonce,nonce_len);
	crypt_handle->crypt_cfg.key_len=key_len;
	crypt_handle->crypt_cfg.nonce_len=nonce_len;

    switch(crypt_handle->crypt_cfg.type)
    {
        case IPRO_CRYPT_TYPE_AES_CBC:
            if(dir==IPRO_CRYPT_DIR_ENCRYPT){
                ret=mbedtls_aes_setkey_enc((mbedtls_aes_context*)&crypt_handle->crypt_ctx,
                                        key, key_len*8);
            }else{
                ret=mbedtls_aes_setkey_dec((mbedtls_aes_context*)&crypt_handle->crypt_ctx,
                                        key, key_len*8);
            }
            break;
        case IPRO_CRYPT_TYPE_AES_CTR:
            ret=mbedtls_aes_setkey_enc((mbedtls_aes_context*)&crypt_handle->crypt_ctx,
                                        key, key_len*8);
            break;
        case IPRO_CRYPT_TYPE_CHACHA:
		   ret|=Chacha_SetIV((ChaCha*)&crypt_handle->crypt_ctx, nonce, nonce_len,0);
		   ret|=Chacha_SetKey((ChaCha*)&crypt_handle->crypt_ctx, key, key_len);
		   break;
        case IPRO_CRYPT_TYPE_AES_CCM:
            ret=mbedtls_ccm_setkey((mbedtls_ccm_context*)&crypt_handle->crypt_ctx,
                                    MBEDTLS_CIPHER_ID_AES, key, key_len*8 ) ;
            break;
        case IPRO_CRYPT_TYPE_AES_GCM:
            ret=mbedtls_gcm_setkey((mbedtls_gcm_context*)&crypt_handle->crypt_ctx,
                                    MBEDTLS_CIPHER_ID_AES, key, key_len*8 ) ;
            break;
        case IPRO_CRYPT_TYPE_CHACHAPOLY1305:
        	break;
        default:
            ipro_crypt_printe("unsupported type\r\n");
            return IPRO_CRYPT_ERROR;
    }

    return ret;
}

int32_t ipro_crypt_setadd_do(ipro_crypt_handle_t *crypt_handle,const uint8_t *add,uint8_t len,
                            uint8_t dir)
{
    int32_t ret = IPRO_CRYPT_OK;

    switch(crypt_handle->crypt_cfg.type)
    {
        case IPRO_CRYPT_TYPE_AES_GCM:
            ret = mbedtls_gcm_starts( (mbedtls_gcm_context*)&crypt_handle->crypt_ctx,
                    dir==IPRO_CRYPT_DIR_ENCRYPT?MBEDTLS_GCM_ENCRYPT:MBEDTLS_GCM_DECRYPT,
                    crypt_handle->crypt_cfg.iv_nonce, crypt_handle->crypt_cfg.nonce_len,add,len);
            break;
        default:
            ipro_crypt_printe("unsupported type\r\n");
            return IPRO_CRYPT_ERROR;
    }
    return ret;
}

int32_t ipro_crypt_encrypt_do(ipro_crypt_handle_t *crypt_handle,const uint8_t *in,uint32_t len,
                        size_t offset,uint8_t *out)
{
    int result;
    size_t offset2=offset;


    switch(crypt_handle->crypt_cfg.type)
    {
        case IPRO_CRYPT_TYPE_AES_CBC:
            if(len%IPRO_CRYPT_BLK_SIZE!=0){
                ipro_crypt_printe("input size must be IPRO_CRYPT_BLK_SIZE align\r\n");
                return IPRO_CRYPT_ERROR;
            }
            result=mbedtls_aes_crypt_cbc((mbedtls_aes_context*)&crypt_handle->crypt_ctx,
                    MBEDTLS_AES_ENCRYPT, len, crypt_handle->crypt_cfg.iv_nonce, in, out);
            break;
        case IPRO_CRYPT_TYPE_AES_CTR:
			result=mbedtls_aes_crypt_ctr((mbedtls_aes_context*)&crypt_handle->crypt_ctx,
					len, &offset2, crypt_handle->crypt_cfg.iv_nonce, crypt_handle->crypt_cfg.stream_block,
					in, out );
			break;
        case IPRO_CRYPT_TYPE_CHACHA:
        	result=Chacha_Process((ChaCha*)&crypt_handle->crypt_ctx,out,in,len);
        	break;
        default:
            ipro_crypt_printe("unsupported type\r\n");
            return IPRO_CRYPT_ERROR;

    }
    return result;
}

int32_t ipro_crypt_encrypt_tag_do(ipro_crypt_handle_t *crypt_handle,const uint8_t *in,uint32_t in_len,
                        const uint8_t *add,uint32_t add_len,size_t offset,uint8_t *out,
                        uint8_t *tag,uint8_t tag_len)
{
    int result;

    switch(crypt_handle->crypt_cfg.type)
    {
        case IPRO_CRYPT_TYPE_AES_CCM:
            result=mbedtls_ccm_encrypt_and_tag((mbedtls_ccm_context*)&crypt_handle->crypt_ctx,
                    in_len,crypt_handle->crypt_cfg.iv_nonce, crypt_handle->crypt_cfg.nonce_len,add,add_len,in,out,
                    tag,tag_len);
            break;
         case IPRO_CRYPT_TYPE_AES_GCM:
            result = mbedtls_gcm_crypt_and_tag((mbedtls_gcm_context*)&crypt_handle->crypt_ctx,
                    MBEDTLS_GCM_ENCRYPT,in_len,crypt_handle->crypt_cfg.iv_nonce,crypt_handle->crypt_cfg.nonce_len,
                    add,add_len,in,out,tag_len,tag);
            break;
         case IPRO_CRYPT_TYPE_CHACHAPOLY1305:
			result = ChaCha20Poly1305_Encrypt((ChaChaPoly1305*)&crypt_handle->crypt_ctx,
					crypt_handle->crypt_cfg.key,crypt_handle->crypt_cfg.key_len,
					crypt_handle->crypt_cfg.iv_nonce,crypt_handle->crypt_cfg.nonce_len,
					add,add_len,in,in_len,out,tag,tag_len);
			break;
        default:
        	ipro_crypt_printe("unsupported type\r\n");
            return IPRO_CRYPT_ERROR;

    }
    return result;
}

int32_t ipro_crypt_update_do(ipro_crypt_handle_t *crypt_handle,const uint8_t *in,uint32_t len,
                        uint8_t *out)
{
    int result;

    switch(crypt_handle->crypt_cfg.type)
    {
        case IPRO_CRYPT_TYPE_AES_GCM:
            result=mbedtls_gcm_update((mbedtls_gcm_context*)&crypt_handle->crypt_ctx,
                                        len, in, out);
            break;
        default:
            ipro_crypt_printe("unsupported type\r\n");
            return IPRO_CRYPT_ERROR;
            break;

    }
    return result;
}


int32_t ipro_crypt_decrypt_do(ipro_crypt_handle_t *crypt_handle,const uint8_t *in,uint32_t len,
                        size_t offset,uint8_t *out)
{
    int result;
    size_t offset2=offset;

    switch(crypt_handle->crypt_cfg.type)
    {
        case IPRO_CRYPT_TYPE_AES_CBC:
            if(len%IPRO_CRYPT_BLK_SIZE!=0){
                ipro_crypt_printe("input size must be IPRO_CRYPT_BLK_SIZE align\r\n");
                return IPRO_CRYPT_ERROR;
            }
            result=mbedtls_aes_crypt_cbc((mbedtls_aes_context*)&crypt_handle->crypt_ctx,
                    MBEDTLS_AES_DECRYPT, len, crypt_handle->crypt_cfg.iv_nonce, in, out);
            break;
        case IPRO_CRYPT_TYPE_AES_CTR:
            result=mbedtls_aes_crypt_ctr((mbedtls_aes_context*)&crypt_handle->crypt_ctx,
                    len, &offset2, crypt_handle->crypt_cfg.iv_nonce,  crypt_handle->crypt_cfg.stream_block,
                    in, out );
            break;
        case IPRO_CRYPT_TYPE_CHACHA:
			 result=Chacha_Process((ChaCha*)&crypt_handle->crypt_ctx,out,in,len);
			 break;
        default:
            ipro_crypt_printe("unsupported type\r\n");
            return IPRO_CRYPT_ERROR;

    }

    return result;
}

int32_t ipro_crypt_auth_decrypt_do(ipro_crypt_handle_t *crypt_handle,const uint8_t *in,uint32_t in_len,
                        const uint8_t *add,uint32_t add_len,size_t offset,uint8_t *out,
                        const uint8_t *tag,uint8_t tag_len)
{
    int result;

    switch(crypt_handle->crypt_cfg.type)
    {
        case IPRO_CRYPT_TYPE_AES_CCM:
            result=mbedtls_ccm_auth_decrypt((mbedtls_ccm_context*)&crypt_handle->crypt_ctx,
                    in_len,crypt_handle->crypt_cfg.iv_nonce, crypt_handle->crypt_cfg.nonce_len,add,add_len,in,out,
                    tag,tag_len);
            break;
        case IPRO_CRYPT_TYPE_AES_GCM:
            result=mbedtls_gcm_auth_decrypt((mbedtls_gcm_context*)&crypt_handle->crypt_ctx,
                    in_len,(const uint8_t *)crypt_handle->crypt_cfg.iv_nonce,crypt_handle->crypt_cfg.nonce_len,
                    add,add_len,tag,tag_len,in,out);
            break;
        case IPRO_CRYPT_TYPE_CHACHAPOLY1305:
			result = ChaCha20Poly1305_Decrypt((ChaChaPoly1305*)&crypt_handle->crypt_ctx,
					crypt_handle->crypt_cfg.key,crypt_handle->crypt_cfg.key_len,
					crypt_handle->crypt_cfg.iv_nonce,crypt_handle->crypt_cfg.nonce_len,
					add,add_len,in,in_len,tag,tag_len,out);
			break;
        default:
        	ipro_crypt_printe("unsupported type\r\n");
            return IPRO_CRYPT_ERROR;

    }
    return result;
}

int32_t ipro_crypt_finish_do(ipro_crypt_handle_t *crypt_handle,uint8_t *tag,uint32_t len)
{
    int result;

    switch(crypt_handle->crypt_cfg.type)
    {
        case IPRO_CRYPT_TYPE_AES_GCM:
            result= mbedtls_gcm_finish((mbedtls_gcm_context*)&crypt_handle->crypt_ctx,
                                            tag, len );
            break;
        default:
        	ipro_crypt_printe("unsupported type\r\n");
            return IPRO_CRYPT_ERROR;

    }
    return result;
}


int32_t ipro_crypt_deinit_do(ipro_crypt_handle_t *crypt_handle)
{
    switch(crypt_handle->crypt_cfg.type)
    {
        case IPRO_CRYPT_TYPE_AES_CBC:
        case IPRO_CRYPT_TYPE_AES_CTR:
             mbedtls_aes_free((mbedtls_aes_context*)&crypt_handle->crypt_ctx);
            break;
        case IPRO_CRYPT_TYPE_AES_CCM:
             mbedtls_ccm_free((mbedtls_ccm_context*)&crypt_handle->crypt_ctx);
            break;
        case IPRO_CRYPT_TYPE_AES_GCM:
             mbedtls_gcm_free((mbedtls_gcm_context*)&crypt_handle->crypt_ctx);
            break;
        case IPRO_CRYPT_TYPE_CHACHA:
        case IPRO_CRYPT_TYPE_CHACHAPOLY1305:
        	break;
        default:
        	ipro_crypt_printe("unsupported type\r\n");
            return IPRO_CRYPT_ERROR;

    }
    return IPRO_CRYPT_OK;
}
#else
static SEC_Eng_AES_Ctx aesCtx;
int32_t ipro_crypt_init_do(ipro_crypt_handle_t *crypt_handle,uint8_t type)
{
    int32_t ret = IPRO_CRYPT_OK;

    return ret;
}

int32_t ipro_crypt_setkey_do(ipro_crypt_handle_t *crypt_handle,const uint8_t *key, uint8_t keytype, uint8_t key_len,
                        const uint8_t *nonce,uint8_t nonce_len,uint8_t dir)
{
	int32_t ret = IPRO_CRYPT_OK;

	switch(crypt_handle->crypt_cfg.type)
	{
		case IPRO_CRYPT_TYPE_AES_CBC:
			Sec_Eng_AES_Enable_BE(SEC_ENG_AES_ID0);
            Sec_Eng_AES_Init(&aesCtx,SEC_ENG_AES_ID0,SEC_ENG_AES_CBC,(SEC_ENG_AES_Key_Type)keytype,
					IPRO_CRYPT_DIR_ENCRYPT==dir?SEC_ENG_AES_ENCRYPTION:SEC_ENG_AES_DECRYPTION);
			break;
		case IPRO_CRYPT_TYPE_AES_CTR:
			Sec_Eng_AES_Enable_BE(SEC_ENG_AES_ID0);
			Sec_Eng_AES_Init(&aesCtx,SEC_ENG_AES_ID0,SEC_ENG_AES_CTR,(SEC_ENG_AES_Key_Type)keytype,
					IPRO_CRYPT_DIR_ENCRYPT==dir?SEC_ENG_AES_ENCRYPTION:SEC_ENG_AES_DECRYPTION);
			break;
		case IPRO_CRYPT_TYPE_AES_CCM:
			break;
		case IPRO_CRYPT_TYPE_AES_GCM:
			break;
		case IPRO_CRYPT_TYPE_CHACHA:
		case IPRO_CRYPT_TYPE_CHACHAPOLY1305:
			break;
		default:
			ipro_crypt_printe("unsupported type\r\n");
			return IPRO_CRYPT_ERROR;
	}

	/* if key len is 0, means key is from efuse and *key value is key_sel value */
	if(key_len==0){
		Sec_Eng_AES_Set_Key_IV_BE(SEC_ENG_AES_ID0,SEC_ENG_AES_KEY_HW,key,nonce);
	}else{
		Sec_Eng_AES_Set_Key_IV_BE(SEC_ENG_AES_ID0,SEC_ENG_AES_KEY_SW,key,nonce);
	}

	return ret;
}

int32_t ipro_crypt_setadd_do(ipro_crypt_handle_t *crypt_handle,const uint8_t *add,uint8_t len,
                            uint8_t dir)
{
    int32_t ret = IPRO_CRYPT_OK;


    return ret;
}

int32_t ipro_crypt_encrypt_do(ipro_crypt_handle_t *crypt_handle,const uint8_t *in,uint32_t len,
                        size_t offset,uint8_t *out)
{
    int result=IPRO_CRYPT_OK;

    Sec_Eng_AES_Crypt(&aesCtx,SEC_ENG_AES_ID0,in, len,out);

    return result;
}

int32_t ipro_crypt_encrypt_tag_do(ipro_crypt_handle_t *crypt_handle,const uint8_t *in,uint32_t in_len,
                        const uint8_t *add,uint32_t add_len,size_t offset,uint8_t *out,
                        uint8_t *tag,uint8_t tag_len)
{
    int result=IPRO_CRYPT_OK;


    return result;
}

int32_t ipro_crypt_update_do(ipro_crypt_handle_t *crypt_handle,const uint8_t *in,uint32_t len,
                        uint8_t *out)
{
    int result=IPRO_CRYPT_OK;

    Sec_Eng_AES_Crypt(&aesCtx,SEC_ENG_AES_ID0,in, len,out);

    return result;
}


int32_t ipro_crypt_decrypt_do(ipro_crypt_handle_t *crypt_handle,const uint8_t *in,uint32_t len,
                        size_t offset,uint8_t *out)
{
    int result=IPRO_CRYPT_OK;

    Sec_Eng_AES_Crypt(&aesCtx,SEC_ENG_AES_ID0,in, len,out);

    return result;
}

int32_t ipro_crypt_auth_decrypt_do(ipro_crypt_handle_t *crypt_handle,const uint8_t *in,uint32_t in_len,
                        const uint8_t *add,uint32_t add_len,size_t offset,uint8_t *out,
                        const uint8_t *tag,uint8_t tag_len)
{
    int result=IPRO_CRYPT_OK;


    return result;
}

int32_t ipro_crypt_finish_do(ipro_crypt_handle_t *crypt_handle,uint8_t *tag,uint32_t len)
{
    int result=IPRO_CRYPT_OK;

    Sec_Eng_AES_Finish(SEC_ENG_AES_ID0);

    return result;
}


int32_t ipro_crypt_deinit_do(ipro_crypt_handle_t *crypt_handle)
{
    switch(crypt_handle->crypt_cfg.type)
    {
        case IPRO_CRYPT_TYPE_AES_CBC:
        case IPRO_CRYPT_TYPE_AES_CTR:
            break;
        default:
        	ipro_crypt_printe("unsupported type\r\n");
            return IPRO_CRYPT_ERROR;

    }
    return IPRO_CRYPT_OK;
}
#endif

int32_t ipro_crypt_init(ipro_crypt_handle_t *crypt_handle,uint8_t type)
{
    int32_t result = ipro_crypt_init_do(crypt_handle,type);
    
    if(result==IPRO_CRYPT_OK){
        crypt_handle->crypt_cfg.type=type;
    }

    return result;
}

int32_t ipro_crypt_setkey(ipro_crypt_handle_t *crypt_handle,const uint8_t *key,uint8_t keytype,uint8_t key_len,
                        const uint8_t *nonce,uint8_t nonce_len,uint8_t dir)
{
    int32_t result = IPRO_CRYPT_OK;

    result=ipro_crypt_setkey_do(crypt_handle,key,keytype,key_len,nonce,nonce_len,dir);

    if(result!=IPRO_CRYPT_OK){
    	ipro_crypt_printe("crypt set key fail\r\n");
    	result=IPRO_CRYPT_ERROR;
    }

    return result;
}

int32_t ipro_crypt_setadd(ipro_crypt_handle_t *crypt_handle,const uint8_t *add,uint8_t len,
                            uint8_t dir)
{
    int32_t result;

    result = ipro_crypt_setadd_do(crypt_handle,add,len,dir);

    if(result!=IPRO_CRYPT_OK){
    	ipro_crypt_printe("crypt set add fail\r\n");
    	result=IPRO_CRYPT_ERROR;
    }

    return result;
}

int32_t ipro_crypt_encrypt(ipro_crypt_handle_t *crypt_handle,const uint8_t *in,uint32_t len,
                        size_t offset,uint8_t *out)
{
	int32_t result;

    result=ipro_crypt_encrypt_do(crypt_handle,in,len, offset,out);

    if( result != IPRO_CRYPT_OK)
    {
        ipro_crypt_printe("crypt encrypt fail\r\n");
        return IPRO_CRYPT_ERROR;
    }

    return IPRO_CRYPT_OK;
}

int32_t ipro_crypt_encrypt_tag(ipro_crypt_handle_t *crypt_handle,const uint8_t *in,uint32_t in_len,
                        const uint8_t *add,uint32_t add_len,size_t offset,uint8_t *out,
                        uint8_t *tag,uint8_t tag_len)
{
	int32_t result;
    
    result= ipro_crypt_encrypt_tag_do(crypt_handle,in,in_len,add,add_len,offset,out,tag,tag_len);

    if( result != IPRO_CRYPT_OK)
    {
    	ipro_crypt_printe("crypt encrypt and tag fail\r\n");
        return IPRO_CRYPT_ERROR;
    }

    return IPRO_CRYPT_OK;
}

int32_t ipro_crypt_update(ipro_crypt_handle_t *crypt_handle,const uint8_t *in,uint32_t len,
                        uint8_t *out)
{
	int32_t result;

    result=ipro_crypt_update_do(crypt_handle,in,len,out);

    if( result != IPRO_CRYPT_OK)
    {
        ipro_crypt_printe("crypt update fail\r\n");
        return IPRO_CRYPT_ERROR;
    }

    return IPRO_CRYPT_OK;
}


int32_t ipro_crypt_decrypt(ipro_crypt_handle_t *crypt_handle,const uint8_t *in,uint32_t len,
                        size_t offset,uint8_t *out)
{
	int32_t result;
    
    result=ipro_crypt_decrypt_do(crypt_handle,in,len,offset,out);

    if( result != IPRO_CRYPT_OK)
    {
        ipro_crypt_printe("crypt decrypt fail\r\n");
        return IPRO_CRYPT_ERROR;
    }

    return IPRO_CRYPT_OK;
}

int32_t ipro_crypt_auth_decrypt(ipro_crypt_handle_t *crypt_handle,const uint8_t *in,uint32_t in_len,
                        const uint8_t *add,uint32_t add_len,size_t offset,uint8_t *out,
                        const uint8_t *tag,uint8_t tag_len)
{
	int32_t result;
    
    result= ipro_crypt_auth_decrypt_do(crypt_handle,in,in_len,add,add_len,offset,out,tag,tag_len);
    
    if( result != IPRO_CRYPT_OK)
    {
        ipro_crypt_printe("crypt auth and decrypt fail\r\n");
        return IPRO_CRYPT_ERROR;
    }

    return IPRO_CRYPT_OK;
}

int32_t ipro_crypt_finish(ipro_crypt_handle_t *crypt_handle,uint8_t *tag,uint32_t len)
{
	int32_t result;
    
    result=ipro_crypt_finish_do(crypt_handle,tag,len);
    
    if( result != IPRO_CRYPT_OK)
    {
        ipro_crypt_printe("crypt finish fail\r\n");
        return IPRO_CRYPT_ERROR;
    }

    return IPRO_CRYPT_OK;
}


int32_t ipro_crypt_deinit(ipro_crypt_handle_t *crypt_handle)
{

	ipro_crypt_deinit_do(crypt_handle);
    memset(crypt_handle,0,sizeof(ipro_crypt_handle_t));

    return IPRO_CRYPT_OK;
}
