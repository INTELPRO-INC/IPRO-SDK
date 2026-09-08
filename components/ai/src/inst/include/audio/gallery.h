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

#ifndef GALLERY_H
#define GALLERY_H

#include <stddef.h>
#include "embedding_dim.h"

/*
 * Speaker gallery — RTOS port of speaker_id_rtos/gallery.{c,h}.
 *
 * The file-backed gallery_load()/gallery_save() of the Linux reference are
 * dropped (no filesystem). gallery_load_from_buf() is kept for a future
 * flash-resident enrolled set; the live demo enrolls into RAM only and the
 * gallery is cleared on reboot.
 */

#define GALLERY_MAX_SPEAKERS  64
#define GALLERY_NAME_LEN      64
#define GALLERY_MAGIC         0x474C5279u  /* "GLRy" */

/* Decision threshold for the C-fbank deployment (PORTING_GUIDE §5).
 * NOTE: 0.42, not the Python-calibrated 0.53 — enrollment and identification
 * both run the same C fbank, so the C-vs-Python feature offset cancels. */
#define GALLERY_DEFAULT_THRESHOLD  0.42f

typedef struct {
    int     n;
    int     dim;
    float   threshold;
    char    names[GALLERY_MAX_SPEAKERS][GALLERY_NAME_LEN];
    float   embeddings[GALLERY_MAX_SPEAKERS][EMBEDDING_DIM];  /* L2-normalised mean */
    /* Multi-clip enrollment accumulator (gallery_enroll): running sum of the
     * per-clip embeddings + clip count. embeddings[] = normalise(acc). */
    float   acc[GALLERY_MAX_SPEAKERS][EMBEDDING_DIM];
    int     count[GALLERY_MAX_SPEAKERS];
} Gallery;

/* Reset to an empty gallery with the given decision threshold. */
void gallery_init(Gallery *g, float threshold);

/* Drop all enrolled speakers (keeps dim/threshold). */
void gallery_clear(Gallery *g);

/*
 * Load a gallery_c.bin image from a memory buffer (e.g. flash section).
 * Returns 0 on success, -1 on error. After a successful load, each
 * speaker's enrollment accumulator is seeded with the stored mean
 * (count = 1) so further gallery_enroll() clips accumulate sanely.
 */
int gallery_load_from_buf(Gallery *g, const void *buf, size_t len);

/*
 * Serialize the gallery to the gallery_c.bin format understood by
 * gallery_load_from_buf (acc/count are NOT persisted). At most
 * max_speakers entries are written (pass GALLERY_MAX_SPEAKERS or -1 for
 * no extra cap). Returns the number of bytes written, or -1 if buf_size
 * is too small.
 */
int gallery_save_to_buf(const Gallery *g, void *buf, size_t buf_size,
                        int max_speakers);

/*
 * Add or update a speaker (single embedding, OVERWRITES any existing one).
 * embedding must be L2-normalised [EMBEDDING_DIM].
 * Returns 0 on success, -1 if the gallery is full.
 */
int gallery_upsert(Gallery *g, const char *name,
                   const float embedding[EMBEDDING_DIM]);

/*
 * Multi-clip enrollment: ACCUMULATE this embedding into the named speaker
 * (creating it on first call) and store the L2-normalised running mean.
 * Call once per enrollment clip — 3+ clips give a more robust voiceprint
 * than a single one. embedding must be L2-normalised [EMBEDDING_DIM].
 * Returns the number of clips accumulated so far (>=1), or -1 if full.
 */
int gallery_enroll(Gallery *g, const char *name,
                   const float embedding[EMBEDDING_DIM]);

/*
 * Identify a speaker by cosine similarity (dot product of unit vectors).
 * Returns the best-match index if score >= threshold, else -1 (reject).
 * score_out (may be NULL) receives the best cosine score.
 */
int gallery_identify(const Gallery *g,
                     const float embedding[EMBEDDING_DIM],
                     float *score_out);

/*
 * Remove speaker at index idx (0..n-1), shifting later entries down so slot
 * ids stay dense. Returns 0 on success, -1 if idx is out of range.
 * NOT thread-safe — callers sharing a Gallery across tasks must serialize.
 */
int gallery_remove(Gallery *g, int idx);

/*
 * Rename speaker at index idx. name is truncated to GALLERY_NAME_LEN-1.
 * Returns 0 on success, -1 if idx out of range or name NULL/empty,
 * -2 if another slot already holds that name (would break the name lookups
 * used by gallery_enroll/gallery_upsert/gallery_verify).
 */
int gallery_rename(Gallery *g, int idx, const char *name);

/*
 * Verify a claimed identity (1:1). Compares the embedding only against speaker
 * `name`'s stored voiceprint.
 *   returns  1 = ACCEPT (cosine >= threshold)
 *            0 = REJECT (cosine <  threshold)
 *           -2 = no speaker named `name` in the gallery
 * score_out (may be NULL) receives the cosine score.
 */
int gallery_verify(const Gallery *g, const char *name,
                   const float embedding[EMBEDDING_DIM], float *score_out);

#endif /* GALLERY_H */
