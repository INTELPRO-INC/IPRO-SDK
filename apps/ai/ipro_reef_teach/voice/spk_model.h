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
 * @file spk_model.h
 * @brief Model B voiceprint: 2.575 s of 16 kHz PCM -> L2-normalised embedding.
 *
 * The model is one SPKB blob in the mfg partition (behind the 4 KB partition
 * header): an NPU trunk (.inm) that turns 256 log-mel frames into a 16 x 5 x C
 * feature map, and a CPU head - mean and standard deviation over time, one
 * linear layer - that turns the map into a voiceprint. spk_model.c's
 * spkb_hdr_t is the layout: a 128-byte header, the .inm, the head in float16,
 * a CRC over everything after the header.
 *
 * The front end is components/ai's fbank.c, the same one the model was trained
 * on, so the model sees on the board what it saw in training.
 */

#ifndef SPK_MODEL_H
#define SPK_MODEL_H

#include <stdint.h>

/* Where the voiceprint model (SPKB) lives in the mfg partition. On its own the app takes
 * everything after the 4 KB partition header; the combined Teach firmware
 * (apps/ai/ipro_reef_teach) gives each Teach its own slot. */
#ifndef VOICE_MFG_OFFSET
#define VOICE_MFG_OFFSET 0x1000u
#endif
#ifndef VOICE_MFG_SIZE
#define VOICE_MFG_SIZE   0u                /* 0: to the end of the partition */
#endif

#include "audio/embedding_dim.h"
#include "ipro_ai_store.h"

#define SPK_MODEL_FRAMES   256u
/** PCM a window must hold: (frames - 1) * 160 + 400 samples at 16 kHz. */
#define SPK_MODEL_SAMPLES  ((SPK_MODEL_FRAMES - 1u) * 160u + 400u)

typedef struct {
    char     name[32];
    uint32_t blob_bytes;
    uint32_t inm_bytes;
    uint32_t dim;
    uint32_t crc;          /**< the blob's CRC: identifies the model voiceprints came from */
    uint32_t last_us;      /**< duration of the last spk_model_embed() */
} spk_model_info_t;

/** Load and check the blob from mfg, set up the NPU. 0, or -1 (logged). */
int spk_model_init(void);

/** The model's flash slot, for deploys (ipro_ai_store.h): one bank, the model is too big for two. */
const ipro_ai_slot_t *spk_model_slot(void);

/** 1 when bytes is an SPKB this firmware can run: header, sizes, dimension and CRC. */
int spk_model_payload_ok(const uint8_t *blob, uint32_t bytes);

/** 1 once spk_model_init() has succeeded. */
int spk_model_ready(void);

/**
 * Voiceprint of exactly SPK_MODEL_SAMPLES samples. Serialised with the
 * functions below by the model's own lock. 0, or -1.
 */
int spk_model_embed(const int16_t *pcm, float emb[EMBEDDING_DIM]);

void spk_model_get_info(spk_model_info_t *info);

/** RUN: the trunk on raw input bytes (exactly the input tensor's); its raw output. 0, -1 no
 *  model, -2 wrong size, -4 NPU failed. Safe beside the voice task. */
int spk_model_run_raw(const uint8_t *in, uint32_t bytes, uint8_t *out, uint32_t cap,
                      uint32_t *out_bytes, uint32_t *us);
/** FEAT: the front end (log-mel, quantised) of SPK_MODEL_SAMPLES samples into
 *  SPK_MODEL_FRAMES x 80 bytes. 0 or -1. */
int spk_model_feat(const int16_t *pcm, uint32_t samples, uint8_t *out);
/** BENCH and MODEL reply lines (docs/edge_ai/06 §5). */
int spk_model_bench(char *line, uint32_t size, uint32_t n);
int spk_model_describe(char *line, uint32_t size);

#endif /* SPK_MODEL_H */
