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
 * @file teach_model.h
 * @brief A user-trained N-class image classifier on the IPRO7 NPU (Reef,
 *        CONFIG_UVC_TEACH), as IPRO AI Studio's Teach page produces it.
 *
 * The model is a frozen MobileNet backbone with a 1x1-conv head: 128x128x3 in,
 * a 4x4xN map of logits out. Averaging the 16 positions equals global average
 * pooling + a dense layer, but keeps every layer on the NPU. Studio writes the
 * .inm into the "mfg" partition at +0x1000, followed by a label block:
 *
 *   "TCH1" | u8 version (1) | u8 class count | u16 reserved | 8 x char[16] labels
 *
 * Without the block the classes are called CLASS 0..N-1. The camera hands over
 * one ISP channel-1 frame at a time (the sensor ROI scaled to 128x128 ARGB).
 */
#ifndef TEACH_MODEL_H
#define TEACH_MODEL_H

#include <stdbool.h>
#include <stdint.h>

/* Where the Teach model (.inm + TCH1) lives in the mfg partition. On its own the app takes
 * everything after the 4 KB partition header; the combined Teach firmware
 * (apps/ai/ipro_reef_teach) gives each Teach its own slot. */
#ifndef TEACH_MFG_OFFSET
#define TEACH_MFG_OFFSET 0x1000u
#endif
#ifndef TEACH_MFG_SIZE
#define TEACH_MFG_SIZE   0u                /* 0: to the end of the partition */
#endif

#include "ipro_ai_store.h"

#define TEACH_IN_W        128
#define TEACH_IN_H        128
#define TEACH_MAX_CLASSES 8
#define TEACH_LABEL_LEN   16

#define TEACH_TRAILER_MAGIC   "TCH1"
#define TEACH_TRAILER_VERSION 1u

typedef struct {
    uint8_t  cls;                          /* argmax                         */
    uint8_t  conf;                         /* softmax of the winner, 0..100  */
    uint8_t  prob[TEACH_MAX_CLASSES];      /* softmax, 0..100 %              */
    int16_t  logit_x100[TEACH_MAX_CLASSES];/* averaged logits x100, bench    */
    uint32_t seq;                          /* bumps once per result          */
    uint32_t npu_ms;
} teach_result_t;

typedef struct {
    uint32_t frames, errors;
    uint32_t fps_x10;
    uint32_t model_bytes, load_ms, free_heap;
    uint8_t  nclass;                       /* from the label block (or out_c) */
    bool     labelled;                     /* label block found              */
    uint16_t out_w, out_h, out_c, out_pitch;
    float    out_scale;
    int      out_zp;
} teach_stats_t;

typedef void (*teach_done_cb_t)(void *arg);

/** @brief Allocate the input frame, load the model, start the task. Call
 *         before the camera allocates its frames. 0 = ready. */
int  teach_model_init(void);
bool teach_model_ready(void);
const char *teach_model_fail_reason(void);
/** The model's flash slot, for deploys (ipro_ai_store.h): one bank, the model is too big for two. */
const ipro_ai_slot_t *teach_model_slot(void);

/** @brief ISP channel-1 destination (128x128 ARGB), 0 before init. */
uint32_t teach_model_input_addr(void);

/** @brief Hand a completed frame to the NPU. ISR-safe. 0 = taken. */
int  teach_model_push(uint32_t addr);
void teach_model_set_done_cb(teach_done_cb_t cb, void *arg);

void teach_model_get_result(teach_result_t *out);
void teach_model_get_stats(teach_stats_t *out);
const char *teach_model_label(unsigned cls);

/**
 * @brief Copy of the next frame the NPU processes (128x128 ARGB, 4 B/px, as
 *        the ISP wrote it). Blocks up to @p timeout_ms.
 * @param seq  if not NULL, the result seq this frame's inference will carry,
 *             so its logits can be matched to the copy
 * @return 0 and *out valid until the next call; <0 on timeout / not ready.
 */
int teach_model_capture(const uint8_t **out, uint32_t *seq, uint32_t timeout_ms);

/** @brief The result of the frame the last capture copied, once inferred.
 *  @return 0 and *out filled; <0 if that frame's result is not in yet. */
int teach_model_capture_result(teach_result_t *out);

/**
 * @brief Stop handing frames to the NPU (pause = true) and wait up to
 *        @p timeout_ms for the inference in flight to finish. The model is read
 *        over XIP, so this must hold before its flash is erased.
 * @return 0 once the NPU is idle (or when resuming); <0 on timeout.
 */
int teach_model_pause(bool pause, uint32_t timeout_ms);

/** @brief Print every result to the console (default off). */
void teach_model_set_print(bool on);

/**
 * @brief One inference on raw input bytes (exactly the input tensor's, as the
 *        NPU reads them), the camera's frames held off meanwhile: the image
 *        protocol's TRUN. Copies the raw output (at most out_cap bytes).
 * @return 0, -1 not loaded, -2 wrong size (or an output over out_cap), -3 no
 *         memory, -4 NPU failed, -5 the camera's frame did not finish in time
 */
int teach_model_run_raw(const uint8_t *in, uint32_t bytes, uint8_t *out, uint32_t out_cap,
                        uint32_t *out_bytes, uint32_t *us);

/** @brief TBEN: the model n times on the frame buffer, camera held off; a reply line. */
int teach_model_bench(char *line, uint32_t size, uint32_t n);

/** @brief TMOD: what the store found and the tensors, as a reply line. */
int teach_model_describe(char *line, uint32_t size, const char *word);

#endif /* TEACH_MODEL_H */
