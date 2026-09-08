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

#ifndef DIARIZE_H
#define DIARIZE_H

#include "embedding_dim.h"

/*
 * Speaker diarization — online greedy clustering of speaker embeddings.
 * No enrollment: each segment's embedding is matched to existing cluster
 * centroids; merge if cosine >= threshold, else start a new cluster.
 * Same returned index == same speaker. Fixed-size state, no heap.
 *
 * Port of speaker_id_rtos/diarize.c. Takes the L2-normalised embedding from
 * speaker_embed() (clustering only — decoupled from fbank/NPU).
 */

#define DIARIZE_MAX_CLUSTERS        32
#define DIARIZE_DEFAULT_THRESHOLD   0.55f   /* higher = fewer clusters */

typedef struct {
    float centroids[DIARIZE_MAX_CLUSTERS][EMBEDDING_DIM];  /* L2-normalised */
    int   counts[DIARIZE_MAX_CLUSTERS];
    int   n_clusters;
} DiarizeState;

/* Reset to zero clusters. Call once before the first diarize_update(). */
void diarize_init(DiarizeState *s);

/*
 * Assign one segment embedding to a speaker cluster.
 *   emb       : L2-normalised embedding (from speaker_embed)
 *   threshold : merge threshold (e.g. DIARIZE_DEFAULT_THRESHOLD)
 * Returns the 0-based cluster index (new speaker => new index), or -1 if the
 * cluster table is full.
 */
int diarize_update(DiarizeState *s, const float emb[EMBEDDING_DIM], float threshold);

#endif /* DIARIZE_H */
