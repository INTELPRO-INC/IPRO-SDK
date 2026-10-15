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
 * @file llm_grammar.c
 * @brief The call grammar: allowed next tokens, state steps, call extraction.
 *
 * A line-for-line port of GrammarMask in tools/ipro_llm/train/decode.py (the
 * mask the model was evaluated with) and of CallGrammar.decode(validate=False):
 *
 *   output := <tool_call> call* </tool_call> <|im_end|>
 *   call   := TOOL arg*            arguments in schema order
 *   arg    := INT_KEY digit+ | ENUM_VALUE | BOOL_VALUE
 *
 * An unset required argument cannot be skipped, a call (or the list) can only
 * end once its required arguments are written, integers have no leading zero
 * and are limited to 0-999: the model copies the number the user said and the
 * caller reads out_of_range against the schema bounds.
 */
#include <string.h>

#include "llm_internal.h"

#define INT_LO 0
#define INT_HI 999

void llm_g_start(llm_gstate_t *s)
{
    memset(s, 0, sizeof *s);
    s->phase = G_START;
    s->tool = -1;
    s->key = -1;
}

static int push(uint16_t *out, int n, int max, uint16_t tok)
{
    if (n < max) {
        out[n] = tok;
    }
    return n + 1;
}

static bool required_ok(const llm_grammar_t *g, int tool, uint32_t set)
{
    const llm_tool_t *t = &g->tools[tool];
    for (int i = 0; i < t->n_args; i++) {
        if (g->args[t->first_arg + i].required && !(set & (1u << i))) {
            return false;
        }
    }
    return true;
}

static int push_tools_close(const llm_grammar_t *g, uint16_t *out, int n, int max)
{
    for (int i = 0; i < g->n_tools; i++) {
        n = push(out, n, max, g->tools[i].token);
    }
    return push(out, n, max, g->close);
}

/* decode.py _after_arg() */
static int after_arg(const llm_grammar_t *g, int tool, int pos, uint32_t set,
                     uint16_t *out, int n, int max)
{
    const llm_tool_t *t = &g->tools[tool];
    for (int i = pos; i < t->n_args; i++) {
        const llm_arg_t *a = &g->args[t->first_arg + i];
        if (a->kind == LLM_ARG_INT) {
            n = push(out, n, max, a->key_token);
        } else {
            for (int v = 0; v < a->n_values; v++) {
                n = push(out, n, max, g->values[a->first_value + v].token);
            }
        }
        if (a->required && !(set & (1u << i))) {
            break;
        }
    }
    if (required_ok(g, tool, set)) {
        n = push_tools_close(g, out, n, max);
    }
    return n;
}

int llm_g_allowed(const llm_grammar_t *g, const llm_gstate_t *s, uint16_t *out, int max)
{
    int n = 0;
    switch (s->phase) {
    case G_START:
        return push(out, 0, max, g->open);
    case G_BETWEEN:
        return push_tools_close(g, out, 0, max);
    case G_CALL:
        return after_arg(g, s->tool, s->pos, s->args_set, out, 0, max);
    case G_INT:
        if (!s->lead_zero) {
            for (int d = 0; d < 10; d++) {
                if (s->n_digits == 0 && d == 0 && INT_LO > 0) {
                    continue;
                }
                if (s->digits * 10 + d <= INT_HI) {
                    n = push(out, n, max, g->digit[d]);
                }
            }
        }
        if (s->n_digits > 0 && s->digits >= INT_LO) {
            n = after_arg(g, s->tool, s->pos, s->args_set | (1u << s->key), out, n, max);
        }
        return n;
    case G_END:
        return push(out, 0, max, g->im_end);
    case G_DONE:
    default:
        return 0;
    }
}

static int digit_of(const llm_grammar_t *g, uint16_t tok)
{
    for (int d = 0; d < 10; d++) {
        if (g->digit[d] == tok) {
            return d;
        }
    }
    return -1;
}

static int tool_of(const llm_grammar_t *g, uint16_t tok)
{
    for (int i = 0; i < g->n_tools; i++) {
        if (g->tools[i].token == tok) {
            return i;
        }
    }
    return -1;
}

/* decode.py GrammarMask.step(); returns IPRO_LLM_E_GRAMMAR for a token the
 * state does not allow (the decoder only ever picks allowed ones). */
