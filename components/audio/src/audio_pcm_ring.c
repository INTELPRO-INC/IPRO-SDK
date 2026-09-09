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

#include "audio_pcm_ring.h"

#include <limits.h>
#include <string.h>

static uint32_t load_acquire(const volatile uint32_t *value)
{
    return __atomic_load_n(value, __ATOMIC_ACQUIRE);
}

static uint32_t load_relaxed(const volatile uint32_t *value)
{
    return __atomic_load_n(value, __ATOMIC_RELAXED);
}

static void store_release(volatile uint32_t *value, uint32_t next)
{
    __atomic_store_n(value, next, __ATOMIC_RELEASE);
}

int audio_pcm_ring_init(audio_pcm_ring_t *ring, void *storage,
                        size_t capacity)
{
    if (ring == NULL || storage == NULL || capacity == 0u ||
        capacity > (size_t)INT32_MAX) {
        return -1;
    }

    ring->storage = storage;
    ring->capacity = (uint32_t)capacity;
    ring->write_count = 0u;
    ring->read_count = 0u;
    return 0;
}

void audio_pcm_ring_reset(audio_pcm_ring_t *ring)
{
    if (ring == NULL) {
        return;
    }

    /* Reset is only valid while producer and consumer are quiescent. */
    __atomic_store_n(&ring->write_count, 0u, __ATOMIC_RELAXED);
    __atomic_store_n(&ring->read_count, 0u, __ATOMIC_RELAXED);
}

size_t audio_pcm_ring_available(const audio_pcm_ring_t *ring)
{
    uint32_t write_count;
    uint32_t read_count;

    if (ring == NULL || ring->storage == NULL || ring->capacity == 0u) {
        return 0u;
    }

    write_count = load_acquire(&ring->write_count);
    read_count = load_acquire(&ring->read_count);
    return (size_t)(write_count - read_count);
}

size_t audio_pcm_ring_free(const audio_pcm_ring_t *ring)
{
    size_t used;

    if (ring == NULL || ring->storage == NULL || ring->capacity == 0u) {
        return 0u;
    }

    used = audio_pcm_ring_available(ring);
    return used <= ring->capacity ? ring->capacity - used : 0u;
}

static void copy_wrapped(uint8_t *ring_storage, uint32_t capacity,
                         uint32_t count, void *data, size_t length,
                         bool to_ring)
{
    uint32_t offset = count % capacity;
    size_t first = capacity - offset;

    if (first > length) {
        first = length;
    }

    if (to_ring) {
        memcpy(&ring_storage[offset], data, first);
        memcpy(ring_storage, (const uint8_t *)data + first, length - first);
    } else {
        memcpy(data, &ring_storage[offset], first);
        memcpy((uint8_t *)data + first, ring_storage, length - first);
    }
}

bool audio_pcm_ring_write(audio_pcm_ring_t *ring, const void *data,
                          size_t length)
{
    uint32_t write_count;
    uint32_t read_count;
    uint32_t used;

    if (ring == NULL || ring->storage == NULL || ring->capacity == 0u ||
        (data == NULL && length != 0u) || length > ring->capacity) {
        return false;
    }
    if (length == 0u) {
        return true;
    }

    write_count = load_relaxed(&ring->write_count);
    read_count = load_acquire(&ring->read_count);
    used = write_count - read_count;
    if (used > ring->capacity || length > ring->capacity - used) {
        return false;
    }

    copy_wrapped(ring->storage, ring->capacity, write_count,
                 (void *)data, length, true);
    store_release(&ring->write_count, write_count + (uint32_t)length);
    return true;
}

bool audio_pcm_ring_read(audio_pcm_ring_t *ring, void *data, size_t length)
{
    uint32_t write_count;
    uint32_t read_count;
    uint32_t used;

    if (ring == NULL || ring->storage == NULL || ring->capacity == 0u ||
        (data == NULL && length != 0u) || length > ring->capacity) {
        return false;
    }
    if (length == 0u) {
        return true;
    }

    read_count = load_relaxed(&ring->read_count);
    write_count = load_acquire(&ring->write_count);
    used = write_count - read_count;
    if (used > ring->capacity || length > used) {
        return false;
    }

    copy_wrapped(ring->storage, ring->capacity, read_count, data, length,
                 false);
    store_release(&ring->read_count, read_count + (uint32_t)length);
    return true;
}
