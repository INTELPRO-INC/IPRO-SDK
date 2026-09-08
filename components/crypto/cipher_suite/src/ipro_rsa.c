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

#include "ipro_rsa.h"
#include "ipro_hash.h"

int32_t ipro_rsa_init(ipro_rsa_handle_t *rsa_handle,int padding,int hashtype )
{
    int32_t ret = IPRO_RSA_OK;
    memset(rsa_handle,0,sizeof(ipro_rsa_handle_t));
    mbedtls_rsa_init((mbedtls_rsa_context*)&rsa_handle->rsa_ctx, padding, ipro_hash_get_type(hashtype));
    return ret;
}

int32_t ipro_rsa_set_parameter(ipro_rsa_handle_t *rsa_handle,int type,uint8_t *value,
                                uint32_t len )
{
    int32_t ret = IPRO_RSA_OK;
    mbedtls_rsa_context* rsa=(mbedtls_rsa_context*)&rsa_handle->rsa_ctx;
    
	switch(type)
	{
        case IPRO_RSA_PARA_N:
            mbedtls_mpi_read_binary(&rsa->N,value,len);
            rsa->len = ( mbedtls_mpi_bitlen( &rsa->N ) + 7 ) >> 3;
            break;
        case IPRO_RSA_PARA_E:
            mbedtls_mpi_read_binary(&rsa->E,value,len);
            break;
        case IPRO_RSA_PARA_D:
            mbedtls_mpi_read_binary(&rsa->D,value,len);
            break;
        case IPRO_RSA_PARA_P:
            mbedtls_mpi_read_binary(&rsa->P,value,len);
            break;
        case IPRO_RSA_PARA_Q:
            mbedtls_mpi_read_binary(&rsa->Q,value,len);
            break;
        case IPRO_RSA_PARA_DP:
            mbedtls_mpi_read_binary(&rsa->DP,value,len);
            break;
        case IPRO_RSA_PARA_DQ:
            mbedtls_mpi_read_binary(&rsa->DQ,value,len);
            break;
        case IPRO_RSA_PARA_QP:
            mbedtls_mpi_read_binary(&rsa->QP,value,len);
            break;
        default:
            ret=IPRO_RSA_ERROR;
            break;
	}
	
    return ret;
}

int32_t ipro_rsa_check_private(ipro_rsa_handle_t *rsa_handle)
{
    int32_t ret = IPRO_RSA_OK;
    mbedtls_rsa_context* rsa=(mbedtls_rsa_context*)&rsa_handle->rsa_ctx;
    
	if( ( ret = mbedtls_rsa_check_privkey( rsa ) ) != 0 )
    {
        ipro_rsa_printe("failed\r\n!rsa_check_privkey failed with -0x%0lx\n", -ret );
        return IPRO_RSA_ERROR;
    }

    return IPRO_RSA_OK;
}

int32_t ipro_rsa_check_public(ipro_rsa_handle_t *rsa_handle)
{
    int32_t ret = IPRO_RSA_OK;
    mbedtls_rsa_context* rsa=(mbedtls_rsa_context*)&rsa_handle->rsa_ctx;

	if( ( ret = mbedtls_rsa_check_pubkey( rsa ) ) != 0 )
    {
        ipro_rsa_printe("failed\r\n!rsa_check_privkey failed with -0x%0lx\n", -ret );
        return IPRO_RSA_ERROR;
    }
	
    return IPRO_RSA_OK;
}

int32_t ipro_rsa_sign( ipro_rsa_handle_t *rsa_handle,const uint8_t *hash,
                            uint8_t hashlen,uint8_t hashtype,uint8_t *sig,uint32_t *slen)
{
    int32_t ret = IPRO_RSA_OK;
    mbedtls_rsa_context* rsa=(mbedtls_rsa_context*)&rsa_handle->rsa_ctx;
    
	ret = mbedtls_rsa_pkcs1_sign( rsa, NULL, NULL, MBEDTLS_RSA_PRIVATE,
    						(mbedtls_md_type_t)ipro_hash_get_type(hashtype),
                            hashlen, hash, sig );
    if( ret != 0 )
    {
        ipro_rsa_printe("failed\r\n!rsa_pkcs1_sign failed with -0x%0lx\n", -ret );
        return IPRO_RSA_ERROR;
    }	
    *slen=rsa->len;
    return IPRO_RSA_OK;                        
}

int32_t ipro_rsa_verify( ipro_rsa_handle_t *rsa_handle,const uint8_t *hash,
                            uint8_t hashlen,uint8_t hashtype,const uint8_t *sig,uint32_t slen)
{
    int32_t ret = IPRO_RSA_OK;
    mbedtls_rsa_context* rsa=(mbedtls_rsa_context*)&rsa_handle->rsa_ctx;
    
    ret = mbedtls_rsa_pkcs1_verify( rsa, NULL, NULL, MBEDTLS_RSA_PUBLIC, 
    						(mbedtls_md_type_t)ipro_hash_get_type(hashtype),
                            hashlen, hash, sig );
    if( ret != 0 )
    {
        ipro_rsa_printe("failed\r\n!rsa_pkcs1_verify failed with -0x%0lx\n", -ret );
        return IPRO_RSA_ERROR;
    }	
    return IPRO_RSA_OK;                        
}

int32_t ipro_rsa_deinit( ipro_rsa_handle_t *rsa_handle)
{
    mbedtls_rsa_context* rsa=(mbedtls_rsa_context*)&rsa_handle->rsa_ctx;
    
    mbedtls_rsa_free( rsa );
    memset(rsa_handle,0,sizeof(ipro_rsa_handle_t));
    
    return IPRO_RSA_OK;                        
}