int llm_g_step(const llm_grammar_t *g, llm_gstate_t *s, uint16_t tok)
{
    if (s->phase == G_START) {
        s->phase = G_BETWEEN;
        return IPRO_LLM_OK;
    }
    if (tok == g->close) {
        llm_g_start(s);
        s->phase = G_END;
        return IPRO_LLM_OK;
    }
    if (tok == g->im_end) {
        llm_g_start(s);
        s->phase = G_DONE;
        return IPRO_LLM_OK;
    }
    const int d = digit_of(g, tok);
    if (s->phase == G_INT && d >= 0) {
        s->digits = s->digits * 10 + d;
        s->lead_zero = s->n_digits == 0 && d == 0;
        s->n_digits++;
        return IPRO_LLM_OK;
    }
    if (s->phase == G_INT) {        /* the integer ended; tok starts what follows */
        s->phase = G_CALL;
        s->args_set |= 1u << s->key;
        s->key = -1;
        s->n_digits = 0;
        s->digits = 0;
        s->lead_zero = false;
    }
    const int t = tool_of(g, tok);
    if (t >= 0) {
        llm_g_start(s);
        s->phase = G_CALL;
        s->tool = (int8_t)t;
        return IPRO_LLM_OK;
    }
    if (s->tool < 0) {
        return IPRO_LLM_E_GRAMMAR;
    }
    const llm_tool_t *tl = &g->tools[s->tool];
    for (int i = s->pos; i < tl->n_args; i++) {
        const llm_arg_t *a = &g->args[tl->first_arg + i];
        if (a->kind == LLM_ARG_INT) {
            if (a->key_token == tok) {
                s->phase = G_INT;
                s->pos = (uint8_t)(i + 1);
                s->key = (int8_t)i;
                s->n_digits = 0;
                s->digits = 0;
                s->lead_zero = false;
                return IPRO_LLM_OK;
            }
            continue;
        }
        for (int v = 0; v < a->n_values; v++) {
            if (g->values[a->first_value + v].token == tok) {
                s->phase = G_CALL;
                s->pos = (uint8_t)(i + 1);
                s->args_set |= 1u << i;
                return IPRO_LLM_OK;
            }
        }
    }
    return IPRO_LLM_E_GRAMMAR;
}

/* CallGrammar.decode(validate=False) over the generated tokens: everything
 * from <tool_call> to </tool_call>; an unterminated output is an error. */
int llm_g_calls(const llm_grammar_t *g, const uint16_t *gen, int n, ipro_llm_result_t *res)
{
    res->n_calls = 0;
    if (n < 2 || gen[0] != g->open) {
        return IPRO_LLM_E_GRAMMAR;
    }
    ipro_llm_call_t *cur = NULL;
    ipro_llm_arg_t *num = NULL;         /* integer being read */
    for (int i = 1; i < n; i++) {
        const uint16_t tok = gen[i];
        const int d = digit_of(g, tok);
        if (num && d >= 0) {
            num->value = (int16_t)(num->value * 10 + d);
            continue;
        }
        if (num) {
            const llm_arg_t *a = &g->args[g->tools[cur->tool].first_arg + num->arg];
            num->out_of_range = num->value < a->min || num->value > a->max;
            num = NULL;
        }
        if (tok == g->close) {
            return IPRO_LLM_OK;     /* the grammar never closes on a bare key */
        }
        const int t = tool_of(g, tok);
        if (t >= 0) {
            if (res->n_calls >= IPRO_LLM_MAX_CALLS) {
                return IPRO_LLM_E_GRAMMAR;
            }
            cur = &res->calls[res->n_calls++];
            memset(cur, 0, sizeof *cur);
            cur->tool = (uint8_t)t;
            continue;
        }
        if (!cur || cur->n_args >= IPRO_LLM_MAX_ARGS) {
            return IPRO_LLM_E_GRAMMAR;
        }
        const llm_tool_t *tl = &g->tools[cur->tool];
        bool found = false;
        for (int a = 0; a < tl->n_args && !found; a++) {
            const llm_arg_t *ar = &g->args[tl->first_arg + a];
            if (ar->kind == LLM_ARG_INT && ar->key_token == tok) {
                num = &cur->args[cur->n_args++];
                num->arg = (uint8_t)a;
                num->value = 0;
                num->out_of_range = 0;
                found = true;
            }
            for (int v = 0; ar->kind != LLM_ARG_INT && v < ar->n_values; v++) {
                if (g->values[ar->first_value + v].token == tok) {
                    ipro_llm_arg_t *x = &cur->args[cur->n_args++];
                    x->arg = (uint8_t)a;
                    x->value = g->values[ar->first_value + v].value;
                    x->out_of_range = 0;
                    found = true;
                    break;
                }
            }
        }
        if (!found) {
            return IPRO_LLM_E_GRAMMAR;
        }
    }
    return IPRO_LLM_E_GRAMMAR;      /* no </tool_call> */
}
