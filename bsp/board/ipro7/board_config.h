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

#ifndef __BOARD_CONFIG_H__
#define __BOARD_CONFIG_H__

#include "peripheral_config.h"

#if defined(CONFIG_DUAL_IMU_BOARD)
#include "ipro7_dual_imu_board_config.h"
#elif defined(CONFIG_EVB) || defined(CONFIG_DVK)
#include "ipro7_evb_board_config.h"
#elif defined(CONFIG_CLMAX_7AI_EVB)
#include "ipro7_clmax_7ai_evb_board_config.h"
#elif defined(CONFIG_IPRO7AI_EVK)
#include "ipro7ai_evk_board_config.h"
#elif defined(CONFIG_FPGA)
#include "ipro7_fpga_board_config.h"
#elif defined(CONFIG_SLT)
#include "ipro7_slt_board_config.h"
#elif defined(CONFIG_DEMO_QFN32)
#include "ipro7_demo_qfn32_board_config.h"
#elif defined(CONFIG_DEMO_QFN56)
#include "ipro7_demo_qfn56_board_config.h"
#elif defined(CONFIG_IPRO7_ARDUINO)
#include "ipro7_arduino_board_config.h"
#else
#error "Please define a board (CONFIG_EVB, CONFIG_DVK, CONFIG_FPGA, CONFIG_SLT, CONFIG_DUAL_IMU_BOARD, CONFIG_DEMO_QFN32, CONFIG_DEMO_QFN56, CONFIG_IPRO7_ARDUINO, or CONFIG_IPRO7AI_EVK) in Kconfig"
#endif

#endif /* __BOARD_CONFIG_H__ */
