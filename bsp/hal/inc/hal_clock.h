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

#ifndef __HAL_CLOCK__H__
#define __HAL_CLOCK__H__

#ifdef __cplusplus
extern "C"{
#endif

#include "hal_common.h"
#include "board_config.h"

/*XTAL_TYPE*/
#define XTAL_NONE         0
#define EXTERNAL_XTAL_32M 1
#define INTERNAL_RC_32M   2

/*CLOCK_32K_XTAL*/
#define EXTERNAL_XTAL_32K 0
#define INTERNAL_RC_32K   1

/*BSP_ROOT_CLOCK_SOURCE*/
#if XTAL_TYPE != EXTERNAL_XTAL_32M
#define ROOT_CLOCK_SOURCE_XCLK 0
#else
#define ROOT_CLOCK_SOURCE_XCLK 1
#endif
#define ROOT_CLOCK_SOURCE_PLL_57P6M 2
#define ROOT_CLOCK_SOURCE_PLL_96M   3
#define ROOT_CLOCK_SOURCE_PLL_144M  4
/*BSP_XXX_CLOCK_SOURCE*/
#define ROOT_CLOCK_SOURCE_32K_CLK 5
#define ROOT_CLOCK_SOURCE_FCLK    6
#define ROOT_CLOCK_SOURCE_BCLK    7
#define ROOT_CLOCK_SOURCE_1K_CLK  8

/*BSP_AUDIO_PLL_CLOCK_SOURCE*/
#define ROOT_CLOCK_SOURCE_AUPLL_12288000_HZ 9
#define ROOT_CLOCK_SOURCE_AUPLL_11289600_HZ 10
#define ROOT_CLOCK_SOURCE_AUPLL_5644800_HZ  11
#define ROOT_CLOCK_SOURCE_AUPLL_24576000_HZ 12
#define ROOT_CLOCK_SOURCE_AUPLL_24000000_HZ 13

void system_clock_init(void);
void peripheral_clock_init(void);
void system_mtimer_clock_init(void);
#ifdef __cplusplus
}
#endif
#endif
