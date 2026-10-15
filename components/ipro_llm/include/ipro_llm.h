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
 * @file ipro_llm.h
 * @brief On-device tool-calling model: text request in, tool calls out.
 *
 * Runs the IPRO SAN models trained in tools/ipro_llm (XS: d192, 6 layers,
 * CQ2/CQ4 weights) from one blob written by tools/ipro_llm/export/export_blob.py.
 * The tools live in the weights; decoding is constrained to the call grammar,
 * so every result is a valid call list (empty = the request is refused).
 *
 * Portable C11, no allocation: the caller hands over one work buffer
 * (PSRAM on IPRO7) of ipro_llm_work_bytes(). Not reentrant; one context per
 * task. Uses float (the IPRO7 FreeRTOS port saves the FPU registers on task
 * switches and interrupts since 2026-10-09, apps/test/ipro_fpu_ctx_test).
 */
#ifndef IPRO_LLM_H
#define IPRO_LLM_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define IPRO_LLM_MAX_CALLS   4
#define IPRO_LLM_MAX_ARGS    4
#define IPRO_LLM_MAX_NEW     40     /* generated tokens, as the Python decoder */
#define IPRO_LLM_MAX_PROMPT  128    /* prompt tokens, special tokens included */

typedef enum {
    IPRO_LLM_OK         = 0,
    IPRO_LLM_E_BLOB     = -1,   /* not an IPLM blob, unknown version or layout */
    IPRO_LLM_E_CRC      = -2,   /* blob corrupted */
    IPRO_LLM_E_NOMEM    = -3,   /* work buffer smaller than ipro_llm_work_bytes() */
    IPRO_LLM_E_TOOLONG  = -4,   /* request longer than the context */
    IPRO_LLM_E_ARG      = -5,   /* NULL or invalid argument */
    IPRO_LLM_E_GRAMMAR  = -6,   /* decoder left the grammar (never expected) */
    IPRO_LLM_E_ACCEL    = -7,   /* no NPU scales in the blob, or the accelerator refused */
} ipro_llm_err_t;

typedef struct {
    uint8_t arg;            /* argument index within the tool (schema order) */
    uint8_t out_of_range;   /* integer outside the schema's [min, max] */
    int16_t value;          /* int: the number; enum: value index; bool: 0 / 1 */
} ipro_llm_arg_t;

typedef struct {
    uint8_t        tool;    /* tool index (schema order) */
    uint8_t        n_args;
    ipro_llm_arg_t args[IPRO_LLM_MAX_ARGS];
} ipro_llm_call_t;

typedef struct {
    uint8_t         n_calls;    /* 0 = refused */
    ipro_llm_call_t calls[IPRO_LLM_MAX_CALLS];
    uint16_t        n_prompt;   /* prompt tokens */
    uint16_t        n_gen;      /* generated tokens, <|im_end|> included */
    uint16_t        gen[IPRO_LLM_MAX_NEW];
} ipro_llm_result_t;

typedef struct ipro_llm ipro_llm_t;

/** CQ weight kernels. INT8 (default) quantises each Hadamard-rotated group of
 *  activations to int8 and multiplies by int8 codebook values (SMAQA on IPRO7,
 *  the same integer arithmetic in C elsewhere). FLOAT is the reference that
 *  matches the PyTorch model to ~1e-5. */
typedef enum {
    IPRO_LLM_KERNEL_INT8  = 0,
    IPRO_LLM_KERNEL_FLOAT = 1,
} ipro_llm_kernel_t;

/** Where the last ipro_llm_run() spent its time, in ipro_llm_clock() units. */
typedef struct {
    uint32_t prefill, decode;           /* the two phases, everything included */
    uint32_t embed, engram, attn_proj, attn_core, mlp, head;
    uint32_t accel;                     /* inside accelerator runs (part of attn_proj) */
    uint16_t prefill_tokens, decode_tokens;
    uint16_t accel_runs, accel_fails;   /* projections the accelerator ran / gave back to the CPU */
} ipro_llm_prof_t;

/**
 * Accelerator for the attention projections (IPRO7: the NPU, port/ipro7). The engine hands it
 * each projection once as int8 weights and then, per prompt chunk or decode step, int8
 * activations; it returns int8 outputs. The arithmetic is TFLite's int8 convolution with zero
 * points 0: acc = sum x * w (int32), y = sat8(MultiplyByQuantizedMultiplier(acc, mult, shift)).
 * Needs a blob exported with --npu-calib (the NPUS section: static scales per projection).
 */
