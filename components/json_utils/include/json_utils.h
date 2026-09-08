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

#ifndef JSON_UTILS_H
#define JSON_UTILS_H

#include <stdint.h>
#include <stdbool.h>

/**
 * Lightweight JSON helpers for embedded use.
 * No dynamic allocation — works with caller-provided buffers.
 */

/* Find a string value for a key in a JSON object. Returns pointer into json. */
const char *json_find_str(const char *json, const char *key,
                             char *out, uint32_t out_size);

/* Find an integer value for a key in a JSON object. */
int json_find_int(const char *json, const char *key, int *out);

/* Find a 64-bit integer value for a key. */
int json_find_int64(const char *json, const char *key, int64_t *out);

/* Find a boolean value for a key. */
int json_find_bool(const char *json, const char *key, bool *out);

/* Find a nested object/array for a key. Returns pointer to opening brace/bracket. */
const char *json_find_object(const char *json, const char *key);

/* Escape a string for JSON output. Returns bytes written. */
int json_escape_str(const char *src, char *dst, uint32_t dst_size);

/* Append a key-value pair to a JSON buffer being built. */
int json_append_str(char *buf, uint32_t buf_size, uint32_t *pos,
                       const char *key, const char *value);
int json_append_int(char *buf, uint32_t buf_size, uint32_t *pos,
                       const char *key, int value);

/* Append an unsigned integer key-value pair (renders %u, so values
 * above INT_MAX serialize correctly instead of wrapping negative). */
int json_append_uint(char *buf, uint32_t buf_size, uint32_t *pos,
                       const char *key, uint32_t value);

#endif /* JSON_UTILS_H */
