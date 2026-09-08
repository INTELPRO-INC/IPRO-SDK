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

#ifndef _IPRO_ECDSA_PORT_H
#define _IPRO_ECDSA_PORT_H

#ifndef IPRO_PKA_HARDWARE
#include "mbedtls/ecdsa.h"
#include "mbedtls/md.h"
#include "mbedtls/asn1.h"
#endif
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

typedef union tag_ipro_ecdsa_ctx_t
{
#ifndef IPRO_PKA_HARDWARE
	mbedtls_ecdsa_context ctx_ecdsa;
#endif
}ipro_ecdsa_ctx_t;


#endif
