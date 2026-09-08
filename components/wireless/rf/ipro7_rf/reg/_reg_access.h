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

#ifndef _____REG_ACCESS_H
#define _____REG_ACCESS_H
// #include "co_utils.h"
#include <limits.h>
#include <stdint.h>

#define CHAR_LEN    (CHAR_BIT/8)
#define HW2CPU(ptr) ((void *)(((uint32_t)(ptr)) / CHAR_LEN))

/// Macro to read a platform register
#define REG_PL_RD(addr)              (*(volatile uint32_t *)(HW2CPU(addr)))

/// Macro to write a platform register
#define REG_PL_WR(addr, value)       (*(volatile uint32_t *)(HW2CPU(addr))) = (value)

 
void wait_us(uint32_t);
void wait_ms(uint32_t);
void reg_write_bits(uint32_t, uint8_t, uint8_t, uint32_t);
void reg_write_1bit(uint32_t, uint8_t, uint32_t);
void reg_write_dwrd(uint32_t, uint32_t);
uint32_t reg_read_bits(uint32_t, uint8_t, uint8_t);
uint32_t reg_read_1bit(uint32_t, uint8_t);
uint32_t reg_read_dwrd(uint32_t);

#endif
