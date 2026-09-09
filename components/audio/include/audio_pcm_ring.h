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
 * @file audio_pcm_ring.h
 * @brief Lock-free single-producer/single-consumer PCM byte ring.
 *
 * The producer and consumer may run in task/ISR context in either order, but
 * there must be exactly one of each.  Writes and reads are frame-atomic: a
 * request that does not fit is rejected without modifying the ring.
 */
#ifndef AUDIO_PCM_RING_H
#define AUDIO_PCM_RING_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint8_t *storage;
    uint32_t capacity;
    volatile uint32_t write_count;
    volatile uint32_t read_count;
} audio_pcm_ring_t;

int audio_pcm_ring_init(audio_pcm_ring_t *ring, void *storage,
                        size_t capacity);
void audio_pcm_ring_reset(audio_pcm_ring_t *ring);
size_t audio_pcm_ring_available(const audio_pcm_ring_t *ring);
size_t audio_pcm_ring_free(const audio_pcm_ring_t *ring);
bool audio_pcm_ring_write(audio_pcm_ring_t *ring, const void *data,
                          size_t length);
bool audio_pcm_ring_read(audio_pcm_ring_t *ring, void *data, size_t length);

#endif /* AUDIO_PCM_RING_H */
