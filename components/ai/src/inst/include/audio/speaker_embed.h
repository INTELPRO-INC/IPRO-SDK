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

#ifndef SPEAKER_EMBED_H
#define SPEAKER_EMBED_H

#include <stdint.h>
#include "audio/embedding_dim.h"

/*
 * Speaker embedding engine — the shared front-end behind all speaker apps
 * (identify / verify / diarize / batch benchmark). Pipeline:
 *
 *   int16 PCM 16k mono -> fbank[300][80] -> int8 offset-binary
 *     -> res2dnet on NPU -> uint8 dequant -> L2-norm -> embedding[256]
 *
 * The fbank back-end (float / integer) is chosen by CONFIG_AI_FBANK_INT; the
 * NPU I/O contract (offset-binary int8 in via imgBuffer+4, uint8 out) is the
 * validated path from demo_ai ai_run_ext 5. Not reentrant (shared scratch).
 */

/* Load the res2dnet model from `model_path` (the SD card must already be
 * mounted), init the fbank front-end and NPU buffers. Call once before
 * speaker_embed(). Returns 0 on success, -1 on error. */
int speaker_embed_init(const char *model_path);

/* Init the engine from a model already resident in RAM (e.g. read from a flash
 * partition). `model_buf` need only stay valid for the duration of this call.
 * Same as speaker_embed_init() minus the file read. Returns 0 on success. */
int speaker_embed_init_buf(const uint8_t *model_buf);

/* Compute the L2-normalised speaker embedding for one PCM clip.
 *   pcm        : int16 mono 16 kHz
 *   n_samples  : clip length (capped at 3 s / 300 frames; shorter is zero-padded)
 *   emb        : output [EMBEDDING_DIM], L2-normalised
 * Returns 0 on success, -1 on error. */
int speaker_embed(const int16_t *pcm, int n_samples, float emb[EMBEDDING_DIM]);

#endif /* SPEAKER_EMBED_H */
