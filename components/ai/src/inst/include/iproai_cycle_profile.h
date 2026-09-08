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
 * @file iproai_cycle_profile.h
 * @brief Cycle-accurate NPU-busy vs CPU-busy split for one .inm inference.
 *
 * The existing IPROAI_PROFILING counters use xTaskGetTickCount() at
 * configTICK_RATE_HZ = 1000, i.e. 1 ms granularity - a layer that takes tens of
 * microseconds registers as zero. That is fine for a frame-rate readout and
 * useless for the question this answers:
 *
 *   how much of an inference is the CPU parked on the NPU semaphore?
 *
 * That fraction is the concurrency budget. iproai_npu_inference() blocks on a
 * FreeRTOS semaphore rather than spinning, so every cycle counted as NPU-busy
 * is a cycle another task (e.g. a TFLM model) can actually use.
 *
 * Enable with CONFIG_AI_CYCLE_PROFILE. Compiled out entirely when off.
 */

#ifndef __IPROAI_CYCLE_PROFILE_H__
#define __IPROAI_CYCLE_PROFILE_H__

#include <stdint.h>

#include <generated/autoconf.h>

#ifdef __cplusplus
extern "C" {
#endif

/** How many spans one inference may record before the rest are dropped.
 *  A fixed array on purpose: the layer loop must not touch the heap. */
#define IPROAI_CYCLE_MAX_SPANS 256

/**
 * One timed stretch of an inference.
 *
 * 🔴 `first` and `last` differ for NPU work, and that is the hardware rather
 * than a convenience. A dispatch covers a whole **contiguous run of NPU
 * layers**: iproai_inst_cpu.c walks `while (++i)` to the end of the run and
 * returns IPROAI_PROCESS_NPU once, so no per-layer boundary exists to measure.
 * Dividing a segment evenly among its layers would be inventing numbers, so
 * the segment is reported as a segment.
 *
 * Per-layer NPU timing would need IPROAI_NPU_HALT_EACH_LAYER, which costs a
 * semaphore round trip per layer -- it perturbs the thing being measured.
 *
 * CPU and DSP layers are genuinely per-layer, so first == last.
 */
typedef struct {
    uint16_t first;
    uint16_t last;
    uint32_t cycles;        /**< saturates at UINT32_MAX rather than wrapping */
    uint8_t  on_npu;
} iproai_cycle_span_t;

#ifdef CONFIG_AI_CYCLE_PROFILE


/** Spans recorded for the most recent inference. */
uint16_t iproai_cycle_spans(void);
/** Span @p i of the most recent inference, or NULL when out of range. */
const iproai_cycle_span_t *iproai_cycle_span(uint16_t i);
/** Non-zero when the last inference produced more spans than the array holds. */
int iproai_cycle_spans_truncated(void);
/** Print the per-layer breakdown of the most recent inference. */
void iproai_cycle_layer_report(void);

/** Accumulated cycles spent blocked on the NPU, since the last reset. */
uint64_t iproai_cycle_npu(void);
/** Accumulated cycles for whole inferences, since the last reset. */
uint64_t iproai_cycle_total(void);
/** Number of completed inferences since the last reset. */
uint32_t iproai_cycle_runs(void);
/** Number of NPU layer dispatches since the last reset. */
uint32_t iproai_cycle_npu_dispatches(void);

void iproai_cycle_reset(void);

/** Print the split, including the share of CPU time a co-running task could use. */
void iproai_cycle_report(void);

/* Internal - called from the AI runtime. */
void iproai_cycle_npu_enter(void);
void iproai_cycle_npu_exit(void);
void iproai_cycle_run_enter(void);
void iproai_cycle_run_exit(void);
/** Start timing one CPU/DSP layer. */
void iproai_cycle_layer_enter(void);
/** Close the layer opened by iproai_cycle_layer_enter(). */
void iproai_cycle_layer_exit(uint16_t index);
/** Declare which layers the next NPU dispatch will cover.
 *  Only iproai_inst_cpu.c knows the range; the dispatch itself, over in
 *  iproai_npu_inference(), does not. */
void iproai_cycle_npu_span(uint16_t first, uint16_t last);

#else  /* profiling off - every hook folds away */

#define iproai_cycle_npu()              ((uint64_t)0)
#define iproai_cycle_total()            ((uint64_t)0)
#define iproai_cycle_runs()             ((uint32_t)0)
#define iproai_cycle_npu_dispatches()   ((uint32_t)0)
#define iproai_cycle_spans()            ((uint16_t)0)
#define iproai_cycle_span(i)            ((const iproai_cycle_span_t *)0)
#define iproai_cycle_spans_truncated()  (0)
#define iproai_cycle_reset()            do { } while (0)
#define iproai_cycle_report()           do { } while (0)
#define iproai_cycle_layer_report()     do { } while (0)
#define iproai_cycle_npu_enter()        do { } while (0)
#define iproai_cycle_npu_exit()         do { } while (0)
#define iproai_cycle_run_enter()        do { } while (0)
#define iproai_cycle_run_exit()         do { } while (0)
#define iproai_cycle_layer_enter()      do { } while (0)
#define iproai_cycle_layer_exit(i)      do { (void)(i); } while (0)
#define iproai_cycle_npu_span(a, b)     do { (void)(a); (void)(b); } while (0)

#endif /* CONFIG_AI_CYCLE_PROFILE */

#ifdef __cplusplus
}
#endif

#endif /* __IPROAI_CYCLE_PROFILE_H__ */
