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

/**
 * @file ipro7_fpga_board_config.h
 * @brief Board configuration stub for IPRO7 FPGA
 *
 * FPGA does not require pinmux or clock configuration.
 * This file provides empty macros for build compatibility.
 */

#ifndef __IPRO7_FPGA_BOARD_CONFIG_H__
#define __IPRO7_FPGA_BOARD_CONFIG_H__

#include "drv_clock.h"
#include "drv_glb.h"
#include "drv_hbn.h"

/*==============================================================================
 * Pin Configuration (Not used on FPGA)
 *============================================================================*/

#define BOARD_PINMUX_TABLE
#define BOARD_PINMUX_COUNT 0
#define BOARD_PINMUX_INIT()         do { } while(0)

/*==============================================================================
 * UART Routing Configuration (Not used on FPGA)
 *============================================================================*/

#define BOARD_UART_ROUTING_INIT()   do { } while(0)

/*==============================================================================
 * Clock Configuration (Not used on FPGA)
 *============================================================================*/

#define BOARD_PERIPHERAL_CLOCK_INIT() do { } while(0)
#define BOARD_CLOCK_GATING_INIT()     do { } while(0)
#define BOARD_SYSTEM_CLOCK_INIT()     do { } while(0)
#define BOARD_32K_CLOCK_INIT()        do { } while(0)

/* FPGA has no external XTAL — XCLK runs from RC32M.  MTimer is configured
 * from whatever XCLK ends up at so the formula below is always valid. */
#define BOARD_MTIMER_CLK_INIT() \
    GLB_Set_MTimer_CLK(1, GLB_MTIMER_CLK_XCLK, \
        Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_XCLK) / 1000 / 1000 / 2 - 1)
#define BOARD_XCLK_INIT()             HBN_Set_XCLK_CLK_Sel(HBN_XCLK_CLK_RC32M)

/*==============================================================================
 * Board Initialization
 *============================================================================*/

#define BOARD_INIT()                do { } while(0)

#endif /* __IPRO7_FPGA_BOARD_CONFIG_H__ */
