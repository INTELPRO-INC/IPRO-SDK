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
 * @file npu_share.h
 * @brief Two models, one NPU: serialise inferences and re-apply each model's
 *        NPU state before it runs.
 *
 * components/ai writes the NPU's base registers when it loads a model
 * (iproai_npu_initCfg) and not again: the instruction, weight, bias and
 * multiplier/shift addresses (hal_npu_layer_setup), the work buffer
 * (hal_npu_set_input_buffer), the net parameters (hal_npu_set_net_param) and
 * the external input (NPU_Img_Ext_Addr_Cfg). With two models loaded, the one
 * loaded last would otherwise lend its weights to the other. (This header
 * used to say the addresses were re-programmed per layer, and restored only
 * the net parameters and the external input; the IPRO Edge AI runtime's A/B
 * runs on the board showed otherwise, docs/edge_ai/01 §2.4.)
 *
 * An inference here is: take the NPU lock - the runtime's
 * (ipro_ai_npu_lock), so a runtime user in the same image waits too - write
 * the model's state as its load did, run, release. Gesture (7 ms) waits
 * behind a person detection (~100 ms) at worst.
 */
#ifndef NPU_SHARE_H
#define NPU_SHARE_H

#include <stdint.h>

#include <iproai_core.h>

int  npu_share_init(void);

/** @brief Lock the NPU for one inference of model @p hdl reading @p ext_addr
 *         (0 = the model has its own input buffer). Blocks. */
void npu_share_begin(iproai_model_hdl_t hdl, uint32_t ext_addr);
void npu_share_end(void);

/** @brief Hold the NPU while a model loads: iproai_load_model_*() writes the NPU
 *         registers and reads imgBuffer, so a load must not overlap the other
 *         model's inference. Set imgBuffer after npu_share_load_begin(). */
void npu_share_load_begin(void);
void npu_share_load_end(void);

#endif /* NPU_SHARE_H */
