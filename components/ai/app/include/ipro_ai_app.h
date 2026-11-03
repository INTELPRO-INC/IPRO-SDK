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
 * @file ipro_ai_app.h
 * @brief Application-level inference: source -> preprocessing -> model ->
 *        task adapter -> callbacks and events.
 *
 *   ipro_ai_app_create(&cfg, &app);
 *   ipro_ai_app_set_model(app, model, &adapter);    // ipro_ai_model_open'ed
 *   ipro_ai_app_set_source(app, src, NULL);          // ipro_ai_source.h
 *   ipro_ai_app_set_preproc(app, my_pre, ctx);       // window -> input tensor
 *   ipro_ai_app_register_callback(app, on_result, ctx);
 *   ipro_ai_app_start(app);                          // a task pumps the source
 *   ...
 *   ipro_ai_app_stop(app);  ipro_ai_app_destroy(app);
 *
 * An app that already owns its acquisition (it streams the samples to a
 * host too, or runs a VAD first) skips the source and calls
 * ipro_ai_app_process() with each window instead; the rest is the same.
 *
 * Results are split in two: the generic part (raw output, timings, the
 * window's time) and the task adapter's part. Adapters exist for
 * classification, embeddings and anomaly scores; another task (regression,
 * detection) is another ipro_ai_adapter_t with its own post function, the
 * pipeline does not change.
 *
 * Threads: create/set/start/stop/destroy from one task. Callbacks run on the
 * app's task (or the caller of ipro_ai_app_process); events go out through
 * ipro_ai_event.h to its dispatcher.
 */
#ifndef IPRO_AI_APP_H
#define IPRO_AI_APP_H

#include <stdbool.h>
#include <stdint.h>

#include "ipro_ai.h"
#include "ipro_ai_event.h"
#include "ipro_ai_source.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IPRO_AI_MAX_CLASSES   16u
#define IPRO_AI_LABEL_MAX     16u          /* bytes, with the NUL */

typedef enum {
    IPRO_AI_TASK_CLASSIFY = 0,
    IPRO_AI_TASK_EMBED,
    IPRO_AI_TASK_ANOMALY,
} ipro_ai_task_t;

/* ---------------------------------------------------------------- results */

typedef struct {
    int32_t  index;                        /* the class reported; -1 = UNKNOWN (best under its floor) */
    int32_t  best;                         /* the top class, floor or not */
    uint32_t n;
    float    logit[IPRO_AI_MAX_CLASSES];   /* averaged over the output positions */
    float    exp[IPRO_AI_MAX_CLASSES];     /* expf(logit - top): probability = exp / exp_sum */
    float    exp_sum;
    const char *label;                     /* labels[index], or "UNKNOWN" */
    bool     changed;                      /* index differs from the previous result */
} ipro_ai_class_result_t;

typedef struct {
    const float *vec;                      /* dim values, owned by the adapter until the next run */
    uint32_t dim;
    int32_t  match;                        /* gallery entry with the best cosine, -1 when none passes */
    float    score;                        /* that cosine */
    const char *name;
} ipro_ai_embed_result_t;

typedef struct {
    float    score;                        /* higher = more anomalous */
    float    threshold;
    bool     over;                         /* this window */
    bool     alarm;                        /* after hold / release */
    bool     changed;                      /* alarm started or ended with this window */
} ipro_ai_anomaly_result_t;

typedef struct {
    /* generic */
    const uint8_t *raw;                    /* the model's output bytes (ipro_ai_tensor_info_t) */
    uint32_t raw_bytes;
    uint64_t t_us;                         /* capture time of the window's first frame */
    uint32_t pre_us, npu_us, post_us;
    uint32_t seq;                          /* results produced by this app, from 1 */
    /* the adapter's */
    ipro_ai_task_t task;
    union {
        ipro_ai_class_result_t   cls;
        ipro_ai_embed_result_t   emb;
        ipro_ai_anomaly_result_t anom;
    };
} ipro_ai_result_t;

/* ---------------------------------------------------------------- adapters */

/**
 * A task adapter: turns the raw output into the task's result. post() gets
 * the output tensor's description (ipro_ai_tensor_get_info) and fills the
 * union member of its task; events() (optional) publishes what the result
 * means as events.
 */
typedef struct ipro_ai_adapter {
    ipro_ai_task_t task;
    int  (*post)(struct ipro_ai_adapter *a, const uint8_t *raw, const ipro_ai_tensor_info_t *out,
                 ipro_ai_result_t *res);
    void (*events)(struct ipro_ai_adapter *a, const ipro_ai_result_t *res, ipro_ai_event_t *proto);
    void *cfg;                             /* the adapter's settings and state */
} ipro_ai_adapter_t;

/* Classification: average every class over the output positions in the
 * integer domain, (sum(byte - zp) / positions) * scale; argmax; softmax by
 * expf(logit - top). With floors, a top class scoring under its floor is
 * UNKNOWN. Events: CLASS on every result when every_result, CHANGED (or
 * UNKNOWN) when the reported class changes. */
typedef struct {
    uint32_t n;                            /* classes, <= IPRO_AI_MAX_CLASSES and <= the output channels */
    const char (*labels)[IPRO_AI_LABEL_MAX];
    const float *floor;                    /* n floors, or NULL */
    bool     every_result;
    int32_t  last;                         /* state: the previous index (start at -2) */
} ipro_ai_classify_cfg_t;

/* Embedding: head() turns the raw output into dim values (the speaker
 * model's pooling and projection stay in the app); normalize scales them to
 * unit length; then the best cosine against gallery[gallery_n][dim] above
 * threshold is the match. Event: SPEAKER per result when publish is set. */
typedef int (*ipro_ai_embed_head_fn)(void *ctx, const uint8_t *raw, const ipro_ai_tensor_info_t *out,
                                     float *vec, uint32_t dim);
typedef struct {
    uint32_t dim;
    ipro_ai_embed_head_fn head;
    void    *head_ctx;
    bool     normalize;
    const float *gallery;
    const char *const *names;
    uint32_t gallery_n;
    float    threshold;
    bool     publish;
    float   *vec;                          /* dim floats the adapter writes (caller's storage) */
} ipro_ai_embed_cfg_t;

/* Anomaly: score = logit[anomaly] - logit[normal] of a two-class head
 * (LOGIT_MARGIN; Sensor Teach's anomaly mode trains exactly that), or
 * 1 - cosine(embedding, reference) (DISTANCE, with an embedding head).
 * The alarm starts after `hold` windows over the threshold in a row and ends
 * after `release` under it. Event: ANOMALY when the alarm starts or ends. */
typedef enum { IPRO_AI_ANOM_LOGIT_MARGIN = 0, IPRO_AI_ANOM_DISTANCE } ipro_ai_anom_mode_t;
typedef struct {
    ipro_ai_anom_mode_t mode;
    uint32_t normal_index, anomaly_index;  /* LOGIT_MARGIN */
    ipro_ai_embed_cfg_t *embed;            /* DISTANCE: its head and storage */
    const float *reference;                /* DISTANCE: embed->dim values, unit length */
    float    threshold;
    uint32_t hold, release;                /* windows; 0 counts as 1 */
    /* state */
    uint32_t run_over, run_under;
    bool     alarm;
} ipro_ai_anomaly_cfg_t;

void ipro_ai_adapter_classify(ipro_ai_adapter_t *a, ipro_ai_classify_cfg_t *cfg);
void ipro_ai_adapter_embed(ipro_ai_adapter_t *a, ipro_ai_embed_cfg_t *cfg);
void ipro_ai_adapter_anomaly(ipro_ai_adapter_t *a, ipro_ai_anomaly_cfg_t *cfg);

/* ---------------------------------------------------------------- app */

typedef struct ipro_ai_app ipro_ai_app_t;

/** Window of `frames` samples (channels interleaved, oldest first) -> the
 *  model's input buffer. Return IPRO_AI_OK or an error (the window is skipped). */
typedef int (*ipro_ai_pre_fn)(void *ctx, const int16_t *window, uint32_t frames, uint32_t channels,
                              uint8_t *input, uint32_t input_bytes);
typedef void (*ipro_ai_result_cb)(const ipro_ai_result_t *res, void *ctx);

typedef struct {
    const char *name;                      /* events' model name (15 chars kept) */
    uint8_t  model_id;
    uint32_t window;                       /* frames per inference (managed mode) */
    uint32_t hop;                          /* frames between inferences; 0: window */
    uint32_t stack_words;                  /* managed task; 0: 1024 */
    uint32_t priority;                     /* managed task; 0: the port's default */
    bool     publish;                      /* send the adapter's events */
} ipro_ai_app_cfg_t;

typedef struct {
    uint32_t runs, errors, skipped;        /* skipped: windows the preprocessing refused */
    uint32_t pre_us_avg, npu_us_avg, post_us_avg;
    uint32_t pre_us_max, npu_us_max, post_us_max;
    uint64_t frames;                       /* managed: read from the source */
    uint32_t source_lost, source_stalls;
    bool     running;
} ipro_ai_app_metrics_t;

int  ipro_ai_app_create(const ipro_ai_app_cfg_t *cfg, ipro_ai_app_t **out);
/** The model stays the caller's: closing it is the caller's job, after destroy. */
int  ipro_ai_app_set_model(ipro_ai_app_t *app, ipro_ai_model_t *model, ipro_ai_adapter_t *adapter);
int  ipro_ai_app_set_source(ipro_ai_app_t *app, ipro_ai_source_t *src, const ipro_ai_source_cfg_t *cfg);
int  ipro_ai_app_set_preproc(ipro_ai_app_t *app, ipro_ai_pre_fn fn, void *ctx);
int  ipro_ai_app_register_callback(ipro_ai_app_t *app, ipro_ai_result_cb cb, void *ctx);
/** Managed mode: open and start the source, then a task reads it and runs
 *  every hop frames. IPRO_AI_E_STATE without a model, source or preproc. */
int  ipro_ai_app_start(ipro_ai_app_t *app);
int  ipro_ai_app_stop(ipro_ai_app_t *app);
void ipro_ai_app_destroy(ipro_ai_app_t *app);
/**
 * One inference on a window the caller holds (frames x channels int16), or
 * with window NULL on an input already in the model's input buffer. t_us is
 * the window's capture time. res (optional) receives the result.
 */
int  ipro_ai_app_process(ipro_ai_app_t *app, const int16_t *window, uint32_t frames, uint32_t channels,
                         uint64_t t_us, ipro_ai_result_t *res);
int  ipro_ai_app_get_metrics(const ipro_ai_app_t *app, ipro_ai_app_metrics_t *m);

#ifdef __cplusplus
}
#endif

#endif /* IPRO_AI_APP_H */
