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
 * @file ipro_llm_npu.h
 * @brief The IPRO7 NPU as the ipro_llm accelerator: the attention projections of prompt chunks
 *        and decode steps as int8 1x1 convolutions (CONFIG_IPRO_LLM_NPU, needs CONFIG_AI_SUPPORT).
 *
 *   ipro_llm_accel_t npu;
 *   ipro_llm_npu_init(&npu);
 *   void *scratch = pvPortMalloc(ipro_llm_accel_scratch_bytes(m));
 *   ipro_llm_accel_attach(m, &npu, scratch, ipro_llm_accel_scratch_bytes(m));   // 24 .inm loads
 *   vPortFree(scratch);
 *
 * Each projection becomes two models: a prompt chunk's (32 tokens) and a decode step's (1 token).
 * The instruction words come from llm_npu_templates.h (tools/ipro_llm/export/npu_templates.py,
 * one per shape and token count), bias / weights / multipliers are packed here (layout in
 * tools/ipro_llm/export/npu_inm.py). The runtime copies the sections to the heap (weights ~0.9 MB
 * for XS; the one-token model uses the chunk model's); all models share one activation buffer.
 * A prompt chunk shorter than 32 tokens is padded: the NPU's cost is the weight stream, not the
 * tokens (apps/test/ipro_llm_npu_bench). Call from one task; the model task does.
 */
#ifndef IPRO_LLM_NPU_H
#define IPRO_LLM_NPU_H

#include <stdint.h>

#include "ipro_llm.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int      models;            /* projections loaded */
    uint32_t heap_bytes;        /* model sections the runtime copied + activation + feed */
    uint32_t load_us;           /* building and loading every model */
} ipro_llm_npu_stats_t;

/** Fill `a` with the NPU accelerator; allocates the shared activation buffer and the input feed.
 *  0 = ready. */
int ipro_llm_npu_init(ipro_llm_accel_t *a);

/** Free every loaded projection model (after a failed attach, or to give the heap back); the
 *  activation buffer and feed stay for the next attach. Detach the engine first. */
void ipro_llm_npu_release(void);

const ipro_llm_npu_stats_t *ipro_llm_npu_stats(void);

/**
 * Run projection `slot` on a pseudo-random input of n tokens (1: on the one-token model) and
 * compare the NPU's outputs with the integer reference computed from the weights, bias and
 * multipliers the runtime holds (TFLite arithmetic, ipro_llm_requant). 0 = ran; *mismatches
 * counts outputs that differ, *max_diff the largest difference in int8 steps.
 */
int ipro_llm_npu_check(int slot, int n, int *mismatches, int *max_diff);

#ifdef __cplusplus
}
#endif

#endif /* IPRO_LLM_NPU_H */
