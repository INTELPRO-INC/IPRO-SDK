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

#ifndef __IPROAI_NMSISNN_BRIDGE_H__
#define __IPROAI_NMSISNN_BRIDGE_H__

/*
 * Bridge from the .inm runtime's uint8 tensors to the real NMSIS-NN int8
 * kernels (components/3rdparty/nmsis_nn).
 *
 * Two things make this a separate translation unit rather than a few lines in
 * iproai_inst_cpu_ops.c:
 *
 *  1. Header collision. components/ai ships its own riscv_nn_types.h whose
 *     nmsis_nn_conv_params carries an extra filter_offset field that upstream
 *     does not have. Including both in one file gives either a redefinition
 *     error or - worse, if the search order shifts - silently misaligned
 *     struct fields. This header therefore exposes plain C types only, and
 *     the .c file includes upstream's headers and nothing from components/ai.
 *
 *  2. Domain. The runtime keeps activations and weights as uint8 biased by
 *     128 (the value v is stored as v ^ 0x80 - the same convention the NPU
 *     uses at both ends of its external input). NMSIS-NN is int8. The
 *     conversion is exactly XOR 0x80 per byte, done into scratch buffers.
 */

#include <stdint.h>

/* Returns 0 on success, negative on failure (allocation or kernel error).
 * All tensors are NHWC. input/filter/output are in the uint8 (XOR 0x80)
 * domain; bias, multiplier and shift are the ordinary TFLite int32 values.
 *
 * input_offset / output_offset follow the NMSIS-NN convention: the NEGATIVE
 * of the tensor's zero point. */
int iproai_nmsisnn_conv_u8(const uint8_t *input, int in_h, int in_w, int in_c,
                           const uint8_t *filter, int k_h, int k_w,
                           const int32_t *bias,
                           uint8_t *output, int out_h, int out_w, int out_c,
                           int stride_x, int stride_y,
                           int pad_x, int pad_y,
                           int dilation_x, int dilation_y,
                           int input_offset, int output_offset,
                           const int32_t *multiplier, const int32_t *shift,
                           int act_min, int act_max);

#endif /* __IPROAI_NMSISNN_BRIDGE_H__ */
