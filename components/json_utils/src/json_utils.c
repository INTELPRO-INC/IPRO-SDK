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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "json_utils.h"

/*
 * Minimal JSON parser for embedded use.
 * Not a full parser — handles the subset we need:
 * - Top-level or nested object key lookup
 * - String, integer, boolean value extraction
 * - JSON string escaping for output
 */

/* Skip whitespace */
static const char *skip_ws(const char *p)
{
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        p++;
    return p;
}

/*
 * Skip a JSON string body (cursor starts after opening quote).
 * Handles escape sequences correctly, including \\" (escaped backslash
 * followed by closing quote).
 */
static const char *skip_json_string_body(const char *p)
{
    while (*p) {
        if (*p == '\\') {
            if (*(p + 1) == '\0')
                return p; /* truncated escape at end of input */
            p += 2; /* skip escape sequence (\n, \\, \", etc.) */
            continue;
        }
        if (*p == '"')
            return p + 1; /* past closing quote */
        p++;
    }
    return p;
}

/* Skip a JSON value (string, number, object, array, bool, null) */
static const char *skip_value(const char *p)
{
    p = skip_ws(p);
    if (*p == '"') {
        return skip_json_string_body(p + 1);
    }
    if (*p == '{' || *p == '[') {
        /* Object or array: match braces/brackets */
        char open = *p;
        char close = (open == '{') ? '}' : ']';
        int depth = 1;
        p++;
        while (*p && depth > 0) {
            if (*p == '"') {
                p = skip_json_string_body(p + 1);
                continue;
            }
            if (*p == open) depth++;
            if (*p == close) depth--;
            p++;
        }
        return p;
    }
    /* Number, bool, null: skip to delimiter */
    while (*p && *p != ',' && *p != '}' && *p != ']' &&
           *p != ' ' && *p != '\t' && *p != '\n' && *p != '\r')
        p++;
    return p;
}

/* Find a key in the current JSON object level and return pointer to its value */
static const char *find_key(const char *json, const char *key)
{
    int key_len = strlen(key);

    const char *p = skip_ws(json);
    if (*p != '{')
        return NULL;
    p++;

    while (*p) {
        p = skip_ws(p);
        if (*p == '}')
            return NULL;

        /* Expect a key string */
        if (*p != '"')
            return NULL;
        p++;
        const char *k_start = p;
        while (*p && *p != '"') {
            if (*p == '\\' && *(p + 1))
                p++;
            p++;
        }
        int k_len = p - k_start;
        if (*p == '"')
            p++;

        /* Skip colon */
        p = skip_ws(p);
        if (*p != ':')
            return NULL;
        p++;
        p = skip_ws(p);

        /* Check if this is our key */
        if (k_len == key_len && memcmp(k_start, key, key_len) == 0)
            return p; /* pointer to value */

        /* Skip this value */
        p = skip_value(p);
        p = skip_ws(p);
        if (*p == ',')
            p++;
    }
    return NULL;
}

const char *json_find_str(const char *json, const char *key,
                             char *out, uint32_t out_size)
{
    if (!out || out_size == 0)
        return NULL;

    const char *val = find_key(json, key);
    if (!val || *val != '"')
        return NULL;

    val++; /* skip opening quote */
    uint32_t i = 0;
    while (*val && *val != '"' && i < out_size - 1) {
        if (*val == '\\' && *(val + 1)) {
            val++;
            switch (*val) {
            case 'n': out[i++] = '\n'; break;
            case 'r': out[i++] = '\r'; break;
            case 't': out[i++] = '\t'; break;
            case '"': out[i++] = '"';  break;
            case '\\': out[i++] = '\\'; break;
            case '/': out[i++] = '/';  break;
            case 'u': {
                /* \uXXXX Unicode escape → decode to UTF-8 */
                uint32_t cp = 0;
                int ok = 1;
                for (int j = 1; j <= 4; j++) {
                    char c = val[j];
                    cp <<= 4;
                    if (c >= '0' && c <= '9')      cp |= c - '0';
                    else if (c >= 'a' && c <= 'f') cp |= c - 'a' + 10;
                    else if (c >= 'A' && c <= 'F') cp |= c - 'A' + 10;
                    else { ok = 0; break; }
                }
                if (!ok) { out[i++] = 'u'; break; }
                val += 4;

                /* Surrogate pair: high \uD800-DBFF + low \uDC00-DFFF */
                if (cp >= 0xD800 && cp <= 0xDBFF &&
                    val[1] == '\\' && val[2] == 'u') {
                    uint32_t lo = 0;
                    ok = 1;
                    for (int j = 3; j <= 6; j++) {
                        char c = val[j];
                        lo <<= 4;
                        if (c >= '0' && c <= '9')      lo |= c - '0';
                        else if (c >= 'a' && c <= 'f') lo |= c - 'a' + 10;
                        else if (c >= 'A' && c <= 'F') lo |= c - 'A' + 10;
                        else { ok = 0; break; }
                    }
                    if (ok && lo >= 0xDC00 && lo <= 0xDFFF) {
                        cp = 0x10000 + ((cp - 0xD800) << 10) +
                             (lo - 0xDC00);
                        val += 6;
                    }
                }

                /* Reject unpaired surrogates — never valid in UTF-8 */
                if (cp >= 0xD800 && cp <= 0xDFFF) {
                    out[i++] = '?';
                    break;
                }

                /* Encode codepoint as UTF-8 */
                if (cp < 0x80) {
                    if (i < out_size - 1)
                        out[i++] = (char)cp;
                } else if (cp < 0x800) {
                    if (i + 2 <= out_size - 1) {
                        out[i++] = 0xC0 | (cp >> 6);
                        out[i++] = 0x80 | (cp & 0x3F);
                    }
                } else if (cp < 0x10000) {
                    if (i + 3 <= out_size - 1) {
                        out[i++] = 0xE0 | (cp >> 12);
                        out[i++] = 0x80 | ((cp >> 6) & 0x3F);
                        out[i++] = 0x80 | (cp & 0x3F);
                    }
                } else if (cp < 0x110000) {
                    if (i + 4 <= out_size - 1) {
                        out[i++] = 0xF0 | (cp >> 18);
                        out[i++] = 0x80 | ((cp >> 12) & 0x3F);
                        out[i++] = 0x80 | ((cp >> 6) & 0x3F);
                        out[i++] = 0x80 | (cp & 0x3F);
                    }
                }
                break;
            }
            default: out[i++] = *val; break;
            }
        } else {
            out[i++] = *val;
        }
        val++;
    }
    out[i] = '\0';
    return out;
}

