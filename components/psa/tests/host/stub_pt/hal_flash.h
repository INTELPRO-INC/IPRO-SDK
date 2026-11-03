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
 * Host stand-in for bsp/hal/inc/hal_flash.h: the flash calls of
 * hal_ptmgr.c and svc_fwu.c, defined by the host test.
 */
#pragma once

#include <stdint.h>
#include <string.h>

#define SUCCESS 0
#define ERROR   1

int flash_read(uint32_t addr, uint8_t *data, uint32_t len);
int flash_erase(uint32_t startaddr, uint32_t len);
int flash_write(uint32_t addr, uint8_t *data, uint32_t len);

#define arch_memcpy_fast memcpy
