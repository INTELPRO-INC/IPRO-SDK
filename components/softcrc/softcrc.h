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

#ifndef __SOFTCRC_H__
#define __SOFTCRC_H__

#include "stdint.h"

uint16_t Utils_Soft_CRC16(void *dataIn, uint32_t len);
uint32_t Utils_Soft_CRC32_Ex(uint32_t initial, void *dataIn, uint32_t len);
uint32_t Utils_Soft_CRC32(void *dataIn, uint32_t len);

/* Legacy API compatibility */
#define IPRO_Soft_CRC32(data, len)  Utils_Soft_CRC32(data, len)

#endif
