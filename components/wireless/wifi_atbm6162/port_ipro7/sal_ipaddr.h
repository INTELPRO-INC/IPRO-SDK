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

/*
 * IPRO7 port shim for the vendor's "sal_ipaddr.h" (an external socket-abstraction
 * layer header not shipped with the driver). atbm_os_skbuf.c only needs the lwIP
 * IP-address types/macros, so route to lwIP's ip_addr.h.
 */
#ifndef ATBM_SAL_IPADDR_SHIM_H
#define ATBM_SAL_IPADDR_SHIM_H

#include "lwip/ip_addr.h"
#include "lwip/ip4_addr.h"

#endif /* ATBM_SAL_IPADDR_SHIM_H */
