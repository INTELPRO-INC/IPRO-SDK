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
 * @file softmax_q.h
 * @brief Integer softmax over quantised logits. No float, no libm.
 *
 * The classifier's output is int8 with a scale s and zero point z:
 * logit_i = s * (q_i - z). Softmax only needs the differences, so z drops
 * out; s is folded into a Q16 constant once (the one place a float is
 * touched, at load time on the app task before any other task uses the
 * FPU - see softmax_q_scale()).
 */
#ifndef SOFTMAX_Q_H
#define SOFTMAX_Q_H

#include <stdint.h>

/** @brief Scale as Q16 from the model's float output scale (load time only). */
uint32_t softmax_q_scale(float out_scale);

/**
 * @brief Probabilities in percent (sum ~100) and the argmax.
 * @param q         Raw output codes (uint8 as the runtime presents them, or
 *                  int8 as the header quantises them - only differences count).
 * @param n         Number of classes (<= 16).
 * @param scale_q16 From softmax_q_scale().
 * @param pct_out   n entries, 0..100.
 * @return Index of the largest logit.
 */
int softmax_q_pct(const uint8_t *q, int n, uint32_t scale_q16, uint8_t *pct_out);

#endif /* SOFTMAX_Q_H */
