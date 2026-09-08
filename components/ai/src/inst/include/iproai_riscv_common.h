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

#pragma once

#define IPROAI_BITTURE 0
typedef enum {
    LEAKY, RELU, LINEAR, RELU6, MISH, ELU, RELIE, LOGISTIC, RAMP, TANH, PLSE, LOGGY, STAIR, HARDTAN, LHTAN, SELU, GELU, SWISH, NORM_CHAN, NORM_CHAN_SOFTMAX, NORM_CHAN_SOFTMAX_MAXVAL, RELUN
}ACTIVATION;

int32_t PE_LEAKY(int32_t pass_data);
int32_t PE_RELU(int32_t pass_data);
int32_t PE_RELU6(int32_t pass_data, int fshift);
int8_t PE_MISH(int32_t pass_data, int fin, int fout);
int32_t PE_RELUN(int32_t pass_data, int fshift, int n);
q7_t IPROAI_ROUND(int conv_out, uint16_t out_shift, int round, int dbg);