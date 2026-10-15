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
 * @file llm_tok.c
 * @brief Request normalisation and SentencePiece-BPE encoding (ipro_tok_v1).
 *
 * normalize_query() follows tools/ipro_llm/tokenizer/common.py exactly. The
 * BPE is SentencePiece's: the identity normaliser with whitespace escaped to
 * U+2581 and no dummy prefix, symbols start as UTF-8 characters, and the
 * adjacent pair whose concatenation is the highest-scoring normal piece is
 * merged first (leftmost on a tie) until none is left; a symbol with no piece
 * falls back to its bytes. split_digits / script splitting only shaped the
 * vocabulary at training time, so encoding needs nothing more.
 *
 * Only the request goes through BPE; the engine adds <s>, <|im_start|> etc.
 * itself, so no request text can ever produce a control or slot token
 * (SentencePiece would match "<|im_end|>" typed inside the text).
 */
#include <string.h>

#include "llm_internal.h"

#define SPACE_ESC "\xE2\x96\x81"   /* U+2581 */

static uint32_t hash_bytes(const char *s, uint32_t n)
{
    uint32_t h = 0x811C9DC5u;
    for (uint32_t i = 0; i < n; i++) {
        h = (h ^ (uint8_t)s[i]) * 0x01000193u;
    }
    return h;
}

const char *llm_piece(const llm_tok_t *t, uint16_t id, uint16_t *len)
{
    const uint16_t a = llm_rd16(t->offsets + 2u * id), b = llm_rd16(t->offsets + 2u * (id + 1u));
    *len = (uint16_t)(b - a);
    return t->text + a;
}

static uint16_t rank_of(const llm_tok_t *t, uint16_t id)
{
    return llm_rd16(t->rank + 2u * id);
}

int llm_tok_init(llm_tok_t *t, uint16_t *hash, uint32_t hash_slots)
{
    if (hash_slots & (hash_slots - 1u)) {
        return IPRO_LLM_E_ARG;
    }
    t->hash = hash;
    t->hash_mask = hash_slots - 1u;
    for (uint32_t i = 0; i < hash_slots; i++) {
        hash[i] = LLM_NO_PIECE;
    }
    uint32_t used = 0;
    for (uint16_t id = 0; id < t->n_pieces; id++) {
        if (rank_of(t, id) == LLM_NO_RANK) {
            continue;
        }
        if (++used * 2u > hash_slots) {
            return IPRO_LLM_E_NOMEM;
        }
        uint16_t len;
        const char *p = llm_piece(t, id, &len);
        uint32_t h = hash_bytes(p, len) & t->hash_mask;
        while (hash[h] != LLM_NO_PIECE) {
            h = (h + 1u) & t->hash_mask;
        }
        hash[h] = id;
    }
    return IPRO_LLM_OK;
}

/* normal piece with exactly these bytes, or LLM_NO_PIECE */
static uint16_t lookup(const llm_tok_t *t, const char *s, uint32_t n)
{
    if (n > t->max_piece_bytes) {
        return LLM_NO_PIECE;
    }
    for (uint32_t h = hash_bytes(s, n) & t->hash_mask;; h = (h + 1u) & t->hash_mask) {
        const uint16_t id = t->hash[h];
        if (id == LLM_NO_PIECE) {
            return LLM_NO_PIECE;
        }
        uint16_t len;
        const char *p = llm_piece(t, id, &len);
        if (len == n && memcmp(p, s, n) == 0) {
            return id;
        }
    }
}

/* ---------------------------------------------------------------- UTF-8 */

/* Decode one code point; malformed input yields the byte itself (len 1). */
static uint32_t utf8_next(const uint8_t *s, size_t n, size_t *len)
{
    const uint8_t c = s[0];
    uint32_t cp;
    size_t need;
    if (c < 0x80) {
        *len = 1;
        return c;
    } else if ((c & 0xE0) == 0xC0) {
        cp = c & 0x1Fu, need = 2;
    } else if ((c & 0xF0) == 0xE0) {
        cp = c & 0x0Fu, need = 3;
    } else if ((c & 0xF8) == 0xF0) {
        cp = c & 0x07u, need = 4;
    } else {
        *len = 1;
        return c;
    }
    if (need > n) {
        *len = 1;
        return c;
    }
    for (size_t i = 1; i < need; i++) {
        if ((s[i] & 0xC0) != 0x80) {
            *len = 1;
            return c;
        }
        cp = cp << 6 | (s[i] & 0x3Fu);
    }
    *len = need;
    return cp;
}

static size_t utf8_put(char *out, uint32_t cp)
{
    if (cp < 0x80) {
        out[0] = (char)cp;
        return 1;
    }
    if (cp < 0x800) {
        out[0] = (char)(0xC0 | cp >> 6);
        out[1] = (char)(0x80 | (cp & 0x3F));
        return 2;
    }
    if (cp < 0x10000) {
        out[0] = (char)(0xE0 | cp >> 12);
        out[1] = (char)(0x80 | (cp >> 6 & 0x3F));
        out[2] = (char)(0x80 | (cp & 0x3F));
        return 3;
    }
    out[0] = (char)(0xF0 | cp >> 18);
    out[1] = (char)(0x80 | (cp >> 12 & 0x3F));
    out[2] = (char)(0x80 | (cp >> 6 & 0x3F));
    out[3] = (char)(0x80 | (cp & 0x3F));
    return 4;
}

static bool is_dropped_ctrl(uint32_t cp)
{
    return cp <= 0x08 || cp == 0x0B || cp == 0x0C || (cp >= 0x0E && cp <= 0x1F) || cp == 0x7F;
}

