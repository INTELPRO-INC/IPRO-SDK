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

#ifndef APP_JSON_H
#define APP_JSON_H

#include <stdio.h>
#include <string.h>

/**
 * Decode JSON Unicode escapes (\uXXXX) to UTF-8 in-place.
 * Also handles standard escapes (\n, \t, \\, \", \/).
 * Result is always <= input length, so in-place is safe.
 */
static inline void json_unescape_inplace(char *s) {
    char *r = s, *w = s;
    while (*r) {
        if (r[0] == '\\' && r[1]) {
            r++; /* skip backslash */
            switch (*r) {
            case '"':  *w++ = '"';  r++; break;
            case '\\': *w++ = '\\'; r++; break;
            case '/':  *w++ = '/';  r++; break;
            case 'n':  *w++ = '\n'; r++; break;
            case 'r':  *w++ = '\r'; r++; break;
            case 't':  *w++ = '\t'; r++; break;
            case 'b':  *w++ = '\b'; r++; break;
            case 'f':  *w++ = '\f'; r++; break;
            case 'u': {
                /* Parse \uXXXX hex code point */
                r++; /* skip 'u' */
                unsigned cp = 0;
                for (int i = 0; i < 4 && *r; i++, r++) {
                    cp <<= 4;
                    if (*r >= '0' && *r <= '9')      cp |= *r - '0';
                    else if (*r >= 'a' && *r <= 'f') cp |= *r - 'a' + 10;
                    else if (*r >= 'A' && *r <= 'F') cp |= *r - 'A' + 10;
                    else break;
                }
                /* UTF-16 surrogate pair: \uD800-\uDBFF \uDC00-\uDFFF */
                if (cp >= 0xD800 && cp <= 0xDBFF &&
                    r[0] == '\\' && r[1] == 'u') {
                    r += 2; /* skip \u */
                    unsigned lo = 0;
                    for (int i = 0; i < 4 && *r; i++, r++) {
                        lo <<= 4;
                        if (*r >= '0' && *r <= '9')      lo |= *r - '0';
                        else if (*r >= 'a' && *r <= 'f') lo |= *r - 'a' + 10;
                        else if (*r >= 'A' && *r <= 'F') lo |= *r - 'A' + 10;
                        else break;
                    }
                    cp = 0x10000 + ((cp - 0xD800) << 10) + (lo - 0xDC00);
                }
                /* Encode code point as UTF-8 */
                if (cp < 0x80) {
                    *w++ = (char)cp;
                } else if (cp < 0x800) {
                    *w++ = (char)(0xC0 | (cp >> 6));
                    *w++ = (char)(0x80 | (cp & 0x3F));
                } else if (cp < 0x10000) {
                    *w++ = (char)(0xE0 | (cp >> 12));
                    *w++ = (char)(0x80 | ((cp >> 6) & 0x3F));
                    *w++ = (char)(0x80 | (cp & 0x3F));
                } else {
                    *w++ = (char)(0xF0 | (cp >> 18));
                    *w++ = (char)(0x80 | ((cp >> 12) & 0x3F));
                    *w++ = (char)(0x80 | ((cp >> 6) & 0x3F));
                    *w++ = (char)(0x80 | (cp & 0x3F));
                }
                break;
            }
            default: *w++ = *r++; break;
            }
        } else {
            *w++ = *r++;
        }
    }
    *w = '\0';
}

/**
 * Simple JSON string extractor (destructive — null-terminates in-place).
 * Returns pointer into the buffer, or NULL if key not found.
 * Unicode escapes (\uXXXX) are decoded to UTF-8.
 */
static inline char *json_extract_string(char *json, const char *key) {
    char search[64];
    int n = snprintf(search, sizeof(search), "\"%s\"", key);
    if (n <= 0 || (size_t)n >= sizeof(search))
        return NULL;

    char *p = strstr(json, search);
    if (!p)
        return NULL;

    p += n; /* skip "key" */
    /* skip optional whitespace and colon */
    while (*p == ' ' || *p == ':' || *p == '\t')
        p++;
    if (*p != '"')
        return NULL;
    p++; /* skip opening quote */

    /* Find closing quote (handle escaped quotes) */
    char *start = p;
    while (*p && !(*p == '"' && *(p - 1) != '\\'))
        p++;
    if (*p == '"')
        *p = '\0';

    json_unescape_inplace(start);
    return start;
}

#endif /* APP_JSON_H */
