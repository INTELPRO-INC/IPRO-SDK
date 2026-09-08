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

#ifndef _IPRO_RSA_H
#define _IPRO_RSA_H

#include "ipro_rsa_port.h"
#include "platform_common.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define ipro_rsa_printf     printf
#define ipro_rsa_printe     printf
#define ipro_rsa_printd     printf
#define ipro_rsa_printw     printf


#define IPRO_RSA_OK             0
#define IPRO_RSA_ERROR          -1

enum IPRO_RSA_PARA
{
    IPRO_RSA_PARA_N,
    IPRO_RSA_PARA_E,
    IPRO_RSA_PARA_D,
    IPRO_RSA_PARA_P,
    IPRO_RSA_PARA_Q,
    IPRO_RSA_PARA_DP,
    IPRO_RSA_PARA_DQ,
    IPRO_RSA_PARA_QP,
};
   
typedef struct tag_ipro_rsa_handle_t
{
    ipro_rsa_ctx_t rsa_ctx;
}ipro_rsa_handle_t;

int32_t ipro_rsa_init(ipro_rsa_handle_t *rsa_handle,int padding,int hashtype );
int32_t ipro_rsa_set_parameter(ipro_rsa_handle_t *rsa_handle,int type,uint8_t *value,
                                uint32_t len );
int32_t ipro_rsa_check_private(ipro_rsa_handle_t *rsa_handle);
int32_t ipro_rsa_sign( ipro_rsa_handle_t *rsa_handle,const uint8_t *hash,
                            uint8_t hashlen,uint8_t hashtype,uint8_t *sig,uint32_t *slen);
int32_t ipro_rsa_verify( ipro_rsa_handle_t *rsa_handle,const uint8_t *hash,
                            uint8_t hashlen,uint8_t hashtype,const uint8_t *sig,uint32_t slen);
                            
int32_t ipro_rsa_deinit( ipro_rsa_handle_t *rsa_handle);

#endif
