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
 * @brief Two models, one NPU: serialise inferences and re-apply per-model
 *        global state before each one.
 *
 * components/ai locks the NPU per layer, so two inference tasks would
 * interleave layers of different models. That is fine for everything the
 * runtime re-programs per layer (instruction / weight / bias / activation
 * addresses) but not for the two things it writes once at model load:
 *
 *   - the external-input address (NPU_Img_Ext_Addr_Cfg), which the first
 *     layer of an ext_input model reads;
 *   - the net parameters (hal_npu_set_net_param: unsigned input, relu_n,
 *     tflite parameter loading, input mode).
 *
 * So an inference here is: take the lock, re-apply both from the model's
 * own net info, run, release. Gesture (7 ms) waits behind a person
 * detection (~100 ms) at worst.
 */
#ifndef NPU_SHARE_H
#define NPU_SHARE_H

#include <stdint.h>

struct iproai_net_info_t;

int  npu_share_init(void);

/** @brief Lock the NPU for one inference of @p net reading @p ext_addr
 *         (0 = the model has its own input buffer). Blocks. */
void npu_share_begin(const struct iproai_net_info_t *net, uint32_t ext_addr);
void npu_share_end(void);

#endif /* NPU_SHARE_H */