int json_find_int(const char *json, const char *key, int *out)
{
    const char *val = find_key(json, key);
    if (!val)
        return -1;

    *out = atoi(val);
    return 0;
}

int json_find_int64(const char *json, const char *key, int64_t *out)
{
    const char *val = find_key(json, key);
    if (!val)
        return -1;

    *out = strtoll(val, NULL, 10);
    return 0;
}

int json_find_bool(const char *json, const char *key, bool *out)
{
    const char *val = find_key(json, key);
    if (!val)
        return -1;

    *out = (*val == 't');
    return 0;
}

const char *json_find_object(const char *json, const char *key)
{
    const char *val = find_key(json, key);
    if (!val)
        return NULL;

    val = skip_ws(val);
    if (*val != '{' && *val != '[')
        return NULL;

    return val;
}

int json_escape_str(const char *src, char *dst, uint32_t dst_size)
{
    if (!dst || dst_size < 2) {
        if (dst && dst_size > 0) dst[0] = '\0';
        return 0;
    }

    uint32_t i = 0;

    while (*src && i < dst_size - 2) {
        unsigned char c = (unsigned char)*src;

        /* ASCII fast path */
        if (c < 0x80) {
            switch (c) {
            case '"':  dst[i++] = '\\'; dst[i++] = '"';  break;
            case '\\': dst[i++] = '\\'; dst[i++] = '\\'; break;
            case '\n': dst[i++] = '\\'; dst[i++] = 'n';  break;
            case '\r': dst[i++] = '\\'; dst[i++] = 'r';  break;
            case '\t': dst[i++] = '\\'; dst[i++] = 't';  break;
            default:
                if (c >= 0x20)
                    dst[i++] = c;
                break;
            }
            src++;
            continue;
        }

        /*
         * Multi-byte UTF-8: determine sequence length from lead byte.
         * Copy entire sequence atomically to avoid mid-character truncation.
         * Reject surrogate-encoded bytes (U+D800..U+DFFF → ED A0..ED BF).
         */
        int seq_len;
        if ((c & 0xE0) == 0xC0)      seq_len = 2;
        else if ((c & 0xF0) == 0xE0) seq_len = 3;
        else if ((c & 0xF8) == 0xF0) seq_len = 4;
        else { src++; continue; }  /* invalid lead byte — skip */

        /* Verify all continuation bytes are present */
        int valid = 1;
        for (int j = 1; j < seq_len; j++) {
            if (((unsigned char)src[j] & 0xC0) != 0x80) {
                valid = 0;
                break;
            }
        }
        if (!valid) { src++; continue; }

        /* Reject UTF-8 encoded surrogates: ED A0..ED BF xx */
        if (c == 0xED && (unsigned char)src[1] >= 0xA0) {
            src += seq_len;
            continue;
        }

        /* Check buffer space for entire sequence */
        if (i + seq_len > dst_size - 1) break;

        for (int j = 0; j < seq_len; j++)
            dst[i++] = src[j];
        src += seq_len;
    }
    dst[i] = '\0';
    return i;
}

int json_append_str(char *buf, uint32_t buf_size, uint32_t *pos,
                       const char *key, const char *value)
{
    char escaped[1024];
    json_escape_str(value, escaped, sizeof(escaped));

    int written = snprintf(buf + *pos, buf_size - *pos,
                           "\"%s\":\"%s\"", key, escaped);
    if (written < 0 || (uint32_t)written >= buf_size - *pos)
        return -1;
    *pos += written;
    return 0;
}

int json_append_int(char *buf, uint32_t buf_size, uint32_t *pos,
                       const char *key, int value)
{
    int written = snprintf(buf + *pos, buf_size - *pos,
                           "\"%s\":%d", key, value);
    if (written < 0 || (uint32_t)written >= buf_size - *pos)
        return -1;
    *pos += written;
    return 0;
}

int json_append_uint(char *buf, uint32_t buf_size, uint32_t *pos,
                       const char *key, uint32_t value)
{
    int written = snprintf(buf + *pos, buf_size - *pos,
                           "\"%s\":%u", key, (unsigned)value);
    if (written < 0 || (uint32_t)written >= buf_size - *pos)
        return -1;
    *pos += written;
    return 0;
}
