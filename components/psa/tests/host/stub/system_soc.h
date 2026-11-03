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
 * Host stand-in of system_soc.h for spm_periph.c: the UART0, DMA and
 * USB bases and ECLIC ids of bsp/drivers/ipro7_soc/regs/ipro7.h
 * without RFDTBT2.
 */
#pragma once

#include <stdint.h>

#define UART0_BASE ((uint32_t)0x3000B000)
#define DMA_BASE   ((uint32_t)0x30082000)
#define USB_BASE   ((uint32_t)0x30084000)

typedef enum {
    DMA0_ALL_IRQn = 19 + 15,
    USB_IRQn = 19 + 28,
    UART0_IRQn = 19 + 29,
} IRQn_Type;