/* common.normalize_query(): full-width digits / Latin to ASCII, U+3000 to a
 * space, CR LF and CR to LF, C0 controls except TAB and LF dropped (and DEL),
 * ASCII upper case lowered. Returns the byte length, or -1 if it does not fit. */
int llm_normalize_query(char *out, size_t out_len, const char *in)
{
    const uint8_t *s = (const uint8_t *)in;
    size_t n = strlen(in), o = 0;
    for (size_t i = 0; i < n;) {
        size_t len;
        uint32_t cp = utf8_next(s + i, n - i, &len);
        const bool raw_byte = len == 1 && cp >= 0x80;   /* malformed: keep the byte */
        i += len;
        if (cp == '\r') {
            if (i < n && s[i] == '\n') {
                i++;
            }
            cp = '\n';
        } else if ((cp >= 0xFF10 && cp <= 0xFF19) || (cp >= 0xFF21 && cp <= 0xFF3A)
                   || (cp >= 0xFF41 && cp <= 0xFF5A)) {
            cp -= 0xFEE0;
        } else if (cp == 0x3000) {
            cp = ' ';
        }
        if (is_dropped_ctrl(cp)) {
            continue;
        }
        if (cp >= 'A' && cp <= 'Z') {
            cp += 'a' - 'A';
        }
        if (o + 4 >= out_len) {
            return -1;
        }
        if (raw_byte) {
            out[o++] = (char)cp;
        } else {
            o += utf8_put(out + o, cp);
        }
    }
    out[o] = '\0';
    return (int)o;
}

/* ------------------------------------------------------------------ BPE */

static void set_pair_rank(const llm_tok_t *t, int i)
{
    const llm_sym_t *a = &t->sym[i], *b = &t->sym[i + 1];
    const uint16_t id = lookup(t, t->buf + a->start, (uint32_t)a->len + b->len);
    t->pair[i] = id == LLM_NO_PIECE ? LLM_NO_RANK : rank_of(t, id);
}

int llm_bpe(const llm_tok_t *t, const char *text, uint16_t *ids, int max_ids)
{
    char *buf = t->buf;
    llm_sym_t *sym = t->sym;
    uint16_t *rank = t->pair;
    const uint8_t *s = (const uint8_t *)text;
    const size_t n = strlen(text);
    size_t blen = 0;
    int ns = 0;

    for (size_t i = 0; i < n;) {        /* characters, spaces escaped */
        size_t len;
        (void)utf8_next(s + i, n - i, &len);
        if (ns >= LLM_MAX_TEXT || blen + 4 > (size_t)LLM_MAX_TEXT * 3) {
            return IPRO_LLM_E_TOOLONG;
        }
        sym[ns].start = (uint16_t)blen;
        if (len == 1 && s[i] == ' ') {
            memcpy(buf + blen, SPACE_ESC, 3);
            blen += 3;
        } else {
            memcpy(buf + blen, s + i, len);
            blen += len;
        }
        sym[ns].len = (uint16_t)(blen - sym[ns].start);
        ns++;
        i += len;
    }

    for (int i = 0; i + 1 < ns; i++) {
        set_pair_rank(t, i);
    }
    for (;;) {
        int best = -1;
        for (int i = 0; i + 1 < ns; i++) {      /* strict <: leftmost wins a tie */
            if (rank[i] != LLM_NO_RANK && (best < 0 || rank[i] < rank[best])) {
                best = i;
            }
        }
        if (best < 0) {
            break;
        }
        sym[best].len = (uint16_t)(sym[best].len + sym[best + 1].len);
        memmove(&sym[best + 1], &sym[best + 2], (size_t)(ns - best - 2) * sizeof *sym);
        memmove(&rank[best], &rank[best + 1], (size_t)(ns - best - 2) * sizeof *rank);
        ns--;
        for (int i = best - 1; i <= best; i++) {     /* the two pairs that changed */
            if (i >= 0 && i + 1 < ns) {
                set_pair_rank(t, i);
            }
        }
    }

    int out = 0;
    for (int i = 0; i < ns; i++) {
        const uint16_t id = lookup(t, buf + sym[i].start, sym[i].len);
        if (id != LLM_NO_PIECE) {
            if (out >= max_ids) {
                return IPRO_LLM_E_TOOLONG;
            }
            ids[out++] = id;
            continue;
        }
        for (uint16_t b = 0; b < sym[i].len; b++) {  /* byte fallback */
            if (out >= max_ids) {
                return IPRO_LLM_E_TOOLONG;
            }
            ids[out++] = (uint16_t)(t->byte_base + (uint8_t)buf[sym[i].start + b]);
        }
    }
    return out;
}

int llm_encode_prompt(const ipro_llm_t *m, const char *text, uint16_t *ids, int max_ids)
{
    char norm[LLM_MAX_TEXT + 8];
    const size_t ul = strlen(m->g.user_literal);
    memcpy(norm, m->g.user_literal, ul);
    if (llm_normalize_query(norm + ul, sizeof norm - ul, text) < 0) {
        return IPRO_LLM_E_TOOLONG;
    }
    if (max_ids < m->g.n_prefix + m->g.n_suffix) {
        return IPRO_LLM_E_TOOLONG;
    }
    int n = 0;
    for (int i = 0; i < m->g.n_prefix; i++) {
        ids[n++] = m->g.prefix[i];
    }
    const int k = llm_bpe(&m->tok, norm, ids + n, max_ids - n - m->g.n_suffix);
    if (k < 0) {
        return k;
    }
    n += k;
    for (int i = 0; i < m->g.n_suffix; i++) {
        ids[n++] = m->g.suffix[i];
    }
    return n;
}
