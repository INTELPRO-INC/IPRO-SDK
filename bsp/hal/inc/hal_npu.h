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
 * NPU Hardware Abstraction Layer for IPRO7
 *
 * This header provides NPU hardware control functions for IPRO7
 */

#ifndef __HAL_NPU_H__
#define __HAL_NPU_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void NPU_Clk_Div(int n);
void NPU_Bus_Setting(void);

/**
 * NPU Clock Control
 */
void hal_npu_clk_enable(uint8_t enable);

/**
 * NPU Interrupt Handling
 */
void hal_npu_ack_irq(void);

/**
 * NPU Layer Configuration
 */
void hal_npu_layer_setup(uint8_t *inst_buf, uint8_t *weights_buf, uint8_t *bias_buf, uint8_t *mult_shift_buf);

/**
 * NPU Input Buffer Configuration
 */
void hal_npu_set_input_buffer(uint8_t *input_buf, int patch_size);

/**
 * NPU Network Parameters
 */
void hal_npu_set_net_param(uint8_t unsign_input, uint8_t relu_n, uint8_t use_tflite, uint32_t mode);

/**
 * NPU Reset Unsigned Flag
 */
void hal_npu_reset_unsign(void);

/**
 * NPU Execution Control
 */
void hal_npu_start(void);
void hal_npu_stop(void);

/**
 * NPU Image External Address Configuration
 */
void NPU_Img_Ext_Addr_Cfg(uint32_t addr);

/**
 * NPU Image External Address Get
 * @return Current external image address configured in APU_DM2_ADDR register
 */
uint32_t NPU_Img_Ext_Addr_Get(void);

/**
 * NPU Bus Delay Configuration
 */
void NPU_Bus_Delay(uint8_t enable, uint8_t rcmd, uint8_t wcmd);

void NPU_SRAM_EN();


/* Global image buffer - user should define this in their application */
extern char *imgBuffer;

#ifdef __cplusplus
}
#endif

#endif /* __HAL_NPU_H__ */