typedef struct {
    /** Take projection `slot` (2 * layer: q, k, v and gate as one, 2 * layer + 1: out): int8
     *  weights [cout][cin] row-major, per output channel the Q31 multiplier and shift, and the
     *  output scale (the NPU's instruction word wants it). 0 = taken. */
    int (*load)(void *ctx, int slot, int cin, int cout, const int8_t *w, const int32_t *mult,
                const int8_t *shift, float s_out);
    /** y[t * cout + c] for t < n (min_tokens <= n <= max_tokens) from x[t * cin + i]. 0 = done. */
    int (*run)(void *ctx, int slot, const int8_t *x, int n, int8_t *y);
    void *ctx;
    int   max_tokens;
    int   min_tokens;   /* 1: decode steps too; 2: prompt chunks only */
} ipro_llm_accel_t;

/** What the attached accelerator runs (ipro_llm_accel_enable); attaching turns on what it takes. */
enum {
    IPRO_LLM_ACCEL_OFF     = 0,
    IPRO_LLM_ACCEL_PREFILL = 1,     /* prompt chunks of 2 or more tokens */
    IPRO_LLM_ACCEL_DECODE  = 2,     /* one-token decode steps (min_tokens == 1) */
    IPRO_LLM_ACCEL_ALL     = 3,
};

/** Work bytes the blob needs, or 0 if it is not a valid blob header. */
size_t ipro_llm_work_bytes(const void *blob, size_t blob_len);

/**
 * Check the blob (header, CRC, tensors) and set up a context inside `work`.
 * The blob must stay readable for the life of the context; keep it in RAM
 * (PSRAM) on IPRO7, XIP flash is about 4x slower to read.
 */
int ipro_llm_open(ipro_llm_t **out, const void *blob, size_t blob_len,
                  void *work, size_t work_len);

/** Run one request (UTF-8 text as the phone's ASR produced it). */
int ipro_llm_run(ipro_llm_t *m, const char *text, ipro_llm_result_t *res);

void ipro_llm_set_kernel(ipro_llm_t *m, ipro_llm_kernel_t kernel);

const ipro_llm_prof_t *ipro_llm_profile(const ipro_llm_t *m);

/** Scratch bytes ipro_llm_accel_attach() needs (one projection's int8 weights and
 *  multipliers), or 0 if the blob has no NPU scales. */
size_t ipro_llm_accel_scratch_bytes(const ipro_llm_t *m);

/**
 * Build every attention projection's int8 weights from the CQ weights (one scale per output
 * channel) in `scratch`, one projection at a time, and hand them to a->load. From then on prompt
 * chunks of two or more tokens, and decode steps if a->min_tokens is 1, run those projections
 * through a->run; a run that fails falls back to the CPU kernels. `a` must outlive the context;
 * NULL detaches. The scratch is free again when this returns.
 */
int ipro_llm_accel_attach(ipro_llm_t *m, const ipro_llm_accel_t *a, void *scratch, size_t len);

/** IPRO_LLM_ACCEL_* bits: what the attached accelerator runs (A/B runs). */
void ipro_llm_accel_enable(ipro_llm_t *m, int what);

/** The same integer arithmetic in portable C, as an accelerator: what the host tests measure
 *  the NPU's accuracy with. It keeps every projection's weights in `mem`. */
size_t ipro_llm_accel_ref_bytes(const ipro_llm_t *m);
int    ipro_llm_accel_ref(ipro_llm_accel_t *a, void *mem, size_t len, const ipro_llm_t *m);

/** TFLite MultiplyByQuantizedMultiplier, saturated to int8. */
int8_t ipro_llm_requant(int32_t acc, int32_t mult, int shift);

/** Clock for the profile, e.g. the cycle counter. Weak; the default returns
 *  0, which leaves the profile at zero. */
uint32_t ipro_llm_clock(void);

/** Calls as JSON, e.g. [{"name":"set_light","arguments":{"color":"blue"}}]. */
int ipro_llm_result_json(const ipro_llm_t *m, const ipro_llm_result_t *res,
                         char *buf, size_t len);

int         ipro_llm_n_tools(const ipro_llm_t *m);
const char *ipro_llm_tool_name(const ipro_llm_t *m, int tool);
const char *ipro_llm_arg_name(const ipro_llm_t *m, int tool, int arg);
/** Enum value name or "true" / "false"; NULL for integer arguments. */
const char *ipro_llm_value_name(const ipro_llm_t *m, int tool, int arg, int value);

#ifdef __cplusplus
}
#endif

#endif /* IPRO_LLM_H */
